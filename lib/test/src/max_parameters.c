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
  s =
    "void func(int a, int b, int c)\n"
    "{\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.max_parameter.active = false;
  p.max_parameter.value = 0;
  p.max_parameter.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.max_parameter.counter == 0);
  p.max_parameter.active = true;
  p.max_parameter.value = 4;
  p.max_parameter.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.max_parameter.counter == 0);
  p.max_parameter.active = true;
  p.max_parameter.value = 2;
  p.max_parameter.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.max_parameter.counter == 1);
  p.max_parameter.active = true;
  p.max_parameter.value = 3;
  p.max_parameter.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.max_parameter.counter == 0);
  
  TEST_OUTRO();
}
