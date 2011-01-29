from cake.tools import script, compiler, variant
from cake import path

variantString = "%s_%s_%s" % (variant.compiler, variant.architecture, variant.release)

# Sources List
# src/
sources = script.cwd(["src/chat.cpp",
    "src/cliScreen.cpp",
    "src/config.cpp",
    "src/constants.cpp",
    "src/furnace.cpp",
    "src/furnaceManager.cpp",
    "src/inventory.cpp",
    "src/lighting.cpp",
    "src/logger.cpp",
    "src/map.cpp",
    "src/mineserver.cpp",
    "src/mob.cpp",
    "src/nbt.cpp",
    "src/packets.cpp",
    "src/physics.cpp",
    "src/plugin.cpp",
    "src/plugin_api.cpp",
    "src/screenBase.cpp",
    "src/sockets.cpp",
    "src/tools.cpp",
    "src/tree.cpp",
    "src/user.cpp"
  ])

# src/blocks
sources_blocks = script.cwd([
    "src/blocks/basic.cpp",
    "src/blocks/cake.cpp",
    "src/blocks/chest.cpp",
    "src/blocks/default.cpp",
    "src/blocks/door.cpp",
    "src/blocks/falling.cpp",
    "src/blocks/fire.cpp",
    "src/blocks/ladder.cpp",
    "src/blocks/liquid.cpp",
    "src/blocks/plant.cpp",
    "src/blocks/sign.cpp",
    "src/blocks/snow.cpp",
    "src/blocks/stair.cpp",
    "src/blocks/torch.cpp",
    "src/blocks/tracks.cpp"
    ])

sources_config= script.cwd([
    "src/config/lexer.cpp",
    "src/config/node.cpp",
    "src/config/parser.cpp",
    "src/config/scanner.cpp"
 ])

sources_worldgen= script.cwd([
    "src/worldgen/cavegen.cpp",
    "src/worldgen/heavengen.cpp",
    "src/worldgen/mapgen.cpp",
    "src/worldgen/nethergen.cpp"
    ])

compiler.addIncludePath(script.cwd("mineserver/includes"))
compiler.addLibrary("libevent")
compiler.addLibrary("libnoise")
compiler.addLibrary("zlibwapi")
compiler.addDefine("FADOR_PLUGIN")

# Windows only
compiler.addLibrary("Winmm")
compiler.addLibrary("Ws2_32")
compiler.addDefine("ZLIB_WINAPI")

# Build the objects.
def build_objects(subFolder, sources):
  # TODO: build the src/$(subfolder)/sourcefilename in this function
  return compiler.objects(
    targetDir=script.cwd(path.join('objs', variantString, subFolder)),
    sources=sources,
  )

objects = build_objects('', sources)
objects_blocks = build_objects('blocks',  sources_blocks)
objects_blocks = build_objects('blocks',  sources_blocks)
objects_config = build_objects('configs',  sources_config)
objects_worldgen = build_objects('worldgen',  sources_worldgen)

# Build the program.
compiler.program(
  target=script.cwd("bin/mineserver_" + variantString),
  sources=[objects, objects_blocks, objects_config, objects_worldgen],
  )
