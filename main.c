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
  int nivel;
  Similaridade *similar;
} Filme;

typedef struct Lista {
  Filme **filmes;
  int tamanho;
} Lista;

typedef struct Recomendacao {
  Filme **filmes;
} Recomendacao;

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

Filme *criar_filme(char nome[]) {
  Filme *filme = malloc(sizeof(Filme));
  if (!filme) exit(1);
  filme->cor = BRANCO;
  filme->identificador = -1;
  strcpy(filme->nome, nome);
  filme->nivel = 0;
  filme->similar = NULL;
  return filme;
}

void destruir_filme(Filme *filme) {
  if (filme == NULL)
    return;

  destruir_similaridades(filme->similar);
  filme->similar = NULL;

  free(filme);
  filme = NULL;
}

void printar_similares(Filme *filme) {
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
    printf("[%d] - %s - %d\n", filme->identificador, filme->nome, filme->nivel);
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
    recomendacao->filmes[i] = criar_filme(nomes_filmes[i]);

  return recomendacao;
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

  similaridade = criar_similaridade(posicao_para);
  filme = recomendacao->filmes[posicao_de];

  // Verificar se já está na lista de similares
  cursor = filme->similar;
  while (cursor != NULL) {
    if (cursor->posicao == similaridade->posicao) return;
    cursor = cursor->proximo;
  }

  similaridade->proximo = filme->similar;
  filme->similar = similaridade;

  // ----------------- VOLTA ------------------------

  similaridade = criar_similaridade(posicao_de);
  filme = recomendacao->filmes[posicao_para];

  // Verificar se já está na lista de similares
  cursor = filme->similar;
  while (cursor != NULL) {
    if (cursor->posicao == similaridade->posicao) return;
    cursor = cursor->proximo;
  }

  similaridade->proximo = filme->similar;
  filme->similar = similaridade;
}

void gerar_similaridades(Recomendacao *recomendacao) {
  int posicao_origem = 0, posicao_destino = 0;

  for (int i = 0; i < NUMERO_FILMES; i++) {
    posicao_origem = rand() % NUMERO_FILMES;
    posicao_destino = rand() % NUMERO_FILMES;

    adicionar_similaridade(recomendacao, posicao_origem, posicao_destino);
  }
}

void buscar_similaridades_interno(Recomendacao *recomendacao, Lista *lista, int posicao, int nivel, int posicao_anterior) {
  Filme *filme = recomendacao->filmes[posicao];
  
  // Nó descoberto
  filme->cor = CINZA;
  
  filme->identificador = posicao;
  filme->nivel = nivel;

  printf("%-10d %-50s Nivel: %-10d Anterior: %-10d\n", posicao, filme->nome, nivel, posicao_anterior);
  inserir_lista(lista, filme);

  Similaridade *cursor = filme->similar;
  while (cursor != NULL) {
    int posicao_similar = cursor->posicao;
    Filme *filme_similar = recomendacao->filmes[posicao_similar];

    if (filme_similar->cor == BRANCO)
      buscar_similaridades_interno(recomendacao, lista, posicao_similar, nivel + 1, posicao);

    cursor = cursor->proximo;
  }

  // Nó finalizado
  filme->cor = PRETO;
}

int comparar_por_nivel(const void *a, const void *b) {
  // Cast dos ponteiros para ponteiros para Filme
  const Filme *filme_a = *(const Filme **)a;
  const Filme *filme_b = *(const Filme **)b;

  // Compara os níveis
  if (filme_a->nivel < filme_b->nivel) return -1;
  if (filme_a->nivel > filme_b->nivel) return 1;
  return 0;
}

void buscar_similaridades(Recomendacao *recomendacao, int posicao_inicial) {
  Lista *lista_filmes = criar_lista();
  int nivel = 0;

  // Reseta os nós para a busca funcionar mais uma vez
  Filme *cursor = NULL;
  for (int i = 0; i < NUMERO_FILMES; i++) {
    cursor = recomendacao->filmes[i];
    
    cursor->cor = BRANCO;
    cursor->nivel = 0;
  }

  puts("---------- Buscando todos os similares... ----------\n");
  printf("Filme inicial: %d\n\n", posicao_inicial);
  buscar_similaridades_interno(recomendacao, lista_filmes, posicao_inicial, nivel, -1);
  printf("\n");

  // Ordena as listas por nível
  qsort(lista_filmes->filmes, lista_filmes->tamanho, sizeof(Filme *), comparar_por_nivel);
  
  exibir_lista(lista_filmes);
  destruir_lista(lista_filmes);
}

int main(void) {
  srand(time(NULL));

  Recomendacao *recomendacao = criar_recomendacao();
  int posicao_inicial = rand() % NUMERO_FILMES;

  gerar_similaridades(recomendacao);

  puts("\n---------- Exibindo os similares de cada filme... ----------\n");
  for (int i = 0; i < NUMERO_FILMES; i++) {
    printf("[%d]: ", i);
    printar_similares(recomendacao->filmes[i]);
  }

  buscar_similaridades(recomendacao, posicao_inicial);

  destruir_recomendacao(recomendacao);
}