#include <stdio.h>

int main(int argc, char **argv, char **envp)
{
    int i = 0;

    // Boucle à travers chaque variable d'environnement
    while (envp[i] != NULL) {
        printf("%s\n", envp[i]);
        i++;
    }

    return 0;
}
