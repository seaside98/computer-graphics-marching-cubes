def Settings( **kwargs ):
  return {
    'flags': [ '-x', 'c++', '-Wall', '-Wextra', '-Werror', '-I../nanogui/include', '-I../nanogui/ext/eigen',
    '-I../nanogui/ext/nanovg/src', '-I..', '-framework', 'OpenGL', '-L../nanogui/build', '-lGLEW', '-lnanogui',
    '/usr/local/include' ],
  }
