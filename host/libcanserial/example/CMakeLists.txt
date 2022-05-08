cmake_minimum_required(VERSION 3.10)

find_package(Boost COMPONENTS system REQUIRED)

# set the project name
project(stm32canbus_serial_example)

# add the executable
add_executable(${PROJECT_NAME}
    main.cpp 
)

target_link_libraries(${PROJECT_NAME} 
    stm32canbus_serialif
    ${Boost_SYSTEM_LIBRARY}
    pthread
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ../stm32canbusif/include
)