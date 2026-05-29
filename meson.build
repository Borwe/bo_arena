project(
  'bo_arena',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1.1',
  default_options : [
    'warning_level=3',
    'c_std=c11'],
)

cc = meson.get_compiler('c')
ubsan = cc.find_library('ubsan', required: false)

test_options = ['b_sanitize=undefined']

dependencies = [ ubsan ]

bo_arena = declare_dependency(
  include_directories: ['./include']
)

meson.override_dependency('bo_arena', bo_arena)

arena_test = executable(
  'arena_test',
  'arena_test.c',
  override_options: test_options,
  dependencies : dependencies + bo_arena,
  install : true,
)

test('arena_test', arena_test, verbose: true)
