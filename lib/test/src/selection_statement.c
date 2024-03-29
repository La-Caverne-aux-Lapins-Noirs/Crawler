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
  s = "if ((i = new()) == -1) { }";
  if (read_selection_statement(&p, s, &i) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
