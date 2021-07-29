/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

bool			check_pointer_star_position(t_parsing	*p,
						    const char	*code,
						    int		pos)
{
  int			i = pos;
  bool			first = true;
  bool			statement = false;

  if (p->inbetween_ptr_symbol_space.value == 0 &&
      p->ptr_symbol_on_name.value == 0 &&
      p->ptr_symbol_on_type.value == 0)
    return (true);
  if (i > 0 && code[i - 1] != '\n')
    i -= 1;
  while (i > 0 && code[i - 1] != '\n')
    {
      if (!isspace(code[i]) && code[i] != '*')
	statement = true;
      if (code[i] == '*')
	{
	  if (first == false || statement == true)
	    {
	      if (p->inbetween_ptr_symbol_space.value)
		if (check_one_space_around
		    (p, code, i, 1, true,
		     &p->inbetween_ptr_symbol_space.counter
		     ) == -1)
		  return (false);
	    }
	  else
	    {
	      first = false;
	      if (p->ptr_symbol_on_name.value)
		if (!check_parenthesis_space(p, code, i, '(', &p->ptr_symbol_on_name.counter))
		  return (false);
	      if (p->ptr_symbol_on_type.value)
		if (!check_parenthesis_space(p, code, i, ')', &p->ptr_symbol_on_type.counter))
		  return (false);
	    }
	}
      i -= 1;
    }
  return (true);
}
