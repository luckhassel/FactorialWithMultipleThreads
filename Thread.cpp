#include <iostream>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <thread>

// Cria uma estrura para passar os dados à Thread.
struct argumentos{
    float num_inicio;
    float num_fim;
    long double resultados;
};

// Cria a função que será chamada pela Thread.
void* fatorial (void*arg) {
    struct argumentos *args = (struct argumentos*) arg;
    long double resultado = args->num_inicio;

    if (resultado == 0){ // Se o fatorial desejado for de 0!, thread entra aqui e morre.
        args->resultados = 1;
        pthread_exit(0);
    }
    //Thread fica incrementando até que, seja maior que 1, ou seja maior que o número de fim da Thread.
    while ((args->num_inicio > 1) && ((args->num_inicio > args->num_fim))) 
        resultado *= --args->num_inicio;
    if (args->num_inicio == args->num_fim){
        args->resultados = resultado;
        pthread_exit(0);
    }
    //Caso não seja nenhum dos casos anteriores, Thread recebe o resultado, e é morta.
    args->resultados = resultado;
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    //Verifica se a quantidade de argumentos está correta
    if (argc != 3) 
    { 
        fprintf(stderr,"Erro - Entre com o valor do fatorial e thread\n");
        return -EINVAL; 
    }

    // Verifica se o fatorial está dentro do intervalo
    if ( atoi(argv[1]) < 0 || atoi(argv[1]) > 1000000000) 
    { 
        fprintf(stderr, "Numero fatorial deve estar entre 0 e 1000000000\n");
        return -EDOM;
    } 
    
    //Verifica se o numero de processos está dentro do intervalo
    if ( atoi(argv[2]) <= 0 || atoi(argv[2]) > 1000000) 
    { 
        fprintf(stderr, "Numero de threads deve estar entre 1 e 10000\n"); 
        return -EDOM;
    } 

    int processos, err;
    int i = 0;
    int result_code;

    processos = atoi(argv[2]); // Recebe a quantidade de processos definida pelo usuário
    struct argumentos final_args[processos]; // Cria um vetor de estruturas

    final_args[0].num_inicio = atoi(argv[1]); // Atribui ao num_inicio da estrutura o valor fatorial passado


    pthread_t tid[processos]; // Cria um vetor de elementos pthread_t

    int fatorial_por_processo = final_args[0].num_inicio/processos; // Calcula o número de elementos em cada fatorial
    final_args[0].num_fim = final_args[0].num_inicio - fatorial_por_processo; // Calcula o número final de cada thread

    // Loop para criação de múltiplas Threads
    for(i=0; i<processos; i++){
        // Faz verificações para caso os numeros sejam menores que 0
        if (final_args[i].num_inicio < 0) final_args[i].num_inicio = 0;
        if (final_args[i].num_fim < 0) final_args[i].num_fim = 0;
        std::cout << "\nCRIANDO A THREAD DE ID: " << i+1 << std::endl;

        std::cout << "NUMERO MAXIMO: " << final_args[i].num_inicio << std::endl;
        std::cout << "NUMERO MINIMO: " << final_args[i].num_fim << std::endl;

        // Cria a Thread, passando a função fatorial e os argumentos como estrutura em uma posição i do vetor de estruturas
        err = pthread_create(&(tid[i]), NULL, &fatorial, &final_args[i]);
        if (err != 0) std::cout << "Erro ao criar a thread" <<  std::endl; // Caso erro, retornará !=0

        //Inicializa, na próxima struct que irá para a thread, os elementos máximos e mínimo
        final_args[i+1].num_inicio = final_args[i].num_fim - 1;
        final_args[i+1].num_fim = final_args[i+1].num_inicio - fatorial_por_processo;
    }


    //Faz um loop para verificar se todas as Threads foram concluídas
    for (i=0;i<processos;i++){
        result_code = pthread_join(tid[i], 0);
    }

    long double resultado_final = 1;

    //Realiza um loop, mostranso o resultado de cada thread, bem como calcula o resultado final
    for (int j=0; j<processos; j++) {
        std::cout << "THREAD " << j + 1 << ": " << final_args[j].resultados << std::endl;
        resultado_final *= final_args[j].resultados;
    }

    //Mostra o resultado final e calcula o tempo de execução
    std::cout << "\nRESULTADO FINAL --> " << atoi(argv[1]) << "! = " << resultado_final << std::endl;

    return 0;
}