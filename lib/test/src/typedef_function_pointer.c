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

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  // Un type pointeur sur fonction qui renvoi un pointeur sur fonction
  
  // Le probleme est que le systeme pense etre dans la liste de paramètre
  // Une fois entré dans *__compar_fnt alors qu'il est en fait dans un autre
  // niveau de pointeur sur fonction
  s = "typedef void (*(*sigf)(int spng))(int song);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  // Un pointeur sur fonction simple
  s = "typedef void (*signalf)(int song);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  // Un type pointeur sur fonction plus riche
  s = "typedef void (*signalf)(int song, float sang, char sing);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
