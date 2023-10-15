#include "signal_reading.hpp"

// ojh
string find_pid(const string& path) {
    FILE *fp;
    char res[MAX_LINE] = {0};
    const string command = "ps | grep " + path + " | grep -v \"grep\" | awk 'NR==1 {print $1}' | tr -d '\n'";

    if ((fp = popen(command.data(), "r")) == NULL) {
        cerr << "popen error for " << command << endl;
        exit(1);
    }

    if (fread(res, MAX_LINE, MAX_LINE, fp) < 0) {
        cerr << "popen error for " << command << endl;
        exit(1);
    }

    pclose(fp);
    return string(res);
}

// ojh
void init_log(const string& pid, const string& path) {
    const string command = "strace -fttt -e trace=signal -s 256 -o " + path + " -p " + pid;
    FILE *fp;

    if ((fp = popen(command.data(), "r")) == NULL) {
        cerr << "popen error for " << command << endl;
        exit(1);
    }
}

// ojh
void *init_thread(void *arg) {
    string target = "server_CP";
    string target_pid = find_pid(target);
    string log_path = "./" + target + "_log.txt";
    init_log(target_pid, log_path);

    return (void *)0;    
}

void ignore_signals() {
    for (int signum = 1; signum < NSIG; signum++) { 
        signal(signum, SIG_IGN);
    }
}

string getTime(const string& ts) {
    istringstream iss(ts);
    string time, ms;
    getline(iss, time, '.');
    getline(iss, ms);
    ostringstream oss;
    time_t timestamp = chrono::system_clock::to_time_t(chrono::system_clock::from_time_t(stoll(time)));
    tm* timeInfo = localtime(&timestamp);
    oss << put_time(timeInfo, "%y-%m-%d %T");
    return oss.str() + ":" + ms;
}