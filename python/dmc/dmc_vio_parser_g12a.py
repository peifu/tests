#!/usr/bin/env python
#
# Copyright (C) 2018 Spacetime Studio, Inc. All rights reserved.
#
#
# DMC Violation Register Parser
#
# Author:  jiangpeifu@gmail.com
# Version: 0.1
#

DMC_PORT0 = {
	'id': 0,
	'port': 'CPU',
}
DMC_PORT1 = {
	'id': 1,
	'port': 'GPU',
}
DMC_PORT2 = {
	'id': 2,
	'port': 'PCID',
}
DMC_PORT3 = {
	'id': 3,
	'port': 'HDMI',
}
DMC_PORT4 = {
	'id': 4,
	'port': 'HEVC_F',
}
DMC_PORT5 = {
	'id': 5,
	'port': 'TEST',
}
DMC_PORT6 = {
	'id': 6,
	'port': 'USB3.0',
}
DMC_PORT7 = {
	'id': 7,
	'port': 'DEVICE',
}

DMC_PORT8 = {
	'id': 8,
	'port': 'HEVC_B',
}
DMC_PORT9 = {
	'id': 9,
	'port': 'WAVE',
}
DMC_PORT10 = {
	'id': 10,
	'port': 'NNA',
}
DMC_PORT11 = {
	'id': 11,
	'port': 'GDC',
}
DMC_PORT12 = {
	'id': 12,
	'port': 'ISP',
}
DMC_PORT13 = {
	'id': 13,
	'port': 'NOTUSED',
}
DMC_PORT14 = {
	'id': 14,
	'port': 'NOTUSED',
}
DMC_PORT15 = {
	'id': 15,
	'port': 'NOTUSED',
}
DMC_PORT16 = {
	'id': 16,
	'port': 'VPU_R0',
}
DMC_PORT17 = {
	'id': 17,
	'port': 'VPU_R1',
}
DMC_PORT18 = {
	'id': 18,
	'port': 'VPU_R2',
}
DMC_PORT19 = {
	'id': 19,
	'port': 'VPU_W0',
}
DMC_PORT20 = {
	'id': 20,
	'port': 'VPU_W1',
}
DMC_PORT21 = {
	'id': 21,
	'port': 'VDEC',
}
DMC_PORT22 = {
	'id': 22,
	'port': 'HCODEC',
}
DMC_PORT23 = {
	'id': 23,
	'port':'GE2D',
}

DMC_PORTS = [
	DMC_PORT0,
	DMC_PORT1,
	DMC_PORT2,
	DMC_PORT3,
	DMC_PORT4,
	DMC_PORT5,
	DMC_PORT6,
	DMC_PORT7,
	DMC_PORT8,
	DMC_PORT9,
	DMC_PORT10,
	DMC_PORT11,
	DMC_PORT12,
	DMC_PORT13,
	DMC_PORT14,
	DMC_PORT15,
	DMC_PORT16,
	DMC_PORT17,
	DMC_PORT18,
	DMC_PORT19,
	DMC_PORT20,
	DMC_PORT21,
	DMC_PORT22,
	DMC_PORT23,
]

DMC_SUBID2 = {
	'id': 2,
	'mod': 'GPU',
}
DMC_SUBID3 = {
	'id': 3,
	'mod': 'A53 cluster',
}
DMC_SUBID4 = {
	'id': 4,
	'mod': 'A73 cluster',
}
DMC_SUBID7 = {
	'id': 7,
	'mod': 'CCI',
}

DMC_SUBIDS = [
	DMC_SUBID2,
	DMC_SUBID3,
	DMC_SUBID4,
	DMC_SUBID7,
]

def get_args():
	from argparse import ArgumentParser

	parser = ArgumentParser()
	parser.add_argument('-i', required=True, help='Violation Value')
	return parser.parse_args()

def rprint(arg):
#	print('\033[1;31;40m')                                                                                
	print('\033[1;31m' + arg + '\033[0m')                                                                                   
def reg_parser(reg):
	val = int(reg, 16)
	bit_26 = (val >> 26) & 1
	bit_25 = (val >> 25) & 1
	bit_24 = (val >> 24) & 1
	bit_23 = (val >> 23) & 1
	bit_22 = (val >> 22) & 1
	bit_21 = (val >> 21) & 1
	bit_20_18 = (val >> 18) & 0x7
	bit_17_13 = (val >> 13) & 0x1F
	bit_9_6 = (val >> 6) & 0xF
	bit_4_2 = (val >> 2) & 0x7
	portid = bit_17_13;
	subid = bit_9_6;
	subid2 = bit_4_2;

	print('violation value: ' + reg)
	print('  read secure check violation     bit 26: ' + str(bit_26))
	print('  read protection 1 violation     bit 25: ' + str(bit_25))
	print('  read protection 0 violation     bit 24: ' + str(bit_24))
	print('  read trap 1 violation           bit 23: ' + str(bit_23))
	print('  read trap 0 violation           bit 22: ' + str(bit_22))
	print('  read address overflow           bit 21: ' + str(bit_21))
	print('  read violation ARPROT bits   bit 20_18: ' + str(bit_20_18))
	rprint('  read violation port ID       bit 17_13: ' + str(bit_17_13))
	rprint('  read violation sub ID          bit 9_6: ' + str(bit_9_6))
	rprint('  read violation sub ID          bit 4_2: ' + str(bit_4_2))
	for port in DMC_PORTS:
		if port['id'] == portid:
			port_name = port['port']
			break
		else:
			port_name = 'UNKNOWN'
	for mod in DMC_SUBIDS:
		if mod['id'] == subid2:
			sub_name = mod['mod']
			break
		else:
			sub_name = 'UNKNOWN'

	rprint('  read violation sub ID          bit 4_2: ' + str(bit_4_2))
	rprint('  READ VIOLATION                    PORT: ' + port_name)
	rprint('  READ VIOLATION                  SUBMOD: ' + sub_name)


def main():
	import os

	args = get_args();
	reg = args.i
	reg_parser(reg)

if __name__ == "__main__":
	main()
