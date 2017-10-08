PCAPPLUSPLUS_HOME := /home/centos/PcapPlusPlus

### COMMON ###

# includes
PCAPPP_INCLUDES := -I$(PCAPPLUSPLUS_HOME)/Dist/header

# libs dir
PCAPPP_LIBS_DIR := -L$(PCAPPLUSPLUS_HOME)/Dist

# libs
PCAPPP_LIBS := -lPcap++ -lPacket++ -lCommon++

# post build
PCAPPP_POST_BUILD :=

# build flags
PCAPPP_BUILD_FLAGS :=

### LINUX ###

# includes
PCAPPP_INCLUDES += -I/usr/include/netinet

# libs
PCAPPP_LIBS += -lpcap -lpthread


