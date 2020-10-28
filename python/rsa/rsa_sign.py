#!/usr/bin/env python
#
# Copyright (C) 2016 Amlogic, Inc. All rights reserved.
#
#
# This source code is subject to the terms and conditions defined in the
# file 'LICENSE' which is part of this source code package.
#
#


def get_args():
	from argparse import ArgumentParser

	parser = ArgumentParser()
	parser.add_argument('--rsa_key', dest = 'prv_key', required = True, \
			help = 'ta rsa private key, input file')
	parser.add_argument('--in', dest = 'unsigned_file', required = True, \
			help = 'input, unsigned file')
	parser.add_argument('--out', dest = 'signature', type = str, \
			default = 'null', help = 'output, signature')

	return parser.parse_args()

def main():
	import sys
	import struct
	from Crypto.Signature import PKCS1_v1_5
	from Crypto.Hash import SHA256
	from Crypto.PublicKey import RSA

	args = get_args()

	f = open(args.unsigned_file, 'rb')
	input_data = f.read()
	f.close()

	f = open(args.prv_key, 'rb')
	prv_key = RSA.importKey(f.read())
	pub_key = prv_key.publickey()
	f.close()

	# gen ta sig
	sha256 = SHA256.new()
	sha256.update(input_data)
	sig = PKCS1_v1_5.new(prv_key).sign(sha256)

	if args.signature == 'null':
		args.signature = args.unsigned_file + ".sig"

	f = open(args.signature, 'wb')
	f.write(sig)
	f.close()

	print 'Signing File ...'
	print '  Input:                              '
	print '                       prv_key.size = {}'.format(prv_key.size() + 1)
	print '                 unsigned_file.name = ' + args.unsigned_file
	print '  Output:            signature.name = ' + args.signature

if __name__ == "__main__":
	main()
