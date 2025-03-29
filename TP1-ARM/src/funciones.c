#include "funciones.h"
#include "shell.h"
#include <stddef.h> // For NULL definition


void initialize_hashmap(HashmapEntry *hashmap) {
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        hashmap[i].opcode = 0;       // Inicializa el opcode como 0 (vacío)
        hashmap[i].handle = NULL;    // Inicializa el manejador como NULL
    }

    // Agregar entradas al hashmap
    hashmap[hash_function(0xD4400000)] = (HashmapEntry){0xD4400000, execute_hlt};// HLT
    hashmap[hash_function(0x8B200000)] = (HashmapEntry){0x8B200000, };//ADD (register)
    hashmap[hash_function(0xAB000000)] = (HashmapEntry){0xAB000000, execute_adds};// ADDS (shifted register)
    hashmap[hash_function(0xEB000000)] = (HashmapEntry){0xEB000000, execute_subs};// SUBS (shifted register)
    hashmap[hash_function(0xEA000000)] = (HashmapEntry){0xEA000000, execute_ands};// ANDS (register)
    hashmap[hash_function(0xF8000000)] = (HashmapEntry){0xF8000000, };//STUR 
    hashmap[hash_function(0x38000000)] = (HashmapEntry){0x38000000, };//STURB
    hashmap[hash_function(0x78000000)] = (HashmapEntry){0x78000000, };//STURH
    hashmap[hash_function(0x9B000000)] = (HashmapEntry){0x9B000000, execute_mul};// MUL (register)
    hashmap[hash_function(0xAA000000)] = (HashmapEntry){0xAA000000, execute_orr};// ORRS (register)
    hashmap[hash_function(0xCA000000)] = (HashmapEntry){0xCA000000, execute_eor};// EORS (register)
    hashmap[hash_function(0xD2800000)] = (HashmapEntry){0xD2800000, execute_movz};// MOVZ
    hashmap[hash_function(0xF8400000)] = (HashmapEntry){0xF8400000, };//LDUR
    hashmap[hash_function(0x38400000)] = (HashmapEntry){0x38400000, };//LDURB
    hashmap[hash_function(0x78400000)] = (HashmapEntry){0x78400000, }; //LDURH
    hashmap[hash_function(0xD3400000)] = (HashmapEntry){0xD3400000, execute_lsr};  // LSL/LSR
    hashmap[hash_function(0xF1000000)] = (HashmapEntry){0xF1000000, execute_subis}; // CMP/SUBIS
    hashmap[hash_function(0xB4000000)] = (HashmapEntry){0xB4000000, execute_b};    // CBZ
    hashmap[hash_function(0xB5000000)] = (HashmapEntry){0xB5000000, execute_b};    // CBNZ
    hashmap[hash_function(0x54000000)] = (HashmapEntry){0x54000000, execute_b};    // B.COND
    hashmap[hash_function(0xB1000000)] = (HashmapEntry){0xB1000000, execute_addis}; // ADDS IMM
    hashmap[hash_function(0x91000000)] = (HashmapEntry){0x91000000, execute_addis}; // ADD IMM
    hashmap[hash_function(0x14000000)] = (HashmapEntry){0x14000000, execute_b};    // B_TARGET
    hashmap[hash_function(0xD61F0000)] = (HashmapEntry){0xD61F0000, execute_b};    // BR
}
uint32_t hash_function(uint32_t opcode) {
return opcode % HASHMAP_SIZE; // Usa el módulo para calcular el índice
}

void execute_adds(BitMap bitmap) {
    // Sumar los registros rn y rm, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] + CURRENT_STATE.REGS[bitmap.rm];
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0); 

}


void execute_subs(BitMap bitmap) {
    // Restar los registros rn y rm, almacenar el resultado en rd
    // Si rd es 31, no se almacena el resultado
    uint32_t result = CURRENT_STATE.REGS[bitmap.rn] - CURRENT_STATE.REGS[bitmap.rm];
    if (bitmap.rd != 31) { //CMP
        NEXT_STATE.REGS[bitmap.rd] = result;
    }
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (result >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (result == 0);       // Zero flag
}

void execute_addis(BitMap bitmap) {
    // Sumar el registro rn y el inmediato, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12;

    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag

}
void execute_subis(BitMap bitmap) {
    // Restar el registro rn y el inmediato, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] - bitmap.imm12;

    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag


}

void execute_cmp(BitMap bitmap) {


}

void execute_lsr(BitMap bitmap) {
    // Desplazar a la derecha el registro rn por el inmediato, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] >> bitmap.imm12;
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag
}

void execute_lsl(BitMap bitmap) {
    // Desplazar a la izquierda el registro rn por el inmediato, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] << bitmap.imm12;
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag    
}

void execute_mul(BitMap bitmap) {
    // Multiplicar los registros rn y rm, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] * CURRENT_STATE.REGS[bitmap.rm];
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag
}
void execute_ands(BitMap bitmap) {
   // AND entre los registros rn y rm, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] & CURRENT_STATE.REGS[bitmap.rm];
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag
}

void execute_orr(BitMap bitmap) {
    // OR entre los registros rn y rm, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] | CURRENT_STATE.REGS[bitmap.rm];
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag
}

void execute_eor(BitMap bitmap) {
    // XOR entre los registros rn y rm, almacenar el resultado en rd
    NEXT_STATE.REGS[bitmap.rd] = CURRENT_STATE.REGS[bitmap.rn] ^ CURRENT_STATE.REGS[bitmap.rm];
    // Actualizar los flags de estado
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag
}

void execute_movz(BitMap bitmap) {
    
}

void execute_hlt() {
    RUN_BIT = 0;
}

void execute_b(BitMap bitmap) {
    switch(bitmap.cond){
        case 0x0:  // BEQ
            if (CURRENT_STATE.FLAG_Z) {
                NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.imm19 << 2);
            }
            break;
        case 0x1:  // BNE
            if (!CURRENT_STATE.FLAG_Z) {
                NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.imm19 << 2);
            }
            break;
        case 0xA:  // BGE
            if (CURRENT_STATE.FLAG_N == CURRENT_STATE.FLAG_Z) {
                NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.imm19 << 2);
            }
            break;
        case 0xB:  // BLT
            if (CURRENT_STATE.FLAG_N != CURRENT_STATE.FLAG_Z) {
                NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.imm19 << 2);
            }
            break;
        case 0xC:  // BGT
            if (!CURRENT_STATE.FLAG_Z && (CURRENT_STATE.FLAG_N == CURRENT_STATE.FLAG_Z)) {
                NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.imm19 << 2);
            }
            break;
        case 0xD:  // BLE
            if (CURRENT_STATE.FLAG_Z || (CURRENT_STATE.FLAG_N != CURRENT_STATE.FLAG_Z)) {
                NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.imm19 << 2);
            }
            break;
        
    }
    
}