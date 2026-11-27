
Advanced Features
=================

This section covers the advanced features of the JSON Schema validator. These features provide enhanced capabilities and 
customization options for schema validation.


Operators of JSON Schema
------------------------
.. _operators-of-json-schema:

In JSON Schema, operators are used to define constraints and conditions. This section details the various operators available, 
their usage, and examples. The result of any operator will be a value of a basic type, such as an integer, boolean, string, etc.

Each operator is defined by a dictionary (or object) with several fields describing the type of the operator and the properties 
of each operator.

The type of the operator is defined through the field '**operatorType**', the content of which is a string. The available types are 
shown below:

    - :ref:`Equality and Comparison Operators <equality-and-comparison-operators>`

        .. code-block:: json

            {
                "operatorType": "comparison"
                // Remaining fields
            }
    
    - :ref:`Logical Operators <logical-operators>`

        .. code-block:: json

            {
                "operatorType": "logical"
                // Remaining fields
            }
    
    - :ref:`Property Operators <property-operators>`

        .. code-block:: json

            {
                "operatorType": "property"
                // Remaining fields
            }

    - :ref:`Constant Operators <constant-operators>`

        .. code-block:: json

            {
                "operatorType": "constant"
                // Remaining fields
            }
    
    - :ref:`Arithmetical Operators <arithmetical-operators>`

        .. code-block:: json

            {
                "operatorType": "arithmetical"
                // Remaining fields
            }


Equality and Comparison Operators
"""""""""""""""""""""""""""""""""
.. _equality-and-comparison-operators:

These operators are used to compare values and define constraints based on those comparisons. Each comparison involves two 
parts: the left-hand side (`lhs`) and the right-hand side (`rhs`). On both sides another operator must be defined. 

The type of the comparison operator is defined through the field '**comparisonType**', the content of which is a string. The 
comparison types are defined below:

**********
Equal (==)
**********

    Checks if the '**lhs**' is **equal to** the '**rhs**'. The value of the '**comparisonType**' field is **"equal"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "comparison",
            "comparisonType": "equal",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

**************
Not Equal (!=)
**************

    Checks if the '**lhs**' is **not equal to** the '**rhs**'. The value of the '**comparisonType**' field is **"not_equal"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "comparison",
            "comparisonType": "not_equal",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

*************
Less Than (<)
*************

    Checks if the '**lhs**' is **less than** the '**rhs**'. The value of the '**comparisonType**' field is **"less_than"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "comparison",
            "comparisonType": "less_than",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

***********************
Less Than or Equal (<=)
***********************

    Checks if the '**lhs**' is **less than or equal to** the '**rhs**'. The value of the '**comparisonType**' field is **"less_equal_than"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "comparison",
            "comparisonType": "less_equal_than",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

****************
Greater Than (>)
****************

    Checks if the '**lhs**' is **greater than** the '**rhs**'. The value of the '**comparisonType**' field is **"greater_than"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "comparison",
            "comparisonType": "greater_than",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

**************************
Greater Than or Equal (>=)
**************************

    Checks if the '**lhs**' is **greater than or equal to** the '**rhs**'. The value of the '**comparisonType**' field is **"greater_equal_than"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "comparison",
            "comparisonType": "greater_equal_than",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }


Logical Operators
"""""""""""""""""
.. _logical-operators:

These operators are used to combine or negate conditions. Each logical operator involves a list of operands, with each operand 
defining another operator.

The type of the logical operator is defined through the field '**logicalType**', the content of which is a string. The 
logical types are defined below:

**************
Negation (not)
**************

    .. warning::

        Negation needs only one operand.

    Negates a operand. The value of the '**logicalType**' field is **"negation"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "logical",
            "logicalType": "negation",
            "operands": [
                {
                    // Define another operator
                }
            ]
        }

*****************
Conjunction (and)
*****************

    Checks if all operands are true. The value of the '**logicalType**' field is **"conjunction"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "logical",
            "logicalType": "conjunction",
            "operands": [
                {
                    // Define another operator
                },
                {
                    // Define another operator
                },
                {
                    // Define another operator
                }
            ]
        }

****************
Disjunction (or)
****************

    Checks if at least one operand is true. The value of the '**logicalType**' field is **"disjunction"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "logical",
            "logicalType": "disjunction",
            "operands": [
                {
                    // Define another operator
                },
                {
                    // Define another operator
                },
                {
                    // Define another operator
                }
            ]
        }


Property Operators
""""""""""""""""""
.. _property-operators:

These operators are used to extract the characteristics of schema properties; such as the value associated with the property in 
the configuration and checking for the possibility that the property is null, or in the case of an array property, the size of 
the array or the value of an array element.

All property operators have a field called '**relativePath**', where the path to the desired property is defined by a list of 
strings.

The property element to be extracted is defined through the field '**propertyElement**', whose content is a string. The property 
elements are defined below:

*********************
Value of the Property
*********************

    Extracts the value associated with the property in the configuration. The value of the '**propertyElement**' field is **"value"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "property",
            "propertyElement": "value",
            "relativePath": [
                "path", 
                "to",
                "property"
            ]
        }

********************
Item of the Property
********************

    Extracts the value of the desired item of the property in the configuration. The value of the '**propertyElement**' field is **"item"**.

    The selection of the item is defined through the '**selectedItem**' field, which can be defined as a positive integer or as an operator.

    Example:

    .. code-block:: json

        {
            "operatorType": "property",
            "propertyElement": "item",
            "relativePath": [
                "path", 
                "to",
                "property"
            ],
            "selectedItem": 2
        }

    .. code-block:: json

        {
            "operatorType": "property",
            "propertyElement": "item",
            "relativePath": [
                "path", 
                "to",
                "property"
            ],
            "selectedItem": {
                // Define another operator
            }
        }

********************
Size of the Property
********************

    Extracts the size of the property defined in the configuration. The value of the '**propertyElement**' field is **"size"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "property",
            "propertyElement": "size",
            "relativePath": [
                "path", 
                "to",
                "property"
            ]
        }

***************************
Null Status of the Property
***************************

    .. note::

        Always returns a boolean value

    Extracts the possibility of being or not being null of the property. The value of the '**propertyElement**' field is **"nullProperty"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "property",
            "propertyElement": "nullProperty",
            "relativePath": [
                "path", 
                "to",
                "property"
            ]
        }


Constant Operators
""""""""""""""""""
.. _constant-operators:

These operators are used to return constant values independent of the configuration used. The constant value is defined through 
the '**value**' field, which takes values of a basic type such as an integer, a string, a boolean or a floating point number.

Example:

.. code-block:: json

    {
        "operatorType": "constant",
        "value": 6
    }

.. code-block:: json

    {
        "operatorType": "constant",
        "value": "string"
    }

.. code-block:: json

    {
        "operatorType": "constant",
        "value": false
    }

.. code-block:: json

    {
        "operatorType": "constant",
        "value": 5.7
    }


Arithmetical Operators
""""""""""""""""""""""
.. _arithmetical-operators:

In JSON Schema, arithmetical operators are used to perform basic arithmetic operations and define numeric constraints. Each 
arithmetic operation consists of two parts: the left-hand side (`lhs`) and the right-hand side (`rhs`). On both sides another 
operator must be defined. 

The type of the arithmetic operator is defined by the field '**arithmeticalType**', whose content is a string. The arithmetic 
operator types are defined below:

************
Subtract (-)
************

    The subtract operator is used to subtract one number from another. The value of the '**arithmeticalType**' field is **"subtract"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "arithmetical",
            "arithmeticalType": "subtract",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

************
Addition (+)
************

    The addition operator is used to sum two numbers. The value of the '**arithmeticalType**' field is **"addition"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "arithmetical",
            "arithmeticalType": "addition",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

************
Multiply (*)
************

    The multiply operator is used to multiply two numbers. The value of the '**arithmeticalType**' field is **"multiply"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "arithmetical",
            "arithmeticalType": "multiply",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

************
Division (/)
************

    The division operator is used to divide one number by another. The value of the '**arithmeticalType**' field is **"division"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "arithmetical",
            "arithmeticalType": "division",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }

**********
Module (%)
**********

    The module operator is used to find the remainder of the division of one number by another. The value of the '**arithmeticalType**' field is **"module"**.

    Example:

    .. code-block:: json

        {
            "operatorType": "arithmetical",
            "arithmeticalType": "module",
            "lhs": {
                // Define another operator
            },
            "rhs": {
                // Define another operator
            }
        }


Conditionals of JSON Schema
---------------------------
.. _conditionals-of-json-schema:

Conditionals in JSON schemas function as schema modifiers based on the values of the configuration used in the validation. Each 
conditional is defined by a dictionary (or object). These conditionals, like a conditional in programming, have three distinct 
parts, which are defined in three fields:

Condition
"""""""""

    The '**condition**' field is in charge of deciding which block of the conditional will modify the schema, 
    the :ref:`Then Block <then-block>` or the :ref:`Else Block <else-block>`.

    This field is defined by the use of operators. The result of the operator resolution shall be evaluated as a condition. For more 
    information on operators, see the :ref:`Operators of JSON Schema <operators-of-json-schema>` section.

Then Block
""""""""""
.. _then-block:

    The block defined in the '**then**' field shall be applied in case the conditional condition is true. 

    This block is defined by a dictionary (or object), which has three fields to be defined, as follows:

    - **workingPath**:

        .. note::

            The '**workingPath**' field is optional. Its default value is the height within the scheme in which the conditional is defined.

        It is defined as a list of strings, which indicates the relative path to the current position within the schema where the modification is to be applied. 
        
        This field is necessary because it may be the case that the zone where the modification is to be applied is at a different height from the zone where the values for the resolution of the condition are obtained.

    - **add**:

        The '**add**' field is defined as a dictionary (or object), in which different fields are defined depending on the additions to be made to the schema. The available modification fields are the following:

        - **nullProperty**:

            The purpose of this field is to activate the possibility of a schema element to have a null value in the configuration. The way to define this field is through a list of strings indicating the names of the affected properties.

    - **remove**:

        The '**remove**' field is defined as a dictionary (or object), in which different fields are defined depending on the deletions to be made in the schema. The following modification fields are available:

        - **nullProperty**:

            The purpose of this field is to disable the possibility that a schema element can take a null value in the configuration. The way to define this field is through a list of strings indicating the names of the affected properties.


Else Block
""""""""""
.. _else-block:

    The block defined in the "**else**" field shall be applied in case the conditional condition is false.

    The structure of this block is exactly the same as in the :ref:`Then Block <then-block>` section.


In the case of having defined a pattern-based conditional, the above structure is maintained in its entirety except for a single 
addition, the possibility of using the value that matches the pattern.

This value can be used within the path defined by the '**workingPath**' field as well as in the schema modification fields, such 
as '**nullProperty**'. To use this value, the string **"<pattern>"** must be placed, which will be replaced by the value matching 
the pattern automatically within the validator.


Conditions of JSON Schema
-------------------------
.. _conditions-of-json-schema:

Conditions in JSON schemas are used to define certain restrictions that warn the user with errors or warnings of possible 
malfunctioning of the validation due to the configuration values used.

Al igual que sucede con los condicionales, estas condiciones se definen como un diccionario (o objeto) en el cuál existe un campo '**condition**', cuya estructura es exactamente igual a la del campo 'condition' de los condicionales, la cual se puede ver en la sección Condition; y un campo donde se define la advertencia. Esta campo de advertencia se define como una cadena, que representa el mensaje a mostrar al usuario. Si la advertencia es de error se define el campo '**error**', mientras que si la advertencia es de aviso se define el campo '**warning**'.
