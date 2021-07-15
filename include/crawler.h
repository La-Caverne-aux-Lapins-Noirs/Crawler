/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2020
**
** TechnoCore
*/

#ifndef			__CRAWLER_H__
# define		__CRAWLER_H__
# include		<stdbool.h>
# include		"lapin.h"

# define		SYMBOL_SIZE				127

typedef enum		e_style
  {
    MIXED_CASE,		// THIS_IS_IT
    SNAKE_CASE,		// this_is_it
    CAMEL_CASE,		// thisIsIt
    PASCAL_CASE		// ThisIsIt
  }			t_style;

typedef struct		s_last_function
{
  // /!\ Alligné de la même manière que dans read_storage_class_specifier
  bool			is_typedef;
  bool			is_extern;
  bool			is_static;
  bool			is_auto;
  bool			is_register;
  bool			inside_function;
  bool			inside_variable;
  bool			inside_struct;
  bool			inside_union;
  char			symbol[1024];
}			t_last_function;

typedef struct		s_criteria
{
  bool			active;
  int			position; // unused
  int			value;
  char			_unused2[32 - sizeof(int)];
  int			pts;
  int			counter;
}			t_criteria;

typedef struct		s_string_critera
{
  bool			active;
  int			position;
  char			value[32];
  int			pts;
  int			counter;
}			t_string_criteria;

typedef struct		s_parsing
{
  const char		*file;

  char			new_type[8192][SYMBOL_SIZE + 1];
  size_t		last_new_type;

  char			typedef_stack[16][SYMBOL_SIZE + 1];
  int			typedef_stack_top;

  
  t_last_function	last_declaration;

  const char		*last_error_msg[256];
  int			last_error_id;

  int			maximum_error_points;
  int			nbr_error_points;
  int			nbr_mistakes;
  t_criteria		*criteria;

  // About functions inside files
  t_criteria		function_per_file;
  t_criteria		non_static_function_per_file;
  // About symbols
  t_criteria		function_style;
  t_string_criteria	function_infix;
  t_criteria		local_variable_style;
  t_string_criteria	local_variable_infix;
  t_criteria		global_variable_style;
  t_string_criteria	global_variable_infix;
  t_criteria		struct_style;
  t_string_criteria	struct_infix;
  t_criteria		enum_style;
  t_string_criteria	enum_infix;
  t_criteria		enum_constant_style;
  t_string_criteria	enum_constant_infix;
  t_criteria		union_style;
  t_string_criteria	union_infix;
  t_criteria		struct_attribute_style;
  t_string_criteria	struct_attribute_infix;
  t_criteria		union_attribute_style;
  t_string_criteria	union_attribute_infix;
  t_criteria		function_pointer_attribute_style;
  t_string_criteria	function_pointer_attribute_infix;
  t_criteria		function_pointer_type_style;
  t_string_criteria	function_pointer_type_infix;
  t_criteria		typedef_style;
  t_string_criteria	typedef_infix;
  t_criteria		typedef_matching;

  //
  t_criteria		function_matching_path;
  t_criteria		local_variable_inline_init_forbidden;
  
  t_criteria		end[0];
  
  // Configuration
  /* AJOUTER AU FUR ET A MESURE
  bool			global_symbol_align;		// Align symbols in file
  bool			local_symbol_align;		// Align symbols of functions
  int			indentation_size;		// To check alignment
  int			preproc_indent_size;
  int			symbol_align_cost;		// If not, how many error points I get
  int			misindent_cost;
  bool			bad_align_flag;
  bool			bad_indent_flag;

  bool			tab_indent;			// Tab or space?
  int			tab_indent_cost;
  bool			bad_indent_token_flag;

  bool			header_macro;			// Intestable du fait de gcc -E
  int			header_macro_cost;

  bool			single_non_static_function;	// Only one function is not static.
  int			single_non_static_function_cost;
  bool			func_file_matching;		// The only non static func is named after file
  int			func_file_matching_cost;	// but the directory name can be used as prefix

  bool			function_braces_alone;		// if ()\n{\nxxx\n}\n etc.
  int			function_braces_alone_cost;

  bool			function_braces_inline;		// if () { \n xxx \n } else { \n etc.
  int			function_braces_inline_cost;

  bool			avoid_braces_if_possible;
  int			avoid_braces_if_possible_cost;

  int			max_functions_per_file;
  int			max_functions_per_file_cost;

  int			max_parameters_per_function;
  int			max_parameters_per_function_cost;

  int			max_lines_per_function;
  int			max_lines_per_function_cost;

  int			max_columns_per_line;
  int			max_columns_per_line_cost;

  bool			break_forbidden;
  int			break_cost;
  bool			space_after_break;
  int			space_after_break_cost;

  bool			goto_forbidden;
  int			goto_cost;

  bool			continue_forbidden;
  int			continue_cost;
  bool			space_after_continue;
  int			space_after_continue_cost;

  bool			not_terminating_return_forbidden;
  int			not_terminating_return_cost;
  bool			space_after_return;
  int			space_after_return_cost;

  int			maximum_if;			// In a single function, of course
  int			maximum_if_cost;

  bool			while_forbidden;
  int			while_cost;

  bool			for_forbidden;
  int			for_cost;

  bool			do_forbidden;
  int			do_cost;

  bool			switch_forbidden;
  int			switch_cost;

  char			header[512];
  int			header_cost;

  bool			declare_init_variable_forbidden;	// int i = 42;
  int			declare_init_variable_cost;

  int			max_declare_per_line_forbidden;		// int i, j;
  int			max_delcare_per_line_cost;

  int			max_statement_per_line;			// Max one ; and no operator ,
  int			max_statement_per_line_cost;

  bool			space_around_unary_operator;		// 1 + 2 + 3
  int			space_around_unary_operator_cost;

  bool			space_around_parenthesis;		// (4 + 5) / ( 4 + 5 )
  int			space_around_parenthesis_cost;

  bool			space_after_comma;			// 1, 2, 3 / 1,2,3
  int			space_after_comma_cost;

  bool			no_trailing_whitespace;
  int			trailing_whitespace_cost;

  bool			empty_line_at_the_end;
  int			no_empty_line_cost;

  bool			variable_at_beginning;
  int			variable_at_beginning_cost;

  bool			empty_line_after_declaration;
  int			empty_line_after_declaration_cost;

  bool			no_empty_line_in_function;		// Except after variable declaration...
  int			no_empty_line_in_function_cost;

  bool			unix_text_format;
  int			unix_text_format_cost;

  bool			ptr_symbol_on_name;
  int			ptr_symbol_on_name_cost;

  bool			ptr_symbol_on_type;
  int			ptr_symbol_on_type_cost;

  bool			no_magic_const_val;			// Impossible a faire actuellement
  int			no_magic_const_val_cost;		// du fait de l'utilisation de gcc -E

  int			max_copy_parameter_size;
  int			max_copy_parameter_size_cost;

  bool			all_globals_are_const;
  int			non_const_global_cost;

  char			struct_prefix[8];
  char			struct_suffix[8];
  char			union_prefix[8];
  char			union_suffix[8];
  char			global_prefix[8];
  char			global_suffix[8];
  char			typedef_prefix[8];
  char			typedef_suffix[8];
  bool			typedef_matching;			// typedef struct s_lol {} t_lol;
  int			missing_fix_cost;

  bool			typedef_mandatory;
  int			typedef_mandatory_cost;

  bool			no_short_name;				// except i, j & k
  int			no_short_name_cost;

  t_style		function_style;
  int			function_style_cost;

  t_style		variable_style;
  int			variable_style_cost;

  t_style		macro_style;				// gcc -E empeche de tester ca
  int			macro_style_cost;

  t_style		enum_val_style;
  int			enum_val_style_cost;
  */
}			t_parsing;

bool			read_whitespace(const char		*code,
					ssize_t			*i);
typedef int		t_read(t_parsing			*p,
			       const char			*code,
			       ssize_t				*i);
t_read
  read_function_declaration,
  read_primary_expression,
  read_expression,
  read_postfix_expression,
  read_unary_expression,
  read_cast_expression,
  read_multiplicative_expression,
  read_additive_expression,
  read_shift_expression,
  read_relational_expression,
  read_equality_expression,
  read_and_expression,
  read_exclusive_or_expression,
  read_inclusive_or_expression,
  read_logical_and_expression,
  read_logical_or_expression,
  read_assignment_expression,
  read_expression,
  read_conditional_expression,
  read_constant_expression,
  read_type_qualifier,
  read_type_specifier,
  read_storage_class_specifier,
  read_declaration_specifiers,
  read_direct_abstract_declarator,
  read_abstract_declarator,
  read_parameter_declaration,
  read_parameter_list,
  read_parameter_type_list,
  read_pointer,
  read_direct_declarator,
  read_declarator,
  read_initializer_list,
  read_initializer,
  read_init_declarator,
  read_init_declarator_list,
  read_declaration,
  read_compound_statement,
  read_declaration_list,
  read_statement_list,
  read_statement,
  read_labeled_statement,
  read_expression_statement,
  read_selection_statement,
  read_iteration_statement,
  read_jump_statement,
  read_argument_expression_list,
  read_type_name,
  read_specifier_qualifier_list,
  read_unary_operator,
  read_identifier_list,
  read_external_declaration
  ;

int			read_identifier(t_parsing			*p,
					const char			*code,
					ssize_t				*i,
					bool				kw);

bool			check_style(t_parsing				*p,
				    const char				*context,
				    const char				*symbol,
				    t_criteria				*style,
				    t_string_criteria			*string_fix,
				    const char				*code,
				    int					pos);

int			read_translation_unit(t_parsing			*p,
					      const char		*code,
					      ssize_t			*i,
					      bool			verbose);
void			reset_last_declaration(t_parsing		*f);
void			load_norm_configuration(t_parsing		*p,
						const char		*file,
						t_bunny_configuration	*e);
char			*load_c_file(const char				*file,
				     t_bunny_configuration		*exe);

int			tcpopen(const char				*module_name,
				const char				*cmd,
				char					*out,
				int					*max,
				char					*message,
				size_t					msg_size);

#endif	/*		__CRAWLER_H__					*/
