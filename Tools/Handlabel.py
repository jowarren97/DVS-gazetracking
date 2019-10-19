import cv2
import sys
from PyAedatTools.ImportAedat import ImportAedat
import numpy as np

def locate_click(event, x, y, flags, param):
	global pupil_coord
	
	if event == cv2.EVENT_LBUTTONDOWN:
		pupil_coord[1] = x
		pupil_coord[2] = y
		print(pupil_coord)
		clone = frame.copy()
		cv2.circle(clone,(x,y), 1, (0,0,255), -1)
		cv2.imshow("image", clone)

def get_frames(relative_path):
# Create a dict with which to pass in the input parameters.
	aedat = {}
	aedat['importParams'] = {}

	# Put the filename, including full path, in the 'filePath' field.
	#aedat['importParams']['filePath'] = sys.path[0] + relative_path
	aedat['importParams']['filePath'] = "C:\\Users\\JW\\DVS-gazetracking\\n2d2_data\\dvs\\dvs\\DAVIS240C-2019-06-07T19-25-25+0100-84010088-0.aedat"
	# Alternatively, make sure the file is already on the python path.
	#sys.path.append('/home/project/')
	aedat['importParams']['startTime'] = 25;
	aedat['importParams']['endTime'] = 50;
	#These examples limit the read out to certain types of event only
	aedat['importParams']['dataTypes'] = {'frame'};

	# Setting the dataTypes empty tells the function to not import any data;
	# You get the header info, plus packet indices info for Aedat3.x

	# Working with a file where the source hasn't been declared - do this explicitly:
	aedat['importParams']['source'] = 'Davis240c';

	# Invoke the function
	aedat = ImportAedat(aedat)
	return aedat

	
file = open("DAVIS240C-2019-06-07T19-25-25+0100-84010088-0-groundtruth-25-50.txt", 'w')
aedat_data = get_frames('C:\\Users\\JW\\DVS-gazetracking\\n2d2_data\\dvs\\DAVIS240C-2019-05-24T19-21-22+0100-84010088-0.aedat')
pupil_coord = [1, 0, 0]
#print(aedat_data['data'])

# cap = cv2.VideoCapture('Participant 2-2-eye-left.avi')
# if (cap.isOpened()== False): 
	# print("Error opening video stream or file")

#ret, frame = cap.read()
frame = aedat_data['data']['frame']['samples'][0]
rows = aedat_data['data']['frame']['yLength'][0]
col = aedat_data['data']['frame']['xLength'][0]
#col = frame.shape[1]
N = len(aedat_data['data']['frame']['samples'])
print(N)

frames = [np.uint8(frame/4) for frame in aedat_data['data']['frame']['samples']]
frames = [cv2.cvtColor(frame,cv2.COLOR_GRAY2RGB) for frame in frames]

cv2.namedWindow("image", cv2.WINDOW_NORMAL)
#frame = np.array(frame/4, dtype=np.uint8)
cv2.imshow("image", frames[0])
cv2.setMouseCallback("image", locate_click)

counter = 0 #replace w timestamp later!!!
timestamp = int(round((aedat_data['data']['frame']['timeStampStart'][counter] + aedat_data['data']['frame']['timeStampEnd'][counter])/2))
pupil_coord = [timestamp, 0, 0]
while counter < N-1:
	#cv2.imshow("image", frame)
	key = cv2.waitKey(1) & 0xFF

	#ESCAPE TO EXIT
	if key == 27:
		break
	
	#SPACE TO GO NEXT FRAME
	elif key == 32:
		print("final pos", pupil_coord)
		print("next frame")
		print(counter)
		to_print = ','.join([str(timestamp), str(pupil_coord[1]), str(pupil_coord[2])])
		file.write(to_print)
		file.write('\n')
		
		counter = counter + 1
		#frame = aedat_data['data']['frame']['samples'][counter]
		#frame = np.array(frame/4, dtype=np.uint8)
		timestamp = int(round((aedat_data['data']['frame']['timeStampStart'][counter] + aedat_data['data']['frame']['timeStampEnd'][counter])/2))
		frame = frames[counter]
		cv2.imshow("image", frame)
		#reset for next frame
		blink = False
		pupil_coord[0] = timestamp
		clone = frame.copy()
		cv2.circle(clone,tuple(pupil_coord[1:]), 1, (0,0,255), -1)
		cv2.imshow("image", clone)
	
	
	elif key == ord("x"): #bad frame
		print("faulty frame")
		print("next frame")
		counter = counter + 1
		timestamp = int(round((aedat_data['data']['frame']['timeStampStart'][counter] + aedat_data['data']['frame']['timeStampEnd'][counter])/2))
		frame = frames[counter]
		cv2.imshow("image", frame)
		#reset for next frame
		blink = False
		pupil_coord = [timestamp, 0, 0]
	
	elif key == ord("b"):
		blink = True
		print("Blink!")
	
	elif key == ord("s"):
		if (pupil_coord[2] < rows):
			pupil_coord[2] = pupil_coord[2] + 1
		
		#update visual output
		clone = frame.copy()
		cv2.circle(clone,tuple(pupil_coord[1:]), 1, (0,0,255), -1)
		cv2.imshow("image", clone)
	
	elif key == ord("w"):
		if (pupil_coord[2] > 0):
			pupil_coord[2] = pupil_coord[2] - 1
		
		#update visual output
		clone = frame.copy()
		cv2.circle(clone,tuple(pupil_coord[1:]), 1, (0,0,255), -1)
		cv2.imshow("image", clone)
		
	elif key == ord("a"):
		if (pupil_coord[1] > 0):
			pupil_coord[1] = pupil_coord[1] - 1
		
		#update visual output
		clone = frame.copy()
		cv2.circle(clone,tuple(pupil_coord[1:]), 1, (0,0,255), -1)
		cv2.imshow("image", clone)
		
	elif key == ord("d"):
		if (pupil_coord[1] < col):
			pupil_coord[1] = pupil_coord[1] + 1
		
		#update visual output
		clone = frame.copy()
		cv2.circle(clone,tuple(pupil_coord[1:]), 1, (0,0,255), -1)
		cv2.imshow("image", clone)
		
cv2.destroyAllWindows()
file.close()