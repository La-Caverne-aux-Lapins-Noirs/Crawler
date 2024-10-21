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

  // VERIF DE PARSING SIMPLE
  file = "./res/cast.c";
  assert(s = load_c_file(file, cnf, true));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.active = false;
  p.single_instruction_per_line.active = false;
  if (read_translation_unit(&p, "file", s, &i, true, true) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  s = "int main(void) \n{\n return((double)5/2);\n}";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
