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
  if (dir == '(')
    {
      // Désactivé car redondant avec d'autres vérifications
      if (0 && pos > 0 && !isspace(code[pos - 1]) && code[pos - 1] != code[pos])
	if (!add_warning(p, code, pos, cnt,
			 "A space was expected before %c.", code[pos]))
	  return (false);
      // Mais on verifie toujours l'interieur
      if (isspace(code[pos + 1]) && code[pos + 1] != code[pos])
	if (!add_warning(p, code, pos, cnt,
			 "No space was expected after %c.", code[pos]))
	  return (false);
      return (true);
    }
  else if (dir == ')')
    {
      // On verifie l'interieur
      if (pos > 0 && isspace(code[pos - 1]) && code[pos - 1] != code[pos])
	if (!add_warning(p, code, pos, cnt,
			 "No space was expected before %c.", code[pos]))
	  return (false);
      // Mais on laisse l'exterieur a d'autres verificateurs
      if (0 && !isspace(code[pos + 1]) && code[pos + 1] != code[pos])
	if (!add_warning(p, code, pos, cnt,
			 "A space was expected after %c.", code[pos]))
	  return (false);
      return (true);
    }
  return (true);
}
