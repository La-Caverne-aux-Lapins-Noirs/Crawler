/* ************************************************ */
/* 08/11/2024 11:33:52 */
/* Keryan HOUSSIN  */
/* Crawler */
/* ************************************************ */

#include		"test.h"
#include		<stdio.h>

int			main(int	argc,
			     char	**argv)
{
  TEST_INTRO();
  
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  file = "./res/test_assert.c";
  assert(cfile = load_c_file(file, cnf, true));
  if (read_translation_unit(&p, file, cfile, &i, true, true) == -1)
    GOTOERROR(); // LCOV_EXCL_LINE

  TEST_OUTRO();
}
