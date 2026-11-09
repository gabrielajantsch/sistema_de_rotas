#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N 5 
#define INF 1000000
#define TAM_LINHA 100

// definindo os blocos
#define BLOCO_A       0
#define BLOCO_B       1
#define BIBLIOTECA    2
#define REFEITORIO    3
#define LABORATORIO   4


static const char *nomes[N] = {
"Bloco A", "Bloco B", "Bibiblioteca", "Refeitorio", "Laboratorio"
};
