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

  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a == b) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.while_forbidden.value = 0;
  p.while_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.while_forbidden.counter == 0);
  p.while_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.while_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  do { } while (a == b);\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.do_while_forbidden.value = 0;
  p.do_while_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.do_while_forbidden.counter == 0);
  p.do_while_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.do_while_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  for (a = 0; a < b; ++a) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.for_forbidden.value = 0;
  p.for_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.for_forbidden.counter == 0);
  p.for_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.for_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a == b) { } else { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.else_forbidden.value = 0;
  p.else_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.else_forbidden.counter == 0);
  p.else_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.else_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  switch (a)\n"
    "  {\n"
    "    case A:\n"
    "      break;\n"
    "    case B:\n"
    "      break;\n"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.switch_forbidden.value = 0;
  p.switch_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.switch_forbidden.counter == 0);
  p.switch_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.switch_forbidden.counter == 1);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.break_forbidden.value = 0;
  p.break_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.break_forbidden.counter == 0);
  p.break_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.break_forbidden.counter == 2);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a < b)\n"
    "  {\n"
    "    if (a == c)\n"
    "      continue;"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.continue_forbidden.value = 0;
  p.continue_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.continue_forbidden.counter == 0);
  p.continue_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.continue_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a < b)\n"
    "  {\n"
    "    Lol:\n"
    "    if (a == c)\n"
    "      goto Lol;\n"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.goto_forbidden.value = 0;
  p.goto_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.goto_forbidden.counter == 0);
  p.goto_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.goto_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a < b)\n"
    "  {\n"
    "    if (a == c)\n"
    "      return (b);\n"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.return_forbidden.value = 0;
  p.return_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.return_forbidden.counter == 0);
  p.return_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.return_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  i = j > 2 ? 4 : 2;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.ternary_forbidden.value = 0;
  p.ternary_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.ternary_forbidden.counter == 0);
  p.ternary_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.ternary_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  i++;"
    "  ++i;"
    "  --i;"
    "  i--;"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.inline_mod_forbidden.value = 0;
  p.inline_mod_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.inline_mod_forbidden.counter == 0);
  p.inline_mod_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.inline_mod_forbidden.counter == 4);
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
