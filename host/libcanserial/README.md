libcanserial 
============

Biblioteca en C++ para comunicarse con STM32 con firmware para control de CANBus.

**Instrucciones**

Requisitos:

- C++
- Boost
- CMake

Para compilar:

~~~bash
cd libcanserial
mkdir build
cd build
cmake ..
make
~~~

Ejecución de programa de ejemplo:

~~~bash
cd build
./example/stm32canbus_serial_example /dev/ttyACM0 
~~~

