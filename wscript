srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  # if -d '.git':
  conf.env.append_unique('CXXFLAGS', ["-Wall"])
  conf.env.append_unique('LINKFLAGS', '-lgdbm')

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'gdbm'
  obj.source = 'gdbm.cc'
  obj.uselib = "GDBM"

