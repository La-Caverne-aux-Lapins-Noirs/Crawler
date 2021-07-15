/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

bool			add_warning(t_parsing			*p,
				    const char			*code,
				    int				pos,
				    const char			*fmt,
				    ...)
{
  char			buf[512];
  va_list		lst;
  int			end;

  va_start(lst, fmt);
  end = vsnprintf(&buf[0], sizeof(buf), fmt, lst);
  snprintf(&buf[end], sizeof(buf) - end, " (%s, line %d)", p->file, bunny_which_line(code, pos));
  if ((p->last_error_msg[++p->last_error_id] = bunny_strdup(&buf[0])) == NULL)
    { // LCOV_EXCL_START
      p->last_error_id -= 1;
      return (false);
    } // LCOV_EXCL_STOP
  return (true);
}
