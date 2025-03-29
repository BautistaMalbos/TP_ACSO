#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <stdint.h>
#define HASHMAP_SIZE 256

// Estructura BitMap utilizada para decodificar instrucciones ARM
typedef struct {
    uint32_t opcode;   // Código de operación (opcode)
    uint8_t rd;        // Registro destino
    uint8_t rn;        // Registro fuente 1
    uint8_t rm;        // Registro fuente 2
    uint8_t rt;        // Registro temporal
    uint16_t imm12;    // Inmediato de 12 bits
    uint8_t shamt;     // Shift amount (desplazamiento)
    int32_t imm19;     // Inmediato de 19 bits con signo (para B.cond)
    uint32_t address;  // Dirección o desplazamiento (para CB_TYPE y D_TYPE)
    uint8_t cond;      // Condición (para instrucciones condicionales)
} BitMap;


// Estructura para almacenar un manejador
typedef struct {
    uint32_t opcode;               // Opcode de la instrucción
    void (*handle)(BitMap bitmap); // Puntero a la función manejadora
} HashmapEntry;

#endif // ESTRUCTURAS_H