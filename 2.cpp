#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

void la_do(vector<string> in){
	bool bg = false;
	char *data[in.size()+1];
	int counter = 0;

	for(auto&& c : in){
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

	int fd[2];	pid_t pid;
	pipe(fd); 	pid = fork();
	
	if( pid < 0 ){
		cout << "ERROR \n";
		exit(1);
	}else if ( pid == 0 ){
		dup2( fd[1], fileno(stdout) );
		close(fd[1]);
		close(fd[0]);
		execl("/usr/bin/which","which",data[0], NULL);
	}else {
		wait(NULL);
		char fdin[80];
		int length = read( fd[0], fdin, sizeof(fdin) );
		string tok = "";
		for(int i=0;i<length-1;i++)
			tok += fdin[i];
		// execute command
		int fd2[2]; pipe(fd2);
		pid_t pid2 = fork();
		if( pid2 < 0){
			cout << "ERROR2 \n";
			exit(1);
		}else if ( pid2 == 0 ){
			if(bg)
				dup2( fd[1], fileno(stdout) );
			close(fd[1]); close(fd[0]);
			execv(tok.c_str(),data);
		}else {
			wait(NULL);
		}
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
