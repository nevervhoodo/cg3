CXX = g++
LD = g++
CXXFLAGS = -g -Wall -O0 -std=c++0x

# library includes
CXXFLAGS += -Dcimg_use_png
LDFLAGS = -L/usr/X11R6 -lpthread -lX11 -lpng -lOpenCL

TARGET = raytracer
CL_TARGET = clraytracer
BUILD_SHARED_DIR = build
BUILD_CL_DIR = build
BUILD_NOCL_DIR = build
TARGET_DIR = bin
SRC_DIR = src
CL_SRC_DIR = src/cl
NOCL_SRC_DIR = src/nocl

# libraries
LIB_DIR = external
CXXFLAGS += -I$(LIB_DIR)/CImg/

_TARGET = $(TARGET_DIR)/$(TARGET)
_CL_TARGET = $(TARGET_DIR)/$(CL_TARGET)
_OBJS = $(patsubst %.cpp, %.o, $(patsubst $(SRC_DIR)/%, $(BUILD_SHARED_DIR)/%, $(wildcard $(SRC_DIR)/*.cpp)))
_CL_OBJS = $(patsubst %.cpp, %.o, $(patsubst $(CL_SRC_DIR)/%, $(BUILD_CL_DIR)/%, $(wildcard $(CL_SRC_DIR)/*.cpp)))
_NOCL_OBJS = $(patsubst %.cpp, %.o, $(patsubst $(NOCL_SRC_DIR)/%, $(BUILD_NOCL_DIR)/%, $(wildcard $(NOCL_SRC_DIR)/*.cpp)))

all: $(_TARGET) $(_CL_TARGET)

cl: $(_CL_TARGET)

nocl: $(_TARGET)

$(_TARGET): $(_OBJS) $(_NOCL_OBJS) $(TARGET_DIR)
	$(LD) -o $@ $(_OBJS) $(_NOCL_OBJS) $(LDFLAGS)

$(_CL_TARGET): $(_OBJS) $(_CL_OBJS) $(TARGET_DIR)
	$(LD) -o $@ $(_OBJS) $(_CL_OBJS) $(LDFLAGS)

$(BUILD_SHARED_DIR)/%.o: $(SRC_DIR)/%.cpp 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD_CL_DIR)/%.o: $(CL_SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD_NOCL_DIR)/%.o: $(NOCL_SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(TARGET_DIR):
	mkdir $(TARGET_DIR)

clean:
	rm $(_TARGET) build/* -f
