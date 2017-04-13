#include <stdio.h>
#include <sys/time.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <iostream>
#include <fstream>
#include <signal.h>

using namespace std;
vector<int> vec;
sem_t sem[16] , sem_done;
pthread_t pthread[16];
int T[16] = {};
pair<int,int> data[16];

struct stu_thread{
    int id;
    int layer = 0;
    bool godie = false;
    bool busy = false;
};

void *test( void* v ){
    while( !((stu_thread*) v) -> godie ){
        sem_wait( &sem[((stu_thread*)v)->id] );
        if( ((stu_thread*) v) -> godie ) break;
        int layer = ((stu_thread*)v)-> layer;
        int left = data[layer].first;
        int right = data[layer].second;
        //cout <<"layer " << layer << " l " << left << " r " << right << endl;
        //for(int i=0;i<10;i++) cout << i << " ";
        //for(auto c : vec ) cout << c << " ";
       // cout <<endl;
        if( layer < 8 ){
            if( left == right ){
                data[layer*2].first = left; 
                data[layer*2].second = left;
                data[layer*2+1].first = left;
                data[layer*2+1].second = left;
            }else{
                int piv = left; left++;
                while( left < right ){
                    while( vec[left] <= vec[piv] && left < right ) left++;
                    while( vec[right] > vec[piv] && left < right ) right--;
                    swap( vec[left] , vec[right] );
                }
                //swap( vec[piv] , vec[left-1] );
                if( piv != left-1 ) swap( vec[piv] , vec[left-1] );
                else if(vec[piv] > vec[left] )  swap( vec[piv] , vec[left] );
                //else cout << " fuc \n";
                //cout << left << " | " << right << endl;
                data[layer*2].first = data[layer].first;
                data[layer*2].second = left-1;
                data[layer*2+1].first = left;
                data[layer*2+1].second = data[layer].second;
            }
        }else{
//            cout << "left " << left << " rigtht " << right << "  | ";
//            for(int i=left; i<=right; i++)
//                cout << vec[i] << " ";
            for(int i=left; i<=right; i++){
                for(int a=left; a<right; a++){
                    if( vec[a] > vec[a+1] )
                        swap( vec[a] , vec[a+1] );
                }
            }
            //cout << endl;
        }
        //        cout << "thread " << ((stu_thread*) v) -> layer << " set T to 2 \n";
        T[ ((stu_thread*) v) -> layer ] = 2;
        ((stu_thread*) v) -> busy = false;
    }
    //cout << "DIEE \n";
    pthread_exit(NULL);
}

int main(){
    struct timeval start , end;
    ifstream fin("input.txt");
    int in, n;
    fin >> n >> in;
    vector<int> in_data;
    for(int i=0;i<n; fin>>in,i++)
        in_data.push_back(in);
    for(int numofthread=1; numofthread<=8; numofthread++){
        for(int i=0;i<16;i++){
            T[i] = false;
            data[i] = make_pair(-1,-1);
        }
        string outpath = "output_" + to_string(numofthread) + ".txt";
        ofstream fout(outpath);
        gettimeofday(&start,0);
        vec = in_data;
        stu_thread *thread = new stu_thread[numofthread];
        sem_init( &sem_done ,0,0);
        for(int i=0;i<numofthread;i++){
            sem_init(&sem[i],0,0);
            thread[i].id = i;
            pthread_create(&pthread[i],NULL,test,&thread[i]);
        }
        data[1] = make_pair( 0 , vec.size()-1);
        for(int i=1; i<16; i++){
            //            sem_wait( &sem_done );
            //            for(auto c:T) cout << c << " ";
            //            cout << endl;
            int pick = -1 ;
            while(1 ){
                for(int a=0; a<numofthread; a++){
                    if( !thread[a].busy ){
                        pick = a;
                        break;
                    }
                }
                if( pick != -1 ) break;
                // cout << "still picking \n";
                // sleep(1);
            }
            int layer = -1 ;
            for(int a=1;a<16;a++){
                if( T[a] == 0 ){
                    if( (a>1 && T[a/2]==2) || a==1 ){
                        layer = a;
                        break;
                    }
                }
            }
            if( layer == -1 ){
                //cout << "layer is -1 \n";
                i--;
                continue;
            }
            //cout << "thread " << pick << " do layer " << layer << endl;
            thread[pick].layer = layer;
            T[layer] = 1;
            thread[pick].busy = true;
            sem_post( &sem[pick] );
        }
        int tmpb = false;
        while(1){
            tmpb = true;
            for(int i=1; i<16;i++)
                if( T[i] != 2 ){ 
                    tmpb = false;
                    break;
                }
            if( tmpb ) break;
        }
        gettimeofday(&end,0);
        for(auto c:vec) fout << c << " ";
        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        cout << numofthread << " thread time " << sec*1000+(usec/1000.0) << "ms" << endl;
        //cout << numofthread << " thread time " << sec << "s" << endl;

        //
        fout.close();
        for(int i=0;i<numofthread;i++){
            thread[i].godie = true;
            sem_post( &sem[i] );
            pthread_join( pthread[i] , NULL);
            sem_destroy(&sem[i]);
        }
        sem_destroy( &sem_done );
        delete [] thread;
//        for(auto c:vec) cout << c << " ";
//        cout << endl;
    }

}
