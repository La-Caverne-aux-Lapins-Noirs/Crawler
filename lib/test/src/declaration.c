k/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2022
** EFRITS SAS 2021-2024
** Pentacle Technologie 2008-2024
**
** TechnoCore
*/

#include		"test.h"


int			main(int		argc,
			     char		**argv)
{
  TEST_INTRO();

  i = 0;
  s =
    "register auto unsigned float const volatile /*             */ "
    "* const * identifier, * const id2 = a || b && c | d ^ e & f == g != h <= i >= j < k > l << m >> n "
    "+ o - p * q / r % (int)s;"
    ;
  if (read_declaration(&p, s, &i) != 1)
    GOTOERROR();
  i = 0;
  s = "int var[42] = {0, 1, 2, 3, 4};";
  if (read_declaration(&p, s, &i) != 1)
    GOTOERROR();
  i = 0;
  assert(read_declaration(&p, "int i", &i) == -1);

  i = 0;
  assert(read_declaration(&p, "int i = ", &i) == -1);
  
  TEST_OUTRO();
}
