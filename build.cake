from cake.tools import script, compiler, variant
from cake import path
from cake.library import flatten

variantString = "%s_%s_%s" % (variant.compiler, variant.architecture, variant.release)

# Sources List
sources = script.cwd('src', [
  'chat.cpp',
  'cliScreen.cpp',
  'config.cpp',
  'constants.cpp',
  'furnace.cpp',
  'furnaceManager.cpp',
  'inventory.cpp',
  'lighting.cpp',
  'logger.cpp',
  'map.cpp',
  'mineserver.cpp',
  'mob.cpp',
  'nbt.cpp',
  'packets.cpp',
  'physics.cpp',
  'plugin.cpp',
  'plugin_api.cpp',
  'screenBase.cpp',
  'sockets.cpp',
  'tools.cpp',
  'tree.cpp',
  'user.cpp'
  ])

# blocks
sources_blocks = script.cwd('src/blocks',[
  'basic.cpp',
  'blockfurnace.cpp',
  'cake.cpp',
  'chest.cpp',
  'default.cpp',
  'door.cpp',
  'falling.cpp',
  'fire.cpp',
  'ladder.cpp',
  'liquid.cpp',
  'note.cpp',
  'plant.cpp',
  'sign.cpp',
  'snow.cpp',
  'stair.cpp',
  'torch.cpp',
  'tracks.cpp',
  'workbench.cpp',
  ])

sources_config= script.cwd('src/config', [
  'lexer.cpp',
  'node.cpp',
  'parser.cpp',
  'scanner.cpp',
  ])

sources_worldgen= script.cwd('src/worldgen', [
  'cavegen.cpp',
  'heavengen.cpp',
  'mapgen.cpp',
  'nethergen.cpp',
  ])

compiler.addIncludePath(script.cwd('mineserver/includes'))
compiler.addLibrary('libevent')
compiler.addLibrary('libnoise')
compiler.addLibrary('zlibwapi')
compiler.addDefine('FADOR_PLUGIN')

# Windows only
if variant.platform == 'windows':
  compiler.addLibrary('Winmm')
  compiler.addLibrary('Ws2_32')
  compiler.addDefine('ZLIB_WINAPI')


def buildObjects(subFolder, sources):
  """ Helper function for creating the object sets """
  return compiler.objects(
    targetDir=script.cwd(path.join('objs', variantString, subFolder)),
    sources=sources,
  )

objects = buildObjects('', sources)
objects_blocks = buildObjects('blocks',  sources_blocks)
objects_blocks = buildObjects('blocks',  sources_blocks)
objects_config = buildObjects('configs',  sources_config)
objects_worldgen = buildObjects('worldgen',  sources_worldgen)

# Build the program.
compiler.program(
  target=script.cwd('bin/mineserver_' + variantString),
  sources=flatten([objects, objects_blocks, objects_config, objects_worldgen]),
  )
