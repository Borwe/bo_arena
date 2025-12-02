project(
  'bo_arena',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

cc = meson.get_compiler('c')
ubsan = cc.find_library('ubsan')


dependencies = [ ubsan ]

c_args = ['-std=c11', '-Wall', '-Wextra']
test_args = ['-fsanitize=undefined']

bo_arena = declare_dependency(
  compile_args: c_args,
  include_directories: ['./include']
)

meson.override_dependency('bo_arena', bo_arena)

arena_test = executable(
  'arena_test',
  'arena_test.c',
  dependencies : dependencies + bo_arena,
  c_args: c_args + test_args,
  install : true,
)

test('arena_test', arena_test, verbose: true)
