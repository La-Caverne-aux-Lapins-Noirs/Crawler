/*
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
  TEST_INTRO(); // LCOV_EXCL_LINE

  i = 0;
  s = "i == 42 ? 5 + 1 : 3 - lel";
  if (read_conditional_expression(&p, s, &i) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
