/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<string.h>
#include		"crawler.h"

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
