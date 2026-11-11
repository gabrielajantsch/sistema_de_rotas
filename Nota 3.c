
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N 5            
#define INF 1000000    
#define TAM_LINHA 100

#define BLOCO_A      0
#define BLOCO_B      1
#define BIBLIOTECA   2
#define REFEITORIO   3
#define LABORATORIO  4

static const char *nomes[N] = {
    "Bloco A", "Bloco B", "Biblioteca", "Refeitorio", "Laboratorio"
};

static int grafo[N][N];

static int distFW[N][N];
static int alcanca[N][N];


static void adicionarAresta(int a, int b, int metros)
{
    grafo[a][b] = metros;
    grafo[b][a] = metros;   
}

static void inicializarGrafo(void)
{
    int i, j;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            grafo[i][j] = 0;

    adicionarAresta(BLOCO_B,    LABORATORIO, 240);
    adicionarAresta(LABORATORIO, BLOCO_A,    210);
    adicionarAresta(BLOCO_A,    REFEITORIO,  190);
    adicionarAresta(REFEITORIO, BIBLIOTECA,  280);
    adicionarAresta(BIBLIOTECA, BLOCO_A,     160);
}

static void lerLinha(char *buf, int tam)
{
    int len;

    if (fgets(buf, tam, stdin) == NULL) {
        printf("\nEntrada encerrada.\n");
        exit(0);
    }
    len = (int)strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r' ||
                       buf[len - 1] == ' ')) {
        buf[--len] = '\0';
    }
}

static int igualIgnorandoCaso(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static int soDigitos(const char *s)
{
    if (*s == '\0')
        return 0;
    while (*s) {
        if (!isdigit((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

static void listarPredios(void)
{
    int i;

    printf("\nPredios disponiveis:\n");
    for (i = 0; i < N; i++)
        printf("  %d - %s\n", i + 1, nomes[i]);
}

static int lerPredio(const char *mensagem)
{
    char buf[TAM_LINHA];
    int i, n;

    for (;;) {
        printf("%s", mensagem);
        lerLinha(buf, TAM_LINHA);

        if (buf[0] == '\0')
            continue;

        if (soDigitos(buf)) {
            n = atoi(buf);
            if (n >= 1 && n <= N)
                return n - 1;
        } else {
            for (i = 0; i < N; i++)
                if (igualIgnorandoCaso(buf, nomes[i]))
                    return i;
            if (igualIgnorandoCaso(buf, "Refeit\xC3\xB3rio") ||
                igualIgnorandoCaso(buf, "Refeit\xF3rio"))
                return REFEITORIO;
            if (igualIgnorandoCaso(buf, "Laborat\xC3\xB3rio") ||
                igualIgnorandoCaso(buf, "Laborat\xF3rio"))
                return LABORATORIO;
        }
        printf("Predio invalido! Digite o numero (1-%d) ou o nome.\n", N);
    }
}


static void dijkstra(int origem, int destino)
{
    int dist[N], ant[N], visitado[N];
    int caminho[N];
    int i, k, u, v, menor, tam;

    for (i = 0; i < N; i++) {
        dist[i] = INF;
        ant[i] = -1;
        visitado[i] = 0;
    }
    dist[origem] = 0;

    for (k = 0; k < N; k++) {
        u = -1;
        menor = INF;
        for (i = 0; i < N; i++) {
            if (!visitado[i] && dist[i] < menor) {
                menor = dist[i];
                u = i;
            }
        }
        if (u == -1)
            break;  
        visitado[u] = 1;

        for (v = 0; v < N; v++) {
            if (grafo[u][v] > 0 && !visitado[v] &&
                dist[u] + grafo[u][v] < dist[v]) {
                dist[v] = dist[u] + grafo[u][v];
                ant[v] = u;
            }
        }
    }

    if (dist[destino] == INF) {
        printf("\nNao existe caminho entre %s e %s.\n",
               nomes[origem], nomes[destino]);
        return;
    }

    tam = 0;
    for (v = destino; v != -1; v = ant[v])
        caminho[tam++] = v;

    printf("\nMenor rota: ");
    for (i = tam - 1; i >= 0; i--) {
        printf("%s", nomes[caminho[i]]);
        if (i > 0)
            printf(" -> ");
    }
    printf("\nDistancia total: %d m\n", dist[destino]);
}


static void floydWarshall(void)
{
    int i, j, k;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (i == j)
                distFW[i][j] = 0;
            else if (grafo[i][j] > 0)
                distFW[i][j] = grafo[i][j];
            else
                distFW[i][j] = INF;
        }
    }

    for (k = 0; k < N; k++)
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
                if (distFW[i][k] != INF && distFW[k][j] != INF &&
                    distFW[i][k] + distFW[k][j] < distFW[i][j])
                    distFW[i][j] = distFW[i][k] + distFW[k][j];

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            alcanca[i][j] = (distFW[i][j] != INF) ? 1 : 0;
}


static void exibirMapa(void)
{
    int i, j;

    printf("\n====== MAPA DO CAMPUS ======\n");
    for (i = 0; i < N; i++)
        for (j = i + 1; j < N; j++)
            if (grafo[i][j] > 0)
                printf("%s <-> %s : %d m\n",
                       nomes[i], nomes[j], grafo[i][j]);
    printf("============================\n");
}


static void menorRota(void)
{
    int origem, destino;

    listarPredios();
    origem = lerPredio("\nPredio de origem (numero ou nome): ");
    destino = lerPredio("Predio de destino (numero ou nome): ");
    dijkstra(origem, destino);
}


static void verificarConexao(void)
{
    int a, b;

    listarPredios();
    a = lerPredio("\nPrimeiro predio (numero ou nome): ");
    b = lerPredio("Segundo predio (numero ou nome): ");

    if (alcanca[a][b])
        printf("\n%s e %s estao conectados.\n", nomes[a], nomes[b]);
    else
        printf("\nNao existe caminho entre %s e %s.\n", nomes[a], nomes[b]);
}

static void exibirMatrizAlcancabilidade(void)
{
    int i, j;

    printf("\n====== MATRIZ DE ALCANCABILIDADE ======\n\n");
    printf("     ");
    for (j = 0; j < N; j++)
        printf("%d ", j + 1);
    printf("\n");
    for (i = 0; i < N; i++) {
        printf("%d    ", i + 1);
        for (j = 0; j < N; j++)
            printf("%d ", alcanca[i][j]);
        printf("\n");
    }
    printf("\nLegenda:\n");
    for (i = 0; i < N; i++)
        printf("  %d = %s\n", i + 1, nomes[i]);
    printf("\n1 - Existe caminho\n0 - Nao existe caminho\n");
    printf("=======================================\n");
}

static void exibirMenu(void)
{
    printf("\n=======================================\n");
    printf("        SISTEMA DE ROTAS DO CAMPUS\n");
    printf("=======================================\n\n");
    printf("1 - Exibir mapa do campus\n");
    printf("2 - Encontrar menor rota entre dois predios\n");
    printf("3 - Verificar conexao entre dois predios\n");
    printf("4 - Exibir matriz de alcancabilidade\n");
    printf("0 - Sair\n\n");
    printf("Escolha uma opcao: ");
}

int main(void)
{
    char buf[TAM_LINHA];
    int opcao;

    inicializarGrafo();
    floydWarshall();

    do {
        exibirMenu();
        lerLinha(buf, TAM_LINHA);
        opcao = soDigitos(buf) ? atoi(buf) : -1;

        switch (opcao) {
        case 1: exibirMapa(); break;
        case 2: menorRota(); break;
        case 3: verificarConexao(); break;
        case 4: exibirMatrizAlcancabilidade(); break;
        case 0: printf("\nEncerrando o programa.\n"); break;
        default: printf("\nOpcao invalida!\n"); break;
        }
    } while (opcao != 0);

    return 0;
}