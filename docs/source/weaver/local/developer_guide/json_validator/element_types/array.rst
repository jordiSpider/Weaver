
Array Type
==========
.. _array-type:

The '**array**' type represents an ordered list of items.


Specific Fields
"""""""""""""""

The following specific fields to the '**array**' type are related to the definition of the array elements:

*****
items
*****

    .. warning::

        The '**items**' field is required. The '**items**' must be a schema.

    Specifies the schema that the items in the array must adhere to.

The following specific fields to the '**array**' type are related to constraints on the array:

********
minItems
********

    .. note::

        The '**minItems**' field is optional. Its default value is **0**.

    .. warning::

        The '**minItems**' must be a positive integer.

    Specifies the minimum number of items in the array.

********
maxItems
********

    .. note::

        The '**maxItems**' field is optional. Its default value is **18446744073709551615**.

    .. warning::

        The '**maxItems**' must be a positive integer.

    Specifies the maximum number of items in the array.

**********
itemsOrder
**********

    .. note::

        The '**itemsOrder**' field is optional.

    .. warning::

        The '**itemsOrder**' must be a string.

    Specifies the order of the elements in the array. The possible values are:

        - **increasing**: Items must be in increasing order.
        - **strictly_increasing**: Items must be in strictly increasing order.
        - **decreasing**: Items must be in decreasing order.
        - **strictly_decreasing**: Items must be in strictly decreasing order.


These constraints ensure that the array adheres to specified rules, such as being within a certain size range or sorted in a 
certain specified order.

For more details on the common fields, see the section ':ref:`Common Fields <common-fields>`'.


Example
"""""""

.. code-block:: json

    {
        "type": "array",
        "items": {
            "type": "number"
        },
        "minItems": 1,
        "maxItems": 5,
        "itemsOrder": "increasing",
        "nullProperty": false,
        "description": "List of scores in increasing order"
    }
