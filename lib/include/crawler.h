/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2021
**
** C-C-C CRAWLER!
** Configurable C Code Crawler !
** Bloc constitutif du "TechnoCentre", suite logiciel du projet "Pentacle School"
** Vérificateur de confirmité du code (entre autre) niveau style.
*/

#ifndef			__CRAWLER_H__
# define		__CRAWLER_H__
# include		<stdbool.h>
# include		"lapin.h"

// Renvoi 0, permet de s'arreter quand une erreur est rencontrée
int			trace(void);

#define			RETURN(a)				\
  return (p->last_error_msg[++p->last_error_id + trace()] = (a " (" STRINGIFY(__LINE__) ")" ), crawler_stop())
# define		SYMBOL_SIZE				127

# ifdef DEACTIVATED
#  define		IZ(p, i)				\
  ((p)->header.value == false || (p)->last_declaration.last_header <= *(i))
# else
#  define		IZ(p, i)				true
# endif

typedef enum		e_style
  {
    MIXED_CASE,		// THIS_IS_IT
    SNAKE_CASE,		// this_is_it
    CAMEL_CASE,		// thisIsIt
    PASCAL_CASE		// ThisIsIt
  }			t_style;

typedef enum		e_coding_style
  {
    GNU_STYLE,
    ALLMAN_STYLE,
    KNR_STYLE
  }			t_coding_style;

typedef struct		s_type
{
  char			name[SYMBOL_SIZE + 1];
  int			size;
}			t_type;

typedef struct		s_last_function
{
  // /!\ Alligné de la même manière que dans read_storage_class_specifier
  bool			is_typedef;
  bool			is_extern;
  bool			is_static;
  bool			is_auto;
  bool			is_register;
  bool			is_const;
  bool			is_restrict;
  bool			is_volatile;
  bool			inside_function;
  bool			inside_function_name;
  bool			inside_variable;
  bool			inside_struct;
  bool			inside_union;
  bool			inside_for_statement;
  bool			inside_parameter;
  bool			after_statement;
  char			last_type[1024];
  char			symbol[1024];
  char			function[1024];
  int			nbr_if;
  int			nbr_variable;
  int			indent_depth;
  int			depth_bonus;
  int			last_line;
  int			last_char;
  int			nbr_pointer;
  int			ptr_acc;
  int			last_type_size;
  int			cumulated_attribute_size;
  int			scope_depth;
  int			scope_length;
  bool			was_defining;
  bool			was_named;
  t_type		copied_parameters[16]; // 16, ca devrait aller...
  int			nbr_copied_parameters;
  int			last_header;
  int			header_line;
  int			end_of_declaration;
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
  t_bunny_configuration	*configuration;

  t_type		new_type[8192];
  size_t		last_new_type;

  char			typedef_stack[16][SYMBOL_SIZE + 1];
  int			typedef_stack_top;

  t_last_function	last_declaration;

  // Etait dans last_declaration, mais doivent survivre au reset...
  int			local_symbol_alignment;
  int			local_parameter_type_alignment;
  int			local_parameter_name_alignment;
  int			global_parameter_name_alignment;
  int			global_symbol_alignment;
  int			ldec_function_per_file;
  int			ldec_non_static_function_per_file;

  const char		*last_error_msg[256];
  int			last_error_id;

  int			maximum_error_points;
  int			nbr_error_points;
  int			nbr_mistakes;
  char			header_data[4096];
  int			last_line_marker;
  int			last_line_marker_line;
  bool			ansi_c;
  t_criteria		start[0];

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
  t_criteria		function_matching_path; // ERROR TEST
  t_criteria		local_variable_inline_init_forbidden;

  // Indentation
  t_criteria		indent_style; // 0: GNU, 1: BSD, 2: K&R
  t_criteria		base_indent; // taille de l'indentation
  t_criteria		tab_or_space; // 0: espace, autre: taille de la tabulation
  t_criteria		symbol_alignment; // noms de fonctions, structures, tdef et variables
  t_criteria		parameter_type_alignment;
  t_criteria		parameter_name_alignment;
  t_criteria		file_parameter_name_alignment;
  t_criteria		file_symbol_alignment;

  // Capacité de fonctions
  t_criteria		single_instruction_per_line;
  t_criteria		max_column_width;
  t_criteria		max_function_length;
  t_criteria		max_parameter;
  t_criteria		only_by_reference;
  t_criteria		always_braces;
  t_criteria		avoid_braces;
  t_criteria		maximum_scope_length; // A FAIRE ============

  // Espaces et sauts de lignes
  t_criteria		declaration_statement_separator;
  t_criteria		no_empty_line_in_function;
  t_criteria		no_trailing_whitespace;
  t_criteria		no_space_inside_parenthesis;
  t_criteria		space_after_statement;
  t_criteria		space_around_binary_operator;
  t_criteria		space_after_comma;
  t_criteria		newline_before_logic; // A FAIRE ========== (42)

  // Autre trucs...
  t_string_criteria	header;
  t_criteria		ptr_symbol_on_name;
  t_criteria		ptr_symbol_on_type;
  t_criteria		inbetween_ptr_symbol_space;
  t_criteria		all_globals_are_const;
  t_criteria		no_magic_value;
  t_criteria		no_short_name;
  t_criteria		maximum_variable;
  t_criteria		no_global;
  t_criteria		return_parenthesis;
  t_criteria		sizeof_parenthesis;

  // Mot clefs et operateurs
  t_criteria		for_forbidden;
  t_criteria		while_forbidden;
  t_criteria		do_while_forbidden;
  t_criteria		goto_forbidden;
  t_criteria		return_forbidden;
  t_criteria		break_forbidden;
  t_criteria		continue_forbidden;
  t_criteria		maximum_if_in_function;
  t_criteria		else_forbidden;
  t_criteria		switch_forbidden;
  t_criteria		ternary_forbidden;
  t_criteria		inline_mod_forbidden;
  t_criteria		end[0];
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
  read_external_declaration,
  read_assembler,
  read_gcc_attribute,
  read_gcc_attribute_list_node
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
					      const char		*file,
					      const char		*code,
					      ssize_t			*i,
					      bool			verbose);
void			reset_last_declaration(t_parsing		*f);
void			load_norm_configuration(t_parsing		*p,
						t_bunny_configuration	*e);
char			*load_c_file(const char				*file,
				     t_bunny_configuration		*exe,
				     bool				preprocessed);

int			tcpopen(const char				*module_name,
				const char				*cmd,
				char					*out,
				int					*max,
				char					*message,
				size_t					msg_size);

bool			add_warning(t_parsing				*p,
				    bool				real,
				    const char				*code,
				    int					pos,
				    int					*cnt,
				    const char				*fmt,
				    ...);

int			store_real_typename(t_parsing			*p,
					    char			*target,
					    const char			*symbol,
					    int				len,
					    int				typ);
int			compare_file_and_function_name(t_parsing	*p,
						       const char	*func,
						       const char	*code,
						       int		pos);

int			check_base_indentation(t_parsing		*p,
					       const char		*code,
					       int			pos);
int			check_is_alone(t_parsing			*p,
				       const char			*tok,
				       const char			*code,
				       int				pos);
int			check_trailing_whitespace(t_parsing		*p,
						  const char		*code);
int			check_no_empty_line(t_parsing			*p,
					    const char			*code,
					    int				pos,
					    int				separator,
					    int				begin,
					    int				end);
int			check_line_width(t_parsing			*p,
					 const char			*code,
					 int				begin,
					 int				end);

int			check_function_length(t_parsing			*p,
					      const char		*code,
					      int			begin,
					      int			end);
bool			check_white_then_newline(t_parsing		*p,
						 const char		*code,
						 int			pos,
						 bool			statement);
int			write_line_and_position(const char		*code,
						int			pos,
						char			*buf,
						size_t			len,
						bool			position);
void			print_line_and_position(t_parsing		*p,
						const char		*code,
						int			pos,
						bool			position);
void			write_indent(t_parsing				*p);
bool			read_whitespace(const char			*code,
					ssize_t				*i);
int			check_single_space(t_parsing			*p,
					   const char			*code,
					   int				pos);
int			check_no_space_before_space_after(t_parsing	*p,
							  const char	*code,
							  int		pos);
int			check_one_space_around(t_parsing		*p,
					       const char		*code,
					       ssize_t			pos,
					       int			toklen,
					       int			val,
					       int			*cnt);

void			full_write_with_arrow(t_parsing			*p,
					      const char		*code,
					      int			pos);
bool			check_pointer_star_position(t_parsing		*p,
						    const char		*code,
						    int			pos);
bool			check_parenthesis_space(t_parsing		*p,
						const char		*code,
						int			pos,
						char			direction,
						int			*cnt);
bool			check_last_parameter_is_reference(t_parsing	*p,
							  const char	*code,
							  int		pos);

bool			add_new_type(t_parsing				*p,
				     const char				*sym,
				     int				size);

bool			check_header(t_parsing				*p,
				     const char				*code);

int			count_to_new_line(t_parsing			*p,
					  const char			*code,
					  int				pos);

bool			check_on_same_line(t_parsing			*p,
					   const char			*code,
					   int				i,
					   const char			*tok,
					   bool				right);

// A appeler pour verifier le code non préprocéssé
int			check_header_file(t_parsing			*p,
					  const char			*code);
int			check_all_lines_width(t_parsing			*p,
					      const char		*code);

int			crawler_stop(void);

#endif	/*		__CRAWLER_H__					*/
