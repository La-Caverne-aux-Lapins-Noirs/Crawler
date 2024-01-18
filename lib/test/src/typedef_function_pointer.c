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

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  // Un pointeur sur fonction simple
  s = "typedef int (*func)(int a);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  // Un type pointeur sur fonction plus riche
  s = "typedef int (*__compar_fn_t) (const void *aa, const void *bb);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  // Un type pointeur sur fonction qui renvoi un pointeur sur fonction
  s = "typedef int (*)(*__compar_fn_t) (const void *aa, const void *bb)(int a);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  
  TEST_OUTRO();
}
