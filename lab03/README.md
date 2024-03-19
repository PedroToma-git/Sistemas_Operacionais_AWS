# ATIVIDADE 3
# EXERCÍCIO  – COMUNICAÇÃO ENTRE PROCESSO PAI E THREAD

## Alunos:
* Eduardo Honorio Friaca - RA: 10408959
* Matheus Marcal Ramos de Oliveira - RA: 10409001
* Pedro Akira Cardoso Toma - RA: 10390171

### O trecho abaixo apresenta uma criação de thread no Linux:


Resultado da execução na AWS do código fornecido (exemplo.c)


### EXERCÍCIO: Incremente o exemplo acima para representar a troca de informações de contexto entre a thread e o processo pai.



A ilustração acima é a modificação do código fornecido (exemplo.c) para realizar a comunicação da thread criada com o processo pai (exercicio.c).

Primeiramente, nós utilizamos a biblioteca pthread.h para a criação e manipulação do thread criado. 

Começamos criando uma variável inteira “var”, com valor igual a 1, que será utilizada para demonstrar a comunicação entre processo pai e thread, e a variável “thread” do tipo pthread_t, para receber o identificador do thread, que será criado a criado ao chamar a função pthread_create(), esta recebe quatro parâmetros na seguinte ordem, um ponteiro para o identificador da thread, os atributos da thread, que foram utilizado os valores padrões (NULL), o nome da função a ser executada pela thread, e os argumentos utilizados na função (endereço de var, convertido para (void *)).

Dentro da função (execução do thread), é criado um ponteiro para inteiro (variavel) e ele aponta para o endereço de var, o qual é convertido para (int *) ponteiro de inteiro. O valor da variável (var) é acrescido de 1, e como a thread utiliza o mesmo espaço de memória que o processo pai, o local da memória da variável na thread é o mesmo para o local da variável var no processo pai. Dessa forma, a alteração da variável na thread acarretará na mudança da variável var no processo pai.

A função pthread_join() espera o término da execução da thread passada como argumento.

Por fim, a modificação no valor da variável na execução da thread resultou na alteração do valor da variável no processo pai, isso foi demonstrado através de prints durante a execução do programa, como demonstrado abaixo: 


