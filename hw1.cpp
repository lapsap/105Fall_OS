#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

void kill_child( int signum ) {
	int status;
	waitpid( -1, &status, WNOHANG );
}

void pa(string cmd1, char **cmd2, int fd, int std, bool bg){
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
        int status;
        if(!bg) waitpid(pid, &status, 0);
    }
}

void la_do(vector<string> in){
    bool bg = false , lpipe = false , lless = false, lmore = false;
    char *data[in.size()+1];
    char *data2[in.size()];
    int counter = 0;

    for(auto&& c : in){
        if( c == "|"){
            lpipe = true;
            break;
        }else if ( c == "<"){
            lless = true;
            break;
        }else if( c == ">"){
            lmore = true;
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

    if( lpipe || lless || lmore ){
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
    pa( which, data0, fd0[1], fd0[0] ,false);	
    close( fd0[1] );
    char path[90]; 
    int le = read( fd0[0], path, sizeof(path) );
    close( fd0[0] );
    string pat = "";
    for( int i=0; i<le-1; i++)
        pat += path[i];
    if( !lpipe && !bg && !lless && !lmore )
        pa( pat, data, -1 , -1, false);  //exec without pipe
    else if(bg)
        pa( pat, data, -1, -1, true);
    else if(lpipe) {
        int fd1[2]; pipe(fd1);
        //		pa( pat, data, fd1[1], 1);
        data0[1] = data2[0];
        pa( which, data0, fd1[1], fd1[0], false );
        close( fd1[1] );
        le = read( fd1[0], path, sizeof(path) );	
        close( fd1[0] );
        string pat1 = "";
        for( int i=0; i<le-1; i++)
            pat1 += path[i];

        int fd2[2]; pipe(fd2);
        pa( pat, data, fd2[1] , 1, false);
        close( fd2[1] );
        pa( pat1, data2, fd2[0], 0 , false );
        close( fd2[0] );

    }else if(lmore){
        ofstream fout(data2[0]);
        int fd1[2]; pipe(fd1);
        pa( pat, data, fd1[1], 1 ,false );
        close( fd1[1] );
        char str[10000];
        int le = read( fd1[0], str, sizeof(str) );
        close( fd1[0] );
        str[le] = '\0';
        fout << str;
        fout.close();
    }else if(lless){
        ifstream fin(data2[0]);
        int fd1[2]; pipe(fd1);
        string tmp;
        string buff;
        while( getline( fin, tmp) )
            buff += tmp + '\n';
        write( fd1[1], buff.c_str(), (strlen(buff.c_str())+1) );
        close( fd1[1] );
        pa( pat, data, fd1[0], 0 ,false );

    }

}

vector<string> la_split(string in){
    vector<string> result;
    string tmp = "";
    for(int i=0; i<in.size(); i++){
        if( in[i] == ' '){
            if(tmp!="") result.push_back(tmp);
            tmp = "";
        }else if( in[i] == '|' || in[i] == '>' || in[i] == '<' ){
            if(tmp!="") result.push_back(tmp);
            tmp = "";
            tmp += in[i];
            result.push_back(tmp);
            tmp = "";
        }else
            tmp += in[i];
    }
    if( tmp != "" )
        result.push_back(tmp);

    return result;
}

int main(){
    signal(SIGCHLD, kill_child);
    cout << ">";
    string in, tmp;
    getline(cin,in);
    while(in!="exit"){
        vector<string> cmd = la_split(in);
        la_do(cmd);
        cout << ">";
        getline(cin,in);
    }   
}
