#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// criar variaveis globais: mutex, variavel condicional e vez
pthread_mutex_t lock;
pthread_cond_t cond;
int vez; // armazena valor para decidir qual thread deve executar

// tipo de dado para armazenar tempo e direcao para cada pessoa
struct Pessoa {
    int tempo;
    int direcao;
};

// tipo de dado para armazenar informacoes enviadas para as threads
struct thread_data {
    struct Pessoa *fila;
    struct Pessoa *outra_fila;
    struct Pessoa *pessoas;
    int threadid;
    int qtd;
    int *tempo;
    int *indice_fila;
    int *indice_outra_fila;
};

// funcao para realizar um ciclo da escada rolante (direcao 1 ou 0)
int escada_direcao (struct Pessoa fila[], int qtd, int tempo, int *indice){
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

// funcao que cada thread executara
void *thread_func(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;

    // loop para garantir que todas as pessoas (direcao 0 e 1) usem a escada rolante
    while (*(data->indice_fila) < data->qtd) {

        // thread pega o "cadeado", garante o direito de executar ciclos na escada rolante
        pthread_mutex_lock(&lock);

        // se thread0 ou thread1 esta executando na vez em que outra thread deveria ser executada
        if ((vez == 1 && (data->threadid) == 0) || (vez == 0 && (data->threadid) == 1)){
            // libera o "cadeado" e espera sinal para voltar a executar
            pthread_cond_wait(&cond, &lock);
        }
        *(data->tempo) = escada_direcao(data->fila, data->qtd, *(data->tempo), data->indice_fila);
        
        // se o tempo de chegada da proxima pessoa da fila que esta sendo executada 
        // for maior que o tempo de chegada da proxima pessoa da fila em espera
        if (data->fila[*(data->indice_fila)].tempo > data->outra_fila[*(data->indice_outra_fila)].tempo){
            // passa a vez para proxima thread executar (fila)
            if (vez == 1 && (data->threadid == 1)){
                vez = 0;
            }else {
                vez = 1;
            }
            // manda um sinal para acordar thread em espera
            pthread_cond_signal(&cond);
        }
        // libera o "cadeado"
        pthread_mutex_unlock(&lock);
    }
    
    // se uma das filas nao possui mais pessoas, 
    // faz a outra fila executar uma ultima vez
    if (vez == 1){
        vez = 0;
    }else{
        vez = 1;
    }
    if ((*(data->indice_fila) != 0) && (*(data->indice_outra_fila) != 0)){
      pthread_cond_signal(&cond);
    }

    return NULL;
}

int main(void) {
    // declaracao de variaveis
    int qtd_pessoas, i, qtd_0 = 0, qtd_1 = 0, indice0 = 0, indice1 = 0;
    FILE * arquivo;
    // ponteiros para indice de arrays que atuam como filas
    int *ptr0, *ptr1;
    ptr0 = &indice0;
    ptr1 = &indice1;

    // ler arquivo e criar vetor de pessoas
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

    // criar variavel tempo e duas threads, uma pra cada direcao
    int tempo = 0;
    pthread_t thread0, thread1;

    // criar variaveis "thread_data", que sao os argumentos passados para thread
    struct thread_data data0 = {fila0, fila1, pessoas, 0, qtd_0, &tempo, ptr0, ptr1};
    struct thread_data data1 = {fila1, fila0, pessoas, 1, qtd_1, &tempo, ptr1, ptr0};
    
    // inicializar variaveis lock e cond, com atributos padrao
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    // se primeira pessoa a usar a escada rolante for na direcao 0
    if (pessoas[0].direcao == 0){
        vez = 0;
        // criar primeiro a thread0 (ciclos da escada rolante para direcao 0)
        if(pthread_create(&thread0, NULL, thread_func, &data0) != 0){
            printf("Erro ao criar thread0!");
        }
        // apos a criacao da thread 0, criar thread1 (ciclos da escada rolante para direcao 1)
        if(pthread_create(&thread1, NULL, thread_func, &data1) != 0){
            printf("Erro ao criar thread1!");
        } 
    } else{
        vez = 1;
        // criar primeiro a thread1 (ciclos da escada rolante para direcao 1)
        if(pthread_create(&thread1, NULL, thread_func, &data1) != 0){
            printf("Erro ao criar thread1!");
        }
        // apos a criacao da thread 1, criar thread0 (ciclos da escada rolante para direcao 0)
        if(pthread_create(&thread0, NULL, thread_func, &data0) != 0){
            printf("Erro ao criar thread0!");
        }
    }
    
    // esperar as threads terminarem a execucao
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);

    // destruir as variaveis lock e cond
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    // imprimir tempo total 
    printf("%d\n", tempo);
    return 0;
}
