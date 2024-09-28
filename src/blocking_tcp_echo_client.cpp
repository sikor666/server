#include "parallel/Client.h"
#include "protocol/Command.h"

#include "ExecutionFactory.h"

#include <iostream>

const char data[]{"\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
0123456789012345678901234567890123456789\n\
"};

using namespace network::protocol;

int main(int argc, char * argv[])
{
    try
    {
        std::cout << "Header: " << sizeof(Header) << "\n";
        std::cout << "End: " << sizeof(Footer) << "\n";

        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        std::atomic_size_t number{0};

        const auto start = std::chrono::steady_clock::now();
        {
            const auto m_executionPool = core::CreateExecutionPool();
            const auto m_executionQueue =
                core::CreateConcurrentExecutionQueue<void, std::shared_ptr<network::parallel::Client>>(m_executionPool,
                    // Execution function is called in parallel on the next free thread with the next object from the queue
                    [&number, host{std::string{argv[1]}}, service{std::string{argv[2]}}](
                        const std::atomic_bool & isCanceled, std::shared_ptr<network::parallel::Client> && object) {
                        object->send(host, service, command::create("abc")->frame());
                        object->send(host, service, command::create(1234)->frame());
                        object->send(host, service, command::error()->frame());
                        std::cout << "[" << std::this_thread::get_id() << "][" << ++number << "] done\n";
                    });

            for (size_t i = 0; i < 2; i++)
            {
                std::cout << "[" << std::this_thread::get_id() << "][" << number << "] push\n";
                m_executionQueue->push(std::make_shared<network::parallel::Client>(io_context));
            }
        }
        const auto stop = std::chrono::steady_clock::now();
        const auto time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

        std::cout << "[number: " << number << "][time: " << static_cast<double>(time) / 1000000.0 << " s]\n";
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
