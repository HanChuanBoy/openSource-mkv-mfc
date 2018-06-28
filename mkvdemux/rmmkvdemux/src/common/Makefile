TARGET_TYPE = OBJECT_LIBRARY
TARGET = libmkvcommon.a

RMCFLAGS += -I$(PREFIX)/include -I../../include -fno-rtti

SRC= $(wildcard *.cpp)

USE_STD_LIB=1
DISABLE_WARNINGS=1

base_dir := ../../..
include $(base_dir)/scripts/inc.Makefile

