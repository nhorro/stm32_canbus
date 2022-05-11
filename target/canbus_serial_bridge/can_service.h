#pragma once

#include "BufferedSerial.h"
#include "SerialBase.h"

#if !DEVICE_CAN
#error [NOT_SUPPORTED] CAN not supported for this target
#endif

#include "mbed.h"

#include "config.h"

#include "protocol.h"
#include "cmd_def.h"

class can_service : 
    private protocol::packet_encoder,
    private protocol::packet_decoder

{
public:
    can_service();
    ~can_service() {}
    void setup();
private:   
    void serial_read_command();
    void can_read_message(int device_id);
    void can1_send_sync_message();

    /* Led indicador de transmisión en canal 1 */
    DigitalOut led1;

    /* Led indicador de recepción en canal 2 */
    DigitalOut led2;

    /* Contador de mensajes de SYNC enviados por canal 1. */
    char sync_counter;

    /* Cola de eventos de acceso a dispositivo CAN (prioridad alta) */
    EventQueue can_dev_queue;

    /* Hilo de alta prioridad para manejo de dispositivos. */
    Thread can_event_thread;
    
    /* Ticker para mensajes SYNC */
    Ticker can_sync_ticker;

    /* Dispositivos CAN */
    CAN can[2];
    
    /* Interfaz serie con PC */
    BufferedSerial serial_port;

    /* Protocolo de comunicación serie */
	using opcode_callback = can_service::error_code(can_service::*)(const uint8_t* payload, uint8_t n);

    /** Telecomandos */

    /** Flags de un telecomando 
     *  Por defecto los telecomandos sólo se reciben y se procesan.
     */
	enum opcode_flags {
		default_flags 				   	= 0x00
	};

    /** Descriptor de un OPCODE. */
	struct opcode_descr {
		opcode_callback fn;
		uint8_t flags;    
	};
        
    /* Tabla de opcodes */
	opcode_descr opcodes[OPCODE_LAST];

	/* requeridos por packet_decoder */

    /** Dispatcher de telecomandos recibidos. 
      * @param payload bytes del paylaod del telecomando.
      * @param n cantidad de bytes.
      * @warning Tener mucho cuidado de desde donde se llama a esta función.
      *          Si se está ejecutando la FSM en el contexto de interrupciones,
      *          Despachar al contexto de aplicación.
      */    
	void handle_packet(const uint8_t* payload, uint8_t n) override;

    /** Establecer código de error de último telecomando procesado.
      * @param error_code código de error (0=éxito)
      */    
	void set_error(error_code ec) override;

    /** Implementación de envío de bytes. En este caso se utiliza puerto serie.
     * @param buf buffer con datos a transmitir (inmutable).
     * @param n cantidad de bytes.
     */
	void send_impl(const uint8_t* buf, uint8_t n) override;

    /** Manejar errores de conexión (cuando no se recibe HEARTBEAT).
     */
	void handle_connection_lost() override;


    /* Comandos */

    
    can_service::error_code cmd_send_message(const uint8_t* payload, uint8_t n);
};