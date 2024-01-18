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
  p.last_error_id = -1;
  p.last_new_type = 0;
  load_norm_configuration(&p, cnf);
  s = "const int * function_symbol(int * const *, int [42 + 4], int* (*)(void)) {}";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  
  TEST_OUTRO();
}