#include "calls.h"

void a(void)
{
    // Bruit : on dirait un appel mais ce n'en est pas un.
    // bb();

    aa();

    ab();

    register_callback(&aa);
    register_callback(&ab);
}

void aa(void)
{
    int useless = 42;
    useless = useless + 1;

    // Aucun appel réel ici.
}

