# GLFW + ImGui (+ SKIA)

## Setup

This should compile with [meson](https://mesonbuild.com/index.html)

You need to install these dependencies

```bash
$ sudo apt-get install libglfw3-dev
```

You need to compile Skia by yourself
see [glfw_ship.cpp ](https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add)
or [Skia Graphics Engine](https://yduf.github.io/lib-skia/)

Once setup

To compile
```bash
$ meson setup build . 
$ meson compile -C build 
$ ./build/glfw
```