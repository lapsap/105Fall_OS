#include<stdlib.h>
#include<stdio.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<wait.h>
#include<sys/time.h>
#include<sys/resource.h>

#define RECORD_SIZE 4096
int search(char* addr , int leng, char*keyword, int start, int end){
// fucking old
    int low=0,  high= (leng/RECORD_SIZE)-1, ret;
    static char buffer[4100];
    static char tmp[5];
    tmp[4] = '\0';
    while (low <= high){
        int mid = (low + high) / 2;
        strncpy(tmp,(addr+mid*RECORD_SIZE),4);
//        printf("copy %s \n" , tmp );
        ret = strcmp(tmp, keyword);
        if (ret == 0){
            memset(buffer, '\0', sizeof(buffer));
            strncpy(buffer, addr + RECORD_SIZE * mid + 4 + start,  end - start + 1);
            printf("key %s found : %s\n", keyword, buffer);
            return mid;
        } else if (ret > 0){
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    printf("key %s not found\n", keyword);
    return -1;
}
int la_search(char* addr , int leng, char*keyword, int start, int end){
    int low=0,  high= (leng/RECORD_SIZE)-1, ret;
    static char buffer[4100];
    static char tmp[5];
    int la_length = (leng/RECORD_SIZE) * (RECORD_SIZE-4);
    tmp[4] = '\0';
    while (low <= high){
        int mid = (low + high) / 2;
        //strncpy(tmp,(addr+mid*RECORD_SIZE),4);
        strncpy(tmp,(addr+la_length+(4*mid) ),4);
        //printf("copy %s \n" , tmp );
        //return 0;
        ret = strcmp(tmp, keyword);
        if (ret == 0){
            memset(buffer, '\0', sizeof(buffer));
            strncpy(buffer, addr + (RECORD_SIZE-4) * mid + start,  end - start + 1);
            printf("key %s found : %s\n", keyword, buffer);
            return mid;
        } else if (ret > 0){
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    printf("key %s not found\n", keyword);
    return -1;
}
void print_max_rss(){
    struct rusage r_stat;
    getrusage(RUSAGE_SELF, &r_stat);
    printf("max rss = %ld\n", r_stat.ru_maxrss);    
}

int main(int argc, char* argv[]){
    char *addr;
    int dataset_fd;
    struct stat sb;
    int N, start, end, ret;
    FILE *testcase ;
    char key[5];
    key[4] = '\0';
   // dataset_fd = open("data.txt", O_RDWR);
    dataset_fd = open("new_data.txt", O_RDWR);
    testcase = fopen(argv[1], "r");
    fstat(dataset_fd,&sb);
    N = sb.st_size; 
    addr = mmap(NULL, N , PROT_READ| PROT_WRITE , MAP_PRIVATE ,dataset_fd , 0);
    while( fscanf(testcase, "%4s %d %d\n", key, &start, &end)!= EOF){
        la_search(addr,N, key, start , end);
        //search(addr,N, key, start , end);
    }
    munmap(addr, N);
    print_max_rss();
    close(dataset_fd);
    return 0;

}
