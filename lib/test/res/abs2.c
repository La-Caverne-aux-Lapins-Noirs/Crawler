#include<unistd.h>
#include <stdlib.h>
#include <stdio.h>

int std_abs(int nbr)
{
  int compteur;
  compteur = 0;
  if ((int)nbr > 0)
    {
      return(compteur);
    }
  if ((int)nbr == 0)
    {
      return(compteur);
    }
  else
    {
      while (( (int)nbr + compteur) < 0)
	{
          compteur = compteur + 1;
	}
    }
  return (compteur);
}

int test_abs(void)
{
  int score;
  score = 0;
  if (tc_abs(-5) != 5)
    {
      score = score + 1;
    }
  else if (tc_abs(-56) != 56)
    {
      score = score + 1;
    }
  else if (tc_abs(-759) != 759)
    {
      score = score + 1;
    }
  else if (tc_abs(-642154) != 642154)
    {
      score = score + 1;
    }
  if (score != 0)
    {
      return (0);
    }
  else
    {
      return (1);
    }
}
