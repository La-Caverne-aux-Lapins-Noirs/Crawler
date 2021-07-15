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
  while ((rd = fread(&out[i], 1, *max - i - 1, pip)) > 0)
    i += rd;
  if (ferror(pip) && message)
    { // LCOV_EXCL_START
      snprintf(&message[0], msg_size,
	       "%s: Error encountered while getting the command output.\n",
	       module_name
	       );
    } // LCOV_EXCL_STOP
  out[i] = '\0';
  *max = i;
  return (pclose(pip));
}
