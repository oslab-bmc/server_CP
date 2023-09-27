#include<cstdio>
#include<dirent.h>
#include<sys/stat.h>//stat속성을 불러오기위함
#include<string.h>//문자열 함수를 불러오기위함
#include<stdlib.h>
#include<sys/time.h>//time속성을 내장하고있는 헤더
#include<time.h>
#include<sys/types.h>
#include<fcntl.h>//oflag를 사용하기위함
#include <unistd.h>
#include<sys/termios.h>
#include<vector>
#include<string>
#include<iostream>

using namespace std;

#define FILE_MAX 4096
#define FILE_MIN 128
#define PARSING_COUNT 3


struct process_status {
    char process_detail[FILE_MAX];
    vector<string> Name;
    vector<string> Pid;
    vector<string> State;
    vector<string> cpu_usage;
    vector<string> mem_usage;
};

int cur_process_status(struct process_status *);
void status_parsing(struct process_status *);