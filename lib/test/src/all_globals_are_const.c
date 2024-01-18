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
  s = "int i = 0;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.all_globals_are_const.active = false;
  p.all_globals_are_const.value = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.all_globals_are_const.counter == 0);

  i = 0;
  s = "int i = 0;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.all_globals_are_const.active = true;
  p.all_globals_are_const.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.all_globals_are_const.counter == 1);

  i = 0;
  s = "const int i = 0;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.all_globals_are_const.active = true;
  p.all_globals_are_const.value = 1;
  p.all_globals_are_const.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.all_globals_are_const.counter == 0);
  
  TEST_OUTRO();
}
