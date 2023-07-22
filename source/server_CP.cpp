#include <iostream>
#include <cpprest/http_listener.h>
#include <fstream>
#include <string>
#include "json/json.h"
#include "jsoncpp.cpp"
// #include <json.h>
#include <cctype>
#include <algorithm>
#include <unistd.h>
#include <vector>   //system call 
#include<time.h>
#include<ctime>


using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;
#define TRACE(msg) wcout << msg

//////////////////Set server base url//////////////////////////
// #define SERVER_ADDR "http://192.168.0.101:8000" [oslab]
#define SERVER_ADDR "http://203.253.25.207:8000"
//////////////////////////////////////////////////////////////
vector<string> split(string str, char delimiter);
// void request_cnt(string req_kind, clock_t req_time, string path, string req_code);

vector<string> split(string input, char delimiter) {
    vector<string> answer;
    stringstream ss(input);
    string temp; 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    } 
    return answer;
}
int fan_status = 0;

// void request_cnt(string req_kind, clock_t req_time, string path, string req_code){
//     int cnt = 0;
//     std::ifstream json_file;
//     json_file.open("./output.json", std::ifstream::in|std::ifstream::binary);
//     // 이처럼 open함수를 이용할 수 있으며, open mode는 | 연산자를 이용하여 여러 모드 가능
    
//     Json::Value root, cnt_js, t_json;
//     json_file >> root;
//     json_file.close();
//     // open한 file에서 얻어온 내용을 root에 대입하여 Json::Value로 사용
//     cnt_js = root[req_kind+" "+path+" "+req_code];

//     cnt = cnt_js["Count"].asDouble()+(double)1;
//     root[req_kind+" "+path+" "+req_code]["Count"] = cnt;
//     t_json["response_time_"+to_string(cnt)]=((double)req_time);
//     root[req_kind+" "+path+" "+req_code]["Time"].append(t_json);

//     // 원하는 수정을 거친 후 formating json
//     Json::StyledWriter writer;
//     string str = writer.write(root);

//     // output to json file
//     std::ofstream output_file("output.json");
//     output_file << str;
//     output_file.close();
// }
void handleGet(http_request request)
{
    string pp = uri::decode(request.relative_uri().path());
    auto path = uri::split_path(pp);

    if (!path.empty() && path[0] == "fan")
    {
        // TODO: fan정보 리턴 구현
        TRACE(L"\n/fan\n");
        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U(fan_status));
        sleep(1);
        //request.reply(status_codes::OK, );
        request.reply(response);
    }
    else if(!path.empty() && path[0] == "request" && path[1] == "report"){
        std::ifstream json_file;
        json_file.open("./output.json", std::ifstream::in|std::ifstream::binary);
        // 이처럼 open함수를 이용할 수 있으며, open mode는 | 연산자를 이용하여 여러 모드 가능
        
        string str;
        Json::Value root;
        json_file >> root;
        json_file.close();
        
        Json::StyledWriter writer;
        str = writer.write(root);

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U(str));
        request.reply(response);
    }
    else if (!path.empty() && path[0] == "cpuinfo" && path[1] == "on")
    {   
        TRACE(L"\n/[GET] :: cpuinfo\n");
        string readline;
        string k, v;
        string delimiter = ":";
        size_t pos = 0;
        Json::Value root, processor;
        string str;
        int64_t index = 0;

        ifstream file("/proc/cpuinfo");
        if (file.is_open())
        {
            while (getline(file, readline))
            {
                cout<<"DATA:: "<<readline<<"\n";
                readline.erase(std::remove_if(readline.begin(), readline.end(), [](unsigned char x) { return std::isspace(x); }), readline.end());
                if ((pos = readline.find(delimiter)) != std::string::npos)
                {
                    k = readline.substr(0, pos);
                    readline.erase(0, pos + delimiter.length());
                    v = readline;
                }
                if (k.compare("processor") == 0 && index == 0)
                {
                    index++;
                    processor[k] = v;
                    continue;
                }
                else if (k.compare("processor") == 0 && index != 0)
                {
                    root["processor " + to_string(index++ - 1)] = processor;
                    processor[k] = v;
                }
                else
                {
                    processor[k] = v;
                }
            }
            root["processor " + to_string(index - 1)] = processor;
            file.close();
        }
        // cout << "==============CPUINFO===================" << endl;
        Json::StyledWriter writer;
        str = writer.write(root);

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U(str));
        request.reply(response);
    }
    else if(!path.empty() && path[0] == "stat_mem" && path[1] == "result")
    {
        string indexArry[10] = {"user", "nice", "system", "idle", "iowait","irq","softirq","steal","guest","guest_nice"};
        string readLine, readLine2, k, v, delimiter =":", str;  
        size_t pos = 0;
        int64_t index = 0; 
        
        Json::Value root, cpu_usage_data, memory_usage_data, semi;

        ifstream file("/proc/stat");
        
        if(file.is_open()){
            while(getline(file, readLine)){
                
                if(readLine.find("intr") == string::npos){
                    vector<string> result = split(readLine, ' ');
                    readLine2 = readLine.erase(0,result[0].length());
                    vector<string> result2 = split(readLine, ' ');
                    if(result2.size() == 11){
                    for(int i = 0; i < 10; i++){
                        semi[indexArry[i]] = result2.at(i+1);
                    }
                    }else{
                        // int j = 0;
                        for(int i = 0; i < 10; i++){
                            semi[indexArry[i]] = result2.at(i+2);
                        }
                    }
                    cpu_usage_data[result[0]] = semi;
                }else{
                    break;
                }

            }       
            
            file.close(); 
        }
        ifstream file2("/proc/meminfo");
        readLine = "";
        if(file2.is_open()){
            while(getline(file2, readLine)){
                readLine.erase(std::remove_if(readLine.begin(), readLine.end(), [](unsigned char x) {return std::isspace(x);}), readLine.end());
                if((pos = readLine.find(delimiter)) != std::string::npos){
                    k = readLine.substr(0, pos);
                    readLine.erase(0, pos+delimiter.length());
                    v = readLine;
                }
                memory_usage_data[k] = v;
            }

            root["memory"] = memory_usage_data;
            file2.close();
        }else{
            cout<<"file open fail";
            // return 1;
        }
        //meminfo
        root["stat_result"] = cpu_usage_data;
        //root["memory"] = memory_usage_data;

        Json::StyledWriter writer;
        str = writer.write(root);

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U(str));
        request.reply(response);
    }
    else
        {
            TRACE(L"\n/fanERRRRRRRRRRRRR\n");

            request.reply(status_codes::NotFound, U("Err : please check the url\n"));
        }
        
    
}

void handlePost(http_request request)
{   
    // clock_t start, end, result;
    // start = clock();
    // auto path = uri::split_path(uri::decode(request.relative_uri().path()));
    string pp = uri::decode(request.relative_uri().path());
    auto path = uri::split_path(pp);
    
    if (!path.empty() && path[0] == "fan" && path[1] == "do")
    {
        // TODO: fan 시작 구현
        TRACE(L"\n/fan/do\n");
        // fan_status = 1;

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U("start Fan\n"));
        request.reply(response);
        TRACE(L"\n/fan/do\n");
        // request.reply(status_codes::OK, fan_status);
    }
    else if (!path.empty() && path[0] == "fan" && path[1] == "end")
    {
        // TODO: fan 정지 구현
        TRACE(L"\n[POST] :: /fan/end\n");
        // fan_status = 0;

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U("Stop Fan\n"));
        request.reply(response);
        // request.reply(status_codes::OK, fan_status);
    }
    else
    {
        request.reply(status_codes::NotFound, U("Err : please check the url\n"));
    }
    
    // end = clock();
    // result = (double) ((end - start));
    // request_cnt("POST", result, pp, to_string(status_codes::OK));
}

void handleDelete(http_request request)
{

    string pp = uri::decode(request.relative_uri().path());
    auto path = uri::split_path(pp);

    // request.reply(status_codes::NotImplemented, U("Err : NotImplemented\n"));
    if (!path.empty() && path[0] == "fan" && path[1] == "end")
    {
        // TODO: fan 정지 구현
        TRACE(L"\n[DELETE] :: /fan/end\n");
        // fan_status = 0;

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U("Stop Fan\n"));
        request.reply(response);
    }
}
void handlePut(http_request request)
{
    request.reply(status_codes::NotImplemented, U("Err : NotImplemented\n"));
}
void handlePatch(http_request request)
{
    request.reply(status_codes::NotImplemented, U("Err : NotImplemented\n"));
}

int main()
{
    http_listener listener(U(SERVER_ADDR)); // Server URL, Port 지정.

    listener.support(methods::GET, handleGet);
    listener.support(methods::POST, handlePost);
    listener.support(methods::DEL, handleDelete);
    listener.support(methods::PUT, handlePut);
    listener.support(methods::PATCH, handlePatch);

    try
    {
        listener
            .open()
            .then([&listener]()
                  { TRACE(L"\nstarting to listen\n"); })
            .wait();

        while (true)
            ;
    }
    catch (exception const &e)
    {
        wcout << e.what() << endl;
    }
    listener.close();
    return 0;
}
