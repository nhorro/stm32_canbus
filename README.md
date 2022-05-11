# Interfaz CANBus de dos canales en STM32F7

Este repositorio contiene código fuente para implementar una interfaz CANBus utilizando dos canales de la placa Nucleo 144 STM32F767ZI.

**Contenido**

- [Instructivo para armar ambiente de prueba](doc/hw_setup.md) 
- [Descripción de protocolo de comunicación](doc/protocol.md)
- Código fuente para firmware
  - [Test de conexión](./target/can_connection_test/README.md).
  - [Aplicaciòn "bridge" bàsica](./target/canbus_serial_bridge/README.md).
- Código fuente del lado del anfitrión (PC x86/x64)
  - [Biblioteca para control y monitoreo de CAN por puerto serie](host/libcanserial/README.md)

