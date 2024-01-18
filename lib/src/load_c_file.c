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
  size_t	i;

  if ((fd = open(file, O_RDONLY)) == -1)
    return (NULL);
  len = 0;
  do
    if ((rd = read(fd, &bunny_big_buffer[len], (sizeof(bunny_big_buffer) - 1) - len)) == -1)
      {
	close(fd);
	return (NULL);
      }
    else
      len += rd;
  while (rd > 0);
  close(fd);
  bunny_big_buffer[len] = '\0';
  bool		in_preproc;

  in_preproc = false;
  for (size_t i = 0; i < len; ++i)
    if (bunny_big_buffer[i] == '#')
      in_preproc = true;
    else if (bunny_big_buffer[i] == '\n')
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
      }

  if (preprocessed)
    {
      char	*match;

      // On cherche le dernier include afin de marquer un symbole
      // permettant de ne pas coller des fautes de norme
      // a cause des fichiers inclus...
      // Pourquoi ne pas simplement ne pas utiliser le preprocesseur?
      // Faire tourner le preprocesseur au lieu de ne pas le faire
      // permet d'enrichir crawler des types déterminés par ces includes.
      i = strlen(&bunny_big_buffer[0]);
      while (i > 0 && !(match = match_include(&bunny_big_buffer[i])))
	i = i - 1;
      if (match)
	{
	  // On déplace tout pour faire de la place pour le marqueur
	  memmove(match + 2, match, sizeof(bunny_big_buffer) - (match - &bunny_big_buffer[0]) - 2);
 	  match[0] = '\n'; // Au cas ou l'on soit dans un commentaire inline
	  match[1] = '\035';
	}
    }

  len = strlen(&bunny_big_buffer[0]);
  snprintf(&filename[0], sizeof(filename), "%s!", file);
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
  close(fd);

  char		buffer[512];
  const char	*cmd;
  int		length;

  if (preprocessed)
    {
      // -fdirectives-only cause too many bugs
      // and prevent being able to read code generated by macros
      // that would be harmful, and prevent being able to read because
      // it may be syntaxicaly wrong before being preprocessed
      if (!bunny_configuration_getf(exe, &cmd, "PrecompilationCommand"))
	cmd = "cpp -std=c11 -dD -E -P -I./ -I./include/ -I/usr/local/include/ '%s' ";
    }
  else
    cmd = "cat '%s'";
  snprintf(&buffer[0], sizeof(buffer), cmd, filename);
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