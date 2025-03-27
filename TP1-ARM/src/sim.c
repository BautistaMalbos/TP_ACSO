#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "shell.h"

typedef enum { R_TYPE, I_TYPE, D_TYPE, B_TYPE, CB_TYPE, IW_TYPE, SYS_TYPE ,UNKNOWN } InstrType;

typedef struct {
    uint32_t opcode;
    uint8_t rd, rn, rm, rt;
    uint16_t imm12;
    uint8_t shamt;
    uint32_t address;

} BitMap;

void process_instruction();
InstrType define_instruction_type(uint32_t instruction);
BitMap decode_instruction(uint32_t instruction, InstrType type);
void print_decoded_instruction(BitMap bitmap, InstrType type);

void process_instruction()
{   
    
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    InstrType type = define_instruction_type(instruction);
    printf("Tipo de instruccion: %d\n", type);
    BitMap bitmap = decode_instruction(instruction, type);
    print_decoded_instruction(bitmap, type);

    switch (type)
    {
        case R_TYPE:
            printf("Tipo R\n");
            u_int32_t result;   
            switch (bitmap.opcode)
            {
                case(0x00000558):
                    // ADDS (shifted register)
                    result = CURRENT_STATE.REGS[bitmap.rn] + CURRENT_STATE.REGS[bitmap.rm];
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;

                case(0x00000758):
                    // SUBS (shifted register)
                    result = CURRENT_STATE.REGS[bitmap.rn] - CURRENT_STATE.REGS[bitmap.rm];
                    if (bitmap.rd != 31) { //CMP
                        NEXT_STATE.REGS[bitmap.rd] = result;
                    }
                    break;

                case(0x000005A8):
                    // ANDS (shifted register)
                    result = CURRENT_STATE.REGS[bitmap.rn] & CURRENT_STATE.REGS[bitmap.rm];
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;

                case(0x000005B1):
                    // EORS (shifted register)
                    result = CURRENT_STATE.REGS[bitmap.rn] ^ CURRENT_STATE.REGS[bitmap.rm];
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;

                case(0x00000D58):
                    // MUL (shifted register)
                    result = CURRENT_STATE.REGS[bitmap.rn] * CURRENT_STATE.REGS[bitmap.rm];
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;
            }
        
            NEXT_STATE.FLAG_N = (result >> 31) & 1;  // Negative flag
            NEXT_STATE.FLAG_Z = (result == 0);       // Zero flag
            break;


        case I_TYPE:
            printf("Tipo I\n");
            u_int32_t result;
            switch (bitmap.opcode){
                case 0x69A:  // Logical Shift Right (LSR)
                    result = CURRENT_STATE.REGS[bitmap.rn] >> bitmap.imm12;
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;

                case 0x69B:  // Logical Shift Left (LSL)
                    result = CURRENT_STATE.REGS[bitmap.rn] << bitmap.imm12;
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;

                case(0x00000b10):
                    // ADDS (Immediate)
                    result = CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12;
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;

                case(0x00000f10):
                    // SUBS (Immediate)
                    result = CURRENT_STATE.REGS[bitmap.rn] - bitmap.imm12;
                    NEXT_STATE.REGS[bitmap.rd] = result;
                    break;
            }
            
            NEXT_STATE.FLAG_N = (result >> 31) & 1;  // Negative flag
            NEXT_STATE.FLAG_Z = (result == 0);       // Zero flag
            break;

            NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[bitmap.rd] >> 31) & 1;  // Negative flag
            NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[bitmap.rd] == 0);       // Zero flag

        case D_TYPE:
            printf("Tipo D\n");
            switch (bitmap.opcode) {
                case 0x7C0:  // STUR (almacena 64 bits)
                    mem_write_32(CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12, CURRENT_STATE.REGS[bitmap.rt]);
                    break;

                case 0x7C2:  // LDUR (carga 64 bits)
                    NEXT_STATE.REGS[bitmap.rt] = mem_read_32(CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12);
                    break;  

                case 0x1C0:  // **STURB** (almacena 1 byte)
                    mem_write_8(CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12, CURRENT_STATE.REGS[bitmap.rt] & 0xFF);
                    break;

                case 0x1C2:  // **LDURB** (carga 1 byte)
                    NEXT_STATE.REGS[bitmap.rt] = mem_read_8(CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12);
                    break;

                case 0x3C2:  // **LDURH** (carga 2 bytes)
                    NEXT_STATE.REGS[bitmap.rt] = mem_read_16(CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12);
                    break;

                case 0x3C0:  // **STURH** (almacena 2 bytes)
                    mem_write_16(CURRENT_STATE.REGS[bitmap.rn] + bitmap.imm12, CURRENT_STATE.REGS[bitmap.rt] & 0xFFFF);
                    break;
            }
            break;

        case B_TYPE:
            printf("Tipo B\n");
            switch(bitmap.opcode){
                
                

                
            }
            // Manejar instrucciones de tipo B
            break;
        

        case CB_TYPE:
            printf("Tipo CB\n");
            switch (bitmap.opcode)
            {
                case 0x54:  // BEQ
                    if (CURRENT_STATE.FLAG_Z) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
            
                case 0x55:  // BNE
                    if (!CURRENT_STATE.FLAG_Z) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
                case 0x28:  // BGE
                    if (CURRENT_STATE.FLAG_N == CURRENT_STATE.FLAG_Z) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
                    
                case 0x540:  // BLT
                    if (CURRENT_STATE.FLAG_N) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
         
                case 0x5C: // BGT
                    if (!CURRENT_STATE.FLAG_Z && (CURRENT_STATE.FLAG_N == CURRENT_STATE.FLAG_Z)) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
                case 0x5D: // BLE
                    if (CURRENT_STATE.FLAG_Z || (CURRENT_STATE.FLAG_N != CURRENT_STATE.FLAG_Z)) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
                case 0xB4:  // CBZ
                    if (CURRENT_STATE.REGS[bitmap.rt] == 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
                case 0xB5:  // CBNZ
                    if (CURRENT_STATE.REGS[bitmap.rt] != 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + ((int32_t)bitmap.address << 2);
                    }
                    break;
                default:
                    break;
            }
            break;
            

        case IW_TYPE:
            printf("Tipo IW\n");
            switch(bitmap.opcode){
                case 0x25:  // MOVZ
                    NEXT_STATE.REGS[bitmap.rd] = bitmap.imm12;
                    break;
            }
            // Manejar instrucciones de tipo IW
            break;

        case SYS_TYPE:
            printf("Tipo SYS\n");
            // Manejar instrucciones de tipo SYS
            RUN_BIT = FALSE;
            break;
        
        default:
            break;
            
    
    }
    CURRENT_STATE.PC = NEXT_STATE.PC;
}

InstrType define_instruction_type(uint32_t instruction)
{
    if ((instruction & 0xFFFF0000) == 0xD4400000) return SYS_TYPE; // HLT
    if ((instruction & 0xFFE00000) == 0xAB000000) return R_TYPE;  // ADDS (shifted register)
    if ((instruction & 0xFFE00000) == 0xEA000000) return R_TYPE;  // ANDS (shifted register)
    if ((instruction & 0xFFE00000) == 0xEB000000) return R_TYPE;  // SUBS (shifted register)
    if ((instruction & 0xFFE00000) == 0x6A800000) return R_TYPE;  // ORRS (shifted register)
    if ((instruction & 0xFFE00000) == 0xCA000000) return R_TYPE;  // EORS (shifted register)
    if ((instruction & 0xFFFFFC00) == 0x1B007C00) return R_TYPE; // MUL
    if ((instruction & 0xFFF80000) == 0xB1000000) return I_TYPE;  // ADDS (Immediate)
    if ((instruction & 0xFFF80000) == 0xF1000000) return I_TYPE;  // SUBS (Immediate)
    if ((instruction & 0x7C000000) == 0x08000000) return I_TYPE;
    if ((instruction & 0x7C000000) == 0x0C000000) return I_TYPE;
    if ((instruction & 0xFFC00000) == 0x7C000000) return D_TYPE; // STUR (64 bits)
    if ((instruction & 0xFFC00000) == 0x7C400000) return D_TYPE; // LDUR (64 bits)
    if ((instruction & 0xFFC00000) == 0x1C000000) return D_TYPE; // STURB (8 bits)
    if ((instruction & 0xFFC00000) == 0x1C400000) return D_TYPE; // LDURB (8 bits)
    if ((instruction & 0xFFC00000) == 0x3C000000) return D_TYPE; // STURH (16 bits)
    if ((instruction & 0xFFC00000) == 0x3C400000) return D_TYPE; // LDURH (16 bits)
    if ((instruction & 0x7E000000) == 0x20000000) return B_TYPE;
    if ((instruction & 0x7E000000) == 0x54000000) return B_TYPE; // B.cond (including BLE) (mismo que cb type?)
    if ((instruction & 0x7E000000) == 0x28000000) return CB_TYPE;
    if ((instruction & 0x7E000000) == 0x54000000) return CB_TYPE; //BLT
    if ((instruction & 0x7E000000) == 0x5C000000) return CB_TYPE; // BGT
    if ((instruction & 0x7E000000) == 0x5D000000) return CB_TYPE; // BLE
    if ((instruction & 0x7E000000) == 0x34000000) return CB_TYPE; // CBZ
    if ((instruction & 0x7E000000) == 0x35000000) return CB_TYPE; // CBNZ
    if ((instruction & 0x7C000000) == 0x60000000) return IW_TYPE;

        
    return UNKNOWN;
}

BitMap decode_instruction(uint32_t instruction, InstrType type)
{
    BitMap bitmap = {0};
    switch (type)
    {
        case R_TYPE:
            bitmap.opcode = (instruction & 0xFFE00000) >> 21;
            bitmap.rd = (instruction & 0x0000001F);          // Bits 4:0
            bitmap.rn = (instruction & 0x000003E0) >> 5;     // Bits 9:5
            bitmap.rm = (instruction & 0x001F0000) >> 16;    // Bits 20:16
            bitmap.shamt = (instruction & 0x0000FC00) >> 10; // Bits 15:10
            break;

        case I_TYPE:
            bitmap.opcode = (instruction & 0xFFF00000) >> 20;
            bitmap.rd = (instruction & 0x0000001F);          // Bits 4:0
            bitmap.rn = (instruction >> 5) & 0x1F;
            bitmap.imm12 = (instruction >> 10) & 0xFFF;
            
            break;

        case D_TYPE:
            bitmap.opcode = (instruction & 0xFFE00000) >> 21;
            bitmap.rd = (instruction & 0x0000001F);          // Bits 4:0
            bitmap.rn = (instruction & 0x000003E0) >> 5;     // Bits 9:5
            bitmap.imm12 = (instruction & 0x003FFC00) >> 10; // Bits 21:10
            bitmap.rt = (instruction & 0x01F00000) >> 20;    // Bits 24:20
            break;

        case B_TYPE:
            bitmap.opcode = (instruction & 0xFC000000) >> 26;
            bitmap.address = (instruction & 0x03FFFFFF);     // Bits 25:0
            break;

        case CB_TYPE:
            bitmap.opcode = (instruction & 0xFF000000) >> 24;
            bitmap.address = (instruction & 0x00FFFFE0) >> 5; // Bits 23:5
            if (bitmap.address & (1 << 18)) { // Si el bit 18 está establecido, es un número negativo
                bitmap.address |= 0xFFF80000; // Extiende el signo a 32 bits
            }
            bitmap.rt = (instruction & 0x0000001F);          // Bits 4:0
            break;

        case IW_TYPE:
            bitmap.opcode = (instruction & 0xFFE00000) >> 21;
            bitmap.rd = (instruction & 0x0000001F);          // Bits 4:0
            bitmap.imm12 = (instruction & 0x001FFFE0) >> 5;  // Bits 20:5
            break;

        case SYS_TYPE:
            bitmap.opcode = (instruction & 0xFFFF0000) >> 16;
            break;

        default:
            break;
    }

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    return bitmap;
}

void print_decoded_instruction(BitMap bitmap, InstrType type)
{
    printf("Opcode: 0x%X\n", bitmap.opcode);
    
    if (type == R_TYPE) {
        printf("RD: X%d, RN: X%d, RM: X%d, SHAMT: %d\n", bitmap.rd, bitmap.rn, bitmap.rm, bitmap.shamt);
    } else if (type == I_TYPE) {
        printf("RD: X%d, RN: X%d, IMM12: %d\n", bitmap.rd, bitmap.rn, bitmap.imm12);
    } else if (type == D_TYPE) {
        printf("RD: X%d, RN: X%d, IMM12: %d, RT: X%d\n", bitmap.rd, bitmap.rn, bitmap.imm12, bitmap.rt);
    } else if (type == B_TYPE) {
        printf("ADDRESS: 0x%X\n", bitmap.address);
    } else if (type == CB_TYPE) {
        printf("RT: X%d, ADDRESS: 0x%X\n", bitmap.rt, bitmap.address);
    } else if (type == IW_TYPE) {
        printf("RD: X%d, IMM12: %d\n", bitmap.rd, bitmap.imm12);
    } else if (type == SYS_TYPE) {
        printf("HALT\n");
    } else {
        printf("UNKNOWN INSTRUCTION\n");
    }
}