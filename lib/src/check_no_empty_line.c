/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

int			check_no_empty_line(t_parsing		*p,
					    const char		*code,
					    int			pos,
					    int			sep,
					    int			begin,
					    int			end)
{
  if (!IZ(p, &pos))
    return (1);
  int			i = begin;
  int			dbl = 0;

  while (i != end)
    {
      if (i < end - 1 && code[i] == '\n' && code[i - 1] == '\n')
	dbl += 1;
      i += 1;
    }

  if (dbl > sep && i > p->last_declaration.end_of_declaration)
    {
      if (!add_warning
	  (p, true, code, pos, &p->no_empty_line_in_function.counter,
	   "Forbidden empty line in scope."))
	return (-1);
      return (1);
    }

  return (1);
}
