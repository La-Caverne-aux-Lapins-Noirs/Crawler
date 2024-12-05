/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

/*
** Cette fonction verifie l'integralité des étoiles.
** Les étoiles situés a droite (avant le nom de variable)
** comme les étoiles placées juste après le premier type
** et les étoiles placés de facon intermediaire.
*/

bool			check_pointer_star_position(t_parsing	*p,
						    const char	*code,
						    int		pos,
						    int		nb_pointer)
{
  int			i = pos;
  int			n_pointer;
  bool			statement = false;

  // On commence sur le nom de la variable.

  if (p->inbetween_ptr_symbol_space.value == 0 &&
      p->ptr_symbol_on_name.value == 0 &&
      p->ptr_symbol_on_type.value == 0)
    return (true);

  // On se dégage du nom de la variable
  if (i > 0 && code[i - 1] != '\n' && code[i - 1] != '(' && code[i - 1] != ')' && code[i - 1] != ',')
    i -= 1;
  n_pointer = 0;
  // On continue jusqu'au début de la ligne ou la première parenthèse ouvrante (si on est dans
  // les paramètres) ou fermante (protection au cas ou l'on soit dans un pointeur sur fonction...)
  // de meme que virgule pour la fin du paramètre...
  while (i > 0 && code[i - 1] != '\n' && code[i - 1] != '(' && code[i - 1] != ')' && code[i - 1] != ',')
    {
      if (!isspace(code[i]) && code[i] != '*')
	statement = true;
      if (code[i] == '*')
	{
	  if (n_pointer >= nb_pointer || statement == true)
	    {
	      if (p->inbetween_ptr_symbol_space.value)
		if (check_one_space_around
		    (p, code, i, 1, true,
		     &p->inbetween_ptr_symbol_space.counter
		     ) == -1)
		  return (false);
	    }
	  else
	    {
	      if (p->ptr_symbol_on_name.value)
		if (!check_parenthesis_space(p, code, i, '(', &p->ptr_symbol_on_name.counter))
		  return (false);
	      if (p->ptr_symbol_on_type.value)
		if (!check_parenthesis_space(p, code, i, ')', &p->ptr_symbol_on_type.counter))
		  return (false);
	    }
	  n_pointer += 1;
	}
      i -= 1;
    }
  return (true);
}
