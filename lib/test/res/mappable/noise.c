#include "calls.h"

/*
   Ce fichier sert uniquement à polluer un peu l'analyse.
   Certaines fonctions ne sont jamais appelées depuis main.
*/

static void unused_helper(void)
{
    // Faux appel en commentaire : a();
}

static void dead_branch(void)
{
    if (0) {
        a();
        b();
        c();
    }
}

void completely_unused_entry_point(void)
{
    const char *s = "ab(); ab(); ab();";
    (void)s;

    unused_helper();

    // Branche morte mais syntaxiquement présente.
    dead_branch();
}

