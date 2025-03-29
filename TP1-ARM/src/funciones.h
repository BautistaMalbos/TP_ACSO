#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdint.h>
#include "shell.h"
#include "estructuras.h" // Asegúrate de incluir la definición de BitMap

void initialize_hashmap(HashmapEntry *hashmap);
uint32_t hash_function(uint32_t opcode);


void execute_adds(BitMap bitmap);
void execute_subs(BitMap bitmap);
void execute_addis(BitMap bitmap);
void execute_subis(BitMap bitmap);
void execute_cmp(BitMap bitmap);
void execute_lsr(BitMap bitmap);
void execute_lsl(BitMap bitmap);
void execute_mul(BitMap bitmap);
void execute_ands(BitMap bitmap);
void execute_eor(BitMap bitmap);
void execute_orr(BitMap bitmap);
void execute_movz(BitMap bitmap);
void execute_hlt();
void execute_b(BitMap bitmap);
void execute_cbz(BitMap bitmap);
void execute_cbnz(BitMap bitmap);

#endif