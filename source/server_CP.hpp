#include <iostream>
#include <cpprest/http_listener.h>
#include <cctype>
#include <algorithm>
#include <unistd.h>
#include <vector>   //system call 
#include <time.h>
#include <ctime>
#include <thread>
#include <fstream>
#include <string>

using std::thread;

using namespace std;
//////////////////Set server base url//////////////////////////
//#define SERVER_ADDR "http://192.168.0.101:8000"
#define SERVER_ADDR "http://0.0.0.0:9000"
//////////////////////////////////////////////////////////////

struct stJiffies
{
    int user;
    int nice;
    int system;
    int idle;
};

vector<string> split(string str, char delimiter);
// void request_cnt(string req_kind, clock_t req_time, string path, string req_code);

void cpu_calc(void);
