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
    "int* var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 1;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.ptr_symbol_on_name.counter == 1);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int *var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 1;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int *var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 1;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 1);

  i = 0;
  s =
    "int* var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 1;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int * const var;"
    ;
  load_norm_configuration(&p, cnf);
  p.inbetween_ptr_symbol_space.value = 1;
  p.inbetween_ptr_symbol_space.counter = 0;
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.inbetween_ptr_symbol_space.counter == 0);
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int* const var;"
    ;
  load_norm_configuration(&p, cnf);
  p.inbetween_ptr_symbol_space.value = 1;
  p.inbetween_ptr_symbol_space.counter = 0;
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.inbetween_ptr_symbol_space.counter == 1);
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int* const func(int* const ptr, int *a);"
    ;
  load_norm_configuration(&p, cnf);
  p.inbetween_ptr_symbol_space.value = 1;
  p.inbetween_ptr_symbol_space.counter = 0;
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.inbetween_ptr_symbol_space.counter == 2);
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);
  
  TEST_OUTRO();
}
