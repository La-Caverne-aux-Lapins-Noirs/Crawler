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

#include		"crawler.h"

// Tolere qu'on débute par des étoiles.
int			count_to_new_line(t_parsing		*p,
					  const char		*code,
					  int			pos)
{
  if (!IZ(p, &pos))
    return (1);
  int			tabsize = p->tab_or_space.value;
  int			column = 0;
  int			i = pos;
  bool			stars = true;

  while (i >= 0 && code[i] != '\n')
    {
      if (code[i] == '\t')
	{
	  if (tabsize == 0)
	    if (!add_warning
		(p, true, code, pos,
		 &p->base_indent.counter,
		 "Bad indentation. "
		 "Space are only authorized after tabulation and not inside."
		 ))
	      return (-1);
	  column += tabsize;
	}
      else if (code[i] == '*' && stars == false)
	// On incremente pas, de sorte a ce que le décompte de colonne
	// commence ici: void func(int   *x)
	//                               ^
	// et non au x
	{}
      else
	{
	  stars = false;
	  column += 1;
	}
      i -= 1;
    }
  if (code[i] == '\n')
    i += 1;
  return (column);
}
