TARGET_TYPE = OBJECT_LIBRARY
TARGET = libmkvextract.a

RMCFLAGS += -I../common -I../../include -fno-rtti 
RMCFLAGS += -I$(PREFIX)/include

SRC= $(wildcard *.cpp)

USE_STD_LIB=1
DISABLE_WARNINGS=1

base_dir := ../../..
include $(base_dir)/scripts/inc.Makefile

