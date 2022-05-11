/**
 * @file cmd_def.h Definición de códigos de telecomandos (OPCODES).
 */
#pragma once

/** Definición de OPCODES (códigos de operación de los telecomandos). */
enum opcode_index_e {
    /** Enviar un mensaje CAN.
     */
	OPCODE_SEND_CAN_MSG 					= 0x00,

	// END Opcodes específicos de la aplicación
	OPCODE_LAST
};