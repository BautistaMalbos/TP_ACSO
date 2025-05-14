; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0
%define SIZE_LIST  16
%define SIZE_NODE  32

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm
extern string_proc_list_concat

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat


string_proc_list_create_asm:
        push    rbp
        mov     rbp, rsp

        ; Reservar espacio para la lista (2 punteros: first, last)
        mov     edi, 16
        call    malloc
        test    rax, rax
        je      .error

        ; Inicializar: first = NULL, last = NULL
        mov     QWORD [rax], 0
        mov     QWORD [rax+8], 0

        ; Devolver puntero a la lista
        jmp     .done

.error:
        xor     eax, eax

.done:
        leave
        ret


string_proc_node_create_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32

        mov     r10d, edi                   ; type en r10b
        mov     QWORD [rbp-32], rsi         ; guardar hash
        mov     BYTE  [rbp-20], r10b        ; guardar type

        mov     edi, 32
        call    malloc
        mov     QWORD [rbp-8], rax

        cmp     QWORD [rbp-8], 0
        jne     .cont

        xor     eax, eax
        jmp     .fin

.cont:
        mov     r11, QWORD [rbp-8]
        movzx   ecx, BYTE [rbp-20]
        mov     BYTE [r11+16], cl

        mov     rcx, QWORD [rbp-32]
        mov     QWORD [r11+24], rcx

        mov     QWORD [r11], 0
        mov     QWORD [r11+8], 0

        mov     rax, r11

.fin:
        leave
        ret

string_proc_list_add_node_asm:
        push    rbp
        mov     rbp, rsp

        ; Guardar parámetros
        mov     rbx, rdi     ; list
        movzx   ecx, sil     ; type
        mov     rdx, rdx     ; hash

        ; Crear nodo
        mov     edi, ecx
        mov     rsi, rdx
        call    string_proc_node_create_asm
        test    rax, rax
        je      .done        ; si malloc falló, terminamos sin hacer nada

        ; rax → new_node
        mov     r8, rax

        ; Si lista vacía (last == NULL)
        mov     rax, [rbx+8]
        test    rax, rax
        jne     .append

        ; Lista vacía → first = last = new_node
        mov     [rbx], r8
        mov     [rbx+8], r8
        jmp     .done

.append:
        ; Conectar nodo actual al nuevo
        mov     rax, [rbx+8]         ; last
        mov     [rax], r8            ; last->next = new_node
        mov     [r8+8], rax          ; new_node->prev = last
        mov     [rbx+8], r8          ; list->last = new_node

.done:
        leave
        ret

string_proc_list_concat_asm:
    jmp     string_proc_list_concat

