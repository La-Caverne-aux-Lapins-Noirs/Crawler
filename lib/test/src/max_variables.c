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

  s =
    "void func(void)\n"
    "{\n"
    "  int i;\n"
    "  int j;\n"
    "}\n"
    ;
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = false;
  p.maximum_variable.value = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.maximum_variable.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = true;
  p.maximum_variable.value = 3;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.maximum_variable.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = true;
  p.maximum_variable.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.maximum_variable.counter == 1);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = true;
  p.maximum_variable.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.maximum_variable.counter == 2);

  bunny_delete_configuration(cnf);
  
  TEST_OUTRO();
}
