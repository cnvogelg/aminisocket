    section "text",code
    
    IFND HARDARE_CIA_I
    include "hardware/cia.i"
    ENDC
    
    IFND EXEC_MACROS_I
    include "exec/macros.i"
    ENDC
    
    xdef    _interrupt
    
BIT_RX  equ 1
BIT_ACK equ 2
    
ciaa     equ     $bfe001
ciab     equ     $bfd000
BaseAX   equ     ciab+ciapra

OutReg   equ     ciaa+ciaprb
StatusReg equ    ciab+ciapra
    
; ----- interrupt handler for parport -----
; a1 = struct state_s *ptr
_interrupt:
    btst    #BIT_RX,(a1)  
    bne.s   setack          

    move.b  StatusReg,d0            ; PAR_STATUS==CIAF_PRTRPOUT ? 
    andi.b  #7,d0
    cmp.b   #2,d0   
    bne.s   skipint

    bset    #BIT_RX,(a1)
    move.l  2(a1),d0
    movea.l 6(a1),a6       ; Signal
    movea.l 10(a1),a1
gosig:  
    JSRLIB  Signal
skipint: 
    moveq #0,d0                     ; set Z flag => next server
    rts
setack: ; already in RX mode...
    bset  #BIT_ACK,(a1)
    moveq #0,d0                     ; set Z flag => next server
    rts
    
    
    end

    