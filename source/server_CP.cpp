#include "server_CP.hpp"
#include "handler.hpp"
#include "process_reading.hpp"
#include "signal_reading.hpp"

int main()
{
    // http_listener listener(U(SERVER_ADDR)); // Server URL, Port 지정.
    unique_ptr<P_handler> http_listener;
    http_listener = unique_ptr<P_handler>(new P_handler(U(SERVER_ADDR)));

    // ojh - 12 ~ 33 라인 추가 및 수정
    ignore_signals();

    pthread_t log_thread;

    if (pthread_create(&log_thread, NULL, init_thread, NULL) != 0) {
        wcout << "Error creating thread\n" << endl;
        // exit(1);
    }

    try
    {
        http_listener->open().wait();

        while (true)
            ;
    }
    catch (std::exception const &e)
    {
        wcout << e.what() << endl;
    }
    http_listener->close();

    return 0;
}
