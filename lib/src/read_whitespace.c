/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<string.h>
#include		"crawler.h"

t_parsing		*gl_parsing_save = NULL;

bool			read_whitespace(const char		*code,
					ssize_t			*i)
{
  bool			ret;

  gl_bunny_read_whitespace = NULL;
  do
    {
      ret = false;
      if (bunny_read_char(code, i, " \t\r\n\035"))
	ret = true;
      if (bunny_read_text(code, i, "/*"))
	{
	  ret = true;
	  while (code[*i] && !bunny_check_text(code, i, "*/"))
	    *i += 1;
	  if (code[*i] == '\0')
	    goto BadEnd;
	  bunny_read_text(code, i, "*/");
	}
      if (bunny_read_text(code, i, "//"))
	{
	  ret = true;
	  while (code[*i] && !bunny_check_text(code, i, "\n"))
	    *i += 1;
	  if (!code[*i])
	    goto GoodEnd;
	  bunny_read_text(code, i, "\n");
	}
      if (bunny_read_text(code, i, "#")) // Au cas ou il reste du preprocesseur
	{
	  char		buffer[1024];
	  int		line;

	  bunny_read_char(code, i, " \t\r\n\035");
	  if (gl_parsing_save && bunny_read_integer(code, i, &line))
	    {
	      bunny_read_char(code, i, " \t\r\n\035");
	      if (bunny_read_cstring(code, i, &buffer[0], sizeof(buffer)))
		{
		  // gl_parsing_save->last_line_marker = line;
		  gl_parsing_save->last_line_marker_line = bunny_which_line(code, *i);
		}
	    }

	  ret = true;
	  do
	    {
	      while (code[*i] && !bunny_check_text(code, i, "\n"))
		*i += 1;
	      if (!code[*i])
		goto GoodEnd;
	      bunny_read_text(code, i, "\n");
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
