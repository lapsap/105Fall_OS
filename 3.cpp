#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

void pa(string cmd1, char **cmd2, int fd, int std){
	pid_t pid = fork();
	
	if( pid <0 ){
		cout << "error \n";
		exit(1);
	}else if( pid == 0 ){
		if( fd != -1){
			if(std == 0 )
				dup2( fd, fileno(stdin) );
			else
				dup2( fd, fileno(stdout) );
		}
		execv( cmd1.c_str() , cmd2);
	}else{
		wait(NULL);
	}
}

void la_do(vector<string> in){
	bool bg = false , lpipe = false;
	char *data[in.size()+1];
	char *data2[in.size()];
	int counter = 0;

	for(auto&& c : in){
		if( c == "|"){
			lpipe = true;
			break;
		}
		data[counter] = new char[c.size()+1];
		strcpy(data[counter], c.c_str());
		counter++;
	}
	if( strcmp( data[counter-1], "&") == 0 ){ 
		bg = true;
		counter--;
	}
	data[counter] = new char();
	data[counter] = NULL;

	if( lpipe ){
		int count = 0;
		for(int i=counter+1; i<in.size(); i++){
			data2[count] = new char[in[i].size()+1];
			strcpy(data2[count], in[i].c_str());
			count++;
		}
		data2[count] = new char();
		data2[count] = NULL;
	}

	char which[] = "/usr/bin/which";
	char *data0[3];
	data0[0] = "which";
	data0[1] = data[0];
	data0[2] = NULL;
	int fd0[2]; pipe(fd0);
	pa( which, data0, fd0[1], fd0[0]);	
	close( fd0[1] );
	char path[90]; 
	int le = read( fd0[0], path, sizeof(path) );
	close( fd0[0] );
	string pat = "";
	for( int i=0; i<le-1; i++)
		pat += path[i];
	if( !lpipe && !bg )
		pa( pat, data, -1 , -1);
	else{
		int fd1[2]; pipe(fd1);
//		pa( pat, data, fd1[1], 1);
		data0[1] = data2[0];
		pa( which, data0, fd1[1], fd1[0] );
		close( fd1[1] );
		le = read( fd1[0], path, sizeof(path) );	
		close( fd1[0] );
		string pat1 = "";
		for( int i=0; i<le-1; i++)
			pat1 += path[i];

		int fd2[2]; pipe(fd2);
		pa( pat, data, fd2[1] , 1);
		close( fd2[1] );
		pa( pat1, data2, fd2[0], 0 );
		close( fd2[0] );

	}

}

int main(){
	cout << ">";
	string in, tmp;
	getline(cin,in);
	while(in!="exit"){
		stringstream ss(in);
		int counter = 0;
		vector<string> cmd;
		while(ss >> tmp, ss){
			counter++;
			cmd.push_back(tmp);
		}   
		la_do(cmd);
		cout << ">";
		getline(cin,in);
	}   
}
