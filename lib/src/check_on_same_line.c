/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2021
**
** C-C-C CRAWLER!
** Configurable C Code Crawler !
** Bloc constitutif du "TechnoCentre", suite logiciel du projet "Pentacle School"
** Vérificateur de confirmité du code (entre autre) niveau style.
**
** Merci pour la grammaire du C ANSI:
** https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
*/

#include	<ctype.h>
#include	"crawler.h"

bool		check_on_same_line(t_parsing		*p,
				   const char		*code,
				   int			pos,
				   const char		*tok,
				   bool			right)
{
  if (!IZ(p, &pos))
    return (true);
  if (p->indent_style.value != KNR_STYLE)
    return (true);
  int		toklen = strlen(tok);
  int		nl = 0;
  int		i;

  if (right)
    for (i = pos; code[i] && strncmp(&code[i], tok, toklen); ++i)
      {
	if (code[i] == '\n')
	  nl += 1;
	if (!isspace(code[i])) // Il n'y avait peut etre pas d'accolades...
	  {
	    nl = 0;
	    break ;
	  }
      }
  else
    for (i = pos - 1; i >= 0 && strncmp(&code[i], tok, toklen); --i)
      {
	if (code[i] == '\n')
	  nl += 1;
	if (!isspace(code[i])) // Il n'y avait peut etre pas d'accolades...
	  {
	    nl = 0;
	    break ;
	  }
      }
  if (nl)
    if (!add_warning(p, true, code, pos, &p->base_indent.counter,
		     "%s was expected to be found aside its statement.",
		     tok))
      return (false);
  return (true);
}

