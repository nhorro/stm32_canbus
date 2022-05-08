Test de conexión CANBus de dos canales en STM32F7
=========================================

Aplicación en [MbedOS](https://os.mbed.com/mbed-os/) para control de dos canales CAN en Nucleo 144 STM32F767ZI.

Descripción: verificar la conexión entre dos canales CAN. Se envían periódicamente mensajes por CAN0 y se imprimen los mensajes recibidos en CAN1. 

Instrucciones
-------------

1. Importar proyecto en [Mbed Studio](https://os.mbed.com/studio/). Probado con MBedOS 6.
2. [Conectar HW de prueba](../../doc/hw_setup.md).
3. Grabar firmware.