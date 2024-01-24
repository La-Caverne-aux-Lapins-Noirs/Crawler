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

  // Correlation nom de fonction - chemin jusqu'au fichier
  p.last_error_id = -1;
  p.function_style.value = SNAKE_CASE;
  p.last_line_marker = 0;
  p.file = "src/color_hair.c";
  assert(compare_file_and_function_name
	 (&p, "color_hair", "void color_hair(void) { return (PINK); }", 5) == 1);
  assert(p.function_matching_path.counter == 0);
  p.file = "wear/miniskirt.c";
  assert(compare_file_and_function_name
	 (&p, "wear_miniskirt", "void wear_miniskirt(int len) {  if (len > 30) return; }", 5) == 1);
  assert(p.function_matching_path.counter == 0);
  p.file = "lock/high/heels.c";
  assert(compare_file_and_function_name
	 (&p, "lock_high_heels", "void lock_high_heels(char*code) { if (h < 18) return; }", 5) == 1);
  assert(p.function_matching_path.counter == 0);
  p.file = "add_glossy_lipstick.c";
  assert(compare_file_and_function_name
	 (&p, "add_glossy_lipstick", "void add_glossy_lipstick() { }", 5) == 1);
  assert(p.function_matching_path.counter == 0);

  p.file = "src/wear_football_clothes.c";
  assert(compare_file_and_function_name
	 (&p, "football_clothes", "void football_clothes() { }", 5) == 1);
  assert(p.function_matching_path.counter == 1);
  p.file = "disobey/auntie.c";
  assert(compare_file_and_function_name
	 (&p, "disobey", "void disobey() { }", 5) == 1);
  assert(p.function_matching_path.counter == 2);
  p.file = "be_free/again_one_day.c";
  assert(compare_file_and_function_name
	 (&p, "be_freeagain_one_day", "void be_freeagain_one_day() { }", 5) == 1);
  assert(p.function_matching_path.counter == 3);
  p.file = "src/complain.c";
  assert(compare_file_and_function_name
	 (&p, "src_complain", "void src_complain() { }", 5) == 1);
  assert(p.function_matching_path.counter == 4);
    
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
