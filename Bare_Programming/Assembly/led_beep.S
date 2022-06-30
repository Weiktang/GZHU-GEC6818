#define GPIOE_ALTFN0    0xC001E020
#define GPIOE_OUTENB    0xC001E004
#define GPIOE_OUT       0xC001E000

#define GPIOC_ALTFN0    0xC001C020
#define GPIOC_ALTFN1    0xC001C024
#define GPIOC_OUTENB    0xC001C004
#define GPIOC_OUT       0xC001C000

.global  _start

_start:
// LED ALT_Function set
ldr r0, =GPIOE_ALTFN0
ldr r1, [r0]
mov r2, #(3<<26)
bic r1, r1, r2
str r1, [r0]

ldr r0, =GPIOC_ALTFN1
ldr r1, [r0]
mov r2, #(1<<1)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOC_ALTFN0
ldr r1, [r0]
mov r2, #(1<<16)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOC_ALTFN0
ldr r1, [r0]
mov r2, #(1<<14)
orr r1, r1, r2
str r1, [r0]

// LED Out enable set
ldr r0, =GPIOE_OUTENB
ldr r1, [r0]
mov r2, #(1<<13)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOC_OUTENB
ldr r1, [r0]
mov r2, #(1<<17)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOC_OUTENB
ldr r1, [r0]
mov r2, #(1<<8)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOC_OUTENB
ldr r1, [r0]
mov r2, #(1<<7)
orr r1, r1, r2
str r1, [r0]

// LED close set
ldr r0, =GPIOE_OUT
ldr r1, [r0]
mov r2, #(1<<13)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOE_OUT
ldr r1, [r0]
mov r2, #(1<<17)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOE_OUT
ldr r1, [r0]
mov r2, #(1<<8)
orr r1, r1, r2
str r1, [r0]

ldr r0, =GPIOE_OUT
ldr r1, [r0]
mov r2, #(1<<7)
orr r1, r1, r2
str r1, [r0]

// Beep ALT_Function set
ldr r0, =GPIOC_ALTFN0
ldr r1, [r0]
mov r2, #(1<<28)
orr r1, r1, r2
str r1, [r0]

// Beep Out enable set
ldr r0, =GPIOC_OUTENB
ldr r1, [r0]
mov r2, #(1<<14)
orr r1, r1, r2
str r1, [r0]

// Beep close set
ldr r0, =GPIOC_OUT
ldr r1, [r0]
mov r2, #(1<<14)
bic r1, r1, r2

// main loop
loop:
    // Beep
    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    orr r1, #(1<<14)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    bic r1, #(1<<14)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    // LED
    ldr r0, =GPIOE_OUT
    ldr r1, [r0]
    bic r1, #(1<<13)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOE_OUT
    ldr r1, [r0]
    orr r1, #(1<<13)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    bic r1, #(1<<17)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    orr r1, #(1<<17)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    bic r1, #(1<<8)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    orr r1, #(1<<8)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    bic r1, #(1<<7)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    ldr r0, =GPIOC_OUT
    ldr r1, [r0]
    orr r1, #(1<<7)
    str r1, [r0]
    ldr r0, =0x3000000 
    bl delay

    b loop

delay:
    sub r0, #1
    cmp r0, #0
    bne delay
    mov pc,lr

.end
