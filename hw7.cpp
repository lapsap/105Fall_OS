#include <dirent.h> 
#include <vector>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h> 
#include <iostream>
#include <sys/stat.h>

using namespace std;

void la_dir( string path , string mode , string value , double la_max , double la_min , string inode){
    DIR           *d;
    struct dirent *dir;
    string tmp;
    d = opendir( path.c_str() );
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            tmp = path + '/' + dir->d_name;
            char *fd = (char*)tmp.c_str();
            struct stat *buf;
            buf = (struct stat*)malloc(sizeof(struct stat));

            stat(fd, buf);
            int size = buf->st_size;
            double size_mb = size / ( pow(2,20) );
            if( mode == "all" || (mode=="-name"&&value==dir->d_name) )
                if( la_max >= size_mb && size_mb >= la_min )
                    if( inode == "-1" || inode == to_string(dir->d_ino) ) 
                        if( strncmp(dir->d_name, ".",1) != 0 && strncmp(dir->d_name, "..", 2) != 0 )
                            cout << tmp << '\t' << dir->d_ino << '\t' << size_mb << " MB" << endl;
            free(buf);
            if( strncmp(dir->d_name, ".",1) != 0 && strncmp(dir->d_name, "..", 2) != 0 ){
                la_dir( tmp , mode , value, la_max, la_min, inode);
            }
        }

        closedir(d);
    } 
}

int main(int argc, char** argv) 
{
    if( argc < 4 ){
        cout << "Too little arguments\n";
        return 0;
    }
    vector<string> vec;
    bool b_min = false, b_max = false, b_inode = false, gotsize = false, gotinode = false;
    double la_min = -1, la_max = 99999999;
    string path = argv[1] , inode = "-1";
    for(int i=2; i<argc; i++){
        if( b_min ){
            la_min = atoi(argv[i]);
            b_min = false;
        }else if( b_max ){
            la_max = atoi(argv[i]);
            b_max = false;
        }else if( b_inode ){
            inode = argv[i];
            b_inode = false;
        }else if( strncmp(argv[i],"-size_min",9) == 0 ){
            b_min = true;
            gotsize = true;
        }
        else if( strncmp(argv[i],"-size_max",9) == 0 ){
            b_max = true;
            gotsize = true;
        }
        else if( strncmp(argv[i],"-inode",6) == 0 ){
            b_inode = true;
            gotinode = true;
        }
        else 
            vec.push_back(argv[i]);
    }
    for(int i=0; i<vec.size(); i+=2){
        la_dir( path , vec[i] , vec[i+1], la_max, la_min , inode );
    }
    if( (gotinode||gotsize) && vec.size() == 0 )
        la_dir( path , "all" , "hehe" , la_max, la_min , inode);
    //la_dir( "./WINDOWS" , "all" , "100" , 2.7 , -1 );
    //la_dir( "./WINDOWS" , "name" , "one" );
    //la_dir( "./WINDOWS" , "inode" , "1361674880" );

}
