/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<string.h>
#include		"crawler.h"

int			check_base_indentation(t_parsing		*p,
					       const char		*code,
					       int			pos)
{
  ssize_t		i = pos;
  int			tab = 0;
  int			space = 0;
  bool			first_space = false;
  int			ilen;

  if (p->base_indent.active == false)
    return (1);
  // Afin d'éviter de calculer deux fois l'indentation d'une meme ligne
  ilen = bunny_which_line(code, pos);
  if (p->last_declaration.last_line >= ilen)
    return (1);
  p->last_declaration.last_line = ilen;

  read_whitespace(code, &i);
  while (i > 0 && code[i] != '\n')
    i -= 1;
  if (code[i] == '\n')
    i += 1;
  while (code[i] == ' ' || code[i] == '\t')
    {
      if (code[i] == ' ')
	{
	  space += 1;
	  first_space = true;
	}
      else if (code[i] == '\t')
	{
	  tab += 1;
	  // Si on a une tabulation apres un espace et qu'on tolerait la tabulation
	  if (first_space && p->tab_or_space.value != 0)
	    {
	      if (!add_warning
		  (p, code, pos,
		   &p->base_indent.counter,
		   "Bad indentation. "
		   "Space are only authorized after tabulation and not inside."
		   ))
		return (-1);
	      return (1);
	    }
	}
      i += 1;
    }
  if (p->tab_or_space.value == 0 && tab != 0)
    {
      if (!add_warning
	  (p, code, pos, &p->base_indent.counter,
	   "Tabulation are forbidden for indentation purpose."))
	return (-1);
      return (1);
    }
  ilen = tab * p->tab_or_space.value + space;
  if (ilen != p->last_declaration.indent_depth * p->base_indent.value)
    {
      // full_write_with_arrow(p, code, pos); // Debug
      // printf("Calculated indentation: %.2f\n", (float)ilen / p->base_indent.value); // Dbg
      if (!add_warning
	  (p, code, pos, &p->base_indent.counter,
	   "Bad indentation. "
	   "Indentation width is %d where %d was expected.",
	   ilen,
	   p->last_declaration.indent_depth * p->base_indent.value
	   ))
	return (-1);
      return (1);
    }
  return (1);
}

