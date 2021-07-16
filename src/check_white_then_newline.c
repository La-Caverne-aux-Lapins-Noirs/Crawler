/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

bool			check_white_then_newline(t_parsing	*p,
						 const char	*code,
						 int		pos,
						 bool		statement)
{
  int			i = pos;

  while (code[i] && code[i] != '\n')
    {
      if (!isblank(code[i]))
	{
	  if (statement && p->indent_style.value == KNR_STYLE)
	    if (code[i] == '{')
	      return (check_white_then_newline(p, code, i + 1, statement));
	  if (code[i] == '}')
	    return (1);
	  p->single_instruction_per_line.counter += 1;
	  if (!add_warning(p, code, i, "Multiple instructions on the same line."))
	    return (-1);
	  return (1);
	}
      i += 1;
    }
  return (1);
}
