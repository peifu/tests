#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import base64
import binascii

def get_args():
	from argparse import ArgumentParser

	parser = ArgumentParser()
	parser.add_argument('-e', help='Base64 encode, input should be hex string')
	parser.add_argument('-d', help='Base64 decode, input should Base64 encoded string')
	return parser.parse_args()

def usage():
	print("base64.py Usage:")
	print("-s    Input HEX string")
	print("-b    Input Base64 encoded string")

def red(arg):
	return '\033[1;31m' + arg + '\033[0m'

def rprint(arg):
#       print('\033[1;31;40m')
	print('\033[1;31m' + arg + '\033[0m')

def base64encode(arg):
	inf = binascii.unhexlify(arg)
	out = base64.b64encode(inf)
	print('Input:               Byte string: ' + arg)
	print('Output:    Base64 encoded string: ' + red(out))

def base64decode(arg):
	out = base64.b64decode(arg)
	hstr = binascii.b2a_hex(out)
	print('Input:             Base64 string: ' + arg)
	print('Output:       Decoded Hex string: ' + hstr)

def main():
	args = get_args();
	estr = args.e
	dstr = args.d

	if (estr != None):
		base64encode(estr)
	elif (dstr != None):
		base64decode(dstr)
	else:
		usage()
		return

if __name__ == "__main__":
	main()
