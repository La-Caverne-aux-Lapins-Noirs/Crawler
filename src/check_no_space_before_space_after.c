/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

int			check_no_space_before_space_after(t_parsing	*p,
							  const char	*code,
							  int		pos)
{
  if (!p->space_after_comma.value)
    return (1);
  if (pos > 0 && code[pos - 1] == ' ')
    if (!add_warning(p, code, pos, &p->space_after_comma.counter,
		     "Spaces before ',' are forbidden."))
      return (-1);
  if (code[pos + 1] != '\0')
    {
      if ((code[pos + 1] != ' ' && code[pos + 1] != '\n')
	  || (code[pos + 2] != '\0' && isblank(code[pos + 2])))
	if (!add_warning(p, code, pos, &p->space_after_comma.counter,
			 "A single space was expected after comma ','"))
	  return (-1);
    }
  return (1);

}
