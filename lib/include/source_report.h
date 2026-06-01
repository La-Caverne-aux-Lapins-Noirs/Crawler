/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
** Pentacle Technologie 2008-2026
** EFRITS SAS 2022-2026
**
** C-C-C CRAWLER!
** Source metrics report.
*/

#ifndef			__SOURCE_REPORT_H__
# define		__SOURCE_REPORT_H__
# include		<stdbool.h>
# include		<stddef.h>
# include		"funcgraph.h"

struct				s_parsing;

typedef enum		e_source_report_instruction
  {
    SOURCE_REPORT_EXPRESSION,
    SOURCE_REPORT_DECLARATION,
    SOURCE_REPORT_BRANCH,
    SOURCE_REPORT_LOOP,
    SOURCE_REPORT_JUMP,
    SOURCE_REPORT_RETURN
  }				t_source_report_instruction;

typedef struct		s_source_function_report
{
  char			name[1024];
  char			file[4096];
  int			start_line;
  int			end_line;
  int			line_count;
  int			instructions;
  int			expressions;
  int			declarations;
  int			branches;
  int			loops;
  int			jumps;
  int			returns;
  int			calls;
  int			cyclomatic;
  int			max_control_depth;
  int			current_control_depth;
}				t_source_function_report;

typedef struct		s_source_report
{
  bool			enabled;
  t_source_function_report *functions;
  size_t		function_count;
  size_t		function_capacity;
  int			current_function;
  int			total_instructions;
  int			total_expressions;
  int			total_declarations;
  int			total_branches;
  int			total_loops;
  int			total_jumps;
  int			total_returns;
  int			total_calls;
  int			total_cyclomatic;
  int			total_complexity_score;
  int			max_control_depth;
  int			total_lines;
}				t_source_report;

void				source_report_enable(t_source_report		*report);
void				source_report_clear(t_source_report		*report);
bool				source_report_begin_function(struct s_parsing	*parsing,
					     const char		*name,
					     const char		*file,
					     int			line);
void				source_report_cancel_function(struct s_parsing	*parsing,
					      int		previous_function,
					      size_t		previous_count);
void				source_report_end_function(struct s_parsing	*parsing,
					   int			previous_function,
					   int			end_line);
void				source_report_add_instruction(struct s_parsing	*parsing,
					      t_source_report_instruction kind);
void				source_report_add_call(struct s_parsing		*parsing);
void				source_report_enter_control(struct s_parsing	*parsing);
void				source_report_leave_control(struct s_parsing	*parsing);
bool				source_report_write(const char			*file,
					    const t_source_report	*report);
bool				source_report_write_with_map(const char		*file,
					     const t_source_report	*report,
					     const t_function_map	*map);

# endif	/*		__SOURCE_REPORT_H__	*/
