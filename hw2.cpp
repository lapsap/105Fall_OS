#include <iostream>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int main(){
    int dimension;
    cout << "Input the matrix dimension: "; cin >> dimension; cout << endl << endl;
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * dimension * dimension * 3, IPC_CREAT | 0644);
    void *shmaddr = shmat(shmid, 0, 0 );
    unsigned *mat_a = new (shmaddr) unsigned[dimension*dimension];
    unsigned *mat_b = new (mat_a + dimension * dimension) unsigned[dimension*dimension];
    unsigned *mat_c = new (mat_b + dimension * dimension) unsigned[dimension*dimension];
    for(int i=0;i<dimension*dimension;i++)
        mat_b[i] = mat_a[i] = i;
    for(int process_count=1; process_count<=16; process_count++){
        struct timeval start, end;
        int fstart, fend;
        gettimeofday(&start, 0);

        for(int fork_count=0; fork_count<process_count; fork_count++){
            pid_t pid = fork();
            if( pid == 0 ){
                fstart = dimension / process_count * fork_count; 
                fend = fstart + ( dimension / process_count ) - 1;  
                if( fork_count==process_count-1 && fend!=dimension-1) fend = dimension - 1; //dan shu process
                if( process_count > dimension ){
                    if( fork_count >= dimension )
                        exit(0);
                    fstart = fork_count ;   
                    fend = fstart ;
                }
                shmaddr = shmat(shmid, 0, 0 );
                mat_a = (unsigned*) shmaddr; 
                mat_b = mat_a + dimension * dimension; 
                mat_c = mat_b + dimension * dimension;
                for(int i=fstart; i<=fend; i++)
                    for(int a=0; a<dimension; a++){
                        int sum = 0;
                        for(int c=0; c<dimension; c++)
                            sum += mat_a[i*dimension+c] * mat_b[c*dimension+a];
                        mat_c[i*dimension+a] = sum;
                    }
                shmdt(shmaddr);
                exit(0);
            } 
        }
        wait(NULL);
        gettimeofday(&end, 0);
        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        unsigned checksum = 0;
        for(int i=0; i<dimension*dimension; i++)
            checksum += mat_c[i];
        cout << "Multiplying matrices using " << process_count << " procses\n";
        printf("elapsed %f ms, Checksum: %u\n", sec*1000+(usec/1000.0), checksum);
    }   
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL );
}
