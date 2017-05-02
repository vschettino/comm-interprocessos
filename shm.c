/*
                COMPARTILHANDO MEMORIA ENTRE PROCESSOS

    Neste exemplo ve-se como dois processos podem compartilhar uma
����parte da mem�ria. Lembre-se de que quando um processo realiza
    um fork, um novo processo filho e criado como uma c�pia id�ntica
    das vari�veis do processo pai. Ap�s o fork, o pai e o filho
    podem atualizar suas pr�prias c�pias das vari�veis como quiserem,
    uma vez que n�o compartilham realmente a vari�vel.
    Aqui mostramos como eles podem compartilhar mem�ria, de modo que
    quando um processo atualizar o valor de uma variavel, o outro pode ver
����esse valor.
*/

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>    /*  necessario para a utilizacao da
                            memoria compartilhada
                        */

int main()
{
        int shmid,status;
        int *a, *b;
        int i;

        /*
        O sistema operacional mant�m controle do conjunto de
        segmentos de mem�ria compartilhada. Para criar uma porcao
        de mem�ria compartilhada devemos primeiro solicitar
        essa mem�ria compartilhada ao sistema operacional atraves da
        system call shmget().
        O segundo parametro da funcao passa o o numero de bytes da memoria
        requisitado
        shmget() retorna o identificador da memoria compartilhada(SHMID)
        que e um valor inteiro.

        O terceiro parametro e a permissao 0777 (rwx para todos)
        */
        shmid = shmget(IPC_PRIVATE, 2*sizeof(int), 0777|IPC_CREAT);
                        /* Nos requisitamos 2 arrays de inteiros */

        /*
            Depois do fork, o processo pai e filho devem ser acoplhados "attach"
            na porcao de memoria compartilhada criada. Isso e feito
            pela system call shmat(). shmat() pega o SHMID do segmento memoria
            compartilhada como parametro de entrada e retorna o endereco
            a qual o segmento foi acoplado (attached). Entao shmat() retorna um
            ponteiro char.
        */
        if (fork() == 0) {

                /* Child Process */

                /*  shmat() Retorna um ponteiro char que sofreu um typecast
                para int e armazena esse endere�o no ponteiro int b. */
                b = (int *) shmat(shmid, 0, 0);

                for( i=0; i< 10; i++) {
                        sleep(5);
                        printf("\t\t\t Child reads: %d,%d\n",b[0],b[1]);
                }
                /* cada processo deve se desacoplar ("detach")
                   da memoria compartilhada depois de usa-la */

                shmdt(b);

        }
        else {

                /* Parent Process */

                /*  shmat() retorna um ponteiro char que sofre um typecast
                    para int e armazena o endereo no ponteiro int a.
                    Entao, os enderecos de memoria a[0] e a[1] e b[0] e b[1]
                    de pai e filho sao os mesmos, promovendo o compartilhamento.
                */
                a = (int *) shmat(shmid, 0, 0);

                a[0] = 0; a[1] = 1;
                for( i=0; i< 10; i++) {
                        sleep(1);
                        a[0] = a[0] + a[1];
                        a[1] = a[0] + a[1];
                        printf("Parent writes: %d,%d\n",a[0],a[1]);
                }
                wait(&status);

                /* cada processo deve se desacoplar ("detach")
                   da memoria compartilhada depois de usa-la */

                shmdt(a);

                /* Quando o processo filho (child) sai, o processo pai (parent)
            deve apagar o segmento de memoria compartilhada criada.
            Ao contr�rio de anexar e desanexar, que deve ser feito para
            cada processo separadamente, excluir a mem�ria compartilhada
            tem que ser feito apenas por um processo ap�s ter certeza
            de que ningu�m mais ir� us�-la
                 */

                shmctl(shmid, IPC_RMID, 0);
        }
}
