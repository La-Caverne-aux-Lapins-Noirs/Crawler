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
  s =
    "void cnt(void)\n"
    "{\n"
    "  int i = 57;\n"
    "  double j = 4.2;\n"
    "  int k;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.no_short_name.active = true;
  p.no_short_name.value = 4;
  p.no_magic_value.active = false;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.no_short_name.counter == 0);
  assert(p.no_magic_value.counter == 0);

  i = 0;
  s =
    "void foo(void)\n"
    "{\n"
    "  int bar = 57;\n"
    "  int fob = 4.2;\n"
    "  int bao;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.no_short_name.active = true;
  p.no_short_name.value = 4;
  p.no_magic_value.active = true;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.no_short_name.counter == 4);
  assert(p.no_magic_value.counter == 2);
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
