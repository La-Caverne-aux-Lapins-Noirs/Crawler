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

  bunny_configuration_setf(cnf, 1, "DeclarationStatementSeparator");
  load_norm_configuration(&p, cnf);
  i = 0;
  s =
    "void prototype(void);\n"
    "void func(void)\n"
    "{\n"
    "  void prototype(void);\n"
    "  int i;\n"
    "\n"
    "  if (lol) i = 0;\n"
    "  if (lol) { i = 0; }\n"
    "  while (lol) i = 0;\n"
    "  while (lol) { i = 0; }\n"
    "  for (lol; lol; lol) i = 0;\n"
    "  for (lol; lol; lol) { i = 0; }\n"
    "  do i = 0; while (lol); i = 0;\n"
    "  do { i = 0; } while (lol); i = 0;\n"
    "  return (1); i = 0;\n"
    "  return; i = 0;\n"
    "  goto start; i = 0;\n"
    "  break; i = 0;\n"
    "  continue; i = 0;\n"
    "  i = 0; return (1);\n"
    "  i = 0; return;\n"
    "  i = 0; goto start;\n"
    "  i = 0; break;\n"
    "  i = 0; continue;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.single_instruction_per_line.counter == 21);

  bunny_delete_configuration(cnf);
  TEST_OUTRO();
}
