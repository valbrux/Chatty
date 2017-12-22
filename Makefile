#
#	Root Makefile
#
SOURCE_DIR = src/

all:
	if [ ! -d bin/ ];then mkdir bin/;fi;cd $(SOURCE_DIR);make;


