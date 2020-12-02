#include <iostream>
#include <vector>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define QUANTIDADE_DE_JOGADORES 5
#define QUANTIDADE_DE_RODADAS QUANTIDADE_DE_JOGADORES-1

vector<int> n_dos_jogadores;

sem_t mutex;

int j;
int quantidade_de_cadeiras;

void *jogar(void *id) {
    int *n_jogador = (int *) id;

    /*!< Regiao critica para controlar o acesso a variavel quantidade_de_cadeiras */
    sem_wait(&mutex);

    // Caso ainda existam cadeiras disponiveis, o jogador pode sentar na cadeira
    if (quantidade_de_cadeiras > 0) {
        printf("O jogador %d sentou na cadeira\n", *n_jogador);
        quantidade_de_cadeiras--;
    } else {
        // Caso contrario, ele ficou de fora e nao conseguiu sentar
        
        int i;
        // Eliminar o jogador da lista de participantes ainda ativos
        printf("O jogador %d foi desclassificado!\n", *n_jogador);

        for (i = 0; i < QUANTIDADE_DE_JOGADORES-j; i++) 
            if (n_dos_jogadores[i] == (*n_jogador)) break;
        n_dos_jogadores.erase(n_dos_jogadores.begin()+i);
    }

    sem_post(&mutex);
}

int main(void) {
    pthread_t jogadores[QUANTIDADE_DE_JOGADORES];

    /*!< Inicializando id dos jogadores */
    for (int i = 0; i < QUANTIDADE_DE_JOGADORES; i++)
        n_dos_jogadores.push_back(i+1);

    sem_init(&mutex, 0, 1);

    printf("********** O JOGO INICIOU **********\n");

    /*!< Rodadas */
    for(j = 0; j < QUANTIDADE_DE_RODADAS; j++) {
        printf("-> RODADA %d\n", j+1);
        // O numero de cadeiras eh sempre menor que o de jogadores
        quantidade_de_cadeiras = (QUANTIDADE_DE_JOGADORES-1)-j;
        
        // Os jogadores sao representados por threads e devem acessar as cadeiras (regiao critica)
        for(int i = 0; i < QUANTIDADE_DE_JOGADORES-j; i++)
            pthread_create(&jogadores[i], NULL, jogar, &n_dos_jogadores[i]);

        for(int i = 0; i < QUANTIDADE_DE_JOGADORES-j; i++)
            pthread_join(jogadores[i], NULL);
    }

    printf("********** O JOGO FINALIZOU **********\n");
    printf("O vencedor é: %d\n", n_dos_jogadores[0]);

    return 0;
}