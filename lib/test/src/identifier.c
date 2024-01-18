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

  i = 0; assert(read_identifier(&p, "lel42_", &i, false) == 1);
  i = 0; assert(read_identifier(&p, "42lel_", &i, false) == 0);
  i = 0; assert(read_identifier(&p, "const", &i, false) == 0);
  i = 0; assert(read_identifier(&p, "const", &i, true) == 1);
  i = 0; assert(read_identifier_list(&p, "lel, lol, lul", &i) == 1);
  i = 0; assert(read_identifier_list(&p, "lel, 42, lol", &i) == -1);
  i = 0; assert(read_identifier_list(&p, "", &i) == 0);
  
  TEST_OUTRO();
}
