/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

// CONSIDERANT QUE LE CHEMIN EST FAIT DEPUIS LA RACINE DU PROJET
int			compare_file_and_function_name(t_parsing	*p,
						       const char	*func,
						       const char	*code,
						       int		pos)
{
  if (!IZ(p, &pos))
    return (1);
  const char * const	*split;
  const char		*tokens[4] = {"/", "\\", ".c", NULL};
  const char		*start = p->file;
  char			buffer[512];
  int			i = 0;
  int			j;

  if (strncmp(start, "./", 2) == 0)
    start += 2;
  else if (strncmp(start, "/", 1) == 0)
    start += 1;
  
  if ((split = bunny_split(start, &tokens[0], true)) == NULL)
    return (-1);

  for (j = 0; split[j]; ++j)
    {
      if (j == 0 && strcmp(split[j], "src") == 0)
	continue ;
      if (split[j + 1])
	{
	  i += snprintf(&buffer[i], sizeof(buffer) - i, "%s", split[j]);
	  if (p->function_style.value == MIXED_CASE ||
	      p->function_style.value == SNAKE_CASE)
	    i += snprintf(&buffer[i], sizeof(buffer) - i, "_");
	}
      else
	i += snprintf(&buffer[i], sizeof(buffer) - i, "%s", split[j]);
    }
  bunny_delete_split(split);

  if (strcmp(&buffer[0], func) != 0)
    {
      if (add_warning
	  (p, true, code, pos, &p->function_matching_path.counter,
	   "Function %s does not match its path name %s.",
	   func, &buffer[0]) == false)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
    }
  return (1);
}

