project('pstack', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

# Meson is way more strict than this
DED_FLAGS = '-D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -Wclobbered'

FLAGS = [
  DED_FLAGS,

  # Logging
  '-D PSTACK_LOG',

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
  cpp_args : FLAGS,
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
