
CURDIR := $(shell pwd)
TOPDIR  = $(CURDIR)/../..
include $(TOPDIR)/rules.mk

INCDIR  = $(INCSYS) $(INCBSP) $(INCLIBC) $(INCFINSH) \
		  ./lwip/src/include ./lwip/src/ ./lwip/src/ipv4
INCLUDE = $(foreach dir, $(INCDIR), -I$(dir))

CFLAGS += $(INCLUDE)
AFLAGS += $(INCLUDE)

SRC_COM   = $(wildcard lwip/*.c)
SRC_ARCH = $(wildcard lwip/arch/*.c)
SRC_CORE = $(wildcard lwip/core/*.c)
SRC_CORE_ipv4 = $(wildcard lwip/core/ipv4/*.c)
SRC_NETIF = $(wildcard lwip/netif/*.c)
SRC       = $(SRC_ARCH) $(SRC_CORE) $(SRC_NETIF) $(SRC_CORE_ipv4)

OBJ_ARCH = $(SRC_ARCH:.c=.o)
OBJ_CORE = $(SRC_CORE:.c=.o)
OBJ_NETIF = $(SRC_NETIF:.c=.o)
OBJ_CORE_ipv4  = $(SRC_CORE_ipv4:.c=.o)
OBJ       = $(OBJ_ARCH) $(OBJ_CORE) $(OBJ_NETIF) $(OBJ_CORE_ipv4)  
OBJ_COM   = $(SRC_COM:.c=.o)

.PHONY : all
all: $(LIBNET)

$(LIBNET): $(OBJ) $(OBJ_COM)
	$(AR) rcs $(LIBNET) $(OBJ) $(OBJ_COM)

%.o:	%.S
	$(CC) $(AFLAGS) -c -o $@ $<
%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr $(OBJ) $(OBJ_COM) $(LIBNET)