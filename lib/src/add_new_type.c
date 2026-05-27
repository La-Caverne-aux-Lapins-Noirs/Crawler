/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

bool			add_new_type(t_parsing				*p,
				     const char				*sym,
				     int				size)
{
  size_t		i;

  if (sym == NULL || sym[0] == '\0')
    return (true);
  for (i = 0; i < p->last_new_type; ++i)
    if (strcmp(sym, p->new_type[i].name) == 0)
      {
	if (p->new_type[i].size == -1)
	  p->new_type[i].size = size;
	snprintf(p->last_declaration.last_type,
		 sizeof(p->last_declaration.last_type), "%s", sym);
	return (true);
      }
  if (p->last_new_type >= NBRCELL(p->new_type))
    return (false);
  snprintf(p->new_type[i].name, sizeof(p->new_type[i].name), "%s", sym);
  snprintf(p->last_declaration.last_type,
	   sizeof(p->last_declaration.last_type), "%s", sym);
  p->new_type[i].size = size;
  p->new_type[i].is_function = false;
  p->last_new_type += 1;
  return (true);
}
