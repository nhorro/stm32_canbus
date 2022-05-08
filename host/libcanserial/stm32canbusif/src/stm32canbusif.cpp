
#include "stm32canbusif.h"

stm32canbus_serialif::stm32canbus_serialif(const char *dev_name, int baudrate=9600) 
    : 
        io(), 
        port(io, dev_name)
{
    port.set_option( boost::asio::serial_port_base::parity() );	// default none
    port.set_option( boost::asio::serial_port_base::character_size( 8 ) );
    port.set_option( boost::asio::serial_port_base::stop_bits() );	// default one
    port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );          
}



void stm32canbus_serialif::start()
{     
    comm_thread = std::thread([this](){this->run();});
}


    
void stm32canbus_serialif::stop()
{
    port.cancel();
    comm_thread.join();        
}
   

void stm32canbus_serialif::read_handler( const boost::system::error_code& error, size_t bytes_transferred)
{
    if (error) 
    {
        std::cout << error.message() << std::endl;
    }
    else 
    {
        std::string result = response_get(bytes_transferred);        
        std::cout << result;			
        buffer.consume(bytes_transferred);
        read_until_terminator();
    }
}

void stm32canbus_serialif::read_until_terminator()
{
    boost::asio::async_read_until(port, buffer, "\n",
                                boost::bind( &stm32canbus_serialif::read_handler, this,
                                            boost::asio::placeholders::error, 
                                            boost::asio::placeholders::bytes_transferred)
                                );
}

std::string stm32canbus_serialif::response_get(std::size_t length) 
{
    std::string result;
    std::copy_n(boost::asio::buffers_begin(buffer.data()), length, back_inserter(result));
    return result;
}

void stm32canbus_serialif::run()
{
    read_until_terminator(); 
    io.run();
}