/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		<string.h>
#include		"crawler.h"

int			check_is_alone(t_parsing		*p,
				       const char		*tok,
				       const char		*code,
				       int			pos)
{
  int			start = pos;
  int			end = pos;

  while (start > 0 && code[start] != '\n')
    start -= 1;
  while (code[end] != '\0' && code[end] != '\n')
    end += 1;
  if (start)
    start += 1;
  end -= 1;
  while (isblank(code[start]))
    start += 1;
  while (isblank(code[end]))
    end -= 1;
  if (strncmp(&code[start], tok, end - start) != 0)
    {
      if (!add_warning
	  (p, code, pos, &p->indent_style.counter,
	   "Token %s was expected to be alone on its line.", tok))
	return (-1);
      return (0);
    }
  return (1);
}
