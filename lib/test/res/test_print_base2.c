
#include	<limits.h>

int		efprint_base2(int		nbr);

int		test_print_base2(void)
{
  if (efprint_base2(1) != 1)
    return (7);
  if (efprint_base2(2) != 2)
    return (6);
  if (efprint_base2(3) != 2)
    return (5);
  if (efprint_base2(16) != 5)
    return (4);
  if (efprint_base2(0) != 1)
    return (3);
  // 2147483647
  if (efprint_base2(UINT_MAX) != 32)
    return (2);
  return (0);
}
