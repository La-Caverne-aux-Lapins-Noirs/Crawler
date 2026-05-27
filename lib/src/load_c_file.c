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

#include	<fcntl.h>
#include	"crawler.h"


static bool	safe_append(char		*target,
			    size_t		target_len,
			    const char		*text)
{
  size_t	pos;
  size_t	len;

  pos = strlen(target);
  len = strlen(text);
  if (pos + len >= target_len)
    return (false);
  memcpy(&target[pos], text, len + 1);
  return (true);
}

static bool	append_shell_quoted(char	*target,
				    size_t	target_len,
				    const char	*text)
{
  if (!safe_append(target, target_len, "'"))
    return (false);
  for (size_t i = 0; text[i]; ++i)
    {
      if (text[i] == '\'')
	{
	  if (!safe_append(target, target_len, "'\\''"))
	    return (false);
	}
      else
	{
	  char buf[2];

	  buf[0] = text[i];
	  buf[1] = '\0';
	  if (!safe_append(target, target_len, &buf[0]))
	    return (false);
	}
    }
  return (safe_append(target, target_len, "'"));
}

static bool	append_include_path(char		*target,
				    size_t		target_len,
				    const char		*path)
{
  if (!safe_append(target, target_len, " -I"))
    return (false);
  if (!append_shell_quoted(target, target_len, path))
    return (false);
  return (safe_append(target, target_len, " "));
}


static void	remove_range(char	*data,
			     size_t	*len,
			     size_t	from,
			     size_t	to)
{
  if (from >= to || to > *len)
    return ;
  memmove(&data[from], &data[to], *len - to + 1);
  *len -= to - from;
}

static void	strip_saved_whitespace_before_comments(char	*data,
						 size_t	*len)
{
  bool		in_string;
  bool		in_char;
  bool		escape;
  size_t	i;

  in_string = false;
  in_char = false;
  escape = false;
  i = 0;
  while (i + 1 < *len)
    {
      if (escape)
	{
	  escape = false;
	  i += 1;
	  continue ;
	}
      if ((in_string || in_char) && data[i] == '\\')
	{
	  escape = true;
	  i += 1;
	  continue ;
	}
      if (!in_char && data[i] == '"')
	in_string = !in_string;
      else if (!in_string && data[i] == '\'')
	in_char = !in_char;
      else if (!in_string && !in_char && data[i] == '/' &&
	       (data[i + 1] == '/' || data[i + 1] == '*'))
	{
	  size_t	start;
	  bool	line_comment;

	  start = i;
	  while (start > 0 && (data[start - 1] == '\036' ||
				data[start - 1] == '\037'))
	    start -= 1;
	  if (start != i)
	    {
	      remove_range(data, len, start, i);
	      i = start;
	    }
	  line_comment = (data[i + 1] == '/');
	  if (line_comment)
	    while (i < *len && data[i] != '\n')
	      i += 1;
	  else
	    {
	      i += 2;
	      while (i + 1 < *len && !(data[i] == '*' && data[i + 1] == '/'))
		i += 1;
	      if (i + 1 < *len)
		i += 2;
	    }
	  continue ;
	}
      i += 1;
    }
}

static char *match_include(char				*str)
{
  char		buf[1024];
  ssize_t	i;
  
  if (*str != '#')
    return (NULL);
  i = 1;
  read_whitespace(str, &i);
  if (!bunny_read_text(str, &i, "include"))
    return (NULL);
  read_whitespace(str, &i);
  if (bunny_read_cstring(str, &i, &buf[0], sizeof(buf)))
    {
      read_whitespace(str, &i);
      while (i > 0 && str[i] != '\n')
	i = i - 1;
      if (i == 0)
	return (NULL); // Normalement c'est impossible.
      return (&str[i]);
    }
  if (str[i] != '<')
    return (NULL); // Pareil
  while (str[i] && str[i] != '>')
    i = i + 1;
  if (str[i])
    i = i + 1;
  read_whitespace(str, &i);
  return (&str[i]);
}

char		*load_c_file(const char			*file,
			     t_bunny_configuration	*exe,
			     bool			preprocessed)
{
  char		filename[512];
  int		fd;
  size_t	len;
  size_t	wt;
  ssize_t	rd;
  ssize_t	i;

  if ((fd = open(file, O_RDONLY)) == -1)
    return (NULL);
  len = 0;
  do
    {
      if (len >= sizeof(bunny_big_buffer) - 1)
	{
	  close(fd);
	  return (NULL);
	}
      if ((rd = read(fd, &bunny_big_buffer[len],
		     (sizeof(bunny_big_buffer) - 1) - len)) == -1)
	{
	  close(fd);
	  return (NULL);
	}
      else
	len += rd;
    }
  while (rd > 0);
  close(fd);
  bunny_big_buffer[len] = '\0';

  if (preprocessed)
    {
      char	*match = NULL;

      // On cherche le dernier include afin de marquer un symbole
      // permettant de ne pas coller des fautes de norme
      // a cause des fichiers inclus...
      // Pourquoi ne pas simplement ne pas utiliser le preprocesseur?
      // Faire tourner le preprocesseur au lieu de ne pas le faire
      // permet d'enrichir crawler des types déterminés par ces includes.
      i = strlen(&bunny_big_buffer[0]);
      while (i >= 0 && !(match = match_include(&bunny_big_buffer[i])))
	i = i - 1;
      if (match)
	{
	  // On déplace tout pour faire de la place pour le marqueur
	  memmove(match + 2, match, sizeof(bunny_big_buffer) - (match - &bunny_big_buffer[0]) - 2);
 	  match[0] = '\n'; // Au cas ou l'on soit dans un commentaire inline
	  match[1] = '\035';
	}
    }

  bool		in_preproc;

  in_preproc = false;
  for (size_t i = 0; i < len; ++i)
    if (bunny_big_buffer[i] == '#')
      {
	if (i > 0 && bunny_big_buffer[i - 1] == '\034')
	  bunny_big_buffer[i - 1] = '\n';
	in_preproc = true;
      }
    else if (in_preproc && bunny_big_buffer[i] == '\n')
      in_preproc = false;
    else if (in_preproc == false)
      {
	// On effectue des transformations permettant de conserver l'indentation
	// lors du passage du preprocesseur afin de pouvoir verifier le style
	// apres son passage.
	if (bunny_big_buffer[i] == ' ')
	  bunny_big_buffer[i] = '\036';
	else if (bunny_big_buffer[i] == '\t')
	  bunny_big_buffer[i] = '\037';
	else if (i > 0 &&
		 bunny_big_buffer[i] == '\n' && bunny_big_buffer[i - 1] == '\n')
	  bunny_big_buffer[i] = '\034';
	else if (bunny_big_buffer[i] == '\r')
	  bunny_big_buffer[i] = '\033';
      }


  len = strlen(&bunny_big_buffer[0]);
  if (preprocessed)
    strip_saved_whitespace_before_comments(&bunny_big_buffer[0], &len);
  if (snprintf(&filename[0], sizeof(filename), "%s!", file) >= (int)sizeof(filename))
    return (NULL);
  if ((fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
    return (NULL);
  wt = 0;
  do
    if ((rd = write(fd, &bunny_big_buffer[wt], len - wt)) == -1)
      {
	close(fd);
	unlink(filename);
	return (NULL);
      }
    else
      wt += rd;
  while (rd > 0 && wt != len);
  if (wt != len)
    {
      close(fd);
      unlink(filename);
      return (NULL);
    }
  close(fd);
  
  char		ipath[12 * 1024];
  char		qfilename[2 * sizeof(filename) + 16];
  char		buffer[16 * 1024];
  const char	*cmd;
  int		length;
  bool		custom_command;

  ipath[0] = 0;
  qfilename[0] = 0;
  custom_command = false;
  if (!append_shell_quoted(&qfilename[0], sizeof(qfilename), filename))
    {
      unlink(filename);
      return (NULL);
    }
  if (preprocessed)
    {
      for (size_t fi = 0; bunny_configuration_getf(exe, &cmd, "_AdditionalHeaderPath[%zu]", fi); ++fi)
	if (!append_include_path(&ipath[0], sizeof(ipath), cmd))
	  {
	    unlink(filename);
	    return (NULL);
	  }
      // -fdirectives-only cause too many bugs
      // and prevent being able to read code generated by macros
      // that would be harmful, and prevent being able to read because
      // it may be syntaxicaly wrong before being preprocessed
      if (bunny_configuration_getf(exe, &cmd, "PrecompilationCommand"))
	custom_command = true;
      else
	cmd = "cpp -std=c11 -dD -E -P -I./ -I./include/ -I/usr/local/include/ %s %s ";
    }
  else
    cmd = "cat %s %s";
  if (snprintf(&buffer[0], sizeof(buffer), cmd, ipath,
	       custom_command ? filename : qfilename) >= (int)sizeof(buffer))
    {
      unlink(filename);
      return (NULL);
    }
  cmd = &buffer[0];
  length = sizeof(bunny_big_buffer);
  if (tcpopen("c norm", cmd, &bunny_big_buffer[0], &length, NULL, 0) != 0)
    {
      unlink(filename);
      return (NULL);
    }
  bunny_big_buffer[length] = '\0';
  
  for (i = 0; bunny_big_buffer[i]; ++i)
    { // On rétabli les espaces et tabulations transformées pour leur propre sauvegarde
      if (bunny_big_buffer[i] == '\036')
	bunny_big_buffer[i] = ' ';
      else if (bunny_big_buffer[i] == '\037')
	bunny_big_buffer[i] = '\t';
      else if (bunny_big_buffer[i] == '\034')
	bunny_big_buffer[i] = '\n';
      else if (bunny_big_buffer[i] == '\033')
	bunny_big_buffer[i] = '\r';
    }

  // On laisse le marqueur de fin d'inclusion pour que crawler
  // puis ignorer tous les warnings situé avant
  // Mon intuition me dit que peut etre les utilisateurs pourraient
  // utiliser le fait que crawler ignore ce qu'il y a avant le marqueur
  // pour tricher en posant des #include en fin de fichier...
  // pour regler ce probleme, il faudrait une norminette pre preprocesseur
  // qui s'occupe des directives de precompilation et place des
  // méchants points d'erreurs dans la tronche des grugeurs.  
  unlink(filename);
  return (&bunny_big_buffer[0]);
}
