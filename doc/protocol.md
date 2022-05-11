Descripción del protocolo de comunicación
=========================================

Para comunicarse con la computadora, se utiliza un protocolo de software basado en mensajes, donde cada paquete tiene el siguiente formato:

```
"PKT!<payload><crc16>\n"
```

siendo:

- "PKT!" la palabra de sincronismo (4bytes).
- *payload* es el la carga útil del paquete, una secuencia de un máximo de 64 bytes.
- *crc16* es el código CRC del payload.
- *\n* es el byte terminador del paquete.

Se definen dos tipos de paquetes:

- **Comandos**: la carga útil se compone de un byte indicando un código de operación (OPCODE)  seguido de datos específicos del comando.
- **Reportes**: la carga útil consiste en un byte indicando un código de reporte (REPORT_ID) seguido de datos específicos de cada reporte. 

El anfitrión envía comandos de inyección de mensajes y configuración al firmware.
El firmware envía reportes cada vez que ocurre un evento de interés, como por ejemplo, recepción de uno o más mensajes en un canal.

El siguiente diagrama muestra la máquina de estados que se utiliza para reconocer el protocolo.

![Máquina de estados del protocolo de comunicación](./assets/serial-protocol.png)

Secuencia nominal:

1. El estado inicial es *LIBRE* (es decir, se está a la espera de iniciar la recepción de un paquete).
2. Ante la llegada de un nuevo caracter, si el mismo coincide con el esperado para la secuencia de sincronismo se avanza al siguiente estado.
3. Una vez reconocido el último caracter de la secuencia de sincronismo se inicializa en cero  un contador de bytes leidos *n_recv* y se pasa al estado de recepción de bytes *LEER_C*.
4. En el estado *LEER_C* cada nuevo byte recibido se almacena en un buffer y se incrementa *n_recv*. El máximo es *MAX_BYTES*=64.
5. Se sale de *LEER_C* si se recibe el caracter terminador. En este caso se pasa al estado de *VALIDAR_Y_DESPACHAR*.
6. En el estado de validación se realiza el chequeo de CRC, y si es válido se despacha el paquete (es decir, se ejecuta la función asociada a ese OPCODE). De lo contrario se pasa a *ERROR* con el código *CRC_INVÁLIDO*.

Error:

1. Recibir un caracter inesperado, superar la cantidad máxima de bytes de un paquete o un error en CRC llevará al estado de error. En este estado se incrementará el contador del tipo de error y se transicionará al estado inicial.

Definición y ejecución de comandos
----------------------------------

Una aplicación debe explicitar los telecomandos que implementa en un arreglo, quedando cada telecomando definido a partir de:

- Un código de operación (OPCODE). Se comienza a contar desde 0x80 y este valor se resta al acceder a la tabla.
- Una función miembro de la clase de la aplicación que es llamada cuando se recibe ese comando.
- Opcionalmente, el telecomando puede contener parámetros, en cuyo caso la aplicación podrá leerlos accediendo al puntero del buffer conteniendo el paquete recibido. 
- Banderas indicando si el comando debe indicar su status de ejecución y actualizar contadores. Para comandos no críticos o que se envian periódicamente, puede ser aceptable que en algunos casos no sean procesados.

```c++
    /** Flags de un telecomando 
     *  Por defecto los telecomandos sólo se reciben y se procesan.
     *  Los flags permiten incrementar el contador de recibidos y/o generar un reporte conteniendo
     *  el estado de ejecución del comando.
     */
	enum opcode_flags {
		default_flags 				   	= 0x00, 
		update_execution_counters		= 0x01, /**< Actualizar contador de ejecución. */
		enable_execution_status_report 	= 0x02  /**< Generar un reporte de status. */
	};

	using opcode_callback = application::error_code(application::*)(const uint8_t* payload, uint8_t n);

    /** Descriptor de un OPCODE. */
	struct opcode_descr {
		opcode_callback fn;
		uint8_t flags;    
	};
```

El código a continuación muestra como la aplicación define sus comandos:

```c++
	opcodes
		{ 			
			{ &can_service::send_can_message, opcode_flags::default_flags },
      { &can_service::enable_sync, opcode_flags::default_flags },
      /* ... */
		}
```

Para cada comando se define una función que es llamada cuando se recibe un paquete válido cuyo código de operación (ver OPCODE en descripción de protocolo) coincide con el índice de la tabla (restando el 0x80 a partir del que se empiezan a contar). 

El ejemplo a continuación muestra el comando de envío de un mensaje CAN.

```c++
can_service::error_code can_service::cmd_send_message(const uint8_t* payload, uint8_t n)
{
    // Nota:: se omite validación de parámetros.
    uint32_t device_id = payload[0];
    const uint32_t canid = (payload[1] << 24) | (payload[2] << 16) | (payload[3] << 8) | (payload[4] << 0);
    size_t msg_len = payload[5];        
    can[device_id].write(CANMessage(canid, &payload[6], msg_len));

    return error_code::success;
}
```

Del mismo modo, cada vez que se recibe un mensaje en un canal se genera un reporte:

```c++
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
```

Pendiente: referencia de comandos y reportes.
