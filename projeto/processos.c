#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <wait.h>

// tipo de dado para armazenar tempo e direcao para cada pessoa
struct Pessoa {
  int tempo;
  int direcao;
};

// funcao para realizar um ciclo da escada rolante (direcao 1 ou 0)
int escada_direcao (struct Pessoa fila[], int qtd, int tempo, short *indice){
  int t = tempo, t_aux;
  for (int i = *indice; i < qtd; i++){
    t_aux = fila[i].tempo;
    // se o tempo da pessoa no topo da fila for menor que o tempo contado
    if (fila[i].tempo < t){
      t_aux = tempo;
    }
    // se o tempo da pessoa no topo da fila for menor ou igual ao tempo contado + 10
    // mais pessoas irao usar a escada rolante nesse ciclo
    if (fila[i].tempo <= t+10){
      (*indice)++;
      t = t_aux;
    // senao, retornar tempo do ciclo
    // nao ha mais pessoas para usar a escada rolante nesse ciclo
    } else{
      return t+10;
    }
  }
  return t+10;
}

int main(void) {
  // declaracao de variaveis
  int qtd_pessoas, i, qtd_0 = 0, qtd_1 = 0, indice0 = 0, indice1 = 0;
  FILE * arquivo;
  // ponteiros para indice de arrays que atuam como filas
  int *ptr0, *ptr1;
  ptr0 = &indice0;
  ptr1 = &indice1;

  // le arquivo e cria vetor de pessoas
  arquivo = fopen ("entrada.txt", "r");
  if (arquivo == NULL){ // erro ao abrir arquivo
    printf("Erro ao abrir arquivo");
    return -1;
  }
  fscanf(arquivo, "%d", &qtd_pessoas);
  struct Pessoa pessoas [qtd_pessoas];

  // atribuir valores para vetor "pessoas" e calcular quantas pessoas em cada direcao
  for (i = 0; i < qtd_pessoas; i++) {
    fscanf(arquivo, "%d %d", &pessoas[i].tempo, &pessoas[i].direcao);
    if (pessoas[i].direcao == 0){
      qtd_0++;
    } else{
      qtd_1++;
    }
  }
  
  fclose(arquivo); // fechar arquivo
  
  // alocar pessoas em filas (arrays) de acordo com a direcao
  struct Pessoa fila0 [qtd_0];
  struct Pessoa fila1 [qtd_1];
  int pessoas0 = 0, pessoas1 = 0;
  for (i = 0; i < qtd_pessoas; i++) {
    if (pessoas[i].direcao == 0){
      fila0 [pessoas0] = pessoas[i];
      pessoas0++;
    } else{
      fila1 [pessoas1] = pessoas[i];
      pessoas1++;
    }
  }
    
  // descobrir e armazenar a primeira direcao da escada rolante
  short primeiro;
  if (pessoas[0].direcao == 0){
    primeiro = 0;
  } else{
    primeiro = 1;
  }
    
  pid_t filhoPID = fork(); // cria processo filho
  int t1, t2, t;
  if (filhoPID < 0){
    printf("Erro ao criar processo filho\n");
  } else if (filhoPID == 0){
  // cria uma chave com um "filepath" e um id
  key_t chave = ftok("entrada.txt",11);

  // usa chave criada para alocar 10 bytes de memoria compartilhada
  int shmid = shmget(chave, 10, 0666 | IPC_CREAT);
  if (shmid == -1) {
    perror("Nao foi possivel criar regiao de memoria compartilhada!\n");
    return EXIT_FAILURE;
  }

  // alocar ponteiro para memoria compartilhada
  void* memoriaComp = (void*) shmat(shmid, (void*)0, 0);

  // criar ponteiros e faze-los apontar para regioes da memoria compartilhada
  int* tempo;
  short* indice0;
  short* indice1;
  short* vez;
  tempo = memoriaComp; // endereço da memoriaComp alocada
  // int = tamanho 4 bytes (32bits)
  // endereco da memoriaComp + espaco alocado para tempo (int) + 1
  indice0 = memoriaComp + 33;
  indice1 = indice0 + 1; // endereco alocado para indice0 + 1
  vez = indice1 + 1; // endereco alocado para indice1 + 1

  // alocar valores iniciais
  *tempo = 0;
  *indice0 = 0;
  *indice1 = 0;
  *vez = primeiro;
     
  // loop para o tempo de cada ciclo da escada rolante para a direcao 0
  // garantir que todas as pessoas na direcao 0 usem a escada rolante
  while(*indice0 < qtd_0){

    // busy waiting para esperar a vez de executar a direcao 0 na escada rolante
    while(*vez == 1){};
    *tempo = escada_direcao(fila0, qtd_0, *tempo, indice0);

    // se o tempo da proxima pessoa a utilizar a escada rolante na direcao 0 for maior que o tempo 
    // da proxima pessoa a utilizar a escada rolante na direcao 1
    if (fila0[*indice0].tempo > fila1[*indice1].tempo){
      *vez = 1; // troca de direcao
    }
  }

  // se apos percorrer toda a fila0 ainda existir pessoas na fila1
  // mudar escada de direcao uma ultima vez
  if (*indice1 < qtd_1){
    *vez = 1; 
  }

  // "shared memory detach"
  shmdt(tempo); // desconectar da memoria compartilhada

  return 0; // termino de execucao do processo filho
} else{
    // cria uma chave com um "filepath" e um id
    key_t chave = ftok("entrada.txt",11);

    // usa chave criada para alocar 10 bytes de memoria compartilhada
    int shmid = shmget(chave, 10, 0666 | IPC_CREAT);
    if (shmid == -1) {
      perror("Nao foi possivel criar regiao de memoria compartilhada!\n");
      return EXIT_FAILURE;
    }

    // alocar ponteiro para memoria compartilhada
    void* memoriaComp = (void*) shmat(shmid, (void*)0, 0);

    // criar ponteiros e faze-los apontar para regioes da memoria compartilhada
    int* tempo;
    short* indice0;
    short* indice1;
    short* vez;
    tempo = memoriaComp; // endereço da memoriaComp alocada
    // int = tamanho 4 bytes (32bits)
    // endereco da memoriaComp + espaco alocado para tempo (int) + 1
    indice0 = memoriaComp + 33;
    indice1 = indice0 + 1; // endereco alocado para indice0 + 1
    vez = indice1 + 1; // endereco alocado para indice1 + 1

    // alocar valores iniciais
    *tempo = 0;
    *indice0 = 0;
    *indice1 = 0;
    *vez = primeiro;

    // loop para o tempo de cada ciclo da escada rolante para a direcao 1
    // garantir que todas as pessoas na direcao 1 usem a escada rolante
    while(*indice1 < qtd_1){

      // busy waiting para esperar a vez de executar a direcao 1 na escada rolante
      while(*vez == 0){};
      *tempo = escada_direcao(fila1, qtd_1, *tempo, indice1);

      // se o tempo da proxima pessoa a utilizar a escada rolante na direcao 1 for maior que o tempo 
      // da proxima pessoa a utilizar a escada rolante na direcao 0
      if (fila1[*indice1].tempo > fila0[*indice0].tempo){
        *vez = 0; // troca de direcao 
      }
    }

    // se apos percorrer toda a fila1 ainda existir pessoas na fila0
    // mudar escada de direcao uma ultima vez
    if (*indice0 < qtd_0){
      *vez = 0;
    }

    wait(NULL); // espera processo filho terminar
    printf("%d", *tempo); // imprime tempo total da escada rolante
    shmdt(tempo); // desconectar da memoria compartilhada

    shmctl(shmid, IPC_RMID, NULL); // destroi segmento utilizado para memoria compartilhada
  }
}
