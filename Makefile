TARGET = numpty
OBJS = 	../common/callbacks.o ../common/vram.o \
		obj/Canvas.o \
		obj/EditOverlay.o \
		obj/Game.o \
		obj/Image.o \
		obj/Levels.o \
		obj/NextLevelOverlay.o \
		obj/Overlay.o \
		obj/Path.o \
		obj/Scene.o \
		obj/SDL_Lite.o \
		obj/Segment.o \
		obj/Stroke.o \
		obj/Window.o \
		obj/main.o \
		obj/PauseOverlay.o \
		obj/CanvasSoft.o

obj/%.o: src/%.cpp
	psp-gcc $(CFLAGS) -c -o $@ $<

obj/%.o: %.cpp
	psp-gcc $(CFLAGS) -c -o $@ $<

INCDIR =
CFLAGS = -G0 -Wall -O2 -I./Include -I./
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpspgum -lpspgu -lBox2D -lm -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Numpty Physics

PSP_EBOOT_ICON = img/ICON0.png
PSP_EBOOT_PIC1 = img/PIC1.png


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
