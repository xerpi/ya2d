TARGET_LIB = libya2d.a
OBJS = ya2d_main.o ya2d_image.o ya2d_texture.o ya2d_utils.o

CFLAGS = -G0 -Wall -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)


PSPSDK=$(shell psp-config --pspsdk-path)
PSPDEV=$(shell psp-config --pspdev-path)
include $(PSPSDK)/lib/build.mak

all:
	@echo "Type 'make install' to install the library!"

install: $(TARGET_LIB)
	cp $(TARGET_LIB) $(PSPDEV)/psp/lib
	cp ./*.h $(PSPDEV)/psp/include
	@echo "libya2d installed!"
	
