
Integer Type
============

The '**integer**' type represents whole numbers without a fractional component.


Specific Fields
"""""""""""""""

All specific fields for the '**integer**' type are related to constraints on the integer values:

*******
minimum
*******
.. _integer-minimum:

    .. note::

        The '**minimum**' field is optional. Its default value is **-2147483648**.

    .. warning::

        The '**minimum**' must be an integer.

    Specifies the minimum value for the integer.

****************
exclusiveMinimum
****************

    .. note::

        The '**exclusiveMinimum**' field is optional. Its default value is **false**.
    
    .. warning::

        The '**exclusiveMinimum**' must be a boolean.

    If **true**, the integer value must be strictly greater than the ':ref:`minimum <integer-minimum>`' value. If **false**, the value must be greater than or equal to the ':ref:`minimum <integer-minimum>`' value.

*******
maximum
*******
.. _integer-maximum:

    .. note::

        The '**maximum**' field is optional. Its default value is **2147483647**.

    .. warning::

        The '**maximum**' must be an integer.

    Specifies the maximum value for the integer.

****************
exclusiveMaximum
****************

    .. note::

        The '**exclusiveMaximum**' field is optional. Its default value is **false**.
    
    .. warning::

        The '**exclusiveMaximum**' must be a boolean.

    If **true**, the integer value must be strictly less than the ':ref:`maximum <integer-maximum>`' value. If **false**, the value must be less than or equal to the ':ref:`maximum <integer-maximum>`' value.

**********
multipleOf
**********

    .. note::

        The '**multipleOf**' field is optional.
    
    .. warning::

        The '**multipleOf**' must be an integer.

    The value of the integer must be a multiple of this number.


These constraints ensure that the integer value adheres to specified rules, such as being within a certain range or being a 
multiple of a specified number.

For more details on the common fields, see the section ':ref:`Common Fields <common-fields>`'.


Example
"""""""

.. code-block:: json

    {
       "type": "integer",
       "minimum": 0,
       "maximum": 100,
       "exclusiveMinimum": false,
       "exclusiveMaximum": true,
       "multipleOf": 5,
       "nullProperty": false,
       "description": "The age of the user"
    }
