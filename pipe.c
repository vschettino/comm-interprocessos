/*
                        COMUNICA��O VIA PIPE

    Considere uma aplica��o que requer um processo para escrever um conjunto de
����valores que devem ser lidos por outro processo. A sa�da de um � a entrada para outro
    O Pipe facilita essa comunica��o. Um pipe, ou tubo, � tamb�m um buffer compartilhado.
    Quando um processo tenta ler a partir de um pipe vazio, ele espera at� que
    algu�m tenha escrito algo no pipe. Quando o pipe est� cheio, qualquer processo
    que tente escrever no pipe tem que esperar.

*/

#include <stdio.h>
#include <unistd.h>     /* Para utilzar pipe atente-se aos headers necess�rios */

#define BUFSIZE 80      /* vamos escrever linhas de at� 80 chars no pipe. */

main()
{
        int fd[2];
        int n=0;
        int i;
        char line[BUFSIZE];

        /*  O pipe � tratado como um arquivo de sistema. Entao, voce deve usar
            fopen() para abrir o arquivo. fopen() retorno um "file pointer" que
            e usado no fprintf(), fscanf(), fclose() etc. Entretanto, quando nos
            queremos realizar leituras e gravacoes em blocos para um arquivo, nos
            usamos a system call "open" para abrir o arquivo. Internamente, arquivos
            sao sempre aberto usando a chamada de sistema "open". Para cada processo
            o sistema mantem uma tabela "file descriptor table" (FDT) contendo uma
            entarda para cada arquivo aberto por aquele processo. Quanod um novo arquivo
            e aberto, uma nova entrada e criada na FDT, e um numero de entarda e
            retornado como um inteiro, o que chamamos de "file descriptor".

            Ao contr�rio de um arquivo, podemos querer ler e escrever no pipe ao mesmo tempo.
            Assim, quando um pipe � criado, dois arquivos sao criados (dois descritores)
            - um para ler o pipe e outro para escrever no pipe. A chamada de sistema pipe()
            requer um array de dois inteiros como par�metro. O sistema retorna os arquivos
        descritores atrav�s desse array.

        */
        pipe(fd);  /* fd[0] para leitura,
                      fd[1] para escrita/gravacao
                    */
                            /*
        Para ilustrar o funcionamento do pipe, faremos com que o processo filho (child)
    escreva o "inteiro n" no pipe e faca o pai ler a partir do pipe.
    Colocamos um "sleep" no processo de escrita(neste caso, o child)
    para mostrar que o processo leitor espera o processo de escrita
    escrever algo no pipe.

    Para escrever um bloco de bytes no pipe (ou mais genericamente, em um arquivo)
    a chamada de sistema write() � usada. Similarmente, a funcao read() e usada para
    ler um bloco de bytes de um arquivo. Consulte o man para mais detalhes sobre
    essas chamadas.
    */

        if (fork() == 0) {//P1

                close(fd[0]); /* o processo filho nao vai ler e
                        consequentemente fehcamos fd[0]
                                */

                for (i=0; i < 10; i++) {

                        sprintf(line,"%d",n); /* write() so aceita arrays de bytes,
                                nos primeiramente escrevemos o inteiro n
                                no char "line" do array
                                                  */
                        write(fd[1], line, BUFSIZE);
                        printf("Filho (P1) escreve: %d\n",n);
                        n++;
                        sleep(2);
                }
        }
        else {//P2 E P3

                close(fd[1]); /* o pai nao vai escrever nada
                        entao fechamos o fd[1]
                                */
                if(fork() == 0){//P3
                    for (i=0; i < 10; i++) {

                            printf("\t\t\t\t\t\t (P3) tentando ler o pipe\n");
                            read(fd[0], line, BUFSIZE);
                            sscanf(line,"%d",&n); /* Leia o inteiro da "line" de caracteres
                                    lidos do pipe                                                      */
                            printf("\t\t\t\t\t\t  (P3) le: %d\n",n);
                            sleep(2);
                    }
                }
                else{//P2
                  for (i=0; i < 10; i++) {

                          printf("\t\t\t (P2) tentando ler o pipe\n");
                          read(fd[0], line, BUFSIZE);
                          sscanf(line,"%d",&n); /* Leia o inteiro da "line" de caracteres
                                  lidos do pipe                                                      */
                          printf("\t\t\t (P2) le: %d\n",n);
                          sleep(1);
                  }
                }

        }
}
