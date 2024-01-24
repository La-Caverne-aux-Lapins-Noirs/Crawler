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

  p.indent_style.value = KNR_STYLE;
  p.base_indent.counter = 0;
  assert(check_on_same_line(&p, "    {    ", 2, "{", true));
  assert(p.base_indent.counter == 0);
  assert(check_on_same_line(&p, "   \n{\n   ", 2, "{", true));
  assert(p.base_indent.counter == 1);

  p.base_indent.counter = 0;
  assert(check_on_same_line(&p, "    }    ", 7, "}", false));
  assert(p.base_indent.counter == 0);
  assert(check_on_same_line(&p, "   \n}\n   ", 7, "}", false));
  assert(p.base_indent.counter == 1);
  p.indent_style.value = 0;
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
