#include "calls.h"

int main(void)
{
    // Faux bruit : fake_call(); a(); b(); c();
    const char *noise = "this string contains aa(); ab(); bb();";

    (void)noise;

    a();

    b();

    c();
    c();

    return 0;
}

