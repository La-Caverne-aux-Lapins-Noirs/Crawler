/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
** Pentacle Technologie 2008-2026
** EFRITS SAS 2022-2026
**
** C-C-C CRAWLER!
** Source metrics report.
*/

#ifndef			_POSIX_C_SOURCE
# define		_POSIX_C_SOURCE					200809L
#endif
#include		<stdarg.h>
#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<sys/types.h>
#include		<unistd.h>
#include		"crawler.h"

static void		safe_copy(char					*target,
				  size_t				 size,
				  const char				*source)
{
  size_t		len;

  if (size == 0)
    return ;
  if (source == NULL)
    source = "";
  len = strlen(source);
  if (len >= size)
    len = size - 1;
  memcpy(target, source, len);
  target[len] = '\0';
}

static int		source_report_local_score(const t_source_function_report *function)
{
  if (function == NULL)
    return (0);
  return (function->instructions +
	  2 * function->branches +
	  3 * function->loops +
	  4 * function->max_control_depth +
	  2 * function->jumps +
	  2 * function->returns +
	  function->calls);
}

static const char	*source_report_complexity_class(int score)
{
  if (score < 10)
    return ("Low");
  if (score < 25)
    return ("Medium");
  if (score < 50)
    return ("High");
  return ("VeryHigh");
}

static void		source_report_reset_totals(t_source_report	*report)
{
  report->total_instructions = 0;
  report->total_expressions = 0;
  report->total_declarations = 0;
  report->total_branches = 0;
  report->total_loops = 0;
  report->total_jumps = 0;
  report->total_returns = 0;
  report->total_calls = 0;
  report->total_cyclomatic = 0;
  report->total_complexity_score = 0;
  report->max_control_depth = 0;
  report->total_lines = 0;
}

static void		source_report_recompute_totals(t_source_report	*report)
{
  source_report_reset_totals(report);
  for (size_t i = 0; i < report->function_count; ++i)
    {
      report->total_instructions += report->functions[i].instructions;
      report->total_expressions += report->functions[i].expressions;
      report->total_declarations += report->functions[i].declarations;
      report->total_branches += report->functions[i].branches;
      report->total_loops += report->functions[i].loops;
      report->total_jumps += report->functions[i].jumps;
      report->total_returns += report->functions[i].returns;
      report->total_calls += report->functions[i].calls;
      report->total_cyclomatic += report->functions[i].cyclomatic;
      report->total_complexity_score +=
	source_report_local_score(&report->functions[i]);
      if (report->functions[i].max_control_depth > report->max_control_depth)
	report->max_control_depth = report->functions[i].max_control_depth;
      report->total_lines += report->functions[i].line_count;
    }
}

void			source_report_enable(t_source_report		*report)
{
  if (report == NULL)
    return ;
  report->enabled = true;
  report->current_function = -1;
}

void			source_report_clear(t_source_report		*report)
{
  if (report == NULL)
    return ;
  free(report->functions);
  memset(report, 0, sizeof(*report));
  report->current_function = -1;
}

static bool		source_report_grow(t_source_report		*report)
{
  t_source_function_report *nfunctions;
  size_t		ncapacity;

  if (report->function_count < report->function_capacity)
    return (true);
  ncapacity = report->function_capacity ? report->function_capacity * 2 : 16;
  if ((nfunctions = realloc(report->functions,
				    ncapacity * sizeof(*nfunctions))) == NULL)
    return (false);
  report->functions = nfunctions;
  report->function_capacity = ncapacity;
  return (true);
}

bool			source_report_begin_function(t_parsing		*parsing,
						     const char		*name,
						     const char		*file,
						     int		line)
{
  t_source_report	*report;
  t_source_function_report *function;

  if (parsing == NULL)
    return (true);
  report = &parsing->source_report;
  if (!report->enabled)
    return (true);
  if (!source_report_grow(report))
    return (false);
  function = &report->functions[report->function_count];
  memset(function, 0, sizeof(*function));
  safe_copy(function->name, sizeof(function->name), name);
  safe_copy(function->file, sizeof(function->file), file);
  function->start_line = line;
  function->end_line = line - 1;
  function->line_count = 0;
  function->cyclomatic = 1;
  report->total_cyclomatic += 1;
  report->current_function = (int)report->function_count;
  report->function_count += 1;
  return (true);
}

void			source_report_cancel_function(t_parsing		*parsing,
						      int		 previous_function,
						      size_t		 previous_count)
{
  t_source_report	*report;

  if (parsing == NULL)
    return ;
  report = &parsing->source_report;
  if (!report->enabled)
    return ;
  if (previous_count < report->function_count)
    report->function_count = previous_count;
  report->current_function = previous_function;
  source_report_recompute_totals(report);
}


void			source_report_end_function(t_parsing		*parsing,
					   int			 previous_function,
					   int			 end_line)
{
  t_source_report	*report;
  t_source_function_report *function;

  if (parsing == NULL || !parsing->source_report.enabled)
    return ;
  report = &parsing->source_report;
  if (report->current_function >= 0 &&
      (size_t)report->current_function < report->function_count)
    {
      function = &report->functions[report->current_function];
      report->total_lines -= function->line_count;
      if (end_line < function->start_line)
	{
	  function->end_line = function->start_line - 1;
	  function->line_count = 0;
	}
      else
	{
	  function->end_line = end_line;
	  function->line_count = end_line - function->start_line + 1;
	}
      report->total_lines += function->line_count;
    }
  report->current_function = previous_function;
}

void			source_report_add_instruction(t_parsing		*parsing,
						      t_source_report_instruction kind)
{
  t_source_report	*report;
  t_source_function_report *function;

  if (parsing == NULL)
    return ;
  report = &parsing->source_report;
  if (!report->enabled || report->current_function < 0 ||
      (size_t)report->current_function >= report->function_count)
    return ;
  function = &report->functions[report->current_function];
  function->instructions += 1;
  report->total_instructions += 1;
  report->total_complexity_score += 1;
  if (kind == SOURCE_REPORT_EXPRESSION)
    {
      function->expressions += 1;
      report->total_expressions += 1;
    }
  else if (kind == SOURCE_REPORT_DECLARATION)
    {
      function->declarations += 1;
      report->total_declarations += 1;
    }
  else if (kind == SOURCE_REPORT_BRANCH)
    {
      function->branches += 1;
      report->total_branches += 1;
      report->total_complexity_score += 2;
      function->cyclomatic += 1;
      report->total_cyclomatic += 1;
    }
  else if (kind == SOURCE_REPORT_LOOP)
    {
      function->loops += 1;
      report->total_loops += 1;
      report->total_complexity_score += 3;
      function->cyclomatic += 1;
      report->total_cyclomatic += 1;
    }
  else if (kind == SOURCE_REPORT_JUMP)
    {
      function->jumps += 1;
      report->total_jumps += 1;
      report->total_complexity_score += 2;
    }
  else if (kind == SOURCE_REPORT_RETURN)
    {
      function->returns += 1;
      report->total_returns += 1;
      report->total_complexity_score += 2;
    }
}

void			source_report_add_call(t_parsing		*parsing)
{
  t_source_report	*report;
  t_source_function_report *function;

  if (parsing == NULL)
    return ;
  report = &parsing->source_report;
  if (!report->enabled || report->current_function < 0 ||
      (size_t)report->current_function >= report->function_count)
    return ;
  function = &report->functions[report->current_function];
  function->calls += 1;
  report->total_calls += 1;
  report->total_complexity_score += 1;
}

void			source_report_enter_control(t_parsing		*parsing)
{
  t_source_report	*report;
  t_source_function_report *function;

  if (parsing == NULL)
    return ;
  report = &parsing->source_report;
  if (!report->enabled || report->current_function < 0 ||
      (size_t)report->current_function >= report->function_count)
    return ;
  function = &report->functions[report->current_function];
  function->current_control_depth += 1;
  if (function->current_control_depth > function->max_control_depth)
    {
      function->max_control_depth = function->current_control_depth;
      report->total_complexity_score += 4;
    }
  if (function->max_control_depth > report->max_control_depth)
    report->max_control_depth = function->max_control_depth;
}

void			source_report_leave_control(t_parsing		*parsing)
{
  t_source_report	*report;
  t_source_function_report *function;

  if (parsing == NULL)
    return ;
  report = &parsing->source_report;
  if (!report->enabled || report->current_function < 0 ||
      (size_t)report->current_function >= report->function_count)
    return ;
  function = &report->functions[report->current_function];
  if (function->current_control_depth > 0)
    function->current_control_depth -= 1;
}


static const t_function_map *gl_source_report_map = NULL;

typedef struct		s_source_report_call_analysis
{
  int			*expanded_score;
  int			*potential_score;
  int			*call_depth;
  bool			*recursive;
  bool			*calls_external;
  bool			*partial;
  bool			*function_pointer_escape;
  unsigned char		*state;
  size_t		count;
  int			total_expanded_score;
  int			total_potential_score;
  int			max_call_depth;
  int			recursive_functions;
  int			external_call_functions;
  int			partial_functions;
  int			function_pointer_escape_functions;
} 			t_source_report_call_analysis;

static void		source_report_call_analysis_clear
  (t_source_report_call_analysis	*analysis)
{
  if (analysis == NULL)
    return ;
  free(analysis->expanded_score);
  free(analysis->potential_score);
  free(analysis->call_depth);
  free(analysis->recursive);
  free(analysis->calls_external);
  free(analysis->partial);
  free(analysis->function_pointer_escape);
  free(analysis->state);
  memset(analysis, 0, sizeof(*analysis));
}

static bool		source_report_call_analysis_allocate
  (t_source_report_call_analysis	*analysis,
   size_t				 count)
{
  memset(analysis, 0, sizeof(*analysis));
  analysis->count = count;
  if (count == 0)
    return (true);
  if ((analysis->expanded_score = calloc(count, sizeof(*analysis->expanded_score))) == NULL ||
      (analysis->potential_score = calloc(count, sizeof(*analysis->potential_score))) == NULL ||
      (analysis->call_depth = calloc(count, sizeof(*analysis->call_depth))) == NULL ||
      (analysis->recursive = calloc(count, sizeof(*analysis->recursive))) == NULL ||
      (analysis->calls_external = calloc(count, sizeof(*analysis->calls_external))) == NULL ||
      (analysis->partial = calloc(count, sizeof(*analysis->partial))) == NULL ||
      (analysis->function_pointer_escape = calloc(count, sizeof(*analysis->function_pointer_escape))) == NULL ||
      (analysis->state = calloc(count, sizeof(*analysis->state))) == NULL)
    {
      source_report_call_analysis_clear(analysis);
      return (false);
    }
  return (true);
}

static ssize_t		source_report_find_function(const t_source_report	*report,
					    const char			*name)
{
  if (report == NULL || name == NULL || name[0] == '\0')
    return (-1);
  for (size_t i = 0; i < report->function_count; ++i)
    if (strcmp(report->functions[i].name, name) == 0)
      return ((ssize_t)i);
  return (-1);
}

static ssize_t		source_report_find_node(const t_function_map	*map,
					const char		*name)
{
  if (map == NULL || name == NULL || name[0] == '\0')
    return (-1);
  for (size_t i = 0; i < map->nbr_nodes; ++i)
    if (map->nodes[i].name != NULL && strcmp(map->nodes[i].name, name) == 0)
      return ((ssize_t)i);
  return (-1);
}

static void		source_report_mark_pointer_escapes
  (t_source_report_call_analysis	*analysis,
   const t_source_report		*report,
   const t_function_map		*map)
{
  if (analysis == NULL || report == NULL || map == NULL)
    return ;
  for (size_t i = 0; i < map->nbr_edges; ++i)
    if (map->edges[i].pointer_donation_count > 0 && map->edges[i].to < map->nbr_nodes)
      {
	ssize_t		idx;

	idx = source_report_find_function(report, map->nodes[map->edges[i].to].name);
	if (idx >= 0)
	  analysis->function_pointer_escape[idx] = true;
      }
}

static int		source_report_compute_expanded
  (t_source_report_call_analysis	*analysis,
   const t_source_report		*report,
   const t_function_map		*map,
   size_t			 index)
{
  const t_source_function_report *function;
  ssize_t		node;
  int			score;
  int			depth;

  if (index >= report->function_count)
    return (0);
  if (analysis->state[index] == 2)
    return (analysis->expanded_score[index]);
  function = &report->functions[index];
  if (analysis->state[index] == 1)
    {
      analysis->recursive[index] = true;
      analysis->partial[index] = true;
      return (source_report_local_score(function));
    }
  analysis->state[index] = 1;
  score = source_report_local_score(function);
  depth = 0;
  node = source_report_find_node(map, function->name);
  if (node >= 0 && map != NULL)
    for (size_t i = 0; i < map->nbr_edges; ++i)
      if (map->edges[i].from == (size_t)node && map->edges[i].direct_call_count > 0)
	{
	  ssize_t	child;
	  int		child_score;
	  int		child_depth;

	  if (map->edges[i].to >= map->nbr_nodes)
	    continue ;
	  child = source_report_find_function(report, map->nodes[map->edges[i].to].name);
	  if (child >= 0)
	    {
	      if (analysis->state[child] == 1)
		{
		  analysis->recursive[index] = true;
		  analysis->recursive[child] = true;
		  analysis->partial[index] = true;
		  child_score = source_report_local_score(&report->functions[child]);
		  child_depth = 1;
		}
	      else
		{
		  child_score = source_report_compute_expanded
		    (analysis, report, map, (size_t)child);
		  child_depth = analysis->call_depth[child] + 1;
		  if (analysis->calls_external[child])
		    analysis->calls_external[index] = true;
		  if (analysis->partial[child])
		    analysis->partial[index] = true;
		}
	      score += child_score * map->edges[i].direct_call_count;
	      if (child_depth > depth)
		depth = child_depth;
	    }
	  else
	    {
	      analysis->calls_external[index] = true;
	      analysis->partial[index] = true;
	      score += map->edges[i].direct_call_count;
	      if (depth < 1)
		depth = 1;
	    }
	}
  analysis->state[index] = 2;
  analysis->expanded_score[index] = score;
  analysis->call_depth[index] = depth;
  return (score);
}

static void		source_report_compute_potential
  (t_source_report_call_analysis	*analysis,
   const t_source_report		*report,
   const t_function_map		*map,
   size_t			 index)
{
  ssize_t		node;
  int			score;

  score = analysis->expanded_score[index];
  node = source_report_find_node(map, report->functions[index].name);
  if (node >= 0 && map != NULL)
    for (size_t i = 0; i < map->nbr_edges; ++i)
      if (map->edges[i].from == (size_t)node &&
	  map->edges[i].pointer_donation_count > 0 &&
	  map->edges[i].to < map->nbr_nodes)
	{
	  ssize_t	child;

	  child = source_report_find_function(report, map->nodes[map->edges[i].to].name);
	  if (child >= 0)
	    {
	      score += analysis->expanded_score[child] *
		map->edges[i].pointer_donation_count;
	      if (analysis->partial[child])
		analysis->partial[index] = true;
	    }
	  else
	    analysis->partial[index] = true;
	}
  analysis->potential_score[index] = score;
}

static bool		source_report_call_analysis_prepare
  (t_source_report_call_analysis	*analysis,
   const t_source_report		*report,
   const t_function_map		*map)
{
  if (!source_report_call_analysis_allocate(analysis,
					    report ? report->function_count : 0))
    return (false);
  if (report == NULL)
    return (true);
  source_report_mark_pointer_escapes(analysis, report, map);
  for (size_t i = 0; i < report->function_count; ++i)
    source_report_compute_expanded(analysis, report, map, i);
  for (size_t i = 0; i < report->function_count; ++i)
    source_report_compute_potential(analysis, report, map, i);
  for (size_t i = 0; i < report->function_count; ++i)
    {
      analysis->total_expanded_score += analysis->expanded_score[i];
      analysis->total_potential_score += analysis->potential_score[i];
      if (analysis->call_depth[i] > analysis->max_call_depth)
	analysis->max_call_depth = analysis->call_depth[i];
      if (analysis->recursive[i])
	analysis->recursive_functions += 1;
      if (analysis->calls_external[i])
	analysis->external_call_functions += 1;
      if (analysis->partial[i])
	analysis->partial_functions += 1;
      if (analysis->function_pointer_escape[i])
	analysis->function_pointer_escape_functions += 1;
    }
  return (true);
}

static bool		source_report_fill_configuration(t_bunny_configuration		*cnf,
						 const t_source_report		*report)
{
  t_source_report_call_analysis analysis;

  if (!source_report_call_analysis_prepare(&analysis, report, gl_source_report_map))
    return (false);
  if (!bunny_configuration_setf(cnf, "CrawlerSourceReport", "Report.Kind") ||
      !bunny_configuration_setf(cnf, 2, "Report.Version") ||
      !bunny_configuration_setf(cnf, (int)report->function_count,
				    "Report.Totals.Functions") ||
      !bunny_configuration_setf(cnf, report->total_instructions,
				    "Report.Totals.Instructions") ||
      !bunny_configuration_setf(cnf, report->total_expressions,
				    "Report.Totals.Expressions") ||
      !bunny_configuration_setf(cnf, report->total_declarations,
				    "Report.Totals.Declarations") ||
      !bunny_configuration_setf(cnf, report->total_branches,
				    "Report.Totals.Branches") ||
      !bunny_configuration_setf(cnf, report->total_loops,
				    "Report.Totals.Loops") ||
      !bunny_configuration_setf(cnf, report->total_jumps,
				    "Report.Totals.Jumps") ||
      !bunny_configuration_setf(cnf, report->total_returns,
				    "Report.Totals.Returns") ||
      !bunny_configuration_setf(cnf, report->total_calls,
				    "Report.Totals.Calls") ||
      !bunny_configuration_setf(cnf, report->total_cyclomatic,
				    "Report.Totals.Complexity.Cyclomatic") ||
      !bunny_configuration_setf(cnf, report->max_control_depth,
				    "Report.Totals.Complexity.ControlNestingMax") ||
      !bunny_configuration_setf(cnf, report->total_complexity_score,
				    "Report.Totals.Complexity.LocalScore") ||
      !bunny_configuration_setf(cnf, analysis.total_expanded_score,
				    "Report.Totals.Complexity.CallExpandedScore") ||
      !bunny_configuration_setf(cnf, analysis.total_potential_score,
				    "Report.Totals.Complexity.PotentialExpandedScore") ||
      !bunny_configuration_setf(cnf, analysis.max_call_depth,
				    "Report.Totals.Complexity.MaxCallDepth") ||
      !bunny_configuration_setf(cnf, analysis.recursive_functions,
				    "Report.Totals.Complexity.RecursiveFunctions") ||
      !bunny_configuration_setf(cnf, analysis.external_call_functions,
				    "Report.Totals.Complexity.FunctionsCallingExternal") ||
      !bunny_configuration_setf(cnf, analysis.partial_functions,
				    "Report.Totals.Complexity.PartialFunctions") ||
      !bunny_configuration_setf(cnf, analysis.function_pointer_escape_functions,
				    "Report.Totals.Complexity.FunctionPointerEscapes") ||
      !bunny_configuration_setf(cnf, report->total_lines,
				    "Report.Totals.Lines"))
    {
      source_report_call_analysis_clear(&analysis);
      return (false);
    }
  for (size_t i = 0; i < report->function_count; ++i)
    {
      const t_source_function_report *function = &report->functions[i];
      int		index = (int)i;
      int		score = source_report_local_score(function);

      if (!bunny_configuration_setf(cnf, function->name,
				    "Report.Functions[%d].Name", index) ||
	  !bunny_configuration_setf(cnf, function->file,
				    "Report.Functions[%d].File", index) ||
	  !bunny_configuration_setf(cnf, function->start_line,
				    "Report.Functions[%d].StartLine", index) ||
	  !bunny_configuration_setf(cnf, function->end_line,
				    "Report.Functions[%d].EndLine", index) ||
	  !bunny_configuration_setf(cnf, function->line_count,
				    "Report.Functions[%d].LineCount", index) ||
	  !bunny_configuration_setf(cnf, function->instructions,
				    "Report.Functions[%d].Instructions", index) ||
	  !bunny_configuration_setf(cnf, function->expressions,
				    "Report.Functions[%d].Expressions", index) ||
	  !bunny_configuration_setf(cnf, function->declarations,
				    "Report.Functions[%d].Declarations", index) ||
	  !bunny_configuration_setf(cnf, function->branches,
				    "Report.Functions[%d].Branches", index) ||
	  !bunny_configuration_setf(cnf, function->loops,
				    "Report.Functions[%d].Loops", index) ||
	  !bunny_configuration_setf(cnf, function->jumps,
				    "Report.Functions[%d].Jumps", index) ||
	  !bunny_configuration_setf(cnf, function->returns,
				    "Report.Functions[%d].Returns", index) ||
	  !bunny_configuration_setf(cnf, function->calls,
				    "Report.Functions[%d].Calls", index) ||
	  !bunny_configuration_setf(cnf, function->cyclomatic,
				    "Report.Functions[%d].Complexity.Cyclomatic", index) ||
	  !bunny_configuration_setf(cnf, function->max_control_depth,
				    "Report.Functions[%d].Complexity.ControlNestingMax", index) ||
	  !bunny_configuration_setf(cnf, score,
				    "Report.Functions[%d].Complexity.LocalScore", index) ||
	  !bunny_configuration_setf(cnf, analysis.expanded_score[i],
				    "Report.Functions[%d].Complexity.CallExpandedScore", index) ||
	  !bunny_configuration_setf(cnf, analysis.potential_score[i],
				    "Report.Functions[%d].Complexity.PotentialExpandedScore", index) ||
	  !bunny_configuration_setf(cnf, analysis.call_depth[i],
				    "Report.Functions[%d].Complexity.CallDepth", index) ||
	  !bunny_configuration_setf(cnf, analysis.recursive[i] ? 1 : 0,
				    "Report.Functions[%d].Complexity.Recursive", index) ||
	  !bunny_configuration_setf(cnf, analysis.calls_external[i] ? 1 : 0,
				    "Report.Functions[%d].Complexity.CallsExternal", index) ||
	  !bunny_configuration_setf(cnf, analysis.partial[i] ? 1 : 0,
				    "Report.Functions[%d].Complexity.Partial", index) ||
	  !bunny_configuration_setf(cnf, analysis.function_pointer_escape[i] ? 1 : 0,
				    "Report.Functions[%d].Complexity.FunctionPointerEscape", index) ||
	  !bunny_configuration_setf(cnf, source_report_complexity_class(score),
				    "Report.Functions[%d].Complexity.Class", index))
	{
	  source_report_call_analysis_clear(&analysis);
	  return (false);
	}
    }
  source_report_call_analysis_clear(&analysis);
  return (true);
}

bool			source_report_write_with_map(const char		*file,
				     const t_source_report	*report,
				     const t_function_map	*map)
{
  t_bunny_configuration *cnf;
  bool			ok;

  if (file == NULL || report == NULL)
    return (false);
  if ((cnf = bunny_new_configuration()) == NULL)
    return (false);
  gl_source_report_map = map;
  ok = source_report_fill_configuration(cnf, report) &&
    bunny_save_configuration(BC_DABSIC, file, cnf);
  gl_source_report_map = NULL;
  bunny_delete_configuration(cnf);
  return (ok);
}

bool			source_report_write(const char			*file,
				    const t_source_report		*report)
{
  return (source_report_write_with_map(file, report, NULL));
}

