
String Type
===========

The '**string**' type represents textual data.


Specific Fields
"""""""""""""""

All specific fields for the '**string**' type are related to constraints on the string values:

*********
minLength
*********

    .. note::

        The '**minLength**' field is optional. Its default value is **0**.

    .. warning::

        The '**minLength**' must be a positive integer.

    Specifies the minimum length of the string.

*********
maxLength
*********

    .. note::

        The '**maxLength**' field is optional. Its default value is **18446744073709551615**.

    .. warning::

        The '**maxLength**' must be a positive integer.

    Specifies the maximum length of the string.

*******
pattern
*******

    .. note::

        The '**pattern**' field is optional.

    .. warning::

        The '**pattern**' must be a string.

    Specifies a regular expression that the string must match.

    For more information on regular expressions, please visit the `C++ regular expressions documentation <https://learn.microsoft.com/en-us/cpp/standard-library/regular-expressions-cpp?view=msvc-170>`_.

****
enum
****

    .. note::

        The '**enum**' field is optional.

    .. warning::

        The '**enum**' must be a string vector.

    Specifies a vector of values that the string can take. The string must exactly match one of the strings in the vector.


These constraints ensure that the string value adheres to specified rules, such as being within a certain length range, matching 
a specific pattern, or being one of a specified set of values.

For more details on the common fields, see the section ':ref:`Common Fields <common-fields>`'.


Example
"""""""

.. code-block:: json

    {
       "type": "string",
       "minLength": 1,
       "maxLength": 255,
       "pattern": "^[a-zA-Z0-9_]*$",
       "enum": ["admin", "user", "guest"],
       "nullProperty": false,
       "description": "The role of the user"
    }
