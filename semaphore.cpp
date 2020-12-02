#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define SENTADO 0
#define PROCURANDO 1
#define QUANTIDADE_DE_JOGADORES 5

sem_t mutex;
sem_t cadeiras[QUANTIDADE_DE_JOGADORES-1];

int jogadores[QUANTIDADE_DE_JOGADORES];

void *jogar(void *id) {
    int *n_jogador = (int *) id;
    // Evitar que duas threads acessem a mesma regiao critica (cadeira)
    sem_wait(&mutex); //mutex = 0
    // Deveria testar em todas as caderias disponiveis? guardar qual o indice da cadeira que ela sentou para ser liberado
    // depois
    procurar(*n_jogador);
    // Caso tenha conseguido sentar, liberar a cadeira alocada
    //levantar(*n_jogador);
}

void procurar(int id) {
    // ??? sem_wait(&mutex);
    // Marcar como procurando
    jogadores[id] = PROCURANDO;
    printf("O jogador %d esta PROCURANDO\n", id+1);

    // testa se eh possivel sentar
    sentar(id);

    sem_post(&mutex);
    sem_wait(&cadeiras[id]);
}

void sentar(int id){
    // Qual cadeira ele vai sentar?
    // ???? sem_wait(&mutex);
    if(jogadores[id] == PROCURANDO && cadeira[outroId] == Disponivel){
        jogadores[id] = SENTADO;
        printf("O jogador %d SENTOU\n", id+1);
        // Marca a cadeira a qual ele sentou como ocupada
        // ???? sem_post(&cadeiras[id]);
    } 
}

// Liberar a cadeira para o proximo round
void levantar(int n_jogador){
    sem_wait(&mutex);
    jogadores[n_jogador] = PROCURANDO;

    sem_post(&mutex);
}

int main(void) {
    pthread_t jogadores[QUANTIDADE_DE_JOGADORES];

    // Cria o mutex como semaforo binario (1, 0)
    sem_init(&mutex, 0, 1);

    for(int i = 0; i < QUANTIDADE_DE_JOGADORES; i++) 
        sem_init(&cadeiras[i], 0, 0);

    printf("********** O JOGO INICIOU **********\n");

    /*!< Sao QTD_jogadores -1 rounds */
    for(int ROUNDS = QUANTIDADE_DE_JOGADORES-1; ROUNDS > 0 ; ROUNDS--) {
        // Sempre havera uma unidade de jogador (threads) a mais que cadeiras (zona critica)
        for(int j = 0; j < ROUNDS+1; j++)
            pthread_create(&jogadores[j], NULL, jogar, &j);

        for(int i = 0; i < QUANTIDADE_DE_JOGADORES; i++)
            pthread_join(jogadores[i], NULL);
    }

    printf("**********O JOGO FINALIZOU**********\n");

    return 0;
}