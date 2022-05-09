
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
        can_message_event ev;
        std::cout << bytes_transferred << ", " << sizeof(can_message_event) << std::endl;

        //std::memcpy(&ev,  boost::asio::buffer_cast<const char*>(buffer.data()), bytes_transferred);        
        //std::copy_n(boost::asio::buffers_begin(buffer.data()), bytes_transferred, back_inserter(rx_buffer));

        //std::string result = response_get(bytes_transferred);        
        //std::cout << result;			
        //buffer.consume(bytes_transferred);
        read_some();
    }
}

void stm32canbus_serialif::read_some()
{
    port.async_read_some(boost::asio::buffer(rx_buffer,BUFSIZE), boost::bind( &stm32canbus_serialif::read_handler, this,
                                             boost::asio::placeholders::error, 
                                            boost::asio::placeholders::bytes_transferred));
}


void stm32canbus_serialif::run()
{
    read_some(); 
    io.run();
}

void stm32canbus_serialif::feed(char c)
{


    switch(curr_state)
    {
        case EXPECTING_SYNC
    }

}