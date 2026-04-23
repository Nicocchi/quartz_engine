#!/bin/bash
set -euo pipefail

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
# OPTIONS                              #
########################################
# Usage:
#   ./build.sh                  → incremental build, header tracking ON
#   ./build.sh --no-headers     → incremental build, header tracking OFF
#   ./build.sh --full           → force rebuild everything, header tracking ON
#   ./build.sh --full --no-headers → force rebuild everything, tracking OFF

TRACK_HEADERS=1
FORCE_REBUILD=0

for arg in "$@"; do
    case "$arg" in
        --no-headers)  TRACK_HEADERS=0 ;;
        --full)        FORCE_REBUILD=1 ;;
        *)             echo "Unknown option: $arg"; echo "Usage: $0 [--no-headers] [--full]"; exit 1 ;;
    esac
done

if [[ $TRACK_HEADERS -eq 1 ]]; then
    printf "${Yellow}  Header tracking: ${UBlue}ON${NC}\n"
else
    printf "${Yellow}  Header tracking: ${UBlue}OFF${NC}\n"
fi
if [[ $FORCE_REBUILD -eq 1 ]]; then
    printf "${Yellow}  Mode: ${UBlue}full rebuild${NC}\n"
else
    printf "${Yellow}  Mode: ${UBlue}incremental${NC}\n"
fi
printf "\n"

########################################
# CONFIG                               #
########################################
build_dir="build/win64"
mkdir -p "$build_dir"
mkdir -p "$build_dir/obj"
mkdir -p "$build_dir/vendor_objs"

libs="-luser32 -lopengl32 -lgdi32 -lengine/lib/glfw3dll"
flags="-mavx2"
warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations -Wno-switch"
includes="-Iengine/src/ -Ienigne/vendor/ -Ienigne/vendor/freetype -Ienigne/vendor/freetype/config/ -Ienigne/vendor/glad/ -Ienigne/vendor//KHR/ -Ienigne/vendor/glfw/ -Ienigne/vendor/imgui/ -Ienigne/vendor/raylib/"
defines="-DSHOW_EDITOR=1 -DENGINE_COMP=1"
target="quartz"
pch_src="engine/src/pch.hpp"
pch_out="engine/src/pch.hpp.pch"
pch="-include-pch $pch_out"

engine_src=(
    "engine/src/win32_main.cpp"
    "engine/src/editor.cpp"
    "engine/src/renderer_2d.cpp"
    "engine/src/save.cpp"
    "engine/src/texture_2d.cpp"
    "engine/src/window.cpp"
)

vendor_src=(
    "engine/vendor/glad/glad.c"
    "engine/vendor/imgui/imgui.cpp"
    "engine/vendor/imgui/imgui_draw.cpp"
    "engine/vendor/imgui/imgui_impl_glfw.cpp"
    "engine/vendor/imgui/imgui_impl_opengl3.cpp"
    "engine/vendor/imgui/imgui_stdlib.cpp"
    "engine/vendor/imgui/imgui_tables.cpp"
    "engine/vendor/imgui/imgui_widgets.cpp"
)

########################################
# HELPERS                              #
########################################

# With header tracking ON:  checks src mtime, obj mtime, and all headers
#                            listed in the depfile.
# With header tracking OFF: checks src mtime and obj mtime only.
# FORCE_REBUILD=1 always returns 0 (dirty).
needs_rebuild() {
    local src="$1" obj="$2" dep="${3:-}"

    [[ $FORCE_REBUILD -eq 1 ]] && return 0
    [[ ! -f "$obj" ]]          && return 0
    [[ "$src" -nt "$obj" ]]    && return 0

    if [[ $TRACK_HEADERS -eq 1 && -n "$dep" && -f "$dep" ]]; then
        local dep_content
        dep_content=$(sed 's/\\//g' "$dep")
        dep_content="${dep_content#*:}"
        for header in $dep_content; do
            [[ -z "$header" ]]  && continue
            [[ ! -f "$header" ]] && return 0
            [[ "$header" -nt "$obj" ]] && return 0
        done
    fi

    return 1
}

# Compile one TU in the background.
# Emits a depfile only when header tracking is ON (no point writing
# .d files that will never be read).
# Usage: compile_tu <src> <obj> <dep> <extra_flags...>
compile_tu() {
    local src="$1" obj="$2" dep="$3"
    shift 3
    echo "  [CC] $src"
    local mmd_flags=""
    [[ $TRACK_HEADERS -eq 1 ]] && mmd_flags="-MMD -MF $dep"
    clang++ -std=c++17 $flags $includes -g \
        $mmd_flags \
        "$@" -c "$src" -o "$obj" $warnings
}

start_timer() {
    date +%s%N
}

print_elapsed() {
    local start_time="$1"
    local label="${2:-Build}"
    local end_time
    end_time=$(date +%s%N)
    local duration_ms=$(( (end_time - start_time) / 1000000 ))

    if [[ $duration_ms -ge 1000 ]]; then
        printf "\n${Green} %s compilation time: ${URed}%d ms${NC}\n\n" "$label" "$duration_ms"
    else
        printf "\n${Green} %s compilation time: ${UGreen}%d ms${NC}\n\n" "$label" "$duration_ms"
    fi
}

########################################
# PCH                                  #
########################################
printf "${Yellow}======= PCH =======${NC}\n"
pch_dep="$build_dir/obj/pch.hpp.d"

if needs_rebuild "$pch_src" "$pch_out" "$pch_dep"; then
    printf "${Yellow}  [PCH] ${NC}rebuilding $pch_src\n"
    local_mmd=""
    [[ $TRACK_HEADERS -eq 1 ]] && local_mmd="-MMD -MF $pch_dep"
    clang++ -std=c++17 $flags $defines $includes \
        $local_mmd \
        -x c++-header "$pch_src" -o "$pch_out" $warnings
else
    printf "${Green}  [PCH] up to date, skipping${NC}\n"
fi

########################################
# VENDOR                               #
########################################
# Vendor code is treated as immutable — Only check src vs obj mtime,
# no header scanning. Vendor headers essentially never change, unless
# force rebuild is checked.
printf "${Yellow}======= Vendor =======${NC}\n"
vendor_objs=()
vendor_pids=()

for src in "${vendor_src[@]}"; do
    obj="$build_dir/vendor_objs/$(basename "${src%.*}").o"
    vendor_objs+=("$obj")
    # Vendor always uses src-vs-obj only — no header tracking regardless of mode
    if [[ $FORCE_REBUILD -eq 1 || ! -f "$obj" || "$src" -nt "$obj" ]]; then
        printf "${Yellow}  [VENDOR] ${NC}$src\n"
        clang++ -std=c++17 $flags $includes -g -c "$src" -o "$obj" $warnings &
        vendor_pids+=($!)
    fi
done

for pid in "${vendor_pids[@]}"; do
    wait "$pid" || { printf "${Red}Vendor compile failed (pid $pid)${NC}\n"; exit 1; }
done
printf "${Green}  Vendor up to date ${NC}\n"

########################################
# ENGINE                               #
########################################
printf "${Yellow}======= Engine =======${NC}\n"
t=$(start_timer)
engine_objs=()
engine_pids=()
any_engine_rebuilt=0

for src in "${engine_src[@]}"; do
    base=$(basename "${src%.*}")
    obj="$build_dir/obj/$base.o"
    dep="$build_dir/obj/$base.d"
    engine_objs+=("$obj")

    if needs_rebuild "$src" "$obj" "$dep"; then
        any_engine_rebuilt=1
        compile_tu "$src" "$obj" "$dep" $defines $pch &
        engine_pids+=($!)
    fi
done

for pid in "${engine_pids[@]}"; do
    wait "$pid" || { printf "${Red}Engine compile failed (pid $pid)${NC}\n"; exit 1; }
done

if [[ $any_engine_rebuilt -eq 1 ]]; then
    printf "${Yellow}  [LINK] $target.exe${NC}\n"
    clang++ "${engine_objs[@]}" "${vendor_objs[@]}" \
        -o "$build_dir/$target.exe" $libs
else
    printf "${Green}  Engine up to date, skipping link${NC}\n"
fi

print_elapsed $t "Game"

########################################
# GAME                                 #
########################################
printf "${Yellow}======= Game =======${NC}\n"
t=$(start_timer)
game_src="engine/src/game.cpp"
game_obj="$build_dir/obj/game.o"
game_dep="$build_dir/obj/game.d"
game_target="game_"
timestamp=$(date +%s)

if needs_rebuild "$game_src" "$game_obj" "$game_dep"; then
    printf "${Yellow}  [CC] ${NC}$game_src\n"
    rm -f $build_dir/$game_target*
    local_mmd=""
    [[ $TRACK_HEADERS -eq 1 ]] && local_mmd="-MMD -MF $game_dep"
    clang++ -std=c++17 $flags $includes -g \
        $local_mmd \
        "$game_src" -shared -o "$build_dir/game_$timestamp.dll" $warnings
    mv "$build_dir/game_$timestamp.dll" "$build_dir/game.dll"
    clang++ -std=c++17 $flags $includes -g \
        -c "$game_src" -o "$game_obj" $warnings
else
    printf "${Green}  Game up to date, skipping${NC}\n"
fi

print_elapsed $t "Game"

########################################
# ASSETS                               #
########################################
printf "${Yellow}======= Assets =======${NC}\n"
cp -n engine/lib/glfw3.dll $build_dir
cp -n engine/lib/freetype.dll $build_dir

printf "Copying assets...\n"
cp -r assets $build_dir

printf "${Yellow}======= Done =======${NC}\n"