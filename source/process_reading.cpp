// 프로세스 리딩
#include "process_reading.hpp"

#define FILE_MAX 4096
#define FILE_MIN 128
#define PARSING_COUNT 3

void status_parsing(char *);

void status_parsing(char *process_detail)
{
    char Name[FILE_MIN] = "";
    char Pid[FILE_MIN] = "";
    char State[FILE_MIN] = "";

    char *parsing_Pointer;
    if((parsing_Pointer = strstr(process_detail, "Name:"))!=NULL){
        sscanf(parsing_Pointer, "Name:\t%[^\n\r]", Name);
    }
    if((parsing_Pointer = strstr(process_detail, "State:"))!=NULL){
        sscanf(parsing_Pointer, "State:\t%[^\n\r]", State);
    }
    if((parsing_Pointer = strstr(process_detail, "Pid:"))!=NULL){
        sscanf(parsing_Pointer, "Pid:\t%[^\n\r]", Pid);
    }
    printf("\"%s | %s\", ",Pid, Name);
    // printf("%s\n",Name);
    // printf("%s\n",Pid);
    // printf("%s\n",State);
    
}


void find_process()
{
    struct dirent **namelist;
    int count;
    
    if((count = scandir("/proc",&namelist, NULL, NULL)) == -1 ) //크기별 정렬
        printf("error\n");
    
    for(int i=0;i<count;i++){
        if(atoi(namelist[i]->d_name) == 0)
            continue;
        char *cur_process_dir = (char *)malloc(sizeof(char)*128);
        sprintf(cur_process_dir, "/proc/%s", namelist[i]->d_name);
        struct stat sb;
        lstat(cur_process_dir,&sb);

        if(S_ISDIR(sb.st_mode)==1){
            char *status_process_path = (char *)malloc(sizeof(char)*128);
            char status_buf[FILE_MAX] = "";
            char file_status[FILE_MAX] = "";

            strcpy(status_process_path,cur_process_dir);
            strcat(status_process_path,"/status");
            
            FILE *fp = fopen(status_process_path,"r");
            int eof_check = 1;

            while(fgets(status_buf, 4096, fp) != NULL){
                strcat(file_status,status_buf);
            }
            status_parsing(file_status);

            //exit(1);
        }
    }
}
