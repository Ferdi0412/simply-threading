/**
 * This provides an overview of all functions available in the 
 * `simply::this_thread` namespace
 */

#include <iostream>
#include <chrono>

#include <simply/threading.h>

int main() {
    std::cout << "main thread id: " << simply::this_thread::get_id() << std::endl << std::endl;

    std::cout << "If there are many other threads, I'll now yield" << std::endl << std::endl;
    simply::this_thread::yield();

    std::cout << "For Windows, some people prefer to 'yield' using 'sleep'" << std::endl
              << "This is because sleeping for 0 ms will only yield for"
              << "threads of same priority, or higher" << std::endl << std::endl;
    simply::this_thread::sleep(0);

    std::cout << "I can sleep for a whole second easily" << std::endl << std::endl;
    simply::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "I can also sleep to a specific time - 1 sec from now" << std::endl << std::endl;
    simply::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));

    std::cout << "I can set the name of this thread." << std::endl << std::endl;
    simply::this_thread::set_name("main_thread");

    std::cout << "This thread is now named: " 
              << simply::this_thread::get_name() << std::endl << std::endl;

    // This is missing for Windows less than 8, 
    // and for some Windows compilers without appropriate 
    // compilation flags
    // std::cout << "The main stack size is "
    //             << std::to_string(
    //                 simply::this_thread::get_stack_size()
    //             ) << std::endl;
}