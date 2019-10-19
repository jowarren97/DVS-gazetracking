from PyAedatTools.ImportAedat import ImportAedat
from PyAedatTools.ExportAedat2Polarity import ExportAedat2Polarity
#import sys

aedat = {}
aedat['importParams'] = {}
#aedat['importParams']['filePath'] = "C:\\Users\\JW\\DVS-gazetracking\\groundtruth\\recordings\\DAVIS240C-2019-04-09T14-19-56+0100-84010088-0-frame.aedat"
aedat['importParams']['filePath'] = "C:\\Users\\JW\\DVS-gazetracking\\n2d2_data\\dvs\\dvs\\DAVIS240C-2019-06-07T19-25-25+0100-84010088-0.aedat"
#aedat['importParams']['filePath'] = "C:\\Users\\JW\\DVS-gazetracking\\n2d2_data\\dvs\\DAVIS240C-2019-04-09T14-19-56+0100-84010088-0.aedat"
aedat['importParams']['dataTypes'] = {'polarity'};
aedat['importParams']['source'] = 'Davis240c';
aedat['exportParams'] = {}
#aedat['exportParams']['filePath'] = "C:\\Users\\JW\\DVS-gazetracking\\groundtruth\\recordings\\DAVIS240C-2019-04-09T14-19-56+0100-84010088-0-frame.aedat"
aedat['exportParams']['filePath'] = "C:\\Users\\JW\\DVS-gazetracking\\n2d2_data\\dvs\\dvs\\DAVIS240C-2019-06-07T19-25-25+0100-84010088-0-frame.aedat"

aedat = ImportAedat(aedat)

ExportAedat2Polarity(aedat)

#pol = aedat['data']['polarity']['polarity']

#print(pol)


# output_file = open(aedat['importParams']['filePath'][:-6] + '.dat', 'wb') #overwrite

# header = '#!AER-DAT3\r\n'
# header_bin = header.encode()
# output_file.write(header_bin)
# x_bin = f'{x[0]:08b}'
# y_bin = f'{y[0]:08b}'
# addr = x_bin + y_bin
# output_file.write(addr.encode())




