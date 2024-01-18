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

  cnf = bunny_new_configuration();
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  int i;\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.value = 2;
  p.tab_or_space.value = 8;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "\t  a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.tab_or_space.value = 8;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);

  // Des espaces avant tabulation...
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "    \t  a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.tab_or_space.value = 8;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "if (a)\n"
    "{\n"
    "a = 2;\n"
    "if (b)\n"
    "{\n"
    "a = 4;\n"
    "}\n"
    "}\n"
    "}\n"
    ;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 8);

  // Maintenant on interdit tabulation
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "\t  a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.tab_or_space.value = 0;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "          a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.base_indent.counter = 0;
  p.tab_or_space.value = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);

  //////////////////////////////////////
  // On verifie l'indentation basique //
  //////////////////////////////////////
  p.base_indent.active = true;
  p.base_indent.value = 2;
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;

  //////////// STYLE GNU
  file = "./res/abs.c";
  assert(s = load_c_file(file, cnf, false));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);

  file = "./res/gnu.c";
  assert(s = load_c_file(file, cnf, false));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = ALLMAN_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter != 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = KNR_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter != 0);

  //////////// STYLE BSD
  p.base_indent.active = true;
  p.base_indent.value = 2;
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;
  file = "./res/bsd.c";
  assert(s = load_c_file(file, cnf, false));

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = ALLMAN_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, file, s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, file, s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter != 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = KNR_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, file, s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter != 0);

  //////////// STYLE K&R
  p.base_indent.active = true;
  p.base_indent.value = 2;
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;

  file = "./res/knr.c";
  assert(s = load_c_file(file, cnf, false));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter != 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = ALLMAN_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter != 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = KNR_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.base_indent.counter == 0);

  ////////////////////////////////////////////
  // On verifie l'indentation intermediaire //
  ////////////////////////////////////////////
  load_norm_configuration(&p, cnf);

  // On aligne tout.
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;
  p.symbol_alignment.active = true;
  p.symbol_alignment.value = 1;
  p.parameter_type_alignment.active = true;
  p.parameter_type_alignment.value = 1;
  p.parameter_name_alignment.active = true;
  p.parameter_name_alignment.value = 1;
  p.file_symbol_alignment.active = true;
  p.file_symbol_alignment.value = 1;
  p.file_parameter_name_alignment.active = true;
  p.file_parameter_name_alignment.value = 1;

  i = 0;
  p.last_error_id = -1;
  file = "./res/classic.c";
  assert(s = load_c_file(file, cnf, false));
  p.symbol_alignment.counter = 0;
  p.parameter_type_alignment.counter = 0;
  p.parameter_name_alignment.counter = 0;
  p.file_symbol_alignment.counter = 0;
  p.file_parameter_name_alignment.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.symbol_alignment.counter != 0);
  assert(p.symbol_alignment.counter != 0);
  assert(p.parameter_name_alignment.counter != 0);
  assert(p.file_symbol_alignment.counter != 0);
  assert(p.file_parameter_name_alignment.counter != 0);

  // Indentation des symboles hors paramètre
  i = 0;
  p.last_error_id = -1;
  file = "./res/local.c";
  assert(s = load_c_file(file, cnf, false));

  p.symbol_alignment.counter = 0;
  p.parameter_type_alignment.counter = 0;
  p.parameter_name_alignment.counter = 0;
  p.file_symbol_alignment.counter = 0;
  p.file_parameter_name_alignment.counter = 0;

  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.symbol_alignment.counter == 0);
  assert(p.parameter_type_alignment.counter == 0);
  assert(p.parameter_name_alignment.counter == 0);
  assert(p.file_symbol_alignment.counter != 0);
  assert(p.file_parameter_name_alignment.counter != 0);

  // Indentation des symboles
  i = 0;
  p.last_error_id = -1;
  file = "./res/global.c";
  assert(s = load_c_file(file, cnf, false));
  p.symbol_alignment.counter = 0;
  p.parameter_type_alignment.counter = 0;
  p.parameter_name_alignment.counter = 0;
  p.file_symbol_alignment.counter = 0;
  p.file_parameter_name_alignment.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.symbol_alignment.counter == 0);
  assert(p.parameter_type_alignment.counter == 0);
  assert(p.parameter_name_alignment.counter == 0);
  assert(p.file_symbol_alignment.counter == 0);
  assert(p.file_parameter_name_alignment.counter == 0);
  
  TEST_OUTRO();
}
