/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
** EFRITS SAS 2021-2026
** Pentacle Technologie 2008-2026
**
** TechnoCore
*/

#include		"test.h"

static void		reset_checked_return_test(t_parsing		*p,
						 t_bunny_configuration	*cnf)
{
  memset(p, 0, sizeof(*p));
  load_norm_configuration(p, cnf);
  p->last_error_id = -1;
  p->last_new_type = 0;
  p->checked_return.counter = 0;
}

static void		parse_checked_return_case(t_parsing		*p,
						  t_bunny_configuration	*cnf,
						  const char		*source)
{
  ssize_t		index;

  reset_checked_return_test(p, cnf);
  index = 0;
  assert(read_translation_unit(p, "checked_return", source, &index, true, false) == 1);
}

static t_bunny_configuration *new_checked_return_configuration(void)
{
  t_bunny_configuration	*configuration;

  assert(configuration = bunny_new_configuration());
  assert(bunny_configuration_setf(configuration, 1, "CheckedReturn"));
  return (configuration);
}

static void		test_ignored_return(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void ignored_return(void)\n"
     "{\n"
     "  int fd;\n"
     "  char buffer[42];\n"
     "\n"
     "  fd = 0;\n"
     "  malloc(42);\n"
     "  read(fd, buffer, 42);\n"
     "  close(fd);\n"
     "}\n");
  assert(p.checked_return.counter >= 3);
  bunny_delete_configuration(configuration);
}

static void		test_checked_malloc(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void checked_malloc(void)\n"
     "{\n"
     "  char *ptr;\n"
     "\n"
     "  ptr = malloc(42);\n"
     "  if (!ptr)\n"
     "    return;\n"
     "  ptr[0] = 0;\n"
     "}\n");
  assert(p.checked_return.counter == 0);
  bunny_delete_configuration(configuration);
}

static void		test_use_before_check(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void use_before_check(void)\n"
     "{\n"
     "  char *ptr;\n"
     "\n"
     "  ptr = malloc(42);\n"
     "  ptr[0] = 0;\n"
     "  if (!ptr)\n"
     "    return;\n"
     "}\n");
  assert(p.checked_return.counter >= 1);
  bunny_delete_configuration(configuration);
}

static void		test_stored_never_checked(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void stored_never_checked(void)\n"
     "{\n"
     "  char *ptr;\n"
     "\n"
     "  ptr = malloc(42);\n"
     "}\n");
  assert(p.checked_return.counter >= 1);
  bunny_delete_configuration(configuration);
}

static void		test_realloc_direct_assignment(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void realloc_direct_assignment(void)\n"
     "{\n"
     "  char *ptr;\n"
     "\n"
     "  ptr = realloc(ptr, 42);\n"
     "  if (!ptr)\n"
     "    return;\n"
     "}\n");
  assert(p.checked_return.counter >= 1);
  bunny_delete_configuration(configuration);
}

static void		test_realloc_with_temporary(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void realloc_with_temporary(void)\n"
     "{\n"
     "  char *ptr;\n"
     "  char *tmp;\n"
     "\n"
     "  tmp = realloc(ptr, 42);\n"
     "  if (!tmp)\n"
     "    return;\n"
     "  ptr = tmp;\n"
     "}\n");
  assert(p.checked_return.counter == 0);
  bunny_delete_configuration(configuration);
}

static void		test_partial_write_not_checked(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void partial_write_not_checked(void)\n"
     "{\n"
     "  int fd;\n"
     "  int ret;\n"
     "  int len;\n"
     "  char buffer[42];\n"
     "\n"
     "  ret = write(fd, buffer, len);\n"
     "  if (ret < 0)\n"
     "    return;\n"
     "}\n");
  assert(p.checked_return.counter >= 1);
  bunny_delete_configuration(configuration);
}

static void		test_full_write_checked(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  configuration = new_checked_return_configuration();
  parse_checked_return_case
    (&p, configuration,
     "void full_write_checked(void)\n"
     "{\n"
     "  int fd;\n"
     "  int ret;\n"
     "  int len;\n"
     "  char buffer[42];\n"
     "\n"
     "  ret = write(fd, buffer, len);\n"
     "  if (ret != len)\n"
     "    return;\n"
     "}\n");
  assert(p.checked_return.counter == 0);
  bunny_delete_configuration(configuration);
}

static void		test_configurable_function_list(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  assert(configuration = bunny_new_configuration());
  assert(bunny_configuration_setf(configuration, 1, "CheckedReturn"));
  assert(bunny_configuration_setf(configuration, 0, "CheckedReturn.UseDefaultList"));
  assert(bunny_configuration_setf(configuration, "my_alloc", "CheckedReturn.Functions[0].Name"));
  parse_checked_return_case
    (&p, configuration,
     "void configurable_function_list(void)\n"
     "{\n"
     "  malloc(42);\n"
     "  my_alloc(42);\n"
     "}\n");
  assert(p.checked_return.counter == 1);
  bunny_delete_configuration(configuration);
}

static void		test_disabled_default_function(void)
{
  t_parsing		p;
  t_bunny_configuration	*configuration;

  assert(configuration = bunny_new_configuration());
  assert(bunny_configuration_setf(configuration, 1, "CheckedReturn"));
  assert(bunny_configuration_setf(configuration, "close", "CheckedReturn.Functions[0].Name"));
  assert(bunny_configuration_setf(configuration, 1, "CheckedReturn.Functions[0].Disabled"));
  parse_checked_return_case
    (&p, configuration,
     "void disabled_default_function(void)\n"
     "{\n"
     "  int fd;\n"
     "\n"
     "  close(fd);\n"
     "}\n");
  assert(p.checked_return.counter == 0);
  bunny_delete_configuration(configuration);
}

int			main(int		argc,
			     char		**argv)
{
  TEST_INTRO(); // LCOV_EXCL_LINE

  test_ignored_return();
  test_checked_malloc();
  test_use_before_check();
  test_stored_never_checked();
  test_realloc_direct_assignment();
  test_realloc_with_temporary();
  test_partial_write_not_checked();
  test_full_write_checked();
  test_configurable_function_list();
  test_disabled_default_function();

  TEST_OUTRO(); // LCOV_EXCL_LINE
}
