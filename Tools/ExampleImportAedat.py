# -*- coding: utf-8 -*-
"""
Example script for how to invoke the ImportAedat function
"""

import sys
from PyAedatTools.ImportAedat import ImportAedat

# Create a dict with which to pass in the input parameters.
aedat = {}
aedat['importParams'] = {}

# Put the filename, including full path, in the 'filePath' field.

aedat['importParams']['filePath'] = 'C:\\Users\\JW\\DVS-gazetracking\\groundtruth\\recordings\\DAVIS240C-2019-04-09T14-19-56+0100-84010088-0.aedat' # Windows

#These examples limit the read out to certain types of event only
aedat['importParams']['dataTypes'] = {'polarity'}

# Setting the dataTypes empty tells the function to not import any data;
# You get the header info, plus packet indices info for Aedat3.x

# Working with a file where the source hasn't been declared - do this explicitly:
aedat['importParams']['source'] = 'Davis240c';

# Invoke the function
aedat = ImportAedat(aedat)

print(aedat['data']['polarity']['x'][0:5])

