#include "can_service.h"

struct can_message_event
{   
    uint16_t sop_mark;     // 0xEB90
    uint8_t device_id;
    uint8_t event_type;     // Por ahora un ùnico tipo de evento, se pueden agregar otros: status, error, etc.    
    uint32_t canid;
    uint8_t len;
    uint8_t data[8];
    uint16_t crc;          // A ser usado más adelante.    
    uint16_t eop_mark;          // A ser usado más adelante.    
};


can_service::can_service()
    :
    led1(LED1),
    led2(LED2),
    sync_counter(0),
    serial_port(USBTX, USBRX),
    can{
        {MBED_CONF_APP_CAN1_RD, MBED_CONF_APP_CAN1_TD},
        {MBED_CONF_APP_CAN2_RD, MBED_CONF_APP_CAN2_TD} 
    },
    can_event_thread(osPriorityHigh)
{

}

void can_service::setup()
{
    // 9600-8-N-1
    serial_port.set_baud(115200);
    serial_port.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );

    /* Registrar la función de lectura de CAN en la interrupción Rx. 
       Notar que la función se ejecuta en una cola, y no directamente en la interrupción. */
    can[0].attach(can_dev_queue.event(this, &can_service::can_read_message, 0), CAN::IrqType::RxIrq);
    can[1].attach(can_dev_queue.event(this, &can_service::can_read_message, 1), CAN::IrqType::RxIrq);

    serial_port.sigio(can_dev_queue.event(this,&can_service::serial_read_command));
    
    can_event_thread.start(callback(&can_dev_queue, &EventQueue::dispatch_forever));

    /* Programación de mensajes SYNC. */
    
    can_sync_ticker.attach(can_dev_queue.event(this, &can_service::can1_send_sync_message), 50ms);    
}




/* Enviar mensaje SYNC desde CAN 1. */
void can_service::can1_send_sync_message() 
{    
    const uint32_t canid = 1337;
    char data[2];
    data[0] = 0;
    data[1] = sync_counter;
    can[0].write(CANMessage(canid, data, 2));

    // Incrementar contador con rollover en 19.
    sync_counter++;
    sync_counter%=19;
    
    // Led indicador
    led1 = !led1;
}

/* Leer mensaje de CAN e imprimir en pantalla */
void can_service::can_read_message(int device_id)
{
    CANMessage msg;
    if (can[device_id].read(msg)) 
    {             
        led2 = !led2;    

        get_payload_buffer()[0] = 0x0;  // Event type
        get_payload_buffer()[1] = device_id; // Device Id
        get_payload_buffer()[2] = msg.id >> 24; //
        get_payload_buffer()[3] = msg.id >> 16; 
        get_payload_buffer()[4] = msg.id >> 8;
        get_payload_buffer()[5] = msg.id >> 0;
        get_payload_buffer()[6] = msg.len;
        for(size_t i=0;i<msg.len;i++)
        {
            get_payload_buffer()[7+i] = msg.data[i];
        }
        send( 7 + msg.len );

        // Debug
        /*
        printf("\nCAN%d RX - CANID: 0x%08x, LEN: %d, Data: ", device_id, msg.id, msg.len);
        for(int i=0;i<msg.len;i++)
        {
            printf("%02x ", msg.data[i]);
        } */            
    }
}

void can_service::serial_read_command()
{
    /*
    if (serial_port.readable())
    {
        can_message_event ev;
        
        ev.canid=0x1234;
        ev.len=2;
        ev.device_id=0;
        ev.data[0] = 0xA1;
        ev.data[1] = 0xA2;
        
        can[ev.device_id].write(CANMessage(ev.canid, ev.data, ev.len));
        led1 = !led1;
        
        if (sizeof(can_message_event) == serial_port.read(&ev,sizeof(can_message_event)))
        {
            can[ev.device_id].write(CANMessage(ev.canid, ev.data, ev.len));
            led1 = !led1;
        }
    }
    */
}


void can_service::send_impl(const uint8_t* buf, uint8_t n)
{
    serial_port.write(buf,n);
}