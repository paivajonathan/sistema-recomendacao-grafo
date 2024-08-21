#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUMERO_FILMES 20
#define TAMANHO_NOME 256
#define MAXIMO_LOOPS 3

char nomes_filmes[NUMERO_FILMES][TAMANHO_NOME] = {
  "O Poderoso Chefao",
  "Um Sonho de Liberdade",
  "A Lista de Schindler",
  "Forrest Gump - O Contador de Historias",
  "O Rei Leao",
  "O Senhor dos Aneis - O Retorno do Rei",
  "A Espera de um Milagre",
  "Batman - O Cavaleiro Das Trevas",
  "A Vida e Bela",
  "Vingadores: Ultimato",
  "O Poderoso Chefao 2",
  "Vingadores: Guerra Infinita",
  "O Auto da Compadecida",
  "Viva - A Vida e uma Festa",
  "De Volta para o Futuro",
  "O Resgate do Soldado Ryan",
  "Gladiador",
  "Django Livre",
  "O Senhor dos Aneis - A Sociedade do Anel",
  "O Silencio dos Inocentes"
};

typedef enum Cor {
  BRANCO = 1,
  CINZA = 2,
  PRETO = 3
} Cor;

typedef struct Similaridade {
  int posicao;
  struct Similaridade *proximo;
} Similaridade;

typedef struct Filme {
  Cor cor;
  int identificador;
  char nome[TAMANHO_NOME];
  int distancia;
  Similaridade *similar;
} Filme;

typedef struct Lista {
  Filme **filmes;
  int tamanho;
} Lista;

typedef struct Recomendacao {
  Filme **filmes;
} Recomendacao;

Similaridade *criar_similaridade(int posicao);
void destruir_similaridades(Similaridade *inicial);

Filme *criar_filme(int identificador, char nome[]);
void exibir_similares(Filme *filme);
void destruir_filme(Filme *filme);

Lista *criar_lista(void);
void inserir_lista(Lista *lista, Filme *filme);
void exibir_lista(Lista *lista);
void destruir_lista(Lista *lista);

Recomendacao *criar_recomendacao(void);
void exibir_filmes_similares(Recomendacao *recomendacao);
void resetar_recomendacao(Recomendacao *recomendacao);
void destruir_recomendacao(Recomendacao *recomendacao);

void adicionar_similaridade(Recomendacao *recomendacao, int posicao_de, int posicao_para);
void gerar_similaridades(Recomendacao *recomendacao);

int comparar_por_distancia(const void *a, const void *b);
void buscar_similaridades_interno(Recomendacao *recomendacao, Lista *lista, int posicao, int distancia, int posicao_anterior);
void buscar_similaridades(Recomendacao *recomendacao, int posicao_inicial);

int main(void) {
  srand(time(NULL));

  Recomendacao *recomendacao = criar_recomendacao();
  int posicao_inicial = rand() % NUMERO_FILMES;

  gerar_similaridades(recomendacao);
  exibir_filmes_similares(recomendacao);

  buscar_similaridades(recomendacao, posicao_inicial);

  destruir_recomendacao(recomendacao);
}

Similaridade *criar_similaridade(int posicao) {
  Similaridade *similaridade = malloc(sizeof(Similaridade));
  if (!similaridade) exit(1);
  similaridade->posicao = posicao;
  similaridade->proximo = NULL;
  return similaridade;
}

void destruir_similaridades(Similaridade *inicial) {
  Similaridade *atual = inicial;
  Similaridade *proximo = NULL;

  while (atual != NULL) {
    proximo = atual->proximo;
    free(atual);
    atual = proximo;
  }
}

Filme *criar_filme(int identificador, char nome[]) {
  Filme *filme = malloc(sizeof(Filme));
  if (!filme) exit(1);

  filme->identificador = identificador;
  strcpy(filme->nome, nome);

  filme->cor = BRANCO;
  filme->distancia = 0;

  filme->similar = NULL;
  return filme;
}

void exibir_similares(Filme *filme) {
  int contador = 0;

  printf("%s\n", filme->nome);

  Similaridade *cursor = filme->similar;

  if (cursor == NULL) {
    puts("Nao possui similares.\n");
    return;
  }

  while (cursor != NULL) {
    printf("%d\n", cursor->posicao);
    cursor = cursor->proximo;
    contador++;
  }

  printf("\n");
}

void destruir_filme(Filme *filme) {
  if (filme == NULL)
    return;

  destruir_similaridades(filme->similar);
  filme->similar = NULL;

  free(filme);
  filme = NULL;
}

Lista *criar_lista(void) {
  Lista *lista = malloc(sizeof(Lista));
  if (!lista) exit(1);
  lista->tamanho = 0;
  lista->filmes = NULL;
  return lista;
}

void inserir_lista(Lista *lista, Filme *filme) {
  if (!lista->tamanho)
    lista->filmes = malloc(sizeof(Filme *));
  else
    lista->filmes = realloc(lista->filmes, sizeof(Filme *) * (lista->tamanho + 1));

  lista->filmes[lista->tamanho++] = filme;
}

void exibir_lista(Lista *lista) {
  Filme *filme = NULL;

  filme = lista->filmes[0];
  printf("---------- Exibindo similares do filme \"[%d] - %s\": ----------\n\n", filme->identificador, filme->nome);

  // Existe apenas o filme inicial
  if (lista->tamanho == 1) {
    printf("Nao ha filmes similares.\n");
    return;
  }

  for (int i = 1; i < lista->tamanho; i++) {
    filme = lista->filmes[i];
    printf("[%d] - %s - %d\n", filme->identificador, filme->nome, filme->distancia);
  }
}

void destruir_lista(Lista *lista) {
  if (lista == NULL) return;

  free(lista->filmes);
  lista->filmes = NULL;

  free(lista);
  lista = NULL;
}

Recomendacao *criar_recomendacao(void) {
  Recomendacao *recomendacao = malloc(sizeof(Recomendacao));
  if (!recomendacao) exit(1);

  recomendacao->filmes = malloc(sizeof(Filme *) * NUMERO_FILMES);
  if (!recomendacao->filmes) exit(1);

  for (int i = 0; i < NUMERO_FILMES; i++)
    recomendacao->filmes[i] = criar_filme(i, nomes_filmes[i]);

  return recomendacao;
}

void exibir_filmes_similares(Recomendacao *recomendacao) {
  puts("\n---------- Exibindo os similares de cada filme... ----------\n");
  for (int i = 0; i < NUMERO_FILMES; i++) {
    printf("[%d]: ", i);
    exibir_similares(recomendacao->filmes[i]);
  }
}

void resetar_recomendacao(Recomendacao *recomendacao) {
  Filme *filme = NULL;
  
  for (int i = 0; i < NUMERO_FILMES; i++) {
    filme = recomendacao->filmes[i];

    filme->cor = BRANCO;
    filme->distancia = 0;
  }
}

void destruir_recomendacao(Recomendacao *recomendacao) {
  if (recomendacao == NULL) return;

  for (int i = 0; i < NUMERO_FILMES; i++)
    destruir_filme(recomendacao->filmes[i]);

  free(recomendacao->filmes);
  recomendacao->filmes = NULL;

  free(recomendacao);
  recomendacao = NULL;
}

void adicionar_similaridade(Recomendacao *recomendacao, int posicao_de, int posicao_para) {
  Similaridade *similaridade = NULL;
  Similaridade *cursor = NULL;
  Filme *filme = NULL;

  // ----------------- IDA ------------------------

  filme = recomendacao->filmes[posicao_de];

  // Verificar se posição já está na lista de similares
  cursor = filme->similar;
  while (cursor != NULL) {
    if (cursor->posicao == posicao_para)
      return;
    cursor = cursor->proximo;
  }

  similaridade = criar_similaridade(posicao_para);
  similaridade->proximo = filme->similar;
  filme->similar = similaridade;

  // ----------------- VOLTA ------------------------

  filme = recomendacao->filmes[posicao_para];

  // Verificar se posição já está na lista de similares
  cursor = filme->similar;
  while (cursor != NULL) {
    if (cursor->posicao == posicao_de)
      return;
    cursor = cursor->proximo;
  }

  similaridade = criar_similaridade(posicao_de);
  similaridade->proximo = filme->similar;
  filme->similar = similaridade;
}

void gerar_similaridades(Recomendacao *recomendacao) {
  int posicao_origem = 0, posicao_destino = 0;

  // São realizadas NUMERO_FILMES = 20 tentativas de conexão entre os filmes.
  // Tentativas, por ser possível os mesmos números serem gerados mais de uma vez.
  for (int i = 0; i < NUMERO_FILMES; i++) {
    posicao_origem = rand() % NUMERO_FILMES;  // 0 a 19
    posicao_destino = rand() % NUMERO_FILMES;  // 0 a 19

    adicionar_similaridade(recomendacao, posicao_origem, posicao_destino);
  }
}

int comparar_por_distancia(const void *a, const void *b) {
  // Conversão dos ponteiros para ponteiros para Filme
  const Filme *filme_a = *(const Filme **)a;
  const Filme *filme_b = *(const Filme **)b;

  // Compara as distancias
  if (filme_a->distancia < filme_b->distancia) return -1;
  if (filme_a->distancia > filme_b->distancia) return 1;
  return 0;
}

void buscar_similaridades_interno(Recomendacao *recomendacao, Lista *lista_resultado, int posicao, int distancia, int posicao_anterior) {
  Filme *filme = recomendacao->filmes[posicao];

  // Nó descoberto
  filme->cor = CINZA;

  // Nível de distância pro nó inicial
  filme->distancia = distancia;

  printf("%-10d %-50s Distancia: %-10d Anterior: %-10d\n", posicao, filme->nome, distancia, posicao_anterior);
  
  // Insere na lista para o resultado final da recomendação
  inserir_lista(lista_resultado, filme);

  // Buscar todos os vizinhos do nó atual
  Similaridade *cursor = filme->similar;
  while (cursor != NULL) {
    int posicao_similar = cursor->posicao;
    Filme *filme_similar = recomendacao->filmes[posicao_similar];

    if (filme_similar->cor == BRANCO)
      buscar_similaridades_interno(recomendacao, lista_resultado, posicao_similar, distancia + 1, posicao);

    cursor = cursor->proximo;
  }

  // Nó finalizado
  filme->cor = PRETO;
}

void buscar_similaridades(Recomendacao *recomendacao, int posicao_inicial) {
  Lista *lista_resultado = criar_lista();
  int distancia = 0;

  // Reseta os nós para a busca funcionar mais uma vez
  resetar_recomendacao(recomendacao);

  puts("---------- Buscando todos os similares... ----------\n");
  printf("Filme inicial: %d\n\n", posicao_inicial);
  
  buscar_similaridades_interno(recomendacao, lista_resultado, posicao_inicial, distancia, -1);
  
  printf("\n");

  // Ordena a lista do resultado pela distância dos filmes para o filme escolhido.
  // Quanto menor o seu valor, mais próxima é a similaridade.
  qsort(lista_resultado->filmes, lista_resultado->tamanho, sizeof(Filme *), comparar_por_distancia);
  exibir_lista(lista_resultado);

  destruir_lista(lista_resultado);
}
