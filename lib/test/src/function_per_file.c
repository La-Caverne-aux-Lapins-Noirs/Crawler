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
  bunny_configuration_setf(cnf, 2, "FunctionPerFile[0]");
  bunny_configuration_setf(cnf, -3, "FunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = " void prototype(void); int global = 42; void func1(void){ } void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.function_per_file.active);
  assert(p.function_per_file.counter == 1);
  assert(p.function_per_file.value == 2);
  assert(p.function_per_file.pts == 3);

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
