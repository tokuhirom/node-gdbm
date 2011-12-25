srcdir = '.'
VERSION = '0.0.1'

import Utils
from os import unlink, symlink, chdir
from os.path import exists, lexists

def set_options(opt):
    opt.tool_options('compiler_cxx')

def configure(conf):
    conf.check_tool('compiler_cxx')
    conf.check_tool('node_addon')
    conf.check_cxx(lib = 'gdbm')
    # if -d '.git':
    conf.env.append_unique('CXXFLAGS', ["-Wall", '-g'])
    conf.env.append_unique('LINKFLAGS', '-lgdbm')

def build(bld):
    Utils.exec_command('node author/version.js')

    obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
    obj.target = 'gdbm'
    obj.source = 'gdbm.cc'
    obj.uselib = "GDBM"

    if exists('build/Release/gdbm.node') and not lexists('gdbm.node'):
        symlink('build/default/gdbm.node', 'gdbm.node')

