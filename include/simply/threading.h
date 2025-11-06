/**
 * @file threading.h
 * @brief simply-threading: Drop-in replacements for standard threading objects with additional control, such as priority setting. 
 * 
 * @author Ferdinand Oliver M Tonby-Strandborg
 * @date 2025-11-05
 * @version 0.0.0-alpha
 * 
 * @copyright Copyright (c) 2025 Ferdinand T-S. Licensed under the MIT license.
 */
#ifndef SIMPLY_THREADING_H_
#define SIMPLY_THREADING_H_

/**
 * The following macros could be relevant:
 * 
 * - SIMPLY_WIN_7 - Compile only Windows 7-compliant things
 *                - Sometimes compilers on Windows may miss parts of the SDK after Windows 7
 *                - Notably, you may lose the ability to use get_stack_size, and this makes that explicit
 * - _WIN32_WINNT=0x0602 - An alternative is to manually set this, which may sometimes work
 */

#define SIMPLY_WINDOWS defined(_WIN32)
#define SIMPLY_LINUX defined(__linux__)

#ifdef _MSVC
    #define SIMPLY_stdVERSION _MSVC
#else
    #define SIMPLY_stdVERSION __cplusplus
#endif

#if SIMPLY_stdVERSION < 201703L
    #error "simply/threading.h Requires minimum C++ 17!"
#endif
#define SIMPLY_std20plus SIMPLY_stdVERSION >= 202002L
#define SIMPLY_std23plus SIMPLY_stdVERSION >= 202302L

#include <chrono>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>
#include <system_error>

#if SIMPLY_WINDOWS
    // #if !defined(SIMPLY_WIN_7) && _WIN32_WINNT < 0x0602
    //     #error "simply-threading: Windows API < 8 - compile with either -DSIMPLY_WIN_7 or -D_WIN32_WINNT=0x0602 - see threading.h notices"
    // #endif
    #include <windows.h>

#elif SIMPLY_LINUX
    #include <pthread.h>
    #include <unistd.h>

#endif

namespace simply {
    ///   ms_type
    /// @brief Milliseconds for use in timed functions
    using ms_type = uint32_t;

    // =================================================================
    // >> Thread
    // =================================================================
    class Thread {
    public:
        #if SIMPLY_WINDOWS
            using native_handle_type = HANDLE;

        #elif SIMPLY_LINUX
            using native_handle_type = pthread_t;

        #endif

    public:
        /* === Public Class Declarations === ======================== */
        class id;
        
        #if SIMPLY_WINDOWS
            ///   Priority
            /// The available priorities used by Windows
            enum Priority {
                IDLE          = THREAD_PRIORITY_IDLE,
                LOWEST        = THREAD_PRIORITY_LOWEST,
                LOW           = THREAD_PRIORITY_BELOW_NORMAL,
                NORMAL        = THREAD_PRIORITY_NORMAL,
                HIGH          = THREAD_PRIORITY_ABOVE_NORMAL,
                HIGHEST       = THREAD_PRIORITY_HIGHEST,
                TIME_CRITICAL = THREAD_PRIORITY_TIME_CRITICAL
            };
        #endif

    public:
        /* === Observers === */
        ///   max_sleep
        /// @brief Maximum milliseconds one sleep call supports
        static ms_type max_sleep() noexcept;


        /* === Classes Implementations === ========================== */
        ///   id
        /// @brief Used to uniquely identify a thread instance
        class id {
        public:
            #if SIMPLY_WINDOWS
                using native_id_type = DWORD;

            #elif SIMPLY_LINUX
                using native_id_type = pthread_t;

            #endif

        public:
            /* === Constructors === ================================= */
            ///   id
            /// @brief This does not represent a thread of execution 
            id() noexcept;

            ///   id {internal} 
            id(native_handle_type handle) noexcept;

        public:
            /* === Comparisons === ================================== */
            friend bool operator==(id lhs, id rhs) noexcept;

            #if SIMPLY_std20plus
                friend std::strong_ordering operator<=>(id lhs, id rhs) noexcept;
            
            #else
                friend bool operator!=(id x, id rhs) noexcept;
                friend bool operator<(id lhs, id rhs) noexcept;
                friend bool operator>(id lhs, id rhs) noexcept;
                friend bool operator<=(id lhs, id rhs) noexcept;
                friend bool operator>=(id lhs, id rhs) noexcept;
            #endif

        public:
            /* === Formatting & Hashing === ========================= */
            template <class CharT, class Traits>
            friend std::basic_ostream<CharT, Traits>&
            operator<<(std::basic_ostream<CharT, Traits>& lhs, id rhs);

            // template<class CharT> struct formatter<id, CharT>;

            // template<> struct hash<id>;

        private:
            native_id_type id_;
        };
    };

    // =================================================================
    // >> this_thread
    // =================================================================
    ///   this_thread
    /// @brief A collection of functions for controlling the current (calling) thread
    namespace this_thread {
        ///   get_id
        /// @brief Get an identifier for this thread
        Thread::id get_id() noexcept;

        ///   yield
        /// @brief Allow OS to yield to another thread of execution
        void yield() noexcept;

        ///   sleep
        /// @brief Sleep for a specified number of milliseconds
        /// @throws
        ///  - system_error(invalid_argument) if duration is negative
        ///  - system_error(invalid_argument) if duration is too large - see Thread::max_sleep
        void sleep(ms_type ms_sleep);

        ///   sleep_until
        /// @brief Sleep until a specified clock timepoint
        /// @throws 
        ///  - system_error(invalid_argument) if duration is negative
        ///  - system_error(invalid_argument) if duration is too large - see Thread::max_sleep
        template <class Clock, class Duration>
        void sleep_until(const std::chrono::time_point<Clock, Duration>& abs_time);

        ///   sleep_for
        /// @brief Sleep for a specified clock duration
        /// @throws
        ///  - system_error(invalid_argument) if duration is negative
        ///  - system_error(invalid_argument) if duration is too large - see Thread::max_sleep
        template <class Rep, class Period>
        void sleep_for(const std::chrono::duration<Rep, Period>& rel_time);

        ///   set_name
        /// @brief Set the human-readable name for this thread
        /// @throws
        ///  - system_error(invalid_argument) if too long (>15 char for Linux)
        ///  - system_error if system API calls failed
        void set_name(const std::string& name);

        ///   get_name
        /// @brief Get the human-radable name for this thread
        /// @throws
        ///  - system_error if system API calls failed
        std::string get_name();

        #if SIMPLY_WINDOWS
            ///   set_wide_name
            /// @brief Set the human-readable name using wide chars - Windows native approach
            /// @throws
            ///  - system_error if system API calls failed
            void set_wide_name(const std::wstring& wname);

            ///   get_wide_name
            /// @brief Get the human-readble name in wide chars - Windows native approach
            /// @throws
            ///  - system_error if system API calls failed
            std::wstring get_wide_name();

            ///   from_wide {internal}
            /// @brief Convert from wstring to string
            std::string from_wide(const std::wstring& wname) noexcept;

            ///   to_wide {internal}
            /// @brief Convert from string to wstring
            std::wstring to_wide(const std::string& name) noexcept;

        #endif

        #if SIMPLY_WINDOWS    
            ///   get_priority
            /// @brief Get the priority of the current thread
            Thread::Priority get_priority();
        #endif

        // "Suppress" until a good workaround found through cmake
        /// @todo - Fix the get_stack_size C++ implementation
        // #if !(defined(SIMPLY_WIN_7) && SIMPLY_WINDOWS)
        //     ///   get_stack_size
        //     /// @brief Get the number of bytes reserved for/allocated to the current thread
        //     /// @note {Windows} This is only available for Windows SDK 8+, and may need compilation flags depending on compiler
        //     size_t get_stack_size();
        // #endif

        // To add:
        // xx CPU affinity xx can't GET CPU affinity... Can only set...
    }
}

// =====================================================================
// >> Implementations
// =====================================================================
namespace simply {
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++ Thread::id
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 0 should never be expected for a thread on most modern OSs
    Thread::id::id() noexcept: id_(0) {}

    // Failure should not be expected on most modern OSs
    #if SIMPLY_WINDOWS
        Thread::id::id(Thread::native_handle_type handle) noexcept: id_(GetThreadId(handle)) {}
    
    #elif SIMPLY_LINUX
        Thread::id::id(Thread::native_handle_type handle) noexcept: id_(handle) {}
    #endif

    bool operator==(Thread::id lhs, Thread::id rhs) noexcept { return lhs.id_ == rhs.id_; }

    #if SIMPLY_std20plus
        bool operator<=>(Thread::id lhs, Thread::id rhs) noexcept { return lhs.id_ <=> rhs.id_; }

    #else
        bool operator!=(Thread::id lhs, Thread::id rhs) noexcept { return lhs.id_ != rhs.id_; } 
        bool operator>(Thread::id lhs, Thread::id rhs) noexcept { return lhs.id_ > rhs.id_; } 
        bool operator<(Thread::id lhs, Thread::id rhs) noexcept { return lhs.id_ < rhs.id_; }
        bool operator<=(Thread::id lhs, Thread::id rhs) noexcept { return lhs.id_ <= rhs.id_; } 
        bool operator>=(Thread::id lhs, Thread::id rhs) noexcept { return lhs.id_ >= rhs.id_; }
    #endif

    template <class CharT, class Traits>
    inline std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& lhs, Thread::id rhs) { lhs << std::to_string(rhs.id_); return lhs; }

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++ Thread
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    #if SIMPLY_WINDOWS
        inline ms_type Thread::max_sleep() noexcept {
            return std::numeric_limits<DWORD>::max() - 1;
        }

    #elif SIMPLY_LINUX
        inline ms_type Thread::max_sleep() noexcept {
            return std::numeric_limits<uint32_t>::max();
        }
    #endif

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++ this_thread
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    #if SIMPLY_WINDOWS
        Thread::id this_thread::get_id() noexcept {
            return Thread::id(GetCurrentThread());
        }

        void this_thread::yield() noexcept {
            SwitchToThread();
        }

        void this_thread::sleep(ms_type ms_sleep) {
            if ( ms_sleep > std::numeric_limits<DWORD>::max() )
                throw std::system_error(
                    std::make_error_code(std::errc::invalid_argument),
                    "this_thread::sleep: Windows implementation does not permit 0xFFFFFFFF!"
                );
            Sleep(ms_sleep);
        } 

    #elif SIMPLY_LINUX
        Thread::id this_thread::get_id() noexcept {
            return Thread::id(pthread_self());
        }

        void this_thread::yield() noexcept {
            pthread_yield();
        }

        void this_thread::sleep(ms_type ms_sleep) {
            if ( ms_sleep > Thread::max_sleep() )
                throw std::system_error (
                    std::make_error_code(std::errc::invalid_argument),
                    "this_thread::sleep: Too long a period to sleep for!"
                );
            
            struct timespec duration;
            duration.tv_nsec = (ms_sleep % 1000000) * 1000;
            duration.tv_sec  = (ms_sleep / 1000);
            
            /// @todo - Make nanosleep more robust by checking err and return
            nanosleep(&duration, nullptr);
        }

    #endif

    template <class Clock, class Duration>
    void this_thread::sleep_until(const std::chrono::time_point<Clock, Duration>& abs_time) {
        this_thread::sleep_for(abs_time - Clock::now());
    }

    template <class Rep, class Period>
    void this_thread::sleep_for(const std::chrono::duration<Rep, Period>& rel_time) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(rel_time).count();
        /// @todo - Change this to just repeat calls to sleep() until the time point is reached...
        if ( ms < 0 || ms > std::numeric_limits<ms_type>::max() )
            throw std::system_error(
                std::make_error_code(std::errc::invalid_argument),
                "this_thread::sleep_for: Value was negative or too large!"
            );
        sleep(static_cast<ms_type>(ms));
    }

    #if SIMPLY_WINDOWS
        std::string this_thread::get_name() {
            std::wstring wname = this_thread::get_wide_name();
            return from_wide(this_thread::get_wide_name());
        }

        void this_thread::set_name(const std::string& name) {
            this_thread::set_wide_name(this_thread::to_wide(name));
        }

        std::wstring this_thread::get_wide_name() {
            PWSTR description = nullptr;
            GetThreadDescription(GetCurrentThread(), &description); // Check error...
            std::wstring wname(description);
            LocalFree(description); // Check error...
            return wname;
        }

        void this_thread::set_wide_name(const std::wstring& wname) {
            SetThreadDescription(GetCurrentThread(), wname.c_str());
        }

        inline std::string this_thread::from_wide(const std::wstring& wname) noexcept {
            size_t len = std::wcstombs(nullptr, wname.c_str(), 0) + 1;
            char* buffer = new char[len];
            std::wcstombs(buffer, wname.c_str(), len);
            std::string name(buffer);
            delete[] buffer;
            return name;
        }

        inline std::wstring this_thread::to_wide(const std::string& name) noexcept {
            size_t len = std::mbstowcs(nullptr, name.c_str(), 0) + 1;
            wchar_t* buffer = new wchar_t[len];
            std::mbstowcs(buffer, name.c_str(), len);
            std::wstring wname(buffer);
            delete[] buffer;
            return wname;
        }
    #elif SIMPLY_LINUX
        inline std::string this_thread::get_name() {
            char name[16];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            return name;
        }

        inline void this_thread::set_name(const std::string& name) {
            if ( name.size() > 15 )
                throw std::system_error(
                    std::make_error_code(std::errc::invalid_argument),
                    "this_thread::set_name: Linux only supports 15 chars followed by NULL for name"
                );
            pthread_setname_np(pthread_self(), name.c_str());
        }
    #endif

    #if SIMPLY_WINDOWS
        inline Thread::Priority this_thread::get_priority() {
            int p = GetThreadPriority(GetCurrentThread());
            switch ( p ) {
                case THREAD_PRIORITY_ERROR_RETURN:
                    throw std::system_error(
                        GetLastError(),
                        std::system_category()
                    );
                case THREAD_PRIORITY_IDLE:
                    return Thread::Priority::IDLE;
                case THREAD_PRIORITY_LOWEST:
                    return Thread::Priority::LOWEST;
                case THREAD_PRIORITY_BELOW_NORMAL:
                    return Thread::Priority::LOW;
                case THREAD_PRIORITY_NORMAL:
                    return Thread::Priority::NORMAL;
                case THREAD_PRIORITY_ABOVE_NORMAL:
                    return Thread::Priority::HIGH;
                case THREAD_PRIORITY_HIGHEST:
                    return Thread::Priority::HIGHEST;
                case THREAD_PRIORITY_TIME_CRITICAL:
                    return Thread::Priority::TIME_CRITICAL;
                default:
                    if ( p < 0 )
                        return Thread::Priority::IDLE;
                    else
                        return Thread::Priority::TIME_CRITICAL;
            }
        }

        // Suppressed for reason at declaration of this...
            // size_t this_thread::get_stack_size() {
            //     ULONG_PTR low;
            //     ULONG_PTR high;
            //     GetCurrentThreadStackLimits(&low, &high);
            //     return static_cast<size_t>(high - low);
            // }
    #elif SIMPLY_LINUX 


    #endif
}

#endif // SIMPLY_THREADING_H_