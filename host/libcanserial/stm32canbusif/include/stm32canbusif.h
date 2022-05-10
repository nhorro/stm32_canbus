#pragma once

#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <array>

#include <unistd.h>

#include <boost/ref.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include "protocol.h"

struct can_message_event
{   
    uint16_t sync_mark;     // 0xEB90
    uint8_t device_id;
    uint8_t event_type;     // Por ahora un ùnico tipo de evento, se pueden agregar otros: status, error, etc.    
    uint32_t canid;
    uint8_t len;
    uint8_t data[8];
    uint16_t crc;          // A ser usado más adelante.
};


/**
 * @brief Class to control STM32 CAN interface through serial port
 */
class stm32canbus_serialif : private protocol::packet_decoder {
public:
    static constexpr size_t BUFSIZE = 64;
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
    //boost::asio::streambuf buffer;
    std::array<char,BUFSIZE> rx_buffer;
    std::thread comm_thread;
    std::atomic<bool> keep_running;
    
    void read_handler( const boost::system::error_code& error, size_t bytes_transferred);
    void read_some();
    std::string response_get(std::size_t length);
    void run();

    // Packet protocol
	void handle_packet(const uint8_t* payload, uint8_t n) override;
	void set_error(error_code ec) override;
	void handle_connection_lost() override;
};