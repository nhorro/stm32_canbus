#pragma once

#include "BufferedSerial.h"
#include "SerialBase.h"

#if !DEVICE_CAN
#error [NOT_SUPPORTED] CAN not supported for this target
#endif

#include "mbed.h"

#include "config.h"

#include "protocol.h"


class can_service : 
    public protocol::packet_encoder
    //private protocol::packet_decoder

{
public:
    can_service();
    ~can_service() {}
    void setup();
private:   
    void serial_read_command();
    void can_read_message(int device_id);
    void can1_send_sync_message();

    /* Led indicador de transmisión en canal 1 */
    DigitalOut led1;

    /* Led indicador de recepción en canal 2 */
    DigitalOut led2;

    /* Contador de mensajes de SYNC enviados por canal 1. */
    char sync_counter;

    /* Cola de eventos de acceso a dispositivo CAN (prioridad alta) */
    EventQueue can_dev_queue;

    /* Hilo de alta prioridad para manejo de dispositivos. */
    Thread can_event_thread;
    
    /* Ticker para mensajes SYNC */
    Ticker can_sync_ticker;

    /* Dispositivos CAN */
    CAN can[2];
    
    /* Interfaz serie con PC */
    BufferedSerial serial_port;

    /* Requerido por encoder */
    void send_impl(const uint8_t* buf, uint8_t n);
};