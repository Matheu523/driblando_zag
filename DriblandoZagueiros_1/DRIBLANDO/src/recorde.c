#include <stdio.h>
#include "recorde.h"

#define ARQUIVO_RECORDE "recorde.dat"

int Recorde_Ler(void)
{
    FILE *f = fopen(ARQUIVO_RECORDE, "r");
    if (!f) return 0;
    int valor = 0;
    if (fscanf(f, "%d", &valor) != 1) valor = 0;
    fclose(f);
    return valor;
}

bool Recorde_Salvar(int pontos)
{
    if (pontos <= Recorde_Ler()) return false;
    FILE *f = fopen(ARQUIVO_RECORDE, "w");
    if (!f) return false;
    fprintf(f, "%d\n", pontos);
    fclose(f);
    return true;
}
