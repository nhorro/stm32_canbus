Test de conexión CANBus de dos canales en STM32F7
=========================================

Aplicación en [MbedOS](https://os.mbed.com/mbed-os/) para control de dos canales CAN en Nucleo 144 STM32F767ZI.

Descripción: verificar la conexión entre dos canales CAN. Se envían periódicamente mensajes  por CAN0 y se imprimen los mensajes recibidos en CAN1. 

Instrucciones
-------------

1. Importar proyecto en [Mbed Studio](https://os.mbed.com/studio/). Probado con MBedOS 6.
2. [Conectar HW de prueba](../../doc/hw_setup.md).
3. Grabar firmware.
4. Monitorear salida en consola de debug. Se debe ver:

```
...
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 00 
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 01 
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 02 
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 03 
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 04 
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 05 
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 06 
CAN1 RX - CANID: 0x00000539, LEN: 2, Data: 00 07 
...
```

