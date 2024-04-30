---
title: "String"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    JSON-schema-validator:
      parent: "element-types"
weight: 2210
toc: true
---

The **`string`** type is used for strings of text. It may contain Unicode characters.

```json
{ "type": "string" }
```

Validation results with this scheme are shown below:

{{< example context="success" text="\"This is a proof\"" />}}
{{< example context="success" text="\"Cálculo\"" />}}
{{< example context="success" text="\"\"" />}}
{{< example context="success" text="\"35\"" />}}
{{< example context="danger" text="23" />}}


## Enumerated values

{{< alert context="info" text="This property is optional." />}}

The value of the string can be restricted by a fixed set of strings. This set is defined as the value of the parameter **`enum`**.

{{< alert context="warning" text="It must be an array with at least one element, where each element is unique." />}}

A possible example of a scheme using this property would be as follows:

```json
{ 
  "type": "string",
  "enum": ["flower", "stone", "tree"]
}
```

Validation results with this scheme are shown below:

{{< example context="danger" text="\"lion\"" />}}
{{< example context="success" text="\"flower\"" />}}
{{< example context="success" text="\"tree\"" />}}


## Length

{{< alert context="info" text="This property is optional." />}}

The length of the string can be defined by a range of values. The range consists of a minimum, **`minLength`**; and a maximum, **`maxLength`**.

Both variables are not dependent, the maximum can be specified without specifying the minimum and vice versa.

Both the maximum and the minimum are specified by a **non-negative** number.

A possible example of a scheme using this property would be as follows:

```json
{ 
  "type": "string",
  "minLength": 2,
  "maxLength": 5
}
```

Validation results with this scheme are shown below:

{{< example context="danger" text="\"A\"" />}}
{{< example context="success" text="\"AB\"" />}}
{{< example context="success" text="\"ABCDE\"" />}}
{{< example context="danger" text="\"ABCDEF\"" />}}

## Regular Expressions

{{< alert context="info" text="This property is optional." />}}

The pattern of a string, **`pattern`**, is defined by a string that is interpreted as a regular expression. This regular expression filters out which strings are accepted by the scheme. 

{{< alert context="warning" text="The regular expressions used follow the logic of the <a href=\"https://cplusplus.com/reference/regex/\">regular expressions used in C++</a>." />}}

A possible example of a scheme, where a pattern is used to validate strings starting with the word "task" and ending with a number that can range from 1 to 5, is the following:

```json
{ 
  "type": "string",
  "pattern": "^task[1-5]$"
}
```
Validation results with this scheme are shown below:

{{< example context="danger" text="\"task0\"" />}}
{{< example context="success" text="\"task1\"" />}}
{{< example context="success" text="\"task5\"" />}}
{{< example context="danger" text="\"_task3\"" />}}
