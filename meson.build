# meson setup build_sim --native-file toolchain.txt
project('glfw', 'cpp',
  version : '0.1')

glfw_dep = dependency('glfw3')
# imgui_dep = dependency('imgui')
imgui_dep = subproject('imgui', version: '>=1.91.0')
imgui = imgui_dep.get_variable('imgui_dep')


project = include_directories('.')
myFunc  = include_directories('/home/yves/DEV/cpp')

# Include Skia dependency here
# Declare an external dependency manually
freetype_dep = dependency('freetype2')
opengl_dep = dependency('gl', required: true)

skia_lib_path = '/home/yves/DEV/thirdparty/skia/skia/out/Release/'

skia_dep = declare_dependency(
    include_directories: [ include_directories('/home/yves/DEV/thirdparty/skia/skia/'),
    include_directories('/home/yves/DEV/thirdparty/skia/skia/include/core/'),
    ],
    link_args: ['-L' + skia_lib_path, '-lskia']  # Assuming the lib is 'libexample.a'
)


exe = executable('glfw', 'glfw.cc', 
                    dependencies : [ glfw_dep, imgui, 
                                      skia_dep, freetype_dep, opengl_dep],
                    include_directories : [ project, myFunc],
                    install : true)

test('basic', exe)
