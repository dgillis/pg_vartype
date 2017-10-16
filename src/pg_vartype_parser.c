#include "./pg_vartype.h"


#define is_single_quote(c) (c == '\'')
#define is_double_quote(c) (c == '"')
#define is_digit(c) isdigit((int) c)
#define is_hyphen(c) (c == '-')
#define is_plus(c) (c == '+')
#define is_period(c) (c == '.')
#define is_space(c) (c == ' ')
#define is_colon(c) (c == ':')
#define is_nul(c) (c == '\0')


static int consume_int(char *str, char **eom) {
  char *ptr = str;
  if (eom != NULL) *eom = NULL;

  if (*ptr == '-') {
    ptr++;
  }

  if (is_digit(*ptr)) {
    ptr++;
  } else {
    return 0;
  }

  while (is_digit(*ptr)) {
    ptr++;
  }

  if (eom != NULL) memcpy(eom, &ptr, sizeof(char *));

  return 1;
}


static int consume_float_decimals(char *str, char **eom) {
  char *ptr = str;

  if (eom != NULL) *eom = NULL;

  if (is_period(*ptr)) {
    ptr++;
  } else {
    return 0;
  }

  if (is_digit(*ptr)) {
    ptr++;
  } else {
    return 0;
  }

  while (is_digit(*ptr)) {
    ptr++;
  }

  if (eom != NULL) *eom = ptr;

  return 1;
}


static int consume_float(char *str, char **eom) {
  char *ptr = str;

  if (*ptr == '-') ptr++;

  if (*ptr == '.') {
    return consume_float_decimals(ptr, eom);
  }
  else if (consume_int(ptr, &ptr)) {
    return consume_float_decimals(ptr, eom);
  }
  else {
    return 0;
  }
}


/*
  Return a pointer after consuming a date of format
  YYYY-MM-DD in str, or NULL if the string doesn't match.
 */
static int consume_date(char *str, char **eom) {
  char *ptr = str;
  if (eom != NULL) *eom = NULL;
  int i;
  for (i = 0; i < 4; i++) {
    if (!is_digit(*ptr)) return 0;
    ptr++;
  }

  if (!is_hyphen(*ptr)) return 0;
  ptr++;

  for (i = 0; i < 2; i++) {
    if (!is_digit(*ptr)) return 0;
    ptr++;
  }

  if (!is_hyphen(*ptr)) return 0;
  ptr++;

  for (i = 0; i < 2; i++) {
    if (!is_digit(*ptr)) return 0;
    ptr++;
  }

  if (eom != NULL) *eom = ptr;

  return 1;
}


static int consume_time(char *str, char **eom) {
  char *ptr = str;
  if (eom != NULL) *eom = NULL;
  int i;
  for (i = 0; i < 2; i++) {
    if (!is_digit(*ptr)) return 0;
    ptr++;
  }

  if (!is_colon(*ptr)) return 0;
  ptr++;

  for (i = 0; i < 2; i++) {
    if (!is_digit(*ptr)) return 0;
    ptr++;
  }

  if (!is_colon(*ptr)) return 0;
  ptr++;

  for (i = 0; i < 2; i++) {
    if (!is_digit(*ptr)) {

      return 0;
    }
    ptr++;
  }

  // check for optional milleseconds, and consume if found
  if (is_period(*ptr) && is_digit(*(ptr + 1))) {
    ptr++;
    while (is_digit(*ptr)) ptr++;
  }

  if (eom != NULL) *eom = ptr;

  return 1;
}


static int consume_timezone(char *str, char **eom) {
  char *ptr = str;

  if (eom != NULL) *eom = NULL;

  if ((is_hyphen(*ptr) || is_plus(*ptr))
      && is_digit(*(ptr + 1)) && is_digit(*(ptr + 2))) {
    ptr += 3;
  }
  else {
    return 0;
  }

  // check for optional minutes portion
  if (is_digit(*ptr) && is_digit(*(ptr + 1))) {
    ptr += 2;
  }

  if (eom != NULL) *eom = ptr;

  return 1;
}


static int consume_timetz(char *str, char **eom) {
  char *ptr = str;

  if (!consume_time(str, &ptr)) {
    return 0;
  }

  return consume_timezone(ptr, eom);
}


static int consume_timestamp(char *str, char **eom) {
  char *ptr = str;

  if (!consume_date(str, &ptr)) {
    return 0;
  }

  if (is_space(*ptr)) {
    ptr++;
  } else {
    return 0;
  }

  if (!consume_time(ptr, &ptr)) {
    return 0;
  }

  char *temp;
  if (consume_timezone(ptr, &temp)) {
    ptr = temp;
  }

  if (eom != NULL) *eom = ptr;

  return 1;
}


static int consume_bool(char *str, char **eom) {
  char *ptr = str;
  char *unmatched;
  int i;

  if (eom != NULL) *eom = NULL;

  if (*ptr == 't') {
    ptr++;
    unmatched = "rue";
  }
  else if (*ptr == 'f') {
    ptr++;
    unmatched = "alse";
  }
  else {
    return 0;
  }

  while (*unmatched && *ptr && *ptr == *unmatched) {
    ptr++;
    unmatched++;
  }

  if (*unmatched) {
    return 0;
  }

  if (eom != NULL) *eom = ptr;

  return 1;
}




/*

*/
int vartype_parse_input(char *str) {
  int i;
  char *curr;
  char *ptr;

  ptr = str;

  if (is_double_quote(*ptr)) {
    // can only be a string - advance to end and check for closing quote
    while (*(ptr + 1)) ptr++;
    if (is_double_quote(*ptr)) {
      return VT_STRING;
    }
    else {
      return VT_UNKNOWN_TYPE;
    }
  }

  if (consume_int(str, &ptr) && is_nul(*ptr)) {
    return VT_INT;
  }

  if (consume_float(str, &ptr) && is_nul(*ptr)) {
    return VT_FLOAT;
  }

  if (consume_timestamp(str, &ptr) && is_nul(*ptr)) {
    return VT_TIMESTAMP;
  }

  if (consume_date(str, &ptr) && is_nul(*ptr)) {
    return VT_DATE;
  }

  if (consume_bool(str, &ptr) && is_nul(*ptr)) {
    return VT_BOOL;
  }

  return VT_UNKNOWN_TYPE;
}

