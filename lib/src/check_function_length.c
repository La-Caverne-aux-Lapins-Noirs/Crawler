/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

int			check_function_length(t_parsing		*p,
					      const char	*code,
					      int		begin,
					      int		end)
{
  if (!IZ(p, &begin))
    return (1);
  int			i = begin;
  int			cnt = 0;

  while (i != end)
    {
      if (code[i] == '\n')
	if ((cnt += 1) > p->max_function_length.value + 1)
	  {
	    if (!add_warning
		(p, true, code, i, &p->max_function_length.counter,
		 "Function exceed the maximum of %d lines.",
		 p->max_function_length.value))
	      return (-1);
	    cnt = 0;
	  }
      i += 1;
    }
  return (1);
}
