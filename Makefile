LIBS=-luser32 -lopengl32 -lgdi32 -lengine/lib/glfw3dll -lengine/lib/freetype
FLAGS=
WARNINGS=
INCLUDES=-Ienigne/src
VENDOR_INC=-Ivendor/ -Iengine/vendor/imgui/ -Ivendor/glfw
SRC=engine/src/win32_main.cpp
DEFINES=
TARGET=quartz
PCH=-include-pch engine/src/pch.hpp.pch

CXX=clang++
CXX_STD=-std=c++17

BUILD_DIR=build/win64

TIMESTAMP := $(shell date +"%s")

.PHONY: engine game

engine:
	rm -rf $(BUILD_DIR)/$(TARGET).*
	$(CXX) $(CXX_STD) $(DEFINES) $(INCLUDES) $(VENDOR_INC) -g $(SRC) -o $(BUILD_DIR)/$(TARGET).exe $(LIBS) $(WARNINGS) $(PCH)

game:
	rm -rf $(BUILD_DIR)/game*
	$(CXX) $(CXX_STD) $(INCLUDES)  -g engine/src/game.cpp -shared -o $(BUILD_DIR)/game_$(TIMESTAMP).dll $(WARNINGS)
	mv $(BUILD_DIR)/game_$(TIMESTAMP).dll $(BUILD_DIR)/game.dll
