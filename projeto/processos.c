#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <wait.h>

//const char *semNome = "Semaforo";
sem_t s;

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
    arquivo = fopen ("teste.txt", "r");
    fscanf(arquivo, "%d", &qtd_pessoas);
    struct Pessoa pessoas [qtd_pessoas];

    // calcular quantas pessoas em cada direcao
    for (i = 0; i < qtd_pessoas; i++) {
        fscanf(arquivo, "%d %d", &pessoas[i].tempo, &pessoas[i].direcao);
        if (pessoas[i].direcao == 0){
            qtd_0++;
        } else{
            qtd_1++;
        }
    }

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

    short primeiro;
    if (pessoas[0].direcao == 0){
        primeiro = 0;
    } else{
        primeiro = 1;
    }

    sem_init(&s,1,1);
    pid_t filhoPID = fork();
    int t1, t2, t;
    if (filhoPID < 0){
      printf("Erro ao criar processo filho\n");
    } else if (filhoPID == 0){
      key_t chave = ftok("memoriaComp",11);

      int shmid = shmget(chave, 10, 0666 | IPC_CREAT);
      if (shmid == -1) {
        perror("Could not get shared memory");
        return EXIT_FAILURE;
      }
      void* memoriaComp = (void*) shmat(shmid, (void*)0, 0);
      int* tempo;
      short* indice0;
      short* indice1;
      short* vez;
      tempo = memoriaComp;
      indice0 = memoriaComp + 33; // size of 4 bytes (int) = 32
      indice1 = indice0 + 1;
      vez = indice1 + 1;
      *tempo = 0;
      *indice0 = 0;
      *indice1 = 0;
      *vez = primeiro;

      printf("Aqui eh o processo filho %d\n", filhoPID);
      while(*vez != 0){};

      int i = 0;
      while(*indice0 < qtd_0){
        //if (*vez == 1){
        //   sem_wait(&s);
        //}
        while(*vez == 1){};
        //sleep(4);
        printf("fila0: %d\n", *tempo);
        *tempo = escada_direcao(fila0, qtd_0, *tempo, indice0);
        printf("fila0: %d\n", *tempo);
        if (fila0[*indice0].tempo > fila1[*indice1].tempo){
            *vez = 1;
            printf("volta 1\n");
            //sem_post(&s);
        } else{
            *vez = 0;
            printf("continua 0\n");
        }
      }

      if (*indice1 < qtd_1){
        //sem_post(&s);
        *vez = 1;
      }
      //if (pessoas[qtd_pessoas-1].direcao == 0){
      //  printf("%d", *tempo);
      //}
      shmdt(tempo);
      return 0;
    } else{
      key_t chave = ftok("memoriaComp",11);

      int shmid = shmget(chave, 10, 0666 | IPC_CREAT);
      if (shmid == -1) {
        perror("Could not get shared memory");
        return EXIT_FAILURE;
      }
      void* memoriaComp = (void*) shmat(shmid, (void*)0, 0);
      int* tempo;
      short* indice0;
      short* indice1;
      short* vez;
      tempo = memoriaComp;
      indice0 = memoriaComp + 33; // size of 4 bytes (int) = 32
      indice1 = indice0 + 1;
      vez = indice1 + 1;
      *tempo = 0;
      *indice0 = 0;
      *indice1 = 0;
      *vez = primeiro;

      printf("Aqui eh o processo pai %d\n", filhoPID);

      int i = 0;
      while(*vez != 1){};
      printf("%d",*vez);

      while(*indice1 < qtd_1){
        //if (*vez == 0){
        //    sem_wait(&s);
        //}
        //sleep(4);
        while(*vez == 0){};
        *tempo = escada_direcao(fila1, qtd_1, *tempo, indice1);
        printf("fila1: %d\n", *tempo);
        if (fila1[*indice1].tempo > fila0[*indice0].tempo){
            *vez = 0;
            //sem_post(&s);
            printf("volta 0\n");
        } else{
            *vez = 1;
            printf("continua 1\n");
        }
      }
      if (*indice0 < qtd_0){
        //sem_post(&s);
        *vez = 0;
      }

      //if (pessoas[qtd_pessoas-1].direcao == 1){
      //  printf("%d", *tempo);
      //}
      wait(NULL);
      printf("%d", *tempo);
      shmdt(tempo);

      shmctl(shmid, IPC_RMID, NULL);
    }
    // calcular tempo total
    // usando loop while para calcular cada ciclo da escada rolante
}
