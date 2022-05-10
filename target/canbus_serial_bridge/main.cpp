/** @brief Test de conectividad CANBus v0.0.1
 *  @details
        - v0.0.1: Se envía desde CAN1 mensaje de SYNC con contador 0-19 cada 50ms. Se imprimen mensajes recibidos en CAN2.
*/

#include "can_service.h"

can_service service;
 
int main() 
{
    service.setup();    
    wait_us(osWaitForever);
}