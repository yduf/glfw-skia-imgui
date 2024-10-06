# GLFW + ImGui (+ SKIA)

Rational:
- [**GLFW**](https://www.glfw.org/docs/latest/quick.html) provide the windows layer
- [**Skia**](https://yduf.github.io/lib-skia/) provide a 2D vector layer (with far more advanced feature than sfml)
- [**ImGui**](https://yduf.github.io/imgui/) provide an interactive GUI on top of that.

Should be ideal for some basic to advanced visualisation without requiring 3D.

[**Cairo**](https://yduf.github.io/lib-cairo/) could have been an option, but Cairo does not natively support opengl as rendered (you have to manuall render and then export to a texture).

### Setup

This should compile with [meson](https://mesonbuild.com/index.html)

You need to install these dependencies

```bash
$ sudo apt-get install libglfw3-dev
```

You need to compile Skia by yourself see  
- [glfw_ship.cpp ](https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add)
- or [Skia Graphics Engine](https://yduf.github.io/lib-skia/)

Once setup

### To compile
```bash
$ meson setup build . 
$ meson compile -C build 
$ ./build/glfw
```