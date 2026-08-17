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
asan = cc.find_library('asan', required: false)

bo_arena_dep = declare_dependency(
  include_directories: ['./include']
)

test_dict = {'asan': [asan, ['b_sanitize=address']]}

if get_option('build_tests') == true

  if cc.get_id() == 'msvc' 
    message('msvc doesn\'t support libubsan')
  endif
  if cc.get_id() != 'msvc'
    test_dict+={'ubsan': [ubsan, ['b_sanitize=undefined']]}
  endif

  foreach k, v : test_dict
    arena_test = executable(
      'arena_test_'+k,
      'arena_test.c',
      override_options: v[1],
      dependencies : [v[0],  bo_arena],
    )

    test('arena_test_'+k, arena_test, verbose: true)
  endforeach
endif
