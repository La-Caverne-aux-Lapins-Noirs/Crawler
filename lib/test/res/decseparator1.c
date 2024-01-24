#include	<unistd.h>

static int      efputchar(char          character)
{
  return (write(1, &character, 1) > 0);
}

int             main(void)
{
  char          character;
  int           i;

  i = 0;
  character = ' ';
  while (character <= '~')
    {
      efputchar(character);
      if ((i += 1) >= 16)
	{
	  efputchar('\n');
	  i = 0;
	}
      character += 1;
    }
  efputchar('\n');
}
