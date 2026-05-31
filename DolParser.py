# Parse the DOL header and set up memory sections for Sonic Unleashed Wii
# @author UnwiishedRecomp
# @category Analysis

import struct

def readU32(mem, addr):
    b = [mem.getByte(toAddr(addr + i)) & 0xFF for i in range(4)]
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]

mem = currentProgram.getMemory()

print("=== DOL Header Parser ===")

# Text sections (code) - offsets dans le fichier
for i in range(7):
    offset = readU32(mem, i * 4)
    address = readU32(mem, 0x48 + i * 4)
    size = readU32(mem, 0x90 + i * 4)
    if size > 0:
        print("Text Section {}: offset=0x{:08X} addr=0x{:08X} size=0x{:08X}".format(i, offset, address, size))

# Data sections
for i in range(11):
    offset = readU32(mem, 0x1C + i * 4)
    address = readU32(mem, 0x64 + i * 4)
    size = readU32(mem, 0xAC + i * 4)
    if size > 0:
        print("Data Section {}: offset=0x{:08X} addr=0x{:08X} size=0x{:08X}".format(i, offset, address, size))

# Entry point
entry = readU32(mem, 0xE0)
print("Entry Point: 0x{:08X}".format(entry))