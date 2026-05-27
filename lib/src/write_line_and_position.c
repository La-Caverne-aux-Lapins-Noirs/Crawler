/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"
#define			IGN(a)					{ if (a) {} }

static int		safe_append(char		*buf,
				    size_t		len,
				    size_t		*k,
				    const char		*fmt,
				    ...)
{
  va_list		lst;
  int			ret;

  if (*k >= len)
    return (0);
  va_start(lst, fmt);
  ret = vsnprintf(&buf[*k], len - *k, fmt, lst);
  va_end(lst);
  if (ret < 0)
    return (-1);
  if ((size_t)ret >= len - *k)
    {
      *k = len - 1;
      return (ret);
    }
  *k += ret;
  return (ret);
}

int			write_line_and_position(const char	*code,
						int		pos,
						char		*buf,
						size_t		len,
						bool		position)
{
  int			i = pos;
  int			j;
  size_t		k = 0;

  if (len == 0)
    return (0);
  while (i > 0 && code[i] != '\n')
    i -= 1;
  if (code[i] == '\n')
    i += 1;
  j = i;
  while (code[j] && code[j] != '\n')
    j += 1;
  if (code[j] == '\n')
    j -= 1;
  if (j - i <= 0)
    {
      buf[0] = '\n';
      if (len > 1)
	buf[1] = '\0';
      return (1);
    }
  safe_append(buf, len, &k, "%.*s\n", j - i + 1, &code[i]);
  if (!position)
    return (k);
  while (i < pos)
    {
      if (code[i] == '\t')
	safe_append(buf, len, &k, "\t");
      else
	safe_append(buf, len, &k, " ");
      i += 1;
    }
  safe_append(buf, len, &k, "^\n");
  return (k);
}

// LCOV_EXCL_START
void			print_line_and_position(t_parsing	*p,
						const char	*code,
						int		pos,
						bool		position)
{
  static bool		color = false;
  char			buffer[1024];
  int			l;

  if ((color = !color))
    {
      IGN(write(1, "\033[0;31m", 7));
    }
  else
    {
      IGN(write(1, "\033[1;37m", 7));
    }
  IGN(write(1, "---------------------\n", 22));
  printf("Understood indentation: %d\n", p->last_declaration.indent_depth);
  l = write_line_and_position(code, pos, &buffer[0], sizeof(buffer), position);
  IGN(write(1, buffer, l));
}

void			full_write_with_arrow(t_parsing		*p,
					      const char	*code,
					      int		pos)
{
  static bool		color = false;
  char			buffer[4096];
  int			line = bunny_which_line(code, pos);

  if ((color = !color))
    {
      IGN(write(1, "\033[0;31m", 7));
    }
  else
    {
      IGN(write(1, "\033[1;37m", 7));
    }
  IGN(write(1, "---------------------\n", 22));
  printf("Understood indentation: %d\n", p->last_declaration.indent_depth);
  snprintf(&buffer[0], sizeof(buffer),
	   "cat -n << EOF | sed 's/%d/>>/g'\n%s\nEOF\n", line, code);
  IGN(system(&buffer[0]));
}

void			write_indent(t_parsing			*p)
{
  IGN(write(1,
	    "--------------------------------------------------",
	    p->last_declaration.indent_depth));
  IGN(write(1, "\n", 1));
}
// LCOV_EXCL_STOP
