/* 
    Emulation of NES (6502) CPU
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "nescpu.h"


 // Define to use NES 6502 variant
#define NES_CPU
// Define to support undocumented instructions
#define USE_UNDOCUMENTED_INSTRUCTIONS

// STATUS flags
#define FLAG_C  0x01    // carry
#define FLAG_Z  0x02    // zero
#define FLAG_I  0x04    // interrupt enable
#define FLAG_D  0x08    // decimal
#define FLAG_B  0x10    // break
#define FLAG_U  0x20    // unused
#define FLAG_V  0x40    // overflow
#define FLAG_N  0x80    // negative

// STATUS modifier macros
#define SET_C(c) do { c->STATUS |= FLAG_C; } while (0)
#define CLR_C(c) do { c->STATUS &= ~FLAG_C; } while (0)
#define SET_Z(c) do { c->STATUS |= FLAG_Z; } while (0)
#define CLR_Z(c) do { c->STATUS &= ~FLAG_Z; } while (0)
#define SET_I(c) do { c->STATUS |= FLAG_I; } while (0)
#define CLR_I(c) do { c->STATUS &= ~FLAG_I; } while (0)
#define SET_D(c) do { c->STATUS |= FLAG_D; } while (0)
#define CLR_D(c) do { c->STATUS &= ~FLAG_D; } while (0)
#define SET_B(c) do { c->STATUS |= FLAG_B; } while (0)
#define CLR_B(c) do { c->STATUS &= ~FLAG_B; } while (0)
#define SET_U(c) do { c->STATUS |= FLAG_U; } while (0)
#define CLR_U(c) do { c->STATUS &= ~FLAG_U; } while (0)
#define SET_V(c) do { c->STATUS |= FLAG_V; } while (0)
#define CLR_V(c) do { c->STATUS &= ~FLAG_V; } while (0)
#define SET_N(c) do { c->STATUS |= FLAG_N; } while (0)
#define CLR_N(c) do { c->STATUS &= ~FLAG_N; } while (0)

// STATUS calculation macros
#define CALC_Z(c, n)  \
{                     \
    if ((n) & 0x00FF) \
        CLR_Z(c);     \
    else              \
        SET_Z(c);     \
}

#define CALC_N(c, n)  \
{                     \
    if ((n) & 0x0080) \
        SET_N(c);     \
    else              \
        CLR_N(c);     \
}

#define CALC_C(c, n)  \
{                     \
    if ((n) & 0xFF00) \
        SET_C(c);     \
    else              \
        CLR_C(c);     \
}

// n = result, m = accumulator, o = memory
#define CALC_V(c, n, m, o)                              \
{                                                       \
    if (((n) ^ (uint16_t)(m)) & ((n) ^ (o)) & 0x0080)   \
        SET_V(c);                                       \
    else                                                \
        CLR_V(c);                                       \
}


#define BASE_STACK     0x100


// Forward declaraction of helpers
static void push16(nescpu_ctx_t * c, uint16_t val);
static void push8(nescpu_ctx_t * c, uint8_t val);
static uint16_t pop16(nescpu_ctx_t * c);
static uint8_t pop8(nescpu_ctx_t * c);
static uint16_t load_operand(nescpu_ctx_t * c);
static void store_value(nescpu_ctx_t * c, uint16_t val);


//
// CPU Instruction and Operations
//

// addressing mode functions, calculates effective addresses
static void imp(nescpu_ctx_t* c)   // implied
{
}


static void acc(nescpu_ctx_t* c)   // accumulator
{
}


static void imm(nescpu_ctx_t* c)   // immediate
{
    c->abs_addr = c->PC;
    ++(c->PC);
}


static void zp(nescpu_ctx_t* c)    // zero-page
{
    c->abs_addr = (uint16_t)nesbus_read(c->bus, c->PC);
    c->abs_addr &= 0x00FF;
    ++(c->PC);
}


static void zpx(nescpu_ctx_t* c)   // zero-page,X
{
    c->abs_addr = ((uint16_t)nesbus_read(c->bus, c->PC) + c->X) & 0x00FF;
    ++(c->PC);
}


static void zpy(nescpu_ctx_t* c)   // zero-page,Y
{
    c->abs_addr = ((uint16_t)nesbus_read(c->bus, c->PC) + c->Y) & 0x00FF;
    ++(c->PC);
}


static void rel(nescpu_ctx_t* c)   // relative for branch ops (8-bit immediate value (-128 to +127))
{
    c->rel_addr = (uint16_t)nesbus_read(c->bus, c->PC);
    ++(c->PC);
    if (c->rel_addr & 0x80) 
        c->rel_addr |= 0xFF00;
}


static void abso(nescpu_ctx_t* c)  // absolute
{
    c->abs_addr = (uint16_t)nesbus_read(c->bus, c->PC) | ((uint16_t)nesbus_read(c->bus, c->PC + 1) << 8);
    c->PC += 2;
}


static void absx(nescpu_ctx_t* c)  // absolute,X
{
    uint16_t page;
    c->abs_addr = ((uint16_t)nesbus_read(c->bus, c->PC) | ((uint16_t)nesbus_read(c->bus, c->PC + 1) << 8));
    page = c->abs_addr & 0xFF00;
    c->abs_addr += (uint16_t)(c->X);
    if (page != (c->abs_addr & 0xFF00)) // one cycle penlty for page crossing
    {
        c->add_cycle_addr = true;
    }
    c->PC += 2;
}


static void absy(nescpu_ctx_t* c)  // absolute,Y
{
    uint16_t page;
    c->abs_addr = ((uint16_t)nesbus_read(c->bus, c->PC) | ((uint16_t)nesbus_read(c->bus, c->PC + 1) << 8));
    page = c->abs_addr & 0xFF00;
    c->abs_addr += (uint16_t)(c->Y);
    if (page != (c->abs_addr & 0xFF00)) // one cycle penlty for page crossing
    {
        c->add_cycle_addr = true;
    }
    c->PC += 2;
}


static void ind(nescpu_ctx_t* c)   // indirect
{
    uint16_t addr, addr2;
    addr = (uint16_t)nesbus_read(c->bus, c->PC) | ((uint16_t)nesbus_read(c->bus, c->PC + 1) << 8);
    // replicate 6502 page-boundary wraparound bug, if addr + 1 overfolows the page, 
    // the high byte wraps back to the same page
    addr2 = (addr & 0xFF00) | ((addr + 1) & 0x00FF);
    c->abs_addr = (uint16_t)nesbus_read(c->bus, addr) | ((uint16_t)nesbus_read(c->bus, addr2) << 8);
    c->PC += 2;
}


static void indx(nescpu_ctx_t* c)   // (indirect,X)
{
    // The supplied 8-bit address is offset by X Register to index
    // a location in page 0x00. The actual 16-bit address is read 
    // from this location
    uint16_t addr;
    addr = ((uint16_t)nesbus_read(c->bus, c->PC) + (uint16_t)(c->X)) & 0x00FF; // zero-page wraparound
    ++(c->PC);
    c->abs_addr = (uint16_t)nesbus_read(c->bus, addr & 0x00FF) | ((uint16_t)nesbus_read(c->bus, (addr + 1) & 0x00FF) << 8);
}


static void indy(nescpu_ctx_t* c)  // (indirect),Y
{
    // The supplied 8-bit address indexes a location in page 0x00. From 
    // here the actual 16-bit address is read, and the contents of
    // Y Register is added to it to offset it. If the offset causes a
    // change in page then an additional clock cycle is required.
    uint16_t addr, addr2, page;
    addr = (uint16_t)nesbus_read(c->bus, c->PC);
    ++(c->PC);
    // Same bug as indirect
    addr2 = (addr & 0xFF00) | ((addr + 1) & 0x00FF);
    c->abs_addr = (uint16_t)nesbus_read(c->bus, addr) | ((uint16_t)nesbus_read(c->bus, addr2) << 8);
    page = c->abs_addr & 0xFF00;
    c->abs_addr += (uint16_t)(c->Y);
    if (page != (c->abs_addr & 0xFF00))    // one cycle penlty for page crossing
    {
        c->add_cycle_addr = true;
    }
}



// Implement instructions

// Instruction: Add with Carry In
// Function:    A = A + M + C
// Flags Out:   C, V, N, Z
static void ADC(nescpu_ctx_t* c)
{
    uint16_t value, temp;
    c->add_cycle_op = true;   // This instruction may add clock cycle
    value = load_operand(c);
    temp = (uint16_t)(c->A) + value + (uint16_t)(c->STATUS & FLAG_C);
    CALC_C(c, temp);
    CALC_Z(c, temp);
    CALC_V(c, temp, c->A, value);
    CALC_N(c, temp);
#ifndef NES_CPU
    if (c->STATUS & FLAG_D)
    {
        CLR_C(c);
        if ((c->A & 0x0F) > 0x09)
        {
            c->A += 0x06;
        }
        if ((c->A & 0xF0) > 0x90)
        {
            c->A += 0x60;
            SET_C(c);
        }
        ++(c->cycles);
    }
#endif
    c->A = (uint8_t)(temp & 0x00FF);
}


// Instruction: Bitwise Logic AND
// Function:    A = A & M
// Flags Out:   N, Z
static void AND(nescpu_ctx_t* c)
{
    uint16_t value;
    c->add_cycle_op = true;   // This instruction may add clock cycle
    value = load_operand(c);
    value = (uint16_t)(c->A) & value;
    CALC_Z(c, value);
    CALC_N(c, value);
    c->A = (uint8_t)(value & 0x00FF);
}


// Instruction: Arithmetic Shift Left
// Function:    A = C <- (A << 1) <- 0
// Flags Out:   N, Z, C
static void ASL(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    value = value << 1;
    CALC_C(c, value);
    CALC_Z(c, value);
    CALC_N(c, value);
    store_value(c, value);
}


// Instruction: Branch if Carry Clear
// Function:    if(C == 0) PC = address 
static void BCC(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_C) == 0)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: Branch if Carry Set
// Function:    if(C == 1) PC = address
static void BCS(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_C) == FLAG_C)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: Branch if Carry Set
// Function:    if(C == 1) PC = address
static void BEQ(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_Z) == FLAG_Z)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: BIT Test bits in memory with accumulator
// Function:    A & M, M bit7 -> N, M bit6 -> V
// Flags Out:   N, V, Z
static void BIT(nescpu_ctx_t* c)
{
    uint16_t value, temp;
    value = load_operand(c);
    temp = (uint16_t)(c->A) & value;
    CALC_Z(c, temp);
    c->STATUS = (c->STATUS & 0x3F) | (uint8_t)(value & 0xC0);
}


// Instruction: Branch if Negative
// Function:    if(N == 1) PC = address
static void BMI(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_N) == FLAG_N)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: Branch if Not Equal
// Function:    if(Z == 0) PC = address
static void BNE(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_Z) == 0)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: Branch if Positive
// Function:    if(N == 0) PC = address
static void BPL(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_N) == 0)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: Break
// Function:    Program Sourced Interrupt
static void BRK(nescpu_ctx_t* c)
{
    c->PC += 1;
    push16(c, c->PC); // push next instruction address onto stack
    push8(c, c->STATUS | FLAG_B); // push CPU status OR'd with break flag to stack
    SET_I(c);    //set interrupt flag
    c->PC = (uint16_t)(nesbus_read(c->bus, 0xFFFE)) | ((uint16_t)(nesbus_read(c->bus, 0xFFFF)) << 8);
}


// Instruction: Branch if Overflow Clear
// Function:    if(V == 0) PC = address
static void BVC(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_V) == 0)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: Branch if Overflow Set
// Function:    if(V == 1) PC = address
static void BVS(nescpu_ctx_t* c)
{
    uint16_t temp;
    if ((c->STATUS & FLAG_V) == FLAG_V)
    {
        ++(c->cycles);      // branch takes one cycle
        temp = c->PC + c->rel_addr;
        if ((temp & 0xFF00) != (c->PC & 0xFF00))
            ++(c->cycles);  // additional cycle if cross page
        c->PC = temp;
    }
}


// Instruction: Clear Carry Flag
// Function:    C = 0
static void CLC(nescpu_ctx_t* c)
{
    CLR_C(c);
}


// Instruction: Clear Decimal Flag
// Function:    D = 0
static void CLD(nescpu_ctx_t* c)
{
    CLR_D(c);
}


// Instruction: Disable Interrupts / Clear Interrupt Flag
// Function:    I = 0
static void CLI(nescpu_ctx_t* c)
{
    CLR_I(c);
}


// Instruction: Clear Overflow Flag
// Function:    V = 0
static void CLV(nescpu_ctx_t* c)
{
    CLR_V(c);
}


// Instruction: Compare Accumulator
// Function:    C <- (A >= M)  Z <- (A - M) == 0
// Flags Out:   N, C, Z
static void CMP(nescpu_ctx_t* c)
{
    uint16_t value;
    c->add_cycle_op = true;
    value = load_operand(c);
    if (c->A >= (uint8_t)(value & 0x00FF))
        SET_C(c);
    else
        CLR_C(c);
    if (c->A == (uint8_t)(value & 0x00FF))
        SET_Z(c);
    else
        CLR_Z(c);
    value = (uint16_t)(c->A) - value;
    CALC_N(c, value);
}


// Instruction: Compare X Register
// Function:    C <- X >= M      Z <- (X - M) == 0
// Flags Out:   N, C, Z
static void CPX(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    if (c->X >= (uint8_t)(value & 0x00FF))
        SET_C(c);
    else
        CLR_C(c);
    if (c->X == (uint8_t)(value & 0x00FF))
        SET_Z(c);
    else
        CLR_Z(c);
    value = (uint16_t)(c->X) - value;
    CALC_N(c, value);
}


// Instruction: Compare Y Register
// Function:    C <- Y >= M      Z <- (Y - M) == 0
// Flags Out:   N, C, Z
static void CPY(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    if (c->Y >= (uint8_t)(value & 0x00FF))
        SET_C(c);
    else
        CLR_C(c);
    if (c->Y == (uint8_t)(value & 0x00FF))
        SET_Z(c);
    else
        CLR_Z(c);
    value = (uint16_t)(c->Y) - value;
    CALC_N(c, value);
}


// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
static void DEC(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c) - 1;
    CALC_Z(c, value);
    CALC_N(c, value);
    store_value(c, value);
}


// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
static void DEX(nescpu_ctx_t* c)
{
    --(c->X);
    CALC_Z(c, c->X);
    CALC_N(c, c->X);
}


// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
static void DEY(nescpu_ctx_t* c)
{
    --(c->Y);
    CALC_Z(c, c->Y);
    CALC_N(c, c->Y);
}


// Instruction: Bitwise Logic XOR
// Function:    A = A xor M
// Flags Out:   N, Z
static void EOR(nescpu_ctx_t* c)
{
    uint16_t value;
    c->add_cycle_op = true;
    value = load_operand(c);
    value = (uint16_t)(c->A) ^ value;
    CALC_Z(c, value);
    CALC_N(c, value);
    c->A = (uint8_t)(value & 0x00FF);
}


// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
static void INC(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c) + 1;
    CALC_Z(c, value);
    CALC_N(c, value);
    store_value(c, value);
}


// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
static void INX(nescpu_ctx_t* c)
{
    ++(c->X);
    CALC_Z(c, c->X);
    CALC_N(c, c->X);
}


// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
static void INY(nescpu_ctx_t* c)
{
    ++(c->Y);
    CALC_Z(c, c->Y);
    CALC_N(c, c->Y);
}


// Instruction: JAM
// Function: Freeze CPU until reset
static void JAM(nescpu_ctx_t* c)
{
    c->jammed = true;
}


// Instruction: Jump To Location
// Function:    PC = address
static void JMP(nescpu_ctx_t* c)
{
    c->PC = c->abs_addr;
}


// Instruction: Jump To Sub-Routine
// Function:    Push current pc to stack, PC = address
static void JSR(nescpu_ctx_t* c)
{
    push16(c, c->PC - 1);
    c->PC = c->abs_addr;
}


// Instruction: Load The Accumulator
// Function:    A = M
// Flags Out:   N, Z
static void LDA(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    c->A = (uint8_t)(value & 0x00FF);
    CALC_Z(c, c->A);
    CALC_N(c, c->A);
    c->add_cycle_op = true;
}


// Instruction: Load The X Register
// Function:    X = M
// Flags Out:   N, Z
static void LDX(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    c->X = (uint8_t)(value & 0x00FF);
    CALC_Z(c, c->X);
    CALC_N(c, c->X);
    c->add_cycle_op = true;
}


// Instruction: Load The Y Register
// Function:    Y = M
// Flags Out:   N, Z
static void LDY(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    c->Y = (uint8_t)(value & 0x00FF);
    CALC_Z(c, c->Y);
    CALC_N(c, c->Y);
    c->add_cycle_op = true;
}


// Instruction: Right shift one bit at memory location or A
// Function:    M = M >> 1 | A = A >> 1, C <- bit0
// Flags Out:   C, N, Z
static void LSR(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    if (value & 1)
        SET_C(c);
    else
        CLR_C(c);
    value = value >> 1;
    CALC_Z(c, value);
    CALC_N(c, value);
    store_value(c, value);
}


// Instruction: No ops
static void NOP(nescpu_ctx_t* c)
{
    // Refer to https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
    switch (c->opcode)
    {
    case 0x1C:
    case 0x3C:
    case 0x5C:
    case 0x7C:
    case 0xDC:
    case 0xFC:
        c->add_cycle_op = true;
        break;
    }
}


// Instruction: Bitwise Logic OR
// Function:    A = A | M
// Flags Out:   N, Z
static void ORA(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    value = (uint16_t)(c->A) | value;
    CALC_Z(c, value);
    CALC_N(c, value);
    c->A = (uint8_t)(value & 0x00FF);
    c->add_cycle_op = true;
}


// Instruction: Push Accumulator to Stack
// Function:    A -> stack
static void PHA(nescpu_ctx_t* c)
{
    push8(c, c->A);
}


// Instruction: Push Status Register to Stack
// Function:    status -> stack
// Note:        Break flag is set to 1 before push
static void PHP(nescpu_ctx_t* c)
{
    push8(c, c->STATUS | FLAG_B);
}


// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
static void PLA(nescpu_ctx_t* c)
{
    c->A = pop8(c);
    CALC_Z(c, c->A);
    CALC_N(c, c->A);
}


// Instruction: Pop Status Register off Stack
// Function:    Status <- stack
// Flags Out:   N, Z
static void PLP(nescpu_ctx_t* c)
{
    c->STATUS = pop8(c);
    SET_U(c);
    CLR_B(c);    // B flag can be unset by hardware
}


// Instruction: ROL Rotate one bit left (memory or accumulator)
// Function:    M = M << 1 | A = A << 1, bit0 <- C <- bit7
// Flags Out:   N, Z, C
static void ROL(nescpu_ctx_t* c)
{
    uint16_t value;
    value = load_operand(c);
    value = (value << 1) | (c->STATUS & FLAG_C);
    CALC_C(c, value);
    CALC_Z(c, value);
    CALC_N(c, value);
    store_value(c, value);
}


// Instruction: ROL Rotate one bit right (memory or accumulator)
// Function:    M = M >> 1 | A = A >> 1, C -> bit7, bit0 -> C
// Flags Out:   N, Z, C
static void ROR(nescpu_ctx_t* c)
{
    uint16_t value, temp;
    value = load_operand(c);
    temp = (value >> 1) | ((c->STATUS & FLAG_C) << 7);
    if (value & 1)
        SET_C(c);
    else
        CLR_C(c);
    CALC_Z(c, temp);
    CALC_N(c, temp);
    store_value(c, temp);
}


// Instruction: Return from interrupt
// Function:    STATUS <- stack; PC <- stack
static void RTI(nescpu_ctx_t* c)
{
    c->STATUS = pop8(c);
    SET_U(c);
    CLR_B(c);    // B flag can be unset by hardware
    c->PC = pop16(c);
}


// Instruction: Return from subroutine
// Function:    PC <- stack; PC = PC + 1
static void RTS(nescpu_ctx_t* c)
{
    uint16_t value;
    value = pop16(c);
    c->PC = value + 1;
}


// Instruction: Subtract with borrow
// Function:    A = A - M - C
// Flags Out:   C, V, N, Z
static void SBC(nescpu_ctx_t* c)
{
    uint16_t value, temp;
    value = load_operand(c) ^ 0x00FF;
    temp = (uint16_t)(c->A) + value + (uint16_t)(c->STATUS & FLAG_C);
    CALC_C(c, temp);
    CALC_Z(c, temp);
    CALC_V(c, temp, c->A, value);
    CALC_N(c, temp);
    c->add_cycle_op = true;
#ifndef NES_CPU
    if (c->STATUS & FLAG_D)
    {
        CLR_C(c);
        c->A -= 0x66;
        if ((c->A & 0x0F) > 0x09)
        {
            c->A += 0x06;
        }
        if ((c->A & 0xF0) > 0x90)
        {
            c->A += 0x60;
            SET_C(c);
        }
        ++(c->cycles);
    }
#endif
    c->A = (uint8_t)(temp & 0x00FF);
}


// Instruction: Set Carry Flag
// Function:    C = 1
static void SEC(nescpu_ctx_t* c)
{
    SET_C(c);
}


// Instruction: Set Decimal Flag
// Function:    D = 1
static void SED(nescpu_ctx_t* c)
{
    SET_D(c);
}


// Instruction: Set Interrupt Flag / Enable Interrupts
// Function:    I = 1
static void SEI(nescpu_ctx_t* c)
{
    SET_I(c);
}


// Instruction: Store Accumulator at Address
// Function:    M = A
static void STA(nescpu_ctx_t* c)
{
    store_value(c, c->A);
}


// Instruction: Store X Register at Address
// Function:    M = X
static void STX(nescpu_ctx_t* c)
{
    store_value(c, c->X);
}


// Instruction: Store Y Register at Address
// Function:    M = Y
static void STY(nescpu_ctx_t* c)
{
    store_value(c, c->Y);
}


// Instruction: Transfer Accumulator to X Register
// Function:    X = A
// Flags Out:   N, Z
static void TAX(nescpu_ctx_t* c)
{
    c->X = c->A;
    CALC_Z(c, c->X);
    CALC_N(c, c->X);
}


// Instruction: Transfer Accumulator to Y Register
// Function:    Y = A
// Flags Out:   N, Z
static void TAY(nescpu_ctx_t* c)
{
    c->Y = c->A;
    CALC_Z(c, c->Y);
    CALC_N(c, c->Y);
}


// Instruction: Transfer Stack Pointer to X Register
// Function:    X = stack pointer
// Flags Out:   N, Z
static void TSX(nescpu_ctx_t* c)
{
    c->X = c->SP;
    CALC_Z(c, c->X);
    CALC_N(c, c->X);
}


// Instruction: Transfer X Register to Accumulator
// Function:    A = X
// Flags Out:   N, Z
static void TXA(nescpu_ctx_t* c)
{
    c->A = c->X;
    CALC_Z(c, c->A);
    CALC_N(c, c->A);
}


// Instruction: Transfer X Register to Stack Pointer
// Function:    stack pointer = X
static void TXS(nescpu_ctx_t* c)
{
    c->SP = c->X;
}


// Instruction: Transfer Y Register to Accumulator
// Function:    A = Y
// Flags Out:   N, Z
static void TYA(nescpu_ctx_t* c)
{
    c->A = c->Y;
    CALC_Z(c, c->A);
    CALC_N(c, c->A);
}


// undocumented instructions
#ifdef USE_UNDOCUMENTED_INSTRUCTIONS

static void LAX(nescpu_ctx_t* c)
{
    LDA(c);
    LDX(c);
}


static void SAX(nescpu_ctx_t* c)
{
    STA(c);
    STX(c);
    store_value(c, (c->A) & (c->X));
    if (c->add_cycle_op && c->add_cycle_addr)
        --(c->cycles);
}



static void DCP(nescpu_ctx_t* c)
{
    DEC(c);
    CMP(c);
    if (c->add_cycle_op && c->add_cycle_addr)
        --(c->cycles);
}


static void ISB(nescpu_ctx_t* c)
{
    INC(c);
    SBC(c);
    if (c->add_cycle_op && c->add_cycle_addr)
        --(c->cycles);
}


static void SLO(nescpu_ctx_t* c)
{
    ASL(c);
    ORA(c);
    if (c->add_cycle_op && c->add_cycle_addr)
        --(c->cycles);
}


static void RLA(nescpu_ctx_t* c)
{
    ROL(c);
    AND(c);
    if (c->add_cycle_op && c->add_cycle_addr)
        --(c->cycles);
}


static void SRE(nescpu_ctx_t* c)
{
    LSR(c);
    EOR(c);
    if (c->add_cycle_op && c->add_cycle_addr)
        --(c->cycles);
}


static void RRA(nescpu_ctx_t* c)
{
    ROR(c);
    ADC(c);
    if (c->add_cycle_op && c->add_cycle_addr)
        --(c->cycles);
}

#else

static void LAX(nescpu_ctx_t* c)
{
    NOP(c);
}


static void SAX(nescpu_ctx_t* c)
{
    NOP(c);
}


static void DCP(nescpu_ctx_t* c)
{
    NOP(c);
}


static void ISB(nescpu_ctx_t* c)
{
    NOP(c);
}


static void SLO(nescpu_ctx_t* c)
{
    NOP(c);
}


static void RLA(nescpu_ctx_t* c)
{
    NOP(c);
}


static void SRE(nescpu_ctx_t* c)
{
    NOP(c);
}


static void RRA(nescpu_ctx_t* c)
{
    NOP(c);
}


#endif  // USE_UNDOCUMENTED_INSTRUCTIONS


/* opcode -> address mode */
typedef void (*addr_mode_fp)(nescpu_ctx_t* c);
static const addr_mode_fp addrtable[256] =
{
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
/* 0 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm, abso, abso, abso, abso, /* 0 */
/* 1 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 1 */
/* 2 */    abso, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm, abso, abso, abso, abso, /* 2 */
/* 3 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 3 */
/* 4 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm, abso, abso, abso, abso, /* 4 */
/* 5 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 5 */
/* 6 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm,  ind, abso, abso, abso, /* 6 */
/* 7 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 7 */
/* 8 */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* 8 */
/* 9 */     rel, indy,  imp, indy,  zpx,  zpx,  zpy,  zpy,  imp, absy,  imp, absy, absx, absx, absy, absy, /* 9 */
/* A */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* A */
/* B */     rel, indy,  imp, indy,  zpx,  zpx,  zpy,  zpy,  imp, absy,  imp, absy, absx, absx, absy, absy, /* B */
/* C */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* C */
/* D */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* D */
/* E */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* E */
/* F */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx  /* F */
};


/* opcode -> implementation */
typedef void (*op_impl_fp)(nescpu_ctx_t* c);
static const op_impl_fp optable[256] =
{
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |      */
/* 0 */      BRK,  ORA,  NOP,  SLO,  NOP,  ORA,  ASL,  SLO,  PHP,  ORA,  ASL,  NOP,  NOP,  ORA,  ASL,  SLO, /* 0 */
/* 1 */      BPL,  ORA,  NOP,  SLO,  NOP,  ORA,  ASL,  SLO,  CLC,  ORA,  NOP,  SLO,  NOP,  ORA,  ASL,  SLO, /* 1 */
/* 2 */      JSR,  AND,  NOP,  RLA,  BIT,  AND,  ROL,  RLA,  PLP,  AND,  ROL,  NOP,  BIT,  AND,  ROL,  RLA, /* 2 */
/* 3 */      BMI,  AND,  NOP,  RLA,  NOP,  AND,  ROL,  RLA,  SEC,  AND,  NOP,  RLA,  NOP,  AND,  ROL,  RLA, /* 3 */
/* 4 */      RTI,  EOR,  NOP,  SRE,  NOP,  EOR,  LSR,  SRE,  PHA,  EOR,  LSR,  NOP,  JMP,  EOR,  LSR,  SRE, /* 4 */
/* 5 */      BVC,  EOR,  NOP,  SRE,  NOP,  EOR,  LSR,  SRE,  CLI,  EOR,  NOP,  SRE,  NOP,  EOR,  LSR,  SRE, /* 5 */
/* 6 */      RTS,  ADC,  NOP,  RRA,  NOP,  ADC,  ROR,  RRA,  PLA,  ADC,  ROR,  NOP,  JMP,  ADC,  ROR,  RRA, /* 6 */
/* 7 */      BVS,  ADC,  NOP,  RRA,  NOP,  ADC,  ROR,  RRA,  SEI,  ADC,  NOP,  RRA,  NOP,  ADC,  ROR,  RRA, /* 7 */
/* 8 */      NOP,  STA,  NOP,  SAX,  STY,  STA,  STX,  SAX,  DEY,  NOP,  TXA,  NOP,  STY,  STA,  STX,  SAX, /* 8 */
/* 9 */      BCC,  STA,  NOP,  NOP,  STY,  STA,  STX,  SAX,  TYA,  STA,  TXS,  NOP,  NOP,  STA,  NOP,  NOP, /* 9 */
/* A */      LDY,  LDA,  LDX,  LAX,  LDY,  LDA,  LDX,  LAX,  TAY,  LDA,  TAX,  NOP,  LDY,  LDA,  LDX,  LAX, /* A */
/* B */      BCS,  LDA,  NOP,  LAX,  LDY,  LDA,  LDX,  LAX,  CLV,  LDA,  TSX,  LAX,  LDY,  LDA,  LDX,  LAX, /* B */
/* C */      CPY,  CMP,  NOP,  DCP,  CPY,  CMP,  DEC,  DCP,  INY,  CMP,  DEX,  NOP,  CPY,  CMP,  DEC,  DCP, /* C */
/* D */      BNE,  CMP,  NOP,  DCP,  NOP,  CMP,  DEC,  DCP,  CLD,  CMP,  NOP,  DCP,  NOP,  CMP,  DEC,  DCP, /* D */
/* E */      CPX,  SBC,  NOP,  ISB,  CPX,  SBC,  INC,  ISB,  INX,  SBC,  NOP,  SBC,  CPX,  SBC,  INC,  ISB, /* E */
/* F */      BEQ,  SBC,  JAM,  ISB,  NOP,  SBC,  INC,  ISB,  SED,  SBC,  NOP,  ISB,  NOP,  SBC,  INC,  ISB  /* F */
};

static const uint32_t cycletable[256] = {
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
/* 0 */      7,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    4,    4,    6,    6,  /* 0 */
/* 1 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 1 */
/* 2 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    4,    4,    6,    6,  /* 2 */
/* 3 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 3 */
/* 4 */      6,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    3,    4,    6,    6,  /* 4 */
/* 5 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 5 */
/* 6 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    5,    4,    6,    6,  /* 6 */
/* 7 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 7 */
/* 8 */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* 8 */
/* 9 */      2,    6,    2,    6,    4,    4,    4,    4,    2,    5,    2,    5,    5,    5,    5,    5,  /* 9 */
/* A */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* A */
/* B */      2,    5,    2,    5,    4,    4,    4,    4,    2,    4,    2,    4,    4,    4,    4,    4,  /* B */
/* C */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* C */
/* D */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* D */
/* E */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* E */
/* F */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7   /* F */
};



//
// Helpers
//

// push 16-bit onto stack
static void push16(nescpu_ctx_t* c, uint16_t val)
{
    nesbus_write(c->bus, BASE_STACK + c->SP, (val >> 8) & 0xFF);
    nesbus_write(c->bus, BASE_STACK + ((c->SP - 1) & 0xFF), val & 0xFF);
    c->SP -= 2;
}


// push 8-bit onto stack
static void push8(nescpu_ctx_t* c, uint8_t val)
{
    nesbus_write(c->bus, BASE_STACK + c->SP, val);
    --(c->SP);
}


// pop 16-bit from stack
static uint16_t pop16(nescpu_ctx_t* c)
{
    uint16_t t;
    t = nesbus_read(c->bus, BASE_STACK + ((c->SP + 1) & 0xFF)) | ((uint16_t)nesbus_read(c->bus, BASE_STACK + ((c->SP + 2) & 0xFF)) << 8);
    c->SP += 2;
    return t;
}


// pop 8-bit from stack
static uint8_t pop8(nescpu_ctx_t* c)
{
    uint8_t t;
    ++(c->SP);
    t = nesbus_read(c->bus, BASE_STACK + c->SP);
    return t;
}


// load operand from memory or A
static uint16_t load_operand(nescpu_ctx_t* c)
{
    if (addrtable[c->opcode] == acc)
        return((uint16_t)(c->A));
    else
        return((uint16_t)nesbus_read(c->bus, c->abs_addr));
}


// store value to target (memory or A if accumulator address mode)
static void store_value(nescpu_ctx_t* c, uint16_t val)
{
    if (addrtable[c->opcode] == acc)
        c->A = (uint8_t)(val & 0x00FF);
    else
        nesbus_write(c->bus, c->abs_addr, (uint8_t)(val & 0x00FF));
}


//
// Exported functions
//

nescpu_ctx_t* nescpu_create()
{
    nescpu_ctx_t* c = malloc(sizeof(nescpu_ctx_t));
    if (c != 0)
    {
        memset(c, 0, sizeof(nescpu_ctx_t));
    }
    return c;
}


void nescpu_destroy(nescpu_ctx_t* c)
{
    if (c) free(c);
}


void nescpu_attach_bus(nescpu_ctx_t* c, nesbus_ctx_t* bus)
{
    if (c != 0)
        c->bus = bus;
}


void nescpu_reset(nescpu_ctx_t* c, bool reset_to_fffc)
{
    if (0 == c)
        return;
    // Reset to Reset vector at 0xFFFC
    if (reset_to_fffc)
        c->PC = (uint16_t)nesbus_read(c->bus, 0xFFFC) | ((uint16_t)nesbus_read(c->bus, 0xFFFD) << 8);
    // All registers are 0 except the unknow bit in STATUS is 1
    c->A = 0;
    c->X = 0;
    c->Y = 0;
    c->SP = 0xFD;
    c->STATUS = 0;
    CLR_B(c);
    SET_U(c);
    SET_I(c);         // enable interrupt
    c->cycles = 7;    // reset take 7 cycles
    c->jammed = false;
}


void nescpu_irq(nescpu_ctx_t* c)
{
    if (0 == c)
        return;
    // abort if interrupts are inhibited
    if ((c->STATUS & FLAG_I) == FLAG_I)
        return;
    push16(c, c->PC);
    CLR_B(c);
    SET_U(c);
    SET_I(c);
    push8(c, c->STATUS);
    c->PC = (uint16_t)nesbus_read(c->bus, 0xFFFE) | ((uint16_t)nesbus_read(c->bus, 0xFFFF) << 8);
    c->cycles = 7;
}


void nescpu_nmi(nescpu_ctx_t* c)
{
    if (0 == c)
        return;
    // Similar to reset, but load PC address from 0xFFFA
    push16(c, c->PC);
    CLR_B(c);
    SET_U(c);
    SET_I(c);
    push8(c, c->STATUS);
    c->PC = (uint16_t)nesbus_read(c->bus, 0xFFFA) | ((uint16_t)nesbus_read(c->bus, 0xFFFB) << 8);
    c->cycles = 8;
}


bool nescpu_clock(nescpu_ctx_t* c)
{
    if (0 == c)
        return true;
    // only execute new instruction if previous instruction has finished
    if (c->cycles == 0)
    {
        if (!c->jammed)
        {
            c->opcode = nesbus_read(c->bus, c->PC);
            SET_U(c);    // Always set U flag
            ++(c->PC);
            c->cycles = cycletable[c->opcode];
            c->add_cycle_op = false;
            c->add_cycle_addr = false;
            (*addrtable[c->opcode])(c); // Set addresing mode
            (*optable[c->opcode])(c);    // Call instruction
            c->cycles += (c->add_cycle_op & c->add_cycle_addr) ? 1 : 0;
            SET_U(c);    // Always set U flag
        }
        else
        {
            // CPU is JAMed
            c->cycles = 1;  // will reduce to 0 below 
        }
    }
    --(c->cycles);
    return (c->jammed);
}


bool nescpu_is_completed(nescpu_ctx_t* c)
{
    if (0 == c)
        return true;
    return (0 == c->cycles);
}


bool nescpu_is_jammed(nescpu_ctx_t* c)
{
    if (0 == c)
        return true;
    return (c->jammed);
}


void nescpu_unjam(nescpu_ctx_t* c)
{
    if (0 == c)
        return;
    // This simply clear the JAM condition.
    // Caller can use JAM operation to indicate finish of a subroutine, then before 
    // executing another subroutine, set PC and unjam the CPU
    c->jammed = false;
}


void nescpu_skip_cycles(nescpu_ctx_t* c, int cycles)
{
    if (0 == c)
        return;
    c->cycles += cycles;
}


void nescpu_dump(nescpu_ctx_t* c)
{
    if (0 == c)
        return;
    printf("PC:%04X A:%02X X:%02X Y:%02X P:%02X SP:%02X", c->PC, c->A, c->X, c->Y, c->STATUS, c->SP);
}


void nescpu_set_pc(nescpu_ctx_t* c, uint16_t pc)
{
    if (0 == c)
        return;
    c->PC = pc;
}


void nescpu_set_a(nescpu_ctx_t* c, uint8_t a)
{
    if (0 == c)
        return;
    c->A = a;
}


void nescpu_set_x(nescpu_ctx_t* c, uint8_t x)
{
    if (0 == c)
        return;
    c->X = x;
}


void nescpu_set_y(nescpu_ctx_t* c, uint8_t y)
{
    if (0 == c)
        return;
    c->Y = y;
}


void nescpu_set_status(nescpu_ctx_t* c, uint8_t status)
{
    if (0 == c)
        return;
    c->STATUS = status;
}


void nescpu_set_sp(nescpu_ctx_t* c, uint8_t sp)
{
    if (0 == c)
        return;
    c->SP = sp;
}
