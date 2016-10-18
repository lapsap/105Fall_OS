#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

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
		data[count] = new char();
		data[count] = NULL;
	}

	int fd[2];	pid_t pid;
	pipe(fd); 	pid = fork();
	
	if( pid < 0 ){
		cout << "ERROR \n";
		exit(1);
	}else if ( pid == 0 ){
		dup2( fd[1], fileno(stdout) );
		close(fd[1]); close(fd[0]);
		execl("/usr/bin/which","which",data[0], NULL);
	}else {
		wait(NULL);
		char fdin[80];
		int length = read( fd[0], fdin, sizeof(fdin) );
		close(fd[1]); close(fd[0]);
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
			if(bg || lpipe)
				dup2( fd2[1], fileno(stdout) );
			char str[] = "hello\nmy love";
//			write ( fd2[1] , str, (strlen(str)+1) );
			close(fd2[1]); close(fd2[0]);
//			_exit(1);
			execv(tok.c_str(),data);
		}else {
			wait(NULL);
			if( lpipe ){
				dup2( fd2[0], fileno(stdin) );
				close(fd2[0]); close(fd2[1]);
	//			execl("/usr/bin/less","less");
				cout << "hh\n";
				close( fileno(stdin) );
			/*
				pid_t pid3 = fork();
				if( pid3 == 0 ){
					close(fd2[0]); close(fd2[1]);
					execl("/bin/ps", "ps");
				}else{
					wait(NULL);
					dup2( fd2[0], fileno(stdin) );	
					close(fd2[0]); close(fd2[1]);
					cout << "DONE \n";
					//execl("/usr/bin/less","less");
				}
			*/
			}else{
				close(fd2[0]); close(fd2[1]);
			}
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
