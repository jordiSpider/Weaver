---
title: "What is an JSON Schema?"
date: 2022-01-25T14:40:56+01:00
lastmod: 2022-01-25T14:40:56+01:00
draft: false
images: []
menu:
  docs:
    parent: "JSON-schema-validator"
weight: 2100
toc: true
---

**JSON** (an acronym for "JavaScript Object Notation") is a **lightweight**, **easy-to-read** data interchange format used to transmit information between different systems, especially over the web.

In simple terms, JSON is a text format used to represent structured data such as objects, arrays or simple values such as strings or numbers. Data is organised into **key-value pairs**, where each key is a name that identifies a specific value. For example, in a JSON object describing a person, the key might be "name" and the value might be "John".

The syntax of JSON is very similar to that of JavaScript objects and arrays, making it easy to read and write for programmers familiar with that language. However, JSON can be used with any programming language, making it very versatile.

An example of personal information in JSON format is as follows:

```json
{
  "first name": "John",
  "last name": "Smith",
  "age": 30,
  "email": "john.smith@gmail.com",
  "address": {
    "street": "Abbey Road",
    "number": "123",
    "city": "London",
    "postal code": "06140",
    "country": "England"
  }
}
```

A **JSON schema** is a structure that is used to organise information in JSON, as a kind of **template** that describes how the data to be contained in the JSON should be structured.

A possible example of a schema is the schema defining the JSON structure shown above:

```json
{
  "type": "object",
  "properties": {
    "first name": {
      "type": "string",
      "required": true
    },
    "last name": {
      "type": "string",
      "required": true
    },
    "age": {
      "type": "unsigned_integer",
      "required": true
    },
    "email": {
      "type": "string",
      "required": true
    },
    "address": {
      "type": "object",
      "properties": {
        "street": {
          "type": "string",
          "required": true
        },
        "number": {
          "type": "string",
          "required": true
        },
        "city": {
          "type": "string",
          "required": true
        },
        "postal code": {
          "type": "string",
          "minLength": 5,
          "maxLength": 5,
          "required": true
        },
        "country": {
          "type": "string"
        }
      },
      "additionalProperties": false,
      "required": true
    }
  },
  "additionalProperties": false
}
```

For more information on the data types that can be used to define the structure of the **JSON schema**, go to the [Element types](../element-types) section.
