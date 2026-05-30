/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
** EFRITS SAS 2022-2026
**
** Function call graph support for crawler -m.
*/

#ifndef			__FUNCGRAPH_H__
# define		__FUNCGRAPH_H__
# include		<stdbool.h>
# include		<stddef.h>

# define		FUNCGRAPH_SYMBOL_SIZE		127

typedef struct		s_parsing			t_parsing;

typedef enum		e_function_map_edge_kind
  {
    FUNCTION_MAP_DIRECT_CALL,
    FUNCTION_MAP_POINTER_DONATION
  }			t_function_map_edge_kind;

typedef struct		s_function_map_node
{
  char			*name;
  bool			defined;
} 			t_function_map_node;

typedef struct		s_function_map_edge
{
  size_t		from;
  size_t		to;
  int			direct_call_count;
  int			pointer_donation_count;
} 			t_function_map_edge;

typedef struct		s_function_map
{
  bool			enabled;
  int			suppressed;
  char			current_function[FUNCGRAPH_SYMBOL_SIZE + 1];
  char			current_call_receiver[FUNCGRAPH_SYMBOL_SIZE + 1];
  t_function_map_node	*nodes;
  size_t		nbr_nodes;
  size_t		node_capacity;
  t_function_map_edge	*edges;
  size_t		nbr_edges;
  size_t		edge_capacity;
} 			t_function_map;

void			crawler_function_map_enable(t_parsing		*parsing,
					    bool		enabled);
void			crawler_function_map_clear(t_parsing		*parsing);
void			crawler_function_map_define(t_parsing		*parsing,
					   const char		*function);
void			crawler_function_map_enter(t_parsing		*parsing,
					  const char		*function,
					  char			previous[FUNCGRAPH_SYMBOL_SIZE + 1]);
void			crawler_function_map_leave(t_parsing		*parsing,
					  const char		previous[FUNCGRAPH_SYMBOL_SIZE + 1]);
void			crawler_function_map_add_call(t_parsing		*parsing,
					      const char	*caller,
					      const char	*callee);
void			crawler_function_map_add_pointer_donation(t_parsing	*parsing,
						      const char	*receiver,
						      const char	*donated);
bool			crawler_function_map_has_function(t_parsing		*parsing,
					 const char		*function);
bool			crawler_function_map_write_dot(t_parsing		*parsing,
					       const char	*output);

#endif	/*		__FUNCGRAPH_H__					*/
