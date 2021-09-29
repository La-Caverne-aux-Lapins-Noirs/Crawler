/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2020
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"crawler.h"

int			main(void)
{
  t_parsing		p;
  t_bunny_configuration	*cnf;
  char			*s;
  ssize_t		i;
  char			*file;
  char			*cfile;

  memset(&p, 0, sizeof(p));
  assert(cnf = bunny_new_configuration());
  load_norm_configuration(&p, cnf);
  gl_bunny_read_whitespace = read_whitespace;
  // goto NOW;

  /////////////////////////
  // ON TESTE LE PARSING //
  /////////////////////////

  i = 0;
  s = "      \n\r\t      /* lel \n   */\n    // lol      \n     !";
  assert(read_whitespace(s, &i) == true);
  assert(s[i] == '!');

  i = 0;
  assert(read_whitespace("//    ", &i) == true);

  i = 0;
  assert(read_whitespace("/*    ", &i) == false);

  i = 0; assert(read_identifier(&p, "lel42_", &i, false) == 1);
  i = 0; assert(read_identifier(&p, "42lel_", &i, false) == 0);
  i = 0; assert(read_identifier(&p, "const", &i, false) == 0);
  i = 0; assert(read_identifier(&p, "const", &i, true) == 1);
  i = 0; assert(read_identifier_list(&p, "lel, lol, lul", &i) == 1);
  i = 0; assert(read_identifier_list(&p, "lel, 42, lol", &i) == -1);
  i = 0; assert(read_identifier_list(&p, "", &i) == 0);

  i = 0; assert(read_jump_statement(&p, "return (5);", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "return ;", &i) == 1);

  i = 0; assert(read_jump_statement(&p, "goto Identifier;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "goto Identifier", &i) == -1);
  i = 0; assert(read_jump_statement(&p, "goto ", &i) == -1);

  i = 0; assert(read_jump_statement(&p, "continue ;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "continue ", &i) == -1);

  i = 0; assert(read_jump_statement(&p, "break ;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "break ", &i) == -1);


  i = 0; assert(read_primary_expression(&p, "Identifier", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "\"This is it...\n\"", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "42", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "4.2", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "   ", &i) == 0);

  i = 0; assert(read_postfix_expression(&p, "Identifier.SubIdentifier->FarIdentifier++--", &i) == 1);

  i = 0; assert(read_unary_operator(&p, "&", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "*", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "+", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "-", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "~", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "!", &i) == 1);
  
  i = 0; assert(read_unary_expression(&p, "++--~(const double)sizeof(int)", &i) == 1);

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
  
  i = 0;
  s =
    "register auto unsigned float const volatile /*             */ "
    "* const * identifier, * const id2 = a || b && c | d ^ e & f == g != h <= i >= j < k > l << m >> n "
    "+ o - p * q / r % (int)s;"
    ;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  s = "int var[42] = {0, 1, 2, 3, 4};";
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  assert(read_declaration(&p, "int i", &i) == -1);

  i = 0;
  assert(read_declaration(&p, "int i = ", &i) == -1);

  i = 0;
  assert(read_statement(&p, "puts(\"lel\");", &i) == 1); // une expression

  i = 0;
  s = "i = 42, j = 43";
  if (read_expression(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "i == 42 ? 5 + 1 : 3 - lel";
  if (read_conditional_expression(&p, s, &i) != 1)
    goto Error;

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  load_norm_configuration(&p, cnf);
  s = "const int * function_symbol(int * const *, int [42 + 4], int* (*)(void)) {}";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  s = "const int * function_symbol(int, *);";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  s = "const int * function_symbol(int, *);";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  s = "const bool (*bunny_get_koy_button(int, double))[45 + 2];";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s = "extern int **;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  gl_bunny_read_whitespace = read_whitespace;

  i = 0;
  s = "for (i = 0; i < 10; ++i) { }";
  if (read_iteration_statement(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "if ((i = new()) == -1) { }";
  if (read_selection_statement(&p, s, &i) != 1)
    goto Error;

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
  if (read_function_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "/*\n"
    "**\n"
    "**\n"
    "*/\n"
    "typedef struct s_struct {"
    "int a:35;\n"
    "short;\n"
    "double b;\n"
    "} t_struct;";
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s = "typedef int lel;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "typedef union u_union {"
    "int a:35;\n"
    "short;\n"
    "double b;\n"
    "} t_struct;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "typedef enum e_enum {"
    "CONSTANT,\n"
    "ZOUBIDA = 42,\n"
    "ANOTHER\n"
    "} t_enum;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s = "int main(void);";
  p.last_error_id = -1;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  p.last_error_id = -1;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

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
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s = "typedef void * __timer_t;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "typedef float float_t;\n"
    "typedef double double_t;\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "extern int __fpclassify (double __value) __attribute__ ((__nothrow__ , __leaf__));"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "int func(double, int);"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "extern double frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); "
    "extern double __frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  i = 0;
  s =
    "extern double modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)); "
    "extern double __modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;

  ////////////////////////////
  // ON TESTE LA MOULINETTE //
  ////////////////////////////

  // FINAL:
  // Décompte de fonctions par fichier
  i = 0;
  bunny_configuration_setf(cnf, 2, "FunctionPerFile[0]");
  bunny_configuration_setf(cnf, -3, "FunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = " void prototype(void); int global = 42; void func1(void){ } void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_per_file.active);
  assert(p.function_per_file.counter == 1);
  assert(p.function_per_file.value == 2);
  assert(p.function_per_file.pts == 3);

  // Décompte de fonction non statique
  i = 0;
  bunny_configuration_setf(cnf, 1, "NonStaticFunctionPerFile[0]");
  bunny_configuration_setf(cnf, -1, "NonStaticFunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = "  static void func1(void){ } static void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.non_static_function_per_file.active);
  assert(p.non_static_function_per_file.counter == 0);
  assert(p.non_static_function_per_file.value == 1);
  assert(p.non_static_function_per_file.pts == 1);

  i = 0;
  bunny_configuration_setf(cnf, 1, "NonStaticFunctionPerFile[0]");
  bunny_configuration_setf(cnf, -1, "NonStaticFunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = "  static void func1(void){ } void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.non_static_function_per_file.active);
  assert(p.non_static_function_per_file.counter == 1);
  assert(p.non_static_function_per_file.value == 1);
  assert(p.non_static_function_per_file.pts == 1);

  // Verification des styles des symboles
  /// ON COMMENCE AVEC PREFIX ET MIXED CASE
  i = 0;
  bunny_configuration_setf(cnf, MIXED_CASE, "FunctionNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "FunctionNameStyle.Points");

  bunny_configuration_setf(cnf, "PFX_", "FunctionNameInfix.Value");
  bunny_configuration_setf(cnf, "Prefix", "FunctionNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "FunctionNameInfix.Points");

  load_norm_configuration(&p, cnf);
  s = " void PFX_FUNCTION_NAME(void) { } void PFX_FUNCTION_NAME(void);  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.active);
  assert(p.function_style.value == 0);
  assert(p.function_style.pts == 10);
  assert(p.function_style.counter == 0);
  assert(p.function_infix.active);
  assert(strncmp(p.function_infix.value, "PFX_", 4) == 0);
  assert(p.function_infix.pts == 8);
  assert(p.function_infix.counter == 0);

  i = 0;
  s = " void FUNCTION_NAME(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 1);

  i = 0;
  s = " void PFX_FUNCTION__NAME(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void FUNCTION_NAME(void);  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  // Les prototypes ne comptent pas dans le décompte des fonctions mal écrites
  // car on peut prototyper des fonctions externes
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfx_function_name(void) {}  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void pfx_function_name(void);  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.function_infix.counter = 0;
  p.function_style.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  // Comme au dessus, ca ne compte pas quand c'est juste un prototype
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 0);

  i = 0;
  bunny_configuration_setf(cnf, "Suffix", "FunctionNameInfix.Position");
  bunny_configuration_setf(cnf, "_suffix", "FunctionNameInfix.Value");
  bunny_configuration_setf(cnf, SNAKE_CASE, "FunctionNameStyle.Value");
  load_norm_configuration(&p, cnf);
  s = " void function_name_suffix(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfx_function_name(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 1);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfxfunctionname(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 2);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void pfx_function__name(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 3);
  assert(p.function_style.counter == 2);

  i = 0;
  s = " void PFX_FUNCTION_NAME(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_infix.counter == 4);
  assert(p.function_style.counter == 3);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  i = 0;
  bunny_configuration_setf(cnf, CAMEL_CASE, "FunctionNameStyle.Value");
  load_norm_configuration(&p, cnf);
  s = " void theFunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void TheFunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void the_FunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 2);

  i = 0;
  s = " void theZERTYUIOIUYTRERTY(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 3);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  i = 0;
  bunny_configuration_setf(cnf, PASCAL_CASE, "FunctionNameStyle.Value");
  s = " void TheFunctionName42(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void theFunctionName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void TheFunction_Name(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 2);

  i = 0;
  s = " void TheFunctionNNNNNNNNNNNNNNNNNNNName(void) { }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.function_style.counter == 3);

  // Vérification des variables globales
  i = 0;
  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  
  bunny_configuration_setf(cnf, SNAKE_CASE, "GlobalVariableNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "GlobalVariableNameStyle.Points");
  bunny_configuration_setf(cnf, "gl_", "GlobalVariableNameInfix[0]");
  bunny_configuration_setf(cnf, 0, "GlobalVariableNameInfix[1]");
  bunny_configuration_setf(cnf, 8, "GlobalVariableNameInfix[2]");

  bunny_configuration_setf(cnf, MIXED_CASE, "LocalVariableNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "LocalVariableNameStyle.Points");
  bunny_configuration_setf(cnf, "_l", "LocalVariableNameInfix");
  bunny_configuration_setf(cnf, "Suffix", "LocalVariableNameInfixPosition");
  bunny_configuration_setf(cnf, 8, "LocalVariableNameInfixPts");
  bunny_configuration_setf(cnf, 1, "LocalVariableInlineInitForbidden.Value");
  bunny_configuration_setf(cnf, 15, "LocalVariableInlineInitForbidden.Points");

  load_norm_configuration(&p, cnf);
  s = " int gl_value = 50;  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.global_variable_style.active);
  assert(p.global_variable_infix.active);
  assert(p.global_variable_style.value == SNAKE_CASE);
  assert(p.global_variable_style.counter == 0);
  assert(p.global_variable_infix.counter == 0);

  i = 0;
  s = " int value = 50;  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.global_variable_style.counter == 0);
  assert(p.global_variable_infix.counter == 1);

  i = 0;
  s = " int func(void) { int LOCAL_L; LOCAL_L = 52; return 2; }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.local_variable_infix.active);
  assert(p.local_variable_infix.counter == 0);
  assert(p.local_variable_style.counter == 0);

  i = 0;
  s = " int func(void) { int LOCAL_L = 52; return 2; }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.local_variable_infix.active);
  assert(p.local_variable_infix.counter == 0);
  assert(p.local_variable_style.counter == 0);
  assert(p.local_variable_inline_init_forbidden.counter == 1);

  i = 0;
  s = " int func(void) { int local; local = 52; return (2); }  ";
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.local_variable_infix.counter == 1);
  assert(p.local_variable_style.counter == 1);

  /// STRUCT
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
  bunny_configuration_setf(cnf, 3, "TypedefNameStyle.Points");
  
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
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.struct_style.counter == 0);
  assert(p.enum_style.counter == 0);
  assert(p.enum_constant_style.counter == 0);
  assert(p.union_style.counter == 0);
  assert(p.struct_attribute_style.counter == 0);
  assert(p.union_attribute_style.counter == 0);
  assert(p.typedef_style.counter == 0);

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
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.struct_style.counter == 2);
  assert(p.enum_style.counter == 2);
  assert(p.enum_constant_style.counter == 4);
  assert(p.union_style.counter == 2);
  assert(p.struct_attribute_style.counter == 6);
  assert(p.union_attribute_style.counter == 6);
  assert(p.typedef_style.counter == 3);

  load_norm_configuration(&p, cnf);
  s =
    " typedef struct s_lol { int sa_d; int sa_e; int sa_f; } t_lolex; \n"
    " typedef union u_lel { int su_d; int su_e; int su_f; } t_lelex; \n"
    " typedef enum e_lal { E_THE_ENUM, E_THA_ENUM } t_lalex; \n"
    ;
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.typedef_matching.counter == 3);

  cnf = bunny_new_configuration();
  bunny_configuration_setf(cnf, 1, "NoTrailingWhitespace");
  bunny_configuration_setf(cnf, 1, "NoEmptyLineInFunction");
  bunny_configuration_setf(cnf, 1, "SingleInstructionPerLine");
  load_norm_configuration(&p, cnf);
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  int tab[2][2] =\n"
    "    {\n"
    "      {\n"
    "        42,\n"
    "        89\n"
    "      },\n"
    "      {\n"
    "        42,\n"
    "        89\n"
    "      }\n"
    "    };\n"
    "  int i;\n"
    "  i = 42;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_trailing_whitespace.counter == 0);
  assert(p.no_empty_line_in_function.counter == 0);
  assert(p.single_instruction_per_line.counter == 0);

  i = 0;
  s =
    "void func(void) \n"
    "{  \n"
    "  int i, j; \n"
    "\n"
    "  i = 42; j += 1; \n"
    "  i = 42, j += 1; \n"
    "\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_trailing_whitespace.counter == 6);
  assert(p.no_empty_line_in_function.counter == 1);
  assert(p.single_instruction_per_line.counter == 3);
  
  bunny_configuration_setf(cnf, 1, "DeclarationStatementSeparator");
  load_norm_configuration(&p, cnf);
  i = 0;
  s =
    "void prototype(void);\n"
    "void func(void)\n"
    "{\n"
    "  void prototype(void);\n"
    "  int i;\n"
    "\n"
    "  if (lol) i = 0;\n"
    "  if (lol) { i = 0; }\n"
    "  while (lol) i = 0;\n"
    "  while (lol) { i = 0; }\n"
    "  for (lol; lol; lol) i = 0;\n"
    "  for (lol; lol; lol) { i = 0; }\n"
    "  do i = 0; while (lol); i = 0;\n"
    "  do { i = 0; } while (lol); i = 0;\n"
    "  return (1); i = 0;\n"
    "  return; i = 0;\n"
    "  goto start; i = 0;\n"
    "  break; i = 0;\n"
    "  continue; i = 0;\n"
    "  i = 0; return (1);\n"
    "  i = 0; return;\n"
    "  i = 0; goto start;\n"
    "  i = 0; break;\n"
    "  i = 0; continue;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.single_instruction_per_line.counter == 21);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a == b) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  bunny_configuration_setf(cnf, 3, "MaximumIfInFunction.Value");
  bunny_configuration_setf(cnf, 1, "MaximumIfInFunction.Points");
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.maximum_if_in_function.counter == 0);
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "  if (a == b) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.maximum_if_in_function.counter == 2);

  bunny_delete_node(cnf, "MaximumIfInFunction");
  bunny_configuration_setf(cnf, 1, "IfForbidden.Value");
  bunny_configuration_setf(cnf, 1, "IfForbidden.Points");
  load_norm_configuration(&p, cnf);
  p.maximum_if_in_function.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.maximum_if_in_function.counter == 5);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a == b) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.while_forbidden.value = 0;
  p.while_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.while_forbidden.counter == 0);
  p.while_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.while_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  do { } while (a == b);\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.do_while_forbidden.value = 0;
  p.do_while_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.do_while_forbidden.counter == 0);
  p.do_while_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.do_while_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  for (a = 0; a < b; ++a) { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.for_forbidden.value = 0;
  p.for_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.for_forbidden.counter == 0);
  p.for_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.for_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a == b) { } else { }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.else_forbidden.value = 0;
  p.else_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.else_forbidden.counter == 0);
  p.else_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.else_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  switch (a)\n"
    "  {\n"
    "    case A:\n"
    "      break;\n"
    "    case B:\n"
    "      break;\n"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.switch_forbidden.value = 0;
  p.switch_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.switch_forbidden.counter == 0);
  p.switch_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.switch_forbidden.counter == 1);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.break_forbidden.value = 0;
  p.break_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.break_forbidden.counter == 0);
  p.break_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.break_forbidden.counter == 2);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a < b)\n"
    "  {\n"
    "    if (a == c)\n"
    "      continue;"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.continue_forbidden.value = 0;
  p.continue_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.continue_forbidden.counter == 0);
  p.continue_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.continue_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a < b)\n"
    "  {\n"
    "    Lol:\n"
    "    if (a == c)\n"
    "      goto Lol;\n"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.goto_forbidden.value = 0;
  p.goto_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.goto_forbidden.counter == 0);
  p.goto_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.goto_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  while (a < b)\n"
    "  {\n"
    "    if (a == c)\n"
    "      return (b);\n"
    "  }\n"
    "} \n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.return_forbidden.value = 0;
  p.return_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.return_forbidden.counter == 0);
  p.return_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.return_forbidden.counter == 1);
  
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  i = j > 2 ? 4 : 2;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.ternary_forbidden.value = 0;
  p.ternary_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.ternary_forbidden.counter == 0);
  p.ternary_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.ternary_forbidden.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  i++;"
    "  ++i;"
    "  --i;"
    "  i--;"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.inline_mod_forbidden.value = 0;
  p.inline_mod_forbidden.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.inline_mod_forbidden.counter == 0);
  p.inline_mod_forbidden.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.inline_mod_forbidden.counter == 4);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "  {\n"
    "     a = 2;\n"
    "  }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.always_braces.value = 0;
  p.always_braces.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.always_braces.counter == 0);
  p.always_braces.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.always_braces.counter == 0);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "     a = 2;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.always_braces.value = 0;
  p.always_braces.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.always_braces.counter == 0);
  p.always_braces.value = 1;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.always_braces.counter == 1);

  i = 0;
  s =
    "void func(int a, int b, int c)\n"
    "{\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.max_parameter.active = false;
  p.max_parameter.value = 0;
  p.max_parameter.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.max_parameter.counter == 0);
  p.max_parameter.active = true;
  p.max_parameter.value = 4;
  p.max_parameter.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.max_parameter.counter == 0);
  p.max_parameter.active = true;
  p.max_parameter.value = 2;
  p.max_parameter.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.max_parameter.counter == 1);
  p.max_parameter.active = true;
  p.max_parameter.value = 3;
  p.max_parameter.counter = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.max_parameter.counter == 0);

  s =
    "void func(void)\n"
    "{\n"
    "  int i;\n"
    "  int j;\n"
    "}\n"
    ;
  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = false;
  p.maximum_variable.value = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.maximum_variable.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = true;
  p.maximum_variable.value = 3;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.maximum_variable.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = true;
  p.maximum_variable.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.maximum_variable.counter == 1);

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.maximum_variable.active = true;
  p.maximum_variable.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.maximum_variable.counter == 2);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);
  s = "int main(void) { return sizeof 47; }";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.return_parenthesis.active = false;
  p.return_parenthesis.value = 0;
  p.sizeof_parenthesis.active = false;
  p.sizeof_parenthesis.value = 0;
  i = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.return_parenthesis.counter == 0);
  assert(p.sizeof_parenthesis.counter == 0);

  i = 0;
  s = "int main(void) { return sizeof 47; }";
  p.return_parenthesis.active = true;
  p.return_parenthesis.value = 1;
  p.sizeof_parenthesis.active = true;
  p.sizeof_parenthesis.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.return_parenthesis.counter == 1);
  assert(p.sizeof_parenthesis.counter == 1);

  /////////////////
  // INDENTATION //
  /////////////////

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  int i;\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.value = 2;
  p.tab_or_space.value = 8;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 0);
  
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "\t  a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.tab_or_space.value = 8;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 0);

  // Des espaces avant tabulation...
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "    \t  a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.tab_or_space.value = 8;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 1);
  
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "if (a)\n"
    "{\n"
    "a = 2;\n"
    "if (b)\n"
    "{\n"
    "a = 4;\n"
    "}\n"
    "}\n"
    "}\n"
    ;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 8);

  // Maintenant on interdit tabulation
  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "\t  a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.tab_or_space.value = 0;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 1);

  i = 0;
  s =
    "void func(void)\n"
    "{\n"
    "  if (a)\n"
    "    {\n"
    "      a = 2;\n"
    "      if (b)\n"
    "        {\n"
    "          a = 4;\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.base_indent.active = true;
  p.indent_style.value = 0;
  p.base_indent.value = 2;
  p.base_indent.counter = 0;
  p.tab_or_space.value = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 0);

  i = 0;
  s =
    "void cnt(void)\n"
    "{\n"
    "  int i = 57;\n"
    "  double j = 4.2;\n"
    "  int k;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.no_short_name.active = true;
  p.no_short_name.value = 4;
  p.no_magic_value.active = false;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_short_name.counter == 0);
  assert(p.no_magic_value.counter == 0);

  i = 0;
  s =
    "void foo(void)\n"
    "{\n"
    "  int bar = 57;\n"
    "  int fob = 4.2;\n"
    "  int bao;\n"
    "}\n"
    ;
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.no_short_name.active = true;
  p.no_short_name.value = 4;
  p.no_magic_value.active = true;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_short_name.counter == 4);
  assert(p.no_magic_value.counter == 2);

  i = 0;
  s = "int i = 0;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.all_globals_are_const.active = false;
  p.all_globals_are_const.value = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.all_globals_are_const.counter == 0);

  i = 0;
  s = "int i = 0;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.all_globals_are_const.active = true;
  p.all_globals_are_const.value = 1;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.all_globals_are_const.counter == 1);

  i = 0;
  s = "const int i = 0;";
  p.last_error_id = -1;
  p.last_new_type = 0;
  p.all_globals_are_const.active = true;
  p.all_globals_are_const.value = 1;
  p.all_globals_are_const.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.all_globals_are_const.counter == 0);
  
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
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
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
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.space_after_statement.counter == 5);
  assert(p.space_around_binary_operator.counter == 11);
  assert(p.space_after_comma.counter == 5);

  // Allignement de l'étoile
  i = 0;
  s =
    "int* var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 1;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.ptr_symbol_on_name.counter == 1);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int *var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 1;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int *var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 1;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 1);

  i = 0;
  s =
    "int* var;"
    ;
  load_norm_configuration(&p, cnf);
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 1;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int * const var;"
    ;
  load_norm_configuration(&p, cnf);
  p.inbetween_ptr_symbol_space.value = 1;
  p.inbetween_ptr_symbol_space.counter = 0;
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.inbetween_ptr_symbol_space.counter == 0);
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  i = 0;
  s =
    "int* const var;"
    ;
  load_norm_configuration(&p, cnf);
  p.inbetween_ptr_symbol_space.value = 1;
  p.inbetween_ptr_symbol_space.counter = 0;
  p.ptr_symbol_on_name.value = 0;
  p.ptr_symbol_on_type.value = 0;
  p.ptr_symbol_on_name.counter = 0;
  p.ptr_symbol_on_type.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.inbetween_ptr_symbol_space.counter == 1);
  assert(p.ptr_symbol_on_name.counter == 0);
  assert(p.ptr_symbol_on_type.counter == 0);

  //// Test du passage de paramètre par copie ou par reference
  // On commence par verifier que le compteur de taille de structure est fonctionnel
  // Il est possible que certains types natifs aient été oubliés
  p.last_new_type = 0;
  i = 0;
  s = "typedef int Integer;\n";
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(strcmp(p.new_type[0].name, "Integer") == 0);
  assert(p.new_type[0].size == 4);

  i = 0;
  s = "typedef int * IntPointer;\n";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(strcmp(p.new_type[1].name, "IntPointer") == 0);
  assert(p.new_type[1].size == 8);

  i = 0;
  s =
    "typedef struct s_8 {\n"
    "  int a;\n"
    "  Integer b;\n"
    "} t_8;\n"
    "\n"
    "typedef struct s_24 {\n"
    "  struct s_8 c;\n"
    "  t_8 d;\n"
    "  void *e;\n"
    "} t_24;\n"
    ;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(strcmp(p.new_type[2].name, "s_8") == 0);
  assert(p.new_type[2].size == 8);
  assert(strcmp(p.new_type[3].name, "t_8") == 0);
  assert(p.new_type[3].size == 8);
  assert(strcmp(p.new_type[4].name, "s_24") == 0);
  assert(p.new_type[4].size == 24);
  assert(strcmp(p.new_type[5].name, "t_24") == 0);
  assert(p.new_type[5].size == 24);

  // On ne reset pas le nombre de type afin de profiter de la config au dessus
  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 16;
  p.only_by_reference.counter = 0;
  // Les prototypes ne sont pas censés être vérifiés
  s = "void func(t_24 copy);";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.only_by_reference.counter == 0);

  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 32;
  p.only_by_reference.counter = 0;
  // On est dans les clous, car t_24 est plus petit que 32
  s = "void func(t_24 copy) { return ; }";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.only_by_reference.counter == 0);

  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 16;
  p.only_by_reference.counter = 0;
  // On est dans l'illegalité car 16 est maintenant le max
  s = "void func(t_24 copy) { return ; }";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.only_by_reference.counter == 1);

  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 16;
  p.only_by_reference.counter = 0;
  // On est de nouveau dans la légalité car maintenant on passe par reference
  s = "void func(t_24 *copy) { return ; }";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.only_by_reference.counter == 0);

  // On interdit les globlas
  i = 0;
  p.last_error_id = -1;
  p.no_global.active = false;
  p.no_global.value = 0;
  p.no_global.counter = 0;
  s = "void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_global.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.no_global.active = true;
  p.no_global.value = 1;
  p.no_global.counter = 0;
  s = "void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_global.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.no_global.active = false;
  p.no_global.value = 0;
  p.no_global.counter = 0;
  s = "int j; void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_global.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.no_global.active = true;
  p.no_global.value = 1;
  p.no_global.counter = 0;
  s = "int j; void func(void) { int i; return ; }";
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.no_global.counter == 1);

  p.last_error_id = -1;
  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  bunny_configuration_setf
    (cnf,
     "/*\n"
     "** %FirstName %FamilyName %Nickname %Login <%Mail>\n"
     "** %ProjectName %Year\n"
     "*/\n"
     , "Header");
  bunny_configuration_setf(cnf, "Felina", "FirstName");
  bunny_configuration_setf(cnf, "Rose", "FamilyName");
  bunny_configuration_setf(cnf, "Felinistra", "Nickname");
  bunny_configuration_setf(cnf, "felina.rose", "Login");
  bunny_configuration_setf(cnf, "felina.rose@cln.school", "Mail");
  bunny_configuration_setf(cnf, "CCCCrawler", "ProjectName");
  bunny_configuration_setf(cnf, "2021", "Year");
  load_norm_configuration(&p, cnf);
  i = 0;
  s =
    "\n"
    "typedef __int lol;\n"
    "\n"
    "/*\n"
    "** Felina Rose Felinistra felina.rose <felina.rose@cln.school>\n"
    "** CCCCrawler 2021\n"
    "*/\n"
    "\n"
    "#include <stdio.h>\n"
    "\n"
    "int main(void)\n"
    "{\n"
    "  return (0);\n"
    "}\n"
    ;
  assert(check_header_file(&p, &s[20]));
  assert(p.header.counter == 0);
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.header.counter == 0);

  i = 0;
  s =
    "\n"
    "typedef __int lol;\n"
    "\n"
    "/*\n"
    "** Felina Rose Felinistra    felina.rose <felina.rose@cln.school>\n"
    "** CCCCrawler 2021\n"
    "*/\n"
    "\n"
    "#include <stdio.h>\n"
    "\n"
    "int main(void)\n"
    "{\n"
    "  return (0);\n"
    "}\n"
    ;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  // assert(p.header.counter == 1);

  //////////////////////////////////////
  // On verifie l'indentation basique //
  //////////////////////////////////////
  p.base_indent.active = true;
  p.base_indent.value = 2;
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;

  //////////// STYLE GNU  
  file = "./res/abs.c";
  assert(s = load_c_file(file, cnf, false));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 0);
 
  file = "./res/gnu.c";
  assert(s = load_c_file(file, cnf, false));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = ALLMAN_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter != 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = KNR_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter != 0);
  
  //////////// STYLE BSD
  p.base_indent.active = true;
  p.base_indent.value = 2;
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;
  file = "./res/bsd.c";
  assert(s = load_c_file(file, cnf, false));

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = ALLMAN_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, file, s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, file, s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter != 0);
  
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = KNR_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, file, s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter != 0);

  //////////// STYLE K&R
  p.base_indent.active = true;
  p.base_indent.value = 2;
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;
  
  file = "./res/knr.c";
  assert(s = load_c_file(file, cnf, false));
  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = GNU_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter != 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = ALLMAN_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter != 0);

  i = 0;
  p.last_error_id = -1;
  p.indent_style.value = KNR_STYLE;
  p.base_indent.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.base_indent.counter == 0);

  load_norm_configuration(&p, cnf);

  ////////////////////////////////////////////
  // On verifie l'indentation intermediaire //
  ////////////////////////////////////////////

  // On aligne tout.
  p.tab_or_space.active = true;
  p.tab_or_space.value = 8;
  p.symbol_alignment.active = true;
  p.symbol_alignment.value = 1;
  p.parameter_type_alignment.active = true;
  p.parameter_type_alignment.value = 1;
  p.parameter_name_alignment.active = true;
  p.parameter_name_alignment.value = 1;
  p.file_symbol_alignment.active = true;
  p.file_symbol_alignment.value = 1;
  p.file_parameter_name_alignment.active = true;
  p.file_parameter_name_alignment.value = 1;
  
  i = 0;
  p.last_error_id = -1;
  file = "./res/classic.c";
  assert(s = load_c_file(file, cnf, false));
  p.symbol_alignment.counter = 0;
  p.parameter_type_alignment.counter = 0;
  p.parameter_name_alignment.counter = 0;
  p.file_symbol_alignment.counter = 0;
  p.file_parameter_name_alignment.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.symbol_alignment.counter != 0);
  assert(p.symbol_alignment.counter != 0);
  assert(p.parameter_name_alignment.counter != 0);
  assert(p.file_symbol_alignment.counter != 0);
  assert(p.file_parameter_name_alignment.counter != 0);

  // Indentation des symboles hors paramètre
  i = 0;
  p.last_error_id = -1;
  file = "./res/local.c";
  assert(s = load_c_file(file, cnf, false));
  
  p.symbol_alignment.counter = 0;
  p.parameter_type_alignment.counter = 0;
  p.parameter_name_alignment.counter = 0;
  p.file_symbol_alignment.counter = 0;
  p.file_parameter_name_alignment.counter = 0;

  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.symbol_alignment.counter == 0);
  assert(p.parameter_type_alignment.counter == 0);
  assert(p.parameter_name_alignment.counter == 0);
  assert(p.file_symbol_alignment.counter != 0);
  assert(p.file_parameter_name_alignment.counter != 0);

  // Indentation des symboles
  i = 0;
  p.last_error_id = -1;
  file = "./res/global.c";
  assert(s = load_c_file(file, cnf, false));
  p.symbol_alignment.counter = 0;
  p.parameter_type_alignment.counter = 0;
  p.parameter_name_alignment.counter = 0;
  p.file_symbol_alignment.counter = 0;
  p.file_parameter_name_alignment.counter = 0;
  if (read_translation_unit(&p, "file", s, &i, true) != 1)
    goto Error;
  assert(p.symbol_alignment.counter == 0);
  assert(p.parameter_type_alignment.counter == 0);
  assert(p.parameter_name_alignment.counter == 0);
  assert(p.file_symbol_alignment.counter == 0);
  assert(p.file_parameter_name_alignment.counter == 0);

  /////////////////////////////////
  // GRAND TEST FINAL DE PARSING //
  /////////////////////////////////

  i = 0;
  p.last_error_id = -1;
  p.last_new_type = 0;
  s = NULL;
  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  load_norm_configuration(&p, cnf);
  file = "./../include/crawler.h";
  cfile = load_c_file(file, cnf, true);
  if (cfile == NULL)
    goto Error;
  if (read_translation_unit(&p, file, cfile, &i, true) == -1)
    goto Error;

  ///////////////////////////////////
  // TEST DE FONCTIONS UTILITAIRES //
  ///////////////////////////////////

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

  // Le stockage d'un symbole décoré dans un buffer
  p.last_error_id = -1;
  // A faire, mais la j'ai la flemme de faire ca, je préfère faire un autre axe d'evaluation

  p.max_column_width.counter = 0;
  p.max_column_width.value = 10;
  assert(check_line_width(&p, "12345\n123\n123456789", 0, 19) == 1);
  assert(p.max_column_width.counter == 0);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 8;
  assert(check_line_width(&p, "\n12345\n123\n123456789", 0, 20) == 1);
  assert(p.max_column_width.counter == 1);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 8;
  assert(check_line_width(&p, "12345\n123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 0);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 4;
  assert(check_line_width(&p, "12345\n123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 1);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 4;
  assert(check_line_width(&p, "123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 0);

  p.max_column_width.counter = 0;
  p.max_column_width.value = 2;
  assert(check_line_width(&p, "123\n", 0, 19) == 1);
  assert(p.max_column_width.counter == 1);
  p.max_column_width.counter = 0;
  p.max_column_width.value = 0;

  s = "void func(void) {\n  int i;\n  i = 0;\n}\n";
  p.max_function_length.counter = 0;
  p.max_function_length.value = 5;
  assert(check_function_length(&p, s, 17, 36) == 1);
  assert(p.max_function_length.counter == 0);
  p.max_function_length.counter = 0;
  p.max_function_length.value = 1;
  assert(check_function_length(&p, s, 17, 36) == 1);
  assert(p.max_function_length.counter == 1);

  i = 0;
  p.last_new_type = 0;
  p.last_error_id = -1;
  p.max_function_length.counter = 0;
  p.max_function_length.value = 5;
  p.max_function_length.active = true;
  if (read_translation_unit(&p, file, s, &i, true) == -1)
    goto Error;
  assert(p.max_function_length.counter == 0);

  i = 0;
  p.last_line_marker = 0;
  p.last_new_type = 0;
  p.last_error_id = -1;
  p.max_function_length.counter = 0;
  p.max_function_length.value = 1;
  p.max_function_length.active = true;
  if (read_translation_unit(&p, file, s, &i, true) == -1)
    goto Error;
  assert(p.max_function_length.counter == 1);

  return (EXIT_SUCCESS);

 Error: // LCOV_EXCL_START
  if (s)
    printf("Stopped at %s\nIn:\n%s\n\n", &s[i], s);
  for (int i = 0; i < p.last_error_id; ++i)
    printf("%s\n", p.last_error_msg[i]);
  return (EXIT_FAILURE); // LCOV_EXCL_STOP
}
