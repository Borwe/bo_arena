project(
  'dehash',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
]

c_args = ['-std=c11']

exe = executable(
  'dehash',
  'dehash.c',
  dependencies : dependencies,
  c_args: c_args,
  install : true,
)

test('basic', exe)
