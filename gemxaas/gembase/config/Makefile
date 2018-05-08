# $Id$

#########################################################################
# XDAQ Components for Distributed Data Acquisition                      #
# Copyright (C) 2000-2015, CERN.                                        #
# All rights reserved.                                                  #
# Authors: L. Orsini and D. Simelevicius                                #
#                                                                       #
# For the licensing terms see LICENSE.                                  #
# For the list of contributors see CREDITS.                             #
#########################################################################


##
#
#  Makefile for generic GEM setup machine configuration
#
##

#
# Packages to be built
#
BUILD_HOME:=$(shell pwd)/../../../..
INSTALL_PATH=/opt/$(Project)
ZONE_NAME=$(PackageName)

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=cmsgemos
PackageName=$(shell pwd | awk -F"/" '{split($$0,a,"/");  print a[NF-1]}')
SubPackageName=$(shell pwd | awk -F"/" '{split($$0,a,"/");  print a[NF-0]}')
PackageType=$(shell pwd | awk -F"/" '{split($$0,a,"/");  print a[NF-2]}')
Package=$(PackageType)/$(PackageName)/$(SubPackageName)

Summary=GEM DAQ machine configuration for $(ZONE_NAME) service

Description=This package provides machine configurations for cmsgemos on zone $(ZONE_NAME)

Link=https://gitlab.cern.ch/cms-gem-daq-project/cmsgemos

ifndef BUILD_VERSION
BUILD_VERSION=1
endif

PACKAGE_VER_MAJOR=1
PACKAGE_VER_MINOR=0
PACKAGE_VER_PATCH=0

_all: basic

default: basic

all:
	@echo PackagePath $(BUILD_HOME)/$(Project)/$(Package)
	@echo ${PACKAGE_VER_MAJOR}_${PACKAGE_VER_MINOR}_${PACKAGE_VER_PATCH} > $(PackageName).release
	@echo Project $(Project)
	@echo PackageName $(PackageName)
	@echo PackageType $(PackageType)
	@echo Package $(Package)
basic:
	echo ${PACKAGE_VER_MAJOR}_${PACKAGE_VER_MINOR}_${PACKAGE_VER_PATCH} > $(PackageName).release

_cleanall: clean

clean:
	-rm -f $(PackageName).release

include $(XDAQ_ROOT)/config/mfSetupRPM.rules