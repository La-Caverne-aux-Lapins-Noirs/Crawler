
int			efputchar(char			character);

int			efprint_base2(unsigned int	nbr)
{
  char			character;
  int			len;

  if (nbr < 2)
    len = 0;
  else if ((len = efprint_base2(nbr / 2)) <= 0)
    return (len);
  character = -(nbr % 2) + '0';
  if (efputchar(character) == 0)
    return (-1);
  return (len + 1);
}

