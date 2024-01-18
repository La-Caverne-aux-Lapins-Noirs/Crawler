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
  TEST_INTRO();

  cnf = bunny_new_configuration();
  bunny_configuration_setf
    (cnf,
     "/*\n"
     "** %FirstName %FamilyName %Nickname %Login <%Mail>\n"
     "** %ProjectName %Year\n"
     "*/\n"
     , "Header");
  bunny_configuration_setf(cnf, "Felina", "FirstName");
  bunny_configuration_setf(cnf, "Rose", "FamilyName");
  bunny_configuration_setf(cnf, "Felinistra", "Nickname");
  bunny_configuration_setf(cnf, "felina.rose", "Login");
  bunny_configuration_setf(cnf, "felina.rose@cln.school", "Mail");
  bunny_configuration_setf(cnf, "CCCCrawler", "ProjectName");
  bunny_configuration_setf(cnf, "2021", "Year");
  load_norm_configuration(&p, cnf);

  i = 0;
  s =
    "/*\n"
    "** Felina Rose Felinistra felina.rose <felina.rose@cln.school>\n"
    "** CCCCrawler 2021\n"
    "*/\n"
    "\n"
    "int main(void)\n"
    "{\n"
    "  return (0);\n"
    "}\n"
    ;
  assert(check_header_file(&p, &s[0]));
  assert(p.header.counter == 0);
  p.last_line_marker = 0;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.header.counter == 0);

  i = 0;
  s =
    "\n"
    "typedef int lol;\n"
    "\n"
    "/*\n"
    "** Felina Rose Felinistra    felina.rose <felina.rose@cln.school>\n"
    "** CCCCrawler 2021\n"
    "*/\n"
    "\n"
    "#include <stdio.h>\n"
    "\n"
    "int main(void)\n"
    "{\n"
    "  return (0);\n"
    "}\n"
    ;
  if (read_translation_unit(&p, "file", s, &i, true, false) != 1)
    GOTOERROR();
  assert(p.header.counter == 1);
  
  TEST_OUTRO();
}
