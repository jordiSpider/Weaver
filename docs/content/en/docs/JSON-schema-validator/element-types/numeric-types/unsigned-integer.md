---
title: "Unsigned integer"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    JSON-schema-validator:
      element-types:
        parent: "numeric-types"
weight: 2232
toc: true
---

The **`unsigned_integer`** type is used for unsigned integers.

```json
{ "type": "unsigned_integer" }
```

Validation results with this scheme are shown below:

{{< example context="success" text="34" />}}
{{< example context="success" text="0" />}}
{{< example context="danger" text="-12" />}}
{{< example context="danger" text="5.2" />}}
{{< example context="danger" text="\"56\"" />}}


## Range

{{< alert context="info" text="This property is optional." />}}

The value that the integer can have can be bounded by a range of values. This range of values has a **`minimum`** and a **`maximum`**.

Both variables must be defined by integers numbers, in addition both variables are not dependent, the **`maximum`** can be specified without specifying the **`minimum`** and vice versa.

In addition, it is possible to define whether the values specifying the range limits are included in the range or not. The inclusion or not of the limits is done through the variables **`exclusiveMinimum`** and **`exclusiveMaximum`**.

These exclusion variables have a bool value. If either has a value of true, the value of the boundary to which the variable, whose value is true, is associated will not be included in the range of allowed values.

If x is the value being validated, the following must hold true:

> If **`exclusiveMinimum`** == true
> * x > **`minimum`**
>
> Else
> * x >= **`minimum`**

> If **`exclusiveMaximum`** == true
> * x < **`maximum`**
>
> Else
> * x <= **`maximum`**

{{< alert context="warning" text="The exclusion variable cannot be defined in the schema without having defined the value of the boundary with which it is associated." />}}

A possible example of a scheme using this property would be as follows:

```json
{ 
  "type": "unsigned_integer",
  "minimum": 2,
  "maximum": 6,
  "exclusiveMinimum": true
}
```

Validation results with this scheme are shown below:

{{< example context="danger" text="2" />}}
{{< example context="success" text="3" />}}
{{< example context="success" text="6" />}}
{{< example context="danger" text="7" />}}


## Multiples

{{< alert context="info" text="This property is optional." />}}

You can restrict the value of the integer to the condition that it is a multiple of a certain integer value.

The integer value of which it is to be a multiple is defined by the parameter **`multipleOf`**.

{{< alert context="warning" text="The parameter <strong><code>multipleOf</code></strong> must have a non-zero integer value." />}}

A possible example of a scheme using this property would be as follows:

```json
{ 
  "type": "unsigned_integer",
  "multipleOf": 3
}
```

or

```json
{ 
  "type": "unsigned_integer",
  "multipleOf": -3
}
```

Validation results with this scheme are shown below:

{{< example context="danger" text="2" />}}
{{< example context="success" text="3" />}}
{{< example context="success" text="9" />}}
{{< example context="success" text="0" />}}
