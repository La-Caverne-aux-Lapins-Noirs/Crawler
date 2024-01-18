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
  s = NULL;
  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);
  file = "./../include/crawler.h";
  cfile = load_c_file(file, cnf, true);
  if (cfile == NULL)
    GOTOERROR();
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR();

  TEST_OUTRO();
}
