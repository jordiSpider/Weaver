
Getting Started
===============


What is JSON Schema?
--------------------

A JSON Schema is a declarative format for specifying the structure, content, and semantics of JSON data. It is written in 
JSON itself and provides a comprehensive vocabulary for describing data structures, including:

* The types of values (e.g., strings, numbers, objects, arrays)
* The properties that an object can have
* Constraints on data values (e.g., minimum and maximum values, string lengths)
* Data relationships and dependencies


Elements of JSON Schema
-----------------------

In JSON Schema, each element is defined using a dictionary (or object) with various fields that describe the component's 
properties, constraints, and types.


Common Fields
"""""""""""""
.. _common-fields:

These fields can be present in all types of JSON Schema elements:

****
type
****

    .. warning::

        The '**type**' field is required.

    Determines the nature of the data. Here are all possible types:

    - **Array Type**:

        Represents an ordered list of items.

        .. code-block:: json

            {
                "type": "array"
                // Remaining fields
            }
    
    - **Boolean Type**:

        Represents a true or false value.

        .. code-block:: json

            {
                "type": "boolean"
                // Remaining fields
            }
    
    - **Integer Type**:

        Represents whole numbers without a fractional component.

        .. code-block:: json

            {
                "type": "integer"
                // Remaining fields
            }
    
    - **Number Type**:

        Represents numeric data, including integers and floating-point numbers.

        .. code-block:: json

            {
                "type": "number"
                // Remaining fields
            }
    
    - **Object Type**:

        Represents a collection of key-value pairs.

        .. code-block:: json

            {
                "type": "object"
                // Remaining fields
            }
    
    - **String Type**:

        Represents textual data.

        .. code-block:: json

            {
                "type": "string"
                // Remaining fields
            }
    
    - **Tuple Type**:

        Represent a variant of the '**array**' type where each position in the array can have a different schema. This allows for arrays with fixed-length and heterogeneously-typed items.

        .. code-block:: json

            {
                "type": "tuple"
                // Remaining fields
            }

***********
description
***********

    .. note::

        The '**description**' field is optional, but can be very useful for documentation and understanding the schema.

    Provides a brief description of what the element represents within the schema.

    .. code-block:: json

        {
            "type": "string",
            "description": "The user's name"
            // Remaining fields
        }

************
nullProperty
************

    .. note::

        The '**nullProperty**' field is optional. Its default value is **false**.

    .. warning::

        Ensure that '**nullProperty**' is correctly set based on whether the element is allowed to have null values. Incorrect 
        settings can lead to unexpected validation behavior.

    Indicates whether an element can have a null value. If **nullProperty** is set 
    to **true**, the element can be null and, in that case, the element will not be validated and will be considered correct 
    automatically. If set to **false**, the validator will give an error if the element has a null value.

    .. code-block:: json

        {
            "type": "string",
            "nullProperty": true
            // Remaining fields
        }


Specific Fields for Each Type
"""""""""""""""""""""""""""""

Each element type can have additional specific fields that define its properties and constraints. These fields are documented in 
detail in the sections for each type:

    .. toctree::
       :maxdepth: 1

       element_types/array
       element_types/boolean
       element_types/integer
       element_types/number
       element_types/object
       element_types/string
       element_types/tuple


Benefits of Using JSON Schema
-----------------------------

- **Validation**: JSON Schema ensures that JSON data adheres to a specified structure and set of rules, making data validation straightforward and consistent.

- **Documentation**: It serves as a form of documentation, clearly defining what data is expected and how it should be structured.

- **Interoperability**: JSON Schema promotes interoperability between systems by providing a standard way to describe data formats.

- **Automation**: Tools can automatically generate forms, APIs, and other components based on JSON Schemas, reducing development time and errors.

- **Consistency**: It helps maintain data consistency across different parts of an application or between different systems.


Example of a Complete JSON Schema
---------------------------------

Here's a more comprehensive example of a JSON Schema for a user profile:

.. code-block:: json

    {
        "type": "object",
        "properties": {
            "id": {
                "type": "string",
                "description": "The unique identifier for a user"
            },
            "name": {
                "type": "string",
                "description": "The user's name"
            },
            "age": {
                "type": "integer",
                "description": "The user's age",
                "minimum": 0
            },
            "email": {
                "type": "string",
                "description": "The user's email address"
            },
            "isActive": {
                "type": "boolean",
                "description": "Whether the user is active"
            },
            "roles": {
                "type": "array",
                "items": {
                    "type": "string"
                },
                "description": "Roles assigned to the user"
            }
        }
    }
