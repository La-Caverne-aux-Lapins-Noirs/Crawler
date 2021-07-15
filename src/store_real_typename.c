/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<string.h>
#include		"crawler.h"

int			store_real_typename(t_parsing		*p,
					    char		*target,
					    const char		*symbol,
					    int			len,
					    int			typ)
{
  int			spoint = 0;
  int			flen = strlen(symbol);
  
  if (typ == 0 && p->struct_infix.active)
    {
      if (p->struct_infix.position == 0)
	spoint = strlen(p->struct_infix.value);
      else
	flen -= strlen(p->struct_infix.value);
    }
  else if (typ == 1 && p->union_infix.active)
    {
      if (p->union_infix.position == 0)
	spoint = strlen(p->union_infix.value);
      else
	flen -= strlen(p->union_infix.value);
    }
  else if (typ == 2 && p->typedef_infix.active)
    {
      if (p->typedef_infix.position == 0)
	spoint = strlen(p->typedef_infix.value);
      else
	flen -= strlen(p->typedef_infix.value);
    }
  else if (typ == 3 && p->enum_infix.active) // enum
    {
      if (p->enum_infix.position == 0)
	spoint = strlen(p->enum_infix.value);
      else
	flen -= strlen(p->enum_infix.value);
    }
  else if (typ == 4 && p->function_infix.active) // fonction
    {
      if (p->function_infix.position == 0)
	spoint = strlen(p->function_infix.value);
      else
	flen -= strlen(p->function_infix.value);
    }
  strncpy(target, &symbol[spoint], flen > len ? len : flen);
  return (1);
}
