/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"crawler.h"

bool			check_last_parameter_is_reference(t_parsing	*p,
							  const char	*code,
							  int		pos)
{
  static const char	*kw[] =
    {
      "double", "void", "char", "short", "int", "long", "float", "unsigned", "signed",
      "size_t", "ssize_t", "ptrdiff_t", "off_t", "int8_t", "int16_", "int32_t",
      "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t"
    };

  (void)code;
  (void)pos;
  if (p->only_by_reference.active == false)
    return (true);
  if (p->last_declaration.nbr_pointer != 0)
    return (true);
  for (size_t i = 0; i < NBRCELL(kw); ++i)
    if (strcmp(p->last_declaration.last_type, kw[i]) == 0)
      return (true);
  for (size_t i = 0; i < p->last_new_type; ++i)
    if (strcmp(p->last_declaration.last_type, p->new_type[i].name) == 0)
      {
	if (p->new_type[i].size > p->only_by_reference.value
	    && p->last_declaration.ptr_acc == 0)
	  {
	    strcpy(p->last_declaration.copied_parameters
		   [p->last_declaration.nbr_copied_parameters].name,
		   p->last_declaration.last_type);
	    p->last_declaration.copied_parameters
	      [p->last_declaration.nbr_copied_parameters].size =
	      p->new_type[i].size;
	    p->last_declaration.nbr_copied_parameters += 1;
	  }
	return (true);
      }
  /*

  */
  return (true);
}

