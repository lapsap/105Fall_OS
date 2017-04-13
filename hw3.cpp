#include <stdio.h>
#include <sys/time.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <iostream>
#include <fstream>

using namespace std;

ofstream fout("output1.txt");

struct stu {
	int num;
    int left , right;
};

sem_t sem_done;
sem_t semaphore[16];
pthread_t mythread[16];
vector<int> vec[2];
stu data[16];

void *threadfunc( void* id ) {
	int num = ((stu*) id ) -> num;
	sem_wait(&semaphore[num]);
    int left = ((stu*) id ) -> left;
    int right = ((stu*) id ) -> right;
    if( num*2 < 16 ){
        int piv = left; left++;
        while( left < right ){
            while( vec[0][left] <= vec[0][piv] && left < right ) left++;
            while( vec[0][right] > vec[0][piv] && left < right ) right--;
            swap( vec[0][left] , vec[0][right] );
        }
        swap( vec[0][piv] , vec[0][left-1] );
        data[num*2].left = ((stu*) id ) -> left;
        data[num*2].right = left-2;
        data[num*2+1].left = left -1;
        data[num*2+1].right = ((stu*) id ) -> right;

        sem_post(&semaphore[num*2]);
        sem_post(&semaphore[num*2+1]);

    }else{ 
        sort( vec[0].begin()+left , vec[0].begin()+right+1);
        sem_post(&semaphore[0]);
    }
    pthread_exit(NULL);
}

void *motherthread( void* ) {
    for(int i=1; i<16; i++){
        data[i].num = i;
        sem_init(&semaphore[i], 0, 0);
        pthread_create( &mythread[i], NULL, threadfunc, &data[i] );
    }
    sem_wait(&semaphore[0]); //signal to start sorting
    data[1].left = 0;
    data[1].right = vec[0].size() - 1;
    sem_post(&semaphore[1]);

    for(int i=0;i<8;i++)
        sem_wait(&semaphore[0]);
    sem_post(&sem_done);
    pthread_exit(NULL);
}

int main(void) {
    srand( time(NULL) );
    struct timeval start, end;
    sem_init(&semaphore[0], 0, 0);
    sem_init(&sem_done, 0, 0);
    pthread_create( &mythread[0], NULL, motherthread, NULL );
    string input_file;
    cout << "input file name: ";
    cin >> input_file;
    ifstream fin(input_file.c_str());
    int in, n;
    fin >> n >> in;
    for(int i=0; i<n; fin>>in,i++){
        vec[0].push_back(in);
        vec[1].push_back(in);
    }
    gettimeofday(&start, 0); 
    cout << "Now Counting Multi thread...\n";
    sem_post(&semaphore[0]);

    sem_wait(&sem_done);
    gettimeofday(&end, 0); 
 for( auto c: vec[0] ) fout << c << " ";
    int sec = end.tv_sec - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;
    cout << "MT time " << sec*1000+(usec/1000.0) << "ms" << endl;
    cout << "Now Counting single thread...\n";
    ofstream ffout("output2.txt");
    gettimeofday(&start, 0);
    int num = 1;
    sort( vec[num].begin() , vec[num].end() );
    gettimeofday(&end, 0); 
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    cout << "ST time " << sec*1000+(usec/1000.0) << "ms" << endl;
    for(int i=0; i < vec[num].size() ; i++)
        ffout << vec[num][i] << " ";
    ffout << flush;
    return 0;
}
