VITASDK = C:/VitaSDK
TARGET_LIB = libbox2d.a
OBJS       = Source/Collision/b2BroadPhase.o \
			Source/Collision/b2CollideCircle.o \
			Source/Collision/b2CollidePoly.o \
			Source/Collision/b2Distance.o \
			Source/Collision/b2PairManager.o \
			Source/Collision/b2Shape.o \
			Source/Common/b2BlockAllocator.o \
            Source/Common/b2Settings.o \
			Source/Common/b2StackAllocator.o \
			Source/Dynamics/b2Body.o \
			Source/Dynamics/b2ContactManager.o \
			Source/Dynamics/b2Island.o \
			Source/Dynamics/b2World.o \
			Source/Dynamics/b2WorldCallbacks.o \
			Source/Dynamics/Contacts/b2CircleContact.o \
			Source/Dynamics/Contacts/b2Conservative.o \
			Source/Dynamics/Contacts/b2Contact.o \
			Source/Dynamics/Contacts/b2ContactSolver.o \
			Source/Dynamics/Contacts/b2PolyAndCircleContact.o \
			Source/Dynamics/Contacts/b2PolyContact.o \
			Source/Dynamics/Joints/b2DistanceJoint.o \
			Source/Dynamics/Joints/b2GearJoint.o \
			Source/Dynamics/Joints/b2Joint.o \
			Source/Dynamics/Joints/b2MouseJoint.o \
			Source/Dynamics/Joints/b2PrismaticJoint.o \
			Source/Dynamics/Joints/b2PulleyJoint.o \
			Source/Dynamics/Joints/b2RevoluteJoint.o
INCLUDES   = INCLUDE

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
AR      = $(PREFIX)-ar
CFLAGS  = -Wl,-q -Wall -O3 -I$(INCLUDES)
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

all: $(TARGET_LIB)

debug: CFLAGS += -DDEBUG_BUILD
debug: all

$(TARGET_LIB): $(OBJS)
	$(AR) -rc $@ $^

clean:
	@rm -rf $(TARGET_LIB) $(OBJS)