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
  int			max;
  int			next;

  if (real == false)
    return (true);
  if (p->last_line_marker > pos)
    return (true);
  max = NBRCELL(p->last_error_msg);
  next = p->last_error_id + 1;
  if (next >= max)
    return (true);
  if (next == max - 1)
    {
      if ((p->last_error_msg[next] =
	   bunny_strdup("Too many errors encountered. Stop reporting.")) == NULL)
	return (false);
      p->last_error_id = next;
      return (true);
    }
  char			buf[2048];
  va_list		lst;
  int			end;

  va_start(lst, fmt);
  end = vsnprintf(&buf[0], sizeof(buf), fmt, lst);
  va_end(lst);
  if (end < 0)
    return (false);
  if ((size_t)end >= sizeof(buf))
    end = sizeof(buf) - 1;
  if ((size_t)end < sizeof(buf))
    {
      int written = snprintf(&buf[end], sizeof(buf) - (size_t)end,
			     " (%s, line %d)\n",
			     p->file,
			     bunny_which_line(code, pos) - p->last_line_marker_line
			     );

      if (written < 0)
	return (false);
      if ((size_t)written >= sizeof(buf) - (size_t)end)
	end = sizeof(buf) - 1;
      else
	end += written;
    }
  if ((size_t)end < sizeof(buf))
    write_line_and_position(code, pos, &buf[end], sizeof(buf) - (size_t)end, true);
  if ((p->last_error_msg[next] = bunny_strdup(&buf[0])) == NULL)
    return (false);
  p->last_error_id = next;
  if (cnt)
    *cnt += 1;
  return (true);
}
