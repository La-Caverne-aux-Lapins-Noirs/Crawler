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
  bunny_configuration_setf(cnf, MIXED_CASE, "FunctionNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "FunctionNameStyle.Points");

  bunny_configuration_setf(cnf, "PFX_", "FunctionNameInfix.Value");
  bunny_configuration_setf(cnf, "Prefix", "FunctionNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "FunctionNameInfix.Points");

  load_norm_configuration(&p, cnf);
  s = " void PFX_FUNCTION_NAME(void) { } void PFX_FUNCTION_NAME(void);  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.active);
  assert(p.function_style.value == 0);
  assert(p.function_style.pts == 10);
  assert(p.function_style.counter == 0);
  assert(p.function_infix.active);
  assert(strncmp(p.function_infix.value, "PFX_", 4) == 0);
  assert(p.function_infix.pts == 8);
  assert(p.function_infix.counter == 0);

  i = 0;
  s = " void FUNCTION_NAME(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 1);

  i = 0;
  s = " void PFX_FUNCTION__NAME(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void FUNCTION_NAME(void);  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  // Les prototypes ne comptent pas dans le décompte des fonctions mal écrites
  // car on peut prototyper des fonctions externes
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfx_function_name(void) {}  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void pfx_function_name(void);  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  // Comme au dessus, ca ne compte pas quand c'est juste un prototype
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 0);

  i = 0;
  bunny_configuration_setf(cnf, "Suffix", "FunctionNameInfix.Position");
  bunny_configuration_setf(cnf, "_suffix", "FunctionNameInfix.Value");
  bunny_configuration_setf(cnf, SNAKE_CASE, "FunctionNameStyle.Value");
  load_norm_configuration(&p, cnf);
  s = " void function_name_suffix(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfx_function_name(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 1);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfxfunctionname(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 2);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void pfx_function__name(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 3);
  assert(p.function_style.counter == 2);

  i = 0;
  s = " void PFX_FUNCTION_NAME(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_infix.counter == 4);
  assert(p.function_style.counter == 3);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  i = 0;
  bunny_configuration_setf(cnf, CAMEL_CASE, "FunctionNameStyle.Value");
  load_norm_configuration(&p, cnf);
  s = " void theFunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void TheFunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void the_FunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 2);

  i = 0;
  s = " void theZERTYUIOIUYTRERTY(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 3);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  i = 0;
  bunny_configuration_setf(cnf, PASCAL_CASE, "FunctionNameStyle.Value");
  s = " void TheFunctionName42(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void theFunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void TheFunction_Name(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 2);

  i = 0;
  s = " void TheFunctionNNNNNNNNNNNNNNNNNNNName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.function_style.counter == 3);

  TEST_OUTRO();
}
