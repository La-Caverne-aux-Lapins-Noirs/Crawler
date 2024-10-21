/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2022
** EFRITS SAS 2021-2024
** Pentacle Technologie 2008-2024
**
** TechnoCore
*/

#include		"test.h"
#include		<stdio.h>


int			main(int		argc,
			     char		**argv)
{
  TEST_INTRO(); // LCOV_EXCL_LINE

  // On verifie que les assignations a l'initialisation ne soient pas
  // concerné par l'interdiction d'assigner.
  i = 0;
  s =
    "int i = 3;\nvoid func(void) { int j = 50; double j = 3.6; }"
    ;
  cnf = bunny_new_configuration();
  p.last_error_id = -1;
  load_norm_configuration(&p, cnf);
  p.no_assignment.active = true;
  if (read_translation_unit(&p, "file", s, &i, true, true) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.no_assignment.counter == 0);

  // On verifie que les assignations a l'initialisation ne soient pas
  // concerné par l'interdiction d'assigner.
  i = 0;
  s =
    "void func(void) { int i = 3; i = 6; i -= 2; }"
    ;
  cnf = bunny_new_configuration();
  p.last_error_id = -1;
  load_norm_configuration(&p, cnf);
  p.no_assignment.active = true;
  if (read_translation_unit(&p, "file", s, &i, true, true) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  printf("No assignment Counter : %d\n", p.no_assignment.counter);
  assert(p.no_assignment.counter == 2);

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
