# 此工程的根目录
ROOT_DIR := ../..

# 特殊目录控制
# SDK_ROOT := /var/nrf52/nRF52_SDK
# TEMPLATE_PATH := /var/nrf52/template

# 配置项：主控芯片类型
# 可选：nrf52810 或 nrf52832
# 本项目实测硬件为 NRF52832，如果你确实使用的是 832，请保持如下配置；
# 若使用 810，请改回 nrf52810。
NRF_CHIP := nrf52832

# 配置文件文件名
CONFIG_H = config.h
CONFIG_H_DIR = .
SRC_FILES += keymap_plain.c

# 配置项：功能选项
# 关于更多的可配置项目，请参考doc目录下的相应文档
BOOTMAGIC_ENABLE = no	# 启用Bootmagic
BOOTCHECK_ENABLE = yes  # 启用Bootchek
EXTRAKEY_ENABLE = yes	# 启用媒体键功能
NKRO_ENABLE = no	# 蓝牙模式下无需USB全键无冲
USB_6KRO_ENABLE = no # 蓝牙模式下无需USB六键无冲

ONBOARD_CMSIS_DAP = no # 蓝牙模式下无需板载调试器（CH554）

########## STOP HERE ##########
# 除非你知道下面这些配置项是干什么的，否则不要去动它

INC_FOLDERS += .

APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

all: default

include $(APP_PROJ_DIR)/kbd.mk