#include <stdio.h>
#include <unistd.h>

int tc_putchar(char c)
{
  return(write(1, &c, 1) > 0);
}

int std_is_even(int n);

int main(void)
{
  int c;

  c = 97;

  int i;

  for(i = 0; i < 14; i++)
  {
  if(c < 122)
    {
      tc_putchar(c);
      c = c + 2;
    }
  else
    {
    //n'affiche rien;                                                                                                                                                                          
    }
  }
   tc_putchar('\n');
   return(0);
     }




