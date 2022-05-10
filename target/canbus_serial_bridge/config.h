#pragma once

/* Configuración de interfaces CAN para dos canales en Nucleo-144 STM32F767ZI. 
   Puede requerir reasignaciòn para para otras versiones de placas.*/

/* Canal 1 */
#define MBED_CONF_APP_CAN1_RD PB_8
#define MBED_CONF_APP_CAN1_TD PB_9

/* Canal 2 */
#define MBED_CONF_APP_CAN2_RD PB_5
#define MBED_CONF_APP_CAN2_TD PB_6

#define PACKET_TIMEOUT_IN_MS 500
