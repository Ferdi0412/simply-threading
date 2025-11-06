#include <iostream>

#include <simply/threading.h>

int main() {
    simply::Thread t1([](){ 
        std::cout << "Hello from thread!" << std::endl;
        simply::this_thread::sleep(100);
    });
}