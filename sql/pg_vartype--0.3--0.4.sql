
CREATE FUNCTION vartype_to_text(x vartype)
  RETURNS text AS
$$
BEGIN
  IF vartype_type_id(x) = 3 THEN
    RETURN left(right(vartype_out(x)::text, -1), -1);
  ELSE
    RETURN vartype_out(x)::text;
  END IF;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_varchar(x vartype)
  RETURNS varchar AS
$$
BEGIN
  IF vartype_type_id(x) = 3 THEN
    RETURN left(right(vartype_out(x)::varchar, -1), -1);
  ELSE
    RETURN vartype_out(x)::varchar;
  END IF;
END
$$
LANGUAGE 'plpgsql';


CREATE FUNCTION vartype_to_char(x vartype)
  RETURNS char AS
$$
BEGIN
  IF vartype_type_id(x) = 3 THEN
    RETURN left(right(vartype_out(x)::char, -1), -1);
  ELSE
    RETURN vartype_out(x)::char;
  END IF;
END
$$
LANGUAGE 'plpgsql';


CREATE CAST (vartype AS text)
  WITH FUNCTION vartype_to_text(vartype)
  AS ASSIGNMENT;


CREATE CAST (vartype AS varchar)
  WITH FUNCTION vartype_to_varchar(vartype)
  AS ASSIGNMENT;


CREATE CAST (vartype AS char)
  WITH FUNCTION vartype_to_char(vartype)
  AS ASSIGNMENT;
