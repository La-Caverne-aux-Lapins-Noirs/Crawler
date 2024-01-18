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

#ifndef			__CRAWLER_TEST_H__
# define		__CRAWLER_TEST_H__
# include		<signal.h>
# include		<string.h>
# include		<assert.h>
# include		"crawler.h"

# ifdef			FTRACE
#  define		BIGLINE()		puts("-----------------------------")
# else
#  define		BIGLINE()
# endif

# define		GOTOERROR()			\
  do { last_line = __LINE__; goto Error; } while (0)
static int		last_line;

# define		TEST_VAR()		\
  t_parsing		p;			\
  t_bunny_configuration	*cnf;			\
  char			*s;			\
  ssize_t		i;			\
  char			*file;			\
  char			*cfile;			\
  bool			sig;			\
  bool			got

# define		TEST_INTRO()		\
  TEST_VAR();					\
  if (chdir("./lib/test") != 0)			\
    if (chdir("./test") != 0)			\
      {}					\
  if (argc == 1)				\
    {						\
      sig = true;				\
      got = true;				\
    }						\
  for (int ac = 1; ac < argc; ++ac)		\
    if (strcmp(argv[ac], "nosignal") == 0)	\
      sig = false;				\
    else if (strcmp(argv[ac], "nogoto") == 0)	\
      got = false;				\
						\
  if (sig)					\
    {						\
      signal(SIGALRM, alert);			\
      alarm(10);				\
    }						\
						\
  memset(&p, 0, sizeof(p));			\
  assert(cnf = bunny_new_configuration());	\
  load_norm_configuration(&p, cnf);		\
  gl_bunny_read_whitespace = read_whitespace;	\
  if (got)					\
    {}
// goto NOW

# define		TEST_OUTRO()		\
  return (EXIT_SUCCESS);			\
 Error:						\
  printf("Aborted from line %d.\n", last_line);		\
 if (s)							\
   printf("Stopped at %s\nIn:\n%s\n\n", &s[i], s);	\
 for (int i = 0; i <= p.last_error_id; ++i)		\
   printf("%s\n", p.last_error_msg[i]);			\
 printf("Declared types are:\n");				\
 for (size_t i = 0; i < p.last_new_type; )			\
   {								\
     for (size_t j = 0; j < 4 && i < p.last_new_type; ++j)	\
       {							\
	 printf("%40s", p.new_type[i].name);			\
	 i = i + 1;						\
       }							\
     printf("\n");						\
   }								\
 return (EXIT_FAILURE)

static void		alert(int		alrt)
{
  (void)alrt;
  exit(1);
}

#endif	/*		__CRAWLER_TEST_H__	*/
