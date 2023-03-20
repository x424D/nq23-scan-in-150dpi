from elftools.elf.elffile import ELFFile
import random

f = open('build/ppc/closed_psr_encode.o', 'rb+')
elf = ELFFile(f)

sym_sec = elf.get_section_by_name('.symtab')
sym_info = sym_sec.get_symbol_by_name('get_hashtext')[0].entry

she_sec = elf.get_section_by_name('.shell')
offset_file = she_sec['sh_offset']

f.seek(offset_file)
func_code_bytes = f.read(sym_info['st_size'])
func_code = [a for a in func_code_bytes]
# for a in range(0, len(func_code), 4):
#     print(a)
# func_encoded = [a ^ 0x55 for a in func_code]

open_sum = sum(func_code)

print('open size:', len(func_code), 'sum:', open_sum)

random.seed(0x333)
func_encoded = list()

for a in range(0, len(func_code), 4):
    encoder = random.randrange(0x33, 0xFF)
    func_encoded.append(encoder)
    for bi in range(4):
        byte_encoded = func_code[a + bi] ^ encoder
        func_encoded.append(byte_encoded)

print('closed size:', len(func_encoded), 'sum:',
      sum(func_encoded[:len(func_code)]))

f.seek(offset_file)
f.write(bytes(func_encoded))
f.close()

f_linker_consts = open('src/psr/ppc-psr-compiletime-const.ld', 'w+')
s_linker_consts = 'SIZE_get_hashtext = ' + str(len(func_code)) + ';\n'
s_linker_consts += 'CHECKSUM_open_get_hashtext = ' + str(open_sum) + ';\n'
f_linker_consts.write(s_linker_consts)
