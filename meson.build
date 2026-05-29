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
asan = cc.find_library('absan', required: false)

bo_arena = declare_dependency(
  include_directories: ['./include']
)

meson.override_dependency('bo_arena', bo_arena)

foreach k, v : {'ubsan': [ubsan, ['b_sanitize=undefined']],
  'asan': [asan, ['b_sanitize=address']]} 
  arena_test = executable(
    'arena_test_'+k,
    'arena_test.c',
    override_options: v[1],
    dependencies : [v[0],  bo_arena],
  )

  test('arena_test_'+k, arena_test, verbose: true)
endforeach

