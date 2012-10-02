    section "text",code
    
    ; ----- headers -----
    IFND HARDARE_CIA_I
    include "hardware/cia.i"
    ENDC
    
    IFND EXEC_MACROS_I
    include "exec/macros.i"
    ENDC

    IFND EXEC_TYPES_I
    INCLUDE "exec/types.i"
    ENDC

    ; ----- globals -----
    xdef    _interrupt
    xdef    _hwsend
    xdef    _hwrecv
    
    xref    _CRC16
    xref    _cia_base
    
    ; ----- state_s structure -----
    STRUCTURE state,0
    UWORD     s_Flags
    ULONG     s_IntSigMask
    APTR      s_SysBase
    APTR      s_ServerTask
    UWORD     s_MaxPacketSize
    UBYTE     s_UseCRC
    UBYTE     s_TimeOut
    LABEL     state_SIZE
    
    ; ----- packet_s structure -----
    STRUCTURE packet,0
    UWORD     p_Sync
    UWORD     p_Size
    UWORD     p_CRC
    LABEL     packet_SIZE
    
    IFND TRUE
TRUE  equ 1
    ENDC
    IFND FALSE
FALSE equ 0
    ENDC
    
    ; ----- state_s s_Flags -----
FLAGS_BIT_RX  equ 1
FLAGS_BIT_ACK equ 2

BIT_PTYPE_CRC equ 15 ; crc flag in p_Type
    
ciaa     equ     $bfe001
ciab     equ     $bfd000

OutReg   equ     ciaa+ciaprb
StatusReg equ    ciab+ciapra

HS_REQUEST equ CIAB_PRTRPOUT
HS_LINE    equ CIAB_PRTRBUSY

SETSELECT MACRO
      bset     #CIAB_PRTRSEL,StatusReg   ; raise PRTSEL line
      ENDM

CLRSELECT MACRO
      bclr     #CIAB_PRTRSEL,StatusReg   ; lower PRTSEL line
      ENDM

SETCIAOUTPUT MACRO
      st       ciaa+ciaddrb                ; data dir. => output
      ENDM

SETCIAINPUT MACRO
      sf       ciaa+ciaddrb                ; data dir. => input
      ENDM

SYNCBYTE_HEAD    equ     $42
SYNCBYTE_CRC     equ     $01
SYNCBYTE_NOCRC   equ     $02
SYNCWORD_CRC     equ     ((SYNCBYTE_HEAD<<8)|SYNCBYTE_CRC)
SYNCWORD_NOCRC   equ     ((SYNCBYTE_HEAD<<8)|SYNCBYTE_NOCRC)

;----------------------------------------------------------------------------
;
; NAME
;     interrupt() - ICR FLG interrupt server
;
; SYNOPSIS
;     void interrupt(struct struct_s *)
;                    A1
;
; FUNCTION
;     This is called from CIA resource on the receipt of an parallel port
;     FLG line interrupt. This is the case if the other side starts
;     transmission and writes the first byte to our port.
;
;     We recognise this here and propagate the information to the server
;     task by Signal()ing it and by setting the FLAGS_BIT_RX bit
;     in the flags field.
;
_interrupt:
    btst    #FLAGS_BIT_RX,s_Flags(a1)  
    bne.s   setack          

    move.b  StatusReg,d0            ; make sure HS_LINE is set
    btst    #HS_LINE,d0
    beq.s   skipint

    bset    #FLAGS_BIT_RX,s_Flags(a1)
    move.l  s_IntSigMask(a1),d0
    movea.l s_SysBase(a1),a6       ; Signal task
    movea.l s_ServerTask(a1),a1
    JSRLIB  Signal
skipint: 
    moveq #0,d0                     ; set Z flag => next server
    rts
setack:                             ; already in RX mode...
    bset  #FLAGS_BIT_ACK,s_Flags(a1)
    moveq #0,d0                     ; set Z flag => next server
    rts
    
;----------------------------------------------------------------------------
;
; NAME
;     hwsend() - low level send routine
;
; SYNOPSIS
;     void hwsend(struct packet_s *, struct state_s *)
;                  A0                A1
;
; FUNCTION
;     send a packet
;
_hwsend:
     movem.l  d2-d7/a2-a6,-(sp)
     move.l   a0,a4                               ; a4 = packet_s
     move.l   a1,a2                               ; a2 = state_s
     moveq    #FALSE,d2                           ; d2 = return value
     moveq    #0,d3
     move.l   d3,d4
     moveq    #HS_REQUEST,d3                      ; d3 = HS_REQUEST
     moveq    #HS_LINE,d4                         ; d4 = HS_LINE

     ;
     ; CRC wanted ?
     ;
     tst.b    s_UseCRC(a2)
     beq.s    hww_NoCRC
     ; yes
	 move.w   #SYNCWORD_CRC,p_Sync(a4)
     lea      packet_SIZE(a4),a0
     move.w   p_Size(a4),d0
     jsr      _CRC16(pc)
     move.w   d0,p_CRC(a4)
     bra.s    hww_Start
     
hww_NoCRC:
     move.w   #SYNCWORD_NOCRC,p_Sync(a4)
     
hww_Start:  
     move.l   _cia_base,a6
     moveq    #CIAICRF_FLG,d0
     JSRLIB   AbleICR                             ; DISABLEINT

     SETSELECT
     SETCIAOUTPUT

     lea      StatusReg,a5
     move.b   (a5),d7                             ; SAMPLEINPUT, d7 = State
     move.l   a4,a3                               ; a4 = packet_s
     move.w   p_Size(a4),d6
     addq.w   #packet_SIZE-2,d6
     move.b   (a3)+,OutReg                        ; WRITECIA *p++
hww_LoopShake1:
     move.b   (a5),d0                             ; StatusReg
     eor.b    d7,d0
     btst     d4,d0                               ; WAITINPUTTOGGLE
     bne.s    hww_cont1
     tst.b    s_TimeOut(a2)                       ; check time out
     beq.s    hww_LoopShake1
     bra.s    hww_TimedOut
hww_cont1:
     eor.b    d0,d7

hww_MainLoop:
     move.b   (a3)+,OutReg                        ; WRITECIA *p++
     bchg     d3,(a5)                             ; OUTPUTTOGGLE
hww_LoopShake2:
     move.b   (a5),d0                             ; StatusReg
     eor.b    d7,d0
     btst     d4,d0                               ; WAITINPUTTOGGLE
     bne.s    hww_cont2
     tst.b    s_TimeOut(a2)                       ; check time out
     beq.s    hww_LoopShake2
     bra.s    hww_TimedOut
hww_cont2:
     eor.b    d0,d7
     dbra     d6,hww_MainLoop
     
     moveq    #TRUE,d2                            ; rc = TRUE
hww_TimedOut:
    
     SETCIAINPUT
     bclr     d3,(a5)                             ; CLEARREQUEST StatusReg
     
     moveq    #CIAICRF_FLG,d0
     JSRLIB   SetICR                              ; CLEARINT
     move.w   #CIAICRF_FLG|CIAICRF_SETCLR,d0
     JSRLIB   AbleICR                             ; ENABLEINT
     
     CLRSELECT

     move.l   d2,d0                               ; return rc
     movem.l  (sp)+,d2-d7/a2-a6
     rts

;----------------------------------------------------------------------------
;
; NAME
;     hwrecv() - low level receive routine
;
; SYNOPSIS
;     void hwrecv(struct packet_s *, struct state_s *)
;                 A0                 A1
;
; FUNCTION
;     receive a packet
;
_hwrecv:
     movem.l  d2-d7/a2-a6,-(sp)
     move.l   a0,a4                               ; a4 = packet
     move.l   a1,a2                               ; a2 = state
     moveq    #FALSE,d5                           ; d5 = return value
     move.l   _cia_base,a6                        ; a6 = CIABase
     moveq    #0,d3
     move.l   d3,d2
     moveq    #HS_REQUEST,d3                      ; d3 = HS_REQUEST
     moveq    #HS_LINE,d2                         ; d2 = HS_LINE
     lea      StatusReg,a5                        ; a5 = StatusReg

     moveq    #CIAICRF_FLG,d0
     JSRLIB   AbleICR                             ; DISABLEINT
     
     SETSELECT                                    ; set SEL to mark entering recv loop

     move.b   (a5),d7                             ; SAMPLEINPUT
     cmp.b    #SYNCBYTE_HEAD,OutReg               ; READCIABYTE
     bne      hwr_TimedOut

     bchg     d3,(a5)                             ; OUTPUTTOGGLE
hwr_LoopShake1:
     move.b   (a5),d0                             ; StatusReg
     eor.b    d7,d0
     btst     d2,d0                               ; WAITINPUTTOGGLE
     bne.s    hwr_cont1
     tst.b    s_TimeOut(a2)                       ; check time out
     beq.s    hwr_LoopShake1
     bra      hwr_TimedOut
hwr_cont1:
     eor.b    d0,d7
     move.b   OutReg,d4                           ; READCIABYTE
     bchg     d3,(a5)                             ; OUTPUTTOGGLE
     move.b   d4,d0
     subq.b   #SYNCBYTE_CRC,d0
     bcs.s    hwr_TimedOut
     subq.b   #SYNCBYTE_NOCRC,d0
     bcc.s    hwr_TimedOut
     
     lea      p_Size(a4),a3

     ; Read 1st length byte
     ;
hwr_LoopShake2:
     move.b   (a5),d0                             ; StatusReg
     eor.b    d7,d0
     btst     d2,d0                               ; WAITINPUTTOGGLE
     bne.s    hwr_cont2
     tst.b    s_TimeOut(a2)                       ; check time out
     beq.s    hwr_LoopShake2
     bra.s    hwr_TimedOut
hwr_cont2:
     eor.b    d0,d7
     move.b   OutReg,(a3)+                        ; READCIABYTE
     bchg     d3,(a5)                             ; OUTPUTTOGGLE

     ; Read 2nd length byte
     ;
hwr_LoopShake3:
     move.b   (a5),d0                             ; StatusReg
     eor.b    d7,d0
     btst     d2,d0                               ; WAITINPUTTOGGLE
     bne.s    hwr_cont3
     tst.b    s_TimeOut(a2)                       ; check time out
     beq.s    hwr_LoopShake3
     bra.s    hwr_TimedOut
hwr_cont3:
     eor.b    d0,d7
     move.b   OutReg,(a3)+                        ; READCIABYTE
     bchg     d3,(a5)                             ; OUTPUTTOGGLE StatusReg

     move.w   -2(a3),d6                           ; = length
     
     ; check incoming size
     cmp.w     s_MaxPacketSize(a2),d6
     bhi.s     hwr_TimedOut
     
     ; we need to read CRC WORD extra
     ; (-1 byte due to dbra loop)
     addq.w    #1,d6

     ; Read main packet body
hwr_MainLoop:
hwr_LoopShake4:
     move.b   (a5),d0                             ; StatusReg
     eor.b    d7,d0
     btst     d2,d0
     bne.s    hwr_cont4
     tst.b    s_TimeOut(a2)                       ; a2 = *timeOut
     beq.s    hwr_LoopShake4
     bra.s    hwr_TimedOut
hwr_cont4:
     eor.b    d0,d7
     move.b   OutReg,(a3)+                        ; READCIABYTE
     bchg     d3,(a5)                             ; OUTPUTTOGGLE StatusReg
     dbra     d6,hwr_MainLoop

hwr_DoneRead:
     subq.b   #SYNCBYTE_CRC,d4
     bne.s    hwr_ReadOkay

     ; do CRC check
     lea      packet_SIZE(a4),a0
     move.w   p_Size(a4),d0
     ;subq.w   #PKTFRAMESIZE_2,d0
     jsr      _CRC16(pc)
     cmp.w    p_CRC(a4),d0
     bne.s    hwr_TimedOut

hwr_ReadOkay:
     moveq    #TRUE,d5
hwr_TimedOut:
     bclr     #FLAGS_BIT_RX,s_Flags(a2)
     
     SETCIAINPUT
     
     bclr     d3,(a5)                             ; CLEARREQUEST StatusReg
     moveq    #CIAICRF_FLG,d0
     JSRLIB   SetICR                              ; CLEARINT
     move.w   #CIAICRF_FLG|CIAICRF_SETCLR,d0
     JSRLIB   AbleICR                             ; ENABLEINT
     
     ; make sure POUT is zero (in odd-sized transfers)
     bclr     #CIAB_PRTRPOUT,StatusReg

     CLRSELECT

     move.l   d5,d0                               ; return value
     movem.l  (sp)+,d2-d7/a2-a6
     rts
    
    end

    