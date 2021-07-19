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
					    bool		sep,
					    int			begin,
					    int			end)
{
  int			i = begin;
  int			dbl = 0;

  while (i != end)
    {
      if (i < end - 1 && code[i] == '\n' && code[i - 1] == '\n')
	dbl += 1;
      if ((dbl > 0 && sep == false) || (dbl > 1 && sep == true))
	{
	  if (!add_warning
	      (p, code, pos, &p->no_empty_line_in_function.counter,
	       "Forbidden empty line in scope."))
	    return (-1);
	  return (1);
	}
      i += 1;
    }
  return (1);
}

