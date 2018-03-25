#!/usr/bin/env python

import qrcode

start_num=2055200
total = 20
count = 0

def genseed(num):
	url="http://smf.veiwa.com/AntiFakeTracing/Mobile/Index.aspx?code=%09d" % num
	img=qrcode.make(url)
	img.save("./%09d.png" % num)

print "=== generate seed ..."
print "=== start number", 
print start_num
print "=== total %d" % total 
while count < total:
	count = count + 1
	genseed(start_num + count)
print "=== generating successfully ==="
