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

  i = 0; assert(read_jump_statement(&p, "return (5);", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "return ;", &i) == 1);

  i = 0; assert(read_jump_statement(&p, "goto Identifier;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "goto Identifier", &i) == -1);
  i = 0; assert(read_jump_statement(&p, "goto ", &i) == -1);

  i = 0; assert(read_jump_statement(&p, "continue ;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "continue ", &i) == -1);

  i = 0; assert(read_jump_statement(&p, "break ;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "break ", &i) == -1);

  
  TEST_OUTRO();
}
