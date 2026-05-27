/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<string.h>
#include		"crawler.h"

/*
** La vérification que l'infix est présent n'est
** pas faite pour les suffixes.
*/

static void		apply_infix(const char		*infix,
				    int			position,
				    const char			*symbol,
				    int				*spoint,
				    int				*flen)
{
  size_t		infix_len;

  infix_len = strlen(infix);
  if (position == 0)
    {
      if (strncmp(infix, symbol, infix_len) == 0)
	*spoint = infix_len;
    }
  else if ((size_t)*flen >= infix_len)
    *flen -= infix_len;
}

int			store_real_typename(t_parsing		*p,
					    char		*target,
					    const char		*symbol,
					    int			len,
					    int			typ)
{
  int			spoint;
  int			flen;

  spoint = 0;
  flen = strlen(symbol);
  if (typ == 0 && p->struct_infix.active)
    apply_infix(p->struct_infix.value, p->struct_infix.position,
		symbol, &spoint, &flen);
  else if (typ == 1 && p->union_infix.active)
    apply_infix(p->union_infix.value, p->union_infix.position,
		symbol, &spoint, &flen);
  else if (typ == 2 && p->typedef_infix.active)
    apply_infix(p->typedef_infix.value, p->typedef_infix.position,
		symbol, &spoint, &flen);
  else if (typ == 3 && p->enum_infix.active)
    apply_infix(p->enum_infix.value, p->enum_infix.position,
		symbol, &spoint, &flen);
  else if (typ == 4 && p->function_infix.active)
    {
      if (strcmp("main", symbol) == 0 || bunny_strncasecmp("test_", symbol, 5) == 0)
	{
	  spoint = 0;
	  flen = strlen(symbol);
	}
      else
	apply_infix(p->function_infix.value, p->function_infix.position,
		    symbol, &spoint, &flen);
    }
  if (spoint > flen)
    spoint = flen;
  flen -= spoint;
  if (flen < 0)
    flen = 0;
  snprintf(&p->last_declaration.last_type[0],
	   sizeof(p->last_declaration.last_type), "%s", symbol);
  if (len <= 0)
    return (1);
  if (flen > len - 1)
    flen = len - 1;
  memcpy(target, &symbol[spoint], flen);
  target[flen] = 0;
  return (1);
}
