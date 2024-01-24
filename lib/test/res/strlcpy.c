
#include	<stddef.h>
#include	<errno.h>

static int	efstrlen(const char		*str)
{
  int		i;

  i = 0;
  while (str[i])
    i = i + 1;
  return (i);
}

char		*efstrlcpy(char			*destination,
			   const char		*source,
			   int			length);

static char	*negative_strlcpy(char		*destination,
				  const char	*source,
				  int		length)
{
  int		slen;
  int		position;

  slen = efstrlen(source);
  if ((position = slen - (length - 1)) < 0)
    position = 0;
  if ((slen = slen - position) < 0)
    slen = length;
  slen += 1;
  return (efstrlcpy(destination, &source[position], slen));
}

char		*efstrlcpy(char			*destination,
			   const char		*source,
			   int			length)
{
  int		i;

  if (!destination || !source)
    {
      errno = EINVAL;
      return (NULL);
    }
  if (length == 0)
    return (destination);
  if (length < 0)
    return (negative_strlcpy(destination, source, -length));
  i = 0;
  while (source[i] != '\0' && i < length - 1)
    {
      destination[i] = source[i];
      i = i + 1;
    }
  while (i < length)
    {
      destination[i] = '\0';
      i = i + 1;
    }
  return (destination);
}

