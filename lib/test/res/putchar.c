
#include	<unistd.h>

int		efputchar(char	character)
{
  return (write(1, &character, sizeof(character)) > 0);
}

