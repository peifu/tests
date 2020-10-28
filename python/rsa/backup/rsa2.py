from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP
from Crypto.Signature import PKCS1_v1_5
from Crypto.Hash import SHA512, SHA384, SHA256, SHA, MD5
from Crypto import Random
from base64 import b64encode, b64decode
hash = "SHA-256"

def get_args():
	from argparse import ArgumentParser

	parser = ArgumentParser()
	parser.add_argument('--ta_rsa_key', dest = 'prv_key', required = True, \
			help = 'ta rsa private key, input file')
	parser.add_argument('--in', dest = 'unsigned_file', required = True, \
			help = 'input, unsigned file')
	parser.add_argument('--out', dest = 'signed_file', type = str, \
			default = 'null', help = 'output, signed file')

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

	if args.signed_file == 'null':
		args.signed_file = args.unsigned_file + ".sig"

	f = open(args.signed_file, 'wb')
	f.write(sig)
	f.close()

	print 'Signing File ...'
	print '  Input:                              '
	print '                       prv_key.size = {}'.format(prv_key.size() + 1)
	print '                 unsigned_file.name = ' + args.unsigned_ta
	print '  Output:            signature.name = ' + args.signature

if __name__ == "__main__":
	main()

def newkeys(keysize):
   random_generator = Random.new().read
   key = RSA.generate(keysize, random_generator)
   private, public = key, key.publickey()
   return public, private

def importKey(externKey):
   return RSA.importKey(externKey)

def getpublickey(priv_key):
   return priv_key.publickey()

def encrypt(message, pub_key):
   cipher = PKCS1_OAEP.new(pub_key)
   return cipher.encrypt(message)

def decrypt(ciphertext, priv_key):
   cipher = PKCS1_OAEP.new(priv_key)
   return cipher.decrypt(ciphertext)

def sign(message, priv_key, hashAlg = "SHA-256"):
   global hash
   hash = hashAlg
   signer = PKCS1_v1_5.new(priv_key)
   
   if (hash == "SHA-512"):
      digest = SHA256.new()
   elif (hash == "SHA-256"):
      digest = SHA256.new()
   else:
      digest = MD5.new()
   digest.update(message)
   return signer.sign(digest)

def verify(message, signature, pub_key):
   signer = PKCS1_v1_5.new(pub_key)
   if (hash == "SHA-512"):
      digest = SHA512.new()
   elif (hash == "SHA-256"):
      digest = SHA256.new()
   else:
      digest = MD5.new()

   digest.update(message)
   return signer.verify(digest, signature)
