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

/**
 * @brief Clase para controlar la interfaz CAN implementada en STM32 a través de puerto serie.
 */
class stm32canbus_serialif : 
    private protocol::packet_decoder, 
    private protocol::packet_encoder 
{
public:    
    /**
     * @brief Evento de recepción.
     */
    struct can_message_event
    {       
        uint32_t device_id;     /**< Identificador del dispositivo: 0,1,... */
        uint32_t event_type;    /**< Por ahora un ùnico tipo de evento: recepción, pero se pueden agregar otros: status, error, etc.  */
        uint32_t canid;         /**< CANID (32bits para soportar standard y extended). */
        uint32_t len;           /**< Cantidad de bytes del mensaje (máximo 8 bytes). */
        uint8_t data[8];        /**< Datos del mensaje. */
    };

    /**
     * @brief Callback para cuando llega un mensaje.
     */
    using on_can_message_callback = std::function<void(const can_message_event&)>;

    static constexpr size_t BUFSIZE = 64;
    

    /**
     * @brief Constructor por defecto.
     * 
     * @param dev_name Nombre del puerto serie (ej: /dev/ttyACM0).
     * @param baudrate Baudrate (recomendado 115200).
     * @param on_event_callback Callback definido por el usuario para responder cuando llega un mensaje.
     */
    stm32canbus_serialif(const char *dev_name, int baudrate, on_can_message_callback on_event_callback);

    /**
     * @brief Iniciar monitor/controlador. Cuando el controlador está arrancado, se habilita el monitoreo y capacidad de envío de mensajes.
     */
    void start();

    /**
     * @brief Detener monitor/controlador.
     */
    void stop();
    
private:
    boost::asio::io_service io;
    boost::asio::serial_port port;
    //boost::asio::streambuf buffer;
    std::array<char,BUFSIZE> rx_buffer;
    std::thread comm_thread;
    std::atomic<bool> keep_running;

    on_can_message_callback on_event_callback;
    
    void read_handler( const boost::system::error_code& error, size_t bytes_transferred);
    void read_some();
    std::string response_get(std::size_t length);
    void run();

    // Protocolo de paquetes (receptor)
	void handle_packet(const uint8_t* payload, size_t n) override;
	void set_error(error_code ec) override;
	void handle_connection_lost() override;

    // Protocolo de paquetes (emisor)
    void send_impl(const uint8_t* buf, uint8_t n) override;
};