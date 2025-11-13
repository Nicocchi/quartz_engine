#!/bin/bash

########################################
# TERM COLORS                          #
########################################
# Reset
NC='\033[0m'       # Text Reset

# Regular Colors
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

# Bold
BBlack='\033[1;30m'       # Black
BRed='\033[1;31m'         # Red
BGreen='\033[1;32m'       # Green
BYellow='\033[1;33m'      # Yellow
BBlue='\033[1;34m'        # Blue
BPurple='\033[1;35m'      # Purple
BCyan='\033[1;36m'        # Cyan
BWhite='\033[1;37m'       # White

# Underline
UBlack='\033[4;30m'       # Black
URed='\033[4;31m'         # Red
UGreen='\033[4;32m'       # Green
UYellow='\033[4;33m'      # Yellow
UBlue='\033[4;34m'        # Blue
UPurple='\033[4;35m'      # Purple
UCyan='\033[4;36m'        # Cyan
UWhite='\033[4;37m'       # White

########################################
# VARIABLES                            #
########################################
build_dir="build/win64"

mkdir -p $build_dir

########################################
# ENGINE                               #
########################################
printf "======= Compiling Engine =======\n"
rm -f $build_dir/quartz.*
libs="-luser32 -lopengl32 -lgdi32 -lengine/lib/glfw3dll -lengine/lib/freetype"
flags=""
warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations -Wno-switch"
includes="-Iengine/src/editor/ -Iengine/src/renderer -Iengine/src/"
vendor_includes="-Ivendor/ -Iengine/vendor/imgui/ -Ivendor/glfw"
src="engine/src/win32_main.cpp"
defines="-DSHOW_EDITOR=1"
target="quartz"
pch="-include-pch engine/src/pch.hpp.pch"

start_time=$(date +%s%N)

clang++ -std=c++17 $defines $includes $vendor_includes -g $src -o $build_dir/$target.exe $libs $warnings $pch

end_time=$(date +%s%N)
duration_ns=$((end_time - start_time))
duration_ms=$((duration_ns / 1000000))
if [ $duration_ms -ge 1000 ]; then
    printf "${Green} Engine compilation time: ${URed}${duration_ms} ms ${NC}\n\n"
else
    printf "${Green} Engine compilation time: ${UGreen}${duration_ms} ms ${NC}\n\n"
fi