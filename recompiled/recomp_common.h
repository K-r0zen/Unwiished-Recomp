#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>

#define MEM_SIZE 0x1800000

#define MEM_READ8(addr)  (mem[(addr) & (MEM_SIZE - 1)])
#define MEM_READ16(addr) ((uint16_t)(mem[((addr)) & (MEM_SIZE-1)] << 8 | mem[((addr)+1) & (MEM_SIZE-1)]))
#define MEM_READ32(addr) ((uint32_t)(mem[((addr)) & (MEM_SIZE-1)] << 24 | mem[((addr)+1) & (MEM_SIZE-1)] << 16 | mem[((addr)+2) & (MEM_SIZE-1)] << 8 | mem[((addr)+3) & (MEM_SIZE-1)]))

#define MEM_WRITE8(addr, val)  (mem[(addr) & (MEM_SIZE-1)] = (uint8_t)(val))
#define MEM_WRITE16(addr, val) do { mem[((addr)) & (MEM_SIZE-1)] = (uint8_t)((val)>>8); mem[((addr)+1) & (MEM_SIZE-1)] = (uint8_t)(val); } while(0)
#define MEM_WRITE32(addr, val) do { mem[((addr)) & (MEM_SIZE-1)] = (uint8_t)((val)>>24); mem[((addr)+1) & (MEM_SIZE-1)] = (uint8_t)((val)>>16); mem[((addr)+2) & (MEM_SIZE-1)] = (uint8_t)((val)>>8); mem[((addr)+3) & (MEM_SIZE-1)] = (uint8_t)(val); } while(0)
#define MEM_WRITEF64(addr, val) do { uint64_t _v; memcpy(&_v, &(val), 8); MEM_WRITE32(addr, (uint32_t)(_v>>32)); MEM_WRITE32((addr)+4, (uint32_t)_v); } while(0)
#define MEM_READF64(addr) ([&]{ uint64_t _v = ((uint64_t)MEM_READ32(addr)<<32)|MEM_READ32((addr)+4); double _d; memcpy(&_d,&_v,8); return _d; }())
#define MEM_READF32(addr) ([&]{ uint32_t _v = MEM_READ32(addr); float _f; memcpy(&_f,&_v,4); return (double)_f; }())
#define MEM_WRITEF32(addr, val) do { float _f = (float)(val); uint32_t _v; memcpy(&_v,&_f,4); MEM_WRITE32(addr,_v); } while(0)

#define PSQ_STORE_PAIR(ps0, ps1, addr, gqr) do { MEM_WRITEF32(addr, *(ps0)); MEM_WRITEF32((addr)+4, *(ps1)); } while(0)
#define PSQ_LOAD_PAIR(ps0, ps1, addr, gqr) do { *(ps0) = MEM_READF32(addr); *(ps1) = MEM_READF32((addr)+4); } while(0)

inline uint32_t PPC_ROTL32(uint32_t val, uint32_t shift) {
    shift &= 31;
    if (shift == 0) return val;
    return (val << shift) | (val >> (32 - shift));
}

inline uint32_t PPC_CNTLZW(uint32_t val) {
    if (val == 0) return 32;
    return __builtin_clz(val);
}

using Memory = uint8_t;

struct PPCContext {
    uint32_t r[32];
    double   f[32];
    double   ps[32][2];
    uint32_t gqr[8];
    uint32_t fpscr;
    uint32_t lr;
    uint32_t ctr;
    uint32_t xer;
    uint32_t cr;

    bool xer_so() const { return (xer >> 31) & 1; }
    bool xer_ov() const { return (xer >> 30) & 1; }
    bool xer_ca() const { return (xer >> 29) & 1; }

    void set_xer_ca(bool val) {
        if (val) xer |= (1u << 29);
        else     xer &= ~(1u << 29);
    }

    void set_xer_ov(bool val) {
        if (val) xer |= (1u << 30);
        else     xer &= ~(1u << 30);
    }

    void set_xer_so(bool val) {
        if (val) xer |= (1u << 31);
        else     xer &= ~(1u << 31);
    }

    bool get_cr_bit(int bit) const { return (cr >> (31 - bit)) & 1; }

    void set_cr_bit(int bit, bool val) {
        if (val) cr |= (1u << (31 - bit));
        else     cr &= ~(1u << (31 - bit));
    }

    void set_cr_field(int field, uint32_t val) {
        int shift = (7 - field) * 4;
        cr = (cr & ~(0xF << shift)) | ((val & 0xF) << shift);
    }

    uint32_t get_cr_field(int field) const {
        return (cr >> ((7 - field) * 4)) & 0xF;
    }

    void update_cr0(int32_t val) {
        uint32_t c = 0;
        if (val < 0)      c = 8;
        else if (val > 0) c = 4;
        else              c = 2;
        if (xer_so()) c |= 1;
        set_cr_field(0, c);
    }
};

using RecompFunc = void(*)(PPCContext*, Memory*);
extern RecompFunc func_table[];

struct FuncTable {
    void register_func(uint32_t addr, RecompFunc func) {
        uint32_t idx = (addr - 0x80004000) / 4;
        func_table[idx] = func;
    }
};

inline void CALL_INDIRECT(uint32_t addr, PPCContext* ctx, Memory* mem) {
    uint32_t idx = (addr - 0x80004000) / 4;
    if (func_table[idx]) func_table[idx](ctx, mem);
}