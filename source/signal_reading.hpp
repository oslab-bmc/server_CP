#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <unistd.h>
#include <signal.h>


#define MAX_LINE 1024

using namespace std;

string find_pid(const string&); // ojh
void init_log(const string&, const string&); // ojh
void *init_thread(void *); // ojh
void ignore_signals(); // ojh
string getTime(const string& ts); // ojh