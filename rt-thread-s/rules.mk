#/*
# * File      : cpu.c
# * This file is part of RT-Thread RTOS
# * COPYRIGHT (C) 2006, RT-Thread Develop Team
# *
# * The license and distribution terms for this file may be
# * found in the file LICENSE in this distribution or at
# * http://openlab.rt-thread.com/license/LICENSE
# *
# * Change Logs:
# * Date           Author       Notes
# * 2011-05-29     swkyer       first version
# */

ARCH = arm
CPU  = s3c24x0
#s3c24x0
#s3c6410
BSP  = mini2440
#mini2440
#tiny6410
TEXTBASE = 0x00008000

# cross compile
CROSS_COMPILE  = arm-linux
TOOLCHAIN_PATH = 

CC      = $(CROSS_COMPILE)-gcc
LD      = $(CROSS_COMPILE)-ld
AR      = $(CROSS_COMPILE)-ar
OBJCOPY = $(CROSS_COMPILE)-objcopy
OBJDUMP = $(CROSS_COMPILE)-objdump
STRIP   = $(CROSS_COMPILE)-strip
READELF = $(CROSS_COMPILE)-readelf

# platform depent flags
CPUFLAGS = -mcpu=arm1176jzf-s
PLATFORM_CFLAGS = $(CPUFLAGS) -fno-strict-aliasing -fno-common -ffixed-r8 -msoft-float

# rtt-elements
P_ARCH    = $(TOPDIR)/libcpu/$(ARCH)/$(CPU)
P_BSP     = $(TOPDIR)/bsp/$(BSP)
P_SYS     = $(TOPDIR)/src
P_LIBC    = $(TOPDIR)/components/libc/minilibc
P_PTHREAD = $(TOPDIR)/components/pthread
P_FINSH   = $(TOPDIR)/components/finsh
P_DFS     = $(TOPDIR)/components/dfs
P_GUI     = $(TOPDIR)/components/rtgui
P_NET     = $(TOPDIR)/components/net/lwip/src

INCARCH   = $(P_ARCH)
INCBSP    = $(P_BSP)
INCSYS    = $(TOPDIR)/include
INCLIBC   = $(P_LIBC)
INCPTHREAD= $(P_PTHREAD)
INCFINSH  = $(P_FINSH)
INCDFS    = $(P_DFS)/include
INCGUI    = $(P_GUI)/include
INCNET    = $(P_NET)/include
INCNETIPV4= $(P_NET)/include/ipv4
INCNETSRC = $(P_NET)
INCNETARCH = $(P_NET)/arch/include

LIBARCH   = $(TOPDIR)/lib$(CPU).a
LIBBSP    = $(TOPDIR)/lib$(BSP).a
LIBSYS    = $(TOPDIR)/libsys.a
LIBC      = $(TOPDIR)/libc.a
LIBPTHREAD= $(TOPDIR)/libpthread.a
LIBFINSH  = $(TOPDIR)/libfinsh.a
LIBDFS    = $(TOPDIR)/libdfs.a
#LIBGUI    = $(TOPDIR)/librtgui.a
LIBNET    = $(TOPDIR)/libnet.a

TARGET    = $(TOPDIR)/rtt-$(BSP)

# compile flags
ENDIAN  = 
CFLAGS  = -g -O2 -Wall -Wno-unknown-pragmas -nostdinc -nostdlib -fno-builtin $(PLATFORM_CFLAGS)
AFLAGS  = $(CFLAGS) -D__ASSEMBLY__ -DTEXT_BASE=$(TEXTBASE)

LIB_PATH = $(TOOLCHAIN_PATH)/$(CROSS_COMPILE)/lib
LDSCRIPT = $(INCBSP)/link.lds
LDFLAGS  = $(CPUFLAGS) -nostartfiles -T $(LDSCRIPT) -Ttext $(TEXTBASE) -L$(LIB_PATH)
