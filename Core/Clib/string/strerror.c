#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern struct Kernel _Kernel;

#define MAX(a,b) ((a) > (b) ? (a) : (b))

static void init_error_tables (void);

/* Translation table for errno values.  See intro(2) in most UNIX systems
   Programmers Reference Manuals.

   Note that this table is generally only accessed when it is used at runtime
   to initialize errno name and message tables that are indexed by errno
   value.

   Not all of these errnos will exist on all systems.  This table is the only
   thing that should have to be updated as new error numbers are introduced.
   It's sort of ugly, but at least its portable. */

struct error_info
{
  const int value;		/* The numeric value from <errno.h> */
  const char *const name;	/* The equivalent symbolic value */
  const char *const msg;	/* Short message about this value */
};

#define ENTRY(value, name, msg)	{value, name, msg}

static const struct error_info error_table[] =
{
  /*
  ENTRY(EPERM, "EPERM", "Not owner"),
  ENTRY(ENOENT, "ENOENT", "No such file or directory"),
  ENTRY(ESRCH, "ESRCH", "No such process"),
  ENTRY(EINTR, "EINTR", "Interrupted system call"),
  ENTRY(EIO, "EIO", "I/O error"),
  ENTRY(ENXIO, "ENXIO", "No such device or address"),
  ENTRY(E2BIG, "E2BIG", "Arg list too long"),
  ENTRY(ENOEXEC, "ENOEXEC", "Exec format error"),
  ENTRY(EBADF, "EBADF", "Bad file number"),
  ENTRY(ECHILD, "ECHILD", "No child processes"),
  ENTRY(EWOULDBLOCK, "EWOULDBLOCK", "Operation would block"),
  ENTRY(EAGAIN, "EAGAIN", "No more processes"),
  ENTRY(ENOMEM, "ENOMEM", "Not enough space"),
  ENTRY(EACCES, "EACCES", "Permission denied"),
  ENTRY(EFAULT, "EFAULT", "Bad address"),
  ENTRY(ENOTBLK, "ENOTBLK", "Block device required"),
  ENTRY(EBUSY, "EBUSY", "Device busy"),
  ENTRY(EEXIST, "EEXIST", "File exists"),
  ENTRY(EXDEV, "EXDEV", "Cross-device link"),
  ENTRY(ENODEV, "ENODEV", "No such device"),
  ENTRY(ENOTDIR, "ENOTDIR", "Not a directory"),
  ENTRY(EISDIR, "EISDIR", "Is a directory"),
  ENTRY(EINVAL, "EINVAL", "Invalid argument"),
  ENTRY(ENFILE, "ENFILE", "File table overflow"),
  ENTRY(EMFILE, "EMFILE", "Too many open files"),
  ENTRY(ENOTTY, "ENOTTY", "Not a typewriter"),
  ENTRY(ETXTBSY, "ETXTBSY", "Text file busy"),
  ENTRY(EFBIG, "EFBIG", "File too large"),
  ENTRY(ENOSPC, "ENOSPC", "No space left on device"),
  ENTRY(ESPIPE, "ESPIPE", "Illegal seek"),
  ENTRY(EROFS, "EROFS", "Read-only file system"),
  ENTRY(EMLINK, "EMLINK", "Too many links"),
  ENTRY(EPIPE, "EPIPE", "Broken pipe"),
  ENTRY(EDOM, "EDOM", "Math argument out of domain of func"),
  ENTRY(ERANGE, "ERANGE", "Math result not representable"),
  ENTRY(ENOMSG, "ENOMSG", "No message of desired type"),
  ENTRY(EIDRM, "EIDRM", "Identifier removed"),
  ENTRY(EDEADLK, "EDEADLK", "Deadlock condition"),
  ENTRY(ENOLCK, "ENOLCK", "No record locks available"),
  ENTRY(ENOSTR, "ENOSTR", "Device not a stream"),
  ENTRY(ENODATA, "ENODATA", "No data available"),
  ENTRY(ETIME, "ETIME", "Timer expired"),
  ENTRY(ENOSR, "ENOSR", "Out of streams resources"),
  ENTRY(EREMOTE, "EREMOTE", "Object is remote"),
  ENTRY(ENOLINK, "ENOLINK", "Link has been severed"),
  ENTRY(EMULTIHOP, "EMULTIHOP", "Multihop attempted"),
  ENTRY(EBADMSG, "EBADMSG", "Not a data message"),
  ENTRY(ENAMETOOLONG, "ENAMETOOLONG", "File name too long"),
  ENTRY(EOVERFLOW, "EOVERFLOW", "Value too large for defined data type"),
  ENTRY(EILSEQ, "EILSEQ", "Illegal byte sequence"),
  ENTRY(ENOSYS, "ENOSYS", "Operation not applicable"),
  ENTRY(ELOOP, "ELOOP", "Too many symbolic links encountered"),
  ENTRY(ENOTEMPTY, "ENOTEMPTY", "Directory not empty"),
  ENTRY(EUSERS, "EUSERS", "Too many users"),
  ENTRY(ENOTSOCK, "ENOTSOCK", "Socket operation on non-socket"),
  ENTRY(EDESTADDRREQ, "EDESTADDRREQ", "Destination address required"),
  ENTRY(EMSGSIZE, "EMSGSIZE", "Message too long"),
  ENTRY(EPROTOTYPE, "EPROTOTYPE", "Protocol wrong type for socket"),
  ENTRY(ENOPROTOOPT, "ENOPROTOOPT", "Protocol not available"),
  ENTRY(EPROTONOSUPPORT, "EPROTONOSUPPORT", "Protocol not supported"),
  ENTRY(ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT", "Socket type not supported"),
  ENTRY(EOPNOTSUPP, "EOPNOTSUPP", "Operation not supported on transport endpoint"),
  ENTRY(EPFNOSUPPORT, "EPFNOSUPPORT", "Protocol family not supported"),
  ENTRY(EAFNOSUPPORT, "EAFNOSUPPORT", "Address family not supported by protocol"),
  ENTRY(EADDRINUSE, "EADDRINUSE", "Address already in use"),
  ENTRY(EADDRNOTAVAIL, "EADDRNOTAVAIL","Cannot assign requested address"),
  ENTRY(ENETDOWN, "ENETDOWN", "Network is down"),
  ENTRY(ENETUNREACH, "ENETUNREACH", "Network is unreachable"),
  ENTRY(ENETRESET, "ENETRESET", "Network dropped connection because of reset"),
  ENTRY(ECONNABORTED, "ECONNABORTED", "Software caused connection abort"),
  ENTRY(ECONNRESET, "ECONNRESET", "Connection reset by peer"),
  ENTRY(ENOBUFS, "ENOBUFS", "No buffer space available"),
  ENTRY(EISCONN, "EISCONN", "Transport endpoint is already connected"),
  ENTRY(ENOTCONN, "ENOTCONN", "Transport endpoint is not connected"),
  ENTRY(ESHUTDOWN, "ESHUTDOWN", "Cannot send after transport endpoint shutdown"),
  ENTRY(ETOOMANYREFS, "ETOOMANYREFS", "Too many references: cannot splice"),
  ENTRY(ETIMEDOUT, "ETIMEDOUT", "Connection timed out"),
  ENTRY(ECONNREFUSED, "ECONNREFUSED", "Connection refused"),
  ENTRY(EHOSTDOWN, "EHOSTDOWN", "Host is down"),
  ENTRY(EHOSTUNREACH, "EHOSTUNREACH", "No route to host"),
  ENTRY(EALREADY, "EALREADY", "Operation already in progress"),
  ENTRY(EINPROGRESS, "EINPROGRESS", "Operation now in progress"),
  ENTRY(ESTALE, "ESTALE", "Stale NFS file handle"),
  */
  ENTRY(0, NULL, NULL)
};

/* Translation table allocated and initialized at runtime.  Indexed by the
   errno value to find the equivalent symbolic value. */

static const char **error_names;
static int num_error_names = 0;

/* Translation table allocated and initialized at runtime, if it does not
   already exist in the host environment.  Indexed by the errno value to find
   the descriptive string.

   We don't export it for use in other modules because even though it has the
   same name, it differs from other implementations in that it is dynamically
   initialized rather than statically initialized. */


#define sys_nerr sys_nerr__
#define sys_errlist sys_errlist__
static int sys_nerr;
static const char **sys_errlist;

/*

NAME

	init_error_tables -- initialize the name and message tables

SYNOPSIS

	static void init_error_tables ();

DESCRIPTION

	Using the error_table, which is initialized at compile time, generate
	the error_names and the sys_errlist (if needed) tables, which are
	indexed at runtime by a specific errno value.

BUGS

	The initialization of the tables may fail under low memory conditions,
	in which case we don't do anything particularly useful, but we don't
	bomb either.  Who knows, it might succeed at a later point if we free
	some memory in the meantime.  In any case, the other routines know
	how to deal with lack of a table after trying to initialize it.  This
	may or may not be considered to be a bug, that we don't specifically
	warn about this particular failure mode.

*/

static void init_error_tables (void)
{
  const struct error_info *eip;
  int nbytes;

  /* If we haven't already scanned the error_table once to find the maximum
     errno value, then go find it now. */

  if (num_error_names == 0)
    {
      for (eip = error_table; eip -> name != NULL; eip++)
	{
	  if (eip -> value >= num_error_names)
	    {
	      num_error_names = eip -> value + 1;
	    }
	}
    }

  /* Now attempt to allocate the error_names table, zero it out, and then
     initialize it from the statically initialized error_table. */

  if (error_names == NULL)
    {
      nbytes = num_error_names * sizeof (char *);
      if ((error_names = (const char **) MemoryDriver_Allocate(&_Kernel.Memory, nbytes)) != NULL)
	{
	  memset (error_names, 0, nbytes);
	  for (eip = error_table; eip -> name != NULL; eip++)
	    {
	      error_names[eip -> value] = eip -> name;
	    }
	}
    }

  /* Now attempt to allocate the sys_errlist table, zero it out, and then
     initialize it from the statically initialized error_table. */

  if (sys_errlist == NULL)
    {
      nbytes = num_error_names * sizeof (char *);
      if ((sys_errlist = (const char **) MemoryDriver_Allocate(&_Kernel.Memory, nbytes)) != NULL)
	{
	  memset (sys_errlist, 0, nbytes);
	  sys_nerr = num_error_names;
	  for (eip = error_table; eip -> name != NULL; eip++)
	    {
	      sys_errlist[eip -> value] = eip -> msg;
	    }
	}
    }



}

/*


@deftypefn Extension int errno_max (void)

Returns the maximum @code{errno} value for which a corresponding
symbolic name or message is available.  Note that in the case where we
use the @code{sys_errlist} supplied by the system, it is possible for
there to be more symbolic names than messages, or vice versa.  In
fact, the manual page for @code{perror(3C)} explicitly warns that one
should check the size of the table (@code{sys_nerr}) before indexing
it, since new error codes may be added to the system before they are
added to the table.  Thus @code{sys_nerr} might be smaller than value
implied by the largest @code{errno} value defined in @code{<errno.h>}.

We return the maximum value that can be used to obtain a meaningful
symbolic name or message.

@end deftypefn

*/

int
errno_max (void)
{
  int maxsize;

  if (error_names == NULL)
    {
      init_error_tables ();
    }
  maxsize = MAX (sys_nerr, num_error_names);
  return (maxsize - 1);
}

/*

@deftypefn Supplemental char* strerror (int @var{errnoval})

Maps an @code{errno} number to an error message string, the contents
of which are implementation defined.  On systems which have the
external variables @code{sys_nerr} and @code{sys_errlist}, these
strings will be the same as the ones used by @code{perror}.

If the supplied error number is within the valid range of indices for
the @code{sys_errlist}, but no message is available for the particular
error number, then returns the string @samp{Error @var{num}}, where
@var{num} is the error number.

If the supplied error number is not a valid index into
@code{sys_errlist}, returns @code{NULL}.

The returned string is only guaranteed to be valid only until the
next call to @code{strerror}.

@end deftypefn

*/

char* string_error (int errnoval)
{
  char *msg;
  static char buf[32];

  if (error_names == NULL)
    {
      init_error_tables ();
    }

  if ((errnoval < 0) || (errnoval >= sys_nerr))
    {
      /* Out of range, just return NULL */
      msg = NULL;
    }
  else if ((sys_errlist == NULL) || (sys_errlist[errnoval] == NULL))
    {
      /* In range, but no sys_errlist or no entry at this index. */
      sprintf (buf, "Error %d", errnoval);
      msg = buf;
    }
  else
    {
      /* In range, and a valid message.  Just return the message. */
      msg = (char *) sys_errlist[errnoval];
    }
  
  return (msg);
}

/*

@deftypefn Replacement {const char*} strerrno (int @var{errnum})

Given an error number returned from a system call (typically returned
in @code{errno}), returns a pointer to a string containing the
symbolic name of that error number, as found in @code{<errno.h>}.

If the supplied error number is within the valid range of indices for
symbolic names, but no name is available for the particular error
number, then returns the string @samp{Error @var{num}}, where @var{num}
is the error number.

If the supplied error number is not within the range of valid
indices, then returns @code{NULL}.

The contents of the location pointed to are only guaranteed to be
valid until the next call to @code{strerrno}.

@end deftypefn

*/

const char* strerrno (int errnoval)
{
  const char *name;
  static char buf[32];

  if (error_names == NULL)
    {
      init_error_tables ();
    }

  if ((errnoval < 0) || (errnoval >= num_error_names))
    {
      /* Out of range, just return NULL */
      name = NULL;
    }
  else if ((error_names == NULL) || (error_names[errnoval] == NULL))
    {
      /* In range, but no error_names or no entry at this index. */
      sprintf (buf, "Error %d", errnoval);
      name = (const char *) buf;
    }
  else
    {
      /* In range, and a valid name.  Just return the name. */
      name = error_names[errnoval];
    }

  return (name);
}

/*

@deftypefn Extension int strtoerrno (const char *@var{name})

Given the symbolic name of a error number (e.g., @code{EACCES}), map it
to an errno value.  If no translation is found, returns 0.

@end deftypefn

*/

int strtoerrno (const char *name)
{
  int errnoval = 0;

  if (name != NULL)
    {
      if (error_names == NULL)
	{
	  init_error_tables ();
	}
      for (errnoval = 0; errnoval < num_error_names; errnoval++)
	{
	  if ((error_names[errnoval] != NULL) &&
	      (strcmp (name, error_names[errnoval]) == 0))
	    {
	      break;
	    }
	}
      if (errnoval == num_error_names)
	{
	  errnoval = 0;
	}
    }
  return (errnoval);
}