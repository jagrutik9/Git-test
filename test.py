import struct
def _16bit_unsigned_to_float(LSB, MSB):
	LSB = int(LSB)
	MSB = int(MSB)
	packed_v = struct.pack('BB', MSB, LSB)
	g = struct.unpack('f', packed_v)[0]
	return g
a = 18
b = 12
print(_16bit_unsigned_to_float(a, b))