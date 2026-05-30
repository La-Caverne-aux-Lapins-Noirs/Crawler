/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
** EFRITS SAS 2022-2026
**
** Function call graph support for crawler -m.
*/

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<sys/types.h>
#include		"crawler.h"


static void		map_copy_symbol(char			*target,
					const char			*source,
					size_t			 target_size)
{
  size_t		len;

  if (target == NULL || target_size == 0)
    return ;
  if (source == NULL)
    source = "";
  len = strlen(source);
  if (len >= target_size)
    len = target_size - 1;
  memcpy(target, source, len);
  target[len] = '\0';
}

static char		*map_strdup(const char		*str)
{
  char			*out;
  size_t		len;

  if (str == NULL)
    str = "";
  len = strlen(str) + 1;
  if ((out = malloc(len)) == NULL)
    return (NULL);
  memcpy(out, str, len);
  return (out);
}

static bool		reserve_nodes(t_function_map	*map,
				      size_t		needed)
{
  t_function_map_node	*nodes;
  size_t		capacity;

  if (map->node_capacity >= needed)
    return (true);
  capacity = map->node_capacity ? map->node_capacity * 2 : 64;
  while (capacity < needed)
    capacity *= 2;
  if ((nodes = realloc(map->nodes, capacity * sizeof(*nodes))) == NULL)
    return (false);
  memset(&nodes[map->node_capacity], 0,
	 (capacity - map->node_capacity) * sizeof(*nodes));
  map->nodes = nodes;
  map->node_capacity = capacity;
  return (true);
}

static bool		reserve_edges(t_function_map	*map,
				      size_t		needed)
{
  t_function_map_edge	*edges;
  size_t		capacity;

  if (map->edge_capacity >= needed)
    return (true);
  capacity = map->edge_capacity ? map->edge_capacity * 2 : 128;
  while (capacity < needed)
    capacity *= 2;
  if ((edges = realloc(map->edges, capacity * sizeof(*edges))) == NULL)
    return (false);
  memset(&edges[map->edge_capacity], 0,
	 (capacity - map->edge_capacity) * sizeof(*edges));
  map->edges = edges;
  map->edge_capacity = capacity;
  return (true);
}

static ssize_t		find_node(t_function_map		*map,
				  const char			*name)
{
  size_t		i;

  if (name == NULL || name[0] == '\0')
    return (-1);
  for (i = 0; i < map->nbr_nodes; ++i)
    if (map->nodes[i].name && strcmp(map->nodes[i].name, name) == 0)
      return ((ssize_t)i);
  return (-1);
}

static ssize_t		get_node(t_function_map		*map,
				 const char			*name)
{
  ssize_t		idx;

  if ((idx = find_node(map, name)) >= 0)
    return (idx);
  if (name == NULL || name[0] == '\0')
    return (-1);
  if (!reserve_nodes(map, map->nbr_nodes + 1))
    return (-1);
  if ((map->nodes[map->nbr_nodes].name = map_strdup(name)) == NULL)
    return (-1);
  map->nodes[map->nbr_nodes].defined = false;
  map->nbr_nodes += 1;
  return ((ssize_t)(map->nbr_nodes - 1));
}

static t_function_map_edge *get_edge(t_function_map	*map,
				     size_t		from,
				     size_t		to)
{
  size_t		i;

  for (i = 0; i < map->nbr_edges; ++i)
    if (map->edges[i].from == from && map->edges[i].to == to)
      return (&map->edges[i]);
  if (!reserve_edges(map, map->nbr_edges + 1))
    return (NULL);
  map->edges[map->nbr_edges].from = from;
  map->edges[map->nbr_edges].to = to;
  map->edges[map->nbr_edges].direct_call_count = 0;
  map->edges[map->nbr_edges].pointer_donation_count = 0;
  map->nbr_edges += 1;
  return (&map->edges[map->nbr_edges - 1]);
}

void			crawler_function_map_clear(t_parsing		*p)
{
  size_t		i;
  bool			enabled;

  if (p == NULL)
    return ;
  enabled = p->function_map.enabled;
  for (i = 0; i < p->function_map.nbr_nodes; ++i)
    free(p->function_map.nodes[i].name);
  free(p->function_map.nodes);
  free(p->function_map.edges);
  memset(&p->function_map, 0, sizeof(p->function_map));
  p->function_map.enabled = enabled;
}

void			crawler_function_map_enable(t_parsing		*p,
					    bool			enabled)
{
  if (p == NULL)
    return ;
  crawler_function_map_clear(p);
  p->function_map.enabled = enabled;
}

void			crawler_function_map_define(t_parsing		*p,
					   const char		*function)
{
  ssize_t		idx;

  if (p == NULL || !p->function_map.enabled || function == NULL || function[0] == '\0')
    return ;
  if ((idx = get_node(&p->function_map, function)) < 0)
    return ;
  p->function_map.nodes[idx].defined = true;
}

bool			crawler_function_map_has_function(t_parsing	*p,
						 const char		*function)
{
  ssize_t		idx;

  if (p == NULL || function == NULL || function[0] == '\0')
    return (false);
  idx = find_node(&p->function_map, function);
  return (idx >= 0 && p->function_map.nodes[idx].defined);
}

void			crawler_function_map_enter(t_parsing		*p,
					  const char		*function,
					  char			previous[SYMBOL_SIZE + 1])
{
  if (previous)
    previous[0] = '\0';
  if (p == NULL || !p->function_map.enabled || function == NULL || function[0] == '\0')
    return ;
  if (previous)
    map_copy_symbol(previous, p->function_map.current_function, SYMBOL_SIZE + 1);
  crawler_function_map_define(p, function);
  map_copy_symbol(p->function_map.current_function, function,
		  sizeof(p->function_map.current_function));
}

void			crawler_function_map_leave(t_parsing		*p,
					  const char		previous[SYMBOL_SIZE + 1])
{
  if (p == NULL || !p->function_map.enabled)
    return ;
  if (previous == NULL)
    p->function_map.current_function[0] = '\0';
  else
    map_copy_symbol(p->function_map.current_function, previous,
		    sizeof(p->function_map.current_function));
}

void			crawler_function_map_add_call(t_parsing		*p,
					      const char		*caller,
					      const char		*callee)
{
  ssize_t		from;
  ssize_t		to;
  t_function_map_edge	*edge;

  if (p == NULL || !p->function_map.enabled || p->function_map.suppressed)
    return ;
  if (caller == NULL || caller[0] == '\0' || callee == NULL || callee[0] == '\0')
    return ;
  if ((from = get_node(&p->function_map, caller)) < 0)
    return ;
  if ((to = get_node(&p->function_map, callee)) < 0)
    return ;
  if ((edge = get_edge(&p->function_map, (size_t)from, (size_t)to)) == NULL)
    return ;
  edge->direct_call_count += 1;
}

void			crawler_function_map_add_pointer_donation(t_parsing	*p,
						      const char	*receiver,
						      const char	*donated)
{
  ssize_t		from;
  ssize_t		to;
  t_function_map_edge	*edge;

  if (p == NULL || !p->function_map.enabled || p->function_map.suppressed)
    return ;
  if (receiver == NULL || receiver[0] == '\0' || donated == NULL || donated[0] == '\0')
    return ;
  if ((from = get_node(&p->function_map, receiver)) < 0)
    return ;
  if ((to = get_node(&p->function_map, donated)) < 0)
    return ;
  if ((edge = get_edge(&p->function_map, (size_t)from, (size_t)to)) == NULL)
    return ;
  edge->pointer_donation_count += 1;
}

static void		dot_string(FILE			*out,
				   const char			*str)
{
  fputc('"', out);
  for (; str && *str; ++str)
    {
      if (*str == '"' || *str == '\\')
	fputc('\\', out);
      if (*str == '\n')
	fputs("\\n", out);
      else
	fputc(*str, out);
    }
  fputc('"', out);
}

static void		write_node(FILE			*out,
				   t_function_map_node	*node)
{
  fputs("  ", out);
  dot_string(out, node->name);
  if (node->defined)
    fputs(" [shape=box];\n", out);
  else
    fputs(" [shape=ellipse, style=dashed, label=", out), dot_string(out, node->name), fputs("];\n", out);
}

static void		write_direct_edge(FILE			*out,
					  t_function_map	*map,
					  t_function_map_edge	*edge)
{
  bool			recursive;

  recursive = edge->from == edge->to;
  fputs("  ", out);
  dot_string(out, map->nodes[edge->from].name);
  fputs(" -> ", out);
  dot_string(out, map->nodes[edge->to].name);
  fprintf(out, " [label=\"call:%d\", style=solid", edge->direct_call_count);
  if (recursive)
    fputs(", penwidth=2", out);
  if (!map->nodes[edge->to].defined)
    fputs(", arrowhead=vee", out);
  fputs("];\n", out);
}

static void		write_pointer_edge(FILE			*out,
					   t_function_map	*map,
					   t_function_map_edge	*edge)
{
  fputs("  ", out);
  dot_string(out, map->nodes[edge->from].name);
  fputs(" -> ", out);
  dot_string(out, map->nodes[edge->to].name);
  fprintf(out, " [label=\"funcptr:%d\", style=dashed, arrowhead=onormal];\n",
	  edge->pointer_donation_count);
}

bool			crawler_function_map_write_dot(t_parsing		*p,
					       const char		*output)
{
  FILE			*out;
  size_t		i;

  if (p == NULL || output == NULL || output[0] == '\0')
    return (false);
  if ((out = fopen(output, "w")) == NULL)
    return (false);
  fputs("digraph crawler_function_map {\n", out);
  fputs("  rankdir=LR;\n", out);
  fputs("  node [fontname=\"monospace\"];\n", out);
  fputs("  edge [fontname=\"monospace\"];\n", out);
  fputs("  // box: defined in parsed files; dashed ellipse: external/unresolved.\n", out);
  fputs("  // solid edge: direct call; dashed open edge: function pointer donation / potential callback.\n", out);
  for (i = 0; i < p->function_map.nbr_nodes; ++i)
    write_node(out, &p->function_map.nodes[i]);
  for (i = 0; i < p->function_map.nbr_edges; ++i)
    {
      if (p->function_map.edges[i].direct_call_count > 0)
	write_direct_edge(out, &p->function_map, &p->function_map.edges[i]);
      if (p->function_map.edges[i].pointer_donation_count > 0)
	write_pointer_edge(out, &p->function_map, &p->function_map.edges[i]);
    }
  fputs("}\n", out);
  if (fclose(out) != 0)
    return (false);
  return (true);
}
