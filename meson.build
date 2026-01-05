project(
        'pyalpm',
        'c',
        version: '0.11.0',
        license: 'GPL-3-or-later',
        default_options: ['warning_level=2', 'c_std=c99'],
        meson_version: '>= 1.8.0',
)

add_global_arguments('-Wno-incompatible-pointer-types', language : 'c')


# Dependencies
python = import('python').find_installation('python3', pure: false)
python_dep = python.dependency()
libalpm_dep = dependency('libalpm')

common_c_args = [
    '-DVERSION="@0@"'.format(meson.project_version()),
]


subdir('src')
subdir('pycman')

# Test
build_dir = meson.current_build_dir() / 'src'
test_dir = meson.current_source_dir() / 'test'
test_args = [test_dir]

if get_option('b_coverage')
    test_args = [test_dir, '--junitxml=report.xml', '--cov-report=xml']
endif

test(
  'integration tests', 
  find_program('pytest'),
  args: test_args,
  env: { 'PYTHONPATH': '@0@:@1@'.format(build_dir, meson.current_source_dir()) },
)

input_dir = meson.current_source_dir() / 'doc'
output_dir = meson.current_build_dir() / 'html'
output_file = output_dir / 'index.html'
custom_target('doc',
      output: 'index.html',
      input: files('doc/conf.py'),
      command: [find_program('sphinx-build'), '-b', 'html', input_dir, output_dir],
)
