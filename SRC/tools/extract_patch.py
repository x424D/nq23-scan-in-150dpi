#!/usr/bin/python3
import sys

from elftools.elf.elffile import ELFFile


def extract_patch(filename) -> bytes:

    with open(filename, "rb") as f:
        elffile = ELFFile(f)

        for section in elffile.iter_sections():
            if section.name == '.patch':
                print(f'{filename}: .patch size: {section.data_size}')
                return section.data()

    raise Exception('cannot find section: .patch')


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise Exception('./* [elf file] [output path]')

    path_elf = sys.argv[1]
    path_output = sys.argv[2]

    with open(path_output, 'wb') as f_output:
        patch = extract_patch(path_elf)
        f_output.write(patch)
