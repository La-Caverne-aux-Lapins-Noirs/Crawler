
#include        <sys/types.h>

void            *std_memset(void        *d,
                            int         v,
                            size_t      l)
{
  char          *data;
  size_t        i;

  i = 0;
  data = d;
  while (i < l)
    {
      data[i] = v;
      i = i + 1;
    }
  return (d);
}


