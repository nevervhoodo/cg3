CXX = g++
LD = g++
CXXFLAGS = -g -Wall -O3 -std=c++0x -fopenmp 

# library includes
CXXFLAGS += -Dcimg_use_png
LDFLAGS = -L/usr/X11R6 -lpthread -lX11 -lpng -fopenmp

TARGET = raytracer
BUILD_DIR = build
TARGET_DIR = bin
SRC_DIR = src

# libraries
LIB_DIR = external
CXXFLAGS += -I$(LIB_DIR)/CImg/

_TARGET = $(TARGET_DIR)/$(TARGET)
_OBJS = $(patsubst %.cpp, %.o, $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(wildcard $(SRC_DIR)/*.cpp)))

all: $(_TARGET)

$(_TARGET): $(_OBJS) $(TARGET_DIR)
	$(LD) -o $@ $(_OBJS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(TARGET_DIR):
	mkdir $(TARGET_DIR)

clean:
	rm $(_TARGET) build/* -f
