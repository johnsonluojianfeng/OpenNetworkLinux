###############################################################################
#
#
#
###############################################################################

LIBRARY := x86_64_cel_silverstone_dp
$(LIBRARY)_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/lib.mk
