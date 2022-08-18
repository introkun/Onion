ifeq (,$(BUILD_DIR))
BUILD_DIR=$(shell pwd -P)
endif

PLATFORM ?= $(UNION_PLATFORM)
ifeq (,$(PLATFORM))
PLATFORM=linux
endif

LIB = /mnt/SDCARD/.tmp_update/lib

CC 		= $(CROSS_COMPILE)gcc
CXX 	= $(CROSS_COMPILE)g++
STRIP 	= $(CROSS_COMPILE)strip

SOURCES := $(SOURCES) .
ifdef INCLUDE_CJSON
SOURCES := $(SOURCES) ../../include/cjson
endif
CFILES = $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES = $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.cpp))
OFILES := $(OFILES) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

CFLAGS := $(CFLAGS) -I../../include -I../common -DPLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z) -Wall

ifeq ($(DEBUG),1)
CFLAGS := $(CFLAGS) -DLOG_DEBUG
endif

ifeq ($(TEST),1)
CFLAGS := $(CFLAGS) -std=c++17
endif

CXXFLAGS := $(CFLAGS)
LDFLAGS := -L../../lib

ifeq ($(PLATFORM),miyoomini)
CFLAGS := $(CFLAGS) -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -Wl,-rpath=$(LIB)

ifdef INCLUDE_SHMVAR
LDFLAGS := $(LDFLAGS) -lshmvar
endif

endif
