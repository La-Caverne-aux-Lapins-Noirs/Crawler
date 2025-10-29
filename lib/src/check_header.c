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

bool			check_header(t_parsing		*p,
				     const char		*code)
{
  /// Cette fonction n'est appelée que sur du code non préprocessé
  if (p->header.active == false)
    return (true);
  const char		*template = &p->header_data[0];
  ssize_t		i;
  ssize_t		j;

  i = j = 0;
  gl_bunny_read_whitespace = NULL;
  while (template[i])
    {
      if (template[i] == '%')
	{
	  i += 1;
	  bunny_read_field(template, &i);
	  bunny_read_char(code, &j,
			  "abcdefghijklmnopqrstuvwxyz"
			  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			  "0123456789_-:/.@"
			  );
	}
      else if (template[i] != code[j])
	break ;
      else
	{
	  i += 1;
	  j += 1;
	}
    }
  gl_bunny_read_whitespace = read_whitespace;
  
  if (template[i] && p->header.counter == 0)
    {
      // Le header peut tout à fait etre avant le marqueur, car les #include sont apres
      // les en tete.
      int		tmp = p->last_line_marker;

      p->last_line_marker = 0;
      if (!add_warning(p, true, code, 0, &p->header.counter, "The header is absent or misformed."))
	{
	  p->last_line_marker = tmp;
	  return (false);
	}
      p->last_line_marker = tmp;
    }
  p->last_declaration.last_header = 0;
  p->last_declaration.header_line = bunny_which_line(code, 0);
  return (true);
}

