k/*
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

  cnf = bunny_new_configuration();
  bunny_configuration_setf(cnf, 1, "NoTrailingWhitespace");
  bunny_configuration_setf(cnf, 1, "NoEmptyLineInFunction");
  bunny_configuration_setf(cnf, 1, "SingleInstructionPerLine");
  load_norm_configuration(&p, cnf);
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  int tab[2][2] =\n"
    "    {\n"
    "      {\n"
    "        42,\n"
    "        89\n"
    "      },\n"
    "      {\n"
    "        42,\n"
    "        89\n"
    "      }\n"
    "    };\n"
    "  int i;\n"
    "  i = 42;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.no_trailing_whitespace.counter == 0);
  assert(p.no_empty_line_in_function.counter == 0);
  assert(p.single_instruction_per_line.counter == 0);

  i = 0;
  s =
    "void func(void) \n"
    "{  \n"
    "  int i, j; \n"
    "\n"
    "  i = 42; j += 1; \n"
    "  i = 42, j += 1; \n"
    "\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.no_trailing_whitespace.counter == 6);
  assert(p.no_empty_line_in_function.counter == 1);
  assert(p.single_instruction_per_line.counter == 3);
  
  TEST_OUTRO();
}
