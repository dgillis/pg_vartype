set search_path = @extschema@;


create type vartype;

create function vartype_in(cstring)
  returns vartype as 'pg_vartype', 'vartype_in'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  language c immutable strict;

create function vartype_out(vartype)
  returns cstring as 'pg_vartype', 'vartype_out'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  language c immutable strict;
/*
create function vartype_recv(internal)
  returns vartype as '/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'
  language c immutable strict;

create function vartype_send(vartype)
  returns bytea as '/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'
  language c immutable strict;
*/


create type vartype (
  input = vartype_in,
  output = vartype_out,
  /*receive = vartype_recv,
  send = vartype_send,*/
  internallength = variable,
  alignment = double
);















CREATE FUNCTION vartype_lt(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_lt'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_le(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_le'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT;
  
CREATE FUNCTION vartype_eq(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_eq'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_ne(vartype, vartype) RETURNS bool
   AS 'pg_vartype', 'vartype_ne'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT
;
CREATE FUNCTION vartype_ge(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_ge'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_gt(vartype, vartype) RETURNS bool
  AS 'pg_vartype', 'vartype_gt'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
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
  AS 'pg_vartype', 'vartype_cmp'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_type(vartype) RETURNS int
  AS 'pg_vartype', 'vartype_type' /*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vartype_typename(vartype) RETURNS cstring
  AS 'pg_vartype', 'vartype_typename'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so' */
  LANGUAGE C IMMUTABLE STRICT;

-- create the support function too
CREATE FUNCTION vartype_len(vartype) RETURNS int4
  AS 'pg_vartype', 'vartype_len'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so' */
  LANGUAGE C IMMUTABLE STRICT;


-- create the support function too
CREATE FUNCTION vartype_get_char(vartype, int) RETURNS int4
  AS 'pg_vartype', 'vartype_get_char'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
  LANGUAGE C IMMUTABLE STRICT;

-- create the support function too
CREATE FUNCTION vartype_output_size(vartype) RETURNS int4
  AS 'pg_vartype', 'vartype_output_size'/*'/home/dmg/dystillr/lib/ddm/ddm/core/sql/libvartype.so'*/
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




