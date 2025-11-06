##   Function to simplify adding examples
function(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Threading)

    set_target_properties(${name} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endfunction()

##   Compile examples
add_example(01_this_thread)