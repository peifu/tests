#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import base64
import binascii
import operator

UR_MASK = 0xFFFF00000000
UR_MAGIC = 0xA50400000000
UR_EXT_MASK = 0xFFFF000000000000
UR_EXT_MAGIC = 0xA506000000000000
UR_2NDEXT_MASK = 0xFFFF0000000000000000000000000000
UR_2NDEXT_MAGIC = 0xA50E0000000000000000000000000000
UR_3RDEXT_MASK = 0xFFFF000000000000000000000000000000
UR_3RDEXT_MAGIC = 0xA50F000000000000000000000000000000

KEYPOINT = ['protect', 'hdcp']

UR_DATA = {
	'version': [30, 2, 0],
	'anticipation_flag': [29, 1, 0],
	'recording_flag': [28, 1, 0],
	'original_sub_required': [27, 1, 0],
	'dvr_sub_required': [26, 1, 0],
	'trans_coding_control': [25, 1, 0],
	'image_constraint': [24, 1, 0],
	'trick_mode_control': [20, 4, 0],
	'playcount': [16, 4, 0], 
	'concurrent_viewing': [13, 3, 0],
	'digital_only_token': [12, 1, 0],
	'unprotected_digital_output_token': [11, 1, 0],
	'hdcp_token': [10, 1, 0],
	'dtcp_token': [9, 1, 0],
	'playready_token': [8, 1, 0],
	'PRM_token': [7, 1, 0],
	'others_token': [6, 1, 0],
	'cci': [4, 2, 0],
	'unprotected_analog_output_token': [3, 1, 0],
	'retention_state': [0, 3, 0]
}

UR_EXT_DATA = {
	'version': [46, 2, 0],
	'anticipation_flag': [45, 1, 0],
	'recording_flag': [44, 1, 0],
	'original_sub_required': [43, 1, 0],
	'dvr_sub_required': [42, 1, 0],
	'trans_coding_control': [41, 1, 0],
	'image_constraint': [40, 1, 0],
	'trick_mode_control': [36, 4, 0],
	'playcount': [32, 4, 0], 
	'concurrent_viewing': [29, 3, 0],
	'digital_only_token': [28, 1, 0],
	'unprotected_digital_output_token': [27, 1, 0],
	'hdcp_token': [26, 1, 0],
	'dtcp_token': [25, 1, 0],
	'playready_token': [24, 1, 0],
	'PRM_token': [23, 1, 0],
	'others_token': [22, 1, 0],
	'cci': [20, 2, 0],
	'unprotected_analog_output_token': [19, 1, 0],
	'retention_state': [16, 3, 0],
	'pc_threshold': [12, 4, 0],
	'pc_bitmap': [0, 12, 0]
}

UR_2NDEXT_DATA = {
	'version': [110, 2, 0],
	'anticipation_flag': [109, 1, 0],
	'recording_flag': [108, 1, 0],
	'original_sub_required': [107, 1, 0],
	'dvr_sub_required': [106, 1, 0],
	'trans_coding_control': [105, 1, 0],
	'image_constraint': [104, 1, 0],
	'trick_mode_control': [100, 4, 0],
	'playcount': [96, 4, 0], 
	'concurrent_viewing': [93, 3, 0],
	'digital_only_token': [92, 1, 0],
	'unprotected_uncompressed_digital_output_token': [91, 1, 0],
	'hdcp_uncompressed_token': [90, 1, 0],
	'dtcp_token': [89, 1, 0],
	'playready_token': [88, 1, 0],
	'PRM_token': [87, 1, 0],
	'others_token': [86, 1, 0],
	'cci': [84, 2, 0],
	'unprotected_analog_output_token': [83, 1, 0],
	'retention_state': [80, 3, 0],
	'pc_threshold': [76, 4, 0],
	'pc_bitmap': [64, 12, 0],
	'securemedia_token': [63, 1, 0],
	'fairplay_token': [62, 1, 0],
	'widevine_token': [61, 1, 0],
	'marlin_token': [60, 1, 0],
	'OMAv2_token': [59, 1, 0],
	'adobe_token': [58, 1, 0],
	'hdcp_compressed_token': [57, 1, 0],
	'unprotected_compressed_digital_output_token': [56, 1, 0],
	'HDCP_type': [48, 8, 0],
	'uncompressed_digital_capping_bitrate': [40, 8, 0],
	'analog_capping_bitrate': [32, 8, 0],
	'compressed_digital_capping_bitrate': [24, 8, 0],
	'device_capping_bitrate': [16, 8, 0],
	'uncompressed_digital_capping_resolution': [12, 4, 0],
	'analog_capping_resolution': [8, 4, 0],
	'compressed_digital_capping_resolution': [4, 4, 0],
	'device_capping_resolution': [0, 4, 0],
}

UR_3RDEXT_DATA = {
	'version': [118, 2, 0],
	'anticipation_flag': [117, 1, 0],
	'recording_flag': [116, 1, 0],
	'original_sub_required': [115, 1, 0],
	'dvr_sub_required': [114, 1, 0],
	'trans_coding_control': [113, 1, 0],
	'image_constraint': [112, 1, 0],
	'trick_mode_control': [108, 4, 0],
	'playcount': [104, 4, 0], 
	'concurrent_viewing': [101, 3, 0],
	'digital_only_token': [100, 1, 0],
	'unprotected_uncompressed_digital_output_token': [99, 1, 0],
	'hdcp_uncompressed_token': [98, 1, 0],
	'dtcp_token': [97, 1, 0],
	'playready_token': [96, 1, 0],
	'PRM_token': [95, 1, 0],
	'others_token': [94, 1, 0],
	'cci': [92, 2, 0],
	'unprotected_analog_output_token': [91, 1, 0],
	'retention_state': [88, 3, 0],
	'pc_threshold': [84, 4, 0],
	'pc_bitmap': [72, 12, 0],
	'securemedia_token': [71, 1, 0],
	'fairplay_token': [70, 1, 0],
	'widevine_token': [69, 1, 0],
	'marlin_token': [68, 1, 0],
	'OMAv2_token': [67, 1, 0],
	'adobe_token': [66, 1, 0],
	'hdcp_compressed_token': [65, 1, 0],
	'unprotected_compressed_digital_output_token': [64, 1, 0],
	'HDCP_type': [56, 8, 0],
	'uncompressed_digital_capping_bitrate': [48, 8, 0],
	'analog_capping_bitrate': [40, 8, 0],
	'compressed_digital_capping_bitrate': [32, 8, 0],
	'device_capping_bitrate': [24, 8, 0],
	'uncompressed_digital_capping_resolution': [20, 4, 0],
	'analog_capping_resolution': [16, 4, 0],
	'compressed_digital_capping_resolution': [12, 4, 0],
	'device_capping_resolution': [8, 4, 0],
	'watermarking_enable_token': [7, 1, 0],
	'secure_media_path_activation': [5, 2, 0],
	'minimum_device_security_level': [1, 4, 0],
	'reserved': [0, 1, 0],
}

def get_args():
	from argparse import ArgumentParser

	parser = ArgumentParser()
	parser.add_argument('-p', help='Parse UR, input should be hex string')
	parser.add_argument('-e', help='Base64 encode, input should be hex string')
	parser.add_argument('-d', help='Base64 decode, input should Base64 encoded string')
	return parser.parse_args()

def usage():
	print("Usage:")
	print("-e    Base64 encoding, input Hex string, output Base64 string")
	print("-d    Base64 decoding, input Base64 string, output Hex string")
	print("-p    Nagra usage rule parsing, input Hex string, output NUR table")

def red(arg):
	return '\033[1;31m' + arg + '\033[0m'

def rprint(arg):
	print('\033[1;31m' + arg + '\033[0m')

def iskeypoint(arg):
	for key in KEYPOINT:
		if key in arg:
			return True
	return False


def dump_ur(arg):
	print("------------------------------------------------------------------")
	sorted_items = sorted(arg.items(), key=operator.itemgetter(1), reverse=True)
	for name, data in sorted_items:
		if iskeypoint(name):
			rprint("%48s: %3d, %3d, %3d" %(name, data[0], data[1], data[2]))
		else:
			print("%48s: %3d, %3d, %3d" %(name, data[0], data[1], data[2]))
	print("------------------------------------------------------------------")

def update_ur(val, ur):
	for name, data in ur.items():
		data[2] = ((val >> data[0]) & ((1 << data[1]) - 1))
def parse_ur(arg):
	print('Input:                Hex string: ' + arg)
	val = int(arg, 16)
	if (val & UR_MASK) == UR_MAGIC:
		print('Output:           Usage rules(4): ')
		update_ur(val, UR_DATA)
		dump_ur(UR_DATA)
	elif (val & UR_EXT_MASK) == UR_EXT_MAGIC:
		print('Output:           Usage rules(6): ')
		update_ur(val, UR_EXT_DATA)
		dump_ur(UR_EXT_DATA)
	elif (val & UR_2NDEXT_MASK) == UR_2NDEXT_MAGIC:
		print('Output:          Usage rules(14): ')
		update_ur(val, UR_2NDEXT_DATA)
		dump_ur(UR_2NDEXT_DATA)
	elif (val & UR_3RDEXT_MASK) == UR_3RDEXT_MAGIC:
		print('Output:          Usage rules(15): ')
		update_ur(val, UR_3RDEXT_DATA)
		dump_ur(UR_3RDEXT_DATA)
	else:
		print('                       Bad Magic: ' + hex(val & UR_MASK))
		print('                  Excepted Magic: ' + red(hex(UR_MAGIC)))
		return
	
def base64encode(arg):
	inf = binascii.unhexlify(arg)
	out = base64.b64encode(inf)
	print('Input:                Hex string: ' + arg)
	print('Output:    Base64 encoded string: ' + red(out))

def base64decode(arg):
	out = base64.b64decode(arg)
	hstr = binascii.b2a_hex(out)
	print('Input:             Base64 string: ' + arg)
	print('Output:       Decoded Hex string: ' + hstr)

def main():
	args = get_args();
	pstr = args.p
	estr = args.e
	dstr = args.d

	if (pstr != None):
		parse_ur(pstr)
	elif (estr != None):
		base64encode(estr)
	elif (dstr != None):
		base64decode(dstr)
	else:
		usage()

if __name__ == "__main__":
	main()
