set search_path = @extschema@;


CREATE TYPE vartype;

CREATE FUNCTION vartype_in(cstring)
  RETURNS vartype AS 'pg_vartype', 'vartype_in'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_out(vartype)
  RETURNS CSTRING AS 'pg_vartype', 'vartype_out'
  LANGUAGE C IMMUTABLE STRICT;

/*
CREATE FUNCTION vartype_recv(internal)
  RETURNS VARTYPE AS 'pg_vartype', 'vartype_recv'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_send(vartype)
  RETURNS BYTEA AS 'pg_vartype', 'vartype_send'
  LANGUAGE C IMMUTABLE STRICT;
*/

CREATE TYPE vartype (
  INPUT = vartype_in,
  OUTPUT = vartype_out,
  /*RECEIVE = vartype_recv,
  SEND = vartype_send,*/
  INTERNALLENGTH = variable,
  ALIGNMENT = double
);

CREATE TYPE vartype_type AS ENUM (
  'int',
  'float',
  'bool',
  'string',
  'date',
  'timestamp'
);

CREATE FUNCTION vartype_lt(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_lt'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_le(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_le'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_eq(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_eq'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_ne(vartype, vartype) RETURNS bool
   AS 'pg_vartype', 'vartype_ne'
  LANGUAGE C IMMUTABLE STRICT
;
CREATE FUNCTION vartype_ge(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_ge'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_gt(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_gt'
  LANGUAGE C IMMUTABLE STRICT;



CREATE OPERATOR < (
   leftarg = vartype, rightarg = vartype, procedure = vartype_lt,
   commutator = > , negator = >= ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
   leftarg = vartype, rightarg = vartype, procedure = vartype_le,
   commutator = >= , negator = > ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
   leftarg = vartype, rightarg = vartype, procedure = vartype_eq,
   commutator = = ,
   negator = <>,
   restrict = eqsel, join = eqjoinsel
);
CREATE OPERATOR <> (
   leftarg = vartype, rightarg = vartype, procedure = vartype_ne,
   commutator = <>,
   negator = =,
   restrict = eqsel, join = eqjoinsel
);
CREATE OPERATOR >= (
   leftarg = vartype, rightarg = vartype, procedure = vartype_ge,
   commutator = <= , negator = < ,
   restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR > (
   leftarg = vartype, rightarg = vartype, procedure = vartype_gt,
   commutator = < , negator = <= ,
   restrict = scalargtsel, join = scalargtjoinsel
);



-- create the support function too
CREATE FUNCTION vartype_cmp(vartype, vartype) RETURNS int4
  AS 'pg_vartype', 'vartype_cmp'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_type_id(vartype) RETURNS int
  AS 'pg_vartype', 'vartype_type'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_cname(vartype) RETURNS cstring
  AS 'pg_vartype', 'vartype_typename'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_type(x vartype) RETURNS vartype_type AS
$$
  BEGIN
    RETURN (enum_range(null::vartype_type))[vartype_type_id(x) + 1];
  END;
$$ LANGUAGE 'plpgsql';

-- create the support function too
CREATE FUNCTION vartype_len(vartype) RETURNS int4
  AS 'pg_vartype', 'vartype_len'
  LANGUAGE C IMMUTABLE STRICT;


-- create the support function too
CREATE FUNCTION vartype_get_char(vartype, int) RETURNS int4
  AS 'pg_vartype', 'vartype_get_char'
  LANGUAGE C IMMUTABLE STRICT;

-- create the support function too
CREATE FUNCTION vartype_output_size(vartype) RETURNS int4
  AS 'pg_vartype', 'vartype_output_size'
  LANGUAGE C IMMUTABLE STRICT;


CREATE OPERATOR FAMILY vartype_ops USING btree;

-- now we can make the operator class
CREATE OPERATOR CLASS vartype_ops
DEFAULT FOR TYPE vartype USING btree FAMILY vartype_ops AS
  OPERATOR        1       < ,
  OPERATOR        2       <= ,
  OPERATOR        3       = ,
  OPERATOR        4       >= ,
  OPERATOR        5       > ,
  FUNCTION        1       vartype_cmp(vartype, vartype);


CREATE FUNCTION vartype_int_to_builtin(vartype)
  RETURNS bigint
  AS 'pg_vartype', 'vartype_int_to_builtin'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_float_to_builtin(vartype)
  RETURNS double precision
  AS 'pg_vartype', 'vartype_float_to_builtin'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_bool_to_builtin(vartype)
  RETURNS boolean
  AS 'pg_vartype', 'vartype_bool_to_builtin'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_timestamp_to_builtin(vartype)
  RETURNS timestamptz
  AS 'pg_vartype', 'vartype_timestamp_to_builtin'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_date_to_builtin(vartype)
  RETURNS date
  AS 'pg_vartype', 'vartype_date_to_builtin'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_string_to_builtin(vartype)
  RETURNS text
  AS 'pg_vartype', 'vartype_string_to_builtin'
  LANGUAGE C IMMUTABLE STRICT;



CREATE FUNCTION bigint_to_vartype(bigint)
  RETURNS vartype
  AS 'pg_vartype', 'int64_to_vartype'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION double_to_vartype(double precision)
  RETURNS vartype
  AS 'pg_vartype', 'double_to_vartype'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION bool_to_vartype(boolean)
  RETURNS vartype
  AS 'pg_vartype', 'bool_to_vartype'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION date_to_vartype(date)
  RETURNS vartype
  AS 'pg_vartype', 'date_to_vartype'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION timestamptz_to_vartype(timestamptz)
  RETURNS vartype
  AS 'pg_vartype', 'timestamptz_to_vartype'
  LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION varchar_to_vartype(varchar)
  RETURNS vartype
  AS 'pg_vartype', 'varchar_to_vartype'
  LANGUAGE C IMMUTABLE STRICT;

/*
CREATE FUNCTION text_to_vartype(text)
  RETURNS vartype
  AS 'pg_vartype', 'text_to_vartype'
  LANGUAGE C IMMUTABLE STRICT;
*/



/*******************************************/
/******* vartype to numeric types **********/
/*******************************************/

CREATE FUNCTION vartype_to_bigint(v vartype)
  RETURNS bigint AS
$$
BEGIN
  RETURN vartype_int_to_builtin(v);
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_smallint(v vartype)
  RETURNS smallint AS
$$
BEGIN
  RETURN vartype_to_bigint(v)::smallint;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_int(v vartype)
  RETURNS int AS
$$
BEGIN
  RETURN vartype_to_bigint(v)::int;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_double(v vartype)
  RETURNS double precision AS
$$
BEGIN
  RETURN vartype_float_to_builtin(v)::double precision;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_real(v vartype)
  RETURNS real AS
$$
BEGIN
  RETURN vartype_to_double(v)::real;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_decimal(v vartype)
  RETURNS decimal AS
$$
BEGIN
  RETURN vartype_to_double(v)::decimal;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_numeric(v vartype)
  RETURNS numeric AS
$$
BEGIN
  IF vartype_type(v) = 0 THEN
    RETURN vartype_to_bigint(v)::numeric;
  ELSE
    RETURN vartype_to_double(v)::numeric;
  END IF;
END
$$
LANGUAGE 'plpgsql';




/*******************************************/
/******* numeric types to vartype **********/
/*******************************************/
CREATE FUNCTION int_to_vartype(x int)
  RETURNS vartype AS
$$
BEGIN
  RETURN bigint_to_vartype(x::bigint);
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION smallint_to_vartype(x smallint)
  RETURNS vartype AS
$$
BEGIN
  RETURN bigint_to_vartype(x::bigint);
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION decimal_to_vartype(x decimal)
  RETURNS vartype AS
$$
BEGIN
  RETURN double_to_vartype(x::double precision);
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION real_to_vartype(x real)
  RETURNS vartype AS
$$
BEGIN
  RETURN double_to_vartype(x::double precision);
END
$$
LANGUAGE 'plpgsql';


/*******************************************/
/*** vartype to date/timestamp types *******/
/*******************************************/
CREATE FUNCTION vartype_to_timestamptz(x vartype)
  RETURNS timestamptz AS
$$
BEGIN
  IF vartype_type(x) = 5 THEN
    RETURN vartype_timestamp_to_builtin(x);
  ELSE
    RETURN vartype_date_to_builtin(x)::timestamptz;
  END IF;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_timestamp(x vartype)
  RETURNS timestamp AS
$$
BEGIN
  RETURN vartype_to_timestamptz(x)::timestamp;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_date(x vartype)
  RETURNS date AS
$$
BEGIN
  IF vartype_type(x) = 5 THEN
    RETURN vartype_timestamp_to_builtin(x)::date;
  ELSE
    RETURN vartype_date_to_builtin(x);
  END IF;
END
$$
LANGUAGE 'plpgsql';

/*******************************************/
/*** date/timestamp types to vartype *******/
/*******************************************/
CREATE FUNCTION timestamp_to_vartype(x timestamp)
  RETURNS vartype AS
$$
BEGIN
  RETURN timestamptz_to_vartype(x::timestamptz);
END
$$
LANGUAGE 'plpgsql';


/*******************************************/
/****** bool type to/from vartype **********/
/*******************************************/
CREATE FUNCTION vartype_to_bool(v vartype)
  RETURNS boolean AS
$$
BEGIN
  IF vartype_type(v) = 0 THEN
    RETURN vartype_to_int(v)::int::boolean;
  ELSE
    RETURN vartype_bool_to_builtin(v);
  END IF;
END
$$
LANGUAGE 'plpgsql';


/*******************************************/
/****** text type to/from vartype **********/
/*******************************************/
CREATE FUNCTION text_to_vartype(t text)
  RETURNS vartype AS
$$
BEGIN
  RETURN varchar_to_vartype(t::varchar);
END
$$
LANGUAGE 'plpgsql';

CREATE FUNCTION character_to_vartype(t character)
  RETURNS vartype AS
$$
BEGIN
  RETURN varchar_to_vartype(t::character);
END
$$
LANGUAGE 'plpgsql';





/* casts from vartype to numeric types */
CREATE CAST (vartype AS bigint)
  WITH FUNCTION vartype_to_bigint(vartype)
  AS ASSIGNMENT;

CREATE CAST (vartype AS int)
  WITH FUNCTION vartype_to_int(vartype)
  AS ASSIGNMENT;

CREATE CAST (vartype AS smallint)
  WITH FUNCTION vartype_to_smallint(vartype)
  AS ASSIGNMENT;

CREATE CAST (vartype AS double precision)
  WITH FUNCTION vartype_to_double(vartype)
  AS ASSIGNMENT;

CREATE CAST (vartype AS real)
  WITH FUNCTION vartype_to_real(vartype)
  AS ASSIGNMENT;

CREATE CAST (vartype AS decimal)
  WITH FUNCTION vartype_to_decimal(vartype)
  AS ASSIGNMENT;


/* casts from numeric types to vartypes */
CREATE CAST (bigint AS vartype)
  WITH FUNCTION bigint_to_vartype(bigint)
  AS IMPLICIT;

CREATE CAST (int AS vartype)
  WITH FUNCTION int_to_vartype(int)
  AS IMPLICIT;

CREATE CAST (smallint AS vartype)
  WITH FUNCTION smallint_to_vartype(smallint)
  AS IMPLICIT;

CREATE CAST (double precision AS vartype)
  WITH FUNCTION double_to_vartype(double precision)
  AS IMPLICIT;

CREATE CAST (real AS vartype)
  WITH FUNCTION real_to_vartype(real)
  AS IMPLICIT;

CREATE CAST (decimal AS vartype)
  WITH FUNCTION decimal_to_vartype(decimal)
  AS IMPLICIT;


/* casts from vartype to date/timestamp types */

CREATE CAST (vartype AS timestamptz)
  WITH FUNCTION vartype_to_timestamptz(vartype)
  AS ASSIGNMENT;

CREATE CAST (vartype AS timestamp)
  WITH FUNCTION vartype_to_timestamp(vartype)
  AS ASSIGNMENT;

CREATE CAST (vartype AS date)
  WITH FUNCTION vartype_to_date(vartype)
  AS ASSIGNMENT;


/* casts from date/timestamp to vartype */
CREATE CAST (timestamptz AS vartype)
  WITH FUNCTION timestamptz_to_vartype(timestamptz)
  AS IMPLICIT;

CREATE CAST (timestamp AS vartype)
  WITH FUNCTION timestamp_to_vartype(timestamp)
  AS IMPLICIT;

CREATE CAST (date AS vartype)
  WITH FUNCTION date_to_vartype(date)
  AS IMPLICIT;


/* casts from bool to/from vartype */

CREATE CAST (vartype AS boolean)
  WITH FUNCTION vartype_to_bool(vartype)
  AS ASSIGNMENT;

CREATE CAST (boolean AS vartype)
  WITH FUNCTION bool_to_vartype(boolean)
  AS IMPLICIT;




/* casts string types to vartype */
CREATE CAST (varchar AS vartype)
  WITH FUNCTION varchar_to_vartype(varchar)
  AS IMPLICIT;

CREATE CAST (character AS vartype)
  WITH FUNCTION character_to_vartype(character)
  AS IMPLICIT;

CREATE CAST (text AS vartype)
  WITH FUNCTION text_to_vartype(text)
  AS IMPLICIT;
