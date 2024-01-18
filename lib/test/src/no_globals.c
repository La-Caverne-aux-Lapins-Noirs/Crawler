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
  p.last_error_id = -1;
  p.no_global.active = false;
  p.no_global.value = 0;
  p.no_global.counter = 0;
  s = "void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.no_global.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.no_global.active = true;
  p.no_global.value = 1;
  p.no_global.counter = 0;
  s = "void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.no_global.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.no_global.active = false;
  p.no_global.value = 0;
  p.no_global.counter = 0;
  s = "int j; void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.no_global.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.no_global.active = true;
  p.no_global.value = 1;
  p.no_global.counter = 0;
  s = "int j; void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.no_global.counter == 1);
  
  TEST_OUTRO();
}
