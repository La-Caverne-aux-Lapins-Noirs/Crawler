/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

bool			add_warning(t_parsing			*p,
				    bool			real,
				    const char			*code,
				    int				pos,
				    int				*cnt,
				    const char			*fmt,
				    ...)
{
  if (real == false)
    return (true);
  char			buf[2048];
  va_list		lst;
  int			end;

  va_start(lst, fmt);
  end = 0;
  end += vsnprintf(&buf[0], sizeof(buf) - end, fmt, lst);
  end += snprintf(&buf[end], sizeof(buf) - end, " (%s, line %d)\n",
		  p->file, bunny_which_line(code, pos));
  write_line_and_position(code, pos, &buf[end], sizeof(buf) - end, true);
  if ((p->last_error_msg[++p->last_error_id] = bunny_strdup(&buf[0])) == NULL)
    { // LCOV_EXCL_START
      p->last_error_id -= 1;
      return (false);
    } // LCOV_EXCL_STOP
  if (cnt)
    *cnt += 1;
  return (true);
}
