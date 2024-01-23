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

  bunny_configuration_setf(cnf, MIXED_CASE, "LocalVariableNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "LocalVariableNameStyle.Points");
  bunny_configuration_setf(cnf, "_l", "LocalVariableNameInfix");
  bunny_configuration_setf(cnf, "Suffix", "LocalVariableNameInfixPosition");
  bunny_configuration_setf(cnf, 8, "LocalVariableNameInfixPts");
  bunny_configuration_setf(cnf, 1, "LocalVariableInlineInitForbidden.Value");
  bunny_configuration_setf(cnf, 15, "LocalVariableInlineInitForbidden.Points");
  load_norm_configuration(&p, cnf);
  
  i = 0;
  s = " int func(void) { int LOCAL_L; LOCAL_L = 52; return 2; }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.local_variable_infix.active);
  assert(p.local_variable_infix.counter == 0);
  assert(p.local_variable_style.counter == 0);

  i = 0;
  s = " int func(void) { int LOCAL_L = 52; return 2; }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.local_variable_infix.active);
  assert(p.local_variable_infix.counter == 0);
  assert(p.local_variable_style.counter == 0);
  assert(p.local_variable_inline_init_forbidden.counter == 1);

  i = 0;
  s = " int func(void) { int local; local = 52; return (2); }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.local_variable_infix.counter == 1);
  assert(p.local_variable_style.counter == 1);
  
  TEST_OUTRO();
}
