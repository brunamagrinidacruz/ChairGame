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
char entrada;
char letra;

void *jogar(void *id) {
    int *n_jogador = (int *) id;
    
    /*!< Definição de um tempo de sleep() aleatório de forma que o vencedor seja variável */
    /*!< Assim, a thread inicial não será sempre a primeira a entrar na região critica */
   
    int tmp = (rand() % 2) + 1; /*!< A entrada da thread é entre 1 a 2 segundos */
    sleep(tmp);

    /*!< Regiao critica para controlar o acesso a variavel quantidade_de_cadeiras */
    sem_wait(&mutex);

    /*!< Caso ainda existam cadeiras disponiveis, o jogador pode sentar na cadeira */
    if (quantidade_de_cadeiras > 0) {
        printf("    O jogador %d sentou na cadeira\n", *n_jogador);
        quantidade_de_cadeiras--;
    } else {
        /*!< Caso contrario, ele ficou de fora e nao conseguiu sentar */
        printf("    O jogador %d foi desclassificado!\n", *n_jogador);

        /*!< Eliminar o jogador da lista de participantes ainda ativos */
        int i;
        for (i = 0; i < QUANTIDADE_DE_JOGADORES-j; i++) 
            if (n_dos_jogadores[i] == (*n_jogador)) break;
        n_dos_jogadores.erase(n_dos_jogadores.begin() + i);
    }

    sem_post(&mutex);
}

void *jogador(void *id) {
    int *n_jogador = (int *) id;
    
    scanf("%c", &entrada);
    while (entrada != letra)
        scanf("%c", &entrada);
    
    /*!< Regiao critica para controlar o acesso a variavel quantidade_de_cadeiras */
    sem_wait(&mutex);

    /*!< Caso ainda existam cadeiras disponiveis, o jogador pode sentar na cadeira */
    if (quantidade_de_cadeiras > 0) {
        printf("    Você conseguiu sentar na cadeira!\n");
        quantidade_de_cadeiras--;
    } else {
        /*!< Caso contrario, ele ficou de fora e nao conseguiu sentar -> Finalizar o jogo */

        printf("\n\n********** O JOGO FINALIZOU **********\n");
        printf("Você foi desclassificado!\n");
        exit(0);
    }

    sem_post(&mutex);
}

void imprimir_menu() {
    char iniciar;

    printf("Boas-vindas a danca das cadeiras\n");
    printf("*********** INSTRUCOES ***********\n");
    printf("1) O jogo consiste de 5 rodadas\n");
    printf("2) Sao 5 jogadores incluso voce\n");
    printf("3) O ultimo jogador a sentar na rodada perde, ou seja, a cada rodada um jogador sera eliminado\n");
    printf("4) Uma mensagem de aviso de comeco de rodada sera enviado e a partir disto a rodada pode ter inicio em um tempo aleatorio entre 5 a 9 segundos\n");
    printf("5) A cada rodada sera informado uma letra aleatoria que voce deve clicar seguido do ENTER para sentar\n");

    printf("**********************************\n");
    printf("Digite qualquer tecla para iniciar\n");
    scanf("%c", &iniciar);
    printf("**********************************\n");
}

int main(void) {
    pthread_t jogadores[QUANTIDADE_DE_JOGADORES];

    /*!< Inicializando a seed de aleatorio */
    srand (time(NULL));

    /*!< Inicializando id dos jogadores */
    for (int i = 0; i < QUANTIDADE_DE_JOGADORES; i++)
        n_dos_jogadores.push_back(i+1);

    sem_init(&mutex, 0, 1);

    imprimir_menu();

    /*!< Rodadas */
    for(j = 0; j < QUANTIDADE_DE_RODADAS; j++) { 
        printf("\nAguarde o inicio da rodada %d...\n", j+1);
        
        int tmp = (rand() % 5) + 5; /*!< A partida pode iniciar entre 5 a 9 segundos */
        sleep(tmp);

        /*!< Criando letra aleatoria para ser a entrada do usuario */
        letra = 97 + rand() % 25;
        printf("DIGITE '%c' PARA SENTAR!\n", letra);

        /*!< O numero de cadeiras eh sempre menor que o de jogadores */
        quantidade_de_cadeiras = (QUANTIDADE_DE_JOGADORES-1)-j;

        /*!< Criando a thread responsavel pelo jogador */
        pthread_create(&jogadores[0], NULL, jogador, &n_dos_jogadores[0]);
        
        /*!< Os jogadores sao representados por threads e devem acessar as cadeiras (regiao critica) */
        for(int i = 1; i < QUANTIDADE_DE_JOGADORES-j; i++)
            pthread_create(&jogadores[i], NULL, jogar, &n_dos_jogadores[i]);

        /*!< Aguardando as threads terminarem a execucao */
        for(int i = 0; i < QUANTIDADE_DE_JOGADORES-j; i++)
            pthread_join(jogadores[i], NULL);

    }
    
    printf("\n\n********** O JOGO FINALIZOU **********\n");
    printf("Voce venceu!\n");

    return 0;
}
