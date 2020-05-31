#Path to .h files 
IDIR=include

#Path to .cpp files
SRC_DIR=src

#Path to .o files
OBJ_DIR=$(SRC_DIR)/obj

#Path to cross compiler libraries
RPI_LIB_DIR=/usr/arm-linux-gnueabihf/lib

#Path to wiringPi lib
WIR_PI_DIR=contrib/wiringPi/wiringPi

#Path to Queue lib
#QUEUE_LIB_DIR=contrib/QUEUE

#Path to RF24 lib
RF24_LIB_DIR=contrib/RF24

#Path to arduinolibs aes library
ARDUINO_AES_DIR=contrib/arduinolibs/libraries/Crypto

#Path to arduinolibs (encription library) makefile
ARDUINO_CRYP_MAKE=contrib/arduinolibs/host/Crypto


#All .cpp files
SRC = $(wildcard $(SRC_DIR)/*.cpp)

#All .o files
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

#All .h files
HEADERS=$(wildcard $(IDIR)/*.h)

PI_LIBS =-L$(RPI_LIB_DIR) -lm -lpthread -lrt -lcrypt -lssl -lcrypto -lz -ldl
RF24_LIBS = -L/home/demetri/Projects/Home-Automation-System/CommandCenter/contrib/wiringPi/wiringPi -lwiringPi
TARGET=arm-linux-gnueabihf-
CXX=$(TARGET)g++
CC=$(TARGET)gcc
CXXFLAGS=-std=c++17 -g -I$(IDIR) -I$(WIR_PI_DIR) -I$(ARDUINO_AES_DIR) \
		 -I$(RF24_LIB_DIR) -I/usr/arm-linux-gnueabihf/include/mysql 
LDFLAGS= -L$(WIR_PI_DIR) -lwiringPi $(PI_LIBS) -L$(ARDUINO_CRYP_MAKE) -lCrypto \
		 -L$(RF24_LIB_DIR) -lrf24 \
		 -L/usr/arm-linux-gnueabihf/include/mysql -lmysqlclient 
EXE=CommandCenter

.PHONY: all clean wiringPi arduinolibs

#debug variable prints
##$(warning $(HEADERS))
##$(warning $(OBJ))
##$(warning $(SRC))
#$(warning $(LDFLAGS))

all: $(EXE)

$(EXE): $(OBJ) | wiringPi arduinolibs rf24
ifeq ($(TARGET), arm-linux-gnueabihf-)
	ln -s ~/Projects/Home-Automation-System/CommandCenter/$(WIR_PI_DIR)/libwiringPi.so* ~/Projects/Home-Automation-System/CommandCenter/$(WIR_PI_DIR)/libwiringPi.so
	$(CXX) -fno-stack-protector -o $@ $^ $(LDFLAGS)
	rm ~/Projects/Home-Automation-System/CommandCenter/$(WIR_PI_DIR)/libwiringPi.so
else
	$(CXX) -o $@ $^ $(LDFLAGS)
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

wiringPi:
ifeq ($(TARGET), arm-linux-gnueabihf-)
		make -C $(WIR_PI_DIR) CC=$(CC) LIBS=$(PI_LIBS)
else
		make -C $(WIR_PI_DIR)
endif

arduinolibs:
ifeq ($(TARGET), arm-linux-gnueabihf-)
		make -C $(ARDUINO_CRYP_MAKE) CXX=$(CXX) LIBS=$(PI_LIBS)
else
		make -C $(ARDUINO_CRYP_MAKE)
endif

rf24:
ifeq ($(TARGET), arm-linux-gnueabihf-)
		cd contrib/RF24 && ./configure --soc=BCM2835 \
			--extra-ldflags=-L/home/demetri/Projects/Home-Automation-System/CommandCenter/contrib/wiringPi/wiringPi --extra-cflags=-I/home/demetri/Projects/Home-Automation-System/CommandCenter/contrib/wiringPi/wiringPi
		ln -s ~/Projects/Home-Automation-System/CommandCenter/$(WIR_PI_DIR)/libwiringPi.so* ~/Projects/Home-Automation-System/CommandCenter/$(WIR_PI_DIR)/libwiringPi.so
		make -C $(RF24_LIB_DIR) CXX=$(CXX) CC=$(CC) SHARED_LINKER_LIBS=-pthread SHARED_LINKER_LIBS+=$(RF24_LIBS) LIBNAME=librf24.so
		rm ~/Projects/Home-Automation-System/CommandCenter/$(WIR_PI_DIR)/libwiringPi.so
else
		make -C $(RF24_LIB_DIR)
endif

clean:
	make -C $(WIR_PI_DIR) clean
	make -C $(ARDUINO_CRYP_MAKE) clean
	make -C $(ARDUINO_CRYP_MAKE) clean
	rm $(OBJ_DIR)/*.o
	rm $(EXE)
	clear
