#include "server_CP.hpp"
#include "handler.hpp"
#include "process_reading.hpp"

#define TRACE(msg) wcout << msg

int fan_status = 0;

// cpu 사용량

stJiffies prevJiffies;
stJiffies curJiffies;
stJiffies diffJiffies;
int totalJiffies;
int flag = 0;
int cpu_calc_flag = 0;

void cpu_calc(void)
{
    FILE *pStat = NULL;
    char cpuID[6] = {0};

    while (cpu_calc_flag)
    {
        pStat = fopen("/proc/stat", "r");
        fscanf(pStat, "%s %d %d %d %d", cpuID, &curJiffies.user,
               &curJiffies.nice, &curJiffies.system, &curJiffies.idle);

        diffJiffies.user = curJiffies.user - prevJiffies.user;
        diffJiffies.nice = curJiffies.nice - prevJiffies.nice;
        diffJiffies.system = curJiffies.system - prevJiffies.system;
        diffJiffies.idle = curJiffies.idle - prevJiffies.idle;

        totalJiffies = diffJiffies.user + diffJiffies.nice + diffJiffies.system + diffJiffies.idle;

        printf("Cpu usage : %.2f%%\n", 100.0f * (1.0 - (diffJiffies.idle / (double)totalJiffies)));

        prevJiffies = curJiffies;
        fclose(pStat);
        sleep(1);
    }
}

P_handler::P_handler(utility::string_t url) : listener(url)
{
    TRACE(L"\nstarting to listen ");

    listener.support(methods::GET, [this](http_request request)
                     { handleGet(request); });

    listener.support(methods::POST, [this](http_request request)
                     { handlePost(request); });

    listener.support(methods::DEL, [this](http_request request)
                     { handleDelete(request); });

    listener.support(methods::PUT, [this](http_request request)
                     { handlePut(request); });

    listener.support(methods::PATCH, [this](http_request request)
                     { handlePatch(request); });
}

void P_handler::handleGet(http_request request)
{
    string pp = uri::decode(request.relative_uri().path());
    auto path = uri::split_path(pp);
    if (!path.empty())
        wcout << path[0].c_str();
    if (!path.empty() && path[0] == "fan")
    {
        // TODO: fan정보 리턴 구현
        TRACE(L"\n/fan\n");
        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U(fan_status));
        sleep(1);
        // request.reply(status_codes::OK, );
        request.reply(response);
    }
    else if (!path.empty() && path[0] == "request" && path[1] == "report")
    {
        std::ifstream json_file;
        json_file.open("./output.json", std::ifstream::in | std::ifstream::binary);
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
        Json::Value root, jsonCPU, jsonCore, jsonEtc;
        string str;
        int64_t index = 0;

        ifstream file("/proc/cpuinfo");
        if (file.is_open())
        {
            while (getline(file, readline))
            {

                readline.erase(std::remove_if(readline.begin(), readline.end(), [](unsigned char x)
                                              { return std::isspace(x); }),
                               readline.end());

                if ((pos = readline.find(delimiter)) != std::string::npos)
                {
                    k = readline.substr(0, pos);
                    readline.erase(0, pos + delimiter.length());
                    v = readline;
                    cout << "Key: " << k << "\tValue: " << v << "\nkey is processor?  " << (k.compare("processor") ? "NO\t" : "YES\t") << "result: " << k.compare("processor") << endl;
                }

                if (k.compare("Hardware") == 0)
                {
                    index == 99;
                }
                else if (index == 99)
                {
                    jsonEtc[k] = v;
                    continue;
                }
                else if (k.compare("processor") == 0)
                {

                    if (index != stoi(v))
                    {
                        Json::StyledWriter writer;
                        str = writer.write(root);
                        cout << "jsonCore" << endl;
                        jsonCPU["processor " + v] = jsonCore;
                    }
                    cout << "processor: " << (index != stoi(v)) << "index: " << index << "v" << stoi(v) << endl;
                    index = stoi(v);
                }
                jsonCore[k] = v; // key : value --> ex) model: am7v~~D
            }
            file.close();
            root["CPU"] = jsonCPU;
            root["ETC"] = jsonEtc;
        }
        // cout << "==============CPUINFO===================" << endl;
        Json::StyledWriter writer;
        str = writer.write(root);

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U(str));
        request.reply(response);
    }
    else if (!path.empty() && path[0] == "stat_mem" && path[1] == "result")
    {
        string indexArry[10] = {"user", "nice", "system", "idle", "iowait", "irq", "softirq", "steal", "guest", "guest_nice"};
        string readLine, readLine2, k, v, delimiter = ":", str;
        size_t pos = 0;
        int64_t index = 0;

        Json::Value root, cpu_usage_data, memory_usage_data, semi;

        ifstream file("/proc/stat");

        if (file.is_open())
        {
            while (getline(file, readLine))
            {

                if (readLine.find("intr") == string::npos)
                {
                    vector<string> result = split(readLine, ' ');
                    readLine2 = readLine.erase(0, result[0].length());
                    vector<string> result2 = split(readLine, ' ');
                    if (result2.size() == 11)
                    {
                        for (int i = 0; i < 10; i++)
                        {
                            semi[indexArry[i]] = result2.at(i + 1);
                        }
                    }
                    else
                    {
                        // int j = 0;
                        for (int i = 0; i < 10; i++)
                        {
                            semi[indexArry[i]] = result2.at(i + 2);
                        }
                    }
                    cpu_usage_data[result[0]] = semi;
                }
                else
                {
                    break;
                }
            }

            file.close();
        }
        ifstream file2("/proc/meminfo");
        readLine = "";
        if (file2.is_open())
        {
            while (getline(file2, readLine))
            {
                readLine.erase(std::remove_if(readLine.begin(), readLine.end(), [](unsigned char x)
                                              { return std::isspace(x); }),
                               readLine.end());
                if ((pos = readLine.find(delimiter)) != std::string::npos)
                {
                    k = readLine.substr(0, pos);
                    readLine.erase(0, pos + delimiter.length());
                    v = readLine;
                }
                memory_usage_data[k] = v;
            }

            root["memory"] = memory_usage_data;
            file2.close();
        }
        else
        {
            cout << "file open fail";
            // return 1;
        }
        // meminfo
        root["stat_result"] = cpu_usage_data;
        // root["memory"] = memory_usage_data;

        Json::StyledWriter writer;
        str = writer.write(root);

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U(str));
        request.reply(response);
    }
    else if (!path.empty() && path[0] == "cpu" && path[1] == "usage")
    {
        TRACE(L"\n/CPU_USAGE %\n");
        if (flag == 0)
        {
            string indexArry[10] = {"user", "nice", "system", "idle", "iowait", "irq", "softirq", "steal", "guest", "guest_nice"};
            string readLine, readLine2, k, v, delimiter = ":", str;
            Json::Value root, semi;
            ifstream file("/proc/stat");

            if (file.is_open())
            {
                while (getline(file, readLine))
                {

                    if (readLine.find("intr") == string::npos)
                    {
                        vector<string> result = split(readLine, ' ');
                        readLine2 = readLine.erase(0, result[0].length());
                        vector<string> result2 = split(readLine, ' ');
                        if (result2.size() == 11)
                        {
                            for (int i = 0; i < 10; i++)
                            {
                                semi[indexArry[i]] = result2.at(i + 1);
                            }
                        }
                        else
                        {
                            // int j = 0;
                            for (int i = 0; i < 10; i++)
                            {
                                semi[indexArry[i]] = result2.at(i + 2);
                            }
                        }
                        root[result[0]] = semi;
                    }
                    else
                    {
                        break;
                    }
                }

                file.close();
                Json::StyledWriter writer;
                str = writer.write(root);
            }

            cpu_calc_flag = 1;
            thread t1(cpu_calc);

            t1.detach();
            flag = flag + 1;

            http_response response(status_codes::OK);
            response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
            response.set_body(U(str));
            request.reply(response);
        }
        else
        {
            Json::Value root;
            string str;
            root["usage"] = ("Cpu usage : %.2f%%\n", 100.0f * (1.0 - (diffJiffies.idle / (double)totalJiffies)));
            Json::StyledWriter writer;
            str = writer.write(root);

            http_response response(status_codes::OK);
            response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
            response.set_body(U(str));
            request.reply(response);
        }
    }
    else if(!path.empty() && path[0] == "process_info") //oslab 09.06
    {
        TRACE(L"\n/[GET] :: process_info\n");
        struct process_status proc_info;
        cur_process_status(&proc_info);

        Json::Value response_json, root;
        string str;

        for(int i=1; i<=proc_info.Name.size();i++){
            Json::Value processor;
            processor[U("Name")] = proc_info.Name[i-1];
            processor[U("State")] = proc_info.State[i-1];
            processor[U("Pid")] = proc_info.Pid[i-1];
            processor[U("cpu_usage")] = proc_info.cpu_usage[i-1];
            processor[U("mem_usage")] = proc_info.mem_usage[i-1];

            response_json["processor " + to_string(i)] = processor;
        }
        TRACE(L"\n/[GET] :: process_info\n");

        Json::StyledWriter writer;
        str = writer.write(response_json);
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

void P_handler::handlePost(http_request request)
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
    else if (!path.empty() && path[0] == "cpu" && path[1] == "usagestop")
    {
        // CPU 사용량 측정이 불필요할 때 호출
        TRACE(L"\n[POST] :: /cpu/usagestop\n");

        flag = 0;
        cpu_calc_flag = 0;

        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.set_body(U("Stop get cpu usage %\n"));
        request.reply(response);
    }
    else
    {
        request.reply(status_codes::NotFound, U("Err : please check the url\n"));
    }

    // end = clock();
    // result = (double) ((end - start));
    // request_cnt("POST", result, pp, to_string(status_codes::OK));
}

void P_handler::handleDelete(http_request request)
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
void P_handler::handlePut(http_request request)
{
    request.reply(status_codes::NotImplemented, U("Err : NotImplemented\n"));
}
void P_handler::handlePatch(http_request request)
{
    request.reply(status_codes::NotImplemented, U("Err : NotImplemented\n"));
}