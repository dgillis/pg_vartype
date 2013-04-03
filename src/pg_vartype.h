#ifndef PG_VARTYPE_H
#define PG_VARTYPE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <postgres.h>
#include <utils/palloc.h>
#include <utils/builtins.h>
#include <utils/datetime.h>
#include <utils/date.h>
#include <fmgr.h>
#include <utils/nabstime.h>
#include <miscadmin.h>
#include <libpq/pqformat.h>

typedef struct varlena vartype;

typedef long long vt_int;
typedef double vt_float;
typedef int vt_bool;
typedef DateADT vt_date;
typedef TimestampTz vt_timestamp;

#define VT_INT        0
#define VT_FLOAT      1
#define VT_BOOL       2
#define VT_STRING     3
#define VT_DATE       4
#define VT_TIMESTAMP  5

#define VT_UNKNOWN_TYPE -1

vartype *vartype_alloc(int data_len);

char *vartype_get_data_ptr(const vartype *d);
char *vartype_get_data_len_ptr(const vartype *d);
char *vartype_get_type_ptr(const vartype *d);

void vartype_set_data_len(vartype *d, int data_len);

int vartype_get_precedence(const vartype *d);

/**
 * Assign the variables from d to each pointer. Pass a null pointer to
 * omit the corresponding value.
 **/
void vartype_extract(const vartype *d, int *vt_type, int *data_len, char **data);

void vartype_set_type(vartype *d, int vt_type);
int vartype_get_type(const vartype *d);

vartype *vartype_init_int(vt_int x);
vartype *vartype_init_float(vt_float x);
vartype *vartype_init_bool(vt_bool x);
vartype *vartype_init_date(vt_date x);
vartype *vartype_init_timestamp(vt_timestamp x);
vartype *vartype_init_string(char *s, int len);


int vartype_parse_input(char *str); // see vartype_parser.c


/**
 * See vartype_datetime.c for the timestamp/date functions.
 *
 * Note: the *_to_string functions take a maxlen parameter, but for now do
 * nothing with it. They will eventually, and the signature maintains
 * consistency with the to_pretty_string signature.
 **/
vt_timestamp string_to_timestamp(char *str, int *err);
int timestamp_to_string(vt_timestamp timestamp, char *buf, int maxlen); // returns len of printed string
vt_date string_to_date(char *str, int *err);
int date_to_string(vt_date date, char *buf, int maxlen); // returns len of printed string
vt_timestamp date_to_timestamp(vt_date date);


#define min(a,b) (a < b) ? a : b


Datum	vartype_in(PG_FUNCTION_ARGS);
Datum	vartype_out(PG_FUNCTION_ARGS);
Datum	vartype_recv(PG_FUNCTION_ARGS);
Datum	vartype_send(PG_FUNCTION_ARGS);
Datum	vartype_add(PG_FUNCTION_ARGS);
Datum	vartype_lt(PG_FUNCTION_ARGS);
Datum	vartype_le(PG_FUNCTION_ARGS);
Datum	vartype_eq(PG_FUNCTION_ARGS);
Datum	vartype_ne(PG_FUNCTION_ARGS);
Datum	vartype_ge(PG_FUNCTION_ARGS);
Datum	vartype_gt(PG_FUNCTION_ARGS);
Datum	vartype_cmp(PG_FUNCTION_ARGS);
Datum	vartype_typeid(PG_FUNCTION_ARGS);
Datum	vartype_type(PG_FUNCTION_ARGS);
Datum vartype_len(PG_FUNCTION_ARGS);

Datum vartype_get_char(PG_FUNCTION_ARGS);
Datum vartype_output_size(PG_FUNCTION_ARGS);

#define PG_GETARG_VARTYPE(n) ((vartype *) PG_GETARG_VARLENA_P(n))

#endif /* PG_VARTYPE_H */ 
