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
	parser.add_argument('--rsa_key', dest = 'pub_key', required = True, \
			help = 'input, rsa public key')
	parser.add_argument('--in', dest = 'input_file', required = True, \
			help = 'input, unsigned file')
	parser.add_argument('--sig', dest = 'sig', required = True, \
			help = 'input, signature')

	return parser.parse_args()

def main():
	import sys
	import struct
	from Crypto.Signature import PKCS1_v1_5
	from Crypto.Hash import SHA256
	from Crypto.PublicKey import RSA

	args = get_args()

	f = open(args.input_file, 'rb')
	message = f.read()
	f.close()

	f = open(args.pub_key, 'rb')
	pub_key = RSA.importKey(f.read())
	pub_key = pub_key.publickey()
	f.close()

	f = open(args.sig, 'rb')
	signature = f.read()
	f.close()

	signer = PKCS1_v1_5.new(pub_key)
	digest = SHA256.new()
	digest.update(message)
	result = signer.verify(digest, signature)

	print 'Signing File ...'
	print '  Input:                              '
	print '                       pub_key.size = {}'.format(pub_key.size() + 1)
	print '                 input_file.name = ' + args.input_file
	print '                  signature.name = ' + args.sig
	if result :
		print '  Verify: SUCCESS!'
	else:
		print '  Verify: FAILED!'

if __name__ == "__main__":
	main()
