/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

int			check_single_space(t_parsing		*p,
					   const char		*code,
					   int			pos)
{
  int			start = pos;
  int			end = pos;
  
  if (!p->space_after_statement.value)
    return (1);

  while (start > 1 && code[start - 1] == ' ')
    start -= 1;
  while (code[end] == ' ')
    end += 1;
  if (end - start != 1 || code[start] != ' ')
    {
      if (!add_warning(p, code, pos, &p->space_after_statement.counter,
		       "A single space was expected after the statement."))
	return (-1);
    }
  return (1);
}

