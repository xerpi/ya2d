TARGET = ya2d
OBJS = main.o
DATA_OBJS = test3.o

INCDIR =
CFLAGS = -G0 -Wall -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
OBJS+=$(DATA_OBJS)

LIBDIR =
LDFLAGS =
LIBS= -lya2d -ltinyfont -lpspdisplay -lpspgum -ljpeg -lpspgu -lpng -lz -lm -lpspvram -lpspmath

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = ya2d Sample

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


copy: EBOOT.PBP
    mkdir -p /media/$(USER)/disk/PSP/GAME/$(notdir $(CURDIR))
    cp EBOOT.PBP /media/$(USER)/disk/PSP/GAME/$(notdir $(CURDIR))
    sync

%.o    : %.png
    bin2o -i $< $@ $(addsuffix _png, $(basename $(notdir $<) ))
    
%.o    : %.bmp
    bin2o -i $< $@ $(addsuffix _bmp, $(basename $(notdir $<) ))
    
%.o    : %.jpg
    bin2o -i $< $@ $(addsuffix _jpg, $(basename $(notdir $<) ))
    
%.o    : %.jpeg
    bin2o -i $< $@ $(addsuffix _jpeg, $(basename $(notdir $<) ))
        
