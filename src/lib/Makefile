#
# Tiny C Compiler Makefile for libupp1.a
#

TOP = ../..
include $(TOP)/Makefile
VPATH = $(TOPSRC)/src/lib $(TOPSRC)/src/win32/lib
T = $(or $(CROSS_TARGET),$(NATIVE_TARGET),unknown)
X = $(if $(CROSS_TARGET),$(CROSS_TARGET)-)
BIN = $(TOP)/build/$(X)libupp1.a

XTCC ?= $(TOP)/bin/$(X)upp$(EXESUF)
XCC = $(XTCC)
XAR = $(XTCC) -ar
XFLAGS-unx = -B$(TOPSRC)/src
XFLAGS-win = -B$(TOPSRC)/src/win32 -I$(TOPSRC)/src/include
XFLAGS = $(XFLAGS$(XCFG))
XCFG = $(or $(findstring -win,$T),-unx)

# in order to use gcc, type: make <target>-libupp1-usegcc=yes
arm-libupp1-usegcc ?= no

ifeq "$($(T)-libupp1-usegcc)" "yes"
 XCC = $(CC)
 XAR = $(AR)
 XFLAGS = $(CFLAGS) -fPIC
endif

# only for native compiler
BCHECK_O = bcheck.o

ifeq ($(CONFIG_musl)$(CONFIG_uClibc),yes)
 BCHECK_O =
endif

ifdef CONFIG_OSX
 XFLAGS += -D_ANSI_SOURCE
endif

I386_O = libupp1.o alloca86.o alloca86-bt.o
X86_64_O = libupp1.o alloca86_64.o alloca86_64-bt.o
ARM_O = libupp1.o armeabi.o alloca-arm.o armflush.o
ARM64_O = lib-arm64.o
WIN_O = crt1.o crt1w.o wincrt1.o wincrt1w.o dllcrt1.o dllmain.o

OBJ-i386 = $(I386_O) $(BCHECK_O)
OBJ-x86_64 = $(X86_64_O) va_list.o $(BCHECK_O)
OBJ-x86_64-osx = $(X86_64_O) va_list.o
OBJ-i386-win32 = $(I386_O) chkstk.o bcheck.o $(WIN_O)
OBJ-x86_64-win32 = $(X86_64_O) chkstk.o bcheck.o $(WIN_O)
OBJ-arm64 = $(ARM64_O)
OBJ-arm = $(ARM_O)
OBJ-arm-fpa = $(ARM_O)
OBJ-arm-fpa-ld = $(ARM_O)
OBJ-arm-vfp = $(ARM_O)
OBJ-arm-eabi = $(ARM_O)
OBJ-arm-eabihf = $(ARM_O)
OBJ-arm-wince = $(ARM_O) $(WIN_O)

$(BIN) : $(patsubst %.o,$(TOP)/build/$(X)%.o,$(OBJ-$T))
	$(XAR) rcs $@ $^

$(TOP)/build/$(X)%.o : %.c
	@mkdir -p $(TOP)/build
	$(XCC) -c $< -o $@ $(XFLAGS)

$(TOP)/build/$(X)bcheck.o : bcheck.c
	@mkdir -p $(TOP)/build
	$(XCC) -c $< -o $@ $(XFLAGS) -D__FreeBSD__

$(TOP)/build/$(X)%.o : %.S
	@mkdir -p $(TOP)/build
	$(XCC) -c $< -o $@ $(XFLAGS)

$(TOP)/build/$(X)crt1w.o : crt1.c
$(TOP)/build/$(X)wincrt1w.o : wincrt1.c

clean :
	rm -f *.a *.o $(BIN) $(TOP)/build/$(X)*.o
