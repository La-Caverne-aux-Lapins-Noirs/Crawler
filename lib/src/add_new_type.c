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

  for (i = 0; i < p->last_new_type; ++i)
    if (strcmp(sym, p->new_type[i].name) == 0)
      {
	if (p->new_type[i].size == -1)
	  p->new_type[i].size = size;
	strcpy(p->last_declaration.last_type, sym);
	return (true);
      }
  strcpy(p->new_type[i].name, sym);
  strcpy(p->last_declaration.last_type, sym);
  p->new_type[i].size = size;
  if ((p->last_new_type += 1) >= NBRCELL(p->new_type))
    return (false);
  return (true);
}

