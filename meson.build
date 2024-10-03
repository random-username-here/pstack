project('pstack', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

FLAGS = [

  #---- Warnings, generated only by CLANG.
  # GCC does not complain about them.

  # This is used ONLY for macros, do not panic about it.
  # But this rule is a good one for variable/function names,
  # because some assemblers cannot tolerate symbols
  # with those names (as I was told by GCC docs).
  '-Wno-dollar-in-identifier-extension',
]

incdir = include_directories('include')

# Setup arrays to collect filenames into
sources = []
test_files = []

# Call setup for subdirs
# subdir('include') # No file for now, because it is not needed
subdir('src')
subdir('test')

# Main executable

exe = executable(
  'pstack-demo',
  [main_file] + sources,
  include_directories : incdir,
  c_args : FLAGS,
  install : true
)

# Executable for tests

#tests = executable(
  #  'tests',
  #test_files + sources,
  #c_args : FLAGS,
  #include_directories : incdir,
  #)

#test('normal-tests', tests)
