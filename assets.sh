########################################
# ASSETS & FILES                       #
########################################
printf "${Yellow}======= Assets =======${NC}\n"
build_dir="build/win64"
cp -n engine/lib/glfw3.dll $build_dir
cp -n engine/lib/freetype.dll $build_dir

printf "Copying assets...\n"
cp -r assets $build_dir

