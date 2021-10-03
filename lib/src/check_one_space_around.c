/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

int			check_one_space_around(t_parsing	*p,
					       const char	*code,
					       ssize_t		pos,
					       int		len,
					       int		val,
					       int		*cnt)
{
  if (!val)
    return (1);
  read_whitespace(code, &pos);
  int			to_start = pos;

  if (to_start > 0)
    {
      // A gauche, on a soit un unique espace puis un autre caractère
      // Soit une ligne de blanc jusqu'a un saut de ligne (ou le début de la string)
      while (to_start > 0 && isblank(code[to_start - 1]))
	to_start -= 1;
      if (pos - to_start != 1 && code[to_start - 1] != '\n')
	goto Bad;
    }

  if (code[pos + len] != '\0')
    {
      to_start = pos + len;
      while (isblank(code[to_start]))
	to_start += 1;
      if (to_start - (pos + len) != 1 && code[to_start] != '\n')
	goto Bad;
    }

  return (1);

 Bad:
  if (!add_warning(p, IZ(p, &pos), code, pos, cnt,
		   "A single space was expected around the operator."))
    return (-1);
  return (1);
}
