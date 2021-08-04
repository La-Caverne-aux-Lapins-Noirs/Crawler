/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2021
**
** C-C-C CRAWLER!
** Configurable C Code Crawler !
** Bloc constitutif du "TechnoCentre", suite logiciel du projet "Pentacle School"
** Vérificateur de confirmité du code (entre autre) niveau style.
**
** Merci pour la grammaire du C ANSI:
** https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
*/

#include		<ctype.h>
#include		"crawler.h"

char			*strcasestr(const char			*haystack,
				    const char			*needle);

#define			MSG(a) p->last_error_msg[++p->last_error_id] = (a " (" STRINGIFY(__LINE__) ")" )


static const char	*gl_first_char = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN_";
static const char	*gl_second_char = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN_0123456789";

void			reset_last_declaration(t_parsing	*p)
{
  memset(&p->last_declaration, 0, sizeof(p->last_declaration));
}

static bool		bad_style(t_parsing			*p,
				  const char			*context,
				  const char			*symbol,
				  t_criteria			*style,
				  const char			*code,
				  int				pos)
{
  const char		*sname[] = {"uppercased snake case", "snake case", "camel case", "pascal case"};
  char			buffer[512];

  style->counter += 1;
  snprintf(&buffer[0], sizeof(buffer),
	   "Badly styled symbol %s. Expected style was %s for %s. (%s, line %d)",
	   symbol, sname[style->value], context, p->file, bunny_which_line(code, pos));
  if ((p->last_error_msg[++p->last_error_id] = bunny_strdup(&buffer[0])) == NULL)
    { // LCOV_EXCL_START
      p->last_error_id -= 1;
      return (false);
    } // LCOV_EXCL_STOP
  return (true);
}

static bool		bad_infix(t_parsing			*p,
				  const char			*context,
				  const char			*symbol,
				  t_string_criteria		*infix,
				  const char			*code,
				  int				pos)
{
  char			buffer[512];

  infix->counter += 1;
  snprintf(&buffer[0], sizeof(buffer),
	   "Missing %s %s for %s in symbol %s. (%s, line %d)",
	   infix->position == 0 ? "prefix" : "suffix",
	   &infix->value[0],
	   context,
	   symbol,
	   p->file,
	   bunny_which_line(code, pos));
  if ((p->last_error_msg[++p->last_error_id] = bunny_strdup(&buffer[0])) == NULL)
    { // LCOV_EXCL_START
      p->last_error_id -= 1;
      return (false);
    } // LCOV_EXCL_STOP
  return (true);
}

bool			check_style(t_parsing			*p,
				    const char			*context,
				    const char			*symbol,
				    t_criteria			*style,
				    t_string_criteria		*infix,
				    const char			*code,
				    int				pos)
{
  int			up = 0;
  int			down = 0;
  int			i;

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
		      return (false);
		    break ;
		  }
	      }
	    else if (isupper(symbol[i]) == false && isdigit(symbol[i]) == false)
	      {
		if (bad_style(p, context, symbol, style, code, pos) == false)
		  return (false);
		break ;
	      }
	  if (i > 12 && down == 0) // Un long nom et pas d'underscore ? Bizarre...
	    if (bad_style(p, context, symbol, style, code, pos) == false)
	      return (false);
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
		      return (false);
		    break ;
		  }
	      }
	    else if (islower(symbol[i]) == false && isdigit(symbol[i]) == false)
	      {
		if (bad_style(p, context, symbol, style, code, pos) == false)
		  return (false);
		break ;
	      }
	  if (i > 12 && down == 0) // Un long nom et pas d'underscore ? Bizarre...
	    if (bad_style(p, context, symbol, style, code, pos) == false)
	      return (false);
	  break ;
	case CAMEL_CASE:
	  if (islower(symbol[0]) == false)
	    {
	      if (bad_style(p, context, symbol, style, code, pos) == false)
		return (false);
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
		      return (false);
		    break ;
		  }
	      if (!symbol[i])
		if (up * 2 > down) // Il y a trop de majuscule la dedans... étrange.
		  if (bad_style(p, context, symbol, style, code, pos) == false)
		    return (false);
	    }
	  break ;
	case PASCAL_CASE:
	  if (isupper(symbol[0]) == false)
	    {
	      if (bad_style(p, context, symbol, style, code, pos) == false)
		return (false);
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
		      return (false);
		    break ;
		  }
	      if (!symbol[i])
		if (up * 2 > down) // Il y a trop de majuscule la dedans... étrange.
		  if (bad_style(p, context, symbol, style, code, pos) == false)
		    return (false);
	    }
	  break ;
	}
    }

  if (infix->active)
    {
      char		*s = strcasestr(symbol, &infix->value[0]);

      if (infix->position == 0) // Prefixe
	{
	  if (s != symbol || s == NULL) // On est pas au début...
	    if (bad_infix(p, context, symbol, infix, code, pos) == false)
	      return (false);
	}
      else if (infix->position == 1) // Suffixe
	{
	  if (s == NULL || bunny_strcasecmp(s, &infix->value[0]) != 0)
	    if (bad_infix(p, context, symbol, infix, code, pos) == false)
	      return (false);
	}
    }

  return (true);
}

int			read_identifier(t_parsing		*p,
					const char		*code,
					ssize_t			*i,
					bool			kwx)
{
  static const char	*kw[] =
    {
      "if", "for", "double", "while", "switch", "break", "goto", "continue", "return",
      "const", "volatile", "default", "case", "typedef", "extern", "static", "auto", "register",
      "void", "char", "short", "int", "long", "float", "do", "signed", "unsigned"
    };
  ssize_t		j = *i;
  int			x;

  if (kwx == false)
    {
      // On cherche si c'est un mot clef...
      for (x = 0; x < NBRCELL(kw) && bunny_read_text(code, &j, kw[x]) == false; ++x);
      if (x != NBRCELL(kw) && (code[j] == '\0' || strchr(gl_second_char, code[j]) == NULL)) // Au cas ou ce soit... "ifa" par exemple.
	return (0);
    }

  read_whitespace(code, i);
  j = *i;
  if (bunny_read_char(code, i, gl_first_char) == false)
    return (0);
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

  if (p->no_short_name.active && (int)strlen(&p->last_declaration.symbol[0]) < p->no_short_name.value)
    {
      if (strcmp(&p->last_declaration.symbol[0], "i") == 0)
	return (1);
      if (strcmp(&p->last_declaration.symbol[0], "j") == 0)
	return (1);
      if (strcmp(&p->last_declaration.symbol[0], "k") == 0)
	return (1);
      if (strcmp(&p->last_declaration.symbol[0], "cnt") == 0)
	return (1);
      if (strcmp(&p->last_declaration.symbol[0], "ret") == 0)
	return (1);
      if (strcmp(&p->last_declaration.symbol[0], "end") == 0)
	return (1);
      if (strcmp(&p->last_declaration.symbol[0], "go") == 0)
	return (1);
      if (!add_warning(p, IZ(p, i), code, *i, &p->no_short_name.counter,
		       "The name '%s' is too short. Minimum was %d.",
		       &p->last_declaration.symbol[0], p->no_short_name.value))
	RETURN ("Memory exhausted"); // LCOV_EXCL_LINE
    }

  return (1);
}

int			read_identifier_list(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_identifier(p, code, i, false)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    return (ret);
	  RETURN("Excessive ',' found in declaration."); // LCOV_EXCL_LINE
	}
      else
	cnt += 1;
    }
  while (bunny_read_text(code, i, ","));
  return (cnt >= 1 ? 1 : 0);
}

int			read_labeled_statement(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  ssize_t		j = *i;

  if (read_identifier(p, code, &j, false))
    {
      if (!bunny_read_text(code, &j, ":"))
	return (0);
      if (check_white_then_newline(p, code, j, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      *i = j;
      return (read_statement(p, code, i));
    }
  if (bunny_read_text(code, i, "case"))
    {
      if (read_constant_expression(p, code, i) != 1)
	RETURN ("Missing symbol after 'case'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ":"))
	RETURN ("Missing token ':' after symbol used by 'case'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      return (read_statement(p, code, i));
    }
  if (bunny_read_text(code, i, "default"))
    {
      if (!bunny_read_text(code, i, ":"))
	RETURN ("Missing token ':' after 'default'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      return (read_statement(p, code, i));
    }
  return (0);
}

int			read_selection_statement(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
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
	  (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_base_indentation(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing condition after 'if ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'if (condition'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value == 0 && !check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
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
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'if (condition)'."); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, "else"))
	{
	  bool		elsefix = false;
	  ssize_t	j = *i;

	  if (bunny_read_text(code, &j, "if") == false)
	    {
	      if (p->indent_style.value == GNU_STYLE)
		{
		  p->last_declaration.indent_depth += 1;
		  elsefix = true;
		}
	      else if (bunny_read_text(code, &j, "{") == false)
		p->last_declaration.depth_bonus = 1;
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
	  if (check_white_then_newline(p, code, *i, true) == false)
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (elsefix)
	    p->last_declaration.indent_depth -= 1;
	  if (read_statement(p, code, i) != 1)
	    RETURN ("Missing statement after 'else'."); // LCOV_EXCL_LINE
	}
      return (1);
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
	  (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing expression after 'switch ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'switch (expression'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.after_statement = true;
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'switch (expression)'."); // LCOV_EXCL_LINE
      return (1);
    }
  return (0);
}

int			read_iteration_statement(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  if (bunny_read_text(code, i, "while"))
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
	  (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing condition after 'while ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'while(condition'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.after_statement = true;
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'while (condition)'."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "do"))
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
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'do'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "while"))
	RETURN ("Missing 'while' after 'do statement'."); // LCOV_EXCL_LINE
      if (check_single_space(p, code, *i) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after after 'do statement while'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing condition after 'do statement while ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'do statement while (condition'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'do statement while (condition)'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "for"))
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
	  (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.inside_for_statement = true;
      if (read_expression_statement(p, code, i) != 1)
	RETURN ("Missing initialization after 'for ('."); // LCOV_EXCL_LINE
      if (read_expression_statement(p, code, i) != 1)
	RETURN ("Missing condition after 'for (initialization;'."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) == -1)
	RETURN ("Invalid increment after 'for (initialization; condition;'."); // LCOV_EXCL_LINE
      p->last_declaration.inside_for_statement = false;
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'for (initialization; condition; increment'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, true) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.after_statement = true;
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'for (initialization; condition; increment)'."); // LCOV_EXCL_LINE
      return (1);
    }
  return (0);
}

int			read_jump_statement(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  if (bunny_read_text(code, i, "goto"))
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
      return (1);
    }
  if (bunny_read_text(code, i, "continue"))
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
      return (1);
    }
  if (bunny_read_text(code, i, "break"))
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
      return (1);
    }
  if (bunny_read_text(code, i, "return"))
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
	return (1);
      // Une expression
      if (p->return_parenthesis.active && bunny_check_text(code, i, "(") == false)
	{
	  flag = true;
	  if (!add_warning
	      (p, IZ(p, i), code, *i, &p->return_parenthesis.counter,
	       "There must be parenthesis around the return expression."))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (read_expression(p, code, i) == -1)
	RETURN ("Missing expression or ';' after 'return'."); // LCOV_EXCL_LINE
      if (p->return_parenthesis.active && flag == false)
	{
	  int j;
	  
	  for (j = *i; j >= 0 && (isspace(code[j]) || code[j] == ';'); --j);
	  if (code[j] != ')')
	    if (!add_warning
		(p, IZ(p, i), code, *i, &p->return_parenthesis.counter,
		 "There must be parenthesis around the return expression."))
	      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'return expression'."); // LCOV_EXCL_LINE
      if (check_white_then_newline(p, code, *i, false) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      return (1);
    }
  return (0);
}

int			read_expression_statement(t_parsing	*p,
						  const char	*code,
						  ssize_t	*i)
{
  int			ret;
  int			sc;

  if ((ret = read_expression(p, code, i)) == -1)
    return (-1);
  if (!(sc = bunny_read_text(code, i, ";")) && ret == 1)
    RETURN ("Missing ';' after expression."); // LCOV_EXCL_LINE
  if (ret == 1)
    {
      p->last_declaration.scope_length += 1;
      if (p->last_declaration.inside_for_statement == false &&
	  check_white_then_newline(p, code, *i, false) == false)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
    }
  return (ret + sc >= 1 ? 1 : 0);
}

int			read_statement(t_parsing		*p,
				       const char		*code,
				       ssize_t			*i)
{
  bool			singleindent = false;
  int			ret;

  if ((ret = read_labeled_statement(p, code, i)) != 0)
    return (ret);

  read_whitespace(code, i);
  if (!bunny_check_text(code, i, "{"))
    {
      if (p->last_declaration.after_statement &&
	  p->always_braces.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->always_braces.counter,
	     "'{' is mandatory after if, while, do, for or switch statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE

      /*
      // Pas d'accolade, il faut quand même indenter.
      if (p->indent_style.value != GNU_STYLE)
	{
	  p->last_declaration.indent_depth += 1;
	  singleindent = true;
	}
      */
    }
  p->last_declaration.after_statement = false;
  p->last_declaration.scope_length = 0;
  if ((ret = read_compound_statement(p, code, i)) != 0)
    goto Return;
  if ((ret = read_expression_statement(p, code, i)) != 0)
    goto Return;
  if ((ret = read_selection_statement(p, code, i)) != 0)
    goto Return;
  if ((ret = read_iteration_statement(p, code, i)) != 0)
    goto Return;
  if ((ret = read_jump_statement(p, code, i)) != 0)
    goto Return;
  return (0);

 Return:
  if (singleindent)
    p->last_declaration.indent_depth -= 1;
  return (ret);
}

int			read_statement_list(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  while ((ret = read_statement(p, code, i)) == 1)
    cnt += 1;
  if (ret == -1)
    return (-1);
  return (cnt >= 1 ? 1 : 0);
}

int			read_compound_statement(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  int			ret;
  int			begin;
  int			end;
  bool			separator;

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

	  if (check_base_indentation(p, code, *i) == -1)
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	}
      else if (p->last_declaration.indent_depth != 0
	       && !check_on_same_line(p, code, *i, "{", true))
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
    }
  if (bunny_read_text(code, i, "{") == false)
    return (0);
  // On augmente l'indentation
  p->last_declaration.indent_depth += 1;

  separator = false;
  read_whitespace(code, i);
  ret = *i;
  begin = *i;
  if (read_declaration_list(p, code, i) == -1)
    return (-1);
  // Il y a eu des déclarations et on veut qu'il y ai une ligne de séparation
  if (p->declaration_statement_separator.active && ret != *i)
    {
      while (isblank(code[*i]))
	*i += 1;
      if (code[*i] != '\n' || code[*i + 1] != '\n')
	{
	  if (!add_warning
	      (p, IZ(p, i), code, *i, &p->declaration_statement_separator.counter,
	       "An empty line was expected between variable declaration and "
	       "statement."))
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	}
      else
	separator = true;
    }
  if (read_statement_list(p, code, i) == -1)
    return (-1);

  end = *i;

  // On diminue l'indentation
  if (p->last_declaration.indent_depth > 0)
    p->last_declaration.indent_depth -= 1;

  read_whitespace(code, i);
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
  return (1);
}

int			read_declaration_list(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  while ((ret = read_declaration(p, code, i)) == 1)
    cnt += 1;
  if (ret == -1)
    return (-1);
  return (cnt >= 1 ? 1 : 0);
}

int			read_function_declaration(t_parsing	*p,
						  const char	*code,
						  ssize_t	*i)
{
  t_criteria		save[&p->end[0] - p->criteria]; // Assez d'espace pour tout.
  t_last_function	savefunc;
  ssize_t		k = *i; // Au cas ou cela ne soit pas une declaration de fonction mais de type.
  ssize_t		j;
  int			last_new_type = p->last_new_type;
  int			ret;

  memcpy(&save[0], p->criteria, sizeof(save));
  memcpy(&savefunc, &p->last_declaration, sizeof(savefunc));
  reset_last_declaration(p);
  // Le type de retour
  if (read_declaration_specifiers(p, code, i) == -1)
    return (-1);

  // L'indentation du nom de fonction
  read_whitespace(code, i);
  p->local_symbol_alignment = count_to_new_line(p, code, *i);
  
  //////////////////////
  // Le nom de fonction
  j = *i;
  if (read_declarator(p, code, i) == -1)
    return (-1);
  // On verifie donc le nom si c'est une fonction non statique
  if (!p->last_declaration.is_static)
    {
      if (check_style(p, "function", &p->last_declaration.symbol[0], &p->function_style, &p->function_infix, code, j) == false)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (p->function_matching_path.active && p->non_static_function_per_file.value == 1)
	{
	  char target[512];

	  store_real_typename
	    (p, &target[0], &p->last_declaration.symbol[0], sizeof(target), 4);
	  if (compare_file_and_function_name(p, &target[0], code, j) == -1)
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	}
    }

  // L'assignation eventuelle...
  if (read_declaration_list(p, code, i) == -1)
    return (-1);
  // Pour savoir si les variables sont globales ou locales, par exemple...
  p->last_declaration.inside_function = true;

  // Le corps de fonction
  if ((ret = read_compound_statement(p, code, i)) != 0)
    {
      if (ret > 0) // Si on a implémenté une fonction pour de vrai
	{
	  // On a limité le nombre de fonction par fichier - donc on compte les fonctions
	  if (p->function_per_file.active)
	    p->function_per_file.counter += 1;
	  if (p->non_static_function_per_file.active)
	    if (!p->last_declaration.is_static)
	      p->non_static_function_per_file.counter += 1;
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
		  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	      }
	  p->last_declaration.nbr_copied_parameters = 0;
	}
      p->last_declaration.inside_function = false;
      return (ret);
    }
  p->last_declaration.inside_function = false;

  // On verifie les paramètres seulement des fonctions implémentées et non des prototypes!
  p->last_declaration.nbr_copied_parameters = 0;

  // On revient en arrière, ca n'était pas une declaration de fonction.
  *i = k;
  // On fait revenir en arrière egalement le compte d'erreur du coup, ca on va les recompter
  memcpy(p->criteria, &save[0], sizeof(save));
  // memcpy(&p->last_declaration, &savefunc, sizeof(savefunc)); // XXXXXXXXXXXXXXXXXXXXXX
  // De même, les types eventuellements déclarés ne le sont pas...
  p->last_new_type = last_new_type;
  return (0);
}

int			read_primary_expression(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  char			buffer[1024];
  int			val;
  double		val2;

  if (read_identifier(p, code, i, false))
    return (1);
  if (bunny_read_cstring(code, i, &buffer[0], sizeof(buffer)))
    return (1);
  if (bunny_read_cchar(code, i, &buffer[0]))
    return (1);
  if (bunny_read_double(code, i, &val2))
    {
      if (p->no_magic_value.active && p->last_declaration.last_char < *i)
	if (val > 1.0 || val < 1.0)
	  if (!add_warning
	      (p, IZ(p, i), code, *i, &p->no_magic_value.counter,
	       "Magic values are forbidden. %f found.",
	       val2))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.last_char = *i;
      return (1);
    }
  if (bunny_read_integer(code, i, &val))
    {
      if (p->no_magic_value.active && p->last_declaration.last_char < *i)
	if (val != 0 && val != 1 && val != -1 && val != 2
	    && val != 8 && val != 16 && val != 24 && val != 32 && val != 48
	    && val != 64 && val != 128 && val != 256 && val != 512 && val != 1024
	    && val != 2048 && val != 4096
	    && (val / 10) != 0)
	  if (!add_warning
	      (p, IZ(p, i), code, *i, &p->no_magic_value.counter,
	       "Magic values are forbidden. %d found.",
	       val))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      p->last_declaration.last_char = *i;
      return (1);
    }
  ssize_t		j = *i;

  if (bunny_read_text(code, &j, "("))
    {
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_expression(p, code, &j) != 1)
	RETURN ("Problem encountered in expression after '('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, &j, ")"))
	RETURN ("Missing ')' after '(expression'."); // LCOV_EXCL_LINE
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      *i = j;
      return (1);
    }
  return (0);
}

int			read_argument_expression_list(t_parsing	*p,
						      const char *code,
						      ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_assignment_expression(p, code, i)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    return (ret);
	  RETURN ("Excessive ',' found in argument list."); // LCOV_EXCL_LINE
	}
      else
	cnt += ret;
    }
  while (bunny_read_text(code, i, ","));
  return (cnt > 0 ? 1 : 0);
}

int			read_postfix_expression(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  int			ret;
  bool			once;

  if ((ret = read_primary_expression(p, code, i)) != 1)
    return (ret);
  do
    {
      once = false;
      if (bunny_read_text(code, i, "["))
	{
	  once = true;
	  if (read_expression(p, code, i) != 1)
	    RETURN ("Problem encountered with expression after '['."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, "]"))
	    RETURN ("Missing ']' after '[expression'."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, "("))
	{
	  once = true;
	  if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	      (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  if (read_argument_expression_list(p, code, i) == -1)
	    RETURN ("Problem encountered with argument list after '('."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(argument'."); // LCOV_EXCL_LINE
	  if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	      (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, ".") || bunny_read_text(code, i, "->"))
	{
	  once = true;
	  if (read_identifier(p, code, i, false) == false)
	    RETURN ("Problem encountered with symbol after '.' or '->'."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, "++"))
	once = true;
      if (bunny_read_text(code, i, "--"))
	once = true;
    }
  while (once);
  return (1);
}

int			read_specifier_qualifier_list(t_parsing	*p,
						      const char *code,
						      ssize_t	*i)
{
  int			cnt = 0;
  int			a;
  int			b;

  do
    {
      if ((a = read_type_specifier(p, code, i)) == -1)
	return (-1);
      cnt += a;
      if ((b = read_type_qualifier(p, code, i)) == -1)
	return (-1);
      cnt += b;
    }
  while (a || b);
  return (cnt >= 1 ? 1 : 0);
}

int			read_type_name(t_parsing		*p,
				       const char		*code,
				       ssize_t			*i)
{
  int			ret;

  if ((ret = read_specifier_qualifier_list(p, code, i)) != 1)
    return (ret);
  if ((ret = read_abstract_declarator(p, code, i)) != 0)
    return (ret);
  return (1);
}

int			read_unary_operator(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  (void)p;
  return (bunny_read_text(code, i, "&")
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
  if (bunny_read_text(code, i, "sizeof"))
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
		  (p, code, *i, '(', &p->no_space_inside_parenthesis.counter))
		RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	      if (read_type_name(p, code, i) != 1)
		RETURN ("Problem encountered with type name after 'sizeof('."); // LCOV_EXCL_LINE
	      if (!bunny_read_text(code, i, ")"))
		RETURN ("Missing ')' after 'sizeof(type name'."); // LCOV_EXCL_LINE
	      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
		  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
		RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	      return (1);
	    }
	  RETURN ("Unknown sequence after 'sizeof'."); // LCOV_EXCL_LINE
	}
      return (1);
    }
  if (bunny_read_text(code, i, "++"))
    {
      if (p->inline_mod_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->inline_mod_forbidden.counter,
	     "'++' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      return (read_unary_expression(p, code, i));
    }
  if (bunny_read_text(code, i, "--"))
    {
      if (p->inline_mod_forbidden.value)
	if (!add_warning
	    (p, IZ(p, i), code, *i, &p->inline_mod_forbidden.counter,
	     "'--' is a forbidden statement."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      return (read_unary_expression(p, code, i));
    }
  if (read_unary_operator(p, code, i) == 1)
    return (read_cast_expression(p, code, i));
  return (read_postfix_expression(p, code, i));
}

int			read_cast_expression(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  ssize_t		j = *i;

  if (bunny_read_text(code, &j, "("))
    {
      if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	  (p, code, j, '(', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (read_type_name(p, code, &j) == 1)
	{
	  *i = j;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(typename'."); // LCOV_EXCL_LINE
	  if (p->no_space_inside_parenthesis.value != 0 && !check_parenthesis_space
	      (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  return (read_cast_expression(p, code, i));
	}
    }
  return (read_unary_expression(p, code, i));
}

int			read_multiplicative_expression(t_parsing *p,
						       const char *code,
						       ssize_t	*i)
{
  int			ret;

  if ((ret = read_cast_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_additive_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  if ((ret = read_multiplicative_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_shift_expression(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  int			ret;

  if ((ret = read_additive_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_relational_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  if ((ret = read_shift_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_equality_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  if ((ret = read_relational_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_and_expression(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  int			ret;

  if ((ret = read_equality_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_exclusive_or_expression(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			ret;

  if ((ret = read_and_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_inclusive_or_expression(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			ret;

  if ((ret = read_exclusive_or_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_logical_and_expression(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  int			ret;

  if ((ret = read_inclusive_or_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_logical_or_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  if ((ret = read_logical_and_expression(p, code, i)) != 1)
    return (ret);
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
  return (1);
}

int			read_assignment_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  read_whitespace(code, i);
  ssize_t		j = *i;
  int			sizof = p->sizeof_parenthesis.counter;

  if (read_unary_expression(p, code, &j) == 1)
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
	  if (check_one_space_around(p, code, k, j - k,
				     p->space_around_binary_operator.value,
				     &p->space_around_binary_operator.counter) == -1)
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  *i = j;
	  if (read_assignment_expression(p, code, i) != 1)
	    RETURN ("Problem encountered with expression after assignment."); // LCOV_EXCL_LINE
	  return (1);
	}
      p->sizeof_parenthesis.counter = sizof;
    }
  return (read_conditional_expression(p, code, i));
}

int			read_expression(t_parsing		*p,
					const char		*code,
					ssize_t			*i)
{
  int		ret;

  if ((ret = read_assignment_expression(p, code, i)) == -1)
    return (-1);
  if (bunny_read_text(code, i, ","))
    {
      if (check_no_space_before_space_after(p, code, *i) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if (p->single_instruction_per_line.value)
	if (!add_warning(p, IZ(p, i), code, *i, &p->single_instruction_per_line.counter,
			 "Only a single instruction is authorized per line."))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_expression(p, code, i)) == 0)
	RETURN ("Excessive ',' found in expression."); // LCOV_EXCL_LINE
    }
  return (ret);
}

int			read_conditional_expression(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  int			ret;

  if ((ret = read_logical_or_expression(p, code, i)) != 1)
    return (ret);
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
      if (read_expression(p, code, i) != 1)
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
  return (1);
}

int			read_constant_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  (void)p;
  return (read_conditional_expression(p, code, i));
}

int			read_type_qualifier(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  (void)p;
  if (bunny_read_text(code, i, "const"))
    {
      p->last_declaration.is_const = true;
      return (1);
    }
  if (bunny_read_text(code, i, "volatile"))
    {
      p->last_declaration.is_volatile = true;
      return (1);
    }
  return (0);
}

int			read_struct_declarator(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  int			ret = *i;

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
      return (ret);
    }
  return (ret);
}

int			read_struct_declarator_list(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_struct_declarator(p, code, i)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    return (ret);
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
  return (cnt > 0 ? 1 : 0);
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
  
  // Lit le type et tout le tralala d'une declaration d'attribut
  if ((a = read_specifier_qualifier_list(p, code, i)) != 1)
    return (a);
  // Lit les multiples declarations, potentiellement séparés par des virgules
  // d'attributs exploitant le type précédent.
  if ((b = read_struct_declarator_list(p, code, i)) == -1)
    RETURN ("Problem encountered with attribute name after type definition in struct."); // LCOV_EXCL_LINE
  if ((a = (a || b)) && bunny_read_text(code, i, ";") == false)
    RETURN ("Missing ';' after attribute definition in struct."); // LCOV_EXCL_LINE
  if (a && check_white_then_newline(p, code, *i, false) == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  return (a ? 1 : 0);
}

int			read_struct_declaration_list(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  do
    {
      if ((ret = read_struct_declaration(p, code, i)) == -1)
	return (-1);
      else
	cnt += ret;
    }
  while (ret == 1);
  return (cnt);
}

bool			read_keyword(t_parsing			*p,
				     const char			*code,
				     ssize_t			*i,
				     const char			*symbol,
				     const char			*symchars)
{
  read_whitespace(code, i);
  gl_bunny_read_whitespace = NULL;
  ssize_t		j = *i;

  // On trouve bien le symbole.
  if (bunny_read_text(code, &j, symbol) == false)
    {
      gl_bunny_read_whitespace = read_whitespace;
      return (false);
    }
  // Mais d'autres caractères suivent, donc le symbole lu est incomplet... ce n'est pas le symbole
  if (bunny_read_char(code, &j, symchars))
    {
      gl_bunny_read_whitespace = read_whitespace;
      return (false);
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
  return (true);
}

// Lit le type
int			read_type_specifier(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  struct
  {
    const char		*name;
    size_t		siz;
  } typ[] =
      {
	{"void", sizeof(void)},
	{"char", sizeof(char)},
	{"short", sizeof(short)},
	{"int", sizeof(int)},
	{"long", sizeof(long)},
	{"float", sizeof(float)},
	{"double", sizeof(double)},
	{"signed", sizeof(signed)},
	{"unsigned", sizeof(unsigned)}
	// , "__int8_t", "__int16_t", "__int32_t", "__int64_t"
      };
  
  // Standard types
  for (size_t j = 0; j < NBRCELL(typ); ++j)
    if (read_keyword(p, code, i, typ[j].name, gl_second_char))
      {
	p->last_declaration.last_type_size = typ[j].siz;
	return (1);
      }
  // Custom types
  for (size_t j = 0; j < p->last_new_type; ++j)
    if (read_keyword(p, code, i, &p->new_type[j].name[0], gl_second_char))
      {
	p->last_declaration.last_type_size = p->new_type[j].size;
	return (1);
      }

  if (bunny_read_text(code, i, "enum"))
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
      if (p->last_declaration.is_typedef && p->last_declaration.scope_depth == 0)
	{
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
      return (1);
    }
  bool punion = p->last_declaration.inside_union;
  bool pstruct = p->last_declaration.inside_struct;

  if ((bunny_read_text(code, i, "union") && (p->last_declaration.inside_union = true))
      || (bunny_read_text(code, i, "struct") && (p->last_declaration.inside_struct = true)))
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
      if (p->last_declaration.is_typedef && p->typedef_matching.active && p->last_declaration.scope_depth == 0)
	{
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
	    return (-1);
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
      p->last_declaration.inside_union = punion;
      p->last_declaration.inside_struct = pstruct;
      return (1);
    }  
  return (0);
}

int			read_storage_class_specifier(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  const char		*str[] =
    {
     "typedef", "extern", "static", "auto", "register",
    };

  for (size_t j = 0; j < NBRCELL(str); ++j)
    if (bunny_read_text(code, i, str[j]))
      {
	(&p->last_declaration.is_typedef)[j] = true;
	return (1);
      }
  return (0);
}

int			read_declaration_specifiers(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  int			ret;
  int			cnt = 0;
  bool			once;
  int			prev_ptr;

  do
    {
      if ((ret = read_storage_class_specifier(p, code, i)) == -1) // Comprend typedef
	return (-1);
      cnt += ret;
    }
  while (ret == 1);
  p->last_declaration.nbr_pointer = prev_ptr = 0;
  do
    {
      once = false;
      // On veut un type
      if ((ret = read_type_specifier(p, code, i)) == -1)
	return (-1);
      once = (once || (ret == 1));
      // On veut un const ou autre du style
      if ((ret = read_type_qualifier(p, code, i)) == -1)
	return (-1);
      once = (once || (ret == 1));
      // Un pointeur?
      if ((ret = read_pointer(p, code, i)) == -1)
	return (-1);
      if (p->last_declaration.nbr_pointer != prev_ptr)
	if (!check_pointer_star_position(p, code, *i))
	  RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      prev_ptr = p->last_declaration.nbr_pointer;
      once = (once || (ret == 1));

      // On a eu un typedef au début
      // On a pas eu de type ni de qualifier - c'est peut etre un mot inconnu - un nouveau type
      if (once == 0 && p->last_declaration.is_typedef)
	{
	  int j = *i;

	  p->last_declaration.is_typedef = false;
	  read_whitespace(code, i);
	  gl_bunny_read_whitespace = NULL;
	  if (read_identifier(p, code, i, false) == 0)
	    {
	      // Si il n'y a pas de symbole...
	      // Ce qui peut signifier que le symbole du typedef a été mangé parceque le type
	      // était deja connu, a cause d'un typedef redondant.
	      gl_bunny_read_whitespace = read_whitespace;
	      return (-1);
	    }
	  if (check_style(p, "typedef", &p->last_declaration.symbol[0],
			  &p->typedef_style, &p->typedef_infix,
			  code, j) == false)
	    RETURN("Memory exhausted."); // LCOV_EXCL_LINE

	  if (p->typedef_matching.active)
	    {
	      char buffer[SYMBOL_SIZE + 1];

	      store_real_typename(p, buffer, &p->last_declaration.symbol[0], sizeof(buffer), 2);
	      if (strcmp(buffer, p->typedef_stack[p->typedef_stack_top]) != 0)
		{
		  if (add_warning
		      (p, IZ(p, &j), code, j, &p->typedef_matching.counter,
		       "Typedef name '%s' does not match the typedefed type name '%s'.",
		       buffer, p->typedef_stack[p->typedef_stack_top]
		       ) == false)
		    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
		}
	      p->typedef_stack_top -= 1;
	    }
	  int anonym = p->last_declaration.was_named ? -1 : 0;

	  gl_bunny_read_whitespace = read_whitespace;
	  if (p->last_declaration.was_defining)
	    {
	      p->new_type[p->last_new_type + anonym].size =
		p->last_declaration.cumulated_attribute_size;
	      add_new_type
		(p, p->last_declaration.symbol, p->last_declaration.cumulated_attribute_size);
	    }
	  else
	    add_new_type(p, p->last_declaration.symbol, p->last_declaration.last_type_size);

	}
      cnt += once ? 1 : 0;
    }
  while (once);
  if (p->last_declaration.is_typedef)
    RETURN("Incomplete typedef."); // LCOV_EXCL_LINE
  return (cnt >= 1 ? 1 : 0);
}

int			read_direct_abstract_declarator(t_parsing *p,
							const char *code,
							ssize_t	*i)
{
  bool			once;
  int			cnt = 0;

  do
    {
      once = false;
      if (bunny_read_text(code, i, "("))
	{
	  if (!check_parenthesis_space
	      (p, code, *i, '(',
	       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  int		ret;

	  p->last_declaration.inside_parameter = true;
	  if ((ret = read_abstract_declarator(p, code, i)) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  else if (ret == 0 && read_parameter_type_list(p, code, i) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  p->last_declaration.inside_parameter = false;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '( parameter list"); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i, ')',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, "["))
	{
	  if (read_constant_expression(p, code, i) == -1)
	    RETURN ("Problem encountered with constant expression after '['."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, "]"))
	    RETURN ("Missing ']' after '[constant'"); // LCOV_EXCL_LINE
	}
      cnt += once ? 1 : 0;
    }
  while (once);
  return (cnt >= 1 ? 1 : 0);
}

int			read_abstract_declarator(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			a;
  int			b;

  if ((a = read_pointer(p, code, i)) == -1)
    return (-1);
  if (p->last_declaration.nbr_pointer)
    if (!check_pointer_star_position(p, code, *i))
      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  if ((b = read_direct_abstract_declarator(p, code, i)) == -1)
    return (-1);
  return (a + b >= 1 ? 1 : 0);
}

int			read_parameter_declaration(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  if ((ret = read_declaration_specifiers(p, code, i)) != 1)
    return (ret);
  if ((ret = read_declarator(p, code, i)) == -1)
    return (ret);
  return (read_abstract_declarator(p, code, i));
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
	return (1);
      p->last_declaration.ptr_acc = 0;
      if ((ret = read_parameter_declaration(p, code, i)) == 1)
	return (ret);
      if (!check_last_parameter_is_reference(p, code, *i))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      cnt += 1;
      if (p->max_parameter.active && p->max_parameter.value < cnt && err == false)
	{
	  err = true;
	  if (!add_warning
	      (p, IZ(p, &start), code, start, &p->max_parameter.counter,
	       "Invalid amount of parameters. Maximum was %d.", p->max_parameter.value))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
    }
  while (bunny_read_text(code, i, ","));

  return (1);
}

int			read_parameter_type_list(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  if ((ret = read_parameter_list(p, code, i)) != 1)
    return (ret);
  if (bunny_read_text(code, i, ","))
    {
      if (check_no_space_before_space_after(p, code, *i - 1) == -1)
	RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      return (bunny_read_text(code, i, "...") ? 1 : -1);
    }
  return (1);
}

int			read_pointer(t_parsing			*p,
				     const char			*code,
				     ssize_t			*i)
{
  int			ret;

  if (!bunny_read_text(code, i, "*"))
    return (0);
  p->last_declaration.nbr_pointer += 1;
  p->last_declaration.ptr_acc += 1;
  p->last_declaration.last_type_size = sizeof(void*);
  if (read_type_qualifier(p, code, i) == -1)
    return (-1);
  if ((ret = read_pointer(p, code, i)) == -1)
    return (-1);
  return (1);
}

int			read_direct_declarator(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  int			j = *i;

  if (read_identifier(p, code, i, false) == 0) // symbole de variable ou symbole de fonction
    {
      if (bunny_read_text(code, i, "(")) // Y a t il des paramètres?
	{
	  if (!check_parenthesis_space(p, code, *i, '(',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  p->last_declaration.inside_parameter = true;
	  if (read_declarator(p, code, i) == -1)
	    return (-1);
	  p->last_declaration.inside_parameter = false;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(declaration'."); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i, ')',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  return (1);
	}
      return (0); // Il faut un symbole ou une parenthèse (qui finirait par contenir un symbole)...
    }

  // On a trouvé un symbole.
  if (p->last_declaration.inside_variable)
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
      else
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
  if (p->last_declaration.inside_parameter && p->parameter_name_alignment.value)
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
  
  bool			once;

  do
    {
      once = false;
      if (bunny_read_text(code, i, "["))
	{
	  once = true;
	  if (read_constant_expression(p, code, i) == -1)
	    RETURN ("Problem encountered with constant expression after '['."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, "]"))
	    RETURN ("Missing ']' after '[constant'."); // LCOV_EXCL_LINE
	}
      if (bunny_read_text(code, i, "("))
	{
	  int		ret;

	  once = true;
	  if (!check_parenthesis_space
	      (p, code, *i, '(',
	       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	  p->last_declaration.inside_parameter = true;
	  if ((ret = read_parameter_type_list(p, code, i)) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  else if (ret == 0)
	    if (read_identifier_list(p, code, i) == -1)
	      RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  p->last_declaration.inside_parameter = false;
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '( parameter list"); // LCOV_EXCL_LINE
	  if (!check_parenthesis_space(p, code, *i, ')',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
    }
  while (once);
  return (1);
}

int			read_declarator(t_parsing		*p,
					const char		*code,
					ssize_t			*i)
{
  p->last_declaration.nbr_pointer = 0;
  if (read_pointer(p, code, i) == -1)
    return (-1);
  if (p->last_declaration.nbr_pointer)
    {
      if (!check_pointer_star_position(p, code, *i))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
    }
  return (read_direct_declarator(p, code, i));
}

int			read_initializer_list(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  do
    if ((ret = read_initializer(p, code, i)) != 1)
      {
	if (cnt == 0 || ret == -1)
	  return (ret);
	RETURN ("Excessive ',' found in initializer list."); // LCOV_EXCL_LINE
      }
    else
      cnt += ret;
  while (bunny_read_text(code, i, ","));
  return (cnt > 0 ? 1 : 0);
}

int			read_initializer(t_parsing		*p,
					 const char		*code,
					 ssize_t		*i)
{
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
		return (ret);
	      RETURN("Excessive ',' found in initializer."); // LCOV_EXCL_LINE
	    }
	  else
	    cnt += 1;
	}
      while (bunny_read_text(code, i, ","));
      if (!bunny_read_text(code, i, "}"))
	RETURN("Missing '}' at after '{initializer'."); // LCOV_EXCL_LINE
      return (1);
    }
  return (read_assignment_expression(p, code, i));
}

int			read_init_declarator(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  int			ret;

  p->last_declaration.inside_variable = true;

  // On déclare une variable, globale ou locale
  if ((ret = read_declarator(p, code, i)) != 1)
    {
      p->last_declaration.inside_variable = false;
      return (ret);
    }
  p->last_declaration.inside_variable = false;
  if (!bunny_read_text(code, i, "="))
    return (1);
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
  return (1);
}

int			read_init_declarator_list(t_parsing	*p,
						  const char	*code,
						  ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  do
    {
      if (cnt > 0)
	if (check_no_space_before_space_after(p, code, *i) == -1)
	  RETURN("Memory exhausted."); // LCOV_EXCL_LINE
      if ((ret = read_init_declarator(p, code, i)) != 1)
	{
	  if (cnt == 0 || ret == -1)
	    return (ret);
	  RETURN("Excessive ',' found in declaration."); // LCOV_EXCL_LINE
	}
      else
	cnt += ret;
    }
  while (bunny_read_text(code, i, ","));
  if (cnt > 1 && p->single_instruction_per_line.value)
    if (!add_warning
	(p, IZ(p, i), code, *i, &p->single_instruction_per_line.counter,
	 "Only a single declaration is authorized per line."))
      RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  return (cnt > 0 ? 1 : 0);
}

int			read_gcc_attribute_list_node(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  if (read_identifier(p, code, i, true) != 1)
    return (0);
  if (bunny_read_text(code, i, "("))
    {
      if (!check_parenthesis_space
	  (p, code, *i, '(',
	   &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      int		unused;
      int		cnt = 0;

      do
	{
	  if (cnt > 0)
	    if (check_no_space_before_space_after(p, code, *i) == -1)
	      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
	  if (read_identifier(p, code, i, true) != 1)
	    if (bunny_read_integer(code, i, &unused) == false)
	      return (-1);
	  cnt += 1;
	}
      while (bunny_read_text(code, i, ","));
      if (!check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, ")") == false)
	RETURN("Missing ')' to close attribute parameter."); // LCOV_EXCL_LINE
    }
  return (1);
}

int			read_gcc_attribute(t_parsing		*p,
					   const char		*code,
					   ssize_t		*i)
{
  int			cnt = 0;

  while (bunny_read_text(code, i, "__attribute__"))
    {
      if (bunny_read_text(code, i, "((") == false)
	RETURN("\"((\" was expected after __attribute__."); // LCOV_EXCL_LINE
      if (!check_parenthesis_space(p, code, *i, '(',
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
	  if (!check_parenthesis_space(p, code, *i, ')',
				       &p->no_space_inside_parenthesis.counter))
	    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
	}
      if (comma_end)
	RETURN("Missing parameters for __attribute__ after ','"); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, "))") == false)
	RETURN("\"))\" was expected to close __attribute__."); // LCOV_EXCL_LINE
      if (!check_parenthesis_space
	  (p, code, *i, ')', &p->no_space_inside_parenthesis.counter))
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      cnt += 1;
    }
  return (cnt >= 1 ? 1 : 0);
}

int			read_declaration(t_parsing		*p,
					 const char		*code,
					 ssize_t		*i)
{
  int			ret;

  if (check_base_indentation(p, code, *i) == -1)
    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
  p->last_declaration.was_defining = false;
  if ((ret = read_declaration_specifiers(p, code, i)) != 1)
    return (ret);
  if (read_init_declarator_list(p, code, i) == -1)
    return (-1);
  if (read_gcc_attribute(p, code, i) == -1)
    return (-1);
  if (bunny_read_text(code, i, ";") == false)
    RETURN ("Missing ';' after declaration."); // LCOV_EXCL_LINE
  if (check_white_then_newline(p, code, *i, false) == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  return (1);
}

int			read_external_declaration(t_parsing	*p,
						  const char	*code,
						  ssize_t	*i)
{
  int			ret;
  ssize_t		j = *i;

  // Regardons si on declare une fonction.
  if ((ret = read_function_declaration(p, code, &j)) != 0)
    {
      // C'était bien une fonction... ou une erreur!
      if (ret != -1)
	*i = j;
      return (ret);
    }

  // Ca doit etre autre chose...
  return (read_declaration(p, code, i));
}

int			read_translation_unit(t_parsing		*p,
					      const char	*file,
					      const char	*code,
					      ssize_t		*i,
					      bool		verbose)
{
  int			cnt = 0;
  int			ret;

  p->file = file;
  p->last_declaration.indent_depth = 0;

  p->local_parameter_type_alignment = -1;
  p->local_parameter_name_alignment = -1;
  p->local_symbol_alignment = -1;
  p->global_parameter_name_alignment = -1;
  p->global_symbol_alignment = -1;

  
  gl_bunny_read_whitespace = read_whitespace;
  if (check_header(p, code) == false)
    ret = -1;
  else
    ret = 1;
  while (ret == 1 && code[*i])
    {
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
		printf("- %s\n", p->last_error_msg[--p->last_error_id]);
	    }
	  gl_bunny_read_whitespace = NULL;
	  return (-1);
	} // LCOV_EXCL_STOP
      else
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

	  cnt += ret;
	}
      read_whitespace(code, i);
    }

  gl_bunny_read_whitespace = NULL;

  for (t_criteria *c = p->criteria; c != &p->end[0]; ++c)
    {
      if (!c->active)
	continue ;
      if (c->value > c->counter)
	{
	  p->nbr_mistakes += 1;
	  p->nbr_error_points += c->pts;
	}
    }

  if (p->no_trailing_whitespace.value)
    if (check_trailing_whitespace(p, code) == -1)
      RETURN("Memory exhausted."); // LCOV_EXCL_LINE

  return (cnt >= 1 ? 1 : 0);
}

static void		fetchi(t_bunny_configuration		*e,
			       int				*i,
			       const char			*f,
			       int				def)
{
  if (!bunny_configuration_getf(e, i, f))
    *i = def;
}

static void		fetch_criteria(t_bunny_configuration	*cnf,
				       t_criteria		*crit,
				       const char		*field)
{
  crit->active = false;
  crit->value = 0;
  crit->pts = 1;
  crit->counter = 0;
  // Trois syntaxes alternatives:
  // [Champ Value = 3 Points = 2 ]
  // Ou:
  // Champ = 3, 2
  // Ou:
  // Champ = 3
  // ChampPts = 2
  if (bunny_configuration_getf(cnf, &crit->value, "%s.Value", field))
    {
      crit->active = true;
      bunny_configuration_getf(cnf, &crit->pts, "%s.Points", field);
    }
  else if (bunny_configuration_getf(cnf, &crit->pts, "%s[1]", field))
    {
      if (bunny_configuration_getf(cnf, &crit->value, "%s[0]", field))
	crit->active = true;
    }
  else if (bunny_configuration_getf(cnf, &crit->value, "%s", field))
    {
      bunny_configuration_getf(cnf, &crit->pts, "%sPts", field);
      crit->active = true;
    }
  crit->pts = abs(crit->pts);
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
  const char		*str;

  crit->active = false;
  crit->value[0] = '\0';
  crit->position = 0;
  crit->pts = 1;
  crit->counter = 0;
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
      crit->active = true;
      strxcpy(&crit->value[0], str, sizeof(crit->value) - 1, strlen(str));
      if (bunny_configuration_getf(cnf, &str, "%s.Position", field))
	{
	  if (strcmp(str, "Prefix") == 0)
	    crit->position = 0;
	  else if (strcmp(str, "Suffix") == 0)
	    crit->position = 1;
	  else
	    bunny_configuration_getf(cnf, &crit->position, "%s.Position", field);
	}
      bunny_configuration_getf(cnf, &crit->pts, "%s.Points", field);
      crit->pts = abs(crit->pts);
      return (true);
    }
  if (bunny_configuration_getf(cnf, &crit->pts, "%s[2]", field))
    {
      if (bunny_configuration_getf(cnf, &str, "%s[1]", field))
	{
	  if (strcmp(str, "Prefix") == 0)
	    crit->position = 0;
	  else if (strcmp(str, "Suffix") == 0)
	    crit->position = 1;
	  else
	    bunny_configuration_getf(cnf, &crit->position, "%s[1]", field);
	}
      if (bunny_configuration_getf(cnf, &str, "%s[0]", field))
	{
	  crit->active = true;
	  strxcpy(&crit->value[0], str, sizeof(crit->value) - 1, strlen(str));
	}
      crit->pts = abs(crit->pts);
      return (true);
    }
  if (bunny_configuration_getf(cnf, &str, "%s", field))
    {
      strxcpy(&crit->value[0], str, sizeof(crit->value) - 1, strlen(str));
      if (bunny_configuration_getf(cnf, &str, "%sPosition", field))
	{
	  if (strcmp(str, "Prefix") == 0)
	    crit->position = 0;
	  else if (strcmp(str, "Suffix") == 0)
	    crit->position = 1;
	  else
	    bunny_configuration_getf(cnf, &crit->position, "%sPosition", field);
	}
      bunny_configuration_getf(cnf, &crit->pts, "%sPts", field);
      crit->active = true;
      crit->pts = abs(crit->pts);
      return (true);
    }
  return (false);
}

int			check_header_file(t_parsing		*p,
					  const char		*code)
{
  // Code ici doit etre le fichier nature, et non pas le document préprocessé
  if (check_header(p, code) == false)
    RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
  return (true);
}

int			check_all_lines_width(t_parsing		*p,
					      const char	*code)
{
  int			i;
  int			j;

  i = 0;
  j = 0;
  while (code[j] != '\0')
    {
      while (code[j] != '\0' && code[j] != '\n')
	j = j + 1;
      if (check_line_width(p, code, i, j) == -1)
	RETURN ("Memory exhausted."); // LCOV_EXCL_LINE
      i = (j += (code[j] != '\0' ? 1 : 0));
    }
  return (1);
}

void			load_norm_configuration(t_parsing	*p,
						t_bunny_configuration *e)
{
  memset(p, 0, sizeof(*p));

  p->last_error_id = -1;
  p->configuration = e;

  p->nbr_mistakes = 0; // Le nombre d'erreur faites
  p->nbr_error_points = 0; // Le nombre de points d'erreur accumulés
  fetchi(e, &p->maximum_error_points, "Tolerance", -1);

  p->criteria = &p->function_per_file;
  fetch_criteria(e, &p->function_per_file, "FunctionPerFile");
  fetch_criteria(e, &p->non_static_function_per_file, "NonStaticFunctionPerFile");

  {
    t_criteria		style;
    t_string_criteria	infix;

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

  fetch_criteria(e, &p->local_variable_style, "LocalVariableNameStyle");
  fetch_string_criteria(e, &p->local_variable_infix, "LocalVariableNameInfix");

  fetch_criteria(e, &p->global_variable_style, "GlobalVariableNameStyle");
  fetch_string_criteria(e, &p->global_variable_infix, "GlobalVariableNameInfix");

  fetch_criteria(e, &p->struct_style, "StructNameStyle");
  fetch_string_criteria(e, &p->struct_infix, "StructNameInfix");

  fetch_criteria(e, &p->enum_style, "EnumNameStyle");
  fetch_string_criteria(e, &p->enum_infix, "EnumNameInfix");
  fetch_criteria(e, &p->enum_constant_style, "EnumConstantStyle");
  fetch_string_criteria(e, &p->enum_constant_infix, "EnumConstantInfix");

  fetch_criteria(e, &p->union_style, "UnionNameStyle");
  fetch_string_criteria(e, &p->union_infix, "UnionNameInfix");

  fetch_criteria(e, &p->struct_attribute_style, "StructAttributeNameStyle");
  fetch_string_criteria(e, &p->struct_attribute_infix, "StructAttributeNameInfix");
  fetch_criteria(e, &p->union_attribute_style, "UnionAttributeNameStyle");
  fetch_string_criteria(e, &p->union_attribute_infix, "UnionAttributeNameInfix");

  fetch_criteria(e, &p->function_pointer_attribute_style, "FunctionPointerAttributeStyle");
  fetch_string_criteria(e, &p->function_pointer_attribute_infix, "FunctionPointerAttributeInfix");

  fetch_criteria(e, &p->function_pointer_type_style, "FunctionPointerTypeStyle");
  fetch_string_criteria(e, &p->function_pointer_type_infix, "FunctionPointerTypeInfix");

  fetch_criteria(e, &p->typedef_style, "TypedefNameStyle");
  fetch_string_criteria(e, &p->typedef_infix, "TypedefNameInfix");
  fetch_criteria(e, &p->typedef_matching, "TypedefMatching");

  fetch_criteria(e, &p->local_variable_inline_init_forbidden, "LocalVariableInlineInitForbidden");

  fetch_criteria(e, &p->function_matching_path, "FunctionMatchingPath");

  fetch_criteria(e, &p->indent_style, "IndentationStyle");
  fetch_criteria(e, &p->base_indent, "IndentationSize");
  fetch_criteria(e, &p->tab_or_space, "IndentationToken");
  fetch_criteria(e, &p->declaration_statement_separator, "DeclarationStatementSeparator");
  fetch_criteria(e, &p->no_empty_line_in_function, "NoEmptyLineInFunction");
  fetch_criteria(e, &p->no_trailing_whitespace, "NoTrailingWhitespace");
  fetch_criteria(e, &p->single_instruction_per_line, "SingleInstructionPerLine");
  fetch_criteria(e, &p->max_column_width, "MaximumLineWidth");
  fetch_criteria(e, &p->max_function_length, "MaximumFunctionLength");
  fetch_criteria(e, &p->max_parameter, "MaximumFunctionParameter");
  fetch_criteria(e, &p->always_braces, "AlwaysBraces");
  fetch_criteria(e, &p->avoid_braces, "AvoidBracesForSingleLine");
  fetch_criteria(e, &p->space_after_statement, "SpaceAfterStatement");
  fetch_criteria(e, &p->space_around_binary_operator, "SpaceAroundBinaryOperator");
  fetch_criteria(e, &p->only_by_reference, "OnlyByReference");
  fetch_criteria(e, &p->no_space_inside_parenthesis, "NoSpaceInsideParenthesis");
  if (fetch_string_criteria(e, &p->header, "Header"))
    {
      const char	*str;
      
      bunny_configuration_getf(e, &str, "Header.Value")
	|| bunny_configuration_getf(e, &str, "Header[1]")
	|| bunny_configuration_getf(e, &str, "Header");
      strxcpy(&p->header_data[0], str, sizeof(p->header_data), strlen(str));
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

  fetch_criteria(e, &p->for_forbidden, "ForForbidden");
  fetch_criteria(e, &p->while_forbidden, "WhileForbidden");
  fetch_criteria(e, &p->do_while_forbidden, "DoWhileForbidden");
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
  fetch_criteria(e, &p->inline_mod_forbidden, "InlineModForbidden");
  fetch_criteria(e, &p->ternary_forbidden, "TernaryForbidden");
}

char		*load_c_file(const char				*file,
			     t_bunny_configuration		*exe,
			     bool				preprocessed)
{
  char		buffer[512];
  const char	*cmd;
  int		len;

  if (preprocessed)
    {
      if (!bunny_configuration_getf(exe, &cmd, "PrecompilationCommand"))
	cmd = "gcc -E -CC -I./ -I./include/ -I/usr/local/include/ %s";
    }
  else
    cmd = "cat %s";
  snprintf(&buffer[0], sizeof(buffer), cmd, file);
  cmd = &buffer[0];
  len = sizeof(bunny_big_buffer);
  if (tcpopen("c norm", cmd, &bunny_big_buffer[0], &len, NULL, 0) != 0)
    return (NULL);
  return (&bunny_big_buffer[0]);
}
