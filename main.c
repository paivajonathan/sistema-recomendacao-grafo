#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUMERO_FILMES 20
#define TAMANHO_NOME 256

char nomes_filmes[NUMERO_FILMES][TAMANHO_NOME] = {
  "O Poderoso Chefão",
  "Um Sonho de Liberdade",
  "A Lista de Schindler",
  "Forrest Gump - O Contador de Histórias",
  "O Rei Leão",
  "O Senhor dos Anéis - O Retorno do Rei",
  "À Espera de um Milagre",
  "Batman - O Cavaleiro Das Trevas",
  "A Vida é Bela",
  "Vingadores: Ultimato",
  "O Poderoso Chefão 2",
  "Vingadores: Guerra Infinita",
  "O Auto da Compadecida",
  "Viva - A Vida é uma Festa",
  "De Volta para o Futuro",
  "O Resgate do Soldado Ryan",
  "Gladiador",
  "Django Livre",
  "O Senhor dos Anéis - A Sociedade do Anel",
  "O Silêncio dos Inocentes"
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

Similaridade *criar_similaridade(int posicao) {
  Similaridade *similaridade = malloc(sizeof(Similaridade));
  if (!similaridade) exit(1);
  similaridade->posicao = posicao;
  similaridade->proximo = NULL;
  return similaridade;
}

typedef struct Filme {
  char nome[TAMANHO_NOME];
  int nivel;
  Cor cor;
  Similaridade *similar;
} Filme;

Filme *criar_filme(char nome[]) {
  Filme *filme = malloc(sizeof(Filme));
  if (!filme) exit(1);
  strcpy(filme->nome, nome);
  filme->nivel = 0;
  filme->cor = BRANCO;
  filme->similar = NULL;
  return filme;
}

typedef struct Lista {
  Filme **filmes;
  int tamanho;
} Lista;

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
  for (int i = 0; i < lista->tamanho; i++)
    printf("Filme na posição %d: %s\n", i, lista->filmes[i]->nome);
}

typedef struct Recomendacao {
  Filme **filmes;
} Recomendacao;

Recomendacao *criar_recomendacao(void) {
  Recomendacao *recomendacao = malloc(sizeof(Recomendacao));
  if (!recomendacao) exit(1);
  
  recomendacao->filmes = malloc(sizeof(Filme *) * NUMERO_FILMES);
  if (!recomendacao->filmes) exit(1);

  for (int i = 0; i < NUMERO_FILMES; i++)
    recomendacao->filmes[i] = criar_filme(nomes_filmes[i]);

  return recomendacao;
}

void adicionar_similaridade(Recomendacao *recomendacao, int posicao_de, int posicao_para) {
  Similaridade *similaridade = NULL;
  Similaridade *cursor = NULL;
  Filme *filme = NULL;

  // ----------------- IDA ------------------------

  similaridade = criar_similaridade(posicao_para);
  filme = recomendacao->filmes[posicao_de];

  cursor = filme->similar;

  while (cursor != NULL) {
    if (cursor->posicao == similaridade->posicao)
      return;
    
    cursor = cursor->proximo;
  }

  similaridade->proximo = filme->similar;
  filme->similar = similaridade;
  
  // ----------------- VOLTA ------------------------

  similaridade = criar_similaridade(posicao_de);
  filme = recomendacao->filmes[posicao_para];

  cursor = filme->similar;

  while (cursor != NULL) {
    if (cursor->posicao == similaridade->posicao)
      return;

    cursor = cursor->proximo;    
  }

  similaridade->proximo = filme->similar;
  filme->similar = similaridade;
}

void gerar_similaridades(Recomendacao *recomendacao) {
  int quantidade_loops = 5;
  int posicao_origem = 0, posicao_destino = 0;

  for (int i = 0; i < NUMERO_FILMES; i++) {
    for (int j = 0; j < quantidade_loops; j++) {
      posicao_origem = i;
      posicao_destino = rand() % NUMERO_FILMES;

      adicionar_similaridade(recomendacao, posicao_origem, posicao_destino);
    }
  }
}

void buscar_similaridades_interno(Recomendacao *recomendacao, Lista *lista, int posicao, int nivel) {
  printf("Posicao: %d. Nivel: %d\n", posicao, nivel);

  Filme *filme = recomendacao->filmes[posicao];
  filme->nivel = nivel;
  filme->cor = CINZA;
  
  inserir_lista(lista, filme);

  Similaridade *cursor = filme->similar;
  while (cursor != NULL) {
    int posicao_similar = cursor->posicao;
    Filme *filme_similar = recomendacao->filmes[posicao_similar];

    if (filme_similar->cor == BRANCO)
      buscar_similaridades_interno(recomendacao, lista, posicao_similar, nivel + 1);

    cursor = cursor->proximo;
  }

  filme->cor = PRETO;
}

void buscar_similaridades(Recomendacao *recomendacao, int posicao_inicial) {
  Filme *filme = NULL;
  Lista *lista_filmes = criar_lista();
  int nivel = 0;

  printf("\n\nPosição inicial: %d\n", posicao_inicial);
  
  for (int i = 0; i < NUMERO_FILMES; i++) {
    filme = recomendacao->filmes[i];
    filme->cor = BRANCO;
  }

  puts("Buscando similaridades...");
  buscar_similaridades_interno(recomendacao, lista_filmes, posicao_inicial, nivel);
}

void printar_similares(Filme *filme) {
  Similaridade *cursor = filme->similar;
  int contador = 0;

  printf("Filme: %s\n", filme->nome);

  while (cursor != NULL) {
    printf("Similar %d: %d\n", contador, cursor->posicao);
    cursor = cursor->proximo;
    contador++;
  }
}

int main(void) {
  srand(time(NULL));

  Recomendacao *recomendacao = criar_recomendacao();

  gerar_similaridades(recomendacao);

  for (int i = 0; i < NUMERO_FILMES; i++) {
    printf("\n\n[%d]: ", i);
    printar_similares(recomendacao->filmes[i]);
  }

  buscar_similaridades(recomendacao, rand() % NUMERO_FILMES);
}