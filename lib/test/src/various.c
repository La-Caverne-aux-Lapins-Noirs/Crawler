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
    "typedef long u_int64_t;\n"
    "typedef int register_t __attribute__ ((__mode__ (__word__)));\n"
    ;
  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();

  i = 0;
  s =
    "typedef unsigned int size_t;\n"
    "extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__ , __leaf__)) ;"
    ;
  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();

  i = 0;
  s =
    "typedef int (*func)(int a);\n"
    "typedef _Bool (*t_bunny_read_whitespace)(const char *code, unsigned long long int *i);\n"
    "extern t_bunny_read_whitespace gl_bunny_read_whitespace;\n"
    ;
  p.last_error_id = -1;
  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  
  // Les prototypes ont été considérés comme des globales
  // a un moment du développement, je debug et ce test
  // servira a assurer que c'est bon.
  i = 0;
  s =
    "\n"
    "double efpow(double x, double y);\n"
    "\n"
    ;
  bunny_configuration_setf(cnf, true, "AllGlobalsAreConst.Value");
  bunny_configuration_setf(cnf, 2, "AllGlobalsAreConst.Points");
  load_norm_configuration(&p, cnf);
  assert(p.all_globals_are_const.counter == 0);
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.all_globals_are_const.counter == 0);

  file = "./res/abs3.c";
  assert(s = load_c_file(file, cnf, false));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  p.single_instruction_per_line.active = true;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);
  assert(p.single_instruction_per_line.counter == 0);

  // VERIF DE PARSING SIMPLE
  file = "./res/memset.c";
  assert(s = load_c_file(file, cnf, true));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.active = false;
  p.single_instruction_per_line.active = false;
  if (read_translation_unit(&p, "file", s, &i, true, true) != 1)
    GOTOERROR();

  file = "./res/gergios.c";
  assert(s = load_c_file(file, cnf, true));
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.ansi_c = false;
  if (read_translation_unit(&p, "file", s, &i, true, true) != 1)
    GOTOERROR();

  i = 0;
  file = "./res/gergios.c";
  assert(cnf = bunny_open_configuration("../../bin/cln.dab", NULL));
  load_norm_configuration(&p, cnf);
  assert(s = load_c_file(file, cnf, false));
  p.last_error_id = -1;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  
  TEST_OUTRO();
}
