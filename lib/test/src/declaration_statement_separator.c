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

  bunny_configuration_setf(cnf, 1, "NoTrailingWhitespace");
  bunny_configuration_setf(cnf, 1, "NoEmptyLineInFunction");
  bunny_configuration_setf(cnf, 1, "SingleInstructionPerLine");
  bunny_configuration_setf(cnf, 1, "DeclarationStatementSeparator");
  load_norm_configuration(&p, cnf);
  i = 0;
  s =
    "void prototype(void);\n"
    "\n"
    "void func(void)\n"
    "{ \n"
    "  void prototype(void);\n"
    "  int i;\n"
    "\n"
    "  if (lol) i = 0;\n"
    "  if (lol) { i = 0; }\n"
    "  while (lol) i = 0;\n"
    "  while (lol) { i = 0; }\n"
    "\n"
    "  for (lol; lol; lol) i = 0;\n"
    "  for (lol; lol; lol) { i = 0; }\n"
    "  do i = 0; while (lol); i = 0;\n"
    "  do { i = 0; } while (lol); i = 0;\n"
    "  return (1); i = 0;\n"
    "  return; i = 0;\n"
    "  goto start; i = 0; \n"
    "  break; i = 0;\n"
    "  continue; i = 0;\n"
    "  i = 0; return (1); \n"
    "  i = 0; return;\n"
    "  i = 0; goto start; \n"
    "  i = 0; break;\n"
    "  i = 0; continue;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.single_instruction_per_line.counter == 21);
  assert(p.declaration_statement_separator.counter == 0);
  assert(p.no_empty_line_in_function.counter == 1);
  assert(p.no_trailing_whitespace.counter == 4);

  i = 0;
  s = "void func(void)\n"
    "{\n"
    "  int i; \n"
    "  i = 0; \n"
    "}\n"
    ;
  p.no_trailing_whitespace.counter = 0;
  p.declaration_statement_separator.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.declaration_statement_separator.counter == 1);
  assert(p.no_trailing_whitespace.counter == 2);

  i = 0;
  assert(s = load_c_file("./res/decseparator1.c", cnf, true));
  load_norm_configuration(&p, cnf);
  p.no_trailing_whitespace.counter = 0;
  p.declaration_statement_separator.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, true) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.declaration_statement_separator.counter == 0);
  assert(p.no_trailing_whitespace.counter == 0);

  i = 0;
  assert(s = load_c_file("./res/decseparator2.c", cnf, true));
  load_norm_configuration(&p, cnf);
  p.no_trailing_whitespace.counter = 0;
  p.declaration_statement_separator.counter = 0;
  fprintf(stderr, "%s\n", s);
  if (read_translation_unit(&p, "file", s, &i, true, true) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.declaration_statement_separator.counter == 1);
  assert(p.no_trailing_whitespace.counter == 2);
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
