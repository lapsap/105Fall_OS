#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>

using namespace std;

string la_which(string in){
	cout << "lawhich \n";
    int fd[2];
    pid_t pid;
    pipe(fd);
    if(pid=fork(),pid<0){
        cout << "error \n";
        return 0;
    }else if( pid == 0){
		cout << "fk \n";
		_exit(0);
        dup2(fd[1], fileno(stdout));
        close(fd[1]);
        close(fd[0]);
        //execl("/usr/bin/which","which",in.c_str(), NULL);
        return 0;
    }else{
        wait(NULL);
		_exit(0);
//        waitpid(pid , NULL , 0);
        //close(fd[1]);
        char in[64];
        int length = read(fd[0], in, sizeof(in) );
        in[length] = '\0';
        char *an = new char[length];
        for(int i=0; i<length-1; i++)
			an[i] = in[i];
        return an;
    }

}

void la_do(vector<string> &in){
	cout << "lado \n";
    char *data[in.size()+1];
    int counter = 0;
    
    for(auto&& c : in){
        data[counter] = new char[c.size()+1];
        strcpy(data[counter], c.c_str());
        counter++;
    }
    data[counter] = new char();
    data[counter] = NULL;

    int fd[2];
    pid_t pid;
    pipe(fd);
    if(pid=fork(),pid<0){
        cout << "error \n";
    }else if( pid == 0){
//        dup2(fd[1], fileno(stdout));
        close(fd[0]);
        close(fd[1]);
        string omg = la_which(in[0]);
		_exit(0);
        execv(omg.c_str(), data);
    }else{
        wait(NULL);
//        waitpid(pid , NULL , 0);
        char in[2048];
//		cout << "be4\n";
 //       int length = read(fd[0], in, sizeof(in) );
//		cout << "after\n";
//        in[length] = '\0';
  //      if(in!=NULL)cout << in ;
    }
}

int main(){
    //	cout << la_which("ls") ;
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
