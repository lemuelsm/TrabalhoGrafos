#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <stdbool.h>

#define INF DBL_MAX

// Estrutura de um grafo
typedef struct {
    int origem;
    int destino;
    double peso;
} Aresta;

// Função para encontrar o vértice com a menor distância
int menorDistancia(double dist[], int visitado[], int V) {
    double min = INF;
    int indice_minimo = -1;

    for (int v = 0; v < V; v++) {
        if (visitado[v] == 0 && dist[v] <= min) {
            min = dist[v];
            indice_minimo = v;
        }
    }
    return indice_minimo;
}

// Função para mostrar o grafo de caminhos mínimos
void mostrarCaminhosMinimos(double dist[], int pred[], int V, FILE* arquivo) {
    printf("Vertice\t\tDistancia da origem\tPredecessor\n");
    fprintf(arquivo, "Vértice\t\tDistancia da origem\tPredecessor\n");
    for (int i = 0; i < V; i++) {
        if (dist[i] == INF) {
            printf("%d\t\tINF\t\t\t-\n", i);
            fprintf(arquivo, "%d\t\tINF\t\t\t-\n", i);
        } else {
            printf("%d\t\t%.2f\t\t\t%d\n", i, dist[i], pred[i]);
            fprintf(arquivo, "%d\t\t%.2f\t\t\t%d\n", i, dist[i], pred[i]);
        }
    }
}

// Implementação do algoritmo de Dijkstra
void dijkstra(Aresta *arestas, int V, int A, int origem, const char *nomeArquivoSaida) {
    double dist[V];   // Armazena as distâncias mínimas
    int visitado[V];  // Armazena quais vértices já foram visitados
    int pred[V];      // Armazena os predecessores de cada vértice

    // Inicializar distâncias como infinito, vértices como não visitados e predecessores como -1
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        visitado[i] = 0;
        pred[i] = -1;
    }

    // A distância do vértice de origem para ele mesmo é sempre 0
    dist[origem] = 0.0;

    // Iterar sobre todos os vértices
    for (int i = 0; i < V - 1; i++) {
        // Escolher o vértice com a menor distância
        int u = menorDistancia(dist, visitado, V);
        visitado[u] = 1;

        // Atualizar as distâncias
        for (int j = 0; j < A; j++) {
            if (arestas[j].origem == u && !visitado[arestas[j].destino] && dist[u] != INF &&
                dist[u] + arestas[j].peso < dist[arestas[j].destino]) {
                dist[arestas[j].destino] = dist[u] + arestas[j].peso;
                pred[arestas[j].destino] = u;
            }
        }
    }

    // Abrir arquivo para escrever a saída
    FILE *arquivoSaida = fopen(nomeArquivoSaida, "w");
    if (arquivoSaida == NULL) {
        printf("Erro ao criar o arquivo de saida!\n");
        return;
    }

    // Mostrar os caminhos mínimos no terminal e no arquivo
    mostrarCaminhosMinimos(dist, pred, V, arquivoSaida);
    fclose(arquivoSaida);
}

// Função para verificar se uma aresta já existe
bool arestaJaExiste(int origem, int destino, int A, int matrizArestas[A][2]) {
    for (int i = 0; i < A; i++) {
        if (matrizArestas[i][0] == origem && matrizArestas[i][1] == destino) {
            return true;
        }
    }
    return false;
}

// Geração de grafo simples e direcionado
void gerarGrafoAleatorio(int V, int A, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo!\n");
        return;
    }

    // Verificar se o número de arestas não excede n(n-1)
    if (A > V * (V - 1)) {
        printf("O numero maximo de arestas com %d vertices eh %d. O numero de arestas sera %d\n", V, V * (V - 1), V * (V - 1));
        A = V * (V - 1);
    }

    fprintf(arquivo, "%d\n%d\n", V, A);

    int matrizArestas[A][2]; // Matriz para armazenar as arestas geradas
    int count = 0;

    srand(time(NULL));

    while (count < A) {

        int origem = rand() % V;
        int destino = rand() % V;
        double peso = (double)rand() / RAND_MAX * 100.0;

        // Verifica se a aresta já existe ou se é um laço
        if (!arestaJaExiste(origem, destino, count, matrizArestas) && origem != destino) {
            matrizArestas[count][0] = origem;
            matrizArestas[count][1] = destino;

            fprintf(arquivo, "%d %d %.2f\n", origem, destino, peso);
            count++;
        }
    }

    fclose(arquivo);
}

// Função para ler o grafo de um arquivo de arcos
Aresta* lerGrafo(const char *nome_arquivo, int *V, int *A) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fscanf(arquivo, "%d", V);
    fscanf(arquivo, "%d", A);

    Aresta *arestas = (Aresta*)malloc((*A) * sizeof(Aresta));

    for (int i = 0; i < *A; i++) {
        fscanf(arquivo, "%d %d %lf", &arestas[i].origem, &arestas[i].destino, &arestas[i].peso);
    }

    fclose(arquivo);
    return arestas;
}

int main() {
    int V, A;
    char nome_arquivo[] = "grafo.txt";
    char nome_arquivo_saida[] = "grafoCaminhosMinimos.txt";

    // Pedir ao usuário para inserir o número de vértices e arestas
    printf("Digite o numero de vertices: ");
    scanf("%d", &V);
    printf("Digite o numero de arestas: ");
    scanf("%d", &A);

    // Gerar um grafo aleatório e salvar no arquivo
    gerarGrafoAleatorio(V, A, nome_arquivo);

    // Ler o grafo do arquivo
    Aresta *arestas = lerGrafo(nome_arquivo, &V, &A);

    // Medir o tempo de execução
    clock_t inicio = clock();

    // Executar o algoritmo de Dijkstra
    dijkstra(arestas, V, A, 0, nome_arquivo_saida);

    // Medir o tempo de término
    clock_t fim = clock();
    double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("Tempo de execucao: %.6f segundos\n", tempo_execucao);

    free(arestas);
    return 0;
}
