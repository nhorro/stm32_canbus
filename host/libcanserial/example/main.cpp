#include <iostream>

#include "stm32canbusif.h"


int main(int argc, const char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "No port specified" << std::endl;
        return -1;
    }
    std::cout << "Port: " << argv[1] << std::endl;
    stm32canbus_serialif can(argv[1],9600);

    can.start();
        
    // Wait for key
    std::cin.ignore(); 

    can.stop();
      
    return 0;
}