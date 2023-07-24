#include <iostream>
#include <cpprest/http_listener.h>
#include <fstream>
#include <string>
#include "json/json.h"
#include <cctype>
#include <algorithm>
#include <unistd.h>
#include <vector>   //system call 
#include<time.h>
#include<ctime>
#include <thread>
using std::thread;

using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;
class P_handler
{
    public:
        P_handler(){};
        P_handler(utility::string_t url);
        pplx::task<void> open() { return listener.open(); }
        pplx::task<void> close() { return listener.close(); }

        ~P_handler(){};
    private:
        http_listener listener;
        void handleGet(http_request request);
        void handlePost(http_request request);
        void handleDelete(http_request request);
        void handlePut(http_request request);
        void handlePatch(http_request request);
};