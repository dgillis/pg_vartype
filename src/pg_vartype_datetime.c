/**
 * For now, my input and output of Dates and Timestamps leans heavily on copied code 
 * from postgres. Much of it is probably superflous.
 **/

#include "./pg_vartype.h"


static void
EncodeSpecialTimestamp(vt_timestamp dt, char *str)
{
  if (TIMESTAMP_IS_NOBEGIN(dt))
    strcpy(str, EARLY);
  else if (TIMESTAMP_IS_NOEND(dt))
    strcpy(str, LATE);
  else  /* shouldn't happen */
    elog(ERROR, "invalid argument for EncodeSpecialTimestamp");
}

static void
EncodeSpecialDate(vt_date dt, char *str)
{
  if (DATE_IS_NOBEGIN(dt))
    strcpy(str, EARLY);
  else if (DATE_IS_NOEND(dt))
    strcpy(str, LATE);
  else  /* shouldn't happen */
    elog(ERROR, "invalid argument for EncodeSpecialDate");
}

static void
AdjustTimestampForTypmod(vt_timestamp *time, int32 typmod)
{
#ifdef HAVE_INT64_TIMESTAMP


  static const int64 TimestampScales[MAX_TIMESTAMP_PRECISION + 1] = {
    INT64CONST(1000000),
    INT64CONST(100000),
    INT64CONST(10000),
    INT64CONST(1000),
    INT64CONST(100),
    INT64CONST(10),
    INT64CONST(1)
  };

  static const int64 TimestampOffsets[MAX_TIMESTAMP_PRECISION + 1] = {
    INT64CONST(500000),
    INT64CONST(50000),
    INT64CONST(5000),
    INT64CONST(500),
    INT64CONST(50),
    INT64CONST(5),
    INT64CONST(0)
  };
#else

  static const double TimestampScales[MAX_TIMESTAMP_PRECISION + 1] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000
  };
#endif

  if (!TIMESTAMP_NOT_FINITE(*time)
    && (typmod != -1) && (typmod != MAX_TIMESTAMP_PRECISION))
  {
    if (typmod < 0 || typmod > MAX_TIMESTAMP_PRECISION)
      ereport(ERROR,
          (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
          errmsg("timestamp(%d) precision must be between %d and %d",
             typmod, 0, MAX_TIMESTAMP_PRECISION)));
    /*                                                                                                                                                            
     * Note: this round-to-nearest code is not completely consistent about
     * rounding values that are exactly halfway between integral values.
     * On most platforms, rint() will implement round-to-nearest-even, but
     * the integer code always rounds up (away from zero). Is it worth
     * trying to be consistent?
     */
#ifdef HAVE_INT64_TIMESTAMP
    if (*time >= INT64CONST(0))
    {
      *time = ((*time + TimestampOffsets[typmod]) / TimestampScales[typmod]) *
        TimestampScales[typmod];
    }
    else
    {
      *time = -((((-*time) + TimestampOffsets[typmod]) / TimestampScales[typmod])
            * TimestampScales[typmod]);
    }
#else
    *time = rint((double) *time * TimestampScales[typmod]) / TimestampScales[typmod];
#endif
  }
}


vt_timestamp string_to_timestamp(char *str, int *err) {
  int32   typmod = MAX_TIMESTAMP_PRECISION;

  vt_timestamp result;
  fsec_t    fsec;
  struct pg_tm tt,
         *tm = &tt;
  int     tz;
  int     dtype;
  int     nf;
  int     dterr;
  char     *field[MAXDATEFIELDS];
  int     ftype[MAXDATEFIELDS];
  char    workbuf[MAXDATELEN + MAXDATEFIELDS];

  dterr = ParseDateTime(str, workbuf, sizeof(workbuf),
              field, ftype, MAXDATEFIELDS, &nf);
  if (dterr == 0) {
    dterr = DecodeDateTime(field, ftype, nf, &dtype, tm, &fsec, &tz);
  }
  memcpy(err, &dterr, sizeof(dterr));
  if (dterr != 0) {
    DateTimeParseError(dterr, str, "timestamp");
    return 0;
  }

  switch (dtype)
  {
    case DTK_DATE:
      if (tm2timestamp(tm, fsec, &tz, &result) != 0)
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
             errmsg("timestamp out of range: \"%s\"", str)));
      break;

    case DTK_EPOCH:
      result = SetEpochTimestamp();
      break;

    case DTK_LATE:
      TIMESTAMP_NOEND(result);
      break;

    case DTK_EARLY:
      TIMESTAMP_NOBEGIN(result);
      break;

    case DTK_INVALID:
      ereport(ERROR,
          (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
        errmsg("date/time value \"%s\" is no longer supported", str)));

      TIMESTAMP_NOEND(result);
      break;

    default:
      elog(ERROR, "unexpected dtype %d while parsing timestamp \"%s\"",
         dtype, str);
      TIMESTAMP_NOEND(result);
  }

  AdjustTimestampForTypmod(&result, typmod);
  
  return result;
}


int timestamp_to_string(vt_timestamp timestamp, char *buf, int maxlen)
{
  char     *result;
  int tz;
  struct pg_tm tt,
         *tm = &tt;
  fsec_t    fsec;
  char     *tzn = NULL;
  //char    buf[MAXDATELEN + 1];

  if (TIMESTAMP_NOT_FINITE(timestamp))
    EncodeSpecialTimestamp(timestamp, buf);
  else if (timestamp2tm(timestamp, &tz, tm, &fsec, &tzn, NULL) == 0)
    EncodeDateTime(tm, fsec, /*&tz - dont want any tz info in output*/NULL, &tzn, DateStyle, buf);
  else
    ereport(ERROR,
        (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
         errmsg("timestamp out of range")));

  return maxlen;
  //result = pstrdup(buf);
  //return result;
}




vt_date string_to_date(char *str, int *err) {

  vt_date   date;
  fsec_t    fsec;
  struct pg_tm tt,
         *tm = &tt;
  int     tzp;
  int     dtype;
  int     nf;
  int     dterr;
  char     *field[MAXDATEFIELDS];
  int     ftype[MAXDATEFIELDS];
  char    workbuf[MAXDATELEN + 1];

  dterr = ParseDateTime(str, workbuf, sizeof(workbuf),
              field, ftype, MAXDATEFIELDS, &nf);

  if (dterr == 0)
    dterr = DecodeDateTime(field, ftype, nf, &dtype, tm, &fsec, &tzp);

  memcpy(err, &dterr, sizeof(dterr));
  if (dterr != 0) {
    //DateTimeParseError(dterr, str, "date");
    return 0;
  }

  switch (dtype)
  {
    case DTK_DATE:
      break;

    case DTK_CURRENT:
      ereport(ERROR,
          (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
        errmsg("date/time value \"current\" is no longer supported")));

      GetCurrentDateTime(tm);
      break;

    case DTK_EPOCH:
      GetEpochTime(tm);
      break;

    case DTK_LATE:
      DATE_NOEND(date);
      PG_RETURN_DATEADT(date);

    case DTK_EARLY:
      DATE_NOBEGIN(date);
      PG_RETURN_DATEADT(date);

    default:
      DateTimeParseError(DTERR_BAD_FORMAT, str, "date");
      break;
  }

  if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday))
    ereport(ERROR,
        (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
         errmsg("date out of range: \"%s\"", str)));

  date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
  
  return date;
}




int date_to_string(vt_date date, char *buf, int maxlen)
{
  char     *result;
  struct pg_tm tt,
         *tm = &tt;
  //char    buf[MAXDATELEN + 1];

  if (DATE_NOT_FINITE(date))
    EncodeSpecialDate(date, buf);
  else
  {
    j2date(date + POSTGRES_EPOCH_JDATE,
         &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
    EncodeDateOnly(tm, DateStyle, buf);
  }
  return maxlen;
  //result = pstrdup(buf);

  //return result;
}



vt_timestamp date_to_timestamp(vt_date date)
{
  vt_timestamp result;
  struct pg_tm tt,
         *tm = &tt;
  int     tz;
  
  if (DATE_IS_NOBEGIN(date))
    TIMESTAMP_NOBEGIN(result);
  else if (DATE_IS_NOEND(date))
    TIMESTAMP_NOEND(result);
  else
  {
    j2date(date + POSTGRES_EPOCH_JDATE,
         &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    tz = DetermineTimeZoneOffset(tm, session_timezone);

#ifdef HAVE_INT64_TIMESTAMP
    result = date * USECS_PER_DAY + tz * USECS_PER_SEC;
    /* Date's range is wider than timestamp's, so check for overflow */
    if ((result - tz * USECS_PER_SEC) / USECS_PER_DAY != date)
      ereport(ERROR,
          (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
           errmsg("date out of range for timestamp")));
#else
    result = date * (double) SECS_PER_DAY + tz;
#endif
  }

  return result;
}
