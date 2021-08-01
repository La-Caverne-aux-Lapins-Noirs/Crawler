/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2021
**
** C-C-C CRAWLER!
** Configurable C Code Crawler !
** Bloc constitutif du "TechnoCentre", suite logiciel du projet "Pentacle School"
** Vérificateur de confirmité du code (entre autre) niveau style.
*/

#include		<stdio.h>
#include		"crawler.h"

int			usage(const char	*s)
{
  fprintf(stderr, "%s: Usage is:\n"
	  "\t%s -c configuration [files]+"
	  "\tTo test conformity\n"
	  "\t%s -m [files]+"
	  "\tTo create a function call map (Not implemented yet)\n"
	  "\t%s -d [files]+"
	  "\tTo create a Dabsic script with prototypes and types (Not implemented yet)\n"
	  "\n\n\t-q to mute error messages\n"
	  , s, s, s, s);
  return (EXIT_FAILURE);
}

int			main(int		argc,
			     char		**argv)
{
  t_bunny_configuration	*cnf;
  t_parsing		parsing;
  int			i;

  if (argc < 3)
    return (usage(argv[0]));
  if (strcmp(argv[1], "-c") == 0)
    {
      if ((cnf = bunny_open_configuration(argv[2], NULL)) == NULL)
	{
	  fprintf(stderr, "%s: Cannot open %s.\n", argv[0], argv[1]);
	  return (EXIT_FAILURE);
	}
      load_norm_configuration(&parsing, cnf);
      for (i = 2; i < argc; ++i)
	{
	  const char	*s;
	  ssize_t	j;

	  if ((s = load_c_file(argv[i], cnf, false)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open %s.\n", argv[0], argv[i]);
	      return (EXIT_FAILURE);
	    }
	  if (check_header_file(&parsing, s) == false)
	    return (EXIT_FAILURE);
	  if ((s = load_c_file(argv[i], cnf, true)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open and precompile %s.\n", argv[0], argv[i]);
	      return (EXIT_FAILURE);
	    }
	  j = 0;
	  if (read_translation_unit(&parsing, argv[i], s, &j, true) == -1)
	    return (EXIT_FAILURE);
	}
      return (EXIT_SUCCESS);
    }
  if (strcmp(argv[1], "-m") == 0)
    {

    }
  if (strcmp(argv[1], "-d") == 0)
    {

    }
  fprintf(stderr, "%s: Unrecognized option '%s'.\n", argv[0], argv[1]);
  return (usage(argv[0]));
}
