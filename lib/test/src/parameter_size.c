/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2022
** EFRITS SAS 2021-2024
** Pentacle Technologie 2008-2024
**
** TechnoCore
*/

#include		"test.h"

int			main(int		argc,
			     char		**argv)
{
  TEST_INTRO(); // LCOV_EXCL_LINE

  //// Test du passage de paramètre par copie ou par reference
  // On commence par verifier que le compteur de taille de structure est fonctionnel
  // Il est possible que certains types natifs aient été oubliés
  p.last_new_type = 0;
  i = 0;
  s = "typedef int Integer;\n";
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(strcmp(p.new_type[0].name, "Integer") == 0);
  assert(p.new_type[0].size == 4);

  i = 0;
  s = "typedef int * IntPointer;\n";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(strcmp(p.new_type[1].name, "IntPointer") == 0);
  assert(p.new_type[1].size == 8);

  i = 0;
  s =
    "typedef struct s_8 {\n"
    "  int a;\n"
    "  Integer b;\n"
    "} t_8;\n"
    "\n"
    "typedef struct s_24 {\n"
    "  struct s_8 c;\n"
    "  t_8 d;\n"
    "  void *e;\n"
    "} t_24;\n"
    ;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(strcmp(p.new_type[2].name, "s_8") == 0);
  assert(p.new_type[2].size == 8);
  assert(strcmp(p.new_type[3].name, "t_8") == 0);
  assert(p.new_type[3].size == 8);
  assert(strcmp(p.new_type[4].name, "s_24") == 0);
  assert(p.new_type[4].size == 24);
  assert(strcmp(p.new_type[5].name, "t_24") == 0);
  assert(p.new_type[5].size == 24);

  // On ne reset pas le nombre de type afin de profiter de la config au dessus
  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 16;
  p.only_by_reference.counter = 0;
  // Les prototypes ne sont pas censés être vérifiés
  s = "void func(t_24 copy);";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.only_by_reference.counter == 0);

  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 32;
  p.only_by_reference.counter = 0;
  // On est dans les clous, car t_24 est plus petit que 32
  s = "void func(t_24 copy) { return ; }";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.only_by_reference.counter == 0);

  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 16;
  p.only_by_reference.counter = 0;
  // On est dans l'illegalité car 16 est maintenant le max
  s = "void func(t_24 copy) { return ; }";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.only_by_reference.counter == 1);

  i = 0;
  p.last_error_id = -1;
  // On limite a 32 octets la taille d'un paramètre par copie
  p.only_by_reference.active = true;
  p.only_by_reference.value = 16;
  p.only_by_reference.counter = 0;
  // On est de nouveau dans la légalité car maintenant on passe par reference
  s = "void func(t_24 *copy) { return ; }";
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR(); // LCOV_EXCL_LINE
  assert(p.only_by_reference.counter == 0);
  
  TEST_OUTRO(); // LCOV_EXCL_LINE
}
