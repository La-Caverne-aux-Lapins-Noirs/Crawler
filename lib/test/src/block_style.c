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

  bunny_configuration_setf(cnf, SNAKE_CASE, "GlobalStyle.Value");
  bunny_configuration_setf(cnf, 10, "GlobalStyle.Points");
  bunny_configuration_setf(cnf, "s_", "GlobalInfix[0]");
  bunny_configuration_setf(cnf, "Prefix", "GlobalInfix[1]");
  bunny_configuration_setf(cnf, 8, "GlobalInfix[2]");

  bunny_configuration_setf(cnf, SNAKE_CASE, "StructNameStyle");
  bunny_configuration_setf(cnf, 10, "StructNameStylePts");
  bunny_configuration_setf(cnf, "s_", "StructNameInfix.Value");
  bunny_configuration_setf(cnf, "Prefix", "StructNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "StructNameInfix.Points");

  bunny_configuration_setf(cnf, SNAKE_CASE, "StructAttributeNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "StructAttributeNameStyle.Points");
  bunny_configuration_setf(cnf, "sa_", "StructAttributeNameInfix");
  bunny_configuration_setf(cnf, "Prefix", "StructAttributeNameInfixPosition");
  bunny_configuration_setf(cnf, 8, "StructAttributeNameInfixPts");

  bunny_configuration_setf(cnf, SNAKE_CASE, "EnumNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "EnumNameStyle.Points");
  bunny_configuration_setf(cnf, "E_", "EnumNameInfix");
  bunny_configuration_setf(cnf, 0, "EnumNameInfixPosition");
  bunny_configuration_setf(cnf, 8, "EnumNameInfixPts");

  bunny_configuration_setf(cnf, MIXED_CASE, "EnumConstantStyle.Value");
  bunny_configuration_setf(cnf, 10, "EnumConstantStyle.Points");
  bunny_configuration_setf(cnf, "C_", "EnumConstantInfix.Value");
  bunny_configuration_setf(cnf, "Prefix", "EnumConstantInfix.Position");
  bunny_configuration_setf(cnf, 8, "EnumConstantInfix.Points");

  bunny_configuration_setf(cnf, SNAKE_CASE, "UnionNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "UnionNameStyle.Points");
  bunny_configuration_setf(cnf, "u_", "UnionNameInfix[0]");
  bunny_configuration_setf(cnf, "Suffix", "UnionNameInfix[1]");
  bunny_configuration_setf(cnf, 8, "UnionNameInfix[2]");

  bunny_configuration_setf(cnf, SNAKE_CASE, "UnionAttributeNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "UnionAttributeNameStyle.Points");
  bunny_configuration_setf(cnf, "su_", "UnionAttributeNameInfix.Value");
  bunny_configuration_setf(cnf, 0, "UnionAttributeNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "UnionAttributeNameInfix.Points");

  bunny_configuration_setf(cnf, SNAKE_CASE, "TypedefNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "TypedefNameStyle.Points");
  bunny_configuration_setf(cnf, "t_", "TypedefNameInfix.Value");
  bunny_configuration_setf(cnf, "Prefix", "TypedefNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "TypedefNameInfix.Points");

  bunny_configuration_setf(cnf, 1, "TypedefMatching.Value");
  bunny_configuration_setf(cnf, 3, "TypedefMatching.Points");

  goto HERE;
  
  load_norm_configuration(&p, cnf);
  s =
    " struct s_struct { int sa_a; int sa_b; int sa_c2; }; \n"
    " typedef struct s_lol { int sa_d; int sa_e; int sa_f; } t_lol; \n"
    " union unionu_ { int su_a; int su_b; int su_c; }; \n"
    " typedef union lelu_ { int su_d; int su_e; int su_f; } t_lel; \n"
    " enum e_enum { E_LOL_ENUM, E_LAL_ENUM }; \n"
    " typedef enum e_lal { E_THE_ENUM, E_THA_ENUM } t_lal; \n"
    ;
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.struct_style.counter == 0);
  assert(p.enum_style.counter == 0);
  assert(p.enum_constant_style.counter == 0);
  assert(p.union_style.counter == 0);
  assert(p.struct_attribute_style.counter == 0);
  assert(p.union_attribute_style.counter == 0);
  assert(p.typedef_style.counter == 0);
  
  load_norm_configuration(&p, cnf);
  s =
    " struct sx_STRUCT { int sax_A; int sax__B; int sax_C; }; \n"
    " typedef struct sx_LOL { int sax_D; int sax_E; int sax_F; } tx_LOL; \n"
    " union ux_UNION { int sux_A; int sux_B; int sux_C; }; \n"
    " typedef union ux_LEL { int sux_D; int sux_E; int sux_F; } tx_LEL; \n"
    " enum ex_ENUM { EX_lol_enum, EX_lal_enum }; \n"
    " typedef enum ex_LAL { EXAZERTYUIOPOIUYTREZ, EX_tha_enum } tx_LAL; \n"
    ;
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.struct_style.counter == 2);
  assert(p.enum_style.counter == 2);
  assert(p.enum_constant_style.counter == 4);
  assert(p.union_style.counter == 2);
  assert(p.struct_attribute_style.counter == 6);
  assert(p.union_attribute_style.counter == 6);
  assert(p.typedef_style.counter == 3);
 HERE:
  load_norm_configuration(&p, cnf);
  s =
    " typedef struct s_lol { int sa_d; int sa_e; int sa_f; } t_lolex; \n"
    " typedef union u_lel { int su_d; int su_e; int su_f; } t_lelex; \n"
    " typedef enum e_lat { E_THE_ENUM, E_THA_ENUM } t_latex; \n"
    ;
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.typedef_matching.counter == 3);

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
