/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
** Implémenté à partir de la grammaire du C ANSI:
** https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
*/

#include		<ctype.h>
#include		"crawler.h"

char			*strcasestr(const char			*haystack,
				    const char			*needle);

#define			RETURN(a) return (p->last_error_msg[++p->last_error_id] = (a " (" STRINGIFY(__LINE__) ")" ), -1)
#define			MSG(a) p->last_error_msg[++p->last_error_id] = (a " (" STRINGIFY(__LINE__) ")" )


static const char	*gl_first_char = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN_";
static const char	*gl_second_char = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN_0123456789";

void			reset_last_declaration(t_parsing	*p)
{
  memset(&p->last_declaration, 0, sizeof(p->last_declaration));
}

static bool		add_warning(t_parsing			*p,
				    const char			*code,
				    int				pos,
				    const char			*fmt,
				    ...)
{
  char			buf[512];
  va_list		lst;
  int			end;

  va_start(lst, fmt);
  end = vsnprintf(&buf[0], sizeof(buf), fmt, lst);
  snprintf(&buf[end], sizeof(buf) - end, " (%s, line %d)", p->file, bunny_which_line(code, pos));
  if ((p->last_error_msg[++p->last_error_id] = bunny_strdup(&buf[0])) == NULL)
    { // LCOV_EXCL_START
      p->last_error_id -= 1;
      return (false);
    } // LCOV_EXCL_STOP
  return (true);
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

bool			read_whitespace(const char		*code,
					ssize_t			*i)
{
  bool			ret;

  gl_bunny_read_whitespace = NULL;
  do
    {
      ret = false;
      if (bunny_read_char(code, i, " \t\r\n"))
	ret = true;
      if (bunny_read_text(code, i, "/*"))
	{
	  ret = true;
	  while (code[*i] && !bunny_read_text(code, i, "*/"))
	    *i += 1;
	  if (code[*i] == '\0')
	    goto BadEnd;
	}
      if (bunny_read_text(code, i, "//"))
	{
	  ret = true;
	  while (code[*i] && !bunny_read_text(code, i, "\n"))
	    *i += 1;
	  if (!code[*i])
	    goto GoodEnd;
	}
      if (bunny_read_text(code, i, "#")) // Au cas ou il reste du preprocesseur
	{
	  ret = true;
	  do
	    {
	      while (code[*i] && !bunny_read_text(code, i, "\n"))
		*i += 1;
	      if (!code[*i])
		goto GoodEnd;
	    }
	  while (code[*i - 1] == '\\');
	}
    }
  while (ret);
 GoodEnd:
  gl_bunny_read_whitespace = read_whitespace;
  return (true);
 BadEnd:
  gl_bunny_read_whitespace = read_whitespace;
  return (false);
}

int			read_identifier(t_parsing		*p,
					const char		*code,
					ssize_t			*i,
					bool			kwx)
{
  const char		*kw[] =
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
    p->last_declaration.symbol[0] = '\0';
  return (1);
}

int			read_identifier_list(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  int			cnt = 0;
  int			ret;

  do
    if ((ret = read_identifier(p, code, i, false)) != 1)
      {
	if (cnt == 0 || ret == -1)
	  return (ret);
	RETURN("Excessive ',' found in declaration."); // LCOV_EXCL_LINE
      }
    else
      cnt += 1;
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
      *i = j;
      return (read_statement(p, code, i));
    }
  if (bunny_read_text(code, i, "case"))
    {
      if (read_constant_expression(p, code, i) != 1)
	RETURN ("Missing symbol after 'case'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ":"))
	RETURN ("Missing token ':' after symbol used by 'case'."); // LCOV_EXCL_LINE
      return (read_statement(p, code, i));
    }
  if (bunny_read_text(code, i, "default"))
    {
      if (!bunny_read_text(code, i, ":"))
	RETURN ("Missing token ':' after 'default'."); // LCOV_EXCL_LINE
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
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'if'."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing condition after 'if ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'if (condition'."); // LCOV_EXCL_LINE
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'if (condition)'."); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, "else"))
	if (read_statement(p, code, i) != 1)
	  RETURN ("Missing statement after 'else'."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "switch"))
    {
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'switch'."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing expression after 'switch ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'switch (expression'."); // LCOV_EXCL_LINE
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
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'while'."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing condition after 'while ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'while(condition'."); // LCOV_EXCL_LINE
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'while (condition)'."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "do"))
    {
      if (read_statement(p, code, i) != 1)
	RETURN ("Missing statement after 'do'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "while"))
	RETURN ("Missing 'while' after 'do statement'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after after 'do statement while'."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) != 1)
	RETURN ("Missing condition after 'do statement while ('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'do statement while (condition'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'do statement while (condition)'."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "for"))
    {
      if (!bunny_read_text(code, i, "("))
	RETURN ("Missing '(' after 'for'."); // LCOV_EXCL_LINE
      if (read_expression_statement(p, code, i) != 1)
	RETURN ("Missing initialization after 'for ('."); // LCOV_EXCL_LINE
      if (read_expression_statement(p, code, i) != 1)
	RETURN ("Missing condition after 'for (initialization;'."); // LCOV_EXCL_LINE
      if (read_expression(p, code, i) == -1)
	RETURN ("Invalid increment after 'for (initialization; condition;'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after 'for (initialization; condition; increment'."); // LCOV_EXCL_LINE
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
      if (!read_identifier(p, code, i, false))
	RETURN ("Missing symbol after 'goto'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'goto symbol'."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "continue"))
    {
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'continue'."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "break"))
    {
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'break'."); // LCOV_EXCL_LINE
      return (1);
    }
  if (bunny_read_text(code, i, "return"))
    {
      if (bunny_read_text(code, i, ";"))
	return (1);
      if (read_expression(p, code, i) == -1)
	RETURN ("Missing expression or ';' after 'return'."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, i, ";"))
	RETURN ("Missing ';' after 'return expression'."); // LCOV_EXCL_LINE
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
  return (ret + sc >= 1 ? 1 : 0);
}

int			read_statement(t_parsing		*p,
				       const char		*code,
				       ssize_t			*i)
{
  int			ret;

  if ((ret = read_labeled_statement(p, code, i)) != 0)
    return (ret);
  if ((ret = read_compound_statement(p, code, i)) != 0)
    return (ret);
  if ((ret = read_expression_statement(p, code, i)) != 0)
    return (ret);
  if ((ret = read_selection_statement(p, code, i)) != 0)
    return (ret);
  if ((ret = read_iteration_statement(p, code, i)) != 0)
    return (ret);
  if ((ret = read_jump_statement(p, code, i)) != 0)
    return (ret);
  return (0);
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
  if (bunny_read_text(code, i, "{") == false)
    return (0);
  if (read_declaration_list(p, code, i) == -1)
    return (-1);
  if (read_statement_list(p, code, i) == -1)
    return (-1);
  if (bunny_read_text(code, i, "}") == false)
    RETURN ("Missing '}' after '{ values'."); // LCOV_EXCL_LINE
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
  ssize_t		k = *i; // Au cas ou cela ne soit pas une declaration de fonction mais de type.
  ssize_t		j;
  int			last_new_type = p->last_new_type;
  int			ret;

  memcpy(&save[0], p->criteria, sizeof(save));
  reset_last_declaration(p);
  // Le type de retour
  if (read_declaration_specifiers(p, code, i) == -1)
    return (-1);

  //////////////////////
  // Le nom de fonction
  j = *i;
  if (read_declarator(p, code, i) == -1)
    return (-1);
  // On verifie donc le nom si c'est une fonction non statique
  if (!p->last_declaration.is_static)
    if (check_style(p, "function", &p->last_declaration.symbol[0], &p->function_style, &p->function_infix, code, j) == false)
      RETURN("Memory exhausted."); // LCOV_EXCL_LINE
  
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
	}
      p->last_declaration.inside_function = false;
      return (ret);
    }
  p->last_declaration.inside_function = false;

  // On revient en arrière, ca n'était pas une declaration de fonction.
  *i = k;
  // On fait revenir en arrière egalement le compte d'erreur du coup, ca on va les recompter
  memcpy(p->criteria, &save[0], sizeof(save));
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
  if (bunny_read_integer(code, i, &val))
    return (1);
  if (bunny_read_double(code, i, &val2))
    return (1);
  ssize_t		j = *i;

  if (bunny_read_text(code, &j, "("))
    {
      if (read_expression(p, code, &j) != 1)
	RETURN ("Problem encountered in expression after '('."); // LCOV_EXCL_LINE
      if (!bunny_read_text(code, &j, ")"))
	RETURN ("Missing ')' after '(expression'."); // LCOV_EXCL_LINE
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
    if ((ret = read_assignment_expression(p, code, i)) != 1)
      {
	if (cnt == 0 || ret == -1)
	  return (ret);
	RETURN ("Excessive ',' found in argument list."); // LCOV_EXCL_LINE
      }
    else
      cnt += ret;
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
	  if (read_argument_expression_list(p, code, i) == -1)
	    RETURN ("Problem encountered with argument list after '('."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(argument'."); // LCOV_EXCL_LINE
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
      if (read_unary_expression(p, code, i) != 1)
	{
	  if (bunny_read_text(code, i, "("))
	    {
	      if (read_type_name(p, code, i) != 1)
		RETURN ("Problem encountered with type name after 'sizeof('."); // LCOV_EXCL_LINE
	      if (!bunny_read_text(code, i, ")"))
		RETURN ("Missing ')' after 'sizeof(type name'."); // LCOV_EXCL_LINE
	      return (1);
	    }
	  RETURN ("Unknown sequence after 'sizeof'."); // LCOV_EXCL_LINE
	}
      return (1);
    }
  if (bunny_read_text(code, i, "++"))
    return (read_unary_expression(p, code, i));
  if (bunny_read_text(code, i, "--"))
    return (read_unary_expression(p, code, i));
  if (read_unary_operator(p, code, i) == 1)
    return (read_cast_expression(p, code, i));
  return (read_postfix_expression(p, code, i));
}

int			read_cast_expression(t_parsing		*p,
					     const char		*code,
					     ssize_t		*i)
{
  ssize_t		j = *i;

  if (bunny_read_text(code, &j, "(") && read_type_name(p, code, &j) == 1)
    {
      *i = j;
      if (!bunny_read_text(code, i, ")"))
	RETURN ("Missing ')' after '(typename'."); // LCOV_EXCL_LINE
      return (read_cast_expression(p, code, i));
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
  if (bunny_read_text(code, i, "*") || bunny_read_text(code, i, "/") || bunny_read_text(code, i, "%"))
    if (read_multiplicative_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '*', '/' or '%'."); // LCOV_EXCL_LINE
  return (1);
}

int			read_additive_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  if ((ret = read_multiplicative_expression(p, code, i)) != 1)
    return (ret);
  if (bunny_read_text(code, i, "+") || bunny_read_text(code, i, "-"))
    if (read_additive_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '+' or '-'."); // LCOV_EXCL_LINE
  return (1);
}

int			read_shift_expression(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i)
{
  int			ret;

  if ((ret = read_additive_expression(p, code, i)) != 1)
    return (ret);
  if (bunny_read_text(code, i, "<<") || bunny_read_text(code, i, ">>"))
    if (read_shift_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '<<' or '>>'."); // LCOV_EXCL_LINE
  return (1);
}

int			read_relational_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  int			ret;

  if ((ret = read_shift_expression(p, code, i)) != 1)
    return (ret);
  if (bunny_read_text(code, i, "<=")
      || bunny_read_text(code, i, ">=")
      || (!bunny_check_text(code, i, ">>") && bunny_read_text(code, i, ">"))
      || (!bunny_check_text(code, i, "<<") && bunny_read_text(code, i, "<"))
      )
    if (read_relational_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '<=', '>=', '<' or '>'."); // LCOV_EXCL_LINE
  return (1);
}

int			read_equality_expression(t_parsing	*p,
						 const char	*code,
						 ssize_t	*i)
{
  int			ret;

  if ((ret = read_relational_expression(p, code, i)) != 1)
    return (ret);
  if (bunny_read_text(code, i, "==") || bunny_read_text(code, i, "!="))
    if (read_equality_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '==' or '!='."); // LCOV_EXCL_LINE
  return (1);
}

int			read_and_expression(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  int			ret;

  if ((ret = read_equality_expression(p, code, i)) != 1)
    return (ret);
  if (!bunny_check_text(code, i, "&&") && bunny_read_text(code, i, "&"))
    if (read_and_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '&'."); // LCOV_EXCL_LINE
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
    if (read_exclusive_or_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '^'."); // LCOV_EXCL_LINE
  return (1);
}

int			read_inclusive_or_expression(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			ret;

  if ((ret = read_exclusive_or_expression(p, code, i)) != 1)
    return (ret);
  if (!bunny_check_text(code, i, "||") && bunny_read_text(code, i, "|"))
    if (read_inclusive_or_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '|'."); // LCOV_EXCL_LINE
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
    if (read_logical_and_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '&&'."); // LCOV_EXCL_LINE
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
    if (read_logical_or_expression(p, code, i) != 1)
      RETURN ("Problem encountered with expression after '||'."); // LCOV_EXCL_LINE
  return (1);
}

int			read_assignment_expression(t_parsing	*p,
						   const char	*code,
						   ssize_t	*i)
{
  ssize_t		j = *i;

  if (read_unary_expression(p, code, &j) == 1 &&
      ((!bunny_check_text(code, &j, "==") && bunny_read_text(code, &j, "="))
       || bunny_read_text(code, &j, "+=")
       || bunny_read_text(code, &j, "-=")
       || bunny_read_text(code, &j, "*=")
       || bunny_read_text(code, &j, "/=")
       || bunny_read_text(code, &j, "%=")
       || bunny_read_text(code, &j, "<<=")
       || bunny_read_text(code, &j, ">>=")
       || bunny_read_text(code, &j, "|=")
       || bunny_read_text(code, &j, "&=")
       ))
    {
      *i = j;
      if (read_assignment_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after assignment."); // LCOV_EXCL_LINE
      return (1);
    }
  return (read_logical_or_expression(p, code, i));
}

int			read_expression(t_parsing		*p,
					const char		*code,
					ssize_t			*i)
{
  int		ret;

  if ((ret = read_assignment_expression(p, code, i)) == -1)
    return (-1);
  if (bunny_read_text(code, i, ","))
    if ((ret = read_expression(p, code, i)) == 0)
      RETURN ("Excessive ',' found in expression."); // LCOV_EXCL_LINE
  return (ret);
}

int			read_conditional_expression(t_parsing	*p,
						    const char	*code,
						    ssize_t	*i)
{
  if (read_logical_or_expression(p, code, i) == -1)
    return (-1);
  if (bunny_read_text(code, i, "?"))
    {
      if (read_expression(p, code, i) != 1)
	RETURN ("Problem encountered with expression after 'condition ?'."); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, ":") == false)
	RETURN ("Missing ':' after 'condition ? case1'."); // LCOV_EXCL_LINE
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
  return (bunny_read_text(code, i, "const") || bunny_read_text(code, i, "volatile"));
}

int			read_struct_declarator(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  int			ret = *i;

  p->last_declaration.symbol[0] = '\0';
  // On va lire maintenant le nom de l'attribut d'union ou de structure
  if ((ret = read_declarator(p, code, i)) == -1)
    RETURN ("Problem encountered with symbol after 'type' in struct."); // LCOV_EXCL_LINE
  //// A VOIR!!!!
  if (p->last_declaration.inside_struct)
    ret = check_style(p, "struct attribute", &p->last_declaration.symbol[0],
		      &p->struct_attribute_style, &p->struct_attribute_infix,
		      code, ret);
  else if (p->last_declaration.inside_union)
    ret = check_style(p, "union attribute", &p->last_declaration.symbol[0],
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
    if ((ret = read_struct_declarator(p, code, i)) != 1)
      {
	if (cnt == 0 || ret == -1)
	  return (ret);
	RETURN ("Excessive ',' in declaration."); // LCOV_EXCL_LINE
      }
    else
      cnt += ret;
  while (bunny_read_text(code, i, ","));
  return (cnt > 0 ? 1 : 0);
}

int			read_struct_declaration(t_parsing	*p,
						const char	*code,
						ssize_t		*i)
{
  int			a;
  int			b;

  if ((a = read_specifier_qualifier_list(p, code, i)) != 1)
    return (a);
  if ((b = read_struct_declarator_list(p, code, i)) == -1)
    RETURN ("Problem encountered with attribute name after type definition in struct."); // LCOV_EXCL_LINE
  if ((a = a || b) && bunny_read_text(code, i, ";") == false)
    RETURN ("Missing ';' after attribute definition in struct."); // LCOV_EXCL_LINE
  return (a ? 1 : 0);
}

int			read_struct_declaration_list(t_parsing	*p,
						     const char	*code,
						     ssize_t	*i)
{
  int			cnt = 0;
  int			ret;

  do
    if ((ret = read_struct_declaration(p, code, i)) == -1)
      return (-1);
    else
      cnt += ret;
  while (ret == 1);
  return (cnt);
}

bool			read_keyword(const char			*code,
				     ssize_t			*i,
				     const char			*symbol,
				     const char			*symchars)
{
  ssize_t		j = *i;

  read_whitespace(code, i);
  gl_bunny_read_whitespace = NULL;
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
  *i = j;
  gl_bunny_read_whitespace = read_whitespace;
  return (true);
}

int			store_real_typename(t_parsing		*p,
					    char		*target,
					    const char		*symbol,
					    int			len,
					    int			typ)
{
  int			spoint = 0;
  int			flen = strlen(symbol);
  
  if (typ == 0 && p->struct_infix.active)
    {
      if (p->struct_infix.position == 0)
	spoint = strlen(p->struct_infix.value);
      else
	flen -= strlen(p->struct_infix.value);
    }
  else if (typ == 1 && p->union_infix.active)
    {
      if (p->union_infix.position == 0)
	spoint = strlen(p->union_infix.value);
      else
	flen -= strlen(p->union_infix.value);
    }
  else if (typ == 2 && p->typedef_infix.active)
    {
      if (p->typedef_infix.position == 0)
	spoint = strlen(p->typedef_infix.value);
      else
	flen -= strlen(p->typedef_infix.value);
    }
  else if (typ == 3 && p->enum_infix.active) // enum
    {
      if (p->enum_infix.position == 0)
	spoint = strlen(p->enum_infix.value);
      else
	flen -= strlen(p->enum_infix.value);
    }
  strncpy(target, &symbol[spoint], flen > len ? len : flen);
  return (1);
}

int			read_type_specifier(t_parsing		*p,
					    const char		*code,
					    ssize_t		*i)
{
  const char		*str[] =
    {
     "void", "char", "short", "int", "long", "float", "double", "signed", "unsigned"
     // , "__int8_t", "__int16_t", "__int32_t", "__int64_t"
    };

  // Standard types
  for (size_t j = 0; j < NBRCELL(str); ++j)
    if (read_keyword(code, i, str[j], gl_second_char))
      return (1);
  // Custom types
  for (size_t j = 0; j < p->last_new_type; ++j)
    if (read_keyword(code, i, &p->new_type[j][0], gl_second_char))
      return (1);

  if (bunny_read_text(code, i, "enum"))
    {
      int		j = *i;

      read_identifier(p, code, i, false);
      // On enregistre le symbole pour pouvoir le comparer avec le typedef plus tard
      if (p->last_declaration.is_typedef)
	{
	  store_real_typename
	    (p, &p->typedef_stack[p->typedef_stack_top++][0],
	     &p->last_declaration.symbol[0],
	     sizeof(p->typedef_stack[0]), 3);
	}
      if (check_style(p, "enum", &p->last_declaration.symbol[0],
		      &p->enum_style, &p->enum_infix, code, j) == false)
	RETURN("Memory exhausted"); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, "{"))
	{
	  do
	    {
	      j = *i;
	      read_identifier(p, code, i, false);
	      if (check_style(p, "enum constant", &p->last_declaration.symbol[0],
			      &p->enum_constant_style, &p->enum_constant_infix, code, j) == false)
		RETURN("Memory exhausted"); // LCOV_EXCL_LINE
	      if (bunny_read_text(code, i, "="))
		if (read_constant_expression(p, code, i) != 1)
		  RETURN("Missing value after 'enum enum_symbol { symbol ='."); // LCOV_EXCL_LINE
	    }
	  while (bunny_read_text(code, i, ","));
	  if (!bunny_read_text(code, i, "}"))
	    RETURN ("Missing '}' after 'enum symbol { constants'."); // LCOV_EXCL_LINE
	}
      return (1);
    }
  bool punion = p->last_declaration.inside_union;
  bool pstruct = p->last_declaration.inside_struct;
  
  if ((bunny_read_text(code, i, "union") && (p->last_declaration.inside_union = true))
      || (bunny_read_text(code, i, "struct") && (p->last_declaration.inside_struct = true)))
    {
      bool		ret = true;
      int		j = *i;
      
      read_identifier(p, code, i, false); // optionnel
      // On enregistre le symbole pour pouvoir le comparer avec le typedef plus tard
      if (p->last_declaration.is_typedef && p->typedef_matching.active)
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
	  if (read_struct_declaration_list(p, code, i) == -1)
	    return (-1);
	  if (!bunny_read_text(code, i, "}"))
	    RETURN ("Missing '}' after 'struct/union symbol { attributes'."); // LCOV_EXCL_LINE
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

  do
    {
      if ((ret = read_storage_class_specifier(p, code, i)) == -1) // Comprend typedef
	return (-1);
      cnt += ret;
    }
  while (ret == 1);
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
      once = (once || (ret == 1));

      // On a eu un typedef au début
      // On a pas eu de type ni de qualifier - c'est peut etre un mot inconnu - un nouveau type
      if (once == 0 && p->last_declaration.is_typedef)
	{
	  const char *bef;
	  int j = *i;

	  p->last_declaration.is_typedef = false;
	  bef = &code[*i];
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
		      (p, code, j,
		       "Typedef name '%s' does not match the typedefed type name '%s'.",
		       buffer, p->typedef_stack[p->typedef_stack_top]
		       ) == false)
		    RETURN("Memory exhausted."); // LCOV_EXCL_LINE
		  p->typedef_matching.counter += 1;
		}
	      p->typedef_stack_top -= 1;
	    }
	  
	  gl_bunny_read_whitespace = read_whitespace;
	  memcpy(&p->new_type[p->last_new_type][0], bef, &code[*i] - bef);
	  p->new_type[p->last_new_type][&code[*i] - bef] = '\0';
	  p->last_new_type += 1;
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
	  int		ret;

	  if ((ret = read_abstract_declarator(p, code, i)) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  else if (ret == 0 && read_parameter_type_list(p, code, i) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '( parameter list"); // LCOV_EXCL_LINE
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
  ssize_t		j;
  int			ret;
  do
    {
      j = *i;
      if (bunny_read_text(code, &j, "..."))
	{
	  *i -= 1;
	  return (1);
	}
      if ((ret = read_parameter_declaration(p, code, i)) == 1)
	return (ret);
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
    return (bunny_read_text(code, i, "...") ? 1 : -1);
  return (1);
}

int			read_pointer(t_parsing			*p,
				     const char			*code,
				     ssize_t			*i)
{
  int			ret;

  if (!bunny_read_text(code, i, "*"))
    return (0);
  if (read_type_qualifier(p, code, i) == -1)
    return (-1);
  if ((ret = read_pointer(p, code, i)) == -1)
    return (ret);
  return (1);
}

int			read_direct_declarator(t_parsing	*p,
					       const char	*code,
					       ssize_t		*i)
{
  int			j = *i;

  if (read_identifier(p, code, i, false) == 0) // symbole de variable ou symbole de fonction
    {
      if (bunny_read_text(code, i, "("))
	{
	  if (read_declarator(p, code, i) == -1)
	    return (-1);
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '(declaration'."); // LCOV_EXCL_LINE
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
	  check_style(p, "local variable", &p->last_declaration.symbol[0],
		      &p->local_variable_style,
		      &p->local_variable_infix,
		      code, j);
	}
      // C'est une globale
      else
	{
	  check_style(p, "global variable", &p->last_declaration.symbol[0],
		      &p->global_variable_style,
		      &p->global_variable_infix,
		      code, j);
	}
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
	  once = true;
	  if (read_identifier_list(p, code, i) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  else if (read_parameter_type_list(p, code, i) == -1)
	    RETURN ("Problem encountered with parameter declaration after '('."); // LCOV_EXCL_LINE
	  if (!bunny_read_text(code, i, ")"))
	    RETURN ("Missing ')' after '( parameter list"); // LCOV_EXCL_LINE
	}
    }
  while (once);
  return (1);
}

int			read_declarator(t_parsing		*p,
					const char		*code,
					ssize_t			*i)
{
  if (read_pointer(p, code, i) == -1)
    return (-1);
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
	if ((ret = read_initializer_list(p, code, i)) != 1)
	  {
	    if (cnt == 0 || ret == -1)
	      return (ret);
	    RETURN("Excessive ',' found in initializer."); // LCOV_EXCL_LINE
	  }
	else
	  cnt += 1;
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
      p->local_variable_inline_init_forbidden.counter += 1;
      if (!add_warning(p, code, *i, "Forbidden declaration/assignation of variable."))
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
    if ((ret = read_init_declarator(p, code, i)) != 1)
      {
	if (cnt == 0 || ret == -1)
	  return (ret);
	RETURN("Excessive ',' found in declaration."); // LCOV_EXCL_LINE
      }
    else
      cnt += ret;
  while (bunny_read_text(code, i, ","));
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
      int		unused;

      do
	if (read_identifier(p, code, i, true) != 1)
	  if (bunny_read_integer(code, i, &unused) == false)
	    return (-1);
      while (bunny_read_text(code, i, ","));
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
      bool		comma_end = false;

      while (read_gcc_attribute_list_node(p, code, i))
	{
	  comma_end = false;
	  if (bunny_check_text(code, i, "))") == false)
	    {
	      if (bunny_read_text(code, i, ",") == false)
		RETURN("',' was expected to separate __attribute__ parameters.");
	      comma_end = true;
	    }
	}
      if (comma_end)
	RETURN("Missing parameters for __attribute__ after ','"); // LCOV_EXCL_LINE
      if (bunny_read_text(code, i, "))") == false)
	RETURN("\"))\" was expected to close __attribute__.");
      cnt += 1;
    }
  return (cnt >= 1 ? 1 : 0);
}

int			read_declaration(t_parsing		*p,
					 const char		*code,
					 ssize_t		*i)
{
  int			ret;

  if ((ret = read_declaration_specifiers(p, code, i)) != 1)
    return (ret);
  if (read_init_declarator_list(p, code, i) == -1)
    return (-1);
  if (read_gcc_attribute(p, code, i) == -1)
    return (-1);
  if (bunny_read_text(code, i, ";") == false)
    RETURN ("Missing ';' after declaration."); // LCOV_EXCL_LINE
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
      // C'était bien une fonction
      *i = j;
      return (ret);
    }

  // Ca doit etre autre chose...
  return (read_declaration(p, code, i));
}

int			read_translation_unit(t_parsing		*p,
					      const char	*code,
					      ssize_t		*i,
					      bool		verbose)
{
  int			cnt = 0;
  int			ret;

  gl_bunny_read_whitespace = read_whitespace;
  do
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
	      while (p->last_error_id >= 0)
		printf("- %s\n", p->last_error_msg[--p->last_error_id]);
	    }
	  gl_bunny_read_whitespace = NULL;
	  return (-1);
	} // LCOV_EXCL_STOP
      else
	cnt += ret;
      read_whitespace(code, i);
    }
  while (ret == 1 && code[*i]);

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

static void		fetch_string_criteria(t_bunny_configuration	*cnf,
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
    }
  else if (bunny_configuration_getf(cnf, &crit->pts, "%s[2]", field))
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
    }
  else if (bunny_configuration_getf(cnf, &str, "%s", field))
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
    }
  crit->pts = abs(crit->pts);
}

void			load_norm_configuration(t_parsing	*p,
						const char	*file,
						t_bunny_configuration *e)
{
  memset(p, 0, sizeof(*p));

  p->file = file;
  p->last_error_id = -1;
  
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
  fetch_criteria(e, &p->typedef_matching, "TypedefMatching"); // A FAIRE

  fetch_criteria(e, &p->local_variable_inline_init_forbidden, "LocalVariableInlineInitForbidden");

  fetch_criteria(e, &p->function_matching_path, "FunctionMatchingPath"); // A FAIRE
  
  /*
  fetchi(e, &p->maximum_error_points, "Tolerance", 0);
  fetchb(e, &p->global_symbol_align, "GlobalAlignSymbols", false);
  fetchb(e, &p->local_symbol_align, "LocalAlignSymbols", false);
  fetchi(e, &p->indentation_size, "IdentationSize", 2); // Doit matcher la taille d'une tab si on indente en tab.
  fetchi(e, &p->preproc_indent_size, "PrecompilerIdentationSize", 1);
  fetchi(e, &p->symbol_align_cost, "BadAlignCost", 1);
  fetchi(e, &p->misindent_cost, "BadIndentCost", 1);

  fetchb(e, &p->tab_indent, "IndentWithTab", false);
  fetchi(e, &p->tab_indent_cost, "BadTokenIndentCost", 1);

  fetchb(e, &p->header_macro, "HeaderMacro", false);
  fetchi(e, &p->header_macro_cost, "HeaderMacroCost", 1);

  fetchb(e, &p->single_non_static_function, "SingleNonStaticFunction", false);
  fetchi(e, &p->single_non_static_function_cost, "SingleNonStaticCost", 1);
  fetchb(e, &p->func_file_matching, "FunctionFileNameMatching", false);
  fetchi(e, &p->func_file_matching_cost, "FunctionFileNameMatching", 1);

  fetchb(e, &p->function_braces_alone, "GNUStyleBraces", false);
  fetchi(e, &p->function_braces_alone_cost, "GNUStyleBraces", 3);

  fetchb(e, &p->function_braces_inline, "KRStyleBraces", false);
  fetchi(e, &p->function_braces_inline_cost, "KRStyleBraces", 3);

  fetchb(e, &p->avoid_braces_if_possible, "AvoidBracesIfPossible", false);
  fetchi(e, &p->avoid_braces_if_possible_cost, "AvoidBracesIfPossibleCost", 1);

  fetchi(e, &p->max_functions_per_file, "MaxFunctionPerFile", -1);
  fetchi(e, &p->max_functions_per_file_cost, "MaxFunctionPerFileCost", 3);

  fetchi(e, &p->max_parameters_per_function, "MaxParametersPerFunction", -1);
  fetchi(e, &p->max_parameters_per_function_cost, "MaxParametersPerFunctionCost", 5);

  fetchi(e, &p->max_lines_per_function, "MaxLinesPerFunction", -1);
  fetchi(e, &p->max_lines_per_function_cost, "MaxLinesPerFunctionCost", 5);

  fetchi(e, &p->max_columns_per_line, "MaxColumnPerLine", -1);
  fetchi(e, &p->max_columns_per_line_cost, "MaxColumnPerLineCost", 5);

  fetchb(e, &p->break_forbidden, "BreakForbidden", false);
  fetchi(e, &p->break_cost, "BreakCost", 5);
  fetchb(e, &p->space_after_break, "SpaceAfterBreak", false);
  fetchi(e, &p->space_after_break_cost, "SpaceAfterBreakCost", 1);

  fetchb(e, &p->goto_forbidden, "GotoForbidden", false);
  fetchi(e, &p->goto_cost, "GotoCost", 5);

  fetchb(e, &p->continue_forbidden, "ContinueForbidden", false);
  fetchi(e, &p->continue_cost, "ContinueCost", 5);
  fetchb(e, &p->space_after_continue, "SpaceAfterContinue", false);
  fetchi(e, &p->space_after_continue_cost, "SpaceAfterContinueCost", 1);

  fetchb(e, &p->not_terminating_return_forbidden, "NotTerminatingReturnForbidden", false);
  fetchi(e, &p->not_terminating_return_cost, "NotTerminatingReturnCost", 5);
  fetchb(e, &p->space_after_return, "SpaceAfterReturn", false);
  fetchi(e, &p->space_after_return_cost, "SpaceAfterReturnCost", 1);

  fetchi(e, &p->maximum_if, "MaximumIf", -1);
  fetchi(e, &p->maximum_if_cost, "MaximumIfCost", p->maximum_error_points + 1);

  fetchb(e, &p->while_forbidden, "WhileForbidden", false);
  fetchi(e, &p->while_cost, "WhiteCost", 5);

  fetchb(e, &p->for_forbidden, "ForForbidden", false);
  fetchi(e, &p->for_cost, "ForCost", 5);

  fetchb(e, &p->do_forbidden, "DoForbidden", false);
  fetchi(e, &p->do_cost, "DoCost", 5);

  fetchb(e, &p->switch_forbidden, "SwitchForbidden", false);
  fetchi(e, &p->switch_cost, "SwitchCost", 5);

  /// continuer a header...
  */
}

char		*load_c_file(const char				*file,
			     t_bunny_configuration		*exe)
{
  char		buffer[512];
  const char	*cmd;
  int		len;
  
  if (!bunny_configuration_getf(exe, &cmd, "PrecompilationCommand"))
    cmd = "gcc -E -I./ -I./include/ -I/usr/local/include/ %s";
  snprintf(&buffer[0], sizeof(buffer), cmd, file);
  cmd = &buffer[0];
  len = sizeof(bunny_big_buffer);
  if (tcpopen("c norm", cmd, &bunny_big_buffer[0], &len, NULL, 0) != 0)
    return (NULL);
  return (&bunny_big_buffer[0]);
}
