# Simply Threading
[![Platform](https://img.shields.io/badge/platform-Windows-blue)](https://docs.microsoft.com/en-us/windows/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Simply Threading** provides a drop-in replacement for `std::jthread`, 
with additional control over the threads, such as changing priority.

> **Status:** Alpha - API not inherited from `std::jthread` is subject to change.

**Requirements:**
- C++ 17
- C++ 20 adds `std::stop_token` support
- Windows
    - Windows 7 or later
    - Windows 8 adds **stack size** features
- cmake
    - cmake 3.14 provides **FetchContent_MakeAvailable**



## Why Simply Threading?
The `simply::Thread` class retains the full `std::jthread` interface, 
with additional methods for setting priority, name, stack-size, and CPU 
affinity.

> **Priority:** The implementation of thread priority, and execution 
scheduling is highly system-dependent. As such, no attempt was made
to make the priority setting cross-platform, rather system-specific
methods are provided.

For C++ 20 and later, it is a drop-in replacement for `std::jthread`, with
additional features:

```c++
// Using lambda functions
std::jthread t1([](){ std::cout << "Hello world from jthread!" << std::endl; });
simply::Thread t2([](){ std::cout << "Hello world from Thread!" << std::endl; });

// Set name
t2.set_name("my_thread");

// Loop that only stops when requested
void long_loop(std::stop_token token) {
    while ( !token.stop_requested() )
        /* do something... */;
}

// long_loop runs untile t3 calls request_stop, or join
simply::Thread t3(long_loop);
t3.request_stop();
```

For C++ 17, it is equivalent to `std::thread`, but the destructor calls
*join* if *joinable* instead of terminating the program:

```c++
int main() {
    std::thread t1([](){ std::cout << "Hello world from thread!" << std::endl; });
    simply::Thread t2([](){ std::cout << "Hello world from Thread!" << std::endl; });

    /* t2 will block until thread joins */
    /* t1 will terminate program per std::thread specifications */
}
```



## Quick Start
### Installation
I recommend using **cmake** to install this project. This is because 
certain features may require appropriate flags/links, depending on
you OS and compiler. As such, the simplest method for *full functionality*
is to use **FetchContent** in **cmake**:

```txt
# CMakeLists.txt
cmake_minimum_required(VERSION 3.12)
project(YourProject)

# Declare your build targets
add_executable(main main.cpp)

# Include FetchContent and SimplyThreading
include(FetchContent)

FetchContent_Declare(
    SimplyThreading
    GIT_REPOSITORY https://github.com/Ferdi0412/simply-threading.git
    GIT_TAG main
)

FetchContent_MakeAvailable(SimplyConcurrency)

# Include SimplyThreading in your target
target_link_libraries(main PRIVATE 
    Simply::Threading
)
```

If you are not using **cmake**, you can also simply download
**threading.h**, as this is a single-header library.

```txt
<!-- In this repository -->
/simply-threading
. include/simply/concurrency.h <!-- Download this file -->

<!-- In your directory -->
/your-project
. main.cpp
. concurrency.h                <!-- Place in your project -->
```

```c++
// In main.cpp
#include "concurrency.h"
```



### class `simply::Thread`
Using this class is mostly identical to `std::jthread` or `std::thread`.
For basic usage examples, refer to the documentation for these.

#### Priority
Setting the priority is highly OS-dependent.

**Windows:**

> This is the only OS where raising the priority does not require special permission

For Windows, there are 7 pre-set priority *levels*, each of which are
available through an enum `simply::Thread::Priority`. For example:

```c++
simply::Thread background_thread([](){
    /* For example, monitor system resources */
});
// Set the background thread with the lowest priority
background_thread.set_priority(simply::Thread::Priority::IDLE);
```

**Linux:**

> Raising priority above main/first thread's priority requires root access.

For Linux, there are 2 "types" of priority... **[ADD EXPLANATION...]**



## Development Notes
**Initial Release Roadmap:** (Linux & Windows)
- [x] this_thread namespace
    - [ ] Fix `get_stack_size`
    - [ ] Implement `Priority` for Linux
- [x] Thread class
    - [ ] Fix Windows implementation
    - [ ] Implement for Linux
- [x] tests & examples
    - [ ] Ensure completeness of `simply::Thread` tests
    - [ ] Add some logging/debugging, perhaps also some benchmarking
- RELEASE 0.0.1
- [ ] FutureThread

**Style guide**
- Where "recreating" components of the standard library (such as `class std::thread::id`), use the same conventions
- 4 spaces for a tab
- `PascalCase` for classes, structs and enums
- `snake_case` for functions, arguments, public members, and variables
- `ends_in_type` for any type aliases
- `trailing_underscore_` for private member variables
- Opening curly brace should follow immediately after function variables, and close on a new line, example:
```c++
void some_function(void* some_arg) {
    /* Write here... */
}
```