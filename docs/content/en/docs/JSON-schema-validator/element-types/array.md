---
title: "Array"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    JSON-schema-validator:
      parent: "element-types"
weight: 2240
toc: true
---

The **`array`** type is used to represent two types of structures, lists and tuples. In addition, a third type can be represented, resulting from the combination of both structures, by placing a tuple at the beginning and a list at the end.

## List properties

{{< alert context="info" text="Lists are a series of elements of the same type and where the position of the different elements is not fixed." />}}

A list schema is useful for arrays of arbitrary length where each item matches the same schema.

### Items

{{< alert context="warning" text="This property is required for list schemas." />}}

The schema of the items to be part of the list is defined in the **`items`** property. In this property you can define any [kind of element](..) (string, integer, array, ...).

A possible example of a string list schema using this property would be the following:

```json
{ 
  "type": "array",
  "items": {
    "type": "string"
  }
}
```

Validation results with this scheme are shown below:

{{< example context="success" text="[\"house\", \"garden\", \"kitchen\"]" />}}
{{< example context="danger" text="[\"flower\", 5, \"bird\"]" />}}
{{< example context="success" text="[]" />}}

### Items order

{{< alert context="info" text="This property is optional." />}}

{{< alert context="warning" text="List items must be comparable to each other in order to use this property. The validator uses C++ types internally, therefore it must be checked that the types in the list can be compared in C++." />}}

The **`itemsOrder`** property defines the order to be followed by the items in the list.

The available orders are as follows:

|         **Order**         |     **Keyword**     |
|:-------------------------:|:-------------------:|
|      Increasing order     |      increasing     |
| Strictly increasing order | strictly_increasing |
|      Decreasing order     |      decreasing     |
| Strictly decreasing order | strictly_decreasing |

The property is defined by a string. In the schema, the chosen keyword is placed as a string.

A possible example of a strictly increasing integer list schema using this property would be the following:

```json
{ 
  "type": "array",
  "items": {
    "type": "integer"
  },
  "itemsOrder": "strictly_increasing"
}
```

Validation results with this scheme are shown below:

{{< example context="success" text="[-2, 5, 8]" />}}
{{< example context="danger" text="[0, 3, 3, 7]" />}}
{{< example context="danger" text="[-3, 2, 6, 1]" />}}


## Tuple properties

{{< alert context="info" text="Tuples are a series of elements that need not be of the same type and where the position of the element within the tuple is important." />}}

A tuple schema is useful when the array is a collection of items where each has a different schema and the ordinal index of each item is meaningful.

### Prefix items

The **`prefixItems`** property defines the content of the tuple. This property is defined as an array, where the elements of the array are the schemas of the different elements of the tuple.

For example you can represent a person's data as a tuple as follows:

```
John Stones 25
```

```json
[first_name, last_name, age]
```

Each of these fields will have a different schema:

* first_name: Must be a string
* last_name: Must be a string
* age: Must be an unsigned integer

A possible example of a tuple schema with a person's data using this property would be the following:

```json
{ 
  "type": "array",
  "prefixItems": [
    {
      "type": "string"
    },
    {
      "type": "string"
    },
    {
      "type": "unsigned_integer"
    }
  ]
}
```

Validation results with this scheme are shown below:

{{< example context="success" text="[\"Tom\", \"Davies\", 19]" />}}
{{< example context="success" text="[\"Taylor\", \"Brown\"]" />}}
{{< example context="danger" text="[22, \"Richard\", \"Jones\"]" />}}

### Required all items

{{< alert context="info" text="This property is optional." />}}

If you want all fields in the tuple to be required, you can use **`requiredAll`** property.

The value of this property is defined by a boolean value.

A possible example of a tuple schema, following the example of personal data in the previous section, using this property would be the following:

```json
{ 
  "type": "array",
  "prefixItems": [
    {
      "type": "string"
    },
    {
      "type": "string"
    },
    {
      "type": "unsigned_integer"
    }
  ],
  "requiredAll": true
}
```

Validation results with this scheme are shown below:

{{< example context="success" text="[\"Tom\", \"Davies\", 19]" />}}
{{< example context="danger" text="[\"Taylor\", \"Brown\"]" />}}

### Additional items

{{< alert context="info" text="This property is optional." />}}

{{< alert context="warning" text="If additional elements are defined, the list schema starts to be used once all the elements of the tuple have been validated, i.e. if the tuple is composed of 3 elements, the additional list schema will not start to be used until the 4th element of the array." />}}

{{< alert context="warning" text="The <strong><code>itemsOrder</code></strong> property will only affect the list items and not the tuple items." />}}

In case you want to have additional elements to the elements of the tuple, you can define **`items`** property.

In short, what we are doing is adding a list to the end of the tuple.

For more details about **`items`** property you can go to its [section](#items).

A possible example of schema using this property would be the following:

```json
{ 
  "type": "array",
  "prefixItems": [
    {
      "type": "string"
    },
    {
      "type": "string"
    },
    {
      "type": "unsigned_integer"
    }
  ],
  "requiredAll": true,
  "items": {
    "type": "integer"
  },
  "itemsOrder": "strictly_increasing"
}
```

Validation results with this scheme are shown below:

{{< example context="success" text="[\"Tom\", \"Davies\", 19]" />}}
{{< example context="success" text="[\"Tom\", \"Davies\", 19, -2, 5]" />}}
{{< example context="danger" text="[\"Tom\", \"Davies\", 19, -2, \"flower\", 4]" />}}

## General properties

### Size

{{< alert context="warning" text="This property is optional." />}}

The size of the array can be defined by a range of values. The range consists of a minimum, **`minItems`**; and a maximum, **`maxItems`**.

Both variables are not dependent, the maximum can be specified without specifying the minimum and vice versa.

Both the maximum and the minimum are specified by a **non-negative** number.

A possible example of a scheme using this property and list properties would be as follows:

```json
{ 
  "type": "array",
  "minLength": 2,
  "maxLength": 5,
  "items": {
    "type": "unsigned_integer"
  }
}
```

Validation results with this scheme are shown below:

{{< example context="danger" text="[5]" />}}
{{< example context="success" text="[1, 3]" />}}
{{< example context="success" text="[0, 5, 2, 4, 8]" />}}
{{< example context="danger" text="[8, 6, 2, 9, 4, 6]" />}}