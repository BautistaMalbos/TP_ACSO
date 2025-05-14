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


string_proc_list_create_asm:
    mov     edi, SIZE_LIST
    call    malloc
    cmp     rax, 0
    jnz     .inicializar_lista  
    jmp     .fin_lista           

.inicializar_lista:
    mov     qword [rax+8], 0     
    mov     qword [rax],   0     

.fin_lista:
    ret

string_proc_node_create_asm:
    movzx   edx, dil           
    mov     rcx, rsi         
    push    rcx               
    push    rdx              
    mov     edi, SIZE_NODE
    call    malloc
    pop     rdx             
    pop     rcx                
    test    rax, rax
    je      .fin_nodo

    xor     r9, r9             
    mov     [rax+8], r9         
    mov     [rax],   r9        
    mov     byte [rax+16], dl   
    mov     [rax+24], rcx       

.fin_nodo:
    ret


string_proc_list_add_node_asm:
    push    rdi                
    movzx   edi, sil            
    mov     rsi, rdx          
    call    string_proc_node_create_asm
    pop     r9                  

    test    rax, rax
    je      .fin_agregar

    mov     rcx, [r9+8]       
    test    rcx, rcx
    je      .lista_vacia

    mov     [rcx],    rax       
    mov     [rax+8],  rcx     
    mov     [r9+8],  rax     
    jmp     .fin_agregar

.lista_vacia:
    mov     [r9+8],  rax       
    mov     [r9],    rax      

.fin_agregar:
    ret

string_proc_list_concat_asm:
    push %rbp
    mov %rsp, %rbp
    push %rdi     # Guardamos a
    push %rsi     # Guardamos b

    # strlen(a)
    mov %rdi, %rdi    # a ya está en rdi
    call strlen
    mov %rax, %r8     # len1 = r8

    # strlen(b)
    pop %rsi          # recuperar b
    mov %rsi, %rdi    # poner b en rdi
    call strlen
    mov %rax, %r9     # len2 = r9

    # totalLength = len1 + len2
    add %r8, %r9      # r9 = len1 + len2
    add $1, %r9       # +1 para el null terminator

    # malloc(totalLength)
    mov %r9, %rdi     # tamaño a rdi
    call malloc
    mov %rax, %r10    # puntero resultado = r10

    # strcpy(result, a)
    pop %rsi          # recuperar a
    mov %rsi, %rsi    # a en rsi
    mov %r10, %rdi    # destino en rdi
    call strcpy

    # strcat(result, b)
    mov %r10, %rdi    # destino sigue siendo result
    mov %rsi, %rsi    # b en rsi
    call strcat

    mov %r10, %rax    # retorno = puntero result

    pop %rbp
    ret


