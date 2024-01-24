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

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./../include/crawler.h";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
