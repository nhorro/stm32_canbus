#include "can_service.h"

can_service::can_service()
    :
    opcodes
		{ 
			
			{ &can_service::cmd_send_message, opcode_flags::default_flags }
		},
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
    }
}

void can_service::serial_read_command()
{    
    if (serial_port.readable())
    {
        static char buf[32] = {0};
        ssize_t n = this->serial_port.read(buf, sizeof(buf));
        if (EAGAIN != n)
        {
            char* pbuf = buf;
            do {
                feed(*pbuf++);
            } while(--n);
        }
    }
    
}


void can_service::handle_packet(const uint8_t* payload, uint8_t n)
{
    // El byte 0 es el código de opcode, el resto el payload. */
	uint8_t opcode = payload[0];

    (opcode < OPCODE_LAST) ? (this->*(opcodes[opcode].fn))(payload + 1, n - 1) : error_code::unknown_opcode;
}

void can_service::set_error(error_code ec)
{
	if (packet_decoder::error_code::timeout != ec)
	{
		//TODO
	}
}


void can_service::handle_connection_lost()
{
    // TODO    
}

void can_service::send_impl(const uint8_t* buf, uint8_t n)
{
    serial_port.write(buf,n);
}



can_service::error_code can_service::cmd_send_message(const uint8_t* payload, uint8_t n)
{
    // TODO: agregar validaciòn...
    uint32_t device_id = payload[0];
    const uint32_t canid = (payload[1] << 24) | (payload[2] << 16) | (payload[3] << 8) | (payload[4] << 0);
    size_t msg_len = payload[5];        
    can[device_id].write(CANMessage(canid, &payload[6], msg_len));

    return error_code::success;
}