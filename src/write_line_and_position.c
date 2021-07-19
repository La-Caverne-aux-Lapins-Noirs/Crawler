/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"
#define			IGN(a)					if (a) {}

void			write_line_and_position(const char	*code,
						int		pos,
						char		*buf,
						size_t		len)
{
  int			i = pos;
  int			j = pos;
  int			k = 0;

  while (i > 0 && code[i] != '\n')
    i -= 1;
  if (code[i] == '\n')
    i += 1;
  while (code[j] && code[j] != '\n')
    j += 1;
  if (code[j] == '\n')
    j -= 1;
  k += snprintf(&buf[k], len - k, "%.*s\n", j - i + 1, &code[i]);
  while (i < pos)
    {
      k += snprintf(&buf[k], len - k, " ");
      i += 1;
    }
  k += snprintf(&buf[k], len - k, "^\n");
}

