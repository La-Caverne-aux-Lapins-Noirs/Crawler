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
					       int		pos,
					       int		len)
{
  int			to_start = pos - 1;
  
  if (!p->space_around_binary_operator.value)
    return (1);

  if (to_start > 0)
    {
      // On boucle jusqu'a trouver le début de la ligne, tant que ce sont des espaces
      // au cas ou l'opérateur débute la ligne
      while (code[to_start] && isblank(code[to_start]))
	to_start -= 1;
      // On a pas trouvé un saut de ligne... on a donc quelque chose du type 2     + 1
      // Il y a plus d'un espace ou il n'y a pas d'espace du tout
      if (code[to_start] != '\n' && pos - to_start != 1)
	goto Bad;
    }

  if (code[pos + len] != '\0')
    {
      to_start = pos + len;
      pos += len / 2;
      while (code[to_start] == ' ')
	to_start += 1;
      if (to_start - pos == 0 && code[to_start] != '\n')
	goto Bad;
      if (to_start - pos != 1)
	goto Bad;
    }

  return (1);

 Bad:
  if (!add_warning(p, code, pos, &p->space_around_binary_operator.counter,
		   "A single space was expected around the binary operator."))
    return (-1);
  return (1);
}

