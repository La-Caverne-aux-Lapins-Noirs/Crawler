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

bool			test_ext(const char	*a,
				 const char	*ext)
{
  char			*x;

  if ((x = strstr(a, ext)) == NULL)
    return (false);
  return (strcmp(x, ext) == 0);
}

int			usage(const char	*s)
{
  fprintf(stderr, "%s: Usage is:\n\n"
	  "\t%s -c [configuration]+ [files]+ [--color]? [-v]?\n"
	  "\t\tTo test conformity. Order of parameter is irrelevant.\n"
	  "\t\tSupported configuration format are .dab, .json, .ini and .lua.\n\n"
	  "\t%s -m [files]+\n"
	  "\t\tTo create a function call map (Not implemented yet)\n\n"
	  "\t%s -d [files]+\n"
	  "\t\tTo create a Dabsic script with prototypes and types (Not implemented yet)\n"
	  "\n"
	  , s, s, s, s);
  return (EXIT_FAILURE);
}

int			main(int		argc,
			     char		**argv)
{
  t_bunny_configuration	*cnf;
  static t_parsing	parsing;
  static t_parsing	parsingtmp;
  int			i;

  if (argc < 3)
    return (usage(argv[0]));
  if (strcmp(argv[1], "-c") == 0)
    {
      bool		verbose = false;
      bool		color = false;
      int		total_error = 0;
      int		total_file = 0;
      int		working_file = 0;

      if ((cnf = bunny_new_configuration()) == NULL)
	{
	  fprintf(stderr, "%s: Not enough memory to initiate configuration.\n", argv[0]);
	  return (EXIT_FAILURE);
	}
      
      for (i = 1; i < argc; ++i)
	if (strcmp(argv[i], "--color") == 0)
	  color = true;
	else if (strcmp(argv[i], "-v") == 0)
	  verbose = true;
	else
	  {
	    t_bunny_configuration *new = cnf;
	    
	    if (test_ext(argv[i], ".dab")
		|| test_ext(argv[i], ".json")
		|| test_ext(argv[i], ".ini")
		|| test_ext(argv[i], ".lua"))
	      if ((new = bunny_open_configuration(argv[i], cnf)) == NULL)
		{
		  fprintf(stderr, "%s: Cannot open %s.\n", argv[0], argv[i]);
		  return (EXIT_FAILURE);
		}
	    cnf = new;
	  }
      load_norm_configuration(&parsing, cnf);

      for (i = 3; i < argc; ++i)
	{
	  if (argv[i][0] == '-')
	    continue ;
	  if (test_ext(argv[i], ".c") == false && test_ext(argv[i], ".h") == false)
	    continue ;
	  const char	*s;
	  ssize_t	j;

	  total_file += 1;
	  memcpy(&parsingtmp, &parsing, sizeof(parsingtmp));
	  parsingtmp.file = argv[i];
	  if ((s = load_c_file(argv[i], cnf, false)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open %s.\n", argv[0], argv[i]);
	      continue ;
	    }
	  if (check_header_file(&parsingtmp, s) == false)
	    continue ;
	  if ((s = load_c_file(argv[i], cnf, true)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open and precompile %s.\n", argv[0], argv[i]);
	      continue ;
	    }
	  if (verbose)
	    puts(s);
	  j = 0;
	  if (read_translation_unit(&parsingtmp, argv[i], s, &j, true) == -1)
	    continue ;

	  int		k;

	  for (k = 0; k <= parsingtmp.last_error_id; ++k)
	    {
	      if (color)
		{
		  if (k % 2)
		    printf("\033[1;34m");
		  else
		    printf("\033[1;35m");
		}
	      printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	      printf("%s", parsingtmp.last_error_msg[k]);
	    }
	  if (color)
	    printf("\033[00m\n");
	  if (k)
	    printf("Amount of warning: %d.\n", k);
	  if (parsingtmp.nbr_mistakes)
	    printf("Amount of different kind of mistakes: %d.\n", parsingtmp.nbr_mistakes);
	  if (parsingtmp.nbr_error_points)
	    printf("Amount of error points: %d.\n", parsingtmp.nbr_error_points);
	  working_file += 1;
	  total_error += parsingtmp.nbr_error_points;
	}
      if (total_error == 0 && working_file == total_file)
	printf("No errors were detected.\n");
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
