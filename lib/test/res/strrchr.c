
#include	<stddef.h>

char		*efstrrchr(char		*str,
			   int		character)
{
  char		*last;
  int		i;

  if (!str)
    return (NULL);
  i = 0;
  last = NULL;
  while (str[i])
    {
      if (str[i] == character)
	last = &str[i];
      i = i + 1;
    }
  return (last);
}
