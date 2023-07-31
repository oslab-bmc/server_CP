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



void find_process();