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
  s = "int main(void) { return sizeof 47; }";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.return_parenthesis.active = false;
  p.return_parenthesis.value = 0;
  p.sizeof_parenthesis.active = false;
  p.sizeof_parenthesis.value = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.return_parenthesis.counter == 0);
  assert(p.sizeof_parenthesis.counter == 0);

  i = 0;
  s = "int main(void) { return sizeof 47; }";
  p.return_parenthesis.active = true;
  p.return_parenthesis.value = 1;
  p.sizeof_parenthesis.active = true;
  p.sizeof_parenthesis.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.return_parenthesis.counter == 1);
  assert(p.sizeof_parenthesis.counter == 1);
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
