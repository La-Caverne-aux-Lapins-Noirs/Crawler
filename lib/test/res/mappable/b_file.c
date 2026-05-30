#include "calls.h"

void b(void)
{
    bb();
    bb();
    bb();

    b();
}

void bb(void)
{
    // Rien d'intéressant.
    int x = 0;
    x++;

    // Faux appel dans une chaîne :
    const char *fake = "c(); a(); ab();";
    (void)fake;
}

