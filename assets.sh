########################################
# ASSETS & FILES                       #
########################################
printf "Copying assets =======\n"
build_dir="build/win64"
cp -n engine/lib/glfw3.dll $build_dir
cp -n engine/lib/freetype.dll $build_dir

cp -r assets $build_dir/assets
cp -r engine/src/shaders $build_dir/assets