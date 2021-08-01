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
  if (!IZ(p, &pos))
    return (1);
  // On est pile poil apres la virgule
  if (!p->space_after_comma.value || (pos > 0 && code[pos - 1] != ','))
    return (1);
  if (pos > 1 && code[pos - 2] == ' ')
    if (!add_warning
	(p, true, code, pos, &p->space_after_comma.counter,
	 "Spaces before ',' are forbidden."))
      return (-1);
  if (code[pos] != '\0')
    {
      if ((code[pos] != ' ' && code[pos] != '\n')
	  || (code[pos + 1] != '\0' && isblank(code[pos + 1])))
	if (!add_warning
	    (p, true, code, pos, &p->space_after_comma.counter,
	     "A single space was expected after comma ','"))
	  return (-1);
    }
  return (1);

}
