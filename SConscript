from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c')
path    = [cwd]

group = DefineGroup('sht4x', src, depend = ['PKG_USING_SHT4X'], CPPPATH = path)

Return('group')