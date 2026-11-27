
Number Type
===========

The '**number**' type represents numeric data, including integers and floating-point numbers.


Specific Fields
"""""""""""""""

All specific fields for the '**integer**' type are related to constraints on the integer values:

*******
minimum
*******
.. _number-minimum:

    .. note::

        The '**minimum**' field is optional. Its default value is **-1.79769e+308**.

    .. warning::

        The '**minimum**' must be a floating-point number.

    Specifies the minimum value for the number.

****************
exclusiveMinimum
****************

    .. note::

        The '**exclusiveMinimum**' field is optional. Its default value is **false**.
    
    .. warning::

        The '**exclusiveMinimum**' must be a boolean.

    If **true**, the number value must be strictly greater than the ':ref:`minimum <number-minimum>`' value. If **false**, the value must be greater than or equal to the ':ref:`minimum <number-minimum>`' value.

*******
maximum
*******
.. _number-maximum:

    .. note::

        The '**maximum**' field is optional. Its default value is **1.79769e+308**.

    .. warning::

        The '**maximum**' must be a floating-point number.

    Specifies the maximum value for the number.

****************
exclusiveMaximum
****************

    .. note::

        The '**exclusiveMaximum**' field is optional. Its default value is **false**.
    
    .. warning::

        The '**exclusiveMaximum**' must be a boolean.

    If **true**, the number value must be strictly less than the ':ref:`maximum <number-maximum>`' value. If **false**, the value must be less than or equal to the ':ref:`maximum <number-maximum>`' value.


These constraints ensure that the number value adheres to specified rules, such as being within a certain range.

For more details on the common fields, see the section ':ref:`Common Fields <common-fields>`'.


Example
"""""""

.. code-block:: json

    {
       "type": "number",
       "minimum": 0.0,
       "maximum": 100.0,
       "exclusiveMinimum": false,
       "exclusiveMaximum": true,
       "nullProperty": false,
       "description": "The percentage score of the test"
    }
