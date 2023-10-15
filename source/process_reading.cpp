// 프로세스 리딩
#include "process_reading.hpp"
#include<sys/time.h>
#define FILE_MAX 4096
#define FILE_MIN 128
#define PARSING_COUNT 3

proc_list pstat; //process stat 정보 저장 구조체 변수

double get_uptime(void)
{
	FILE *fp = fopen("/proc/uptime", "r");
	double res;

	fscanf(fp, "%lf", &res);
	fclose(fp);
	return res;
}

void cpu_mem_cal(struct process_status *cur_proc)
{
    FILE *fp = fopen(cur_proc->process_stat_path, "r");
    if (fp == NULL) { // 오류 처리 필요
        printf("%s",cur_proc->process_stat_path);
        perror("Error opening file");
    }
    else{
        memset(&pstat, 0, sizeof(proc_list));

        //stat파일에서 정보 읽어오기
        fscanf(fp, "%d %s %c %d %d %d %d %d %*u %*u %*u %*u %*u %lu %lu"
                "%*d %*d %*d %ld %ld %*d %llu %lu %ld",
                &pstat.pid, pstat.cmdline, &pstat.state, &pstat.ppid, &pstat.pgrp,
                &pstat.sid, &pstat.tty_nr, &pstat.tpgid, &pstat.utime, &pstat.stime, 
                &pstat.nice, &pstat.num_threads, &pstat.starttime, 
                &pstat.vsize, &pstat.rss);
        fclose(fp);

        time_t runtime;
        time_t tmp;
        struct tm t;
        double cpu_per,uptime;
        uptime = get_uptime();
        runtime = pstat.stime + pstat.utime;
        t = *gmtime(&runtime);
        
        cur_proc->cpu_usage.push_back(to_string(runtime/(uptime*100 - pstat.starttime)/2*100));
        cur_proc->mem_usage.push_back(to_string(runtime));
    }
}

void status_parsing(struct process_status *cur_proc)
{
    char status_buf[FILE_MAX] = "";
    FILE *fp = fopen(cur_proc->process_status_path,"r");  //process status 정보
    
    char *parsing_Pointer;
    char buf[FILE_MAX] = "";

    if (fp == NULL) { // 오류 처리 필요
        printf("%s",cur_proc->process_status_path);
        perror("Error opening file");
    }
    else{
        while(fgets(status_buf, 4096, fp) != NULL){
            strcat(cur_proc->process_status_path,status_buf);
        }

        fclose(fp);

        if((parsing_Pointer = strstr(cur_proc->process_status_path, "Name:"))!=NULL){
            sscanf(parsing_Pointer, "Name:\t%[^\n\r]", buf);
            string s(buf);
            cur_proc->Name.push_back(s);
            memset(buf,0,sizeof(buf));
        }
        if((parsing_Pointer = strstr(cur_proc->process_status_path, "State:"))!=NULL){
            sscanf(parsing_Pointer, "State:\t%[^\n\r]", buf);
            string s(buf);
            cur_proc->State.push_back(s);
            memset(buf,0,sizeof(buf));
        }
        if((parsing_Pointer = strstr(cur_proc->process_status_path, "Pid:"))!=NULL){
            sscanf(parsing_Pointer, "Pid:\t%[^\n\r]", buf);
            string s(buf);
            cur_proc->Pid.push_back(s);
            memset(buf,0,sizeof(buf));
        }

        cpu_mem_cal(cur_proc);

        string s = "!23";
        // cur_proc->mem_usage.push_back(s);
    }
}

vector<string> split(string input, char delimiter) {
    vector<string> answer;
    stringstream ss(input);
    string temp; 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    } 
    return answer;
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

            memset(cur_proc->process_status_path,0,sizeof(cur_proc->process_status_path)); //init
            memset(cur_proc->process_stat_path,0,sizeof(cur_proc->process_stat_path));

		    sprintf(cur_proc->process_status_path,"%s/%s", cur_process_dir, "status");
		    sprintf(cur_proc->process_stat_path,"%s/%s", cur_process_dir, "stat");
            
            status_parsing(cur_proc);
            
        }
        free(cur_process_dir);
    }
    return -1;
}
