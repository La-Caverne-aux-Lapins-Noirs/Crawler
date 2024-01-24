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
  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  bunny_configuration_setf(cnf, SNAKE_CASE, "GlobalVariableNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "GlobalVariableNameStyle.Points");
  bunny_configuration_setf(cnf, "gl_", "GlobalVariableNameInfix[0]");
  bunny_configuration_setf(cnf, 0, "GlobalVariableNameInfix[1]");
  bunny_configuration_setf(cnf, 8, "GlobalVariableNameInfix[2]");


  load_norm_configuration(&p, cnf);
  s = " int gl_value = 50;  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.global_variable_style.active);
  assert(p.global_variable_infix.active);
  assert(p.global_variable_style.value == SNAKE_CASE);
  assert(p.global_variable_style.counter == 0);
  assert(p.global_variable_infix.counter == 0);

  i = 0;
  s = " int value = 50;  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.global_variable_style.counter == 0);
  assert(p.global_variable_infix.counter == 1);

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
