/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2020
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<sys/types.h>
#include		<stdio.h>
#include		<stdlib.h>
#include		<stdarg.h>
#include		<stdbool.h>

int			tcpopen(const char		*module_name,
				const char		*cmd,
				char			*out,
				int			*max,
				char			*message,
				size_t			msg_size)
{
  FILE			*pip;
  ssize_t		rd;
  size_t		i;
  size_t		limit;
  bool			truncated;
  int			status;

  if ((pip = popen(cmd, "r")) == NULL)
    { // LCOV_EXCL_START
      if (message)
	snprintf(&message[0], msg_size,
		 "%s: Cannot execute the required command '%s'.\n",
		 module_name, cmd
		 );
      return (-1);
    } // LCOV_EXCL_STOP
  i = 0;
  truncated = false;
  limit = *max > 0 ? (size_t)*max : 0;
  while (limit > 1 && i + 1 < limit
	 && (rd = fread(&out[i], 1, limit - i - 1, pip)) > 0)
    i += rd;
  if (limit > 0)
    out[i] = '\0';
  if (limit > 0 && i + 1 >= limit)
    {
      int c = fgetc(pip);

      if (c != EOF)
	truncated = true;
    }
  if (ferror(pip) && message)
    { // LCOV_EXCL_START
      snprintf(&message[0], msg_size,
	       "%s: Error encountered while getting the command output.\n",
	       module_name
	       );
    } // LCOV_EXCL_STOP
  *max = i;
  status = pclose(pip);
  if (truncated)
    {
      if (message)
	snprintf(&message[0], msg_size,
		 "%s: Command output was too large and was truncated.\n",
		 module_name);
      return (-1);
    }
  return (status);
}
