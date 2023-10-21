// 프로세스 리딩
#include "process_reading.hpp"
#include<sys/time.h>
#define FILE_MAX 4096
#define FILE_MIN 128
#define PARSING_COUNT 3

proc_list pstat; //process stat 정보 저장 구조체 변수
unsigned long total_memory;

double get_uptime(void)
{
	FILE *fp = fopen("/proc/uptime", "r");
	double res;

	fscanf(fp, "%lf", &res);
	fclose(fp);
	return res;
}
/**
 * @brief 프로세스별 cpu, memory 사용률 계산
 * 
 * @param cur_proc 
 */
void cpu_mem_cal(struct process_status *cur_proc)
{
    FILE *fp = fopen(cur_proc->process_stat_path, "r");
    if (fp == NULL) { // 오류 처리 필요
        printf("%s",cur_proc->process_stat_path);
        perror("Error opening file");
        cur_proc->cpu_usage.push_back("0.000000"); //도중에 종료될 가능성 -> list index 맞춤
        cur_proc->mem_usage.push_back("0.000000");
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
        struct tm t;
        double uptime;
        uptime = get_uptime();
        runtime = pstat.stime + pstat.utime;
        t = *gmtime(&runtime);
        if(!total_memory)
            mem_reading();
        cur_proc->cpu_usage.push_back(to_string(runtime/(uptime*100 - pstat.starttime)/2*100));
        cur_proc->mem_usage.push_back(to_string((double)pstat.rss*4/total_memory*100));
    }
}

void status_parsing(struct process_status *cur_proc)
{
    std::ifstream proc_status(cur_proc->process_status_path); //process_status 정보
    string line;

    if(!proc_status.is_open()){ //오류 처리 필요
        printf("%s",cur_proc->process_status_path);
        perror("Error opening file");
    }
    else{
        while (std::getline(proc_status, line)){
            std::istringstream ss(line);
            std::string key;
            std::string value;

            ss >> key >> value;

            if(key == "Name:")
                cur_proc->Name.push_back(value);
            else if(key == "State:")
                cur_proc->State.push_back(value);
            else if(key == "Pid:")
                cur_proc->Pid.push_back(value);
        }
        proc_status.close();
        cpu_mem_cal(cur_proc);
        // cur_proc->mem_usage.push_back("0");
        // mem_reading(cur_proc);
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
    
    if((count = scandir("/proc",&namelist, NULL, NULL)) == -1 ){
        std::cerr << "scandir error ." << std::endl;
        return 1;
    }

    
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
    free(namelist);
    return -1;
}

double mem_reading(){
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    unsigned long free_memory = 0;

    if (!meminfo.is_open()) {
        std::cerr << "파일을 열 수 없습니다." << std::endl;
        return 0;
    }
    else{
        while (std::getline(meminfo, line)) {// /proc/meminfo 파일에서 메모리 정보 읽기
            std::istringstream ss(line);
            std::string key;
            unsigned long value;
            ss >> key >> value;
            if (key == "MemTotal:") {
                total_memory = value;
            }
            else if (key == "MemFree:") {
                free_memory = value;
            }
        }
        meminfo.close();
        if (total_memory > 0) {
            unsigned long used_memory = total_memory - free_memory;// 메모리 사용량 계산
            double memory_usage = static_cast<double>(used_memory) / total_memory * 100.0;// 메모리 사용률 계산
            // 메모리 정보 출력
            std::cout << "메모리 사용률: " << memory_usage << "%" << std::endl;
            return memory_usage;
        } else { //메모리 출력 오류
            
        }
    }
    return 0;
}
/**
 * @brief dmesg 정보 얻는 함수
 * 
 * @param dmesgLog 
 * @return int value 1 -> fail ,int value 0 -> success  
 */
int log_text(string *dmesgLog)
{
    FILE* pipe = popen("dmesg", "r");
    if (!pipe) {
        std::cerr << "dmesg error" << std::endl;
        return 1;
    }

    char buffer[128];
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        *dmesgLog += buffer;
    }
    
    pclose(pipe);
    
    return 0;
}