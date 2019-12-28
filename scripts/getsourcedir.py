# getsourcedir.py : Returns a list with all sources found in a directory (For use with CMake)

import os
import re

inputdir = os.path.abspath(input())
print("Path inputted: " + inputdir)
sourcefilter = r".*\.(c)|(h)|(cpp)|(hpp)|(tpp)"

if not os.path.isdir(inputdir):
	print("Please input a valid directory.")
	exit()
else:
	print("Valid directory")

for (dirpath, dirnames, filenames) in os.walk(inputdir):
	for allowedfilename in filter(lambda f: re.match(sourcefilter, f), filenames):
		path = (os.path.relpath(dirpath, os.getcwd()).lstrip(".")+"/"+allowedfilename).replace("\\", "/")
		print("\"${CMAKE_CURRENT_SOURCE_DIR}/" + path + "\"")