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

  i = 0; assert(read_primary_expression(&p, "Identifier", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "\"This is it...\n\"", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "42", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "4.2", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "   ", &i) == 0);
  
  TEST_OUTRO();
}
