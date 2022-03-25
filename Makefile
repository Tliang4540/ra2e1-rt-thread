#编译器
CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
CP = $(CROSS_COMPILE)objcopy
DP = $(CROSS_COMPILE)objdump
HEX = $(CP) -Oihex
BIN = $(CP) -Obinary -S
CPU = -mcpu=cortex-m23 -mthumb -Wall -O3
CFLAGS = $(CPU) -g
LDFLAGS = $(CPU) -TLink.ld

#输出目标文件名
target = rt_thread_msg
#输出目录
OUT_DIR = Out

#获得所有目录,作为搜索c文件和头文件
VPATH := $(shell find ./ -type d)
INC_DIR := $(addprefix -I , $(VPATH))

CFLAGS += $(INC_DIR)

#获得所有的.c和.S文件
C_SRC := $(shell find ./ -name *.c)
OBJ := $(C_SRC:.c=.o)
A_SRC := $(shell find ./ -name *.S)
OBJ += $(A_SRC:.S=.o)

OBJ := $(addprefix $(OUT_DIR)/, $(notdir $(OBJ)))

$(target).bin:$(target).elf
	$(BIN) $(OUT_DIR)/$(target).elf  $(OUT_DIR)/$(target).bin
	$(DP) -D -m arm $(OUT_DIR)/$(target).elf>$(OUT_DIR)/$(target).dis
	size $(OUT_DIR)/$(target).elf

$(target).elf:$(OBJ)
	$(CC) $(LDFLAGS) $^ -o $(OUT_DIR)/$(target).elf

$(OUT_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	rm -f Out/*
