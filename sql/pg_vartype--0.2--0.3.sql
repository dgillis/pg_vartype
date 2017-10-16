
CREATE TYPE vartype_type AS ENUM (
  'int',
  'float',
  'bool',
  'string',
  'date',
  'timestamp'
);


ALTER FUNCTION vartype_type(vartype) RENAME TO vartype_type_id;

ALTER FUNCTION vartype_typename(vartype) RENAME TO vartype_cname;

CREATE FUNCTION vartype_type(x vartype) RETURNS vartype_type AS
$$
  BEGIN
    RETURN (enum_range(null::vartype_type))[vartype_type_id(x) + 1];
  END;
$$ LANGUAGE 'plpgsql';


