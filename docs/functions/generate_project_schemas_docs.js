function getBooleanContent(schema, level) {
    let schemaContent = '* **Type**: [Boolean](../../element-types/boolean)\n';

    return schemaContent;
}

function getFloatContent(schema, level) {
    let schemaContent = '* **Type**: [Float](../../element-types/numeric-types/float)\n';

    return schemaContent;
}

function getUnsignedIntegerContent(schema, level) {
    let schemaContent = '* **Type**: [Unsigned integer](../../element-types/numeric-types/unsigned-integer)\n';

    return schemaContent;
}

function getIntegerContent(schema, level) {
    let schemaContent = '* **Type**: [Integer](../../element-types/numeric-types/integer)\n';

    return schemaContent;
}

function getStringContent(schema, level) {
    let schemaContent = '* **Type**: [String](../../element-types/string)\n';

    return schemaContent;
}

function getArrayContent(schema, level) {
    let schemaContent = '* **Type**: [Array](../../element-types/array)\n';

    try {
        schemaContent += `* **Minimum items**: ${schema.minItems}\n`;
    } catch (error) {}

    try {
        schemaContent += `* **Maximum items**: ${schema.maxItems}\n`;
    } catch (error) {}

    try {
        let prefixItems = schema.prefixItems;

        schemaContent += '<details><summary> <strong>Prefix items</strong>: </summary>\n\n'

        for (let i = 0; i < prefixItems.length; i++) {
            schemaContent += `<details><summary> <strong>Item ${i}</strong>: </summary>\n\n`

            schemaContent += getElemContent(prefixItems[i], level+1);

            schemaContent += '</details>\n';
        }

        schemaContent += '</details>\n';
    } catch (error) {}

    try {
        let items = schema.items;

        schemaContent += '<details><summary> <strong>Items</strong>: </summary>\n\n'

        schemaContent += getElemContent(items, level+1);

        schemaContent += '</details>\n';
    } catch (error) {}

    try {
        if(schema.requiredAll) {
            schemaContent += `* **Required all**: Yes\n`;
        } else {
            schemaContent += `* **Required all**: No\n`;
        }
    } catch (error) {}

    try {
        let itemsOrder = schema.itemsOrder;

        switch (itemsOrder) {
        case 'increasing':
            schemaContent += `* **Items order**: Increasing order\n`;
            break;
        case 'strictly_increasing':
            schemaContent += `* **Items order**: Strictly increasing order\n`;
            break;
        case 'decreasing':
            schemaContent += `* **Items order**: Decreasing order\n`;
            break;
        case 'strictly_decreasing':
            schemaContent += `* **Items order**: Strictly decreasing order\n`;
            break;
        default:
            break;
        }

        schemaContent += '<details><summary> <strong>Items</strong>: </summary>\n\n'

        schemaContent += getElemContent(items, level+1);

        schemaContent += '</details>\n';
    } catch (error) {}

    return schemaContent;
}

function getObjectContent(schema, level) {
    let schemaContent = '* **Type**: [Object](../../element-types/object)\n';

    schemaContent += '* **Additional properties**: ';

    if (schema.additionalProperties) {
        schemaContent += 'Yes\n';
    } else {
        schemaContent += 'No\n';
    }

    schemaContent += '<details><summary> <strong>Properties</strong>: </summary>\n\n'
    
    for (let property in schema.properties) {
        schemaContent += `<details><summary><h${level+3}>${property}</h${level+3}>: </summary>\n\n`;

        schemaContent += getElemContent(schema.properties[property], level+1);

        schemaContent += '</details>\n';
    }

    schemaContent += '</details>\n';
    
    return schemaContent;
}

function getElemContent(schema, level) {
    switch (schema.type) {
    case 'object':
        return getObjectContent(schema, level);
        break;
    case 'array':
        return getArrayContent(schema, level);
        break;
    case 'string':
        return getStringContent(schema, level);
        break;
    case 'integer':
        return getIntegerContent(schema, level);
        break;
    case 'unsigned_integer':
        return getUnsignedIntegerContent(schema, level);
        break;
    case 'float':
        return getFloatContent(schema, level);
        break;
    case 'boolean':
        return getBooleanContent(schema, level);
        break;
    default:
        break;
    }
}

function getSchemaContent(schema) {
    let schemaContent = '## root\n\n';

    schemaContent += getElemContent(schema, 0);

    return schemaContent;
}

const fs = require('fs');

fs.readdir('../schema', (error, files) => {
    if (error) {
        console.log(error);
        return;
    }

    for (let i = 0; i < files.length; i++) {
        const file = files[i];

        const filename = file.split('.')[0];

        let contentHTML = `---
title: "${file}"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    JSON-schema-validator:
      parent: "project-schemas"
weight: 24${i+1}0
toc: false
---`;

        const schema = JSON.parse(fs.readFileSync('../schema/' + file, 'utf8'));

        const schemaContent = getSchemaContent(schema);

        fs.writeFileSync('content/en/docs/JSON-schema-validator/project-schemas/' + filename + '-schema.md', contentHTML + '\n\n' + schemaContent);
    }
});
