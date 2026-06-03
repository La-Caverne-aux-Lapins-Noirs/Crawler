/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
** Pentacle Technologie 2008-2023
** EFRITS SAS 2022-2023
**
** C-C-C CRAWLER!
** Configurable C Code Crawler !
** Bloc constitutif du "TechnoCentre", suite logiciel du projet "Pentacle School"
** Vérificateur de conformité du code (entre autre) niveau style.
**
** Merci pour la grammaire du C ANSI:
** https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
*/

#include		<limits.h>
#include		<fcntl.h>
#include		<ctype.h>
#include		<math.h>
#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		"crawler.h"

#ifndef			M_PI
# define		M_PI	3.14159265358979323846
#endif

extern t_parsing	*gl_parsing_save;

static int		function_body_end_line(const char	*code,
				       ssize_t		i)
{
  if (i <= 0)
    return (0);
  if (code[i] == '\0')
    i -= 1;
  while (i > 0 && isspace((unsigned char)code[i]))
    i -= 1;
  while (i > 0 && code[i] != '}')
    i -= 1;
  if (code[i] == '}')
    return (bunny_which_line(code, i) - 1);
  return (bunny_which_line(code, i));
}

// Liste des types standards
static struct
{
  const char		*name;
  size_t		siz;
} standard_types[] =
  {
    {"void", sizeof(void)},
    {"char", sizeof(char)},
    {"short", sizeof(short)},
    {"int", sizeof(int)},
    {"long", sizeof(long)},
    {"float", sizeof(float)},
    {"double", sizeof(double)},
    {"signed", sizeof(signed)},
    {"unsigned", sizeof(unsigned)},
    {"_Bool", sizeof(_Bool)},
    {"_Float128", sizeof(_Float128)},
    {"__builtin_va_list", sizeof(__builtin_va_list)}
    // , "__int8_t", "__int16_t", "__int32_t", "__int64_t"
  };

static const char	*keywords[] =
  {
    "if", "for", "double", "while", "switch", "break", "goto", "continue", "return",
    "const", "volatile", "default", "case", "typedef", "extern", "static", "auto", "register", "do",
    "restrict", "__restrict", "__restrict__", "__attribute__", "__asm__",
    "inline", "__inline", "__inline__", "_Noreturn"
  };


typedef struct		s_checked_return_function
{
  const char		*name;
  bool			realloc_like;
  bool			full_transfer;
} 			t_checked_return_function;

static const t_checked_return_function gl_checked_return_functions[] =
  {
    {"malloc", false, false}, {"calloc", false, false}, {"realloc", true, false},
    {"open", false, false}, {"creat", false, false}, {"close", false, false},
    {"read", false, false}, {"write", false, true},
    {"readv", false, false}, {"writev", false, true},
    {"pread", false, false}, {"pwrite", false, true},
    {"lseek", false, false}, {"access", false, false}, {"stat", false, false}, {"lstat", false, false}, {"fstat", false, false},
    {"mkdir", false, false}, {"rmdir", false, false}, {"unlink", false, false}, {"rename", false, false},
    {"dup", false, false}, {"dup2", false, false}, {"pipe", false, false},
    {"fork", false, false}, {"wait", false, false}, {"waitpid", false, false},
    {"socket", false, false}, {"bind", false, false}, {"listen", false, false}, {"accept", false, false},
    {"connect", false, false}, {"send", false, true}, {"recv", false, false},
    {"sendto", false, true}, {"recvfrom", false, false},
    {"sendmsg", false, true}, {"recvmsg", false, false},
    {"select", false, false}, {"poll", false, false},
    {"mmap", false, false}, {"munmap", false, false},
    {NULL, false, false}
  };

static bool		crawler_checked_return_enabled(t_parsing	*p)
{
  return (p != NULL && p->checked_return.active && p->checked_return.value != 0);
}


static void		crawler_checked_return_apply_kind(const char	*kind,
							   bool		*realloc_like,
							   bool		*full_transfer)
{
  if (kind == NULL)
    return ;
  if (strcmp(kind, "realloc") == 0 || strcmp(kind, "allocation-resize") == 0)
    {
      if (realloc_like != NULL)
	*realloc_like = true;
    }
  if (strcmp(kind, "full_transfer") == 0 || strcmp(kind, "full-transfer") == 0 ||
      strcmp(kind, "write") == 0 || strcmp(kind, "send") == 0)
    {
      if (full_transfer != NULL)
	*full_transfer = true;
    }
}

static int		crawler_checked_return_config_match(t_parsing	*p,
							 const char	*name,
							 bool		*realloc_like,
							 bool		*full_transfer)
{
  int			count;
  int			limit;

  if (p == NULL || p->configuration == NULL || name == NULL)
    return (-1);
  count = -1;
  bunny_configuration_getf(p->configuration, &count, "CheckedReturn.FunctionCount");
  limit = (count >= 0 ? count : 512);
  for (int i = 0; i < limit; ++i)
    {
      const char	*entry;
      const char	*kind;
      int		disabled;
      int		value;

      entry = NULL;
      if (!bunny_configuration_getf
	  (p->configuration, &entry, "CheckedReturn.Functions[%d].Name", i) &&
	  !bunny_configuration_getf
	  (p->configuration, &entry, "CheckedReturn.Functions[%d]", i))
	{
	  if (count < 0)
	    break ;
	  continue ;
	}
      if (entry == NULL || strcmp(entry, name) != 0)
	continue ;
      disabled = 0;
      bunny_configuration_getf
	(p->configuration, &disabled, "CheckedReturn.Functions[%d].Disabled", i);
      if (disabled)
	return (0);
      if (realloc_like != NULL)
	*realloc_like = false;
      if (full_transfer != NULL)
	*full_transfer = false;
      value = 0;
      if (bunny_configuration_getf
	  (p->configuration, &value, "CheckedReturn.Functions[%d].ReallocLike", i) &&
	  realloc_like != NULL)
	*realloc_like = value != 0;
      value = 0;
      if (bunny_configuration_getf
	  (p->configuration, &value, "CheckedReturn.Functions[%d].FullTransfer", i) &&
	  full_transfer != NULL)
	*full_transfer = value != 0;
      kind = NULL;
      if (bunny_configuration_getf
	  (p->configuration, &kind, "CheckedReturn.Functions[%d].Kind", i))
	crawler_checked_return_apply_kind(kind, realloc_like, full_transfer);
      return (1);
    }
  return (-1);
}

static bool		crawler_identifier_start(int			c)
{
  return (isalpha((unsigned char)c) || c == '_');
}

static bool		crawler_identifier_part(int			c)
{
  return (isalnum((unsigned char)c) || c == '_');
}

static ssize_t		crawler_checked_return_skip_literal(const char	*code,
						       ssize_t		 i,
						       ssize_t		 end)
{
  char			quote;

  if (i >= end)
    return (i);
  if (code[i] == '/' && i + 1 < end && code[i + 1] == '/')
    {
      i += 2;
      while (i < end && code[i] != '\n')
	i += 1;
      return (i);
    }
  if (code[i] == '/' && i + 1 < end && code[i + 1] == '*')
    {
      i += 2;
      while (i + 1 < end && !(code[i] == '*' && code[i + 1] == '/'))
	i += 1;
      if (i + 1 < end)
	i += 2;
      return (i);
    }
  if (code[i] != '\'' && code[i] != '"')
    return (i);
  quote = code[i++];
  while (i < end && code[i] != quote)
    {
      if (code[i] == '\\' && i + 1 < end)
	i += 2;
      else
	i += 1;
    }
  if (i < end)
    i += 1;
  return (i);
}

static void		crawler_checked_return_copy_identifier(char		*target,
							 size_t		 size,
							 const char	*code,
							 ssize_t	 begin,
							 ssize_t	 end)
{
  size_t		len;

  if (size == 0)
    return ;
  if (begin < 0 || end < begin)
    {
      target[0] = '\0';
      return ;
    }
  len = (size_t)(end - begin);
  if (len >= size)
    len = size - 1;
  memcpy(target, &code[begin], len);
  target[len] = '\0';
}

static bool		crawler_checked_return_is_critical(t_parsing	*p,
							 const char	*name,
							 bool		*realloc_like,
							 bool		*full_transfer)
{
  int			match;
  int			use_default;

  match = crawler_checked_return_config_match(p, name, realloc_like, full_transfer);
  if (match == 1)
    return (true);
  if (match == 0)
    return (false);
  use_default = 1;
  if (p != NULL && p->configuration != NULL)
    bunny_configuration_getf(p->configuration, &use_default, "CheckedReturn.UseDefaultList");
  if (!use_default)
    {
      if (realloc_like != NULL)
	*realloc_like = false;
      if (full_transfer != NULL)
	*full_transfer = false;
      return (false);
    }
  for (int i = 0; gl_checked_return_functions[i].name != NULL; ++i)
    if (strcmp(name, gl_checked_return_functions[i].name) == 0)
      {
	if (realloc_like != NULL)
	  *realloc_like = gl_checked_return_functions[i].realloc_like;
	if (full_transfer != NULL)
	  *full_transfer = gl_checked_return_functions[i].full_transfer;
	return (true);
      }
  if (realloc_like != NULL)
    *realloc_like = false;
  if (full_transfer != NULL)
    *full_transfer = false;
  return (false);
}

static bool		crawler_checked_return_read_identifier_at(const char	*code,
								 ssize_t	 pos,
								 ssize_t	 end,
								 char		*out,
								 size_t		 out_size,
								 ssize_t	*identifier_end)
{
  ssize_t		j;

  if (!crawler_identifier_start(code[pos]))
    return (false);
  j = pos + 1;
  while (j < end && crawler_identifier_part(code[j]))
    j += 1;
  crawler_checked_return_copy_identifier(out, out_size, code, pos, j);
  if (identifier_end != NULL)
    *identifier_end = j;
  return (true);
}

static bool		crawler_checked_return_find_call(t_parsing	*p,
							 const char	*code,
							 ssize_t	 begin,
							 ssize_t	 end,
							 char		*function,
							 size_t		 function_size,
							 ssize_t	*position,
							 bool		*realloc_like,
							 bool		*full_transfer)
{
  ssize_t		i;

  for (i = begin; i < end; ++i)
    {
      ssize_t		j;
      char		name[SYMBOL_SIZE + 1];

      j = crawler_checked_return_skip_literal(code, i, end);
      if (j != i)
	{
	  i = j - 1;
	  continue ;
	}
      if (!crawler_checked_return_read_identifier_at
	  (code, i, end, name, sizeof(name), &j))
	continue ;
      i = j;
      while (i < end && isspace((unsigned char)code[i]))
	i += 1;
      if (i < end && code[i] == '(' && crawler_checked_return_is_critical(p, name, realloc_like, full_transfer))
	{
	  if (function != NULL)
	    crawler_checked_return_copy_identifier(function, function_size, name, 0, strlen(name));
	  if (position != NULL)
	    *position = j - (ssize_t)strlen(name);
	  return (true);
	}
      i = j - 1;
    }
  return (false);
}

static bool		crawler_checked_return_has_identifier(const char	*code,
							   ssize_t	 begin,
							   ssize_t	 end,
							   const char	*name)
{
  ssize_t		i;
  size_t		len;

  if (name == NULL || name[0] == '\0')
    return (false);
  len = strlen(name);
  for (i = begin; i < end; ++i)
    {
      ssize_t		j;

      j = crawler_checked_return_skip_literal(code, i, end);
      if (j != i)
	{
	  i = j - 1;
	  continue ;
	}
      if ((i == begin || !crawler_identifier_part(code[i - 1])) &&
	  (size_t)(end - i) >= len && strncmp(&code[i], name, len) == 0 &&
	  (i + (ssize_t)len >= end || !crawler_identifier_part(code[i + len])))
	return (true);
    }
  return (false);
}

static bool		crawler_checked_return_has_comparison(const char	*code,
							      ssize_t	 begin,
							      ssize_t	 end)
{
  for (ssize_t i = begin; i < end; ++i)
    {
      ssize_t		j;

      j = crawler_checked_return_skip_literal(code, i, end);
      if (j != i)
	{
	  i = j - 1;
	  continue ;
	}
      if (code[i] == '!' || code[i] == '<' || code[i] == '>')
	return (true);
      if (code[i] == '=' && i + 1 < end && code[i + 1] == '=')
	return (true);
    }
  return (false);
}


static bool		crawler_checked_return_has_token(const char	*code,
							 ssize_t	 begin,
							 ssize_t	 end,
							 const char	*token)
{
  size_t		len;

  len = strlen(token);
  for (ssize_t i = begin; i < end; ++i)
    {
      ssize_t		j;

      j = crawler_checked_return_skip_literal(code, i, end);
      if (j != i)
	{
	  i = j - 1;
	  continue ;
	}
      if ((size_t)(end - i) >= len && strncmp(&code[i], token, len) == 0)
	return (true);
    }
  return (false);
}

static bool		crawler_checked_return_has_full_transfer_check(const char	*code,
									 ssize_t	 begin,
									 ssize_t	 end,
									 const char	*name)
{
  if (name != NULL && name[0] != '\0' &&
      !crawler_checked_return_has_identifier(code, begin, end, name))
    return (false);
  if (!crawler_checked_return_has_token(code, begin, end, "!="))
    return (false);
  if (crawler_checked_return_has_token(code, begin, end, "-1"))
    return (false);
  return (true);
}

static ssize_t		crawler_checked_return_find_assignment(const char	*code,
							   ssize_t	 begin,
							   ssize_t	 end)
{
  int			depth;

  depth = 0;
  for (ssize_t i = begin; i < end; ++i)
    {
      ssize_t		j;

      j = crawler_checked_return_skip_literal(code, i, end);
      if (j != i)
	{
	  i = j - 1;
	  continue ;
	}
      if (code[i] == '(' || code[i] == '[')
	depth += 1;
      else if ((code[i] == ')' || code[i] == ']') && depth > 0)
	depth -= 1;
      else if (depth == 0 && code[i] == '=')
	{
	  char		prev = i > begin ? code[i - 1] : '\0';
	  char		next = i + 1 < end ? code[i + 1] : '\0';

	  if (prev != '=' && prev != '!' && prev != '<' && prev != '>' && next != '=')
	    return (i);
	}
    }
  return (-1);
}

static bool		crawler_checked_return_lhs_variable(const char	*code,
							      ssize_t	 begin,
							      ssize_t	 assign,
							      char	*variable,
							      size_t	 size)
{
  ssize_t		end;
  ssize_t		start;

  end = assign;
  while (end > begin && isspace((unsigned char)code[end - 1]))
    end -= 1;
  start = end;
  while (start > begin && crawler_identifier_part(code[start - 1]))
    start -= 1;
  if (start == end || !crawler_identifier_start(code[start]))
    return (false);
  if (start > begin)
    {
      ssize_t		j = start;

      while (j > begin && isspace((unsigned char)code[j - 1]))
	j -= 1;
      if (j > begin && code[j - 1] == '.')
	return (false);
      if (j > begin + 1 && code[j - 1] == '>' && code[j - 2] == '-')
	return (false);
    }
  crawler_checked_return_copy_identifier(variable, size, code, start, end);
  return (true);
}

static bool		crawler_checked_return_realloc_original(const char	*code,
								 ssize_t	 call_pos,
								 ssize_t	 end,
								 char		*target,
								 size_t		 size)
{
  ssize_t		i;
  ssize_t		identifier_end;

  i = call_pos + 7;
  while (i < end && isspace((unsigned char)code[i]))
    i += 1;
  if (i >= end || code[i] != '(')
    return (false);
  i += 1;
  while (i < end && isspace((unsigned char)code[i]))
    i += 1;
  return (crawler_checked_return_read_identifier_at
	  (code, i, end, target, size, &identifier_end));
}

static void		crawler_checked_return_begin_function(t_parsing	*p)
{
  if (p != NULL)
    p->checked_return_state.nbr_pending = 0;
}

static bool		crawler_checked_return_add_pending(t_parsing	*p,
							   const char	*variable,
							   const char	*function,
							   int		 position,
							   bool		 checked,
							   bool		 full_transfer,
							   bool		 full_checked)
{
  t_chk_return_state *state;
  int			index;

  if (!crawler_checked_return_enabled(p) || variable == NULL || variable[0] == '\0')
    return (true);
  state = &p->checked_return_state;
  for (index = 0; index < state->nbr_pending; ++index)
    if (strcmp(state->pending[index].variable, variable) == 0)
      break ;
  if (index >= CRAWLER_CHECKED_RETURN_MAX_PENDING)
    return (false);
  if (index == state->nbr_pending)
    state->nbr_pending += 1;
  crawler_checked_return_copy_identifier
    (state->pending[index].variable, sizeof(state->pending[index].variable),
     variable, 0, strlen(variable));
  crawler_checked_return_copy_identifier
    (state->pending[index].function, sizeof(state->pending[index].function),
     function, 0, strlen(function));
  state->pending[index].position = position;
  state->pending[index].checked = checked;
  state->pending[index].warned = false;
  state->pending[index].full_transfer = full_transfer;
  state->pending[index].full_checked = full_checked;
  state->pending[index].full_transfer = full_transfer;
  state->pending[index].full_checked = full_checked;
  return (true);
}

static bool		crawler_checked_return_warn_usage(t_parsing	*p,
							   const char	*code,
							   ssize_t	 begin,
							   ssize_t	 end,
							   const char	*ignore_lhs)
{
  if (!crawler_checked_return_enabled(p))
    return (true);
  for (int i = 0; i < p->checked_return_state.nbr_pending; ++i)
    {
      t_chk_return_pending *pending = &p->checked_return_state.pending[i];

      if (pending->checked || pending->warned)
	continue ;
      if (ignore_lhs != NULL && strcmp(ignore_lhs, pending->variable) == 0)
	continue ;
      if (crawler_checked_return_has_identifier(code, begin, end, pending->variable))
	{
	  pending->warned = true;
	  if (!add_warning
	      (p, IZ(p, &begin), code, (int)begin, &p->checked_return.counter,
	       "Variable '%s' stores return value of %s and is used before being checked.",
	       pending->variable, pending->function))
	    return (false);
	}
    }
  return (true);
}

static bool		crawler_checked_return_expression_statement(t_parsing	*p,
								 const char	*code,
								 ssize_t	 begin,
								 ssize_t	 end)
{
  ssize_t		assign;
  char			lhs[SYMBOL_SIZE + 1];
  char			function[SYMBOL_SIZE + 1];
  ssize_t		call_pos;
  bool			realloc_like;
  bool			full_transfer;

  if (!crawler_checked_return_enabled(p) || !p->last_declaration.inside_function)
    return (true);
  assign = crawler_checked_return_find_assignment(code, begin, end);
  lhs[0] = '\0';
  if (assign >= 0 && crawler_checked_return_lhs_variable
      (code, begin, assign, lhs, sizeof(lhs)))
    {
      if (!crawler_checked_return_warn_usage(p, code, begin, end, lhs))
	return (false);
      if (crawler_checked_return_find_call
	  (p, code, assign + 1, end, function, sizeof(function), &call_pos, &realloc_like, &full_transfer))
	{
	  if (realloc_like)
	    {
	      char		original[SYMBOL_SIZE + 1];

	      if (crawler_checked_return_realloc_original
		  (code, call_pos, end, original, sizeof(original)) &&
		  strcmp(original, lhs) == 0)
		if (!add_warning
		    (p, IZ(p, &call_pos), code, (int)call_pos,
		     &p->checked_return.counter,
		     "realloc result assigned directly to '%s'; use a temporary pointer before replacing the original allocation.",
		     lhs))
		  return (false);
	    }
	  return (crawler_checked_return_add_pending
		  (p, lhs, function, (int)call_pos, false, full_transfer, false));
	}
      return (true);
    }
  if (!crawler_checked_return_warn_usage(p, code, begin, end, NULL))
    return (false);
  if (crawler_checked_return_find_call
      (p, code, begin, end, function, sizeof(function), &call_pos, &realloc_like, &full_transfer))
    {
      ssize_t		j = begin;

      while (j < end && isspace((unsigned char)code[j]))
	j += 1;
      if (j < end && code[j] == '(')
	{
	  ssize_t	k = j + 1;

	  while (k < end && isspace((unsigned char)code[k]))
	    k += 1;
	  if (k + 4 <= end && strncmp(&code[k], "void", 4) == 0 &&
	      (k + 4 >= end || !crawler_identifier_part(code[k + 4])))
	    return (true);
	}
      return (add_warning
	      (p, IZ(p, &call_pos), code, (int)call_pos,
	       &p->checked_return.counter,
	       "Return value of %s must be checked.", function));
    }
  return (true);
}

static bool		crawler_checked_return_condition(t_parsing	*p,
							 const char	*code,
							 ssize_t	 begin,
							 ssize_t	 end)
{
  ssize_t		assign;
  char			lhs[SYMBOL_SIZE + 1];
  char			function[SYMBOL_SIZE + 1];
  ssize_t		call_pos;
  bool			realloc_like;
  bool			full_transfer;
  bool			has_comparison;

  if (!crawler_checked_return_enabled(p) || !p->last_declaration.inside_function)
    return (true);
  has_comparison = crawler_checked_return_has_comparison(code, begin, end);
  for (int i = 0; i < p->checked_return_state.nbr_pending; ++i)
    if (has_comparison && crawler_checked_return_has_identifier
	(code, begin, end, p->checked_return_state.pending[i].variable))
      {
	p->checked_return_state.pending[i].checked = true;
	if (p->checked_return_state.pending[i].full_transfer &&
	    crawler_checked_return_has_full_transfer_check
	    (code, begin, end, p->checked_return_state.pending[i].variable))
	  p->checked_return_state.pending[i].full_checked = true;
      }
  if (crawler_checked_return_find_call
      (p, code, begin, end, function, sizeof(function), &call_pos,
       &realloc_like, &full_transfer))
    {
      if (full_transfer && has_comparison &&
	  !crawler_checked_return_has_full_transfer_check(code, begin, end, NULL))
	return (add_warning
		(p, IZ(p, &call_pos), code, (int)call_pos,
		 &p->checked_return.counter,
		 "Return value of %s is checked only as failure; partial transfer may be ignored.",
		 function));
    }
  assign = crawler_checked_return_find_assignment(code, begin, end);
  if (assign >= 0 && crawler_checked_return_lhs_variable
      (code, begin, assign, lhs, sizeof(lhs)) &&
      crawler_checked_return_find_call
      (p, code, assign + 1, end, function, sizeof(function), &call_pos, &realloc_like, &full_transfer))
    return (crawler_checked_return_add_pending
	    (p, lhs, function, (int)call_pos, has_comparison, full_transfer,
	     full_transfer && crawler_checked_return_has_full_transfer_check
	     (code, begin, end, lhs)));
  return (true);
}

static bool		crawler_checked_return_end_function(t_parsing	*p,
							 const char	*code,
							 ssize_t	 pos)
{
  if (!crawler_checked_return_enabled(p))
    return (true);
  for (int i = 0; i < p->checked_return_state.nbr_pending; ++i)
    {
      t_chk_return_pending *pending = &p->checked_return_state.pending[i];

      if (!pending->checked && !pending->warned)
	{
	  pending->warned = true;
	  if (!add_warning
	      (p, IZ(p, &pos), code, pending->position,
	       &p->checked_return.counter,
	       "Variable '%s' stores return value of %s but is never checked.",
	       pending->variable, pending->function))
	    return (false);
	}
      else if (pending->full_transfer && !pending->full_checked && !pending->warned)
	{
	  pending->warned = true;
	  if (!add_warning
	      (p, IZ(p, &pos), code, pending->position,
	       &p->checked_return.counter,
	       "Variable '%s' stores return value of %s but partial transfer is not checked.",
	       pending->variable, pending->function))
	    return (false);
	}
      else if (pending->full_transfer && !pending->full_checked && !pending->warned)
	{
	  pending->warned = true;
	  if (!add_warning
	      (p, IZ(p, &pos), code, pending->position,
	       &p->checked_return.counter,
	       "Variable '%s' stores return value of %s but partial transfer is not checked.",
	       pending->variable, pending->function))
	    return (false);
	}
    }
  p->checked_return_state.nbr_pending = 0;
  return (true);
}

char			*strcasestr(const char			*haystack,
				    const char			*needle);

#ifdef			FTRACE
# undef			FTRACE
static int		fdepth;
static void		fdebug(const char			*func,
			       const char			*code,
			       size_t				pos)
{
  int			i;

  for (i = 0; i < fdepth; ++i)
    printf(" ");
  printf("%s - %zd ", func, pos);
  for (i = 0; code[i] && code[i] != '\n' && code[i] != '\r'; ++i);
  printf("%.*s\n", i, code);
  fdepth += 1;
}

# define		FTRACE(c, a)				\
  fdebug(__PRETTY_FUNCTION__, &(c)[a], a)
# define		FADD()					\
  fdepth += 1
# define		FRETURN(a)				\
  return (fdepth -= 1, a)
#else
# define		FTRACE(c, a)
# define		FADD()
# define		FRETURN(a)				\
  return (a)
#endif

#define			MSG(a) p->last_error_msg[++p->last_error_id] = (a " (" STRINGIFY(__LINE__) ")" )

typedef struct		s_function_parse_checkpoint
{
  ssize_t		index;
  size_t		criteria_len;
  t_criteria		*criteria;
  t_type		new_type[8192];
  size_t		last_new_type;
  char			typedef_stack[128][SYMBOL_SIZE + 1];
  int			typedef_stack_top;
  int			func_ptr_counter;
  int			local_symbol_alignment;
  int			local_parameter_type_alignment;
  int			local_parameter_name_alignment;
  int			global_parameter_name_alignment;
  int			global_symbol_alignment;
  int			ldec_function_per_file;
  int			ldec_non_static_function_per_file;
  int			last_error_id;
  int			nbr_error_points;
  int			nbr_mistakes;
} 			t_function_parse_checkpoint;

static char		*criteria_checkpoint_start(t_parsing	*p)
{
  return ((char *)&p->function_per_file);
}

static char		*criteria_checkpoint_end(t_parsing	*p)
{
  return ((char *)&p->no_assignment + sizeof(p->no_assignment));
}

static t_criteria	*criteria_iterator_start(t_parsing	*p)
{
  return ((t_criteria *)(void *)criteria_checkpoint_start(p));
}

static t_criteria	*criteria_iterator_end(t_parsing	*p)
{
  return ((t_criteria *)(void *)criteria_checkpoint_end(p));
}

static bool		type_name_is_function(t_parsing	*p,
					      const char	*name)
{
  for (size_t i = 0; i < p->last_new_type; ++i)
    if (strcmp(p->new_type[i].name, name) == 0)
      return (p->new_type[i].is_function);
  return (false);
}

static bool		current_type_is_function(t_parsing	*p)
{
  return (type_name_is_function(p, p->last_declaration.last_type));
}

static void		mark_type_name_as_function(t_parsing	*p,
						   const char	*name)
{
  for (size_t i = 0; i < p->last_new_type; ++i)
    if (strcmp(p->new_type[i].name, name) == 0)
      {
	p->new_type[i].is_function = true;
	return ;
      }
}

static t_function_parse_checkpoint *create_function_parse_checkpoint(t_parsing	*p,
							    ssize_t	index)
{
  t_function_parse_checkpoint	*checkpoint;

  if ((checkpoint = malloc(sizeof(*checkpoint))) == NULL)
    return (NULL);
  checkpoint->criteria_len = (size_t)(criteria_checkpoint_end(p) - criteria_checkpoint_start(p));
  if ((checkpoint->criteria = malloc(checkpoint->criteria_len)) == NULL)
    {
      free(checkpoint);
      return (NULL);
    }
  checkpoint->index = index;
  memcpy(checkpoint->criteria, criteria_checkpoint_start(p), checkpoint->criteria_len);
  memcpy(&checkpoint->new_type[0], &p->new_type[0], sizeof(p->new_type));
  checkpoint->last_new_type = p->last_new_type;
  memcpy(&checkpoint->typedef_stack[0], &p->typedef_stack[0], sizeof(p->typedef_stack));
  checkpoint->typedef_stack_top = p->typedef_stack_top;
  checkpoint->func_ptr_counter = p->func_ptr_counter;
  checkpoint->local_symbol_alignment = p->local_symbol_alignment;
  checkpoint->local_parameter_type_alignment = p->local_parameter_type_alignment;
  checkpoint->local_parameter_name_alignment = p->local_parameter_name_alignment;
  checkpoint->global_parameter_name_alignment = p->global_parameter_name_alignment;
  checkpoint->global_symbol_alignment = p->global_symbol_alignment;
  checkpoint->ldec_function_per_file = p->ldec_function_per_file;
  checkpoint->ldec_non_static_function_per_file = p->ldec_non_static_function_per_file;
  checkpoint->last_error_id = p->last_error_id;
  checkpoint->nbr_error_points = p->nbr_error_points;
  checkpoint->nbr_mistakes = p->nbr_mistakes;
  return (checkpoint);
}

static void		restore_function_parse_checkpoint(t_parsing			*p,
						  const t_function_parse_checkpoint *checkpoint,
						  ssize_t			*i)
{
  *i = checkpoint->index;
  memcpy(criteria_checkpoint_start(p), checkpoint->criteria, checkpoint->criteria_len);
  memcpy(&p->new_type[0], &checkpoint->new_type[0], sizeof(p->new_type));
  p->last_new_type = checkpoint->last_new_type;
  memcpy(&p->typedef_stack[0], &checkpoint->typedef_stack[0], sizeof(p->typedef_stack));
  p->typedef_stack_top = checkpoint->typedef_stack_top;
  p->func_ptr_counter = checkpoint->func_ptr_counter;
  p->local_symbol_alignment = checkpoint->local_symbol_alignment;
  p->local_parameter_type_alignment = checkpoint->local_parameter_type_alignment;
  p->local_parameter_name_alignment = checkpoint->local_parameter_name_alignment;
  p->global_parameter_name_alignment = checkpoint->global_parameter_name_alignment;
  p->global_symbol_alignment = checkpoint->global_symbol_alignment;
  p->ldec_function_per_file = checkpoint->ldec_function_per_file;
  p->ldec_non_static_function_per_file = checkpoint->ldec_non_static_function_per_file;
  p->last_error_id = checkpoint->last_error_id;
  p->nbr_error_points = checkpoint->nbr_error_points;
  p->nbr_mistakes = checkpoint->nbr_mistakes;
}

static void		delete_function_parse_checkpoint(t_function_parse_checkpoint	*checkpoint)
{
  if (checkpoint)
    free(checkpoint->criteria);
  free(checkpoint);
}


static const char	*gl_first_char = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN_";
static const char	*gl_second_char = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN_0123456789";

int			crawler_stop(void)
{
  // Ici, on s'arrete.
  FRETURN (-1);
}

void			reset_last_declaration(t_parsing	*p)
{
  memset(&p->last_declaration, 0, sizeof(p->last_declaration));
}

static bool		check_read_text(const char	*code,
					ssize_t		*i,
					const char	*target)

{
  ssize_t		j;

  j = *i;
  if (!(bunny_read_text(code, &j, target) && !strchr(gl_second_char, code[j])))
    return(false);
  *i = j;
  return(true);
}



static void		crawler_map_copy_symbol(char		*target,
					const char	*source,
					size_t		 target_size)
{
  size_t		len;

  if (target == NULL || target_size == 0)
    return ;
  if (source == NULL)
    source = "";
  len = strlen(source);
  if (len >= target_size)
    len = target_size - 1;
  memcpy(target, source, len);
  target[len] = '\0';
}

static bool		crawler_map_identifier_is_reserved(const char	*symbol)
{
  size_t		i;

  for (i = 0; i < NBRCELL(keywords); ++i)
    if (strcmp(symbol, keywords[i]) == 0)
      return (true);
  for (i = 0; i < NBRCELL(standard_types); ++i)
    if (strcmp(symbol, standard_types[i].name) == 0)
      return (true);
  return (false);
}

static bool		crawler_map_read_identifier_at(const char	*code,
					       ssize_t		pos,
					       char		out[SYMBOL_SIZE + 1],
					       ssize_t		*end)
{
  ssize_t		j;
  ssize_t		start;
  size_t		len;

  if (out)
    out[0] = '\0';
  j = pos;
  read_whitespace(code, &j);
  if (strchr(gl_first_char, code[j]) == NULL)
    return (false);
  start = j;
  j += 1;
  while (strchr(gl_second_char, code[j]) != NULL)
    j += 1;
  len = (size_t)(j - start);
  if (len == 0 || len > SYMBOL_SIZE)
    return (false);
  if (out)
    {
      memcpy(out, &code[start], len);
      out[len] = '\0';
      if (crawler_map_identifier_is_reserved(out))
	{
	  out[0] = '\0';
	  return (false);
	}
    }
  if (end)
    *end = j;
  return (true);
}

static bool		crawler_map_read_pointer_donation(t_parsing	*p,
						 const char	*code,
						 ssize_t	pos,
						 char		out[SYMBOL_SIZE + 1])
{
  ssize_t		j;
  bool			forced;

  if (out)
    out[0] = '\0';
  j = pos;
  read_whitespace(code, &j);
  forced = bunny_read_text(code, &j, "&");
  if (!crawler_map_read_identifier_at(code, j, out, &j))
    return (false);
  read_whitespace(code, &j);
  if (bunny_check_text(code, &j, "("))
    return (false);
  if (forced)
    return (true);
  return (crawler_function_map_has_function(p, out));
}

static int		handle_typedef(t_parsing	*p,
				       const char	*code,
				       ssize_t		*i,
				       bool		rdid)
{
  // Si on est pas dans un typedef...
  if (!p->last_declaration.is_typedef)
    FRETURN(0);
  // Si on est dans la définition d'attributs ou de constantes
  // if (p->last_declaration.was_defining)
  // return (0);
  ssize_t		j = *i;
  bool			rd = false;;

  FTRACE(code, j);
  if (rdid)
    {
      read_whitespace(code, &j);
      if (p->last_declaration.is_func_ptr)
	{
	  rd = bunny_read_text(code, &j, "("); /// Il faut, si on a trouvé ca, indiquer
      // qu'on est plus dans le typedef.
      // Le probleme actuel c'est qu'on resoud pas le type sur un ptr sur fonction
      // et que du coup c'est le dernier paramètre du ptr sur fonction qui sert
      // de nom
	  read_pointer(p, code, &j);
	}
	  gl_bunny_read_whitespace = NULL;
      if (read_identifier(p, code, &j, false) == 0)
	{
	  // Si il n'y a pas de symbole...
	  // Ce qui peut signifier que le symbole du typedef a été mangé parceque le type
	  // était deja connu, a cause d'un typedef redondant.
	  gl_bunny_read_whitespace = read_whitespace;
	  // return (-1);
	  FRETURN (0);
	}
      gl_bunny_read_whitespace = read_whitespace;
      if (rd && bunny_read_text(code, &j, ")") == false)
	RETURN("A matching ')' was expected to close opening '(*' for function pointer."); // LCOV_EXCL_LINE
      // j = *i; /////////////////////////////////
    }

  // p->last_declaration.is_typedef = false;
  // Pourquoi mettre à faux ? ^

  
  if (check_style(p, "typedef", &p->last_declaration.symbol[0],
		  &p->typedef_style, &p->typedef_infix,
		  code, j) == false)
    RETURN("Memory exhausted."); // LCOV_EXCL_LINE

  if (p->typedef_matching.active &&
      (p->last_declaration.is_struct_last_typedef ||
       p->last_declaration.is_union_last_typedef ||
       p->last_declaration.is_enum_last_typedef))
    {
      p->last_declaration.is_struct_last_typedef = false;
      p->last_declaration.is_union_last_typedef = false;
      p->last_declaration.is_enum_last_typedef = false;
      char buffer[SYMBOL_SIZE + 1];
      
      store_real_typename(p, buffer, &p->last_declaration.symbol[0], sizeof(buffer), 2);
      if (p->typedef_stack_top <= 0)
	RETURN("Typedef matching stack is empty."); // LCOV_EXCL_LINE
      p->typedef_stack_top -= 1;
      if (strcmp(buffer, p->typedef_stack[p->typedef_stack_top]) != 0)
	{
	  if (!add_warning
	      (p, IZ(p, &j), code, j, &p->typedef_matching.counter,
	       "Typedef name '%s' does not match the typedefed type name '%s'.",
	       buffer, p->typedef_stack[p->typedef_stack_top]
	       ))
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	}
    }
  int anonym = p->last_declaration.was_named ? -1 : 0;
  
  gl_bunny_read_whitespace = read_whitespace;

  // EST-CE QUE C'EST NORMAL QU'IL Y EST DEUX FOIS ADD_NEW_TYPE()
  //
  //
  if (p->last_declaration.was_defining)
    {
      p->new_type[p->last_new_type + anonym].size =
	p->last_declaration.cumulated_attribute_size;
      add_new_type
	(p, p->last_declaration.symbol, p->last_declaration.cumulated_attribute_size);
    }
  add_new_type(p, p->last_declaration.symbol, p->last_declaration.last_type_size);

  FRETURN (0);
}

static bool		bad_style(t_parsing			*p,
				  const char			*context,
				  const char			*symbol,
				  t_criteria			*style,
				  const char			*code,
				  int				pos)
{
  static const char	*sname[] =
    {"uppercased snake case", "snake case", "camel case", "pascal case"};

  FTRACE(code, pos);
  if (style->value < MIXED_CASE || style->value > PASCAL_CASE)
    style->value = SNAKE_CASE;
  FRETURN (add_warning
	   (p, true, code, pos, &style->counter,
	    "Badly styled symbol %s. Expected style was %s for %s.",
	    symbol, sname[style->value], context));
}

static bool		bad_infix(t_parsing			*p,
				  const char			*context,
				  const char			*symbol,
				  t_string_criteria		*infix,
				  const char			*code,
				  int				pos)
{
  FTRACE(code, pos);
  FRETURN (add_warning
	   (p, true, code, pos, &infix->counter,
	    "Missing %s %s for %s in symbol %s.",
	    infix->position == 0 ? "prefix" : "suffix",
	    &infix->value[0], context, symbol));
}

bool			check_style(t_parsing			*p,
				    const char			*context,
				    const char			*symbol,
				    t_criteria			*style,
				    t_string_criteria		*infix,
				    const char			*code,
				    ssize_t			pos)
{
  int			up = 0;
  int			down = 0;
  int			i;

  FTRACE(code, pos);
  if (style->active)
    {
      switch (style->value)
	{
	case MIXED_CASE:
	  for (i = 0; symbol[i]; ++i)
	    if (symbol[i] == '_')
	      {
		down += 1;
		if (i > 0 && symbol[i - 1] == '_')
		  {
		    if (bad_style(p, context, symbol, style, code, pos) == false)
		      FRETURN (false);
		    break ;
		  }
	      }
	    else if (isupper(symbol[i]) == false && isdigit(symbol[i]) == false)
	      {
		if (bad_style(p, context, symbol, style, code, pos) == false)
		  FRETURN (false);
		break ;
	      }
	  if (i > 12 && down == 0) // Un long nom et pas d'underscore ? Bizarre...
	    if (bad_style(p, context, symbol, style, code, pos) == false)
	      FRETURN (false);
	  break ;
	default:
	case SNAKE_CASE:
	  for (i = 0; symbol[i]; ++i)
	    if (symbol[i] == '_')
	      {
		down += 1;
		if (i > 0 && symbol[i - 1] == '_')
		  {
		    if (bad_style(p, context, symbol, style, code, pos) == false)
		      FRETURN (false);
		    break ;
		  }
	      }
	    else if (islower(symbol[i]) == false && isdigit(symbol[i]) == false)
	      {
		if (bad_style(p, context, symbol, style, code, pos) == false)
		  FRETURN (false);
		break ;
	      }
	  if (i > 12 && down == 0) // Un long nom et pas d'underscore ? Bizarre...
	    if (bad_style(p, context, symbol, style, code, pos) == false)
	      FRETURN (false);
	  break ;
	case CAMEL_CASE:
	  if (islower(symbol[0]) == false)
	    {
	      if (bad_style(p, context, symbol, style, code, pos) == false)
		FRETURN (false);
	    }
	  else
	    {
	      for (i = 1; symbol[i]; ++i)
		if (islower(symbol[i]))
		  down += 1;
		else if (isupper(symbol[i]))
		  up += 1;
		else if (isdigit(symbol[i]))
		  {}
		else
		  {
		    if (bad_style(p, context, symbol, style, code, pos) == false)
		      FRETURN (false);
		    break ;
		  }
	      if (!symbol[i])
		if (up * 2 > down) // Il y a trop de majuscule la dedans... étrange.
		  if (bad_style(p, context, symbol, style, code, pos) == false)
		    FRETURN (false);
	    }
	  break ;
	case PASCAL_CASE:
	  if (isupper(symbol[0]) == false)
	    {
	      if (bad_style(p, context, symbol, style, code, pos) == false)
		FRETURN (false);
	    }
	  else
	    {
	      for (i = 1; symbol[i]; ++i)
		if (islower(symbol[i]))
		  down += 1;
		else if (isupper(symbol[i]))
		  up += 1;
		else if (isdigit(symbol[i]))
		  {}
		else
		  {
		    if (bad_style(p, context, symbol, style, code, pos) == false)
		      FRETURN (false);
		    break ;
		  }
	      if (!symbol[i])
		if (up * 2 > down) // Il y a trop de majuscule la dedans... étrange.
		  if (bad_style(p, context, symbol, style, code, pos) == false)
		    FRETURN (false);
	    }
	  break ;
	}
    }

  if (strcmp(context, "function") == 0 &&
      (bunny_strncasecmp(symbol, "test_", 5) == 0 ||
       strncmp(symbol, "main", 5) == 0))
    FRETURN (true);
  if (infix->active)
    {      
      char		*s = strcasestr(symbol, &infix->value[0]);

      if (infix->position == 0) // Prefixe
	{
	  if (s != symbol || s == NULL) // On est pas au début...
	    if (bad_infix(p, context, symbol, infix, code, pos) == false)
	      FRETURN (false);
	}
      else if (infix->position == 1) // Suffixe
	{
	  if (s == NULL || bunny_strcasecmp(s, &infix->value[0]) != 0)
	    if (bad_infix(p, context, symbol, infix, code, pos) == false)
	      FRETURN (false);
	}
    }

  FRETURN (true);
}

int			read_identifier(t_parsing		*p,
					const char		*code,
					ssize_t			*i,
					bool			kwx)
{
  ssize_t		j = *i;
  size_t		x;

  FTRACE(code, *i);
  if (kwx == false)
    {
      // On cherche si c'est un mot clef...
      for (x = 0; x < NBRCELL(keywords) && bunny_read_text(code, &j, keywords[x]) == false; ++x);
      if (x != NBRCELL(keywords) && (code[j] == '\0' || strchr(gl_second_char, code[j]) == NULL)) // Au cas ou ce soit... "ifa" par exemple.
	FRETURN (0);
      for (x = 0; x < NBRCELL(standard_types) && bunny_read_text(code, &j, standard_types[x].name) == false; ++x);
      if (x != NBRCELL(standard_types) && (code[j] == '\0' || strchr(gl_second_char, code[j]) == NULL)) // Au cas ou ce soit... "ifa" par exemple.
	FRETURN (0);
    }

  read_whitespace(code, i);
  j = *i;
  if (bunny_read_char(code, i, gl_first_char) == false)
    FRETURN (0);
  // On désactive la lecture d'espace pour eviter les lectures du style
  // int i
  // qui aurait été mangé sans ca
  gl_bunny_read_whitespace = NULL;
  bunny_read_char(code, i, gl_second_char);
  gl_bunny_read_whitespace = read_whitespace;

  // On enregistre le symbole pour en analyser le style en fonction de son usage
  if (*i - j + 1 < (int)sizeof(p->last_declaration.symbol))
    {
      memcpy(&p->last_declaration.symbol[0], &code[j], *i - j);
      p->last_declaration.symbol[*i - j] = '\0';
    }
  else
    p->last_declaration.symbol[0] = '\0'; // LCOV_EXCL_LINE

  // On verifie la validité du nom
  // Si on monitore les noms trops courts de parametre
  // et qu'on allait déclarer un paramètre ou une variable
  // ou un attribut
  if (p->no_short_name.active
      && (p->last_declaration.inside_parameter
	  || (p->last_declaration.inside_variable &&
	      !p->last_declaration.inside_parameter)
	  || p->last_declaration.inside_function_name
	  || p->last_declaration.inside_struct
	  || p->last_declaration.inside_union
	  )
      && (int)strlen(&p->last_declaration.symbol[0]) < p->no_short_name.value)
    {
      int z;
      const char *valid[] =
	{
	  "i", "j", "k", "cnt", "ret", "end", "go",
	  "win", "nbr", "val", "res", "x", "y", "z", "fd", "pip",
	  "w", "h", "d", "wx", "wy", "wz", "hz", "hz", "ms",
	  "us", "ns", "obj", "len", "str", "mem", "ptr",
	  "cnf", "min", "max", "top", "key", "kg", "km", "ts", "src"
	};
      for (z = 0; z < (int)NBRCELL(valid); ++z)
	if (strcmp(&p->last_declaration.symbol[0], valid[z]) == 0)
	  break ;
      if (z == (int)NBRCELL(valid))
	if (!add_warning(p, IZ(p, i), code, *i, &p->no_short_name.counter,
			 "The name '%s' is too short. Minimum was %d.",
			 &p->last_declaration.symbol[0], p->no_short_name.value))
	  RETURN ("Memory exhausted"); // LCOV_EXCL_LINE
    }

  FRETURN (1);
}

int			read_identifier_list(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);
  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_identifier(p, code, i, false)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    FRETURN (ret);
	  RETURN("Excessive ',' found in declaration."); // LCOV_EXCL_LINE
	}
      else
	cnt += 1;
    }
  while (bunny_read_text(code, i, ","));
  FRETURN (cnt >= 1 ? 1 : 0);
}

int			read_labeled_statement(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  ssize_t		j = *i;

  FTRACE(code, *i);
  if (read_identifier(p, code, &j, false))
    {
      if (!bunny_read_text(code, &j, ":"))
	FRETURN (0);
      if (check_white_then_newline(p, code, j, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      *i = j;
      FRETURN (read_statement(p, code, i));
    }
  if (bunny_read_text(code, i, "case"))
    {
      if (read_constant_expression(p, code, i) != 1)
	RETURN ("Missing symbol after 'case'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ":"))
	RETURN ("Missing token ':' after symbol used by 'case'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      FRETURN (read_statement(p, code, i));
    }
  if (bunny_read_text(code, i, "default"))
    {
      if (!bunny_read_text(code, i, ":"))
	RETURN ("Missing token ':' after 'default'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      FRETURN (read_statement(p, code, i));
    }
  FRETURN (0);
}

int			read_selection_statement(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  FTRACE(code, *i);
  if (bunny_read_text(code, i, "if"))
    {
      if (p->maximum_if_in_function.active && p->maximum_if_in_function.value == 0)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->maximum_if_in_function.counter, "'if' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'if'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value == 0 && !check_parenthesis_space
	  (p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      ssize_t checked_return_condition_start = *i;
      if (read_expression(p, code, i, false) != 1)
	RETURN ("Missing condition after 'if ('."); // LCOV_EXCL_LINE
      if (!crawler_checked_return_condition
	  (p, code, checked_return_condition_start, *i))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'if (condition'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value == 0 && !check_parenthesis_space
	  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.after_statement = true;
      if ((p->last_declaration.nbr_if += 1) > p->maximum_if_in_function.value &&
	  p->maximum_if_in_function.value != 0)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->maximum_if_in_function.counter,
	     "The maximum amount of if authorized was %d.",
	     p->maximum_if_in_function.value))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      source_report_enter_control(p);
      if (read_statement(p, code, i) != 1)
	{
	  source_report_leave_control(p);
	  RETURN ("Missing statement after 'if (condition)'."); // LCOV_EXCL_LINE
	}
      source_report_leave_control(p);
      if (bunny_read_text(code, i, "else"))
	{
	  int		elsefix = 0;
	  ssize_t	j = *i;

	  if (bunny_read_text(code, &j, "if") == false)
	    {
	      if (p->indent_style.value == GNU_STYLE)
		{
		  if (bunny_check_text(code, &j, "{") == false)
		    {
		      elsefix = 1;
		      p->last_declaration.depth_bonus += 1;
		    }
		  else
		    {
		      elsefix = 2;
		      p->last_declaration.indent_depth += 1;
		    }
		}
	      else if (bunny_check_text(code, &j, "{") == false)
		{
		  elsefix = 1;
		  p->last_declaration.depth_bonus += 1;
		}
	    }
	  if (!check_on_same_line(p, code, *i - 5, "}", false))
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	  if (!check_on_same_line(p, code, *i, "{", true))
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	  p->last_declaration.after_statement = true;
	  if (p->else_forbidden.value)
	    if (!add_warning
		(p, IZ(p, i), code, *i, &p->else_forbidden.counter,
		 "'else' is a forbidden statement."))
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (check_base_indentation(p, code, *i) == -1)
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	  j = *i;
	  if (!bunny_check_text(code, &j, "if") &&
	      check_white_then_newline(p, code, *i, true) == false)
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (elsefix == 2)
	    p->last_declaration.indent_depth -= 1;
	  if (elsefix == 1)
	    p->last_declaration.depth_bonus -= 1;
	  p->last_declaration.after_statement = false;
	  source_report_enter_control(p);
	  if (read_statement(p, code, i) != 1)
	    {
	      source_report_leave_control(p);
	      RETURN ("Missing statement after 'else'."); // LCOV_EXCL_LINE
	    }
	  source_report_leave_control(p);
	}
      source_report_add_instruction(p, SOURCE_REPORT_BRANCH);
      FRETURN (1);
    }
  if (bunny_read_text(code, i, "switch"))
    {
      if (p->switch_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->switch_forbidden.counter,
	     "'switch' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'switch'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      ssize_t checked_return_condition_start = *i;
      if (read_expression(p, code, i, false) != 1)
	RETURN ("Missing expression after 'switch ('."); // LCOV_EXCL_LINE
      if (!crawler_checked_return_condition
	  (p, code, checked_return_condition_start, *i))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'switch (expression'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.after_statement = true;
      source_report_enter_control(p);
      if (read_statement(p, code, i) != 1)
	{
	  source_report_leave_control(p);
	  RETURN ("Missing statement after 'switch (expression)'."); // LCOV_EXCL_LINE
	}
      source_report_leave_control(p);
      source_report_add_instruction(p, SOURCE_REPORT_BRANCH);
      FRETURN (1);
    }
  FRETURN (0);
}

int			read_iteration_statement(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  FTRACE(code, *i);
  if (check_read_text(code, i, "while"))
    {
      if (p->while_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->while_forbidden.counter,
	     "'while' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'while'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      ssize_t checked_return_condition_start = *i;
      if (read_expression(p, code, i, false) != 1)
	RETURN ("Missing condition after 'while ('."); // LCOV_EXCL_LINE
      if (!crawler_checked_return_condition
	  (p, code, checked_return_condition_start, *i))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'while(condition'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      ssize_t j = *i; // Pour restaurer y compris les blancs sautés
      bool single_line_while = bunny_read_text(code, i, ";");

      if (!single_line_while)
	*i = j;
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!single_line_while)
	{
	  p->last_declaration.after_statement = true;
	  source_report_enter_control(p);
	  if (read_statement(p, code, i) != 1)
	    {
	      source_report_leave_control(p);
	      RETURN ("Missing statement after 'while (condition)'."); // LCOV_EXCL_LINE
	    }
	  source_report_leave_control(p);
	}
      source_report_add_instruction(p, SOURCE_REPORT_LOOP);
      FRETURN (1);
    }
  if (check_read_text(code, i, "do"))
    {
      if (p->do_while_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->do_while_forbidden.counter,
	     "'do' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.after_statement = true;
      source_report_enter_control(p);
      if (read_statement(p, code, i) != 1)
	{
	  source_report_leave_control(p);
	  RETURN ("Missing statement after 'do'."); // LCOV_EXCL_LINE
	}
      source_report_leave_control(p);
      if (!bunny_read_text(code, i, "while"))
	RETURN ("Missing 'while' after 'do statement'."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after after 'do statement while'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      ssize_t checked_return_condition_start = *i;
      if (read_expression(p, code, i, false) != 1)
	RETURN ("Missing condition after 'do statement while ('."); // LCOV_EXCL_LINE
      if (!crawler_checked_return_condition
	  (p, code, checked_return_condition_start, *i))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'do statement while (condition'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'do statement while (condition)'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      source_report_add_instruction(p, SOURCE_REPORT_LOOP);
      FRETURN (1);
    }
  if (check_read_text(code, i, "for"))
    {
      if (p->for_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->for_forbidden.counter,
	     "'for' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'for'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.inside_for_statement = true;
      if (read_expression_statement(p, code, i) != 1)
	RETURN ("Missing initialization after 'for ('."); // LCOV_EXCL_LINE
      if (read_expression_statement(p, code, i) != 1)
	RETURN ("Missing condition after 'for (initialization;'."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i, false) == -1)
	RETURN ("Invalid increment after 'for (initialization; condition;'."); // LCOV_EXCL_LINE
      p->last_declaration.inside_for_statement = false;
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'for (initialization; condition; increment'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      ssize_t j = *i; // Pour restaurer y compris les blancs sautés
      bool single_line_for = bunny_read_text(code, i, ";");

      if (!single_line_for)
	*i = j;
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!single_line_for)
	{
	  p->last_declaration.after_statement = true;
	  source_report_enter_control(p);
	  if (read_statement(p, code, i) != 1)
	    {
	      source_report_leave_control(p);
	      RETURN ("Missing statement after 'for (initialization; condition; increment)'."); // LCOV_EXCL_LINE
	    }
	  source_report_leave_control(p);
	}
      source_report_add_instruction(p, SOURCE_REPORT_LOOP);
      FRETURN (1);
    }
  FRETURN (0);
}

int			read_jump_statement(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  FTRACE(code, *i);
  if (check_read_text(code, i, "goto"))
    {
      if (p->goto_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->goto_forbidden.counter,
	     "'goto' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (!read_identifier(p, code, i, false))
	RETURN ("Missing symbol after 'goto'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'goto symbol'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, false) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      source_report_add_instruction(p, SOURCE_REPORT_JUMP);
      FRETURN (1);
    }
  if (check_read_text(code, i, "continue"))
    {
      if (p->continue_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->continue_forbidden.counter,
	     "'continue' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'continue'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, false) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      source_report_add_instruction(p, SOURCE_REPORT_JUMP);
      FRETURN (1);
    }
  if (check_read_text(code, i, "break"))
    {
      if (p->break_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->break_forbidden.counter,
	     "'break' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'break'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, false) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      source_report_add_instruction(p, SOURCE_REPORT_JUMP);
      FRETURN (1);
    }
  if (check_read_text(code, i, "return"))
    {
      bool		flag = false;

      if (p->return_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->return_forbidden.counter,
	     "'return' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      // Pas d'expression
      if (bunny_read_text(code, i, ";"))
	{
	  source_report_add_instruction(p, SOURCE_REPORT_RETURN);
	  FRETURN (1);
	}
      // Une expression
      if (p->return_parenthesis.active && bunny_check_text(code, i, "(") == false)
	{
	  flag = true;
	  if (!add_warning
	      (p, IZ(p, i), code, *i, &p->return_parenthesis.counter,
	       "There must be parenthesis around the FRETURN expression."))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (read_expression(p, code, i, false) == -1)
	RETURN ("Missing expression or ';' after 'FRETURN'."); // LCOV_EXCL_LINE
      if (p->return_parenthesis.active && flag == false)
	{
	  int j;

	  for (j = *i; j >= 0 && (isspace(code[j]) || code[j] == ';'); --j);
	  if (code[j] != ')')
	    if (!add_warning
		(p, IZ(p, i), code, *i, &p->return_parenthesis.counter,
		 "There must be parenthesis around the FRETURN expression."))
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'FRETURN expression'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, false) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      source_report_add_instruction(p, SOURCE_REPORT_RETURN);
      FRETURN (1);
    }
  FRETURN (0);
}

int			read_expression_statement(t_parsing	*p,
						  const char	*code,
						  ssize_t	*i)
{
  int			ret;
  int			sc;
  ssize_t		expression_start;
  ssize_t		expression_end;

  FTRACE(code, *i);
  expression_start = *i;
  if ((ret = read_expression(p, code, i, true)) == -1)
    FRETURN (-1);
  expression_end = *i;
  if (!(sc = bunny_read_text(code, i, ";")) && ret == 1)
    RETURN ("Missing ';' after expression."); // LCOV_EXCL_LINE
  if (ret == 1)
    {
      if (p->last_declaration.inside_for_statement == false &&
	  !crawler_checked_return_expression_statement
	  (p, code, expression_start, expression_end))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (p->last_declaration.inside_for_statement == false)
	source_report_add_instruction(p, SOURCE_REPORT_EXPRESSION);
      p->last_declaration.scope_length += 1;
      if (p->last_declaration.inside_for_statement == false &&
	  check_white_then_newline(p, code, *i, false) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
    }
  FRETURN (ret + sc >= 1 ? 1 : 0);
}

int			read_statement(t_parsing		*p,
				       const char		*code,
				       ssize_t			*i)
{
  bool			singleindent = false;
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_labeled_statement(p, code, i)) != 0)
    FRETURN (ret);

  read_whitespace(code, i);
  if (!bunny_check_text(code, i, "{"))
    {
      if (p->last_declaration.after_statement &&
	  p->always_braces.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->always_braces.counter,
	     "'{' is mandatory after if, while, do, for or switch statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE

      // On indente pareil dans tous les styles, tant que c'est pas une ouverture de fonction;
      if (p->last_declaration.after_statement)
	{
	  p->last_declaration.depth_bonus += 1;
	  singleindent = true;
	}
    }
  p->last_declaration.after_statement = false;
  p->last_declaration.scope_length = 0;
  if ((ret = read_compound_statement(p, code, i)) != 0)
    goto FRETURN;
  if ((ret = read_assembler(p, code, i)) != 0)
    {
      if (ret > 0)
	if (bunny_read_text(code, i, ";") == false)
	  RETURN("Missing ';' after asm declaration.");
      goto FRETURN;
    }
  if ((ret = read_selection_statement(p, code, i)) != 0)
    goto FRETURN;
  if ((ret = read_iteration_statement(p, code, i)) != 0)
    goto FRETURN;
  if ((ret = read_jump_statement(p, code, i)) != 0)
    goto FRETURN;
  if ((ret = read_expression_statement(p, code, i)) != 0)
    goto FRETURN;
  ret = 0;
 FRETURN:
  if (singleindent)
    p->last_declaration.depth_bonus -= 1;
  FRETURN (ret);
}

int			read_statement_list(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);
  while ((ret = read_statement(p, code, i)) == 1)
    cnt += 1;
  if (ret == -1)
    FRETURN (-1);
  FRETURN (cnt >= 1 ? 1 : 0);
}

int			read_compound_statement(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  int			ret;
  int			begin;
  int			end;
  bool			separator;

  FTRACE(code, *i);
  read_whitespace(code, i);
  // On ne mange pas tout de suite l'accolade, on va d'abord verifier son positionnement
  if (bunny_check_text(code, i, "{"))
    {
      // Si on est pas en mode "if () {"
      if (p->base_indent.active && p->indent_style.value != KNR_STYLE)
	{
	  // On verifie que l'accolade est bien seule sur sa ligne
	  if ((ret = check_is_alone(p, "{", code, *i)) == -1)
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE

	  // Style GNU, on indente avant { si on est dans le scope global
	  if (p->indent_style.value == GNU_STYLE
	      && p->last_declaration.indent_depth != 0)
	    p->last_declaration.indent_depth += 1;

	  // On verifie l'indentation de l'accolade
	  if (check_base_indentation(p, code, *i) == -1)
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	}
      else if (p->last_declaration.indent_depth != 0
	       && !check_on_same_line(p, code, *i, "{", true))
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
    }
  if (bunny_read_text(code, i, "{") == false)
    FRETURN (0);
  // On augmente l'indentation
  p->last_declaration.indent_depth += 1;
  int			fnd;

  // Si on est en C ANSI, on a qu'un seul bloc de declaration de variable AU DEBUT
  // Après, on peut déclarer des blocs de variables n'importe ou.
  begin = *i;
  separator = 0;
  do
    {
      int		ok = 0;

      fnd = 0;
      read_whitespace(code, i);
      ret = *i;
      if ((ok = read_declaration_list(p, code, i)) == -1)
	FRETURN (-1);
      fnd += ok;

      // Il y a eu des déclarations et on veut qu'il y ai une ligne de séparation
      if (ok > 0 && p->declaration_statement_separator.active && ret != *i)
	{
	  int	j = *i;
	  int	nl = 0;

	  // On va remonter tant qu'il y a des espaces
	  while (j > 0 && isspace(code[j - 1]))
	    j -= 1;
	  // Puis on repart a l'endroit
	  while (j != *i)
	    {
	      if (code[j] == '\n')
		if ((nl += 1) == 2)
		  p->last_declaration.end_of_declaration = j;
	      j += 1;
	    }
	  if (nl <= 1)
	    {
	      if (!add_warning
		  (p, IZ(p, i), code, *i, &p->declaration_statement_separator.counter,
		   "An empty line was expected between variable declaration and "
		   "statement."))
		RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	    }
	  else
	    separator += 1;
	}
      if ((ok = read_statement_list(p, code, i)) == -1)
	FRETURN (-1);
      fnd += ok;
    }
  while (!p->ansi_c && fnd); // Si on est pas ANSI et qu'on a trouvé un truc...
  end = *i;

  // On diminue l'indentation
  if (p->last_declaration.indent_depth > 0)
    p->last_declaration.indent_depth -= 1;

  read_whitespace(code, i);
  // On verifie l'indentation de l'accolade
  if (check_base_indentation(p, code, *i) == -1)
    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
  if (p->indent_style.value != KNR_STYLE)
    if ((check_is_alone(p, "}", code, *i)) == -1)
      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  if (bunny_read_text(code, i, "}") == false)
    RETURN ("Missing '}' after '{ values'."); // LCOV_EXCL_LINE
  if (p->avoid_braces.active && p->last_declaration.scope_length > 1)
    if (!add_warning
	(p, IZ(p, i), code, *i, &p->avoid_braces.counter,
	 "Braces '{' '}' are forbidden for single line scopes."))
      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  if (check_white_then_newline(p, code, *i, false) == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE

  // Style GNU, on desindente après si on était pas dans le scope global
  if (p->base_indent.value && p->indent_style.value == GNU_STYLE)
    p->last_declaration.indent_depth -= 1;

  // On verifie l'absence de ligne vide, en dehors du séparator
  if (p->no_empty_line_in_function.value)
    if (check_no_empty_line(p, code, *i, separator, begin, end) == -1)
      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
  // Désactivé. On ne verifie pas la largeur après le préprocesseur mais AVANT
  if (0 && p->max_column_width.value)
    if (check_line_width(p, code, begin, end) == -1)
      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
  if (p->max_function_length.value)
    if (check_function_length(p, code, begin, end) == -1)
      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
  FRETURN (1);
}

int			read_declaration_list(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);

  p->last_declaration.inside_variable = true;
  while ((ret = read_declaration(p, code, i)) == 1)
    cnt += 1;
  p->last_declaration.inside_variable = false;
  if (ret == -1)
    FRETURN (-1);
  FRETURN (cnt >= 1 ? 1 : 0);
}

int			read_function_definition(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  t_function_parse_checkpoint	*checkpoint;
  ssize_t		j;
  int			ret;

  FTRACE(code, *i);
  if ((checkpoint = create_function_parse_checkpoint(p, *i)) == NULL)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  reset_last_declaration(p);
  p->func_ptr_counter = 0;
  
  // Le type de retour
  if (read_declaration_specifiers(p, code, i, true) == -1)
    {
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (-1);
    }
  if (read_gcc_attribute(p, code, i) == -1)
    {
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (-1);
    }

  // Une définition de struct/union/enum complète au niveau global n'est
  // pas une définition de fonction. Ne pas continuer la tentative
  // spéculative, sinon les détails GNU des headers système peuvent
  // transformer une simple déclaration de type en erreur fatale.
  if (p->last_declaration.was_defining)
    {
      restore_function_parse_checkpoint(p, checkpoint, i);
      reset_last_declaration(p);
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (0);
    }

  // L'indentation du nom de fonction
  read_whitespace(code, i);
  p->local_symbol_alignment = count_to_new_line(p, code, *i);


  // On règle des cas gênants généré par typedef
  if (p->last_declaration.is_func_ptr)
    {
      if (!bunny_read_text(code, i, "(") || !bunny_read_text(code, i, "*"))
	{
	  if (!add_warning(p, IZ(p, i), code, *i, NULL, "Syntax Error, missing > ( < or > * < "))
	    {
	      delete_function_parse_checkpoint(checkpoint);
	      RETURN ("Memory exhausted.");
	    }
	  delete_function_parse_checkpoint(checkpoint);
	  FRETURN(-1); // Erreur de syntaxe
	}
    }

  //////////////////////
  // Le nom de fonction
  j = *i;
  p->last_declaration.inside_function_name = true;
  p->last_declaration.inside_function_definition_attempt = true;
  if (read_declarator(p, code, i) == -1)
    {
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (-1);
    }
  p->last_declaration.inside_function_name = false;

  // GCC autorise des attributs après le déclarateur de fonction:
  // extern int f(void) __attribute__((...));
  // Pour la tentative "définition de fonction", on les consomme afin de
  // reconnaître proprement le cas prototype puis revenir en arrière.
  if ((ret = read_gcc_attribute(p, code, i)) == -1)
    {
      restore_function_parse_checkpoint(p, checkpoint, i);
      reset_last_declaration(p);
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (0);
    }
  read_whitespace(code, i);
  if (bunny_check_text(code, i, ";") || bunny_check_text(code, i, ","))
    {
      restore_function_parse_checkpoint(p, checkpoint, i);
      reset_last_declaration(p);
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (0);
    }

  // L'assignation eventuelle...
  if (read_declaration_list(p, code, i) == -1)
    {
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (-1);
    }
  // Pour savoir si les variables sont globales ou locales, par exemple...
  p->last_declaration.inside_function = true;

  if (p->func_ptr_counter > 0)
    {
      int		depth = 0;

      // On passe les potentiels func pointer
      while (code[*i] && (p->func_ptr_counter > 0 || depth > 0))
	{
	  if (code[*i] == '(')
	    {
	      if (depth == 0)
		p->func_ptr_counter -= 1;
	      depth += 1;
	    }
	  else if (depth > 0 && code[*i] == ')')
	    depth -= 1;
	  *i += 1;
	}
      if (code[*i] == '\0')
	{
	  if (!add_warning(p, IZ(p, i), code, *i, NULL, "Syntax Error, missing > ( < "))
	    {
	      delete_function_parse_checkpoint(checkpoint);
	      RETURN ("Memory exhausted.");
	    }
	  delete_function_parse_checkpoint(checkpoint);
	  FRETURN(-1); // Erreur de syntaxe // LCOV_EXCL_LINE
	}
    }
 
  // Le corps de fonction
  char			map_previous_function[SYMBOL_SIZE + 1];
  bool			map_entered = false;
  int			report_previous_function = p->source_report.current_function;
  size_t		report_previous_count = p->source_report.function_count;
  bool			report_started = false;

  read_whitespace(code, i);
  if (p->source_report.enabled && bunny_check_text(code, i, "{") &&
      p->last_declaration.function[0] != '\0')
    {
      if (!source_report_begin_function
	  (p, p->last_declaration.function, p->file, bunny_which_line(code, *i) + 1))
	{
	  delete_function_parse_checkpoint(checkpoint);
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      report_started = true;
    }
  if (p->function_map.enabled && bunny_check_text(code, i, "{") &&
      p->last_declaration.function[0] != '\0')
    {
      crawler_function_map_enter(p, p->last_declaration.function,
				 map_previous_function);
      map_entered = true;
    }
  if (crawler_checked_return_enabled(p) && bunny_check_text(code, i, "{") &&
      p->last_declaration.function[0] != '\0')
    crawler_checked_return_begin_function(p);
  if ((ret = read_compound_statement(p, code, i)) != 0)
    {
      if (map_entered)
	crawler_function_map_leave(p, map_previous_function);
      if (ret > 0) // Si on a implémenté une fonction pour de vrai
	{
	  if (!crawler_checked_return_end_function(p, code, *i))
	    {
	      delete_function_parse_checkpoint(checkpoint);
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	    }
	  if (report_started)
	    source_report_end_function(p, report_previous_function,
				     function_body_end_line(code, *i));

	  // On a limité le nombre de fonction par fichier - donc on compte les fonctions
	  if (p->function_per_file.active)
	    {
	      if ((p->ldec_function_per_file += 1) == p->function_per_file.value + 1)
		{ // On vient de dépasser le maximum
		  if (!add_warning(p, true, code, *i, &p->function_per_file.counter,
				   "Too many functions found in file. %d found. "
				   "%d was the maximum.",
				   p->ldec_function_per_file,
				   p->function_per_file.value))
		    RETURN ("Memory exhausted.");// LCOV_EXCL_LINE
		}
	    }
	  
	  // On verifie donc le nom si c'est une fonction non statique
	  if (!p->last_declaration.is_static)
	    {

	      // On compte les fonctions non static
	      if (p->non_static_function_per_file.active)
		{
		  if ((p->ldec_non_static_function_per_file += 1) == p->non_static_function_per_file.value + 1)
		    { // On vient de dépasser le maximum
		      if (!add_warning(p, true, code, *i,
				       &p->non_static_function_per_file.counter,
				       "Too many non static functions found in file. "
				       "%d was the maximum.",
				       p->non_static_function_per_file.value))
			RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
		    }
		}

	      // On ne vérifie pas le style du nom du main...
	      if (strcmp(&p->last_declaration.symbol[0], "main") &&
		  check_style(p, "function", &p->last_declaration.function[0], &p->function_style, &p->function_infix, code, j) == false)
		{
		  delete_function_parse_checkpoint(checkpoint);
		  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
		}
	      if (p->function_matching_path.active
		  && p->non_static_function_per_file.active
		  && p->non_static_function_per_file.value == 1)
		{
		  char target[512];

		  store_real_typename
		    (p, &target[0], &p->last_declaration.function[0], sizeof(target), 4);
		  if (compare_file_and_function_name(p, &target[0], code, j) == -1)
		    {
		      delete_function_parse_checkpoint(checkpoint);
		      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
		    }
		}
	    }
	  	  
	  if (p->only_by_reference.active)
	    for (int j = 0; j < p->last_declaration.nbr_copied_parameters; ++j)
	      {
		if (!add_warning
		    (p, IZ(p, i), code, *i, &p->only_by_reference.counter,
		     "Passing %s by copy is forbidden. "
		     "Only structures with size inferior or equal to %d bytes "
		     "can be passed by copy. %s is %d bytes long.",
		     p->last_declaration.copied_parameters[j].name,
		     p->only_by_reference.value,
		     p->last_declaration.copied_parameters[j].name,
		     p->last_declaration.copied_parameters[j].size
		     ))
		  {
		    delete_function_parse_checkpoint(checkpoint);
		    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
		  }
	      }
	  p->last_declaration.nbr_copied_parameters = 0;
	}
      p->last_declaration.inside_function = false;
      if (report_started && ret <= 0)
	source_report_cancel_function(p, report_previous_function,
				      report_previous_count);
      delete_function_parse_checkpoint(checkpoint);
      FRETURN (ret);
    }
  if (map_entered)
    crawler_function_map_leave(p, map_previous_function);
  if (report_started)
    source_report_cancel_function(p, report_previous_function, report_previous_count);
  p->checked_return_state.nbr_pending = 0;
  reset_last_declaration(p);
  
  // Les contrôles sémantiques liés à l'implémentation
  // (nombre de paramètres, passage par copie/référence) sont faits
  // uniquement quand un vrai corps de fonction a été lu. Les contrôles
  // de lisibilité des signatures restent, eux, actifs sur les prototypes.

  // On revient en arrière, ca n'était pas une declaration de fonction.
  // On restaure l'état spéculatif du parseur, mais pas les marqueurs globaux
  // du fichier préprocessé: ils suivent la lecture réelle du flux source.
  restore_function_parse_checkpoint(p, checkpoint, i);
  reset_last_declaration(p);
  delete_function_parse_checkpoint(checkpoint);
  FRETURN (0);
}

int			read_primary_expression(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  char			buffer[1024];
  int			val;
  double		val2;

  FTRACE(code, *i);
  if (read_identifier(p, code, i, false))
    FRETURN (1);
  if (bunny_read_cstring(code, i, &buffer[0], sizeof(buffer)))
    {
      read_whitespace(code, i);
      while (bunny_read_cstring(code, i, &buffer[0], sizeof(buffer)))
	read_whitespace(code, i);
      FRETURN (1);
    }
  if (bunny_read_cchar(code, i, &buffer[0]))
    FRETURN (1);
  if (bunny_read_double(code, i, &val2))
    {
      /// SUFFIXE
      while (bunny_read_text(code, i, "f") ||
	     bunny_read_text(code, i, "F") ||
	     bunny_read_text(code, i, "l") ||
	     bunny_read_text(code, i, "L"));

      if (p->no_magic_value.active && p->last_declaration.last_char < *i)
	if ((val2 > 1.0 || val2 < -1.0) &&
	    fabs(val2 - M_PI) > 0.01 &&
	    fabs(val2 - M_PI / 2) > 0.01
	    )
	  if (strncmp("test_", p->last_declaration.function, 5) != 0)
	    if (!add_warning
		(p, IZ(p, i), code, *i, &p->no_magic_value.counter,
		 "Magic values are forbidden. %f found.",
		 val2))
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.last_char = *i;
      FRETURN (1);
    }
  if (bunny_read_integer(code, i, &val))
    {
      /// SUFFIXE
      while (bunny_read_text(code, i, "u") ||
	     bunny_read_text(code, i, "U") ||
	     bunny_read_text(code, i, "l") ||
	     bunny_read_text(code, i, "L"));
      
      if (p->no_magic_value.active && p->last_declaration.last_char < *i)
	{
	  int		accepted[] = {
	    0, 1 << 0, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8,
	    1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15, 1 << 16,
	    1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21, 1 << 22, 1 << 23, 1 << 24,
	    1 << 25, 1 << 26, 1 << 27, 1 << 28, 1 << 29, 1 << 30, 1 << 31,
	    10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
	    SCHAR_MIN, SCHAR_MAX, UCHAR_MAX, SHRT_MIN, SHRT_MAX, USHRT_MAX, 65536,
	    INT_MIN, INT_MAX, UINT_MAX	
	  };
	  size_t	acci;
	  
	  val = abs(val);
	  for (acci = 0; acci < NBRCELL(accepted); ++acci)
	    if (accepted[acci] == val)
	      break ;
	  if (acci == NBRCELL(accepted))
	    if (strncmp("test_", p->last_declaration.function, 5) != 0)
	      if (!add_warning
		  (p, IZ(p, i), code, *i, &p->no_magic_value.counter,
		   "Magic values are forbidden. %d found.",
		   val))
		RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      p->last_declaration.last_char = *i;
      FRETURN (1);
    }
  ssize_t		j = *i;

  if (bunny_read_text(code, &j, "("))
    {
      int		ret;

      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, j - 1, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_expression(p, code, &j, false)) == -1)
	RETURN ("Problem encountered in expression after '('."); // LCOV_EXCL_LINE
      else if (ret == 0)
	FRETURN (0);
      if (!bunny_read_text(code, &j, ")"))
	RETURN ("Missing ')' after '(expression'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, j - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      *i = j;
      FRETURN (1);
    }
  FRETURN (0);
}

int			read_argument_expression_list(t_parsing	*p,
						      const char *code,
						      ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);
  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (p->function_map.enabled && p->function_map.current_call_receiver[0] != '\0')
	{
	  char		donated[SYMBOL_SIZE + 1];

	  if (crawler_map_read_pointer_donation(p, code, *i, donated))
	    crawler_function_map_add_pointer_donation
	      (p, p->function_map.current_call_receiver, donated);
	}
      if ((ret = read_assignment_expression(p, code, i)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    FRETURN (ret);
	  RETURN ("Excessive ',' found in argument list."); // LCOV_EXCL_LINE
	}
      else
	cnt += ret;
    }
  while (bunny_read_text(code, i, ","));
  FRETURN (cnt > 0 ? 1 : 0);
}

int			read_postfix_expression(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  int			ret;
  bool			once;
  char			call_target[SYMBOL_SIZE + 1];
  bool			has_call_target;

  FTRACE(code, *i);
  has_call_target = crawler_map_read_identifier_at(code, *i, call_target, NULL);
  if ((ret = read_primary_expression(p, code, i)) != 1)
    FRETURN (ret);
  do
    {
      once = false;
      if (bunny_read_text(code, i, "["))
	{
	  if (!check_parenthesis_space(p, code, *i - 1, '[', &p->no_space_inside_brackets.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  once = true;
	  has_call_target = false;
	  if (read_expression(p, code, i, false) != 1)
	    RETURN ("Problem encountered with expression after '['."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, "]"))
	    RETURN ("Missing ']' after '[expression'."); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i - 1, ']', &p->no_space_inside_brackets.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, "("))
	{
	  char		previous_receiver[SYMBOL_SIZE + 1];

	  once = true;
	  previous_receiver[0] = '\0';
	  if (has_call_target && p->last_declaration.inside_function)
	    source_report_add_call(p);
	  if (p->function_map.enabled)
	    {
	      crawler_map_copy_symbol(previous_receiver,
				      p->function_map.current_call_receiver,
				      sizeof(previous_receiver));
	      if (has_call_target && p->last_declaration.inside_function &&
		  p->function_map.current_function[0] != '\0')
		{
		  crawler_function_map_add_call
		    (p, p->function_map.current_function, call_target);
		  crawler_map_copy_symbol(p->function_map.current_call_receiver,
					  call_target,
					  sizeof(p->function_map.current_call_receiver));
		}
	      else
		p->function_map.current_call_receiver[0] = '\0';
	    }
	  if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	      (p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (read_argument_expression_list(p, code, i) == -1)
	    RETURN ("Problem encountered with argument list after '('."); // LCOV_EXCL_LINE
	  if (p->function_map.enabled)
	    crawler_map_copy_symbol(p->function_map.current_call_receiver,
				    previous_receiver,
				    sizeof(p->function_map.current_call_receiver));
	  has_call_target = false;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(argument'."); // LCOV_EXCL_LINE
	  if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	      (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, ".") || bunny_read_text(code, i, "->"))
	{
	  once = true;
	  has_call_target = false;
	  if (read_identifier(p, code, i, false) == false)
	    RETURN ("Problem encountered with symbol after '.' or '->'."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, "++"))
	once = true;
      if (bunny_read_text(code, i, "--"))
	once = true;
    }
  while (once);
  FRETURN (1);
}

int			read_specifier_qualifier_list(t_parsing	*p,
						      const char *code,
						      ssize_t	*i)
{
  int			cnt = 0;
  int			type_specifier = 0;
  int			type_qualifier = 0;
  bool			extension = false;

  FTRACE(code, *i);
  do
    {
      // GCC peut produire __extension__ dans les champs de structure,
      // par exemple dans struct cmsghdr: __extension__ unsigned char data[].
      extension = bunny_read_text(code, i, "__extension__");
      if ((type_specifier = read_type_specifier(p, code, i, type_specifier)) == -1)
	FRETURN (-1);
      cnt += type_specifier;
      if ((type_qualifier = read_type_qualifier(p, code, i)) == -1)
	FRETURN (-1);
      cnt += type_qualifier;
    }
  while (extension || type_specifier || type_qualifier);
  FRETURN (cnt >= 1 ? 1 : 0);
}

int			read_type_name(t_parsing		*p,
				       const char		*code,
				       ssize_t			*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_specifier_qualifier_list(p, code, i)) != 1)
    FRETURN (ret);
  if ((ret = read_abstract_declarator(p, code, i)) != 0)
    FRETURN (ret);
  FRETURN (1);
}

int			read_unary_operator(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  (void)p;
  FTRACE(code, *i);
  FRETURN (bunny_read_text(code, i, "&")
	  || bunny_read_text(code, i, "*")
	  || bunny_read_text(code, i, "+")
	  || bunny_read_text(code, i, "-")
	  || bunny_read_text(code, i, "~")
	  || bunny_read_text(code, i, "!")
	  ? 1 : 0);
}

int			read_unary_expression(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  FTRACE(code, *i);
  if (check_read_text(code, i, "sizeof"))
    {
      if (p->sizeof_parenthesis.active && bunny_check_text(code, i, "(") == false)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->sizeof_parenthesis.counter,
	     "There must be parenthesis around the sizeof expression."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_unary_expression(p, code, i) != 1)
	{
	  if (bunny_read_text(code, i, "("))
	    {
	      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
		  (p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
		RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	      if (read_type_name(p, code, i) != 1)
		RETURN ("Problem encountered with type name after 'sizeof('."); // LCOV_EXCL_LINE
	      if (!bunny_read_text(code, i, ")"))
		RETURN ("Missing ')' after 'sizeof(type name'."); // LCOV_EXCL_LINE
	      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
		  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
		RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	      FRETURN (1);
	    }
	  RETURN ("Unknown sequence after 'sizeof'."); // LCOV_EXCL_LINE
	}
      FRETURN (1);
    }
  if (bunny_read_text(code, i, "++"))
    {
      if (p->inline_mod_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->inline_mod_forbidden.counter,
	     "'++' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      FRETURN (read_unary_expression(p, code, i));
    }
  if (bunny_read_text(code, i, "--"))
    {
      if (p->inline_mod_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->inline_mod_forbidden.counter,
	     "'--' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      FRETURN (read_unary_expression(p, code, i));
    }
  if (read_unary_operator(p, code, i) == 1)
    FRETURN (read_cast_expression(p, code, i));
  FRETURN (read_postfix_expression(p, code, i));
}

int			read_cast_expression(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  ssize_t		j = *i;

  FTRACE(code, *i);
  if (bunny_read_text(code, &j, "("))
    {
      p->last_declaration.inside_cast = true;
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, j - 1, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_type_name(p, code, &j) == 1)
	{
	  p->last_declaration.inside_cast = false;
	  *i = j;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(typename'."); // LCOV_EXCL_LINE
	  if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	      (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  FRETURN (read_cast_expression(p, code, i));
	}
      p->last_declaration.inside_cast = false;
    }
  FRETURN (read_unary_expression(p, code, i));
}

int			read_multiplicative_expression(t_parsing *p,
						       const char *code,
						       ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_cast_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "*")
      || bunny_read_text(code, i, "/")
      || bunny_read_text(code, i, "%"))
    {
      if (check_one_space_around
	  (p, code, *i - 1, 1,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_multiplicative_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '*', '/' or '%'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_additive_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_multiplicative_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "+")
      || bunny_read_text(code, i, "-"))
    {
      if (check_one_space_around
	  (p, code, *i - 1, 1,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_additive_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '+' or '-'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_shift_expression(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_additive_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "<<")
      || bunny_read_text(code, i, ">>"))
    {
      if (check_one_space_around
	  (p, code, *i - 2, 2,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_shift_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '<<' or '>>'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_relational_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_shift_expression(p, code, i)) != 1)
    FRETURN (ret);
  read_whitespace(code, i);
  int			j = *i;

  if (bunny_read_text(code, i, "<=")
      || bunny_read_text(code, i, ">=")
      || (!bunny_check_text(code, i, ">>") && bunny_read_text(code, i, ">"))
      || (!bunny_check_text(code, i, "<<") && bunny_read_text(code, i, "<"))
      )
    {
      if (check_one_space_around
	  (p, code, *i - (*i - j), *i - j,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_relational_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '<=', '>=', '<' or '>'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_equality_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_relational_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "==")
      || bunny_read_text(code, i, "!="))
    {
      if (check_one_space_around
	  (p, code, *i - 2, 2,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_equality_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '==' or '!='."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_and_expression(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_equality_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (!bunny_check_text(code, i, "&&")
      && bunny_read_text(code, i, "&"))
    {
      if (check_one_space_around
	  (p, code, *i - 1, 1,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_and_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '&'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_exclusive_or_expression(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_and_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "^"))
    {
      if (check_one_space_around
	  (p, code, *i - 1, 1,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_exclusive_or_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '^'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_inclusive_or_expression(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_exclusive_or_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (!bunny_check_text(code, i, "||")
      && bunny_read_text(code, i, "|"))
    {
      if (check_one_space_around
	  (p, code, *i - 1, 1,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_inclusive_or_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '|'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_logical_and_expression(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_inclusive_or_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "&&"))
    {
      if (check_one_space_around
	  (p, code, *i - 2, 2,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_logical_and_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '&&'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_logical_or_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_logical_and_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "||"))
    {
      if (check_one_space_around
	  (p, code, *i - 2, 2,
	   p->space_around_binary_operator.value,
	   &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_logical_or_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after '||'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_assignment_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  read_whitespace(code, i);
  ssize_t		j = *i;
  int			sizof = p->sizeof_parenthesis.counter;

  p->function_map.suppressed += 1;
  ret = read_unary_expression(p, code, &j);
  p->function_map.suppressed -= 1;
  if (ret == 1)
    {
      int		k = j;

      if ((!bunny_check_text(code, &j, "==")
	   && bunny_read_text(code, &j, "="))
	  || bunny_read_text(code, &j, "+=")
	  || bunny_read_text(code, &j, "-=")
	  || bunny_read_text(code, &j, "*=")
	  || bunny_read_text(code, &j, "/=")
	  || bunny_read_text(code, &j, "%=")
	  || bunny_read_text(code, &j, "<<=")
	  || bunny_read_text(code, &j, ">>=")
	  || bunny_read_text(code, &j, "|=")
	  || bunny_read_text(code, &j, "&=")
	  )
	{
       	  if (p->no_assignment.active)
	    if (!add_warning(p, IZ(p, i), code, *i, &p->no_assignment.counter, "Assignment are forbidden."))
	      RETURN ("Memory exhausted.");
	  if (check_one_space_around(p, code, k, j - k,
				     p->space_around_binary_operator.value,
				     &p->space_around_binary_operator.counter) == -1)
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  *i = j;
	  if (read_assignment_expression(p, code, i) != 1)
	    RETURN ("Problem encountered with expression after assignment."); // LCOV_EXCL_LINE
	  FRETURN (1);
	}
      p->sizeof_parenthesis.counter = sizof;
    }
  FRETURN (read_conditional_expression(p, code, i));
}

int			read_expression(t_parsing		*p,
					const char		*code,
					ssize_t			*i,
					bool			start)
{
  int		ret;

  FTRACE(code, *i);
  if ((ret = read_assignment_expression(p, code, i)) == -1)
    FRETURN (-1);
  if (start && ret == 1 && check_base_indentation(p, code, *i) == -1)
    RETURN("Memory exhausted.");
  if (bunny_read_text(code, i, ","))
    {
      if (check_no_space_before_space_after(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (p->single_instruction_per_line.value)
	if (!add_warning(p, IZ(p, i), code, *i, &p->single_instruction_per_line.counter,
			 "Only a single instruction is authorized per line."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_expression(p, code, i, false)) == 0)
	RETURN ("Excessive ',' found in expression."); // LCOV_EXCL_LINE
    }
  FRETURN (ret);
}

int			read_conditional_expression(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_logical_or_expression(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, "?"))
    {
      if (p->ternary_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->ternary_forbidden.counter,
	     "'?' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_one_space_around(p, code, *i - 1, 1,
				 p->space_around_binary_operator.value,
				 &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i, false) != 1)
	RETURN ("Problem encountered with expression after 'condition ?'."); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, ":") == false)
	RETURN ("Missing ':' after 'condition ? case1'."); // LCOV_EXCL_LINE
      if (check_one_space_around(p, code, *i - 1, 1,
				 p->space_around_binary_operator.value,
				 &p->space_around_binary_operator.counter) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_constant_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after 'condition ? expression :'."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_constant_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  (void)p;
  FTRACE(code, *i);
  FRETURN (read_conditional_expression(p, code, i));
}

int			read_type_qualifier(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  (void)p;
  FTRACE(code, *i);
  if (check_read_text(code, i, "const"))
    {
      p->last_declaration.is_const = true;
      FRETURN (1);
    }
  if (check_read_text(code, i, "volatile"))
    {
      p->last_declaration.is_volatile = true;
      FRETURN (1);
    }
  if (check_read_text(code, i, "restrict"))
    {
      p->last_declaration.is_restrict = true;
      FRETURN (1);
    }
  if (check_read_text(code, i, "__restrict"))
    {
      p->last_declaration.is_restrict = true;
      FRETURN (1);
    }
  FRETURN (0);
}

int			read_struct_declarator(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  int			ret = *i;

  FTRACE(code, *i);
  p->last_declaration.symbol[0] = '\0';
  // On va lire maintenant le nom de l'attribut d'union ou de structure, et savoir si c'est
  // un pointeur ou pas
  if ((ret = read_declarator(p, code, i)) == -1)
    RETURN ("Problem encountered with symbol after 'type' in struct."); // LCOV_EXCL_LINE
  //// A VOIR!!!!
  if (p->last_declaration.inside_struct)
    ret = check_style
      (p, "struct attribute", &p->last_declaration.symbol[0],
       &p->struct_attribute_style, &p->struct_attribute_infix,
       code, ret);
  else if (p->last_declaration.inside_union)
    ret = check_style
      (p, "union attribute", &p->last_declaration.symbol[0],
       &p->union_attribute_style, &p->union_attribute_infix,
       code, ret);
  if (ret == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE

  if (bunny_read_text(code, i, ":"))
    {
      if ((ret = read_constant_expression(p, code, i)) == 0)
	RETURN ("Missing bitfield size after 'type symbol:' in struct."); // LCOV_EXCL_LINE
      FRETURN (ret);
    }
  FRETURN (ret);
}

int			read_struct_declarator_list(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);
  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_struct_declarator(p, code, i)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    FRETURN (ret);
	  RETURN ("Excessive ',' in declaration."); // LCOV_EXCL_LINE
	}
      else
	{
	  if (p->last_declaration.nbr_pointer)
	    {
	      if (p->last_declaration.inside_struct)
		p->last_declaration.cumulated_attribute_size += sizeof(void*);
	      else if (p->last_declaration.cumulated_attribute_size < (int)sizeof(void*))
		p->last_declaration.cumulated_attribute_size = sizeof(void*);
	    }
	  else
	    {
	      if (p->last_declaration.inside_struct)
		p->last_declaration.cumulated_attribute_size +=
		  p->last_declaration.last_type_size;
	      else if (p->last_declaration.cumulated_attribute_size <
		       p->last_declaration.last_type_size)
		p->last_declaration.cumulated_attribute_size =
		  p->last_declaration.last_type_size;
	    }
	  cnt += ret;
	}
    }
  while (bunny_read_text(code, i, ","));
  FRETURN (cnt > 0 ? 1 : 0);
}

int			read_struct_declaration(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  int			a;
  int			b;

  /////////////////////////////////////////
  // On est DANS la structure ou l'union //
  /////////////////////////////////////////

  FTRACE(code, *i);
  // Lit le type et tout le tralala d'une declaration d'attribut
  if ((a = read_specifier_qualifier_list(p, code, i)) != 1)
    FRETURN (a);
  // Lit les multiples declarations, potentiellement séparés par des virgules
  // d'attributs exploitant le type précédent.
  if ((b = read_struct_declarator_list(p, code, i)) == -1)
    RETURN ("Problem encountered with attribute name after type definition in struct."); // LCOV_EXCL_LINE
  read_gcc_attribute(p, code, i);
  if ((a = (a || b)) && bunny_read_text(code, i, ";") == false)
    RETURN ("Missing ';' after attribute definition in struct."); // LCOV_EXCL_LINE
  if (a && check_white_then_newline(p, code, *i, false) == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  FRETURN (a ? 1 : 0);
}

int			read_struct_declaration_list(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);
  do
    {
      if ((ret = read_struct_declaration(p, code, i)) == -1)
	FRETURN (-1);
      else
	cnt += ret;
    }
  while (ret == 1);
  FRETURN (cnt);
}

bool			read_keyword(t_parsing			*p,
				     const char			*code,
				     ssize_t			*i,
				     const char			*symbol,
				     const char			*symchars)
{
  FTRACE(code, *i);
  read_whitespace(code, i);
  gl_bunny_read_whitespace = NULL;
  ssize_t		j = *i;

  // On trouve bien le symbole.
  if (bunny_read_text(code, &j, symbol) == false)
    {
      gl_bunny_read_whitespace = read_whitespace;
      FRETURN (false);
    }
  // Mais d'autres caractères suivent, donc le symbole lu est incomplet... ce n'est pas le symbole
  if (bunny_read_char(code, &j, symchars))
    {
      gl_bunny_read_whitespace = read_whitespace;
      FRETURN (false);
    }
  // C'est bien le bon symbole.
  // On l'enregistre dans last_type
  strncpy(p->last_declaration.last_type, &code[*i],
	  j - *i > (int)sizeof(p->last_declaration.last_type) ?
	  (int)sizeof(p->last_declaration.last_type) :
	  j - *i);
  // On avance le curseur.
  *i = j;
  gl_bunny_read_whitespace = read_whitespace;
  FRETURN (true);
}


int			check_type_is_authorized(t_parsing	*p,
						 const char	*code,
						 ssize_t	i,
						 const char	*symbol)
{
  t_bunny_configuration	*auth;
  t_bunny_configuration	*forb;

  FTRACE(code, i);
  // On vérifie qu'on ne soit pas dans un parametre (void).
  if (p->last_declaration.inside_parameter && strcmp(symbol, "void") == 0)
    {
      ssize_t j = i;

      read_whitespace(code, &j);
      if (code[j] == ')')
	FRETURN (0);
    }
  
  // Si il y a un filet "types autorisés", c'est qu'ils sont tous
  // interdit, sauf ceux qui sont autorisés
  if (bunny_configuration_getf(p->configuration, &auth, "TypeRestriction.AuthorizedTypes"))
    {
      if (!bunny_configuration_getf(auth, NULL, "%s", symbol))
	{
	  if (!add_warning
	      (p, true, code, i, &p->forbidden_type.counter,
	       "Type %s is not authorized.",
	       symbol))
	    FRETURN (-1);
	}
    }
  // Si il y a un filet "types interdits", c'est qu'ils sont tous autorisé,
  // sauf ceux qui sont interdits
  if (bunny_configuration_getf(p->configuration, &forb, "TypeRestriction.ForbiddenTypes"))
    {
      if (bunny_configuration_getf(forb, NULL, "%s", symbol))
	{
	  if (!add_warning
	      (p, true, code, i, &p->forbidden_type.counter,
	       "Type %s is forbidden.",
	       symbol))
	    FRETURN (-1);
	  
	}
    }
  FRETURN (0);
}

// Lit le type
int			read_type_specifier(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i,
					    bool		second_check)
{
  FTRACE(code, *i);

  if (check_read_text(code, i, "enum"))
    {
      int		j = *i;

      // Optionel
      if (read_identifier(p, code, i, false))
	{
	  p->last_declaration.was_named = true;
	  if (add_new_type(p, p->last_declaration.symbol, -1) == false)
	    RETURN ("No more space for new types."); // LCOV_EXCL_LINE
	}
      else
	p->last_declaration.was_named = false;
      // On enregistre le symbole pour pouvoir le comparer avec le typedef plus tard
      if (p->last_declaration.is_typedef && p->typedef_matching.active
	  && p->last_declaration.scope_depth == 0)
	{
	  if (p->typedef_stack_top >= (int)NBRCELL(p->typedef_stack))
	    RETURN("Typedef matching stack is full."); // LCOV_EXCL_LINE
	  store_real_typename
	    (p, &p->typedef_stack[p->typedef_stack_top++][0],
	     &p->last_declaration.symbol[0],
	     sizeof(p->typedef_stack[0]), 3);
	}
      if (check_style
	  (p, "enum", &p->last_declaration.symbol[0],
	   &p->enum_style, &p->enum_infix, code, j) == false)
	RETURN("Memory exhausted"); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, "{"))
	{
	  int		cnt = 0;

	  p->last_declaration.scope_depth += 1;
	  do
	    {
	      if (cnt > 0)
		if (check_no_space_before_space_after(p, code, *i) == -1)
		  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	      j = *i;
	      read_identifier(p, code, i, false);
	      if (check_style(p, "enum constant", &p->last_declaration.symbol[0],
			      &p->enum_constant_style, &p->enum_constant_infix, code, j) == false)
		RETURN("Memory exhausted"); // LCOV_EXCL_LINE
	      if (bunny_read_text(code, i, "="))
		if (read_constant_expression(p, code, i) != 1)
		  RETURN("Missing value after 'enum enum_symbol { symbol ='."); // LCOV_EXCL_LINE
	      cnt += 1;
	    }
	  while (bunny_read_text(code, i, ","));
	  if (!bunny_read_text(code, i, "}"))
	    RETURN ("Missing '}' after 'enum symbol { constants'."); // LCOV_EXCL_LINE
	  p->last_declaration.scope_depth -= 1;
	  p->last_declaration.last_type_size = sizeof(enum { __ABCDEFGH__ });
	}
      p->last_declaration.is_enum_last_typedef = true;
      FRETURN (1);
    }
  bool punion = p->last_declaration.inside_union;
  bool pstruct = p->last_declaration.inside_struct;

  if ((check_read_text(code, i, "union") && (p->last_declaration.inside_union = true))
      || (check_read_text(code, i, "struct") && (p->last_declaration.inside_struct = true)))
    {
      read_whitespace(code, i);
      bool		ret = true;
      int		j = *i;

      p->local_symbol_alignment = count_to_new_line(p, code, j);
      if (read_identifier(p, code, i, false)) // optionnel
	{
	  p->last_declaration.was_named = true;
	  if (add_new_type(p, p->last_declaration.symbol, -1) == false)
	    RETURN ("No more space for new types."); // LCOV_EXCL_LINE
	}
      else
	p->last_declaration.was_named = false;

      // On enregistre le symbole pour pouvoir le comparer avec le typedef plus tard
      if (p->last_declaration.is_typedef && p->typedef_matching.active
	  && p->last_declaration.scope_depth == 0)
	{
	  if (p->typedef_stack_top >= (int)NBRCELL(p->typedef_stack))
	    RETURN("Typedef matching stack is full."); // LCOV_EXCL_LINE
	  store_real_typename
	    (p, &p->typedef_stack[p->typedef_stack_top++][0],
	     &p->last_declaration.symbol[0],
	     sizeof(p->typedef_stack[0]), p->last_declaration.inside_union ? 1 : 0);
	}

      if (p->last_declaration.inside_union)
	{
	  p->last_declaration.inside_struct = false;
	  ret = check_style
	    (p, "union", &p->last_declaration.symbol[0],
	     &p->union_style, &p->union_infix,
	     code, j);
	}
      else if (p->last_declaration.inside_struct)
	{
	  p->last_declaration.inside_union = false;
	  ret = check_style
	    (p, "struct", &p->last_declaration.symbol[0],
	     &p->struct_style, &p->struct_infix,
	     code, j);
	}
      if (ret == false)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE

      if (bunny_read_text(code, i, "{")) // optionnel
	{
	  p->last_declaration.scope_depth += 1;
	  p->last_declaration.cumulated_attribute_size = 0;
	  p->last_declaration.was_defining = true;

	  if (read_struct_declaration_list(p, code, i) == -1)
	    FRETURN (-1);
	  if (!bunny_read_text(code, i, "}"))
	    RETURN ("Missing '}' after 'struct/union symbol { attributes'."); // LCOV_EXCL_LINE

	  p->last_declaration.scope_depth -= 1;
	  p->last_declaration.last_type_size = p->last_declaration.cumulated_attribute_size;
	}
      else
	{
	  for (size_t j = 0; j < p->last_new_type; ++j)
	    if (strcmp(p->last_declaration.last_type, p->new_type[j].name) == 0)
	      p->last_declaration.last_type_size = p->new_type[j].size;
	}
      p->last_declaration.is_union_last_typedef = p->last_declaration.inside_union;
      p->last_declaration.is_struct_last_typedef = p->last_declaration.inside_struct;
      p->last_declaration.inside_union = punion;
      p->last_declaration.inside_struct = pstruct;
      FRETURN (1);
    }

  // Standard types
  for (size_t j = 0; j < NBRCELL(standard_types); ++j)
    if (read_keyword(p, code, i, standard_types[j].name, gl_second_char))
      {
	check_type_is_authorized(p, code, *i, standard_types[j].name);
	p->last_declaration.last_type_size = standard_types[j].siz;
	FRETURN (1);
      }
  // Custom types
  for (size_t j = 0; j < p->last_new_type; ++j)
    if (read_keyword(p, code, i, &p->new_type[j].name[0], gl_second_char))
      {
	check_type_is_authorized(p, code, *i, &p->new_type[j].name[0]);
	p->last_declaration.last_type_size = p->new_type[j].size;
	FRETURN (1);
      }

  size_t	x;
  ssize_t      j = *i;
  // On cherche si c'est un mot clef...
  for (x = 0; x < NBRCELL(keywords) && bunny_read_text(code, &j, keywords[x]) == false; ++x);
  if (x != NBRCELL(keywords) && (code[j] == '\0' || strchr(gl_second_char, code[j]) == NULL)) // Au cas ou ce soit... "ifa" par exemple.
    FRETURN (0);
  
  if (p->last_declaration.is_typedef || second_check || p->last_declaration.inside_cast)
    FRETURN(0);

  char		unknown_type[256];

  j = *i;
  if (bunny_read_field(code, &j))
    {
      size_t		unknown_len;

      unknown_len = j - *i;
      if (unknown_len >= sizeof(unknown_type))
	unknown_len = sizeof(unknown_type) - 1;
      memcpy(unknown_type, &code[*i], unknown_len);
      unknown_type[unknown_len] = '\0';
      if (!add_warning
	  (p, IZ(p, i), code, *i, NULL,
	   "Unknown type : > %s <", unknown_type))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      FRETURN (-1);
    }
  // 0 pour contrer les erreurs de type qui n'en sont pas venant de test si c'est une fonction
  FRETURN(0); 
}

int			read_storage_class_specifier(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  const char		*str[] =
    {
     "typedef", "extern", "static", "auto", "register",
    };

  FTRACE(code, *i);
  // ...
  bunny_read_text(code, i, "__extension__");

  ssize_t		j = *i;

  for (size_t n = 0; n < NBRCELL(str); ++n)
    {
      if (check_read_text(code, &j, str[n]))
	{
	  (&p->last_declaration.is_typedef)[n] = true;
	  *i = j;
	  FRETURN (1);
	}
    }
  FRETURN (0);
}


// Avance à travers code, jusqu'à arriver à une ( de même niveau que target_counter
// Evitez de mettrer target_counter à des valeurs négatives
int			travel_expression(t_parsing		*p,
					  const char		*code,
					  ssize_t		*i,
					  int			target_counter)
{
  FTRACE(code, *i);

  while (code[*i] && (target_counter != p->func_ptr_counter || code[*i] != ')'))
    {
      if (code[*i] == '(')
	p->func_ptr_counter += 1;
      else if (code[*i] == ')')
	p->func_ptr_counter -= 1;
      *i += 1;
    }
  if (code[*i] != '\0' && code[*i] == ')')
    *i += 1; // On passe la ) car la comparaison s'est arrêté dessus

  if (bunny_read_text(code, i, "["))
      FRETURN(2);
  
  // Doit être le début des paramètres de la func ptr
  if (!bunny_read_text(code, i, "("))
    FRETURN(-1);
  FRETURN(0);
}

static bool		check_is_func_ptr(const char		*code,
					  ssize_t		*i)
{
  ssize_t		j = *i;

  if (bunny_read_text(code, &j, "("))
    return (false); // Ce n'est pas un pointeur sur fonction
  for (; code[j] && code[j] != '(' && code[j] != ')'; ++j);
  if (code[j] == ')')
    return (true); // exemple : void (*f) (int tre)
  return (false);  // exemple : void (*signal(int sig)) (int song)
}

int			check_type_is_function(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  FTRACE(code, *i);
  ssize_t		j = *i;
  ssize_t		save_skip;

  // Check si on est dans un format : previous_type (*
  // On utilise j, car si on a la ( mais pas * on revient en arrière
  if (!bunny_read_text(code, &j, "(") || !bunny_read_text(code, &j, "*"))
    FRETURN(0);

  // On élimine le cas: typedef void (*signalf) (int)
  // On reste avec j car ce n'est qu'un test on ne veut pas avancer
  if ((p->last_declaration.is_func_ptr = check_is_func_ptr(code, &j)))
    FRETURN(0);

  save_skip = j; // on se prépare à manger (*
  
  p->func_ptr_counter += 1;
  int			ret;

  if ((ret = travel_expression(p, code, &j, p->func_ptr_counter)) == -1)
    {
      if (!add_warning(p, IZ(p, i), code, *i, NULL, "Syntax Error, missing > ( <"))
	RETURN ("Memory exhausted.");
      FRETURN(-1); // Erreur de syntaxe
    }

  // Si le type n'est pas un pointeur sur fonction mais un tableau
  // exemple : int (*bunny_get_key_button(void))[45 + 2];
  if (ret == 2)
    {
      p->func_ptr_counter -= 1;
      FRETURN(0);
    }
  else
    *i = save_skip; // C'est bien un pointeur fonction, on avance
  
  // On lit le paramètre du pointer sur fonction retourné
  if ((ret = read_parameter_list(p, code, &j)) == -1)
      FRETURN(-1);

  // Sauf erreur, on met à jour la taille du type -> pointer
  if (ret == 1)
    p->last_declaration.last_type_size = sizeof(void *);

  FRETURN(ret);
}

static int		read_function_specifier(t_parsing	*p,
					const char	*code,
					ssize_t		*i)
{
  (void)p;
  FTRACE(code, *i);
  if (check_read_text(code, i, "inline")
      || check_read_text(code, i, "__inline")
      || check_read_text(code, i, "__inline__")
      || check_read_text(code, i, "_Noreturn"))
    FRETURN (1);
  FRETURN (0);
}

int			read_declaration_specifiers(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i,
						    bool	in_read_function_definition)
{
  size_t		last_new_type = p->last_new_type;
  bool			just_typedefed = false;
  bool			init_typedef = p->last_declaration.is_typedef;
  int			ret;
  int			cnt = 0;
  bool			once;
  int			prev_ptr;
  bool			typed = false;

  FTRACE(code, *i);
  p->last_declaration.nbr_pointer = prev_ptr = 0;
  do
    {
      // Reste faux si rien trouvé
      once = false;
      
      // On peut préciser un type de stockage - ou typedef
      if ((ret = read_storage_class_specifier(p, code, i)) == -1)
	FRETURN (-1);
      once = (once || (ret == 1));
      if ((ret = read_function_specifier(p, code, i)) == -1)
	FRETURN (-1);
      once = (once || (ret == 1));
      // On veut un type
      // pas sûr pour le ! devant in_read_function_definition mais ça marche
      if ((ret = read_type_specifier(p, code, i, (typed || !in_read_function_definition))) == -1)
	FRETURN (-1);
      if (ret == 1)
	typed = true;
      once = (once || (ret == 1));
      // On veut un const ou autre du style
      if ((ret = read_type_qualifier(p, code, i)) == -1)
	FRETURN (-1);
      once = (once || (ret == 1));

      // Un pointeur?
      if ((ret = read_pointer(p, code, i)) == -1)
	FRETURN (-1);

      // Si nouveau pointeur, on check la position de l'étoile
      if (p->last_declaration.nbr_pointer != prev_ptr)
	if (!check_pointer_star_position(p, code, *i))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE

      // Mis à jour des variables
      prev_ptr = p->last_declaration.nbr_pointer;
      once = (once || (ret == 1));

      // Ne sert à rien car commentaire sur le FRETURN concerné
      cnt += once ? 1 : 0;

      if ((ret = check_type_is_function(p, code, i)) == -1)
	FRETURN (-1);

      once = (once || (ret == 1));
      
      if (p->last_declaration.is_typedef && init_typedef == false)
	just_typedefed = true;
    }
  while (once);
  // On est peut etre face a un nouveau type...
  if (p->last_new_type != last_new_type || (just_typedefed && typed))
    if ((ret = handle_typedef(p, code, i, true)) == -1)
      return (-1);
  FRETURN (typed ? 1 : 0);
  // FRETURN (cnt >= 1 ? 1 : 0);
}

static int		read_array_declarator_content(t_parsing	*p,
					      const char	*code,
					      ssize_t		*i);

int			read_direct_abstract_declarator(t_parsing *p,
							const char *code,
							ssize_t	*i)
{
  bool			once;
  int			cnt = 0;

  FTRACE(code, *i);
  do
    {
      once = false;
      if (bunny_read_text(code, i, "("))
	{
	  if (!check_parenthesis_space
	      (p, code, *i - 1, '(',
	       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  int		ret;

	  p->last_declaration.inside_parameter = true;
	  if ((ret = read_abstract_declarator(p, code, i)) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  else if (ret == 0 && read_parameter_type_list(p, code, i) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  once = (ret == 1);
	  p->last_declaration.inside_parameter = false;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(' parameter list"); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i - 1, ')',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, "["))
	{
	  if (!check_parenthesis_space(p, code, *i - 1, '[', &p->no_space_inside_brackets.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (read_array_declarator_content(p, code, i) == -1)
	    FRETURN (-1);
	  if (!bunny_read_text(code, i, "]"))
	    RETURN ("Missing ']' after '[constant'"); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i - 1, ']', &p->no_space_inside_brackets.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  once = true;
	}
      cnt += once ? 1 : 0;
    }
  while (once);
  FRETURN (cnt >= 1 ? 1 : 0);
}

int			read_abstract_declarator(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			a;
  int			b;

  FTRACE(code, *i);
  if ((a = read_pointer(p, code, i)) == -1)
    FRETURN (-1);
  if (p->last_declaration.nbr_pointer)
    if (!check_pointer_star_position(p, code, *i))
      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  if ((b = read_direct_abstract_declarator(p, code, i)) == -1)
    FRETURN (-1);
  FRETURN (a + b >= 1 ? 1 : 0);
}

int			read_parameter_declaration(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_declaration_specifiers(p, code, i, false)) != 1)
    FRETURN (ret);
  if ((ret = read_declarator(p, code, i)) == -1)
    FRETURN (ret);
  if (ret == 1)
    FRETURN (1);
  if ((ret = read_abstract_declarator(p, code, i)) == -1)
    FRETURN (-1);
  FRETURN (1);
}

int			read_parameter_list(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  int			ret;
  int			cnt;
  int			start = *i;
  bool			err = false;
  int			alignment = 0;

  FTRACE(code, *i);
  cnt = 0;
  p->local_parameter_type_alignment = -1;
  p->local_parameter_name_alignment = -1;
  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      read_whitespace(code, i);

      // On regarde si les types sont alignés
      if (p->parameter_type_alignment.value)
	{
	  alignment = count_to_new_line(p, code, *i);
	  if (p->local_parameter_type_alignment == -1)
	    p->local_parameter_type_alignment = alignment;
	  if (p->local_parameter_type_alignment != alignment)
	    if (!add_warning
		(p, IZ(p, i), code, *i, &p->parameter_type_alignment.counter,
		 "Parameter types must be aligned."))
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}

      if (bunny_read_text(code, i, "..."))
	FRETURN (1);
      p->last_declaration.ptr_acc = 0;
      if ((ret = read_parameter_declaration(p, code, i)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    FRETURN (ret);
	  RETURN ("Excessive ',' found in parameter list."); // LCOV_EXCL_LINE
	}
      if (p->last_declaration.inside_function_definition_attempt)
	if (!check_last_parameter_is_reference(p, code, *i))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      cnt += 1;
      if (p->last_declaration.inside_function_definition_attempt &&
	  p->max_parameter.active && p->max_parameter.value < cnt &&
	  err == false)
	{
	  err = true;
	  if (!add_warning
	      (p, IZ(p, &start), code, start, &p->max_parameter.counter,
	       "Invalid amount of parameters. Maximum was %d.", p->max_parameter.value))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
    }
  while (bunny_read_text(code, i, ","));

  FRETURN (1);
}

int			read_parameter_type_list(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  FTRACE(code, *i);
  if ((ret = read_parameter_list(p, code, i)) != 1)
    FRETURN (ret);
  if (bunny_read_text(code, i, ","))
    {
      if (check_no_space_before_space_after(p, code, *i - 1) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      FRETURN (bunny_read_text(code, i, "...") ? 1 : -1);
    }
  FRETURN (1);
}

int			read_pointer(t_parsing			*p,
				     const char			*code,
				     ssize_t			*i)
{
  int			ret;

  FTRACE(code, *i);
  if (!bunny_read_text(code, i, "*"))
    FRETURN (0);
  p->last_declaration.nbr_pointer += 1;
  p->last_declaration.ptr_acc += 1;
  p->last_declaration.last_type_size = sizeof(void*);
  if (read_type_qualifier(p, code, i) == -1)
    FRETURN (-1);
  if ((ret = read_pointer(p, code, i)) == -1)
    FRETURN (-1);
  FRETURN (1);
}

static int		read_array_declarator_content(t_parsing	*p,
					      const char	*code,
					      ssize_t		*i)
{
  bool			need_expression;
  int			ret;

  FTRACE(code, *i);
  need_expression = false;
  do
    {
      if ((ret = read_type_qualifier(p, code, i)) == -1)
	FRETURN (-1);
    }
  while (ret == 1);
  if (check_read_text(code, i, "static"))
    {
      need_expression = true;
      do
	{
	  if ((ret = read_type_qualifier(p, code, i)) == -1)
	    FRETURN (-1);
	}
      while (ret == 1);
    }
  if (bunny_read_text(code, i, "*"))
    FRETURN (1);
  if (bunny_check_text(code, i, "]"))
    {
      if (need_expression)
	RETURN ("Missing expression after '[static'."); // LCOV_EXCL_LINE
      FRETURN (1);
    }
  if (p->ansi_c)
    {
      if ((ret = read_constant_expression(p, code, i)) == -1)
	RETURN ("Problem encountered with constant expression after '['."); // LCOV_EXCL_LINE
    }
  else
    {
      if ((ret = read_expression(p, code, i, false)) == -1)
	RETURN ("Problem encountered with expression after '['."); // LCOV_EXCL_LINE
    }
  if (ret == 0 && need_expression)
    RETURN ("Missing expression after '[static'."); // LCOV_EXCL_LINE
  FRETURN (ret);
}

/*
** direct_declarator
**	: IDENTIFIER
**	| '(' declarator ')'
**	| direct_declarator '[' constant_expression ']'
**	| direct_declarator '[' ']'
**	| direct_declarator '(' parameter_type_list ')'
**	| direct_declarator '(' identifier_list ')'
**	| direct_declarator '(' ')'
**	;
** Signifie
** Indentifier ou '(' declarator ')'
** suivi d'un nombre superieur ou égal à zéro de
** '[' contsant ']'
** '[' ']'
** '(' parameter type list ')'
** '(' identifier list ')'
** '(' ')'
*/
int			read_direct_declarator(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  int			j = *i;
  int			tmp;
  bool			once;

  FTRACE(code, *i);
  if (bunny_read_text(code, i, "(")) // '(' declarator ')'
    {
      if (!check_parenthesis_space(p, code, *i - 1, '(',
				   &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE

      if (read_declarator(p, code, i) == -1)
	FRETURN (-1);

      //      if (!bunny_read_text(code, i, ")"))
      //RETURN ("Missing ')' after '(declaration'."); // LCOV_EXCL_LINE
      // On test sans condition bloquante
      if (bunny_read_text(code, i, ")"))
	{
	  if (!check_parenthesis_space(p, code, *i - 1, ')',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
    }
  // IDENTIFIER
  else if ((tmp = read_identifier(p, code, i, false)) != 1)
    FRETURN (tmp);
  bool			parameters = false;
  bool			brackets = false;

  // Pour accéder aux paramètres dans un cas : typedef void (*signalf)(int song) ou void (*f)(int tre)
  if (p->last_declaration.is_func_ptr && p->last_declaration.is_typedef)
    {
      bunny_read_text(code, i, ")");
      p->last_declaration.is_func_ptr = false; // Risque de bloquer d'autres check de condition
    }
  (void)brackets;
  // A partir de la, c'est un nombre indeterminé de match
  do
    {
      once  = false;
      // Des paramètres ?
      if (bunny_read_text(code, i, "("))
	{
	  parameters = true;
	  if (!check_parenthesis_space(p, code, *i - 1, '(', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (bunny_read_text(code, i, ")"))
	    {
	      if (!check_parenthesis_space(p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
		RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	      break ;
	    }
	  strcpy(p->last_declaration.function, p->last_declaration.symbol);
	  p->last_declaration.inside_parameter = true;
	  if ((tmp = read_parameter_type_list(p, code, i)) == -1)
	    FRETURN (tmp);
	  else if (tmp == 0 && read_identifier_list(p, code, i) != 1)
	    FRETURN (-1);
	  p->last_declaration.inside_parameter = false;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '( parameter list"); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  once = true;
	}
      // Un type tableau comme valeur de retour?
      if (bunny_read_text(code, i, "["))
	{
	  brackets = true;
	  if (!check_parenthesis_space(p, code, *i - 1, '[', &p->no_space_inside_brackets.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (read_array_declarator_content(p, code, i) == -1)
	    FRETURN (-1);
	  if (!bunny_read_text(code, i, "]"))
	    RETURN ("Missing ']' after '[constant'."); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i - 1, ']', &p->no_space_inside_brackets.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  once = true;
	}
    }
  while (once);

  if (parameters && p->last_declaration.is_typedef)
    mark_type_name_as_function
      (p, p->last_declaration.function[0] ?
       p->last_declaration.function : p->last_declaration.symbol);

  // On a trouvé une déclaration de variable
  if (parameters == false &&
      p->last_declaration.inside_variable &&
      !p->last_declaration.is_typedef &&
      !current_type_is_function(p))
    {
      // C'est une locale
      if (p->last_declaration.inside_function)
	{
	  p->last_declaration.nbr_variable += 1;
	  if (p->maximum_variable.active)
	    {
	      if (p->maximum_variable.value == 0)
		{
		  if (!add_warning
		      (p, IZ(p, &j), code, j, &p->maximum_variable.counter,
		       "Variable are forbidden."))
		    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
		}
	      else if (p->maximum_variable.value < p->last_declaration.nbr_variable)
		{
		  if (!add_warning
		      (p, IZ(p, &j), code, j, &p->maximum_variable.counter,
		       "Too many variables. Maximum is %d.",
		       p->maximum_variable.value))
		    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
		}
	    }
	  check_style(p, "local variable", &p->last_declaration.symbol[0],
		      &p->local_variable_style,
		      &p->local_variable_infix,
		      code, j);
	}
      // C'est une globale
      else if (p->last_declaration.inside_parameter == false)
	{
	  if (p->no_global.value)
	    if (!add_warning
		(p, IZ(p, &j), code, j, &p->no_global.counter,
		 "Global variable are forbidden."))
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (p->all_globals_are_const.value && p->last_declaration.is_const == false)
	    if (!add_warning
		(p, IZ(p, &j), code, j, &p->all_globals_are_const.counter,
		 "Global variable %s is not const. "
		 "Global variables must be all const.",
		 p->last_declaration.symbol))
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  check_style
	    (p, "global variable", &p->last_declaration.symbol[0],
	     &p->global_variable_style,
	     &p->global_variable_infix,
	     code, j);
	}
    }

  // L'alignement des paramètres
  if (p->last_declaration.inside_parameter &&
      p->parameter_name_alignment.value)
    {
      int		cnt;

      cnt = count_to_new_line(p, code, j);
      if (p->local_parameter_name_alignment == -1)
	p->local_parameter_name_alignment = cnt;
      if (cnt != p->local_parameter_name_alignment)
	if (!add_warning
	    (p, IZ(p, &j), code, j, &p->parameter_name_alignment.counter,
	     "Parameter names must be aligned."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
    }

  // L'alignement des variables et fonctions
  if ((p->last_declaration.inside_function
       || p->last_declaration.inside_union
       || p->last_declaration.inside_struct
       ) && p->symbol_alignment.value)
    {
      int		cnt;

      cnt = count_to_new_line(p, code, j);
      if (p->local_symbol_alignment == -1)
	p->local_symbol_alignment = cnt;
      if (cnt != p->local_symbol_alignment)
	if (!add_warning
	    (p, IZ(p, &j), code, j, &p->symbol_alignment.counter,
	     "Function and variable names must be aligned."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
    }
  FRETURN (1);
}

int			read_declarator(t_parsing		*p,
					const char		*code,
					ssize_t			*i)
{
  int			ret;
  
  FTRACE(code, *i);
  p->last_declaration.nbr_pointer = 0;
  if (read_pointer(p, code, i) == -1)
    FRETURN (-1);
  if (p->last_declaration.nbr_pointer)
    {
      if (!check_pointer_star_position(p, code, *i))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
    }
  ret = read_direct_declarator(p, code, i);
  FRETURN (ret);
}

int			read_initializer_list(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);
  do
    if ((ret = read_initializer(p, code, i)) != 1)
      {
	if (cnt == 0 || ret == -1)
	  FRETURN (ret);
	RETURN ("Excessive ',' found in initializer list."); // LCOV_EXCL_LINE
      }
    else
      cnt += ret;
  while (bunny_read_text(code, i, ","));
  FRETURN (cnt > 0 ? 1 : 0);
}

int			read_initializer(t_parsing		*p,
					 const char		*code,
					 ssize_t		*i)
{
  FTRACE(code, *i);
  if (bunny_read_text(code, i, "{"))
    {
      int		cnt = 0;
      int		ret;

      do
	{
	  if (cnt > 0)
	    if (check_no_space_before_space_after(p, code, *i) == -1)
	      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	  if ((ret = read_initializer_list(p, code, i)) != 1)
	    {
	      if (cnt == 0 || ret == -1)
		FRETURN (ret);
	      RETURN("Excessive ',' found in initializer."); // LCOV_EXCL_LINE
	    }
	  else
	    cnt += 1;
	}
      while (bunny_read_text(code, i, ","));
      if (!bunny_read_text(code, i, "}"))
	RETURN("Missing '}' at after '{initializer'."); // LCOV_EXCL_LINE
      FRETURN (1);
    }
  FRETURN (read_assignment_expression(p, code, i));
}

int			read_init_declarator(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  int			ret;

  FTRACE(code, *i);
  p->last_declaration.inside_variable = true;

  // On déclare une variable, globale ou locale
  if ((ret = read_declarator(p, code, i)) != 1)
    {
      p->last_declaration.inside_variable = false;
      FRETURN (ret);
    }
  p->last_declaration.inside_variable = false;
  if (!bunny_read_text(code, i, "="))
    FRETURN (1);
  // On établie la valeur d'une variable!

  // Est ce interdit ?
  if (p->local_variable_inline_init_forbidden.value == 1
      && p->last_declaration.inside_function)
    {
      if (!add_warning
	  (p, IZ(p, i), code, *i, &p->local_variable_inline_init_forbidden.counter,
	   "Forbidden declaration/assignation of variable."))
	RETURN("Memory exhausted"); // LCOV_EXCL_LINE
    }
  if (read_initializer(p, code, i) != 1)
    RETURN("Problem encountered with initializer after '='."); // LCOV_EXCL_LINE
  FRETURN (1);
}

int			read_init_declarator_list(t_parsing	*p,
						  const char	*code,
						  ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  FTRACE(code, *i);
  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_init_declarator(p, code, i)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    FRETURN (ret);
	  RETURN("Excessive ',' found in declaration."); // LCOV_EXCL_LINE
	}
      else
	cnt += ret;
    }
  while (bunny_read_text(code, i, ","));
  if (cnt > 1 && p->single_instruction_per_line.value &&
      !current_type_is_function(p))
    if (!add_warning
	(p, IZ(p, i), code, *i, &p->single_instruction_per_line.counter,
	 "Only a single declaration is authorized per line."))
      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  FRETURN (cnt > 0 ? 1 : 0);
}

int			read_gcc_attribute_list_node(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			ret;
  bool			once = false;

  FTRACE(code, *i);
  
  while (read_identifier(p, code, i, true) == 1)
    once = true;
  if (once == false)
    if (bunny_read_integer(code, i, &ret) == false)
      FRETURN (0);
  
  if (bunny_read_text(code, i, "("))
    {
      if (!check_parenthesis_space
	  (p, code, *i - 1, '(',
	   &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      int		cnt = 0;

      do
	{
	  if (cnt > 0)
	    if (check_no_space_before_space_after(p, code, *i) == -1)
	      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	  if ((ret = read_gcc_attribute_list_node(p, code, i)) == -1)
	    FRETURN (ret);
	  cnt += 1;
	}
      while (bunny_read_text(code, i, ","));
      if (bunny_read_text(code, i, ")") == false)
	RETURN("Missing ')' to close attribute parameter."); // LCOV_EXCL_LINE
      if (!check_parenthesis_space
	  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE

    }
  FRETURN (1);
}

int			read_gcc_attribute(t_parsing		*p,
					   const char		*code,
					   ssize_t		*i)
{
  int			cnt = 0;

  FTRACE(code, *i);
  while (check_read_text(code, i, "__attribute__"))
    {
      if (bunny_read_text(code, i, "((") == false)
	RETURN("\"((\" was expected after __attribute__."); // LCOV_EXCL_LINE
      if (!check_parenthesis_space(p, code, *i - 1, '(',
				   &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      bool		comma_end = false;

      while (read_gcc_attribute_list_node(p, code, i))
	{
	  comma_end = false;
	  if (bunny_check_text(code, i, "))") == false)
	    {
	      if (bunny_read_text(code, i, ",") == false)
		RETURN("',' was expected to separate __attribute__ parameters."); // LCOV_EXCL_LINE
	      if (check_no_space_before_space_after(p, code, *i) == -1)
		RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	      comma_end = true;
	    }
	  if (!check_parenthesis_space(p, code, *i - 1, ')',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (comma_end)
	RETURN("Missing parameters for __attribute__ after ','"); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, "))") == false)
	RETURN("\"))\" was expected to close __attribute__."); // LCOV_EXCL_LINE
      if (!check_parenthesis_space
	  (p, code, *i - 1, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      cnt += 1;
    }
  FRETURN (cnt >= 1 ? 1 : 0);
}

int			read_assembler(t_parsing		*p,
				       const char		*code,
				       ssize_t			*i)
{
  FTRACE(code, *i);
  if (check_read_text(code, i, "__asm__") == false && check_read_text(code, i, "asm") == false)
    FRETURN (0);
  if (bunny_read_text(code, i, "(") == false)
    RETURN ("'(' was expected after asm.");

  char			buffer[4096];

  while (bunny_read_cstring(code, i, &buffer[0], sizeof(buffer)));

  if (bunny_read_text(code, i, ")") == false)
    RETURN ("')' was expected to close '(' in asm.");
  FRETURN (1);
}

int			read_declaration(t_parsing		*p,
					 const char		*code,
					 ssize_t		*i)
{
  int			ret;

  FTRACE(code, *i);

  // Si ce n'est pas une déclaration
  if (bunny_check_text(code, i, "("))
    FRETURN(0);
  
  p->last_declaration.was_defining = false;
  if ((ret = read_declaration_specifiers(p, code, i, false)) != 1)
    FRETURN (ret);
  if (!p->last_declaration.is_typedef &&
      !p->last_declaration.was_defining &&
      !p->last_declaration.is_struct_last_typedef &&
      !p->last_declaration.is_union_last_typedef &&
      !p->last_declaration.is_enum_last_typedef &&
      !current_type_is_function(p) &&
      check_base_indentation(p, code, *i) == -1)
    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
  if (read_gcc_attribute(p, code, i) == -1)
    FRETURN (-1);
  if (read_init_declarator_list(p, code, i) == -1)
    FRETURN (-1);

  if (read_gcc_attribute(p, code, i) == -1)
    FRETURN (-1);  
  if (read_assembler(p, code, i) == -1)
    FRETURN (-1);
  if (read_gcc_attribute(p, code, i) == -1)
    FRETURN (-1);
  if (bunny_read_text(code, i, ";") == false)
    RETURN ("Missing ';' after declaration."); // LCOV_EXCL_LINE
  if (check_white_then_newline(p, code, *i, false) == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  if (p->last_declaration.inside_function)
    source_report_add_instruction(p, SOURCE_REPORT_DECLARATION);
  FRETURN (1);
}

int			read_external_declaration(t_parsing	*p,
						  const char	*code,
						  ssize_t	*i)
{
  int			ret;
  ssize_t		j = *i;

  FTRACE(code, *i);
  
  // Regardons si on declare une fonction.
  if ((ret = read_function_definition(p, code, &j)) != 0)
    {
      // C'était bien une fonction... ou une erreur!
      if (ret != -1)
	*i = j;
      FRETURN (ret);
    }

  // Ca doit etre autre chose...
  FRETURN (read_declaration(p, code, i));
}

int			read_translation_unit(t_parsing		*p,
					      const char	*file,
					      const char	*code,
					      ssize_t		*i,
					      bool		verbose,
					      bool		was_preprocessed)
{
  int			cnt = 0;
  int			ret;
  int			error_checkpoint;

  FTRACE(code, *i);
  gl_parsing_save = p;

  p->file = file;
  p->last_declaration.indent_depth = 0;

  p->local_parameter_type_alignment = -1;
  p->local_parameter_name_alignment = -1;
  p->local_symbol_alignment = -1;
  p->global_parameter_name_alignment = -1;
  p->global_symbol_alignment = -1;

  p->func_ptr_counter = 0;

  // Recherche du dernier marqueur du preprocessor, en fonction de la situation
  char terminator = was_preprocessed ? '\035' : '#';

  ret = strlen(code);
  while (ret > 0 && code[ret] != terminator)
    ret -= 1;
  if (code[ret] == '#')
    while (code[ret] && code[ret] != '\n')
      ret += 1;
  // On trouve le marqueur spécial "post include"
  else if (code[ret] == '\035')
    {} // read_whitespace le considerera comme un espace
  p->last_line_marker = ret;
  p->last_line_marker_line = 0;
  while (ret > 0)
    {
      if (code[ret] == '\n')
	p->last_line_marker_line += 1;
      ret -= 1;
    }

  gl_bunny_read_whitespace = read_whitespace;

  if (!was_preprocessed && check_header(p, code) == false)
    ret = -1;
  else
    ret = 1;
  while (ret == 1 && code[*i])
    {
      read_whitespace(code, i);
      p->local_parameter_type_alignment = -1;
      p->local_parameter_name_alignment = -1;
      p->local_symbol_alignment = -1;
      error_checkpoint = p->last_error_id;
      
      if ((ret = read_external_declaration(p, code, i)) == -1)
	{ // LCOV_EXCL_START
	  if (verbose)
	    {
	      // Erreur dans le C.
	      // Normalement ca n'arrive pas car le code a déjà été compilé avec succès.
	      int		j = *i;
	      int		col;

	      while (j > 0 && code[j] != '\n')
		j -= 1;
	      if (code[j] == '\n')
		j += 1;
	      col = (int)*i - j;
	      printf
		("Parsing stopped line %d, column %d.\n", bunny_which_line(code, *i), col + 1);
	      while (code[*i] && code[*i] != '\n')
		*i += 1;
	      printf("%.*s\n", (int)(*i - j), &code[j]);
	      for (int x = 0; x < col; ++x)
		printf(" ");
	      printf("^\n");
	      printf("Error backtrack:\n");
	      while (p->last_error_id > 0)
		{
		  printf(" - %s\n", p->last_error_msg[p->last_error_id]);
		  p->last_error_id -= 1;
		}
	    }
	  gl_bunny_read_whitespace = NULL;
	  FRETURN (-1);
	} // LCOV_EXCL_STOP
      else
	{
	  if (*i <= p->last_line_marker)
	    p->last_error_id = error_checkpoint;
	  // On enregistre les indentations au niveau global uniquement dans la partie élève...
	  if (p->last_line_marker < *i)
	    {
	      if (p->global_parameter_name_alignment == -1)
		{
		  if (p->local_parameter_name_alignment != -1)
		    p->global_parameter_name_alignment = p->local_parameter_name_alignment;
		}
	      else if (p->local_parameter_name_alignment != -1 &&
		       p->global_parameter_name_alignment != p->local_parameter_name_alignment)
		{
		  if (!add_warning
		      (p, IZ(p, i), code, *i, &p->file_parameter_name_alignment.counter,
		       "Parameter name must be aligned globally in your file."))
		    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
		  p->global_parameter_name_alignment = p->local_parameter_name_alignment;
		}

	      if (p->global_symbol_alignment == -1)
		{
		  if (p->local_symbol_alignment != -1)
		    p->global_symbol_alignment = p->local_symbol_alignment;
		}
	      else if (p->local_symbol_alignment != -1 &&
		       p->global_symbol_alignment != p->local_symbol_alignment)
		{
		  if (!add_warning
		      (p, IZ(p, i), code, *i, &p->file_symbol_alignment.counter,
		       "Function and variable name must be aligned "
		       "globally in your file."))
		    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
		  p->global_symbol_alignment = p->local_symbol_alignment;
		}
	    }

	  cnt += ret;
	}
      read_whitespace(code, i);
    }

  gl_bunny_read_whitespace = NULL;
  if (p->no_trailing_whitespace.value)
    if (check_trailing_whitespace(p, code) == -1)
      RETURN("Memory exhausted."); // LCOV_EXCL_LINE

  p->nbr_error_points = 0;
  for (t_criteria *c = criteria_iterator_start(p); c < criteria_iterator_end(p); ++c)
    {
      if (!c->active)
	continue ;
      if (c->counter > 0)
	{
	  // printf("Mistake : %td\n", c - criteria_iterator_start(p));
	  p->nbr_mistakes += 1;
	  p->nbr_error_points += c->pts;
	}
    }

  FRETURN (cnt >= 1 ? 1 : 0);
}

static void		fetchi(t_bunny_configuration		*e,
			       int				*i,
			       const char			*f,
			       int				def)
{
  if (!bunny_configuration_getf(e, i, f))
    *i = def;
}

static bool		fetch_criteria(t_bunny_configuration	*cnf,
				       t_criteria		*crit,
				       const char		*field)
{
  t_criteria		tmp;
  bool			found;

  memset(&tmp, 0, sizeof(tmp));
  tmp.pts = 1;
  found = false;
  // Trois syntaxes alternatives:
  // [Champ Value = 3 Points = 2 ]
  // Ou:
  // Champ = 3, 2
  // Ou:
  // Champ = 3
  // ChampPts = 2
  if (bunny_configuration_getf(cnf, &tmp.value, "%s.Value", field))
    {
      tmp.active = true;
      bunny_configuration_getf(cnf, &tmp.pts, "%s.Points", field);
      found = true;
    }
  else if (bunny_configuration_getf(cnf, &tmp.pts, "%s[1]", field))
    {
      if (bunny_configuration_getf(cnf, &tmp.value, "%s[0]", field))
	tmp.active = true;
      found = true;
    }
  else if (bunny_configuration_getf(cnf, &tmp.value, "%s", field))
    {
      bunny_configuration_getf(cnf, &tmp.pts, "%sPts", field);
      tmp.active = true;
      found = true;
    }
  if (bunny_configuration_getf(cnf, NULL, "%s.Disabled", field))
    {
      tmp.active = false;
      found = true;
    }
  if (!found)
    return (false);
  tmp.pts = abs(tmp.pts);
  memcpy(crit, &tmp, sizeof(*crit));
  return (true);
}

static void		strxcpy(char					*target,
				const char				*source,
				int					tarlen,
				int					srclen)
{
  if (tarlen < srclen)
    {
      memcpy(target, source, tarlen);
      target[tarlen] = '\0';
    }
  else
    {
      memcpy(target, source, srclen);
      target[srclen] = '\0';
    }
}

static bool		fetch_string_criteria(t_bunny_configuration	*cnf,
					      t_string_criteria		*crit,
					      const char		*field)
{
  t_string_criteria	tmp;
  const char		*str;
  bool			found;

  FADD();
  memset(&tmp, 0, sizeof(tmp));
  tmp.pts = 1;
  found = false;
  // Trois syntaxes alternatives:
  // [Champ Value = 1 Position = 2 Points = 3 ]
  // Ou:
  // Champ = 1, 2, 3
  // Ou:
  // Champ = 1
  // ChampPosition = 2
  // ChampPts = 3
  //
  // Position peut etre un entier valant O ou 1, ou "Prefix" ou "Suffix"
  if (bunny_configuration_getf(cnf, &str, "%s.Value", field))
    {
      tmp.active = true;
      strxcpy(&tmp.value[0], str, sizeof(tmp.value) - 1, strlen(str));
      if (bunny_configuration_getf(cnf, &str, "%s.Position", field))
	{
	  if (strcmp(str, "Prefix") == 0)
	    tmp.position = 0;
	  else if (strcmp(str, "Suffix") == 0)
	    tmp.position = 1;
	  else
	    bunny_configuration_getf(cnf, &tmp.position, "%s.Position", field);
	}
      bunny_configuration_getf(cnf, &tmp.pts, "%s.Points", field);
      found = true;
    }
  else if (bunny_configuration_getf(cnf, &tmp.pts, "%s[2]", field))
    {
      if (bunny_configuration_getf(cnf, &str, "%s[1]", field))
	{
	  if (strcmp(str, "Prefix") == 0)
	    tmp.position = 0;
	  else if (strcmp(str, "Suffix") == 0)
	    tmp.position = 1;
	  else
	    bunny_configuration_getf(cnf, &tmp.position, "%s[1]", field);
	}
      if (bunny_configuration_getf(cnf, &str, "%s[0]", field))
	{
	  tmp.active = true;
	  strxcpy(&tmp.value[0], str, sizeof(tmp.value) - 1, strlen(str));
	}
      found = true;
    }
  else if (bunny_configuration_getf(cnf, &str, "%s", field))
    {
      strxcpy(&tmp.value[0], str, sizeof(tmp.value) - 1, strlen(str));
      if (bunny_configuration_getf(cnf, &str, "%sPosition", field))
	{
	  if (strcmp(str, "Prefix") == 0)
	    tmp.position = 0;
	  else if (strcmp(str, "Suffix") == 0)
	    tmp.position = 1;
	  else
	    bunny_configuration_getf(cnf, &tmp.position, "%sPosition", field);
	}
      bunny_configuration_getf(cnf, &tmp.pts, "%sPts", field);
      tmp.active = true;
      found = true;
    }
  if (bunny_configuration_getf(cnf, NULL, "%s.Disabled", field))
    {
      tmp.active = false;
      found = true;
    }
  if (!found)
    FRETURN (false);
  tmp.pts = abs(tmp.pts);
  memcpy(crit, &tmp, sizeof(*crit));
  FRETURN (true);
}

int			check_header_file(t_parsing		*p,
					  const char		*code)
{
  FADD();
  // Code ici doit etre le fichier nature, et non pas le document préprocessé
  if (check_header(p, code) == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  FRETURN (true);
}

int			check_all_lines_width(t_parsing		*p,
					      const char	*code)
{
  int			i;
  int			j;

  i = 0;
  j = 0;
  FADD();
  while (code[j] != '\0')
    {
      while (code[j] != '\0' && code[j] != '\n')
	j = j + 1;
      if (check_line_width(p, code, i, j) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      i = (j += (code[j] != '\0' ? 1 : 0));
    }
  FRETURN (1);
}

void			reset_norm_status(t_parsing		*p)
{
  // reset de certains compteurs qui ne peuvent etre transvasé de fichiers en fichiers
  p->ldec_non_static_function_per_file = 0;
  p->ldec_function_per_file = 0;
}

void			load_norm_configuration(t_parsing	*p,
						t_bunny_configuration *e)
{
  memset(p, 0, sizeof(*p));

  // N'existent que les types présents dans l'unité de compilation...
  p->last_new_type = 0;
  
  p->nbr_mistakes = 0; // Le nombre d'erreur faites
  p->nbr_error_points = 0; // Le nombre de points d'erreur accumulés
  p->last_error_id = -1;
  p->configuration = e;

  fetchi(e, &p->maximum_error_points, "Tolerance", -1);

  fetch_criteria(e, &p->function_per_file, "FunctionPerFile");
  fetch_criteria(e, &p->non_static_function_per_file, "NonStaticFunctionPerFile");

  {
    t_criteria		style;
    t_string_criteria	infix;

    memset(&style, 0, sizeof(style));
    memset(&infix, 0, sizeof(infix));
    fetch_criteria(e, &style, "GlobalStyle");
    fetch_string_criteria(e, &infix, "GlobalInfix");
    if (style.active)
      {
	memcpy(&p->function_style, &style, sizeof(style));
	memcpy(&p->local_variable_style, &style, sizeof(style));
	memcpy(&p->global_variable_style, &style, sizeof(style));
	memcpy(&p->struct_style, &style, sizeof(style));
	memcpy(&p->enum_style, &style, sizeof(style));
	memcpy(&p->union_style, &style, sizeof(style));
	memcpy(&p->struct_attribute_style, &style, sizeof(style));
	memcpy(&p->union_attribute_style, &style, sizeof(style));
	memcpy(&p->function_pointer_attribute_style, &style, sizeof(style));
	memcpy(&p->function_pointer_type_style, &style, sizeof(style));
	memcpy(&p->typedef_style, &style, sizeof(style));
      }
    if (infix.active)
      {
	memcpy(&p->function_infix, &infix, sizeof(infix));
	memcpy(&p->local_variable_infix, &infix, sizeof(infix));
	memcpy(&p->global_variable_infix, &infix, sizeof(infix));
	memcpy(&p->struct_infix, &infix, sizeof(infix));
	memcpy(&p->enum_infix, &infix, sizeof(infix));
	memcpy(&p->union_infix, &infix, sizeof(infix));
	memcpy(&p->struct_attribute_infix, &infix, sizeof(infix));
	memcpy(&p->union_attribute_infix, &infix, sizeof(infix));
	memcpy(&p->function_pointer_attribute_infix, &infix, sizeof(infix));
	memcpy(&p->function_pointer_type_infix, &infix, sizeof(infix));
	memcpy(&p->typedef_infix, &infix, sizeof(infix));
      }
  }

  fetch_criteria(e, &p->function_style, "FunctionNameStyle");
  fetch_string_criteria(e, &p->function_infix, "FunctionNameInfix");

  fetch_criteria(e, &p->local_variable_style, "LocalVariableStyle");
  fetch_criteria(e, &p->local_variable_style, "LocalVariableNameStyle");
  fetch_string_criteria(e, &p->local_variable_infix, "LocalVariableInfix");
  fetch_string_criteria(e, &p->local_variable_infix, "LocalVariableNameInfix");

  fetch_criteria(e, &p->global_variable_style, "GlobalVariableStyle");
  fetch_criteria(e, &p->global_variable_style, "GlobalVariableNameStyle");
  fetch_string_criteria(e, &p->global_variable_infix, "GlocalVariableInfix");
  fetch_string_criteria(e, &p->global_variable_infix, "GlobalVariableInfix");
  fetch_string_criteria(e, &p->global_variable_infix, "GlobalVariableNameInfix");

  fetch_criteria(e, &p->struct_style, "StructStyle");
  fetch_criteria(e, &p->struct_style, "StructNameStyle");
  fetch_string_criteria(e, &p->struct_infix, "StructInfix");
  fetch_string_criteria(e, &p->struct_infix, "StructNameInfix");

  fetch_criteria(e, &p->enum_style, "EnumStyle");
  fetch_criteria(e, &p->enum_style, "EnumNameStyle");
  fetch_string_criteria(e, &p->enum_infix, "EnumInfix");
  fetch_string_criteria(e, &p->enum_infix, "EnumNameInfix");
  fetch_criteria(e, &p->enum_constant_style, "EnumConstantStyle");
  fetch_string_criteria(e, &p->enum_constant_infix, "EnumConstantInfix");

  fetch_criteria(e, &p->union_style, "UnionStyle");
  fetch_criteria(e, &p->union_style, "UnionNameStyle");
  fetch_string_criteria(e, &p->union_infix, "UnionInfix");
  fetch_string_criteria(e, &p->union_infix, "UnionNameInfix");

  fetch_criteria(e, &p->struct_attribute_style, "AttributetStyle");
  fetch_criteria(e, &p->struct_attribute_style, "AttributeStyle");
  fetch_criteria(e, &p->struct_attribute_style, "StructAttributeStyle");
  fetch_criteria(e, &p->struct_attribute_style, "StructAttributeNameStyle");
  fetch_string_criteria(e, &p->struct_attribute_infix, "AttributeInfix");
  fetch_string_criteria(e, &p->struct_attribute_infix, "StructAttributeInfix");
  fetch_string_criteria(e, &p->struct_attribute_infix, "StructAttributeNameInfix");
  fetch_criteria(e, &p->union_attribute_style, "AttributetStyle");
  fetch_criteria(e, &p->union_attribute_style, "AttributeStyle");
  fetch_criteria(e, &p->union_attribute_style, "UnionAttributeStyle");
  fetch_criteria(e, &p->union_attribute_style, "UnionAttributeNameStyle");
  fetch_string_criteria(e, &p->union_attribute_infix, "AttributeInfix");
  fetch_string_criteria(e, &p->union_attribute_infix, "UnionAttributeInfix");
  fetch_string_criteria(e, &p->union_attribute_infix, "UnionAttributeNameInfix");

  fetch_criteria(e, &p->function_pointer_attribute_style, "FunctionPointerAttributeStyle");
  fetch_string_criteria(e, &p->function_pointer_attribute_infix, "FunctionPointerAttributeInfix");

  fetch_criteria(e, &p->function_pointer_type_style, "FunctionPointerTypeStyle");
  fetch_string_criteria(e, &p->function_pointer_type_infix, "FunctionPointerTypeInfix");

  fetch_criteria(e, &p->typedef_style, "TypedefStyle");
  fetch_criteria(e, &p->typedef_style, "TypedefNameStyle");
  fetch_string_criteria(e, &p->typedef_infix, "TypedefInfix");
  fetch_string_criteria(e, &p->typedef_infix, "TypedefNameInfix");
  fetch_criteria(e, &p->typedef_matching, "TypedefMatching");

  fetch_criteria(e, &p->local_variable_inline_init_forbidden, "LocalVariableInlineInitForbidden");

  fetch_criteria(e, &p->function_matching_path, "FunctionMatchingPath");

  fetch_criteria(e, &p->indent_style, "IndentationStyle");
  fetch_criteria(e, &p->base_indent, "IndentationSize");
  fetch_criteria(e, &p->tab_or_space, "IndentationToken");
  fetch_criteria(e, &p->declaration_statement_separator, "DeclarationStatementSeparator");
  fetch_criteria(e, &p->no_empty_line_in_function, "NoEmptyLineInFunction");
  fetch_criteria(e, &p->no_trailing_whitespace, "TrailingWhitespace");
  fetch_criteria(e, &p->no_trailing_whitespace, "NoTrailingWhitespace");
  fetch_criteria(e, &p->single_instruction_per_line, "SingleInstructionPerLine");
  fetch_criteria(e, &p->max_column_width, "MaximumLineWidth");
  fetch_criteria(e, &p->max_function_length, "MaximumFunctionLength");
  fetch_criteria(e, &p->max_parameter, "MaximumFunctionParameter");
  fetch_criteria(e, &p->maximum_scope_length, "MaximumScopeLength");
  fetch_criteria(e, &p->always_braces, "AlwaysBraces");
  fetch_criteria(e, &p->avoid_braces, "AvoidBracesForSingleLine");
  fetch_criteria(e, &p->space_after_statement, "SpaceAfterStatement");
  fetch_criteria(e, &p->space_around_binary_operator, "SpaceAroundBinaryOperator");
  fetch_criteria(e, &p->space_after_comma, "SpaceAfterComma");
  fetch_criteria(e, &p->only_by_reference, "OnlyByReference");
  fetch_criteria(e, &p->no_space_inside_parenthesis, "NoSpaceInsideParenthesis");
  fetch_criteria(e, &p->no_space_inside_brackets, "NoSpaceInsideBrackets");
  if (fetch_string_criteria(e, &p->header, "Header") && p->header.active)
    {
      const char	*str;

      (void)(bunny_configuration_getf(e, &str, "Header.Value")
	     || bunny_configuration_getf(e, &str, "Header[0]")
	     || bunny_configuration_getf(e, &str, "Header"));
      strxcpy(&p->header_data[0], str, sizeof(p->header_data) - 1, strlen(str));
    }

  fetch_criteria(e, &p->symbol_alignment, "FunctionVariableDefinitionAlignment");
  fetch_criteria(e, &p->parameter_type_alignment, "ParameterTypeAlignment");
  fetch_criteria(e, &p->parameter_name_alignment, "ParameterNameAlignment");
  fetch_criteria(e, &p->file_symbol_alignment, "GlobalFunctionVariableDefinitionAlignment");
  fetch_criteria(e, &p->file_parameter_name_alignment, "GlobalParameterNameAlignment");

  fetch_criteria(e, &p->inbetween_ptr_symbol_space, "SpaceAroundInbetweenPointerStars");
  fetch_criteria(e, &p->ptr_symbol_on_name, "PointerStarOnName");
  fetch_criteria(e, &p->ptr_symbol_on_type, "PointerStarOnType");
  fetch_criteria(e, &p->all_globals_are_const, "AllGlobalsAreConst");
  fetch_criteria(e, &p->no_magic_value, "NoMagicValue");
  fetch_criteria(e, &p->no_short_name, "NoShortName");
  fetch_criteria(e, &p->maximum_variable, "MaximumVariable");
  fetch_criteria(e, &p->no_global, "NoGlobal");
  fetch_criteria(e, &p->return_parenthesis, "ReturnParenthesis");
  fetch_criteria(e, &p->sizeof_parenthesis, "SizeOfParenthesis");
  fetch_criteria(e, &p->forbidden_type, "TypeRestriction");

  fetch_criteria(e, &p->for_forbidden, "ForForbidden");
  fetch_criteria(e, &p->while_forbidden, "WhileForbidden");
  fetch_criteria(e, &p->do_while_forbidden, "DoWhileForbidden");
  fetch_criteria(e, &p->goto_forbidden, "GotoForbidden");
  fetch_criteria(e, &p->goto_forbidden, "GoToForbidden");
  fetch_criteria(e, &p->return_forbidden, "ReturnForbidden");
  fetch_criteria(e, &p->break_forbidden, "BreakForbidden");
  fetch_criteria(e, &p->continue_forbidden, "ContinueForbidden");
  t_criteria if_forbidden;

  memset(&if_forbidden, 0, sizeof(if_forbidden));
  fetch_criteria(e, &if_forbidden, "IfForbidden");
  if (if_forbidden.active == false)
    fetch_criteria(e, &p->maximum_if_in_function, "MaximumIfInFunction");
  else
    {
      p->maximum_if_in_function.active = true;
      p->maximum_if_in_function.value = 0;
    }
  fetch_criteria(e, &p->else_forbidden, "ElseForbidden");
  fetch_criteria(e, &p->switch_forbidden, "SwitchForbidden");
  fetch_criteria(e, &p->inline_mod_forbidden, "InlineModificationForbidden");
  fetch_criteria(e, &p->inline_mod_forbidden, "InlineModForbidden");
  fetch_criteria(e, &p->checked_return, "CheckedReturn");
  fetch_criteria(e, &p->checked_return, "CheckedReturnValue");
  fetch_criteria(e, &p->ternary_forbidden, "TernaryForbidden");
  fetch_criteria(e, &p->no_assignment, "NoAssignment");

  bunny_configuration_getf(e, &p->ansi_c, "AnsiC");
}

