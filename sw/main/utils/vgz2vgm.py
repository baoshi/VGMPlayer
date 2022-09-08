import time
import gzip
import traceback
import sys
import os

#dir = sys.argv[1]
dir = 'R:/Life_Force_(NES)'
for file in os.listdir(dir):
	if file.endswith('.vgz'):
		vgz = os.path.join(dir, file)
		print(vgz)
		base = os.path.splitext(vgz)[0]
		vgm = base + '.vgm'
		try:
			with gzip.open(vgz, mode='r', compresslevel=9) as f:
				data = f.read()
			f = open(vgm, 'wb')
			f.write(data)
			f.close()
		except:
			traceback.print_exc()
			time.sleep(10)