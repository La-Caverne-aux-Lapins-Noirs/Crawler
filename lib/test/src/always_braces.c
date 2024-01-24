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
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "  {\n"
    "     a = 2;\n"
    "  }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.always_braces.value = 0;
  p.always_braces.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.always_braces.counter == 0);
  p.always_braces.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.always_braces.counter == 0);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "     a = 2;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.always_braces.value = 0;
  p.always_braces.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.always_braces.counter == 0);
  p.always_braces.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.always_braces.counter == 1);
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
