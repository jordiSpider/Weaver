import json
import os
import re

def remove_comments(json_string):
    """Remove comments from a JSON string."""
    pattern = r'(?<!\\)//.*?$|/\*.*?\*/'
    # regular expression to match comments (// and /* */)
    regex = re.compile(pattern, re.DOTALL | re.MULTILINE)
    return regex.sub('', json_string)

def remove_quotes(obj):
    if isinstance(obj, dict):
        for k, v in obj.items():
            obj[k] = remove_quotes(v)
    elif isinstance(obj, list):
        for i, v in enumerate(obj):
            obj[i] = remove_quotes(v)
    elif isinstance(obj, str):
        try:
            obj = int(obj)
        except ValueError:
            try:
                obj = float(obj)
            except ValueError:
                pass
    return obj

def process_json_file(input_path, output_path):
    # Open the JSON file and remove comments
    with open(input_path, 'r') as f:
        json_string = f.read()
    json_string = remove_comments(json_string)
    data = json.loads(json_string)

    # Remove the quotation marks from the numbers
    data = remove_quotes(data)

    # Save the modified JSON file
    with open(output_path, 'w') as f:
        json.dump(data, f, indent=4)

def main(input_path):
    if os.path.isfile(input_path):
        # Process a single file
        process_json_file(input_path, input_path)
    elif os.path.isdir(input_path):
        # Process all files in the directory
        for root, dirs, files in os.walk(input_path):
            for file in files:
                if file.endswith('.json'):
                    input_file_path = os.path.join(root, file)
                    output_file_path = input_file_path
                    process_json_file(input_file_path, output_file_path)

if __name__ == '__main__':
    import sys
    input_path = sys.argv[1]
    main(input_path)
