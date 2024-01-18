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

  // Espaces
  i = 0;
  s =
    "typedef struct s_truc\n"
    "{\n"
    "  int a, b;\n"
    "} t_truc;\n"
    "void func(int a, int b)\n"
    "{\n"
    "  int i, j, k;\n"
    "  while (a)\n"
    "    {\n"
    "    }\n"
    "  for (a; b; c)\n"
    "    {\n"
    "    }\n"
    "  do\n"
    "    {\n"
    "    }\n"
    "  while (a);\n"
    "  switch (a)\n"
    "    {\n"
    "    }\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "    }\n"
    "  a += 3 + b[3] * 7 >> 3 & 9 | 4 ^ 2 ? 6 : (5 + 7);\n"
    "  b = c, d;"
    "}\n"
    ;
  load_norm_configuration(&p, cnf);
  p.space_after_statement.active = true;
  p.space_after_statement.value = 1;
  p.space_around_binary_operator.active = true;
  p.space_around_binary_operator.value = 1;
  p.space_after_comma.active = true;
  p.space_after_comma.value = 1;
  p.max_column_width.value = 20;
  p.max_column_width.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  check_all_lines_width(&p, s);
  assert(p.space_after_statement.counter == 0);
  assert(p.space_around_binary_operator.counter == 0);
  assert(p.space_after_comma.counter == 0);
  assert(p.max_column_width.counter == 4);

  i = 0;
  s =
    "typedef struct s_truc\n"
    "{\n"
    "  int a,b;\n"
    "} t_truc;\n"
    "void func(int a,int b)\n"
    "{\n"
    "  int i,j,k;\n"
    "  while(a)\n"
    "    {\n"
    "    }\n"
    "  for(a; b; c)\n"
    "    {\n"
    "    }\n"
    "  do\n"
    "    {\n"
    "    }\n"
    "  while(a);\n"
    "  switch(a)\n"
    "    {\n"
    "    }\n"
    "  if(a)\n"
    "    {\n"
    "      a = 2;\n"
    "    }\n"
    "  a+=3+b[3]*7>>3&9|4^2?6:(5+7);\n"
    "  b=c,d;"
    "}\n"
    ;
  load_norm_configuration(&p, cnf);
  p.space_after_statement.active = true;
  p.space_after_statement.value = 1;
  p.space_around_binary_operator.active = true;
  p.space_around_binary_operator.value = 1;
  p.space_after_comma.active = true;
  p.space_after_comma.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.space_after_statement.counter == 5);
  assert(p.space_around_binary_operator.counter == 11);
  assert(p.space_after_comma.counter == 5);
  
  TEST_OUTRO();
}
