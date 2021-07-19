/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

int			check_line_width(t_parsing		*p,
					 const char		*code,
					 int			begin,
					 int			end)
{
  int			count = 0;
  int			last_newline = 0;
  int			i = begin;

  while (i != end && code[i])
    {
      if (code[i] == '\n')
	last_newline = 0;
      else if ((last_newline += 1) > p->max_column_width.value)
	{
	  if (!add_warning
	      (p, code, i, &p->max_column_width.counter,
	       "Line exceed the maximum of %d columns.",
	       p->max_column_width.value))
	    return (-1);
	  count += 1;
	  last_newline = 0;
	}
      i += 1;
    }
  return (1);
}
