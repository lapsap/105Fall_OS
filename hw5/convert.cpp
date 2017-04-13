#include <iostream>
#include <fstream>

using namespace std;
#define RECORD_SIZE 4096

int main(){

    ifstream fin("data.txt");
    ofstream fout("new_data.txt");

int count = 0;
    string in; 
    string keys;
    while( getline(fin,in) ){
        for(int i=0;i<4;i++) keys += in[i];
        in.erase(0,4);
        fout << in << endl;
        count ++;
    }
    fout << keys;
}
