
#include "App/Model/Model.h"

#include "App/Model/IBM/IBM.h"


using namespace std;
namespace fs = std::filesystem;



Model* Model::createInstance(Type modelType, View& view)
{
    switch(modelType) {
        case Type::IBM: {
            return new IBM(view);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}



Model::Model(View& view)
    : view(view)
{

}

Model::~Model()
{

}
