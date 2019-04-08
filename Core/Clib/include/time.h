#pragma once

#include <stddef.h>
#include <stdint.h>

// Fakery

typedef uint32_t __timer_t;
typedef uint32_t __clockid_t;
typedef uint32_t __time_t;
typedef __clockid_t clockid_t;
typedef __timer_t timer_t;
typedef  uint32_t __pid_t;
typedef __pid_t pid_t;
typedef void* clock_t;
typedef void* time_t;
typedef  uint32_t __locale_t;

/* POSIX.1b structure for a time value.  This is like a `struct timeval' but
   has nanoseconds instead of microseconds.  */
struct timespec
{
    __time_t tv_sec;		/* Seconds.  */
    long int tv_nsec;		/* Nanoseconds.  */
};

/* Used by other time functions.  */
struct tm
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;	/* Timezone abbreviation.  */
  long int __tm_gmtoff;		/* Seconds east of UTC.  */
  const char *__tm_zone;	/* Timezone abbreviation.  */

/* POSIX.1b structure for timer start values and intervals.  */
struct itimerspec
{
    struct timespec it_interval;
    struct timespec it_value;
};

/* We can use a simple forward declaration.  */
struct sigevent;


static inline clock_t clock (void) { return 0;};

/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
static inline time_t time (time_t *__timer) {return 0; };

/* Return the `time_t' representation of TP and normalize TP.  */
static inline time_t mktime (struct tm *__tp) { return 0; }


/* Format TP into S according to FORMAT.
   Write no more than MAXSIZE characters and return the number
   of characters written, or 0 if it would exceed MAXSIZE.  */
static inline size_t strftime (char *__restrict __s, size_t __maxsize,
			const char *__restrict __format,
			const struct tm *__restrict __tp) { return 0; }

static inline char *strptime (const char *__restrict __s,
		       const char *__restrict __fmt, struct tm *__tp)
     { return 0; }

/* Similar to the two functions above but take the information from
   the provided locale and not the global locale.  */


static inline size_t strftime_l (char *__restrict __s, size_t __maxsize,
			  const char *__restrict __format,
			  const struct tm *__restrict __tp,
			  __locale_t __loc) { return 0; }

static inline char *strptime_l (const char *__restrict __s,
			 const char *__restrict __fmt, struct tm *__tp,
			 __locale_t __loc) { return 0; }


/* Return the `struct tm' representation of *TIMER
   in Universal Coordinated Time (aka Greenwich Mean Time).  */
static inline struct tm *gmtime (const time_t *__timer) { return 0; }

/* Return the `struct tm' representation
   of *TIMER in the local timezone.  */
static inline struct tm *localtime (const time_t *__timer) 
{ 
    return 0; 
}

/* Return a string of the form "Day Mon dd hh:mm:ss yyyy\n"
   that is the representation of TP in this format.  */
static inline char *asctime (const struct tm *__tp) 
{ 
    return 0; 
}

/* Equivalent to `asctime (localtime (timer))'.  */
static inline char *ctime (const time_t *__timer) 
{ 
    return 0; 
}

/* Reentrant versions of the above functions.  */

/* Defined in localtime.c.  */

/* Same as above.  */
static inline char *tzname[2];

/* Set time conversion information from the TZ environment variable.
   If TZ is not defined, a locale-dependent default is used.  */
static inline void tzset (void) 
{ 
    return; 
}

/* Nonzero if YEAR is a leap year (every 4 years,
   except every 100th isn't, and every 400th is).  */
# define __isleap(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))


/* Pause execution for a number of nanoseconds.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
static inline int nanosleep (const struct timespec *__requested_time, struct timespec *__remaining) 
{

}

/* Get resolution of clock CLOCK_ID.  */
static inline int clock_getres (clockid_t __clock_id, struct timespec *__res) 
{ 
    return 0; 
}

/* Get current value of clock CLOCK_ID and store it in TP.  */
static inline int clock_gettime (clockid_t __clock_id, struct timespec *__tp) 
{ 
    return 0; 
}

/* Set clock CLOCK_ID to value TP.  */
static inline int clock_settime (clockid_t __clock_id, const struct timespec *__tp)
{ 
    return 0; 
}

/* Set to one of the following values to indicate an error.
     1  the DATEMSK environment variable is null or undefined,
     2  the template file cannot be opened for reading,
     3  failed to get file status information,
     4  the template file is not a regular file,
     5  an error is encountered while reading the template file,
     6  memory allication failed (not enough memory available),
     7  there is no line in the template that matches the input,
     8  invalid input specification Example: February 31 or a time is
        specified that can not be represented in a time_t (representing
	the time in seconds since 00:00:00 UTC, January 1, 1970) */
static inline int getdate_err;

/* Parse the given string as a date specification and return a value
   representing the value.  The templates from the file identified by
   the environment variable DATEMSK are used.  In case of an error
   `getdate_err' is set.

   This function is a possible cancellation points and therefore not
   marked with __THROW.  */
static inline struct tm* getdate (const char *__string) 
{ 
    return 0; 
}