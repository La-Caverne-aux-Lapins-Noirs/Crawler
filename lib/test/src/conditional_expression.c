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
  s = "i == 42 ? 5 + 1 : 3 - lel";
  if (read_conditional_expression(&p, s, &i) != 1)
    GOTOERROR();
  
  TEST_OUTRO();
}
