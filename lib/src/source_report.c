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

static void		source_report_reset_totals(t_source_report	*report)
{
  report->total_instructions = 0;
  report->total_expressions = 0;
  report->total_declarations = 0;
  report->total_branches = 0;
  report->total_loops = 0;
  report->total_jumps = 0;
  report->total_returns = 0;
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
    }
  else if (kind == SOURCE_REPORT_LOOP)
    {
      function->loops += 1;
      report->total_loops += 1;
    }
  else if (kind == SOURCE_REPORT_JUMP)
    {
      function->jumps += 1;
      report->total_jumps += 1;
    }
  else if (kind == SOURCE_REPORT_RETURN)
    {
      function->returns += 1;
      report->total_returns += 1;
    }
}

static bool		source_report_fill_configuration(t_bunny_configuration		*cnf,
					 const t_source_report		*report)
{
  if (!bunny_configuration_setf(cnf, "CrawlerSourceReport", "Report.Kind") ||
      !bunny_configuration_setf(cnf, 1, "Report.Version") ||
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
      !bunny_configuration_setf(cnf, report->total_lines,
				    "Report.Totals.Lines"))
    return (false);
  for (size_t i = 0; i < report->function_count; ++i)
    {
      const t_source_function_report *function = &report->functions[i];
      int		index = (int)i;

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
				    "Report.Functions[%d].Returns", index))
	return (false);
    }
  return (true);
}

bool			source_report_write(const char			*file,
				    const t_source_report		*report)
{
  t_bunny_configuration *cnf;
  bool			ok;

  if (file == NULL || report == NULL)
    return (false);
  if ((cnf = bunny_new_configuration()) == NULL)
    return (false);
  ok = source_report_fill_configuration(cnf, report) &&
    bunny_save_configuration(BC_DABSIC, file, cnf);
  bunny_delete_configuration(cnf);
  return (ok);
}
