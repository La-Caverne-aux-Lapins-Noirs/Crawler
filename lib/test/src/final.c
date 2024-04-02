/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2022
** EFRITS SAS 2021-2024
** Pentacle Technologie 2008-2024
**
** TechnoCore
*/

#include		<stdio.h>
#include		"test.h"

int			main(int		argc,
			     char		**argv)
{
  TEST_INTRO(); // LCOV_EXCL_LINE

  /// Test de traversé
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./../include/crawler.h";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  assert(cnf = bunny_open_configuration("../../bin/cln.dab", NULL));
  load_norm_configuration(&p, cnf);

  /// Test de vérification de norme sur crawler.h (Normalement 100% valide)
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE
  if (p.last_error_id != -1)
    {
      fprintf(stderr, "Coding style failure of crawler.h: %d error founds:\n", p.last_error_id);
      for (size_t i = 0; i < p.last_error_id; ++i)
	printf("- %s\n", p.last_error_msg[i]);
    }
  assert(p.last_error_id == -1);

  ////////////////////////////
  // D'autres tests plus faible
  
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./res/putchar.c";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./res/test_print_base2.c";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE
    
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./res/print_base2.c";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./res/test_strlcpy.c";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./res/strlcpy.c";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./res/strrchr.c";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
