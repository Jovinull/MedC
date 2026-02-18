#include "core/datetime.h"

#include <stdio.h>
#include <time.h>

int64_t dt_now_unix(void) {
  return (int64_t)time(NULL);
}

void dt_format_hhmm(char out[6]) {
  time_t t = time(NULL);
  struct tm tmv;
#if defined(_WIN32)
  localtime_s(&tmv, &t);
#else
  localtime_r(&t, &tmv);
#endif
  snprintf(out, 6, "%02d:%02d", tmv.tm_hour, tmv.tm_min);
}

void dt_format_ymd_hms(char out[20], int64_t ts) {
  time_t t = (time_t)ts;
  struct tm tmv;
#if defined(_WIN32)
  localtime_s(&tmv, &t);
#else
  localtime_r(&t, &tmv);
#endif
  snprintf(out, 20, "%04d-%02d-%02d %02d:%02d:%02d",
           tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
           tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
}