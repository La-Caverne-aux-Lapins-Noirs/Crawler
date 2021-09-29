/*
** Morgan Ribou Momo
** morgan.ribou - <morgan.ribou@ecole-89.com>
**
** abs – 2021
*/

int	std_abs(int nbr)
{
  if (nbr < 0)
    nbr = -nbr;
  else
    nbr = nbr;
  return (nbr);
}


int	test_abs(void)
{
  if (tc_abs(-5) != 5)
    return (0);
  else
    return (1);
}
