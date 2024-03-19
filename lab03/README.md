# ATIVIDADE 3
# EXERCÍCIO  – COMUNICAÇÃO ENTRE PROCESSO PAI E THREAD

## Alunos:
* Eduardo Honorio Friaca - RA: 10408959
* Matheus Marcal Ramos de Oliveira - RA: 10409001
* Pedro Akira Cardoso Toma - RA: 10390171

### O trecho abaixo apresenta uma criação de thread no Linux:

![Captura de tela 2024-03-18 220129](https://github.com/PedroToma-git/Sistemas_Operacionais_AWS/assets/113950175/159394aa-8dc6-40f7-be0c-223ad837117e)
![Captura de tela 2024-03-18 220206](https://github.com/PedroToma-git/Sistemas_Operacionais_AWS/assets/113950175/b63a7a78-1373-4cf6-93b7-690377b7eb5a)

Resultado da execução na AWS do código fornecido (exemplo.c)

![Captura de tela 2024-03-18 220531](https://github.com/PedroToma-git/Sistemas_Operacionais_AWS/assets/113950175/9f4e0735-453c-47e3-a8c4-7797b2aaa76c)

### EXERCÍCIO: Incremente o exemplo acima para representar a troca de informações de contexto entre a thread e o processo pai.

![Captura de tela 2024-03-18 220940](https://github.com/PedroToma-git/Sistemas_Operacionais_AWS/assets/113950175/aad9bb2c-5169-41da-9931-cf91a880be07)
![Captura de tela 2024-03-18 220953](https://github.com/PedroToma-git/Sistemas_Operacionais_AWS/assets/113950175/50b4cb74-d079-48c6-a2ae-0c09bf7f78ea)

A ilustração acima é a modificação do código fornecido (exemplo.c) para realizar a comunicação da thread criada com o processo pai (exercicio.c).

Primeiramente, nós utilizamos a biblioteca pthread.h para a criação e manipulação do thread criado. 

Começamos criando uma variável inteira “var”, com valor igual a 1, que será utilizada para demonstrar a comunicação entre processo pai e thread, e a variável “thread” do tipo pthread_t, para receber o identificador do thread, que será criado ao chamar a função pthread_create(). Essa função recebe quatro parâmetros na seguinte ordem, um ponteiro para o identificador da thread, os atributos da thread, que foram utilizado os valores padrões (NULL), o nome da função a ser executada pela thread, e os argumentos utilizados na função (endereço de var, convertido para (void *)).

Dentro da função (execução do thread), é criado um ponteiro para inteiro (variavel) e ele aponta para o endereço de var, o qual é convertido para (int *) ponteiro de inteiro. O valor da variável (var) é acrescido de 1, e como a thread utiliza o mesmo espaço de memória que o processo pai, o local da memória da variável na thread é o mesmo para o local da variável var no processo pai. Dessa forma, a alteração da variável na thread acarretará na mudança da variável var no processo pai.

A função pthread_join() espera o término da execução da thread passada como argumento.

Por fim, a modificação no valor da variável na execução da thread resultou na alteração do valor da variável no processo pai, isso foi demonstrado através de prints durante a execução do programa, como demonstrado abaixo: 

![Captura de tela 2024-03-18 214040](https://github.com/PedroToma-git/Sistemas_Operacionais_AWS/assets/113950175/c83a3f8c-8b9c-4c94-b5b5-5778d696e041)
