#include <iostream>
#include <vector>
#include <unistd.h>

using namespace std;

/*!< Thread */
#include <pthread.h>
#include <semaphore.h>

/*!< Rand */
#include <stdlib.h> 
#include <time.h>       

#define QUANTIDADE_DE_JOGADORES 5
#define QUANTIDADE_DE_RODADAS QUANTIDADE_DE_JOGADORES - 1

vector<int> n_dos_jogadores;

sem_t mutex;

int j;
int quantidade_de_cadeiras;

void *jogar(void *id) {
    int *n_jogador = (int *) id;
    
    /*!< Definição de um tempo de sleep() aleatório de forma que o vencedor seja variável */
    /*!< Assim, a thread1 não será sempre a primeira a entrar na região critica */
    int tmp = (rand() % 3); /*!< A entrada da thread é entre entre 0 a 2 segundos */
    sleep(tmp);

    /*!< Regiao critica para controlar o acesso a variavel quantidade_de_cadeiras */
    sem_wait(&mutex);

    /*!< Caso ainda existam cadeiras disponiveis, o jogador pode sentar na cadeira */
    if (quantidade_de_cadeiras > 0) {
        printf("    O jogador %d sentou na cadeira\n", *n_jogador);
        quantidade_de_cadeiras--;
    } else {
        /*!< Caso contrario, ele ficou de fora e nao conseguiu sentar */
        
        int i;
        /*!< Eliminar o jogador da lista de participantes ainda ativos */
        printf("    O jogador %d foi desclassificado!\n", *n_jogador);

        for (i = 0; i < QUANTIDADE_DE_JOGADORES-j; i++) 
            if (n_dos_jogadores[i] == (*n_jogador)) break;
        n_dos_jogadores.erase(n_dos_jogadores.begin() + i);
    }

    sem_post(&mutex);
}

void *jogador(void *id) {
    int *n_jogador = (int *) id;
    
    //fflush(stdin); //setbuf(stdin, NULL);
    // char c;
    // while ((c = getchar()) != '\n' && c != EOF) {}

    char response;
    scanf("%c", &response);
    
    /*!< Regiao critica para controlar o acesso a variavel quantidade_de_cadeiras */
    sem_wait(&mutex);

    /*!< Caso ainda existam cadeiras disponiveis, o jogador pode sentar na cadeira */
    if (quantidade_de_cadeiras > 0) {
        printf("    Você conseguiu sentar na cadeira!\n");
        quantidade_de_cadeiras--;
    } else {
        /*!< Caso contrario, ele ficou de fora e nao conseguiu sentar -> Finalizar o jogo */
        printf("Você foi desclassificado!\n");
        exit(0);
    }

    sem_post(&mutex);
}

int main(void) {
    /* initialize random seed: */
    srand (time(NULL));

    pthread_t jogadores[QUANTIDADE_DE_JOGADORES];

    printf("Boas-vindas a danca das cadeiras\n");
    printf("*********** INSTRUCOES ***********\n");
    printf("1) A cada rodada voce devera sentar em uma cadeira, para tal, pressione qualquer botao + ENTER\n");
    printf("2) Aquele que nao sentar, ou seja, ficar em pe, sera desclassificado. SEJA O MAIS VELOZ!\n");
    printf("3) Serao 5 rounds e voce ira competir contra o computador\n");
    printf("4) Para vencer voce nao pode ser o ultimo a sentar em cada uma das rodadas\n");
    printf("Digite qualquer tecla para iniciar\n");
    printf("**********************************\n");
    
    char response;
    scanf("%c", &response);

    /*!< Inicializando id dos jogadores */
    for (int i = 0; i < QUANTIDADE_DE_JOGADORES; i++)
        n_dos_jogadores.push_back(i+1);

    sem_init(&mutex, 0, 1);

    printf("*********** O JOGO INICIOU ***********\n");

    /*!< Rodadas */
    for(j = 0; j < QUANTIDADE_DE_RODADAS; j++) { 
        printf("Aguarde o inicio da rodada %d...\n", j+1);
        
        int tmp = (rand() % 5) + 5; /*!< A partida pode iniciar entre 5 a 9 segundos */
        sleep(tmp);
    
        fflush(stdin);
        printf("DIGITE PARA SENTAR!\n");
        /*!< O numero de cadeiras eh sempre menor que o de jogadores */
        quantidade_de_cadeiras = (QUANTIDADE_DE_JOGADORES-1)-j;

        // setbuf(stdin, NULL);
        
        pthread_create(&jogadores[0], NULL, jogador, &n_dos_jogadores[0]);
        
        /*!< Os jogadores sao representados por threads e devem acessar as cadeiras (regiao critica) */
        for(int i = 1; i < QUANTIDADE_DE_JOGADORES-j; i++)
            pthread_create(&jogadores[i], NULL, jogar, &n_dos_jogadores[i]);

        for(int i = 0; i < QUANTIDADE_DE_JOGADORES-j; i++)
            pthread_join(jogadores[i], NULL);

    }
    
    printf("********** O JOGO FINALIZOU **********\n");
    printf("Voce venceu!\n");

    return 0;
}

/*
    for (int vetor : variavel) {

    }
*/
