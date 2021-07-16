/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

int			check_trailing_whitespace(t_parsing		*p,
						  const char		*code)
{
  int			i = 0;

  while (code[i])
    {
      if (code[i] == '\n' && i > 0)
	{
	  int		j = i - 1;

	  while (isblank(code[j]))
	    j -= 1;
	  if (j != i - 1)
	    {
	      p->no_trailing_whitespace.counter += 1;
	      if (!add_warning(p, code, j, "Trailing whitespace found."))
		return (-1);
	    }
	}
      i += 1;
    }
  return (1);
}
