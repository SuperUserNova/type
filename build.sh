rm -rf build/
meson setup build
ninja -C build
mv ./build/novatype ./exec/
./exec/novatype
