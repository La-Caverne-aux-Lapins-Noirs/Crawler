/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"
#define			IGN(a)					{ if (a) {} }

int			write_line_and_position(const char	*code,
						int		pos,
						char		*buf,
						size_t		len,
						bool		position)
{
  int			i = pos;
  int			j;
  int			k = 0;

  while (i > 0 && code[i] != '\n')
    i -= 1;
  if (code[i] == '\n')
    i += 1;
  j = i;
  while (code[j] && code[j] != '\n')
    j += 1;
  if (code[j] == '\n')
    j -= 1;
  k += snprintf(&buf[k], len - k, "%.*s\n", j - i + 1, &code[i]);
  if (!position)
    return (k);
  while (i < pos)
    {
      k += snprintf(&buf[k], len - k, " ");
      i += 1;
    }
  k += snprintf(&buf[k], len - k, "^\n");
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
// LCOV_EXCL_STOP
