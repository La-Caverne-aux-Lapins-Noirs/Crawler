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
  char			buffer[sizeof(p->header_data) * 2];
  ssize_t		src = 0;
  ssize_t		tar = 0;

  // On compose le header
  gl_bunny_read_whitespace = NULL;
  while (template[src] != '\0' && tar < (int)sizeof(buffer) - 1)
    {
      if (bunny_read_text(template, &src, "%"))
	{
	  char		tmpbuffer[256];
	  ssize_t	tmp = src;
	  const char	*str;

	  if (!bunny_read_field(template, &src))
	    continue ;
	  snprintf(tmpbuffer, sizeof(tmpbuffer), "%.*s", (int)(src - tmp), &template[tmp]);
	  if (bunny_configuration_getf(p->configuration, &str, "%s", tmpbuffer))
	    tar += snprintf(&buffer[tar], sizeof(buffer) - tar, "%s", str);
	}
      else
	buffer[tar++] = template[src++];
    }
  buffer[tar] = '\0';
  gl_bunny_read_whitespace = read_whitespace;
  // Le header est censé etre tout en haut du fichier
  ssize_t		cursor = 0; // strlen(code) - tar;

  while (cursor >= 0)
    {
      // On verifie qu'il est bien présent au début du fichier
      if (strncmp(&code[cursor], &buffer[0], tar) == 0)
	break ;
      cursor -= 1;
    }
  if (cursor < 0 && p->header.counter == 0)
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
  p->last_declaration.last_header = cursor;
  p->last_declaration.header_line = bunny_which_line(code, cursor);
  return (true);
}

