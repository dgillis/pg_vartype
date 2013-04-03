/*
  gcc vartype.c -I/usr/include/postgresql/9.1/server/
*/
#include "./pg_vartype.h"


PG_MODULE_MAGIC;


const char *VT_TYPE_NAMES[] = {
  "VT_INT",
  "VT_FLOAT",
  "VT_BOOL",
  "VT_STRING",
  "VT_DATE",
  "VT_TIMESTAMP"
};


const int VARTYPE_PRECEDENCE[] = {
  1, // VT_INT 
  1, // VT_FLOAT
  2, // VT_BOOL
  3, // VT_STRING
  4, // VT_DATE
  4  // VT_TIMESTAMP
};


const int VT_BOOL_OUTPUT_STRLEN = 6;
const int VT_STRING_COLLATION = 100;
const char *VT_STRING_FORMAT_STR = "\"%s\"";
const char *VT_INT_FORMAT_STR = "%lld";
const char *VT_FLOAT_FORMAT_STR = "%lf";


vartype *vartype_alloc(int data_len) {
  int full_len = VARHDRSZ + sizeof(int) + sizeof(int) + data_len;
  vartype *d = palloc(full_len);
  SET_VARSIZE(d, full_len);  
  return d;
}


char *vartype_get_data_len_ptr(const vartype *d) {
  return (char *) VARDATA(d);
}


char *vartype_get_type_ptr(const vartype *d) {
  return (char *) VARDATA(d) + sizeof(int);
}


char *vartype_get_data_ptr(const vartype *d) {
  return (char *) VARDATA(d) + sizeof(int) + sizeof(int);
}


void vartype_set_data_len(vartype *d, int data_len) {
  memcpy(vartype_get_data_len_ptr(d), &data_len, sizeof(int));
}


int vartype_get_data_len(const vartype *d) {
  return *(int *) vartype_get_data_len_ptr(d);
}

void vartype_set_type(vartype *d, int vt_type) {
  memcpy(vartype_get_type_ptr(d), &vt_type, sizeof(int));
}


int vartype_get_type(const vartype *d) {
  return *(int *) vartype_get_type_ptr(d);
}


const char *vartype_get_type_name(const vartype *d) {
  int typeid = vartype_get_type(d);
  return VT_TYPE_NAMES[typeid];
}


int vartype_get_precedence(const vartype *d) {
  int vt_type = vartype_get_type(d);
  return VARTYPE_PRECEDENCE[vt_type];
}


void vartype_extract(const vartype *d, int *vt_type, int *data_len, char **data) {
  if (vt_type != NULL) {
    memcpy(vt_type, (int *) vartype_get_type_ptr(d), sizeof(int));
  }
  if (data_len != NULL) {
    memcpy(data_len, (int *) vartype_get_data_len_ptr(d), sizeof(int));
  }
  if (data != NULL) {
    memcpy(data, (char *) vartype_get_data_ptr(d), sizeof(char *));
  }
}


vartype *vartype_init_int(vt_int x) {
  vartype *d = vartype_alloc(sizeof(vt_int));
  vartype_set_type(d, VT_INT);
  vartype_set_data_len(d, sizeof(vt_int));
  memcpy(vartype_get_data_ptr(d), &x, sizeof(vt_int));
  return d;
}


vartype *vartype_init_float(vt_float x) {
  vartype *d = vartype_alloc(sizeof(vt_float));
  vartype_set_type(d, VT_FLOAT);
  vartype_set_data_len(d, sizeof(vt_float));
  memcpy(vartype_get_data_ptr(d), &x, sizeof(vt_float));
  return d;
}


vartype *vartype_init_bool(vt_bool x) {
  vartype *d = vartype_alloc(sizeof(vt_bool));
  vartype_set_type(d, VT_BOOL);
  vartype_set_data_len(d, sizeof(vt_bool));
  memcpy(vartype_get_data_ptr(d), &x, sizeof(vt_bool));
  return d;
}


vartype *vartype_init_date(vt_date x) {
  vartype *d = vartype_alloc(sizeof(vt_date));
  vartype_set_type(d, VT_DATE);
  vartype_set_data_len(d, sizeof(vt_date));
  memcpy(vartype_get_data_ptr(d), &x, sizeof(vt_date));
  return d;
}


vartype *vartype_init_timestamp(vt_timestamp x) {
  vartype *d = vartype_alloc(sizeof(vt_timestamp));
  vartype_set_type(d, VT_TIMESTAMP);
  vartype_set_data_len(d, sizeof(vt_timestamp));
  memcpy(vartype_get_data_ptr(d), &x, sizeof(vt_timestamp));
  return d;
}


vartype *vartype_init_string(char *s, int len) {
  vartype *d = vartype_alloc(len);
  vartype_set_type(d, VT_STRING);
  vartype_set_data_len(d, len);
  memcpy(vartype_get_data_ptr(d), s, len);
  return d;
}


int vartype_get_pretty_string_len(const vartype *d) {
  int vt_type = vartype_get_type(d);
  int len;
  if (vt_type == VT_INT) {
    char dummy[0];
    len = snprintf(dummy, 0, VT_INT_FORMAT_STR, *(vt_int *) vartype_get_data_ptr(d));
  }
  else if (vt_type == VT_FLOAT) {
    char dummy[0];
    len = snprintf(dummy, 0, VT_INT_FORMAT_STR, *(vt_float *) vartype_get_data_ptr(d));
  }
  else if (vt_type == VT_STRING) {
    len = vartype_get_data_len(d) + 2; // data_len + 2 quotes
  }
  else if (vt_type == VT_DATE) {
    len = MAXDATELEN;
  }
  else if (vt_type == VT_TIMESTAMP) {
    len = MAXDATELEN;
  }
  else if (vt_type == VT_BOOL) {
    return VT_BOOL_OUTPUT_STRLEN;
  }
  else {
    ereport(ERROR,
    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
     errmsg("Invalid vartype type of %d",
            vartype_get_type(d)
            )));
  }
  return len + 1;
}


int vartype_to_pretty_string(const vartype *d, char *buf, int maxlen) {
  int retval;
  char *data, *result;
  int vt_type = vartype_get_type(d);
  if (vt_type == VT_INT) {
    retval = snprintf(buf, maxlen, VT_INT_FORMAT_STR, 
                      *(vt_int *) vartype_get_data_ptr(d));
  }
  else if (vt_type == VT_FLOAT) {
    retval = snprintf(buf, maxlen, VT_FLOAT_FORMAT_STR, 
                      *(vt_float *) vartype_get_data_ptr(d));
  }
  else if (vt_type == VT_STRING) {
    int buf_index, lim;
    lim = min(vartype_get_data_len(d) + 3, maxlen);
    data = vartype_get_data_ptr(d);
    for (buf_index = 0; buf_index < lim; buf_index++) {
      if (buf_index == 0 || buf_index == lim - 2) {
        buf[buf_index] = '"';
      }
      else if (buf_index == lim - 1) {
        buf[buf_index] = '\0';
      }
      else {
        buf[buf_index] = data[buf_index - 1];
      }
    }
    retval = buf_index;
  }
  else if (vt_type == VT_BOOL) {
    vt_bool bool_data = *(vt_bool *) vartype_get_data_ptr(d);

    if (*(vt_bool *) vartype_get_data_ptr(d)) {
      retval = snprintf(buf, maxlen, "%s", "true");
    } else {
      retval = snprintf(buf, maxlen, "%s", "false");
    }
  }
  else if (vt_type == VT_DATE) {
    retval = date_to_string(*(vt_date *) vartype_get_data_ptr(d), buf, maxlen);
  }
  else if (vt_type == VT_TIMESTAMP) {
    retval = timestamp_to_string(*(vt_timestamp *) vartype_get_data_ptr(d), buf, maxlen);
  }
  else {
    ereport(ERROR,
        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
         errmsg("Unrecognized vt_type of %d", vt_type)));
  }
  return retval;
}


PG_FUNCTION_INFO_V1(vartype_in);
Datum vartype_in(PG_FUNCTION_ARGS) {
	char *str = PG_GETARG_CSTRING(0);
  char *orig_str = str;
  int vt_type;
  char *data;
  vartype *result;

  /* skip leading whitespace */
  while (*str != '\0' && isspace(*str)) str++;

  if (*str == '\0') {
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for vartype: \"%s\"",
						orig_str)));
  }

  vt_type = vartype_parse_input(str);

  if (vt_type == VT_UNKNOWN_TYPE) {
    ereport(ERROR,
        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
         errmsg("invalid input syntax for vartype: \"%s\"", orig_str)));
  }

  if (vt_type == VT_INT) {
    vt_int idata;
    sscanf(orig_str, VT_INT_FORMAT_STR, &idata);
    result = vartype_init_int(idata);
  }
  else if (vt_type == VT_FLOAT) {
    vt_float fdata;
    sscanf(orig_str, VT_FLOAT_FORMAT_STR, &fdata);
    result = vartype_init_float(fdata);
  }
  else if (vt_type == VT_BOOL) {
    vt_bool bdata;
    if (orig_str[0] == 't') {
      bdata = true;
    } else {
      bdata = false;
    }
    result = vartype_init_bool(bdata);
  }
  else if (vt_type == VT_STRING) {
    result = vartype_init_string(orig_str + 1, strlen(str) - 2);
  }
  else if (vt_type == VT_DATE) {
    int date_err;
    vt_date d = string_to_date(orig_str, &date_err);
    if (date_err == 0) {
      // no errors!
      result = vartype_init_date(d);
    }
    else {
      ereport(ERROR,
              (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
               errmsg("unable to interpret date \"%s\" (error code %d)", 
                      orig_str, date_err)));
    }
  }
  else if (vt_type == VT_TIMESTAMP) {
    int timestamp_err;
    vt_timestamp ts = string_to_timestamp(orig_str, &timestamp_err);
    if (timestamp_err == 0) {
      // no errors!
      result = vartype_init_timestamp(ts);
    }
    else {
      ereport(ERROR,
              (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
               errmsg("unable to interpret timestamp \"%s\" (error code %d)", 
                      orig_str, timestamp_err)));
    }
  }
  else {
    ereport(ERROR,
        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
         errmsg("invalid input syntax for vartype: \"%s\"", orig_str)));
  }

  PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(vartype_out);
Datum vartype_out(PG_FUNCTION_ARGS) {
  vartype *d = PG_GETARG_VARTYPE(0);
  int output_size = vartype_get_pretty_string_len(d);
  char *output = palloc(output_size);
  int result = vartype_to_pretty_string(d, output, output_size);
  PG_RETURN_CSTRING(output);
}


PG_FUNCTION_INFO_V1(vartype_output_size);
Datum vartype_output_size(PG_FUNCTION_ARGS) {
  vartype *d = PG_GETARG_VARTYPE(0);
  PG_RETURN_INT32(vartype_get_pretty_string_len(d));
}


PG_FUNCTION_INFO_V1(vartype_get_char);
Datum vartype_get_char(PG_FUNCTION_ARGS) {
  vartype *d = PG_GETARG_VARTYPE(0);
  int i = PG_GETARG_INT32(1);
  int output_size = vartype_get_pretty_string_len(d);
  char *output = palloc(output_size);
  int result = vartype_to_pretty_string(d, output, output_size);
  
  //PG_FREE_IF_COPY(d, 0);

  if (i < 0) { i += output_size; }
  
  PG_RETURN_CHAR(output[i]);
}





/**************************************************************
 *
 * The comparison operators (and supporting functions) follow
 *
 **************************************************************/

int vt_float_cmp(vt_float a, vt_float b) {
  if (isnan(a)) {
    if (isnan(b)) {
      return 0;
    } else {
      return 1;
    }
  }
  else if (isnan(b)) {
    return -1;
  }
  else
  {
    if (a > b) {
      return 1;
    }
    else if (a < b) {
      return -1;
    }
    else {
      return 0;
    }
  }
}


int vt_int_cmp(vt_int a, vt_int b) {
  if (isnan(a)) {
    if (isnan(b)) {
      return 0;
    } else {
      return 1;
    }
  }
  else if (isnan(b)) {
    return -1;
  }
  else
  {
    if (a > b) {
      return 1;
    }
    else if (a < b) {
      return -1;
    }
    else {
      return 0;
    }
  }
}

int vt_bool_cmp(vt_bool a, vt_bool b) {
  return vt_int_cmp(a, b);
}

int vt_timestamp_cmp(vt_timestamp a, vt_timestamp b) {
  if (isnan(a)) {
    if (isnan(b)) {
      return 0;
    } else {
      return 1;
    }
  }
  else if (isnan(b)) {
    return -1;
  }
  else
  {
    if (a > b) {
      return 1;
    }
    else if (a < b) {
      return -1;
    }
    else {
      return 0;
    }
  }
}

int vt_date_cmp(DateADT a, DateADT b) {
  if (isnan(a)) {
    if (isnan(b)) {
      return 0;
    } else {
      return 1;
    }
  }
  else if (isnan(b)) {
    return -1;
  }
  else
  {
    if (a > b) {
      return 1;
    }
    else if (a < b) {
      return -1;
    }
    else {
      return 0;
    }
  }
}


static int vartype_cmp_internal(vartype *a, vartype *b) {
  int a_type, b_type, a_prec, b_prec;
  char *a_data, *b_data;
  a_type = vartype_get_type(a);
  a_prec = vartype_get_precedence(a);
  b_type = vartype_get_type(b);
  b_prec = vartype_get_precedence(b);
  a_data = vartype_get_data_ptr(a);
  b_data = vartype_get_data_ptr(b);

  if (a_prec < b_prec) {
    return -1;
  }
  else if (a_prec > b_prec) {
    return 1;
  }
  else if (a_type == VT_FLOAT) {
    if (b_type == VT_FLOAT) {
      return vt_float_cmp(*(vt_float *) a_data, *(vt_float *) b_data);
    } else {
      // b_type == VT_INT
      return vt_float_cmp(*(vt_float *) a_data, (vt_float) *(vt_int *) b_data);
    }
  }
  else if (a_type == VT_INT) {
    if (b_type == VT_INT) {
      return vt_int_cmp(*(vt_int *) a_data, *(vt_int *) b_data);
    } else {
      // b_type == VT_FLOAT
      return vt_int_cmp(*(vt_int *) a_data, (vt_int) *(vt_float *) b_data);
    }
  }
  else if (a_type == VT_BOOL) {
    // b must also be a bool
    return vt_bool_cmp(*(vt_bool *) a_data, *(vt_bool *) b_data);
  }
  else if (a_type == VT_TIMESTAMP) {
    if (b_type == VT_TIMESTAMP) {
      return vt_timestamp_cmp(*(vt_timestamp *) a_data, *(vt_timestamp *) b_data);
    } else {
      // b_type == VT_DATE
      return vt_timestamp_cmp(*(vt_timestamp *) a_data, 
                             date_to_timestamp(*(vt_date *) b_data));
    }
  }
  else if (a_type == VT_DATE) {
    if (b_type == VT_DATE) {
      return vt_date_cmp(*(vt_date *) a_data, *(vt_date *) b_data);
    } else {
      // b_type == VT_TIMESTAMP
      return vt_date_cmp(date_to_timestamp(*(vt_date *) a_data),
                        *(vt_timestamp *) b_data);
    }
  }
  else if (a_type == VT_STRING) {
    return varstr_cmp(a_data, vartype_get_data_len(a), 
                      b_data, vartype_get_data_len(b), VT_STRING_COLLATION);
  } 
  ereport(ERROR,
      (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
       errmsg("Unknown error occurred comparing VARTYPE data (vartype_types = %d and %d)",
              a_type, b_type)));
}


PG_FUNCTION_INFO_V1(vartype_eq);
Datum vartype_eq(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  vartype *b = PG_GETARG_VARTYPE(1);
  int cmp = vartype_cmp_internal(a, b);
  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
  PG_RETURN_BOOL(cmp == 0);
}


PG_FUNCTION_INFO_V1(vartype_ne);
Datum vartype_ne(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  vartype *b = PG_GETARG_VARTYPE(1);
  int cmp = vartype_cmp_internal(a, b);
  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
  PG_RETURN_BOOL(cmp != 0);
}


PG_FUNCTION_INFO_V1(vartype_lt);
Datum vartype_lt(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  vartype *b = PG_GETARG_VARTYPE(1);
  int cmp = vartype_cmp_internal(a, b);
  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
  PG_RETURN_BOOL(cmp < 0);
}


PG_FUNCTION_INFO_V1(vartype_le);
Datum vartype_le(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  vartype *b = PG_GETARG_VARTYPE(1);
  int cmp = vartype_cmp_internal(a, b);
  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
  PG_RETURN_BOOL(cmp <= 0);
}


PG_FUNCTION_INFO_V1(vartype_gt);
Datum vartype_gt(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  vartype *b = PG_GETARG_VARTYPE(1);
  int cmp = vartype_cmp_internal(a, b);
  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
  PG_RETURN_BOOL(cmp > 0);
}


PG_FUNCTION_INFO_V1(vartype_ge);
Datum vartype_ge(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  vartype *b = PG_GETARG_VARTYPE(1);
  int cmp = vartype_cmp_internal(a, b);
  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
  PG_RETURN_BOOL(cmp >= 0);
}


PG_FUNCTION_INFO_V1(vartype_cmp);
Datum vartype_cmp(PG_FUNCTION_ARGS)
{
  vartype *a = PG_GETARG_VARTYPE(0);
  vartype *b = PG_GETARG_VARTYPE(1);
  int cmp = vartype_cmp_internal(a, b);
  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
	PG_RETURN_INT32(cmp);
}


PG_FUNCTION_INFO_V1(vartype_type);
Datum vartype_type(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  int typ  = vartype_get_type(a);
  PG_FREE_IF_COPY(a, 0);
  PG_RETURN_INT32(typ);
}


/*
This function may uneccessarily require a call
to palloc. I'm currently unsure if returning the 
constant (the element from the VT_TYPE_NAMES array)
is ok (does postgres later attempt to free the
memory associated with that?).

So, for now, I'm copying the const string to a new
buffer created via a call to palloc.
 */
PG_FUNCTION_INFO_V1(vartype_typename);
Datum vartype_typename(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  const char *typename = vartype_get_type_name(a);
  PG_FREE_IF_COPY(a, 0);
  PG_RETURN_CSTRING(pstrdup(typename));
}


PG_FUNCTION_INFO_V1(vartype_len);
Datum vartype_len(PG_FUNCTION_ARGS) {
  vartype *a = PG_GETARG_VARTYPE(0);
  int len = vartype_get_data_len(a);
  PG_FREE_IF_COPY(a, 0);
	PG_RETURN_INT32(len);
}
