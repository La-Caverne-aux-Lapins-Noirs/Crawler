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
  TEST_INTRO();

  i = 0;
  bunny_configuration_setf(cnf, 1, "NonStaticFunctionPerFile[0]");
  bunny_configuration_setf(cnf, -1, "NonStaticFunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = "  static void func1(void){ } static void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.non_static_function_per_file.active);
  assert(p.non_static_function_per_file.counter == 0);
  assert(p.non_static_function_per_file.value == 1);
  assert(p.non_static_function_per_file.pts == 1);

  i = 0;
  bunny_configuration_setf(cnf, 1, "NonStaticFunctionPerFile[0]");
  bunny_configuration_setf(cnf, -1, "NonStaticFunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = "  static void func1(void){ } void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.non_static_function_per_file.active);
  assert(p.non_static_function_per_file.counter == 1);
  assert(p.non_static_function_per_file.value == 1);
  assert(p.non_static_function_per_file.pts == 1);

  TEST_OUTRO();
}
