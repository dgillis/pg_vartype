# pg_vartype

PostgreSQL extension to allow multiple scalar types to be stored within a single column.

**NOTE**: This extension was created prior to JSONB being added to PostgreSQL. Many of the
use cases for VARTYPE work just as well using JSONB (with DATES/TIMESTAMPS cast to strings).

## Installation

Clone this repo and, from the base directory, run the following commands:

```sh
make
make install
make clean
```

The `VARTYPE` extension should now be added to your library of PostgreSQL extensions.
To add the extension to a particular database, run the following PostgreSQL command:

```sql
CREATE EXTENSION pg_vartype;
```

The VARTYPE datatype should now be defined.

```sql
SELECT pg_typeof(NULL::VARTYPE);  -- vartype
```

## Example

```sql

-- Test table utilizing a vartype column.
CREATE TABLE vartype_test (id SERIAL PRIMARY KEY, x VARTYPE);

-- Insert values of varying type:
INSERT INTO vartype_test (x) VALUES
  (1), (2), (3),
  (0.1), (10.5), (-0.5),
  (TRUE), (FALSE),
  ('2015-06-01'::TIMESTAMP), ('2016-05-30'::TIMESTAMP),
  ('2015-06-01'::DATE), ('2016-05-30'::DATE),
  ('abc'::TEXT), ('xyz'::TEXT),
  (NULL);

-- Select all x-values from the table in ascending order.
SELECT x FROM vartype_test ORDER by x;
-- Results:
--             x
------------------------------
-- -0.500000
-- 0.100000
-- 1.500000
-- 1
-- 2
-- 3
-- 10.500000
-- false
-- false
-- true
-- "abc"
-- "xyz"
-- "xyz"
-- 2015-06-01
-- 2015-06-01 00:00:00
-- 2016-05-30
-- 2016-05-30 00:00:00
-- 2017-10-16 14:09:39.688657

-- Select only those x-values with numeric types:
SELECT x FROM vartype_test WHERE vartype_type(x) IN ('int', 'float');
-- Results:
--     x
-------------
-- 1.500000
-- 1
-- 2
-- 3
-- 0.100000
-- 10.500000
-- -0.500000

```

## String Input Format
All textual types (TEXT, VARCHAR, etc.) cast to the same string in VARTYPE.
However, when dealing with non-typed input, string values are potentially
ambigious (e.g., the non-typed input `'false'` could refer to either the string
`'false'` or boolean value, which has the same non-typed representation).

Therefore, any untyped input intended to represent a string should be surrounded
by double quotes:

```sql
SELECT vartype_type('false'::VARTYPE);  // Interpreted as a bool.
SELECT vartype_type('"false"'::VARTYPE);  Interpreted as a string.
SELECT vartype_type('abc'::VARTYPE);  // Raises an invalid input exception.
SELECT vartype_type('"abc"'::VARTYPE); // Correctly identified as string.
SELECT vartype_type('abc'::TEXT::VARTYPE); // Also correctly identified as a string.
```
