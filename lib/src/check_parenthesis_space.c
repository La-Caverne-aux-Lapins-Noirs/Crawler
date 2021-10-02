/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

bool			check_parenthesis_space(t_parsing	*p,
						const char	*code,
						int		pos,
						char		dir,
						int		*cnt)
{
  int			i;

  // Cette fonction doit etre appellée avec pos SUR la parenthese
  if (!IZ(p, &pos))
    return (true);
  if (dir == '(')
    {
      // On verifie l'interieur
      for (i = 1; isblank(code[pos + i]); ++i);
      if (code[pos + i] != '\n' && code[pos + i] != '\r' && i != 1)
	if (!add_warning
	    (p, true, code, pos, cnt,
	     "No space was expected after %c.", code[pos]))
	  return (false);
      return (true);
    }
  else if (dir == ')')
    {
      // On verifie l'interieur
      for (i = -1; pos + i > 0 && isblank(code[pos + i]); --i);
      if (code[pos + i] != '\n' && code[pos + i] != '\r' && i != -1)
	if (!add_warning
	    (p, true, code, pos, cnt,
	     "No space was expected before %c.", code[pos]))
	  return (false);
      return (true);
    }
  return (true);
}
