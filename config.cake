#-------------------------------------------------------------------------------
# This is the configuration file for the cake-build system.
#
# The cake build system is available from http://cake-build.sourceforge.net/
#
#-------------------------------------------------------------------------------
from cake.library.script import ScriptTool
from cake.library.filesys import FileSystemTool
from cake.library.variant import VariantTool
from cake.library.shell import ShellTool
from cake.library.zipping import ZipTool
from cake.library.logging import LoggingTool
from cake.library.project import ProjectTool
from cake.library.env import Environment
from cake.library.compilers import CompilerNotFoundError
from cake.engine import Script, Variant
import cake.system
import os

script = Script.getCurrent()
engine = script.engine
configuration = script.configuration

hostPlatform = cake.system.platform().lower()
hostArchitecture = cake.system.architecture().lower()

# This is how you set an alternative base-directory
# All relative paths will be relative to this absolute path.
#configuration.baseDir = configuration.baseDir + '/..'

base = Variant()
base.tools["script"] = ScriptTool(configuration=configuration)
filesys = base.tools["filesys"] = FileSystemTool(configuration=configuration)
base.tools["variant"] = VariantTool(configuration=configuration)
shell = base.tools["shell"] = ShellTool(configuration=configuration)
shell.update(os.environ)
zipping = base.tools["zipping"] = ZipTool(configuration=configuration)
base.tools["logging"] = LoggingTool(configuration=configuration)
env = base.tools["env"] = Environment(configuration=configuration)
#projectTool = base.tools["project"] = ProjectTool(configuration=configuration)
#projectTool.product = projectTool.VS2008
#projectTool.enabled = engine.options.createProjects
#engine.addBuildSuccessCallback(projectTool.build)

# Disable tools during project generation
#if engine.options.createProjects:
#  filesys.enabled = False
#  shell.enabled = False
#  zipping.enabled = False

def createVariants(parent):
  for release in ["debug", "release"]:
    variant = parent.clone(release=release)

    platform = variant.keywords["platform"]
    compilerName = variant.keywords["compiler"]
    architecture = variant.keywords["architecture"]

    env = variant.tools["env"]
    env["BUILD"] = "build/" + "_".join([
      platform,
      compilerName,
      architecture,
      release,
      ])

    compiler = variant.tools["compiler"]
    compiler.objectCachePath = "cache/obj"
    compiler.enableRtti = True
    compiler.enableExceptions = True
    compiler.outputMapFile = True
    compiler.messageStyle = compiler.MSVS_CLICKABLE

    if release == "debug":
      compiler.addDefine("_DEBUG")
      compiler.debugSymbols = True
      compiler.useIncrementalLinking = True
      compiler.optimisation = compiler.NO_OPTIMISATION
    elif release == "release":
      compiler.addDefine("NDEBUG")
      compiler.useIncrementalLinking = False
      compiler.useFunctionLevelLinking = True
      compiler.optimisation = compiler.FULL_OPTIMISATION

    # Disable the compiler during project generation
    #if engine.options.createProjects:
    # compiler.enabled = False

    configuration.addVariant(variant)

if cake.system.isWindows():
  # MSVC
  from cake.library.compilers.msvc import findMsvcCompiler
  for a in ["x86", "x64", "ia64"]:
    try:
      msvc = base.clone(platform="windows", compiler="msvc", architecture=a)
      compiler = msvc.tools["compiler"] = findMsvcCompiler(configuration=configuration, architecture=a)
      compiler.addDefine("WIN32")
      if a in ["x64", "ia64"]:
        compiler.addDefine("WIN64")
      createVariants(msvc)
    except CompilerNotFoundError, e:
      pass

  try:
    # MinGW
    from cake.library.compilers.gcc import findMinGWCompiler
    mingw = base.clone(platform="windows", compiler="mingw", architecture=hostArchitecture)
    mingw.tools["compiler"] = findMinGWCompiler(configuration=configuration)
    createVariants(mingw)
  except CompilerNotFoundError:
    pass

try:
  # GCC
  from cake.library.compilers.gcc import findGccCompiler
  gcc = base.clone(platform=hostPlatform, compiler="gcc", architecture=hostArchitecture)
  compiler = gcc.tools["compiler"] = findGccCompiler(configuration=configuration)
  compiler.addLibrary("stdc++")
  createVariants(gcc)
except CompilerNotFoundError:
  pass

