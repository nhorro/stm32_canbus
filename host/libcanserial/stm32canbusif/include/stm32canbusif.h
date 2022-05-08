#pragma once

#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>

#include <unistd.h>

#include <boost/ref.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

// To be used later
struct can_message_event
{
    enum event_type {
        rx_message
    };
    uint8_t device_id;
    uint8_t data[8];
};


/**
 * @brief Class to control STM32 CAN interface through serial port
 */
class stm32canbus_serialif {
public:
    /**
     * @brief Construct a new stm32canbus serialif object.
     * 
     * @param dev_name Device name. In Linux: /dev/ttyUSB0, /dev/ttyACM0, etc.
     * @param baudrate Baudrate (default is 9600).
     */
    stm32canbus_serialif(const char *dev_name, int baudrate);

    /**
     * @brief Start. When started, all the received events from STM32 invoke the callbacks.
     */
    void start();

    /**
     * @brief Stop.
     */
    void stop();
    
private:
    boost::asio::io_service io;
    boost::asio::serial_port port;
    boost::asio::streambuf buffer;
    std::thread comm_thread;
    std::atomic<bool> keep_running;
    
    void read_handler( const boost::system::error_code& error, size_t bytes_transferred);
    void read_until_terminator();
    std::string response_get(std::size_t length);
    void run();
};