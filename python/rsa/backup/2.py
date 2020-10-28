#!/usr/bin/env python3

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import dsa, ec, rsa, utils
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import padding

# configuration per key type, each lambda takes a hashing algorithm
_signing_configs = (
    (dsa.DSAPrivateKey, lambda h: {
        'algorithm': h}),
    (ec.EllipticCurvePrivateKey, lambda h: {
        'signature_algorithm': ec.ECDSA(h)}),
    (rsa.RSAPrivateKey, lambda h: {
        'padding': padding.PKCS1v15(),
        'algorithm': h
    }),
)

def _key_singing_config(key, hashing_algorithm):
    try:
        factory = next(
            config
            for type_, config in _signing_configs
            if isinstance(key, type_)
        )
    except StopIteration:
        raise ValueError('Unsupported key type {!r}'.format(type(key)))
    return factory(hashing_algorithm)

def sign(private_key, data, algorithm=hashes.SHA256()):
    with open(private_key, 'rb') as private_key:
        key = serialization.load_pem_private_key(
            private_key.read(), None, default_backend())

    return key.sign(data, **_key_singing_config(key, algorithm))


signature = sign("./keyfile", b'Hello world!')
import base64

print(base64.encodebytes(signature).decode())
