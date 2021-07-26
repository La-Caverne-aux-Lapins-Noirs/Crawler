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
  if (!p->space_after_statement.value)
    return (1);

  if (code[pos] != ' ' && code[pos] != '\n')
    goto Bad;
  if (code[pos] != '\n' && isblank(code[pos + 1]))
    goto Bad;
  return (1);

 Bad:
  if (!add_warning(p, code, pos, &p->space_after_statement.counter,
		   "A single space was expected after the statement."))
    return (-1);
  return (1);
}

