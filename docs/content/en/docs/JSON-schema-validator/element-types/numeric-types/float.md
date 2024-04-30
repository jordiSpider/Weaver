---
title: "Float"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    JSON-schema-validator:
      element-types:
        parent: "numeric-types"
weight: 2233
toc: true
---

The **`float`** type is used for floating point numbers.

```json
{ "type": "float" }
```

Validation results with this scheme are shown below:

{{< example context="success" text="5.2" />}}
{{< example context="success" text="-12.3" />}}
{{< example context="danger" text="7" />}}
{{< example context="danger" text="\"56\"" />}}


## Range

{{< alert context="info" text="This property is optional." />}}

The value that the floating point number can have can be bounded by a range of values. This range of values has a **`minimum`** and a **`maximum`**. 

Both variables must be defined by floating-point numbers, in addition both variables are not dependent, the **`maximum`** can be specified without specifying the **`minimum`** and vice versa.

If x is the value being validated, the following must hold true:

> * x >= **`minimum`**
> * x <= **`maximum`**

A possible example of a scheme using this property would be as follows:

```json
{ 
  "type": "float",
  "minimum": -3.4,
  "maximum": 6.2,
}
```

Validation results with this scheme are shown below:

{{< example context="danger" text="-3.1" />}}
{{< example context="success" text="0.0" />}}
{{< example context="success" text="5.8" />}}
{{< example context="danger" text="7.4" />}}
