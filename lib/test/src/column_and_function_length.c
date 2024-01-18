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

  // Le stockage d'un symbole décoré dans un buffer
  p.last_error_id = -1;
  // A faire, mais la j'ai la flemme de faire ca, je préfère faire un autre axe d'evaluation

  p.max_column_width.counter = 0;
  p.max_column_width.value = 10;
  assert(check_line_width(&p, "12345\n123\n123456789", 0, 19) == 1);
  assert(p.max_column_width.counter == 0);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 8;
  assert(check_line_width(&p, "\n12345\n123\n123456789", 0, 20) == 1);
  assert(p.max_column_width.counter == 1);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 8;
  assert(check_line_width(&p, "12345\n123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 0);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 4;
  assert(check_line_width(&p, "12345\n123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 1);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 4;
  assert(check_line_width(&p, "123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 0);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 2;
  assert(check_line_width(&p, "123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 1);
  p.max_column_width.counter = 0;
  p.max_column_width.value = 0;

  s = "void func(void) {\n  int i;\n  i = 0;\n}\n";
  p.max_function_length.counter = 0;
  p.max_function_length.value = 5;
  assert(check_function_length(&p, s, 17, 36) == 1);
  assert(p.max_function_length.counter == 0);
  p.max_function_length.counter = 0;
  p.max_function_length.value = 1;
  assert(check_function_length(&p, s, 17, 36) == 1);
  assert(p.max_function_length.counter == 1);

  i = 0;
  p.last_new_type = 0;
  p.last_error_id = -1;
  p.max_function_length.counter = 0;
  p.max_function_length.value = 5;
  p.max_function_length.active = true;
  if (read_translation_unit(&p, file, s, &i, true, false) == -1)
    GOTOERROR();
  assert(p.max_function_length.counter == 0);

  i = 0;
  p.last_line_marker = 0;
  p.last_new_type = 0;
  p.last_error_id = -1;
  p.max_function_length.counter = 0;
  p.max_function_length.value = 1;
  p.max_function_length.active = true;
  if (read_translation_unit(&p, file, s, &i, true, false) == -1)
    GOTOERROR();
  assert(p.max_function_length.counter == 1);
  
  TEST_OUTRO();
}

