Test de conexión CANBus de dos canales en STM32F7
=========================================

Aplicación en [MbedOS](https://os.mbed.com/mbed-os/) para control de dos canales CAN en Nucleo 144 STM32F767ZI.

Descripción: se utiliza un protocolo de paquetes para:

- Recibir comandos del controlador anfitriòn (PC): envìo de mensajes, configuración, etc.
- Enviar mensajes recibidos en el bus CAN (modo monitor).

Instrucciones
-------------

1. Importar proyecto en [Mbed Studio](https://os.mbed.com/studio/). Probado con MBedOS 6.
2. [Conectar HW de prueba](../../doc/hw_setup.md).
3. Grabar firmware.
4. Utilizar ejemplos de [libcanserial](../../host/libcanserial) para probar.
