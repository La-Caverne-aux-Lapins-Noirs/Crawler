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
#include		<stdlib.h>
#include		<string.h>
#include		<sys/types.h>
#include		<sys/ioctl.h>
#include		"crawler.h"

#ifndef			CRAWLER_DEFAULT_CONFIGURATION
# define		CRAWLER_DEFAULT_CONFIGURATION	"/usr/share/crawler/default.dab"
#endif

static bool		test_ext(const char	*filepath,
				 const char	*ext)
{
  size_t		flen;
  size_t		elen;

  flen = strlen(filepath);
  elen = strlen(ext);
  if (flen < elen)
    return (false);
  return (strcmp(&filepath[flen - elen], ext) == 0);
}

static void		print_separator(void)
{
  int			width;

  width = 80;
#ifdef TIOCGWINSZ
  {
    struct winsize	ws;

    if (ioctl(1, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
      width = ws.ws_col;
  }
#endif
  while (width-- > 0)
    putchar('-');
  putchar('\n');
}

static int		usage(const char	*prog_name)
{
  fprintf(stderr, "%s: Usage is:\n\n"
	  "\t%s -c [configuration]* [files]+ [--nocolor]? [-v]? [-I header_path]*\n"
	  "\t\tTo test conformity. Order of parameter is irrelevant.\n"
	  "\t\tSupported configuration format are .dab, .json, .ini and .lua.\n\n"
	  "\t%s -m -o output.dot [files]+ [-I header_path]*\n"
	  "\t\tTo create a Graphviz/DOT function call map.\n\n"
	  "\t%s -d [files]+\n"
	  "\t\tTo create a Dabsic script with prototypes and types (Not implemented yet)\n"
  	  "\t%s -f [files]+\n"
	  "\t\tTo extract function calls (Not implemented yet)\n"
	  "\n"
	  , prog_name, prog_name, prog_name, prog_name, prog_name);
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
      bool		color = true;
      int		total_error = 0;
      int		total_file = 0;
      int		working_file = 0;
      int		processing_error = 0;
      int		cnf_cnt = 0;
      int		hdrfile = 0;

      if ((cnf = bunny_new_configuration()) == NULL)
	{
	  fprintf(stderr, "%s: Not enough memory to initiate configuration.\n", argv[0]);
	  return (EXIT_FAILURE);
	}
      
      for (i = 2; i < argc; ++i)
	if (strcmp(argv[i], "--nocolor") == 0)
	  color = false;
	else if (strcmp(argv[i], "-v") == 0)
	  verbose = true;
      	else if (strcmp(argv[i], "-I") == 0)
	  {
	    if (i + 1 >= argc)
	      {
		fprintf(stderr, "%s: Missing path after -I.\n", argv[0]);
		return (EXIT_FAILURE);
	      }
	    if (!bunny_configuration_setf(cnf, argv[i + 1], "_AdditionalHeaderPath[%d]", hdrfile))
	      {
		fprintf(stderr, "%s: Cannot set additional header in inner configuration.\n", argv[0]);
		return (EXIT_FAILURE);
	      }
	    hdrfile += 1;
	    i += 1;
	  }
	else if (test_ext(argv[i], ".dab")
		 || test_ext(argv[i], ".json")
		 || test_ext(argv[i], ".ini")
		 || test_ext(argv[i], ".lua"))
	  {
	    t_bunny_configuration *new;

	    if ((new = bunny_open_configuration(argv[i], cnf)) == NULL)
	      {
		fprintf(stderr, "%s: Cannot open %s.\n", argv[0], argv[i]);
		return (EXIT_FAILURE);
	      }
	    cnf_cnt += 1;
	    cnf = new;
	  }
      if (cnf_cnt == 0)
	{
	  t_bunny_configuration *new;

	  if ((new = bunny_open_configuration(CRAWLER_DEFAULT_CONFIGURATION, cnf)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open default configuration %s.\n",
		      argv[0], CRAWLER_DEFAULT_CONFIGURATION);
	      return (EXIT_FAILURE);
	    }
	  cnf = new;
	}
      load_norm_configuration(&parsing, cnf);

      for (i = 2; i < argc; ++i)
	{
	  if (strcmp(argv[i], "-I") == 0)
	    {
	      i += 1;
	      continue ;
	    }
	  if (argv[i][0] == '-')
	    continue ;
	  if (test_ext(argv[i], ".dab")
	      || test_ext(argv[i], ".json")
	      || test_ext(argv[i], ".ini")
	      || test_ext(argv[i], ".lua"))
	    continue ;
	  if (test_ext(argv[i], ".c") == false && test_ext(argv[i], ".h") == false)
	    continue ;
	  const char	*s; // dedans se retrouve bunny big buffer
	  ssize_t	j;

	  total_file += 1;
	  memcpy(&parsingtmp, &parsing, sizeof(parsingtmp));
	  parsingtmp.file = argv[i];
	  bool		file_failed = false;

	  if ((s = load_c_file(argv[i], cnf, false)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open %s.\n", argv[0], argv[i]);
	      file_failed = true;
	      goto print_file_report;
	    }
	  if (check_header_file(&parsingtmp, s) == false)
	    {
	      file_failed = true;
	      goto print_file_report;
	    }
	  if ((s = load_c_file(argv[i], cnf, true)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open and precompile %s.\n", argv[0], argv[i]);
	      file_failed = true;
	      goto print_file_report;
	    }
	  if (verbose)
	    puts(s);
	  j = 0;
	  if (read_translation_unit(&parsingtmp, argv[i], s, &j, true, true) == -1)
	    file_failed = true;

	  int		k;

	print_file_report:
	  for (k = 0; k <= parsingtmp.last_error_id; ++k)
	    {
	      if (color)
		{
		  if (k % 2)
		    printf("\033[1;34m");
		  else
		    printf("\033[1;35m");
		}
	      print_separator();
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
	  if (file_failed)
	    processing_error += 1;
	  else
	    working_file += 1;
	  total_error += parsingtmp.nbr_error_points;
	}
      if (total_file == 0)
	{
	  fprintf(stderr, "%s: No C source or header file was provided.\n", argv[0]);
	  return (EXIT_FAILURE);
	}
      if (total_error == 0 && processing_error == 0 && working_file == total_file)
	{
	  printf("No errors were detected.\n");
	  return (EXIT_SUCCESS);
	}
      if (processing_error != 0)
	fprintf(stderr, "%s: %d file(s) could not be checked reliably.\n",
		argv[0], processing_error);
      if (parsing.maximum_error_points >= 0 && total_error <= parsing.maximum_error_points)
	return (processing_error == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
      return (EXIT_FAILURE);
    }
  if (strcmp(argv[1], "-m") == 0)
    {
      const char	*output = NULL;
      int		hdrfile = 0;
      int		total_file = 0;
      int		failed_file = 0;

      if ((cnf = bunny_new_configuration()) == NULL)
	{
	  fprintf(stderr, "%s: Not enough memory to initiate configuration.\n", argv[0]);
	  return (EXIT_FAILURE);
	}
      for (i = 2; i < argc; ++i)
	{
	  if (strcmp(argv[i], "-o") == 0)
	    {
	      if (i + 1 >= argc)
		{
		  fprintf(stderr, "%s: Missing path after -o.\n", argv[0]);
		  return (EXIT_FAILURE);
		}
	      output = argv[i + 1];
	      i += 1;
	    }
	  else if (strcmp(argv[i], "-I") == 0)
	    {
	      if (i + 1 >= argc)
		{
		  fprintf(stderr, "%s: Missing path after -I.\n", argv[0]);
		  return (EXIT_FAILURE);
		}
	      if (!bunny_configuration_setf(cnf, argv[i + 1],
					    "_AdditionalHeaderPath[%d]", hdrfile))
		{
		  fprintf(stderr, "%s: Cannot set additional header in inner configuration.\n", argv[0]);
		  return (EXIT_FAILURE);
		}
	      hdrfile += 1;
	      i += 1;
	    }
	}
      if (output == NULL)
	{
	  fprintf(stderr, "%s: Missing -o output.dot for -m.\n", argv[0]);
	  return (usage(argv[0]));
	}
      memset(&parsing, 0, sizeof(parsing));
      parsing.configuration = cnf;
      parsing.last_error_id = -1;
      crawler_function_map_enable(&parsing, true);
      for (i = 2; i < argc; ++i)
	{
	  const char	*s;
	  ssize_t	j;

	  if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-I") == 0)
	    {
	      i += 1;
	      continue ;
	    }
	  if (argv[i][0] == '-')
	    continue ;
	  if (test_ext(argv[i], ".c") == false && test_ext(argv[i], ".h") == false)
	    continue ;
	  total_file += 1;
	  if ((s = load_c_file(argv[i], cnf, true)) == NULL)
	    {
	      fprintf(stderr, "%s: Cannot open and precompile %s.\n", argv[0], argv[i]);
	      failed_file += 1;
	      continue ;
	    }
	  j = 0;
	  if (read_translation_unit(&parsing, argv[i], s, &j, false, true) == -1)
	    {
	      int	k;

	      fprintf(stderr, "%s: Cannot parse %s reliably.\n", argv[0], argv[i]);
	      for (k = 0; k <= parsing.last_error_id; ++k)
		fprintf(stderr, "%s", parsing.last_error_msg[k]);
	      failed_file += 1;
	    }
	}
      if (total_file == 0)
	{
	  fprintf(stderr, "%s: No C source or header file was provided.\n", argv[0]);
	  crawler_function_map_clear(&parsing);
	  return (EXIT_FAILURE);
	}
      if (failed_file != 0)
	{
	  crawler_function_map_clear(&parsing);
	  return (EXIT_FAILURE);
	}
      if (!crawler_function_map_write_dot(&parsing, output))
	{
	  fprintf(stderr, "%s: Cannot write %s.\n", argv[0], output);
	  crawler_function_map_clear(&parsing);
	  return (EXIT_FAILURE);
	}
      crawler_function_map_clear(&parsing);
      return (EXIT_SUCCESS);
    }
  if (strcmp(argv[1], "-d") == 0)
    {

    }
  fprintf(stderr, "%s: Unrecognized option '%s'.\n", argv[0], argv[1]);
  return (usage(argv[0]));
}
