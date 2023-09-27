// 프로세스 리딩
#include "process_reading.hpp"

#define FILE_MAX 4096
#define FILE_MIN 128
#define PARSING_COUNT 3


void status_parsing(struct process_status *cur_proc)
{
    
    char *parsing_Pointer;
    char buf[FILE_MAX] = "";

    if((parsing_Pointer = strstr(cur_proc->process_detail, "Name:"))!=NULL){
        sscanf(parsing_Pointer, "Name:\t%[^\n\r]", buf);
        string s(buf);
        cur_proc->Name.push_back(s);
        memset(buf,0,sizeof(buf));
    }
    if((parsing_Pointer = strstr(cur_proc->process_detail, "State:"))!=NULL){
        sscanf(parsing_Pointer, "State:\t%[^\n\r]", buf);
        string s(buf);
        cur_proc->State.push_back(s);
        memset(buf,0,sizeof(buf));
    }
    if((parsing_Pointer = strstr(cur_proc->process_detail, "Pid:"))!=NULL){
        sscanf(parsing_Pointer, "Pid:\t%[^\n\r]", buf);
        string s(buf);
        cur_proc->Pid.push_back(s);
        memset(buf,0,sizeof(buf));
    }
    string s = "!23";
    cur_proc->cpu_usage.push_back(s);
    cur_proc->mem_usage.push_back(s);
}


int cur_process_status(struct process_status* cur_proc)
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
            memset(cur_proc->process_detail,0,sizeof(cur_proc->process_detail));

            strcpy(status_process_path,cur_process_dir);
            strcat(status_process_path,"/status");
            
            FILE *fp = fopen(status_process_path,"r");
            if (fp == NULL) {
                perror("Error opening file");
                continue;
            // 오류 처리
            }

            while(fgets(status_buf, 4096, fp) != NULL){
                strcat(cur_proc->process_detail,status_buf);
            }
            status_parsing(cur_proc);
            free(status_process_path);
            fclose(fp);
        }
        free(cur_process_dir);
    }
    return -1;
}
