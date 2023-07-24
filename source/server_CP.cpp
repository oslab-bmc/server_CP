#include "server_CP.hpp"
#include "handler.hpp"

vector<string> split(string input, char delimiter) {
    vector<string> answer;
    stringstream ss(input);
    string temp; 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    } 
    return answer;
}

int main()
{
    // http_listener listener(U(SERVER_ADDR)); // Server URL, Port 지정.
    unique_ptr<P_handler> http_listener;
    http_listener = unique_ptr<P_handler>(new P_handler(U(SERVER_ADDR)));

    try
    {
        http_listener->open().wait();

        while (true)
            ;
    }
    catch (exception const &e)
    {
        wcout << e.what() << endl;
    }
    http_listener->close();
    return 0;
}
