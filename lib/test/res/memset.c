
#include	<errno.h>
#include	<sys/types.h>
#include	<stddef.h>

static void	efmemset25(char		*destination,
			   int		value,
			   size_t	length)
{
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
  *destination++ = value;
}

static void	rmemset(char		*destination,
			int		value,
			size_t		length)
{
  while (length > 25)
    {
      efmemset25(destination, value & 0xFF, length);
      destination = &destination[25];
      length -= 25;
    }
  while (length > 0)
    {
      *destination = value;
      destination++;
      length -= 1;
    }
}

void		*efmemset(void		*destination,
			  int		value,
			  size_t	length)
{
  if (!destination)
    {
      errno = EINVAL;
      return (NULL);
    }
  rmemset(destination, value & 0xFF, length);
  return (destination);
}
