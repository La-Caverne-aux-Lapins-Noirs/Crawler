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

  if (p->single_instruction_per_line.active == false)
    return (true);
  while (code[i] && code[i] != '\n')
    {
      if (!isblank(code[i]))
	{
	  if (statement && p->indent_style.value == KNR_STYLE)
	    if (code[i] == '{')
	      return (check_white_then_newline(p, code, i + 1, statement));
	  if (code[i] == '}')
	    return (true);
	  // write_line_and_position(code, pos);
	  if (!add_warning
	      (p, IZ(p, &i), code, i, &p->single_instruction_per_line.counter,
	       "Multiple instructions on the same line."))
	    return (false);
	  return (true);
	}
      i += 1;
    }
  return (true);
}
