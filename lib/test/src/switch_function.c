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
  s =
    "int global = 42;\n"
    "int ** global2 = 42;\n"
    "void switch_function() {\n"
    "switch (i + 2 - sizeof(*lol)) {\n"
    "case Symbol: puts('e' + sizeof(*lol));\n"
    "case OtherSymbol: strtol(57 + sizeof(lol));\n"
    "default: { return (0); }\n"
    "}\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  
  TEST_OUTRO();
}

