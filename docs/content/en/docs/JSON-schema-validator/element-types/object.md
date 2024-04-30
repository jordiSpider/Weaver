---
title: "Object"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    JSON-schema-validator:
      parent: "element-types"
weight: 2250
toc: true
---

The **`object`** type is used to represent a map of key-value elements.

{{< alert context="warning" text="Map keys must be strings." />}}

```json
{ 
  "type": "object",
  "properties": {},
  "additionalProperties": true
}
```

Validation results with this scheme are shown below:

{{< example context="success" text="{<br> &ensp; \"first_key\"\: \"first_value\",<br> &ensp; \"another_value\"\: 45.0<br>}" />}}
{{< example context="danger" text="{<br> &ensp; 5: \"integer\",<br> &ensp; 2.3: \"float\"<br>}" />}}
{{< example context="danger" text="\"Not an object\"" />}}
{{< example context="danger" text="[\"An\", \"array\", \"not\", \"an\", \"object\"]" />}}

## Define properties

{{< alert context="warning" text="This property is required." />}}

A property is composed of a key, which is the name of the property; and a value, which will be the definition of any element type

### Properties

The properties of an object are defined in **`properties`** property.

A possible example of an object schema with a person's data using this property would be the following:

```json
{ 
  "type": "object",
  "properties": {
    "first_name": {
      "type": "string",
      "required": false
    },
    "last_name": {
      "type": "string",
      "required": false
    },
    "age": {
      "type": "unsigned_integer",
      "required": false
    }
  },
  "additionalProperties": true
}
```

{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\",<br> &ensp; \"age\"\: 23<br>}" />}}
{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"age\"\: 18,<br> &ensp; \"last_name\"\: \"Brown\"<br>}" />}}
{{< example context="danger" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"age\"\: 18.5,<br> &ensp; \"last_name\"\: \"Brown\"<br>}" />}}

### Pattern properties

In case you do not want to define all properties implicitly, you can make use of patterns for the definition of properties. These patterns will be defined as regular expressions by means of strings. To define the properties in this way, **`patternProperties`** property is used.

{{< alert context="warning" text="The regular expressions used follow the logic of the <a href=\"https://cplusplus.com/reference/regex/\">regular expressions used in C++</a>." />}}

This definition of properties can also be complementary to the implicit definition of properties.

In this way of defining properties, the regular expression will replace the key of the property.

A possible example of an object schema with a person's data using this property would be the following:

```json
{ 
  "type": "object",
  "properties": {
    "first_name": {
      "type": "string",
      "required": false
    },
    "last_name": {
      "type": "string",
      "required": false
    },
    "age": {
      "type": "unsigned_integer",
      "required": false
    }
  },
  "patternProperties": {
    "address[1-2]": {
      "type": "string",
      "required": false
    }
  },
  "additionalProperties": true
}
```

{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\",<br> &ensp; \"age\"\: 23,<br> &ensp; \"address1\"\: \"940 Goldendale Dr\",<br> &ensp; \"address2\"\: \"1536 Stellar Dr\"<br>}" />}}
{{< example context="danger" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\",<br> &ensp; \"age\"\: 23,<br> &ensp; \"address1\"\: \"940 Goldendale Dr\",<br> &ensp; \"address2\"\: \"1536 Stellar Dr\",<br> &ensp; \"address3\"\: \"1575 Evergreen Ave\"<br>}" />}}

## Required properties

{{< alert context="warning" text="This property is required." />}}

To define which properties of the object are essential for the definition of the object, **`required`** property is used. This property is added to the definition of the value of each key. It is defined by a boolean value.

If a property is defined as required, for an object to be validated as correct, it must contain this property.

A possible example of an object schema with a person's data using this property would be the following:

```json
{ 
  "type": "object",
  "properties": {
    "first_name": {
      "type": "string",
      "required": true
    },
    "last_name": {
      "type": "string",
      "required": true
    },
    "age": {
      "type": "unsigned_integer",
      "required": false
    }
  },
  "additionalProperties": true
}
```

{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\",<br> &ensp; \"age\"\: 23<br>}" />}}
{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\"<br>}" />}}
{{< example context="danger" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"age\"\: 23,<br>}" />}}

## Additional properties

{{< alert context="warning" text="This property is required." />}}

Once we have defined the required elements for the object definition, we can decide whether we want only the required properties to be involved in the object definition or not. This decision is made using the **`additionalProperties`** property.

This property is defined by a boolean value.

A possible example of an object schema with a person's data using this property would be the following:

```json
{ 
  "type": "object",
  "properties": {
    "first_name": {
      "type": "string",
      "required": true
    },
    "last_name": {
      "type": "string",
      "required": true
    },
    "age": {
      "type": "unsigned_integer",
      "required": false
    }
  },
  "additionalProperties": false
}
```

{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\"<br>}" />}}
{{< example context="danger" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\",<br> &ensp; \"age\"\: 23<br>}" />}}

## Size

{{< alert context="info" text="This property is optional." />}}

The size of the object can be defined by a range of values. The range consists of a minimum, **`minProperties`**; and a maximum, **`maxProperties`**.

Both variables are not dependent, the maximum can be specified without specifying the minimum and vice versa.

Both the maximum and the minimum are specified by a **non-negative** number.

A possible example of an object schema using this property would be the following:

```json
{ 
  "type": "object",
  "properties": {
    "first_name": {
      "type": "string",
      "required": false
    },
    "last_name": {
      "type": "string",
      "required": false
    },
    "age": {
      "type": "unsigned_integer",
      "required": false
    },
    "address": {
      "type": "string",
      "required": false
    }
  },
  "minProperties": 2,
  "maxProperties": 3,
  "additionalProperties": true
}
```

{{< example context="danger" text="{<br> &ensp; \"first_name\"\: \"John\"<br>}" />}}
{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\"<br>}" />}}
{{< example context="success" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\",<br> &ensp; \"age\"\: 23<br>}" />}}
{{< example context="danger" text="{<br> &ensp; \"first_name\"\: \"John\",<br> &ensp; \"last_name\"\: \"Brown\",<br> &ensp; \"age\"\: 23,<br> &ensp; \"address\"\: \"940 Goldendale Dr\"<br>}" />}}

## Required conditions

{{< alert context="info" text="This property is optional." />}}



## Warning conditions

{{< alert context="info" text="This property is optional." />}}



## Conditionals

{{< alert context="info" text="This property is optional." />}}

To improve the adaptability of schemas, objects have parts of the schema that depend on conditions. All these conditional parts are defined in **`conditionals`** property.

This property is defined as a list of conditionals, where in each conditional a condition is checked and the schema block corresponding to the result of the condition is defined. The number of conditions to be defined in the list is unlimited.

Each list item is defined as an object, with three distinct properties.

The object properties are [**`if`**](#if), [**`then`**](#condition-block) and [**`else`**](#condition-block). The [**`if`**](#if) and [**`then`**](#condition-block) property are required in any definition of a condicinal; while [**`else`**](#condition-block) property is optional, in case we want to define a block in case the condicinal is not met.

### If

{{< alert context="warning" text="This property is required." />}}

The **`if`** property is defined as an object, where an operator is defined. For more information on operators, see the [operator's section](../../operators/).

The value of the condition is equal to the result of the operator. This value is evaluated based on C++ conditional logic.

### Condition block 

A conditional block is defined in both then and else property.

A conditional block is defined as an object that can have up to three properties. These properties are [**`dst`**](#destination), [**`add`**](#add-block) and [**`remove`**](#remove-block).

#### Destination

{{< alert context="info" text="This property is optional." />}}

The destination of a conditional block is defined in **`dst`** property.

The destination indicates the level where the changes indicated in the conditional block are to be made. If this property is not defined, the destination will have as default value the current level where the conditional block is located.

The **`dst`** property is defined as an array of strings, indicating the properties that must be accessed to reach the desired level, knowing that we start from the current object.

This property is useful if you want to work on an object that is contained in the current object, to avoid having to specify longer paths in the modification hints.

#### Remove block

{{< alert context="info" text="This property is optional." />}}

The elements to be removed from the current scheme are defined in **`remove`** property.

Essentially the elements to be eliminated are two, the properties and the required condition of the properties. In addition, both elements can be specified explicitly; or by means of patterns, i.e. regular expressions. Therefore, we have four ways of elimination.

The **`remove`** property is defined as an object containing four properties where to define the four removal forms. All properties of the object are optional.

##### Properties

If we want to define the properties to be removed explicitly, we use **`properties`** property. This property is defined as an array of strings, where each string indicates the name of the property to delete.

Whereas if we do not want to indicate each of the properties to delete and we can create a pattern that encompasses all the properties we want to delete, we use **`patternProperties`** property. This property is defined as an array of strings, where each string indicates the pattern that the properties must meet in order to be removed. As indicated above, the patterns are regular expressions.

{{< alert context="warning" text="The regular expressions used follow the logic of the <a href=\"https://cplusplus.com/reference/regex/\">regular expressions used in C++</a>." />}}

##### Required properties

If we want to explicitly define the properties that will no longer be required, we use the **`required`** property. This property is defined as an array of strings, where each string indicates the name of the property that will no longer be required.

While if we do not want to indicate each of the properties that will no longer be required and we can create a pattern that encompasses all the desired properties, we will use the **`patternRequired`** property. This property is defined as an array of strings, where each string indicates the pattern that the properties must meet in order to no longer be required. As indicated above, patterns are regular expressions.

{{< alert context="warning" text="The regular expressions used follow the logic of the <a href=\"https://cplusplus.com/reference/regex/\">regular expressions used in C++</a>." />}}

#### Add block

{{< alert context="info" text="This property is optional." />}}



### Example
