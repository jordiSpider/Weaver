#include "Misc/JsonValidator/ConfigurationValidator.h"
#include "Misc/JsonValidator/Validator.h"

#include <fstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <regex>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <cstdlib>
#include <iterator>

using json = nlohmann::json;
namespace fs = std::filesystem;

json ValidationDiagnostic::toJson() const {
    return {{"code",code},{"message",message},{"file",file},{"instancePath",instancePath},
            {"schemaPath",schemaPath},{"severity",severity}};
}
json ValidationResult::toJson() const {
    json e=json::array(), w=json::array();
    for (const auto& d: errors) e.push_back(d.toJson());
    for (const auto& d: warnings) w.push_back(d.toJson());
    return {{"valid",valid()},{"errors",e},{"warnings",w}};
}
std::string ValidationResult::toPrettyString(int indent) const {
    return toJson().dump(indent);
}

namespace {
void issue(ValidationResult& r, std::string code, std::string message, std::string file,
           std::string path="", std::string schemaPath="") {
    r.errors.push_back({std::move(code),std::move(message),std::move(file),std::move(path),std::move(schemaPath),"error"});
}
void warning(ValidationResult& r, std::string code, std::string message, std::string file,
             std::string path = "", std::string schemaPath = "") {
    r.warnings.push_back({std::move(code), std::move(message), std::move(file), std::move(path), std::move(schemaPath), "warning"});
}
std::pair<std::string,std::string> configExceptionDetails(const std::string& what) {
    std::smatch match;
    static const std::regex context(R"(\) 'root((?:\.[^']*)*)': (.*)$)");
    if (!std::regex_search(what, match, context)) return {"", what};
    std::string pointer;
    std::string dotted=match[1].str();
    std::stringstream parts(dotted); std::string part;
    while (std::getline(parts,part,'.')) {
        if (part.empty()) continue;
        if (part.rfind("item_", 0) == 0 && part.size() > 5 &&
            std::all_of(part.begin() + 5, part.end(), [](unsigned char c) { return std::isdigit(c); })) {
            part.erase(0, 5);
        }
        std::size_t pos=0;
        while ((pos=part.find('~',pos))!=std::string::npos) {part.replace(pos,1,"~0");pos+=2;}
        pos=0;
        while ((pos=part.find('/',pos))!=std::string::npos) {part.replace(pos,1,"~1");pos+=2;}
        pointer += "/" + part;
    }
    return {pointer,match[2].str()};
}
std::vector<std::string> pointerTokens(const std::string& pointer);
std::string escapePointerToken(std::string token) {
    std::size_t pos = 0;
    while ((pos = token.find('~', pos)) != std::string::npos) { token.replace(pos, 1, "~0"); pos += 2; }
    pos = 0;
    while ((pos = token.find('/', pos)) != std::string::npos) { token.replace(pos, 1, "~1"); pos += 2; }
    return token;
}
std::string schemaPathForInstance(const json& schema, const std::string& instancePath, const std::string& message) {
    const json* node = &schema;
    std::string path;
    for (const auto& token : pointerTokens(instancePath)) {
        if (node->contains("items") && node->at("items").is_object()) {
            path += "/items";
            node = &node->at("items");
            continue;
        }
        if (node->contains("properties") && node->at("properties").is_object() && node->at("properties").contains(token)) {
            path += "/properties/" + escapePointerToken(token);
            node = &node->at("properties").at(token);
            continue;
        }
        bool matched = false;
        if (node->contains("patternProperties") && node->at("patternProperties").is_object()) {
            for (const auto& [name, candidate] : node->at("patternProperties").items()) {
                bool keyMatches = candidate.contains("enum") && candidate.at("enum").is_array() &&
                    std::find(candidate.at("enum").begin(), candidate.at("enum").end(), token) != candidate.at("enum").end();
                if (!keyMatches && candidate.contains("pattern") && candidate.at("pattern").is_string()) {
                    try { keyMatches = std::regex_match(token, std::regex(candidate.at("pattern").get<std::string>())); } catch (...) {}
                }
                if (keyMatches) {
                    path += "/patternProperties/" + escapePointerToken(name);
                    node = &candidate;
                    matched = true;
                    break;
                }
            }
        }
        if (!matched) break;
    }
    std::string keyword;
    if (message.find("Cannot be null") != std::string::npos) keyword = "nullProperty";
    else if (message.find("Not an element of type") != std::string::npos) keyword = "type";
    else if (message.find("String length less than minimum") != std::string::npos) keyword = "minLength";
    else if (message.find("String length greater than maximum") != std::string::npos) keyword = "maxLength";
    else if (message.find("not a multiple of") != std::string::npos) keyword = "multipleOf";
    else if (message.find("Items not arranged") != std::string::npos) keyword = "itemsOrder";
    else if (message.find("minimum") != std::string::npos) keyword = node->value("exclusiveMinimum", false) ? "exclusiveMinimum" : "minimum";
    else if (message.find("maximum") != std::string::npos) keyword = node->value("exclusiveMaximum", false) ? "exclusiveMaximum" : "maximum";
    else if (message.find("Number of items less") != std::string::npos) keyword = "minItems";
    else if (message.find("Number of items greater") != std::string::npos) keyword = "maxItems";
    else if (message.find("not among the allowed values") != std::string::npos) keyword = "enum";
    else if (message.find("does not meet defined pattern") != std::string::npos) keyword = "pattern";
    else if (message.find("Required item") != std::string::npos) keyword = "required";
    else if (message.find("not defined in schema") != std::string::npos) keyword = "additionalProperties";
    if (!keyword.empty() && node->contains(keyword)) path += "/" + keyword;
    return "#" + path;
}
json parseJson(std::istream& in) {
    std::stack<std::set<std::string>> keys;
    json::parser_callback_t uniqueKeys = [&keys](int, json::parse_event_t event, json& value) {
        if (event == json::parse_event_t::object_start) keys.emplace();
        else if (event == json::parse_event_t::object_end) { if (!keys.empty()) keys.pop(); }
        else if (event == json::parse_event_t::key) {
            const auto key=value.get<std::string>();
            if (keys.empty() || !keys.top().insert(key).second) throw std::runtime_error("Duplicate JSON object key: " + key);
        }
        return true;
    };
    return json::parse(in, uniqueKeys, true, true);
}
json readJson(const fs::path& p) { std::ifstream in(p); if (!in) throw std::runtime_error("Unable to open JSON file: " + p.string()); return parseJson(in); }
json parseJson(std::string_view content) { std::istringstream in{std::string(content)}; return parseJson(in); }
std::string globRegex(std::string p) {
    std::string out="^";
    for(char c:p) { if(c=='*') out += ".*"; else if(c=='?') out += "."; else { if(std::string(".^$|()[]{}+\\").find(c)!=std::string::npos) out+='\\'; out+=c; } }
    return out+"$";
}

std::vector<std::string> pointerTokens(const std::string& pointer) {
    std::vector<std::string> tokens;
    std::stringstream stream(pointer);
    std::string token;
    std::getline(stream, token, '/');
    while (std::getline(stream, token, '/')) {
        std::size_t pos = 0;
        while ((pos = token.find("~1", pos)) != std::string::npos) token.replace(pos, 2, "/");
        pos = 0;
        while ((pos = token.find("~0", pos)) != std::string::npos) token.replace(pos, 2, "~");
        tokens.push_back(std::move(token));
    }
    return tokens;
}

std::vector<json> selectPath(const json& root, const std::string& pointer, std::vector<std::string>* selectedPaths = nullptr) {
    const auto tokens = pointerTokens(pointer);
    std::vector<json> selected;
    auto children = [](const json& value, const auto& callback) {
        if (value.is_array()) for (std::size_t i = 0; i < value.size(); ++i) callback(std::to_string(i), value[i]);
        else if (value.is_object()) for (const auto& [key, child] : value.items()) callback(key, child);
    };
    auto walk = [&](const auto& self, const json& value, std::size_t index, const std::string& valuePath) -> void {
        if (index == tokens.size()) {
            selected.push_back(value);
            if (selectedPaths != nullptr) selectedPaths->push_back(valuePath);
            return;
        }
        const auto& token = tokens[index];
        if (token == "**") {
            self(self, value, index + 1, valuePath);
            children(value, [&](const std::string& key, const json& child) { self(self, child, index, valuePath + "/" + escapePointerToken(key)); });
        } else if (token == "*") {
            children(value, [&](const std::string& key, const json& child) { self(self, child, index + 1, valuePath + "/" + escapePointerToken(key)); });
        } else if (value.is_object() && value.contains(token)) {
            self(self, value[token], index + 1, valuePath + "/" + escapePointerToken(token));
        } else if (value.is_array()) {
            try { const auto i = std::stoul(token); self(self, value.at(i), index + 1, valuePath + "/" + std::to_string(i)); } catch (...) {}
        }
    };
    walk(walk, root, 0, "");
    return selected;
}

std::string firstReferencePath(const json& expression, const std::string& fileScope) {
    if (expression.is_object()) {
        if (expression.contains("ref") && expression.at("ref").is_object() &&
            expression.at("ref").value("file", "") == fileScope) {
            return expression.at("ref").value("path", "");
        }
        for (const auto& [key, value] : expression.items()) {
            auto path = firstReferencePath(value, fileScope);
            if (!path.empty()) return path;
        }
    } else if (expression.is_array()) {
        for (const auto& value : expression) {
            auto path = firstReferencePath(value, fileScope);
            if (!path.empty()) return path;
        }
    }
    return "";
}

// Constraint references support JSON Pointer indexes, '*' (one level), '**'
// (recursive descent), and the per-item scope used by forEachPath.
json eval(const json& x, const std::unordered_map<std::string,json>& docs, const std::string& currentFile = "", const json* currentItem = nullptr) {
    if (!x.is_object()) return x;
    if (x.contains("ref")) {
        const auto& q=x["ref"]; std::string f=q.value("file",""); std::string p=q.value("path",""); json selected=json::array();
        if (f == "$item") {
            if (currentItem == nullptr) return nullptr;
            const auto values = selectPath(*currentItem, p);
            if (values.empty()) return nullptr;
            if (values.size() == 1) return values.front();
            return values;
        }
        if (f == "$current") {
            auto found = docs.find(currentFile);
            if (found == docs.end()) return nullptr;
            const auto values = selectPath(found->second, p);
            if (values.empty()) return nullptr;
            if (values.size() == 1) return values.front();
            return values;
        }
        std::regex filePattern(globRegex(f));
        for(const auto& [name,doc]:docs) if(std::regex_match(name,filePattern)) {
            std::vector<json> current{doc}; std::stringstream ss(p); std::string part; std::getline(ss,part,'/');
            while(std::getline(ss,part,'/')) { if(part.empty()) continue; std::size_t pos=0; while((pos=part.find("~1"))!=std::string::npos) part.replace(pos,2,"/"); while((pos=part.find("~0"))!=std::string::npos) part.replace(pos,2,"~");
                std::vector<json> next;
                for(auto& v:current) {
                    if(part=="*"&&v.is_array()) for(const auto& e:v) next.push_back(e);
                    else if(part=="*"&&v.is_object()) for(const auto& [k,e]:v.items()) next.push_back(e);
                    else if(v.is_object()&&v.contains(part)) next.push_back(v[part]);
                    else if(v.is_array()) {try{next.push_back(v.at(std::stoul(part)));}catch(...) {}}
                }
                current=std::move(next);
            }
            for(auto& v:current) selected.push_back(std::move(v));
        }
        if(selected.empty()) return nullptr;
        if(selected.size()==1 && f.find_first_of("*?")==std::string::npos) return selected[0];
        return selected;
    }
    if (x.size()!=1) return x;
    auto op=x.begin().key(); const auto& args=x.begin().value();
    auto vals=[&](){std::vector<json> a; if(args.is_array()) for(const auto& z:args)a.push_back(eval(z,docs,currentFile,currentItem)); else a.push_back(eval(args,docs,currentFile,currentItem)); return a;};
    auto a=vals();
    auto truth=[](const json& v){return v.is_boolean()&&v.get<bool>();};
    if(op=="and"||op=="all"||op=="every") { for(auto&v:a) {if(v.is_array()){for(const auto& e:v)if(!truth(e))return false;}else if(!truth(v))return false;} return true; }
    if(op=="or"||op=="any"||op=="some") { for(auto&v:a) {if(v.is_array()){for(const auto& e:v)if(truth(e))return true;}else if(truth(v))return true;} return false; }
    if(op=="not") return a.empty()||!truth(a[0]);
    if(op=="implies") return a.size()<2||!truth(a[0])||truth(a[1]);
    if(op=="xor") { bool result=false; for(const auto& v:a) result=result!=truth(v); return result; }
    if(op=="if"&&a.size()>=3) return truth(a[0])?a[1]:a[2];
    if(op=="coalesce") { for(const auto& v:a) if(!v.is_null()) return v; return nullptr; }
    if(op=="eq"||op=="ne"||op=="lt"||op=="lte"||op=="gt"||op=="gte") {
        if(a.size()<2)return false;
        if(!a[0].is_array()&&!a[1].is_array()&&op=="eq")return a[0]==a[1];
        if(!a[0].is_array()&&!a[1].is_array()&&op=="ne")return a[0]!=a[1];
        if(a[0].is_array()||a[1].is_array()) {
            const size_t n=a[0].is_array()?a[0].size():a[1].size();
            if(a[0].is_array()&&a[1].is_array()&&a[0].size()!=a[1].size())return false;
            json out=json::array();
            for(size_t i=0;i<n;++i) {
                const json& l=a[0].is_array()?a[0][i]:a[0]; const json& r=a[1].is_array()?a[1][i]:a[1];
                if(l.is_number()&&r.is_number()){double u=l.get<double>(),v=r.get<double>();out.push_back(op=="lt"?u<v:op=="lte"?u<=v:op=="gt"?u>v:op=="gte"?u>=v:op=="eq"?u==v:u!=v);}
                else if(l.is_string()&&r.is_string()){const auto& u=l.get_ref<const std::string&>();const auto& v=r.get_ref<const std::string&>();out.push_back(op=="lt"?u<v:op=="lte"?u<=v:op=="gt"?u>v:op=="gte"?u>=v:op=="eq"?u==v:u!=v);}
                else out.push_back(op=="eq"?l==r:op=="ne"?l!=r:false);
            }
            return out;
        }
        if(a[0].is_string()&&a[1].is_string()) {const auto& l=a[0].get_ref<const std::string&>();const auto& r=a[1].get_ref<const std::string&>();if(op=="lt")return l<r;if(op=="lte")return l<=r;if(op=="gt")return l>r;return l>=r;}
        if(!a[0].is_number()||!a[1].is_number()) return false;
        double l=a[0].get<double>(), r=a[1].get<double>();
        if(op=="lt")return l<r; if(op=="lte")return l<=r; if(op=="gt")return l>r; return l>=r;
    }
    if(op=="add"||op=="sub"||op=="mul"||op=="div"||op=="mod"||op=="min"||op=="max"||op=="pow") {
        if(a.empty())return nullptr; double n=0; if(!a[0].is_number())return nullptr; n=a[0].get<double>();
        for(size_t i=1;i<a.size();++i){if(!a[i].is_number())return nullptr; double b=a[i].get<double>(); if(op=="add")n+=b; else if(op=="sub")n-=b; else if(op=="mul")n*=b; else if(op=="div"){if(b==0)return nullptr;n/=b;} else if(op=="mod"){if(b==0)return nullptr;n=std::fmod(n,b);} else if(op=="min")n=std::min(n,b); else if(op=="max")n=std::max(n,b); else n=std::pow(n,b);}
        return n;
    }
    if(op=="abs"&&!a.empty()&&a[0].is_number())return std::abs(a[0].get<double>());
    if(op=="neg"&&!a.empty()&&a[0].is_number())return -a[0].get<double>();
    if(op=="sqrt"&&!a.empty()&&a[0].is_number()&&a[0].get<double>()>=0)return std::sqrt(a[0].get<double>());
    if(op=="log"&&!a.empty()&&a[0].is_number()&&a[0].get<double>()>0)return std::log(a[0].get<double>());
    if(op=="floor"&&!a.empty()&&a[0].is_number())return std::floor(a[0].get<double>());
    if(op=="ceil"&&!a.empty()&&a[0].is_number())return std::ceil(a[0].get<double>());
    if(op=="round"&&!a.empty()&&a[0].is_number())return std::round(a[0].get<double>());
    if(op=="count"&&!a.empty()&&(a[0].is_array()||a[0].is_object()))return a[0].size();
    if(op=="length"&&!a.empty()&&(a[0].is_array()||a[0].is_object()||a[0].is_string()))return a[0].size();
    if(op=="isNull"&&!a.empty())return a[0].is_null();
    if(op=="isNumber"&&!a.empty())return a[0].is_number();
    if(op=="isInteger"&&!a.empty())return a[0].is_number_integer();
    if(op=="isString"&&!a.empty())return a[0].is_string();
    if(op=="isBoolean"&&!a.empty())return a[0].is_boolean();
    if(op=="isArray"&&!a.empty())return a[0].is_array();
    if(op=="isObject"&&!a.empty())return a[0].is_object();
    if(op=="unique"&&!a.empty()&&a[0].is_array()){auto values=a[0];std::sort(values.begin(),values.end());return std::unique(values.begin(),values.end())==values.end();}
    if(op=="between"&&a.size()>=3&&a[0].is_number()&&a[1].is_number()&&a[2].is_number())return a[0].get<double>()>=a[1].get<double>()&&a[0].get<double>()<=a[2].get<double>();
    if(op=="contains"&&a.size()>=2&&a[0].is_array())return std::find(a[0].begin(),a[0].end(),a[1])!=a[0].end();
    if(op=="in"&&a.size()>=2&&a[1].is_array())return std::find(a[1].begin(),a[1].end(),a[0])!=a[1].end();
    if(op=="sum"&&!a.empty()&&a[0].is_array()){double n=0;for(const auto& v:a[0]){if(!v.is_number())return nullptr;n+=v.get<double>();}return n;}
    if(op=="mean"&&!a.empty()&&a[0].is_array()&&!a[0].empty()){double n=0;for(const auto& v:a[0]){if(!v.is_number())return nullptr;n+=v.get<double>();}return n/a[0].size();}
    if(op=="concat"){std::string s;for(const auto& v:a){if(!v.is_string())return nullptr;s+=v.get<std::string>();}return s;}
    if((op=="startsWith"||op=="endsWith")&&a.size()>=2&&a[0].is_string()&&a[1].is_string()){auto s=a[0].get<std::string>(),p=a[1].get<std::string>();return op=="startsWith"?s.rfind(p,0)==0:(s.size()>=p.size()&&s.compare(s.size()-p.size(),p.size(),p)==0);}
    if(op=="matches"&&a.size()>=2&&a[0].is_string()&&a[1].is_string()){try{return std::regex_search(a[0].get<std::string>(),std::regex(a[1].get<std::string>()));}catch(...){return false;}}
    if(op=="adjacent"&&args.is_object()&&args.contains("array")) {
        auto arr=eval(args["array"],docs,currentFile,currentItem); if(!arr.is_array())return false;
        auto left=args.value("left","/max"), right=args.value("right","/min"), relation=args.value("operator","lte");
        auto at=[](json v,const std::string& ptr)->json {std::stringstream s(ptr);std::string part;std::getline(s,part,'/');while(std::getline(s,part,'/')){if(part.empty())continue;if(v.is_object()&&v.contains(part))v=v[part];else return nullptr;}return v;};
        for(size_t i=1;i<arr.size();++i){auto l=at(arr[i-1],left),r=at(arr[i],right);if(!l.is_number()||!r.is_number())return false;double u=l.get<double>(),v=r.get<double>();bool ok=relation=="lt"?u<v:relation=="lte"?u<=v:relation=="gt"?u>v:relation=="gte"?u>=v:relation=="eq"?u==v:u!=v;if(!ok)return false;}return true;
    }
    return nullptr;
}

void validateLevel(const fs::path& dir,const fs::path& configRoot,const json& level,
                   const std::unordered_map<std::string,std::string>& schemas,
                   ValidationResult& result,std::unordered_map<std::string,json>& docs,
                   const std::string& structurePath) {
    if(!level.is_object())return;
    if(level.contains("directories")&&level["directories"].is_array()) for(std::size_t directoryIndex = 0; directoryIndex < level["directories"].size(); ++directoryIndex) {
        const auto& d = level["directories"][directoryIndex];
        const auto directorySchemaPath = structurePath + "/directories/" + std::to_string(directoryIndex);
        auto name=d.value("name",""); auto p=dir/name;
        if(!fs::is_directory(p)){ if(d.value("required",false))issue(result,"required_directory_missing","Required directory is missing: "+name,name,"",directorySchemaPath+"/required"); continue; }
        if(d.contains("namedFiles")&&d["namedFiles"].is_array()) for(std::size_t namedFileIndex = 0; namedFileIndex < d["namedFiles"].size(); ++namedFileIndex) {
            const auto& namedFile = d["namedFiles"][namedFileIndex];
            const auto namedFileName = namedFile.value("name", "");
            if (!fs::is_regular_file(p / namedFileName) && namedFile.value("required", false)) {
                issue(result, "required_file_missing", "Required file is missing: " + namedFileName,
                      (p / namedFileName).lexically_relative(configRoot).generic_string(), "",
                      directorySchemaPath + "/namedFiles/" + std::to_string(namedFileIndex) + "/required");
            }
        }
        if(d.contains("files")&&d["files"].is_object()) {
            auto rule=d["files"]; auto pat=globRegex(rule.value("pattern","*.json")); std::regex re(pat); size_t count=0;
            for(const auto& entry:fs::directory_iterator(p)) if(entry.is_regular_file()&&std::regex_match(entry.path().filename().string(),re)) {
                ++count; auto rel=fs::relative(entry.path(),configRoot).generic_string();
                const auto schemaName=rule.value("schema", "");
                try { auto doc=readJson(entry.path()); docs[rel]=doc; auto schema=schemas.find(schemaName); if(schema==schemas.end()) throw std::runtime_error("Embedded schema not found: "+schemaName); JsonValidator validator(schema->second,schemaName); validator.validate(entry.path().filename().string(),doc); }
                catch(const ValidatorConfigJSONException& e){auto [path,message]=configExceptionDetails(e.what());auto schema=schemas.find(schemaName);const auto schemaPath=schema==schemas.end()?std::string{}:schemaPathForInstance(parseJson(schema->second),path,message);issue(result,"schema_validation",message,rel,path,schemaPath);}
                catch(const std::exception& e){issue(result,"schema_validation",e.what(),rel);}
                catch(...){issue(result,"schema_validation","Unknown validation error",rel);}
            }
            if(count<rule.value("minCount",size_t(0))) issue(result,"minimum_file_count","Directory '"+name+"' requires at least "+std::to_string(rule.value("minCount",size_t(0)))+" matching files",name,"",directorySchemaPath+"/files/minCount");
        }
        if(d.contains("directories"))validateLevel(p,configRoot,d,schemas,result,docs,directorySchemaPath);
    }
    if(level.contains("files")&&level["files"].is_array())for(std::size_t fileIndex = 0; fileIndex < level["files"].size(); ++fileIndex) {
        const auto& f = level["files"][fileIndex];
        const auto fileSchemaPath = structurePath + "/files/" + std::to_string(fileIndex);
        auto name=f.value("name",""); auto p=dir/name;
        if(!fs::is_regular_file(p)){if(f.value("required",false))issue(result,"required_file_missing","Required file is missing",name,"",fileSchemaPath+"/required");continue;}
        const auto schemaName=f.value("schema", "");
        try {auto doc=readJson(p); docs[name]=doc; auto schema=schemas.find(schemaName); if(schema==schemas.end()) throw std::runtime_error("Embedded schema not found: "+schemaName); JsonValidator v(schema->second,schemaName);v.validate(name,doc);}
        catch(const ValidatorConfigJSONException&e){auto [path,message]=configExceptionDetails(e.what());auto schema=schemas.find(schemaName);const auto schemaPath=schema==schemas.end()?std::string{}:schemaPathForInstance(parseJson(schema->second),path,message);issue(result,"schema_validation",message,name,path,schemaPath);}
        catch(const std::exception&e){issue(result,"schema_validation",e.what(),name);} catch(...){issue(result,"schema_validation","Unknown validation error",name);}
    }
}

struct OntogeneticCsvTable {
    std::vector<std::string> columns;
    std::vector<std::string> rows;
    std::vector<std::vector<std::string>> values;
};

std::vector<std::vector<std::string>> readCsvRecords(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Unable to open CSV file");
    const std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    std::vector<std::vector<std::string>> records;
    std::vector<std::string> record;
    std::string field;
    bool quoted = false;
    for (std::size_t i = 0; i < content.size(); ++i) {
        const char ch = content[i];
        if (ch == '"') {
            if (quoted && i + 1 < content.size() && content[i + 1] == '"') {
                field.push_back('"');
                ++i;
            } else if (quoted || field.empty()) {
                quoted = !quoted;
            } else {
                field.push_back(ch);
            }
        } else if (ch == ',' && !quoted) {
            record.push_back(std::move(field));
            field.clear();
        } else if ((ch == '\n' || ch == '\r') && !quoted) {
            if (ch == '\r' && i + 1 < content.size() && content[i + 1] == '\n') ++i;
            record.push_back(std::move(field));
            field.clear();
            if (!(record.size() == 1 && record.front().empty())) records.push_back(std::move(record));
            record.clear();
        } else {
            field.push_back(ch);
        }
    }
    if (quoted) throw std::runtime_error("Unterminated quoted field in CSV");
    if (!field.empty() || !record.empty()) {
        record.push_back(std::move(field));
        records.push_back(std::move(record));
    }
    if (!records.empty() && !records.front().empty() && records.front().front().size() >= 3 &&
        static_cast<unsigned char>(records.front().front()[0]) == 0xEF &&
        static_cast<unsigned char>(records.front().front()[1]) == 0xBB &&
        static_cast<unsigned char>(records.front().front()[2]) == 0xBF) {
        records.front().front().erase(0, 3);
    }
    return records;
}

OntogeneticCsvTable readOntogeneticTable(const fs::path& path) {
    const auto records = readCsvRecords(path);
    if (records.empty() || records.front().size() < 2) throw std::runtime_error("CSV has no column labels");
    OntogeneticCsvTable table;
    table.columns.assign(records.front().begin() + 1, records.front().end());
    for (std::size_t i = 1; i < records.size(); ++i) {
        if (records[i].empty()) continue;
        table.rows.push_back(records[i].front());
        table.values.emplace_back(records[i].begin() + 1, records[i].end());
    }
    return table;
}

bool parseCsvNumber(const std::string& text, double& value) {
    try {
        std::size_t consumed = 0;
        value = std::stod(text, &consumed);
        while (consumed < text.size() && std::isspace(static_cast<unsigned char>(text[consumed]))) ++consumed;
        return consumed == text.size() && std::isfinite(value);
    } catch (...) {
        return false;
    }
}

void validateOntogeneticCsv(const fs::path& configRoot, const std::string& filename,
                            const std::vector<std::string>& expectedColumns,
                            const std::vector<std::string>& expectedRows,
                            ValidationResult& result, const bool checkColumnSums) {
    const std::string relativeFile = "species/" + filename;
    const fs::path path = configRoot / "species" / filename;
    if (!fs::is_regular_file(path)) return; // The structure validator reports required files.

    OntogeneticCsvTable table;
    try {
        table = readOntogeneticTable(path);
    } catch (const std::exception& e) {
        issue(result, "invalid_ontogenetic_csv", e.what(), relativeFile, "", "#/fixedConstraints/ontogeneticLinks/csvFormat");
        return;
    }

    auto validateLabels = [&](const std::vector<std::string>& actual, const std::vector<std::string>& expected,
                              const std::string& axis, const std::string& schemaPath) {
        std::set<std::string> expectedSet(expected.begin(), expected.end());
        std::set<std::string> seen;
        for (const auto& label : actual) {
            if (!seen.insert(label).second) issue(result, "duplicate_ontogenetic_label", "Duplicate " + axis + " label '" + label + "'.", relativeFile, "/" + axis + "/" + escapePointerToken(label), schemaPath);
            if (!expectedSet.contains(label)) issue(result, "unexpected_ontogenetic_label", "Unexpected " + axis + " label '" + label + "'.", relativeFile, "/" + axis + "/" + escapePointerToken(label), schemaPath);
        }
        for (const auto& label : expected) {
            if (!seen.contains(label)) issue(result, "missing_ontogenetic_label", "Missing " + axis + " label '" + label + "'.", relativeFile, "/" + axis + "/" + escapePointerToken(label), schemaPath);
        }
    };
    validateLabels(table.columns, expectedColumns, "columns", "#/fixedConstraints/ontogeneticLinks/columnLabels");
    validateLabels(table.rows, expectedRows, "rows", "#/fixedConstraints/ontogeneticLinks/rowLabels");

    std::unordered_map<std::string, std::size_t> rowIndexes;
    std::unordered_map<std::string, std::size_t> columnIndexes;
    for (std::size_t i = 0; i < table.rows.size(); ++i) rowIndexes.emplace(table.rows[i], i);
    for (std::size_t i = 0; i < table.columns.size(); ++i) columnIndexes.emplace(table.columns[i], i);

    bool matrixShapeValid = true;
    for (std::size_t i = 0; i < table.rows.size(); ++i) {
        if (table.values[i].size() != table.columns.size()) {
            matrixShapeValid = false;
            issue(result, "invalid_ontogenetic_row_width", "CSV row '" + table.rows[i] + "' has " + std::to_string(table.values[i].size()) + " values; expected " + std::to_string(table.columns.size()) + ".", relativeFile, "/rows/" + escapePointerToken(table.rows[i]), "#/fixedConstraints/ontogeneticLinks/matrixShape");
        }
    }
    const std::set<std::string> expectedColumnSet(expectedColumns.begin(), expectedColumns.end());
    const std::set<std::string> expectedRowSet(expectedRows.begin(), expectedRows.end());
    for (const auto& [rowLabel, rowIndex] : rowIndexes) {
        if (!expectedRowSet.contains(rowLabel)) continue;
        for (const auto& [columnLabel, columnIndex] : columnIndexes) {
            if (!expectedColumnSet.contains(columnLabel) || columnIndex >= table.values[rowIndex].size()) continue;
            double value = 0.0;
            if (!parseCsvNumber(table.values[rowIndex][columnIndex], value)) {
                matrixShapeValid = false;
                issue(result, "invalid_ontogenetic_value", "Cell is not a finite number.", relativeFile,
                      "/rows/" + escapePointerToken(rowLabel) + "/columns/" + escapePointerToken(columnLabel),
                      "#/fixedConstraints/ontogeneticLinks/numericCells");
            }
        }
    }

    const bool headersMatch = expectedColumnSet.size() == table.columns.size() && expectedRowSet.size() == table.rows.size() &&
        columnIndexes.size() == expectedColumnSet.size() && rowIndexes.size() == expectedRowSet.size() &&
        std::all_of(expectedColumns.begin(), expectedColumns.end(), [&](const auto& label) { return columnIndexes.contains(label); }) &&
        std::all_of(expectedRows.begin(), expectedRows.end(), [&](const auto& label) { return rowIndexes.contains(label); });
    if (checkColumnSums && matrixShapeValid && headersMatch) {
        for (const auto& column : expectedColumns) {
            const auto columnIndex = columnIndexes.at(column);
            double sum = 0.0;
            for (const auto& row : expectedRows) {
                const auto rowIndex = rowIndexes.at(row);
                double value = 0.0;
                if (!parseCsvNumber(table.values[rowIndex][columnIndex], value)) { matrixShapeValid = false; break; }
                if (value > 0.0) sum += value;
            }
            if (matrixShapeValid && std::abs(sum - 1.0) > 1e-5) {
                issue(result, "ontogenetic_preference_sum", "Preference values for predator '" + column + "' sum to " + std::to_string(sum) + "; expected 1.0.", relativeFile,
                      "/columns/" + escapePointerToken(column), "#/fixedConstraints/ontogeneticLinks/preferenceColumnSum");
            }
        }
    }
}

void validateOntogeneticLinks(const fs::path& configRoot, const std::unordered_map<std::string, json>& docs,
                              const ValidationResult& jsonValidation, ValidationResult& result) {
    for (const auto& diagnostic : jsonValidation.errors) {
        if (diagnostic.code == "schema_validation" &&
            (diagnostic.file.rfind("species/", 0) == 0 || diagnostic.file.rfind("resources/", 0) == 0)) return;
    }

    std::vector<std::string> predatorLabels;
    std::vector<std::string> preyLabels;
    std::set<std::string> speciesNames;
    for (const auto& [file, document] : docs) {
        const bool animal = file.rfind("species/", 0) == 0;
        const bool resource = file.rfind("resources/", 0) == 0;
        if (!animal && !resource) continue;
        try {
            const auto name = animal ? document.at("animal").at("name").get<std::string>()
                                     : document.at("resource").at("name").get<std::string>();
            const std::size_t instarCount = animal ? document.at("animal").at("individualsPerInstar").size() : 1;
            if (instarCount == 0) {
                issue(result, "invalid_ontogenetic_species", "Species '" + name + "' has no instars.", file, "", "#/fixedConstraints/ontogeneticLinks/speciesInstars");
                continue;
            }
            if (!speciesNames.insert(name).second) {
                issue(result, "duplicate_ontogenetic_species_name", "Species name '" + name + "' is duplicated.", file, animal ? "/animal/name" : "/resource/name", "#/fixedConstraints/ontogeneticLinks/speciesNames");
                continue;
            }
            for (std::size_t instar = 1; instar <= instarCount; ++instar) {
                const auto label = name + "$" + std::to_string(instar);
                preyLabels.push_back(label);
                if (animal) predatorLabels.push_back(label);
            }
        } catch (const std::exception& e) {
            issue(result, "invalid_ontogenetic_species", e.what(), file, "", "#/fixedConstraints/ontogeneticLinks/speciesInstars");
        }
    }

    if (predatorLabels.empty() || preyLabels.empty()) return;
    validateOntogeneticCsv(configRoot, "ontogeneticLinksPreference.csv", predatorLabels, preyLabels, result, true);
    validateOntogeneticCsv(configRoot, "ontogeneticLinksProfitability.csv", predatorLabels, preyLabels, result, false);
}
}

ConfigurationValidator::ConfigurationValidator(std::string_view configurationStructure,
                                               std::string_view constraints,
                                               std::unordered_map<std::string,std::string_view> jsonSchemas)
    : configurationStructure_(configurationStructure), constraints_(constraints) {
    for (const auto& [name, content] : jsonSchemas) jsonSchemas_.emplace(name, content);
}

ValidationResult ConfigurationValidator::validate(
    const fs::path& configDirectory,
    const std::string_view expectedSchemaVersion) const {
    ValidationResult result; std::unordered_map<std::string,json> docs;
    try {
        auto structure=parseJson(configurationStructure_);
        validateLevel(configDirectory,configDirectory,structure.at("root"),jsonSchemas_,result,docs,"#/root");
        if(!expectedSchemaVersion.empty()) for(const auto& [file,document]:docs) {
            if(document.is_object()&&document.contains("version")&&document["version"].is_string()&&document["version"].get<std::string>()!=expectedSchemaVersion)
                issue(result,"schema_version_mismatch","Expected configuration schema version "+std::string(expectedSchemaVersion)+", found "+document["version"].get<std::string>(),file,"/version","#/properties/version");
        }
        validateOntogeneticLinks(configDirectory, docs, result, result);
        if(!constraints_.empty()) {
            auto constraints=parseJson(constraints_);
            if(constraints.contains("constraints")&&constraints["constraints"].is_array()) for(std::size_t constraintIndex = 0; constraintIndex < constraints["constraints"].size(); ++constraintIndex) {
                const auto& c = constraints["constraints"][constraintIndex];
                const auto assertSchemaPath = "#/constraints/" + std::to_string(constraintIndex) + "/assert";
                try {
                    const auto filePattern = c.value("forEachFile", "");
                    if (!filePattern.empty()) {
                        const std::regex match(globRegex(filePattern));
                        for (const auto& [file, document] : docs) if (std::regex_match(file, match)) {
                            std::vector<json> items;
                            std::vector<std::string> itemPaths;
                            if (c.contains("forEachPath")) items = selectPath(document, c.at("forEachPath").get<std::string>(), &itemPaths);
                            else items.push_back(nullptr);
                            for (std::size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex) {
                                const auto& item = items[itemIndex];
                                const json* itemScope = c.contains("forEachPath") ? &item : nullptr;
                                if (c.contains("where")) {
                                    const auto filter = eval(c.at("where"), docs, file, itemScope);
                                    if (!filter.is_boolean()) {
                                        issue(result, "invalid_constraint", "Constraint 'where' expression did not evaluate to a boolean", "constraints.json", "", "#/constraints/" + std::to_string(constraintIndex) + "/where");
                                        break;
                                    }
                                    if (!filter.get<bool>()) continue;
                                }
                                const auto resultValue = eval(c.at("assert"), docs, file, itemScope);
                                const bool passed = resultValue.is_boolean() ? resultValue.get<bool>() :
                                    resultValue.is_array() && std::all_of(resultValue.begin(), resultValue.end(), [](const json& v) { return v.is_boolean() && v.get<bool>(); });
                                if (!passed) {
                                    auto message = c.value("description", c.value("message", "Configuration constraint failed"));
                                    std::string relativePath = c.value("path", "");
                                    if (relativePath.empty()) relativePath = firstReferencePath(c.at("assert"), c.contains("forEachPath") ? "$item" : "$current");
                                    std::string instancePath = relativePath;
                                    if (c.contains("forEachPath")) {
                                        const auto& itemPath = itemPaths.at(itemIndex);
                                        instancePath = itemPath + (relativePath.empty() ? "" : relativePath.front() == '/' ? relativePath : "/" + relativePath);
                                    }
                                    if (c.value("severity", "error") == "warning") warning(result, c.value("code", "constraint_failed"), message, file, instancePath, assertSchemaPath);
                                    else issue(result, c.value("code", "constraint_failed"), message, file, instancePath, assertSchemaPath);
                                }
                            }
                        }
                    } else {
                        const auto resultValue = eval(c.at("assert"), docs);
                        if (!resultValue.is_boolean() || !resultValue.get<bool>()) {
                            auto message = c.value("description", c.value("message", "Configuration constraint failed"));
                            auto instancePath = c.value("path", firstReferencePath(c.at("assert"), "$current"));
                            if (c.value("severity", "error") == "warning") warning(result, c.value("code", "constraint_failed"), message, c.value("file", ""), instancePath, assertSchemaPath);
                            else issue(result, c.value("code", "constraint_failed"), message, c.value("file", ""), instancePath, assertSchemaPath);
                        }
                    }
                }
                catch(const std::exception&e){issue(result,"invalid_constraint",e.what(),"constraints.json","",assertSchemaPath);}
            }
        }
    } catch(const std::exception&e){ issue(result, "validator_setup", e.what(), "configuration_structure.json"); }
    return result;
}
