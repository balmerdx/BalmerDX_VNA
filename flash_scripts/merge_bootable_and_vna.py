import hashlib
import os.path
import sys
'''
Собирает вместе vna_filename и bootloader_filename
Полученный файл out_filename можно прошивать в наш VNA
'''
vna_filename = "../4code/output/program_rboot.bin"
bootloader_filename = "../bootloader/output/program_release.bin"
out_filename = "../binary_to_flash/BalmerDX_VNA.bin_bootable"

#Количество байт, которые надо подавать в команду COMMAND_WRITE_FLASH
#округляем размер vna части до такого количеств байт
WRITE_TO_FLASH_SIZE = 512

#максимально возможный размер bootloader
SIZE_BOOTLOADER = 32*1024
#размер данных после bootloader
SIZE_MISC_DATA = 32*1024
#размер блока в котором помещается размер программы (4 байта) и ее md5 сумма
SIZE_PROGRAM_INFO = 1024

def md5digest(in_data):
	h = hashlib.md5()
	h.update(in_data)
	return h.digest()

def align(in_data, block_size):
	additional_bytes_count = (len(in_data)+block_size-1)//block_size*block_size-len(in_data)
	add_bytes = bytes(additional_bytes_count)
	in_data = in_data + add_bytes
	return in_data

def read_and_align(in_filename, block_size):
	if not os.path.isfile(in_filename):
		print("Cannot found file:", in_filename)
		return False
	f = open(in_filename, "rb")
	in_data = f.read()
	f.close()
	#print("len(in_data)=", len(in_data))
	return align(in_data, block_size)


def save(out_filename, bytearrays):
	fout = open(out_filename, "wb")

	for b in bytearrays:
		fout.write(b)
	fout.close()
	print("Write '{}' success".format(out_filename))
	return True

def main():
	vna_bin = read_and_align(vna_filename, WRITE_TO_FLASH_SIZE)
	bootloader_bin = read_and_align(bootloader_filename, SIZE_BOOTLOADER)
	program_info = align(len(vna_bin).to_bytes(4, byteorder='little')+md5digest(vna_bin), SIZE_PROGRAM_INFO)
	save(out_filename, [bootloader_bin, bytearray(SIZE_MISC_DATA), program_info, vna_bin])

main()