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
  s = "int func(double, int);";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  s = "const int * function_symbol(int, float*);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
