# https://github.com/epezent/implot?tab=readme-ov-file#integration
project('implot', 'cpp')

implot_sources = files(
  'implot.cpp',
  'implot_items.cpp',
  'implot_demo.cpp',  # optional
)

implot_lib = static_library(
  'implot',
  implot_sources,
  include_directories: include_directories('.'),
  dependencies: [dependency('imgui', fallback: ['imgui', 'imgui_dep'])],
)

implot_dep = declare_dependency(
  link_with: implot_lib,
  include_directories: include_directories('.'),
)

meson.override_dependency('implot', implot_dep)

