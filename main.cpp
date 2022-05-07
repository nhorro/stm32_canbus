/** @brief Test de conectividad CANBus v0.0.1
 *  @details
        - v0.0.1: Se envía desde CAN1 mensaje de SYNC con contador 0-19 cada 50ms. Se imprimen mensajes recibidos en CAN2.
*/

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
CAN can1(MBED_CONF_APP_CAN1_RD, MBED_CONF_APP_CAN1_TD);
CAN can2(MBED_CONF_APP_CAN2_RD, MBED_CONF_APP_CAN2_TD);
 

/* Enviar mensaje SYNC desde CAN 1. */
void can1_send_sync_message() 
{    
    const uint32_t canid = 1337;
    char data[2];
    data[0] = 0;
    data[1] = sync_counter;
    can1.write(CANMessage(canid, data, 2));

    // Incrementar contador con rollover en 19.
    sync_counter++;
    sync_counter%=19;
    
    // Led indicador
    led1 = !led1;
}

/* Leer mensaje de CAN2 e imprimir en pantalla */
void can2_read_message()
{
    CANMessage msg;
    if (can2.read(msg)) 
    {             
        led2 = !led2;    
        printf("\nCAN2 RX - CANID: 0x%08x, LEN: %d, Data: ", msg.id, msg.len);
        for(int i=0;i<msg.len;i++)
        {
            printf("%02x ", msg.data[i]);
        }             
    }
}

 
int main() 
{
    /* Registrar la función de lectura de CAN2 en la interrupción Rx. 
       Notar que la función se ejecuta en una cola, y no directamente en la interrupción. */
    can2.attach(can_dev_queue.event(&can2_read_message), CAN::IrqType::RxIrq);

    /* Hilo de baja prioridad para imprimir mensajes. */    
    Thread printf_thread(osPriorityNormal);
    printf_thread.start(callback(&printf_queue, &EventQueue::dispatch_forever));

    /* Hilo de alta prioridad para manejo de dispositivos. */
    Thread can_event_thread(osPriorityHigh);
    can_event_thread.start(callback(&can_dev_queue, &EventQueue::dispatch_forever));

    /* Programación de mensajes SYNC. */
    Ticker can_sync_ticker;
    can_sync_ticker.attach(can_dev_queue.event(&can1_send_sync_message), 50ms);
       
    wait_us(osWaitForever);
}