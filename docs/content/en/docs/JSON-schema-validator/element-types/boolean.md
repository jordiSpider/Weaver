---
title: "Boolean"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    JSON-schema-validator:
      parent: "element-types"
weight: 2220
toc: true
---

The **`boolean`** type is used to represent truth values. This type matches only two special values: **`true`** and **`false`**.

```json
{ "type": "boolean" }
```

Validation results with this scheme are shown below:

{{< example context="success" text="true" />}}
{{< example context="success" text="false" />}}
{{< example context="danger" text="\"true\"" />}}
{{< example context="danger" text="0" />}}
