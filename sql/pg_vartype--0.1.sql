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

CREATE FUNCTION vartype_type(vartype) RETURNS int
  AS 'pg_vartype', 'vartype_type'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_typename(vartype) RETURNS cstring
  AS 'pg_vartype', 'vartype_typename'
  LANGUAGE C IMMUTABLE STRICT;

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

-- now we can make the operator class
CREATE OPERATOR CLASS vartype_ops
    DEFAULT FOR TYPE vartype USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       vartype_cmp(vartype, vartype);


CREATE FUNCTION vartype_to_bigint(vartype) RETURNS bigint
  AS 'pg_vartype', 'vartype_to_longlong'
  LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION vartype_to_string(x int) RETURNS int AS
$$
  BEGIN
    RETURN 888;
  END;
$$
LANGUAGE plpgsql;


CREATE CAST (vartype AS bigint) WITH FUNCTION vartype_to_bigint;
