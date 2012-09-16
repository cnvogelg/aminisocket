# common.mk

# AMIGA files needed:
#
# WB_PATH - base directory of Workbench 3.1 install (c is taken from there)
# SC_PATH - base directory of SAS C compilter 6.58 installation
# NETSDK_PATH - AmiTCP SDK path
AMI_BASE?=$(HOME)/Projects/amitools_data
WB_PATH=$(BASE)/wb310
SC_PATH=$(BASE)/sc
NETSDK_PATH=$(BASE)/netsdk

# path of vamos
VAMOS?=vamos

# how to run vamos
VAMOS_RUN=$(VAMOS) -c $(BASE_DIR)/vamosrc

INSTALL_DIR=$(BASE_DIR)/install_dir
