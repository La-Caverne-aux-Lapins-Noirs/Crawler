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

  bunny_configuration_setf(cnf, 3, "MaximumIfInFunction.Value");
  bunny_configuration_setf(cnf, 1, "MaximumIfInFunction.Points");
  load_norm_configuration(&p, cnf);
  
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a == b) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;

  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.maximum_if_in_function.counter == 0);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.maximum_if_in_function.counter == 2);

  bunny_delete_node(cnf, "MaximumIfInFunction");
  bunny_configuration_setf(cnf, 1, "IfForbidden.Value");
  bunny_configuration_setf(cnf, 1, "IfForbidden.Points");
  load_norm_configuration(&p, cnf);
  p.maximum_if_in_function.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.maximum_if_in_function.counter == 5);
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
