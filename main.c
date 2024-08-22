#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define NUMERO_FILMES 20
#define TAMANHO_NOME 256

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
void destruir_recomendacao(Recomendacao *recomendacao);

void adicionar_similaridade(Recomendacao *recomendacao, int posicao_de, int posicao_para);
void gerar_similaridades(Recomendacao *recomendacao);

int comparar_por_distancia(const void *a, const void *b);

void buscar_menores_distancias_interno(Recomendacao *recomendacao, int posicao_inicial, int posicao_final, int distancia_atual, int* distancia_minima);
void buscar_menores_distancias(Recomendacao *recomendacao, int posicao_inicial);

int main(void) {
  srand(time(NULL));

  Recomendacao *recomendacao = criar_recomendacao();
  int posicao_inicial = rand() % NUMERO_FILMES;

  gerar_similaridades(recomendacao);
  exibir_filmes_similares(recomendacao);

  buscar_menores_distancias(recomendacao, posicao_inicial);

  destruir_recomendacao(recomendacao);
}

/**
 * Cria um ponteiro para similaridade.
 */
Similaridade *criar_similaridade(int posicao) {
  Similaridade *similaridade = (Similaridade *) malloc(sizeof(Similaridade));
  if (!similaridade) exit(1);
  similaridade->posicao = posicao;
  similaridade->proximo = NULL;
  return similaridade;
}

/**
 * Desaloca a lista encadeada de similaridades de um filme,
 * a partir do ponteiro inicial.
 */
void destruir_similaridades(Similaridade *inicial) {
  Similaridade *atual = inicial;
  Similaridade *proximo = NULL;

  while (atual != NULL) {
    proximo = atual->proximo;
    free(atual);
    atual = proximo;
  }
}

/**
 * Cria um ponteiro para filme, recebendo um identificador e um nome,
 * para efeitos de exibição ao usuário.
 */
Filme *criar_filme(int identificador, char nome[]) {
  Filme *filme = (Filme *) malloc(sizeof(Filme));
  if (!filme) exit(1);

  filme->identificador = identificador;
  strcpy(filme->nome, nome);

  filme->cor = BRANCO;
  filme->distancia = INT_MAX;

  filme->similar = NULL;
  return filme;
}

/**
 * Exibe todos os similares de um dado filme.
 */
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

/**
 * Desaloca a memória de um ponteiro de filme.
 */
void destruir_filme(Filme *filme) {
  if (filme == NULL)
    return;

  destruir_similaridades(filme->similar);
  filme->similar = NULL;

  free(filme);
  filme = NULL;
}

/**
 * Aloca a memória necessária para um ponteiro para lista de filmes.
 */
Lista *criar_lista(void) {
  Lista *lista = (Lista *) malloc(sizeof(Lista));
  if (!lista) exit(1);
  lista->tamanho = 0;
  lista->filmes = NULL;
  return lista;
}

/**
 * Insere um ponteiro para filme na lista, alocando memória
 * caso seu tamanho seja nulo, e realocando memória caso tenha um tamanho não nulo,
 * sempre aumentando em 1 espaço para ponteiro para filme.
 */
void inserir_lista(Lista *lista, Filme *filme) {
  if (!lista->tamanho)
    lista->filmes = (Filme **) malloc(sizeof(Filme *));
  else
    lista->filmes = (Filme **) realloc(lista->filmes, sizeof(Filme *) * (lista->tamanho + 1));

  if (lista->filmes == NULL) exit(1);

  lista->filmes[lista->tamanho++] = filme;
}

/**
 * Exibe todos os filmes da lista que será exibida para o usuário,
 * validando se a lista tem apenas um filme, sendo a situação do filme não
 * possuir similares.
 */
void exibir_lista(Lista *lista) {
  Filme *filme = NULL;

  filme = lista->filmes[0];
  printf("\n---------- Exibindo similares do filme \"[%d] - %s\": ----------\n\n", filme->identificador, filme->nome);

  // Existe apenas o filme inicial
  if (lista->tamanho == 1) {
    printf("Nao ha filmes similares.\n");
    return;
  }

  for (int i = 1; i < lista->tamanho; i++) {
    filme = lista->filmes[i];
    printf("Posicao: %-10d %-50s Nivel: %-10d\n", filme->identificador, filme->nome, filme->distancia);
  }
}

/**
 * Desaloca a memória da lista de filmes.
 */
void destruir_lista(Lista *lista) {
  if (lista == NULL) return;

  free(lista->filmes);
  lista->filmes = NULL;

  free(lista);
  lista = NULL;
}

/**
 * Cria um ponteiro para uma estrutura de recomendação.
 */
Recomendacao *criar_recomendacao(void) {
  Recomendacao *recomendacao = (Recomendacao *) malloc(sizeof(Recomendacao));
  if (!recomendacao) exit(1);

  recomendacao->filmes = (Filme **) malloc(sizeof(Filme *) * NUMERO_FILMES);
  if (!recomendacao->filmes) exit(1);

  for (int i = 0; i < NUMERO_FILMES; i++)
    recomendacao->filmes[i] = criar_filme(i, nomes_filmes[i]);

  return recomendacao;
}

/**
 * Exibe todos os filmes, junto de seus similares.
 */
void exibir_filmes_similares(Recomendacao *recomendacao) {
  puts("\n---------- Exibindo os similares de cada filme... ----------\n");
  for (int i = 0; i < NUMERO_FILMES; i++) {
    printf("[%d]: ", i);
    exibir_similares(recomendacao->filmes[i]);
  }
}

/**
 * Desaloca a estrutura do sistema de recomendação,
 * bem como seus filmes e as suas respectivas similaridades.
 */
void destruir_recomendacao(Recomendacao *recomendacao) {
  if (recomendacao == NULL) return;

  for (int i = 0; i < NUMERO_FILMES; i++)
    destruir_filme(recomendacao->filmes[i]);

  free(recomendacao->filmes);
  recomendacao->filmes = NULL;

  free(recomendacao);
  recomendacao = NULL;
}

/**
 * Adiciona um ponteiro de estrutura que representa a similaridade entre dois filmes.
 * Essa similaridade é adicionada na lista encadeada de similaridades, tanto do filme referenciado
 * pela 'posicao_de', quanto pelo referenciado pela 'posicao_para'. É feita, também, uma verificação
 * para não adicionar uma mesma similaridade em uma mesma lista duas vezes.
 */
void adicionar_similaridade(Recomendacao *recomendacao, int posicao_de, int posicao_para) {
  Similaridade *similaridade = NULL;
  Similaridade *cursor = NULL;
  Filme *filme = NULL;

  // ----------------- Ida ------------------------

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

  // ----------------- Volta (grafo não direcional) ------------------------

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

/**
 * Gera similaridades aleatoriamente entre os filmes.
 * São realizadas NUMERO_FILMES = 20 tentativas de conexão entre os filmes.
 * Tentativas, por ser possível os mesmos números serem gerados mais de uma vez.
 */
void gerar_similaridades(Recomendacao *recomendacao) {
  int posicao_origem = 0, posicao_destino = 0;

  for (int i = 0; i < NUMERO_FILMES; i++) {
    posicao_origem = rand() % NUMERO_FILMES;  // 0 a 19
    posicao_destino = rand() % NUMERO_FILMES;  // 0 a 19

    adicionar_similaridade(recomendacao, posicao_origem, posicao_destino);
  }
}

/**
 * Utilizado pela função qsort para ordernar os filmes na lista de filmes
 * pela distância que cada um tem pro filme escolhido.
 */
int comparar_por_distancia(const void *a, const void *b) {
  // Conversão dos ponteiros para ponteiros para Filme
  const Filme *filme_a = *(const Filme **)a;
  const Filme *filme_b = *(const Filme **)b;

  // Compara as distancias
  if (filme_a->distancia < filme_b->distancia) return -1;
  if (filme_a->distancia > filme_b->distancia) return 1;
  return 0;
}

/**
 * Busca recursiva para encontrar a menor distância entre dois filmes no grafo.
 * Encontrada a menor distância, o ponteiro para distância mínima é modificado com esse valor.
 */
void buscar_menores_distancias_interno(Recomendacao *recomendacao, int posicao_inicial, int posicao_final, int distancia_atual, int* distancia_minima) {
  Filme *filme = recomendacao->filmes[posicao_inicial];

  // Marcar como visitado para não retornar a esse nó dentro desta busca
  filme->cor = PRETO;

  // Descomentar para mostrar a execução da busca para cada um dos filmes (>)
  // > printf("Posicao atual: %-10d Posicao final: %-10d Distancia: %-10d ", posicao_inicial, posicao_final, distancia_atual);

  if (posicao_inicial == posicao_final && distancia_atual < *distancia_minima) {
    *distancia_minima = distancia_atual;
    // > printf("Distancia minima: %-10d\n", *distancia_minima);
  } else {
    // > printf("Distancia minima: %-10d\n", *distancia_minima);

    // Busca todos os filmes similares
    Similaridade *cursor = filme->similar;

    while (cursor != NULL) {
      int posicao_similar = cursor->posicao;
      Filme *filme_similar = recomendacao->filmes[posicao_similar];

      if (filme_similar->cor == BRANCO)
        buscar_menores_distancias_interno(recomendacao, posicao_similar, posicao_final, distancia_atual + 1, distancia_minima);

      cursor = cursor->proximo;
    }
  }

  // Desmarcar o nó para outras possibilidades de caminhos
  filme->cor = BRANCO;
}

/**
 * Dado o sistema de recomendação e a posição do filme inicial, busca todos os seus similares,
 * guardando a distância do filme para cada um deles. Dessa forma, é construída uma lista
 * de todos os filmes similares ao escolhido, que é ordenada por meio da distância ao inicial,
 * para que seja exibida ao usuário.
 */
void buscar_menores_distancias(Recomendacao *recomendacao, int posicao_inicial) {
  Lista *lista_resultado = criar_lista();
  Filme *filme = NULL;

  int posicao_final = 0, distancia_minima = 0;

  printf("Filme inicial: %d\n\n", posicao_inicial);

  for (int i = 0; i < NUMERO_FILMES; i++) {
    posicao_final = i;  // A distância será verificada para cada um dos filmes
    distancia_minima = INT_MAX;  // Máximo valor que um inteiro pode ter

    printf("---------- Filme: %d <-> Filme: %d ----------\n", posicao_inicial, posicao_final);

    filme = recomendacao->filmes[i];
    buscar_menores_distancias_interno(recomendacao, posicao_inicial, posicao_final, 0, &distancia_minima);
    filme->distancia = distancia_minima;

    if (filme->distancia != INT_MAX) {
      printf("---------- Distancia minima: %d ----------\n\n", distancia_minima);
      inserir_lista(lista_resultado, filme);
      continue;
    }

    printf("---------- Nao ha similaridade ----------\n\n");
  }

  // Ordena a lista que será exibida ao usuário pela distância que cada filme tem
  // para o filme que foi escolhido.
  qsort(lista_resultado->filmes, lista_resultado->tamanho, sizeof(Filme *), comparar_por_distancia);

  exibir_lista(lista_resultado);
  printf("\n");

  destruir_lista(lista_resultado);
}
