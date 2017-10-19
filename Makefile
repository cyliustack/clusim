C_NONE=$(shell echo -e "\033[0m")
C_RED=$(shell echo -e "\033[31m")
C_GREEN=$(shell echo -e "\033[32m")
C_ORANGE=$(shell echo -e "\033[33m")
C_BLUE=$(shell echo -e "\033[34m")
C_PURPLE=$(shell echo -e "\033[35m")
C_CYAN=$(shell echo -e "\033[36m")
C_LIGHT_GRAY=$(shell echo -e "\033[37m")
NS3_BUILD :=  ./ns3/ns-3.26/build
INC_FLAGS := -I./include -I${NS3_BUILD}
TARGET := clusim
OBJS := main.o
LIB_USER := 
LIB_USER_OBJS :=  
OPTIONS := -g -std=c++14 -W -Wunused-variable 
CFLAGS := $(OPTIONS) $(INC_FLAGS)
CXXFLAGS := $(OPTIONS) $(INC_FLAGS)
NS3_LINKFLAGS := -L./${NS3_BUILD}\
					-lns3.26-stats-debug \
					-lns3.26-aodv-debug \
					-lns3.26-nix-vector-routing-debug \
					-lns3.26-internet-debug \
					-lns3.26-network-debug \
					-lns3.26-stats-debug \
					-lns3.26-core-debug \
					-lns3.26-applications-debug \
					-lns3.26-point-to-point-debug \
					-lns3.26-csma-debug \
					-lns3.26-csma-layout-debug \
					-lns3.26-point-to-point-layout-debug \
					-lns3.26-traffic-control-debug \
					-lns3.26-flow-monitor-debug \
					-lns3.26-bridge-debug \
					-lns3.26-netanim-debug \
					-lns3.26-mpi-debug \
					-lns3.26-wifi-debug \
					-lns3.26-propagation-debug \
					-lns3.26-energy-debug \
					-lns3.26-mobility-debug \
					-lns3.26-spectrum-debug \
					-lns3.26-lte-debug \
					-lns3.26-antenna-debug \
					-lns3.26-fd-net-device-debug \
					-lns3.26-buildings-debug \
					-lns3.26-virtual-net-device-debug \
					-lns3.26-config-store-debug \
					-lns3.26-wimax-debug \
					-lns3.26-uan-debug

RUN_ENV := LD_LIBRARY_PATH=./${NS3_BUILD}:$LD_LIBRARY_PATH

LINKFLAGS := -lconfig++ -lpcap $(NS3_LINKFLAGS)
all	: $(TARGET) $(LIB_USER)
$(TARGET): $(OBJS) $(LIB_USER)
	@$(CXX) -o $@ $(OBJS) $(LIB_USER) $(LINKFLAGS)
	@echo -e "$(C_ORANGE)Generated	$@$(C_NONE)"
	@echo -e "$(C_ORANGE)Generated	$@$(C_NONE)"
$(LIB_USER):	$(LIB_USER_OBJS)
	@ar -rcs $@ $^
	@echo -e "$(C_GREEN)LINK	$@	by	$^$(C_NONE)"
%.o	:	%.c
	@$(CC) -c -fPIC $(CFLAGS) $< -o $@
	@echo -e "$(C_GREEN)CC	$@$(C_NONE)"
%.o	:	%.cpp ../include/ppacket.hpp
	@$(CXX) -c -fPIC $(CXXFLAGS) $< -o $@
	@echo -e "$(C_GREEN)CXX	$@$(C_NONE)"

run: $(TARGET)
#	export LD_LIBRARY_PATH=../ns3/ns-3.26/build:$LD_LIBRARY_PATH	
	echo "run!"
	$(RUN_ENV) ./clusim examples/default.cfg
debug: $(TARGET)
	$(RUN_ENV) gdb -args ./clusim default.cfg
	echo "debug!"
install: uninstall
	echo "install"
uninstall:
	echo "rm something for uninstall"
clean:
	@rm $(TARGET) $(LIB_USER) *.o
	@echo -e "$(C_BLUE)Removed all TARGET and objects$(C_NONE)"
