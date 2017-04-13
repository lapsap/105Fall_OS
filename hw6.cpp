#include<iostream>
#include<fstream>
#include<algorithm>
#include<sstream>
#include<vector>
#include<string.h>
#include<list>
#include<map>
#include<stdlib.h>

using namespace std;

void fifo(){
	cout << "FIFO---\n";
	cout << "size\tmiss\thit\tpage fault ratio\n";
	ifstream fin("trace.txt");
	string in ;
	int hit[4] = {} , miss[4] = {};
	vector<string> table[4];
	map<string,int> table_hash[4];
	while( fin>>in>>in ){
		char tmp[6];
		strncpy( tmp , (char*)in.c_str() , 5 );
		for(int i=0,frame=64; i<4; i++,frame*=2){
			if( table_hash[i][tmp] == 1 ){
				hit[i]++;
			}else if( table_hash[i][tmp] == 0 ){
				miss[i]++;
				if( table[i].size() == frame ){
					table_hash[i][ table[i][0] ] = 0;
					table[i].erase( table[i].begin() );
				}
				table[i].push_back(tmp);
				table_hash[i][tmp] = 1;
			}
		}
		//		if( ++count > 100 ) break;
	}
	for(int i=0,frame=64;i<4;i++,frame*=2)
		cout << frame << "\t" << miss[i] << '\t' << hit[i] << '\t' << (double) miss[i]/(hit[i]+miss[i]) << endl;
	fin.close();
}
/*
void lru(){
	cout << "LRU---\n";
	cout << "size\tmiss\thit\tpage fault ratio\n";
	ifstream fin("trace.txt");
	string in ;
	int hit[4] = {} , miss[4] = {};
	list<string> table[4];
	map<string,int> table_hash[4];
	int count = 0;
	while( fin>>in>>in ){
		char tmp[6];
		strncpy( tmp , (char*)in.c_str() , 5 );
		for(int i=0,frame=64; i<1; i++,frame*=2){
			if( table_hash[i][tmp] > 0 ){
				hit[i]++;
				list<string>::iterator it = table[i].begin();
				for(int r=0; r<table_hash[i][tmp]-1; r++){
					table_hash[i][*it]++;
					it++;
				}
				table[i].erase(it);
				table[i].push_front(tmp);
				table_hash[i][tmp] = 1;
			}else if( table_hash[i][tmp] == 0 ){
				miss[i]++;
				if( table[i].size() == frame ){
//					table_hash[i][ table[i][0] ] = 0;
				table[i].erase( table[i].begin() );
				}
				table[i].push_back(tmp);
				table_hash[i][tmp] = table[i].size() ;
			}
		}
				for(auto c : table[0] )
					cout << c << " ";
				cout << endl ;
				if( ++count > 5 ) break;
	}
	for(auto c : table[0] )
		cout << c << endl;
//	for(int i=0,frame=64;i<4;i++,frame*=2)
//		cout << frame << "\t" << miss[i] << '\t' << hit[i] << '\t' << (double) miss[i]/(hit[i]+miss[i]) << endl;
	fin.close();
}
*/
void lru(){
	cout << "LRU---\n";
	cout << "size\tmiss\thit\tpage fault ratio\n";
	ifstream fin("trace.txt");
	string in ;
	int hit[4] = {} , miss[4] = {};
	list<string> table[4];
	while( fin>>in>>in ){
		char tmp[6];
		strncpy( tmp , (char*)in.c_str() , 5 );
		for(int i=0,frame=64; i<4; i++,frame*=2){
			list<string>::iterator it = find( table[i].begin() , table[i].end() , tmp );
			if( it != table[i].end() ){
				hit[i]++;
				table[i].erase(it);
				table[i].push_front(tmp);	
			}
			else{ 
				miss[i]++;
				if( table[i].size() == frame ){
					it = table[i].end();
					it--;
					table[i].erase( it );
				}
				table[i].push_front(tmp);
			}
		}
	}
	for(int i=0,frame=64;i<4;i++,frame*=2)
		cout << frame << "\t" << miss[i] << '\t' << hit[i] << '\t' << (double) miss[i]/(hit[i]+miss[i]) << endl;
	fin.close();
}
int main(){
	fifo();
	lru();
}
