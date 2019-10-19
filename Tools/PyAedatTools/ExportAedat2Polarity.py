import struct

def ExportAedat2Polarity(aedat):

	"""
	This function exports data to a .aedat file. 
	The .aedat file format is documented here:

	http://inilabs.com/support/software/fileformat/
	"""	

	# Create the file
	if not 'exportParams' in aedat or not 'filePath' in aedat['exportParams']:
		raise NameError('Missing file path and name')

	f = open(aedat['exportParams']['filePath'], 'wb');
	# Simple - events only - assume DAVIS

	# CRLF \r\n is needed to not break header parsing in jAER
	string = '#!AER-DAT2.0\r\n'
	bytes = string.encode(encoding='UTF-8')
	f.write(bytes);
	#f.write('# This is a raw AE data file created by saveaerdat.m\r\n');
	#f.write('# Data format is int32 address, int32 timestamp (8 bytes total), repeated for each event\r\n');
	#f.write('# Timestamps tick is 1 us\r\n');

	# Put the source in NEEDS DOING PROPERLY
	#f.write('# AEChip: DAVIS240C\r\n');

	#f.write('# End of ASCII Header\r\n');


	# DAVIS
	# In the 32-bit address:
	# bit 32 (1-based) being 1 indicates an APS sample
	# bit 11 (1-based) being 1 indicates a special event 
	# bits 11 and 32 (1-based) both being zero signals a polarity event

	yShiftBits = 22;
	xShiftBits = 12;
	polShiftBits = 11;
	
	for i in range(len(aedat['data']['polarity']['polarity'])):
	#for i in range(2000000):
	
		x = aedat['data']['polarity']['x'][i]
		#print(x)
		#print('{0:032b}'.format(x))
		x = x * (2 ** xShiftBits)
		#print("x address: ", '{0:032b}'.format(x))
		
		y = aedat['data']['polarity']['y'][i] 
		#print(y)
		#print('{0:032b}'.format(y))
		y = y * (2 ** yShiftBits)
		#print("y address: ", '{0:032b}'.format(y))
		
		pol = aedat['data']['polarity']['polarity'][i] * (2 ** polShiftBits)
		#print("polarity:  ", '{0:032b}'.format(pol))
		
		addr = y + x + pol
		addr32 = struct.pack('>I', addr)
		ts = aedat['data']['polarity']['timeStamp'][i]
		ts32 = struct.pack('>I', ts)
		#addr32 = '{0:032b}'.format(addr)
		#print("final address: ", addr)
		#print("final address: ", addr32)
		#print('\n')
		
		if (i%10000 == 0):
			print(i)
			# print(addr32.encode())
		
		f.write(addr32)
		f.write(ts32)
		#print("Timestamp:", ts)
		
		#print("Address: ", addr)
		#print("Binary Address: ",addr32)
	
	
	

	#output(1:2:end) = y + x + pol;
	#output(2:2:end)=int32(input.data.polarity.timeStamp(:)); # set even elements to timestamps

	# write addresses and timestamps
	#count=fwrite(f,output,'uint32')/2; # write 4 byte data
	f.close();
	#fprintf('wrote #d events to #s\n',count,input.info.filePath);


