
Tuple Type
==========

The '**tuple**' type is a variant of the :ref:`Array Type <array-type>` where each position in the 
array can have a different schema. This allows for arrays with fixed-length and heterogeneously-typed items.


Specific Fields
"""""""""""""""

All specific fields for the '**tuple**' type are related to the definition of the array elements:

*****
items
*****

    .. warning::

        The '**items**' field is required. The '**items**' must be a list of schemas.

    Specifies a list of schemas, each corresponding to a specific position in the array.


For more details on the common fields, see the section ':ref:`Common Fields <common-fields>`'.


Example
"""""""

.. code-block:: json

    {
       "type": "tuple",
       "items": [
           { "type": "string" },
           { "type": "number" },
           { "type": "boolean" }
       ],
       "nullProperty": false,
       "description": "A tuple representing a string, a number, and a boolean"
    }
