project(
  'dehash',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

cc = meson.get_compiler('c')
ubsan = cc.find_library('ubsan', required: true)


dependencies = [ ubsan ]

c_args = ['-std=c11', '-Wall', '-Wextra']
test_args = ['-fsanitize=undefined']

exe = executable(
  'dehash',
  'dehash.c',
  dependencies : dependencies,
  c_args: c_args,
  install : true,
)

arena_test = executable(
  'arena_test',
  'arena_test.c',
  dependencies : dependencies,
  c_args: c_args + test_args,
  install : true,
)

test('arena_test', arena_test)
