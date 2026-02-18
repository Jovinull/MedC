#ifndef CORE_DATETIME_H
#define CORE_DATETIME_H

#include <stdint.h>

int64_t dt_now_unix(void);
void dt_format_hhmm(char out[6]);                 /* "HH:MM" */
void dt_format_ymd_hms(char out[20], int64_t ts); /* "YYYY-MM-DD HH:MM:SS" */

#endif