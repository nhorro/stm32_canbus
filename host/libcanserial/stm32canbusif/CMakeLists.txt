cmake_minimum_required(VERSION 3.10)

find_package(Boost COMPONENTS system REQUIRED)

# set the project name
project(stm32canbus_serialif)

# add the executable
add_library(${PROJECT_NAME} 
    src/stm32canbusif.cpp 
)

target_link_libraries(${PROJECT_NAME} 
    ${Boost_SYSTEM_LIBRARY}
    pthread
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ./include
)