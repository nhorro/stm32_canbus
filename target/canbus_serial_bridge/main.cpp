/** @brief Test de conectividad CANBus v0.0.1
 *  @details
        - v0.0.1: Se envía desde CAN1 mensaje de SYNC con contador 0-19 cada 50ms. Se imprimen mensajes recibidos en CAN2.
*/

#include "BufferedSerial.h"
#include "SerialBase.h"

#if !DEVICE_CAN
#error [NOT_SUPPORTED] CAN not supported for this target
#endif

#include "mbed.h"

/* Configuración de interfaces CAN para dos canales en Nucleo-144 STM32F767ZI. 
   Puede requerir reasignaciòn para para otras versiones de placas.*/

/* Canal 1 */
#define MBED_CONF_APP_CAN1_RD PB_8
#define MBED_CONF_APP_CAN1_TD PB_9

/* Canal 2 */
#define MBED_CONF_APP_CAN2_RD PB_5
#define MBED_CONF_APP_CAN2_TD PB_6

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
 
/* Led indicador de transmisión en canal 1 */
DigitalOut led1(LED1);

/* Led indicador de recepción en canal 2 */
DigitalOut led2(LED2);

/* Contador de mensajes de SYNC enviados por canal 1. */
char sync_counter = 0;

/* Cola de eventos de impresiòn (prioridad baja-media) */
EventQueue printf_queue;

/* Cola de eventos de acceso a dispositivo CAN (prioridad alta) */
EventQueue can_dev_queue;

/* Dispositivos CAN */
CAN can[2]{
    {MBED_CONF_APP_CAN1_RD, MBED_CONF_APP_CAN1_TD},
    {MBED_CONF_APP_CAN2_RD, MBED_CONF_APP_CAN2_TD} 
};
 
/* Interfaz serie con PC */
static BufferedSerial serial_port(USBTX, USBRX);

/* Leer mensaje de CAN2 e imprimir en pantalla */
void can_read_message(int device_id)
{
    CANMessage msg;
    if (can[device_id].read(msg)) 
    {             
        led2 = !led2;    

        can_message_event ev;
        ev.sync_mark = 0xEB90;
        ev.event_type = 1;
        ev.canid = msg.id;
        ev.len = msg.len;
        memcpy(ev.data,msg.data,msg.len);
        serial_port.write(&ev.device_id, sizeof(can_message_event));

        // Debug
        /*
        printf("\nCAN%d RX - CANID: 0x%08x, LEN: %d, Data: ", device_id, msg.id, msg.len);
        for(int i=0;i<msg.len;i++)
        {
            printf("%02x ", msg.data[i]);
        } */            
    }
}

void serial_read_command()
{
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
}

 
int main() 
{
    // 9600-8-N-1
    serial_port.set_baud(9600);
    serial_port.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );

    /* Registrar la función de lectura de CAN en la interrupción Rx. 
       Notar que la función se ejecuta en una cola, y no directamente en la interrupción. */
    can[0].attach(can_dev_queue.event(&can_read_message, 0), CAN::IrqType::RxIrq);
    can[1].attach(can_dev_queue.event(&can_read_message, 1), CAN::IrqType::RxIrq);

    serial_port.sigio(can_dev_queue.event(&serial_read_command));

    /* Hilo de baja prioridad para imprimir mensajes. */    
    Thread printf_thread(osPriorityNormal);
    printf_thread.start(callback(&printf_queue, &EventQueue::dispatch_forever));

    /* Hilo de alta prioridad para manejo de dispositivos. */
    Thread can_event_thread(osPriorityHigh);
    can_event_thread.start(callback(&can_dev_queue, &EventQueue::dispatch_forever));

    /* Programación de mensajes SYNC. */
    //Ticker can_sync_ticker;
    //can_sync_ticker.attach(can_dev_queue.event(&can1_send_sync_message), 50ms);
       
    wait_us(osWaitForever);
}