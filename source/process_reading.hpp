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
#include<fstream>
#include <sstream>

using namespace std;

#define FILE_MAX 4096
#define FILE_MIN 128
#define PARSING_COUNT 3

typedef struct proc_info {
	int pid;			// /proc/pid/stat
	int ppid;			// /porc/pid/stat
	int pgrp, sid, tpgid;		// /proc/pid/stat
	int tty_nr;			// /proc/pid/stat
	char state;			// /proc/pid/stat
	char cmdline[FILE_MIN];		// /porc/pid/cmdline
	char name[64];			// getpwuid..
	unsigned long utime, stime;	// /proc/pid/stat
	unsigned long vsize;		// /proc/pid/stat
	long rss, nice, num_threads;	// /proc/pid/stat
	unsigned long long starttime;	// /proc/pid/stat

	struct proc_info *next;
} proc_list;

struct process_status {
    char process_status_path[FILE_MAX];
    char process_stat_path[FILE_MAX];
    vector<string> Name;
    vector<string> Pid;
    vector<string> State;
    vector<string> cpu_usage;
    vector<string> mem_usage;
};

vector<string> split(string, char);

int cur_process_status(struct process_status *);
void status_parsing(struct process_status *);
double mem_reading();
int log_text(string *);