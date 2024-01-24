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
  s = "const * int * function_symbol(int a[], double b, ...) {\n"
    "if (i == 42) { puts(\"C'est cool\", 5[2], 'c'); i += 1; } else if (*i == 43) { } else atoi(52);\n"
    "start:\n"
    "while (i < 100) { i = &i + 1; } // the loop\n"
    "do { /* yeah */ i -= 32 + ~j; } while (j != !k);\n"
    "for (i = 50; i < 100; ++i) { break ; continue ; }\n"
    "goto start;\n"
    "return (57 + 1);\n"
    "}\n";
  if (read_function_definition(&p, s, &i) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
