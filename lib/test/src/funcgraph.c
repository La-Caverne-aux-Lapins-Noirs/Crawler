/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
** EFRITS SAS 2022-2026
**
** TechnoCore
*/

#include		<stdio.h>
#include		<stdlib.h>
#include		"test.h"
#include		"funcgraph.h"

static ssize_t		find_node(t_function_map		*map,
				  const char			*name)
{
  size_t		i;

  for (i = 0; i < map->nbr_nodes; ++i)
    if (map->nodes[i].name && strcmp(map->nodes[i].name, name) == 0)
      return ((ssize_t)i);
  return (-1);
}

static t_function_map_edge *find_edge(t_function_map		*map,
				       const char		*from,
				       const char		*to)
{
  ssize_t		f;
  ssize_t		t;
  size_t		i;

  assert((f = find_node(map, from)) >= 0);
  assert((t = find_node(map, to)) >= 0);
  for (i = 0; i < map->nbr_edges; ++i)
    if (map->edges[i].from == (size_t)f && map->edges[i].to == (size_t)t)
      return (&map->edges[i]);
  return (NULL);
}

static void		assert_call(t_function_map		*map,
				    const char			*from,
				    const char			*to,
				    int				count)
{
  t_function_map_edge	*edge;

  assert((edge = find_edge(map, from, to)) != NULL);
  assert(edge->direct_call_count == count);
}

static void		assert_funcptr(t_function_map		*map,
				       const char		*from,
				       const char		*to,
				       int			count)
{
  t_function_map_edge	*edge;

  assert((edge = find_edge(map, from, to)) != NULL);
  assert(edge->pointer_donation_count == count);
}

static void		parse_one(t_parsing			*p,
				  t_bunny_configuration		*cnf,
				  const char			*path)
{
  char			*s;
  ssize_t		i;

  assert(s = load_c_file(path, cnf, true));
  i = 0;
  p->last_error_id = -1;
  assert(read_translation_unit(p, path, s, &i, false, true) == 1);
}

int			main(int	argc,
			     char	**argv)
{
  TEST_INTRO(); // LCOV_EXCL_LINE

  (void)file;
  (void)cfile;
  crawler_function_map_enable(&p, true);
  parse_one(&p, cnf, "./res/mappable/main.c");
  parse_one(&p, cnf, "./res/mappable/a_file.c");
  parse_one(&p, cnf, "./res/mappable/ab_file.c");
  parse_one(&p, cnf, "./res/mappable/b_file.c");
  parse_one(&p, cnf, "./res/mappable/c_file.c");
  parse_one(&p, cnf, "./res/mappable/noise.c");

  assert(find_node(&p.function_map, "fake_call") == -1);
  assert(find_node(&p.function_map, "this") == -1);
  assert_call(&p.function_map, "main", "a", 1);
  assert_call(&p.function_map, "main", "b", 1);
  assert_call(&p.function_map, "main", "c", 2);
  assert_call(&p.function_map, "a", "aa", 1);
  assert_call(&p.function_map, "a", "ab", 1);
  assert_call(&p.function_map, "a", "register_callback", 2);
  assert_call(&p.function_map, "b", "bb", 3);
  assert_call(&p.function_map, "b", "b", 1);
  assert_call(&p.function_map, "ab", "ab", 1);
  assert_call(&p.function_map, "ab", "a", 1);
  assert_call(&p.function_map, "dead_branch", "a", 1);
  assert_call(&p.function_map, "dead_branch", "b", 1);
  assert_call(&p.function_map, "dead_branch", "c", 1);
  assert_call(&p.function_map, "completely_unused_entry_point", "unused_helper", 1);
  assert_call(&p.function_map, "completely_unused_entry_point", "dead_branch", 1);
  assert_funcptr(&p.function_map, "register_callback", "aa", 1);
  assert_funcptr(&p.function_map, "register_callback", "ab", 1);
  assert(find_node(&p.function_map, "register_callback") >= 0);
  assert(p.function_map.nodes[find_node(&p.function_map, "register_callback")].defined == false);
  assert(crawler_function_map_write_dot(&p, "./funcgraph.dot"));
  // remove("./funcgraph.dot");
  crawler_function_map_clear(&p);
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
