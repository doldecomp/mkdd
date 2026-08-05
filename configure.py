#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "MarioClub_us",  # 0
    "Release_eu"
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

debug = args.debug
if config.version == "MarioClub_us":
    debug = True

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-1"
config.compilers_tag = "20250812"
config.dtk_tag = "v1.6.2"
config.objdiff_tag = "v3.3.1"
config.sjiswrap_tag = "v1.2.1"
config.wibo_tag = "0.7.0"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if debug:
    config.ldflags.append("-g")
if args.map:
    config.ldflags.append("-mapunused")

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = 66  # Mario Kart: Double Dash!!

cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    # "-W all",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-use_lmw_stmw on",
    "-common on",
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    "-i libs/PowerPC_EABI_Support/include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

if "_eu" in config.version:
    cflags_base.extend(["-DVIDEO_PAL=1", "-DREGION_EU=1"])
elif "_us" in config.version:
    cflags_base.extend(["-DREGION_US=1"])
elif "_jp" in config.version:
    cflags_base.extend(["-DREGION_JP=1"])

if not config.non_matching:
    cflags_base.extend(["-DMATCHING=1"])

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,    
    "-str readonly",
    "-gccinc",
    "-common off",
    "-inline auto, deferred",
    "-O4,p"
]

# dolphin library flags
cflags_dolphin = [
    *cflags_base,
    "-lang=c", 
    "-fp fmadd",
    "-fp_contract off",
    "-char signed",
    "-str reuse", 
    "-common off",
    "-O4,p",
    #"-requireprotos"
]

# JSystem flags
cflags_jsystem = [
    *cflags_base,
    "-lang=c++",
    "-fp fmadd",
    "-char signed",
    "-rostr",
]

# Game flags
cflags_game = [
    *cflags_base,
    "-lang=c++",
    "-fp fmadd",
    "-char signed",
    "-rostr",
    "-O4,s"
]

# Debug flags
if debug:
    cflags_game.extend(["-DDEBUG=1", "-DHIO_SCREENSHOT=1"])
    cflags_jsystem.extend(["-DDEBUG=1", "-DHIO_SCREENSHOT=1"])
else:
    cflags_game.append("-DNDEBUG=1")
    cflags_jsystem.append("-DNDEBUG=1")

config.linker_version = "GC/2.6"


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "src_dir": "libs",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_dolphin,
        "progress_category": "sdk",
        "host": True,
        "objects": objects,
    }


# Helper function for MSL libraries
def mslLib(lib_name: str, extra_cflags: List[str], objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "src_dir": "libs/PowerPC_EABI_Support/src",
        "mw_version": "GC/2.6",
        "cflags": cflags_runtime + extra_cflags,
        "progress_category": "msl",
        "host": True,
        "objects": objects,
    }


def trkLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "src_dir": "libs/runtime_libs",
        "mw_version": "GC/2.6",
        "cflags": cflags_runtime,
        "progress_category": "msl",
        "host": True,
        "objects": objects,
    }


# Helper function for JSystem libraries
def JSystemLib(lib_name: str, extra_cflags: List[str], objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "src_dir": "libs",
        "mw_version": "GC/2.6",
        "cflags": cflags_jsystem + extra_cflags,
        "progress_category": "jsys",
        "host": True,
        "objects": objects,
    }


# Helper function for Game libraries
def GameLib(lib_name: str, extra_cflags: List[str], objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/2.6",
        "cflags": cflags_game + extra_cflags,
        "progress_category": "game",
        "host": True,
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = True
config.progress_all = False
config.libs = [
    GameLib(
        "Localize",
        [],
        [
            Object(Matching, "kartLocale.cpp"),
        ]
    ),
    mslLib(
        "Runtime.PPCEABI.H",
        [],
        [
            Object(Matching, "Runtime/__mem.c"),
            Object(Matching, "Runtime/__va_arg.c"),
            Object(Matching, "Runtime/global_destructor_chain.c"),
            Object(NonMatching, "Runtime/NMWException.cp"),
            Object(Matching, "Runtime/CPlusLibPPC.cp"),
            Object(Matching, "Runtime/ptmf.c"),
            Object(Matching, "Runtime/runtime.c"),
            Object(Matching, "Runtime/__init_cpp_exceptions.cpp"),
            Object(NonMatching, "Runtime/Gecko_ExceptionPPC.cp"),
            Object(Matching, "Runtime/GCN_mem_alloc.c"),
        ]
    ),
    mslLib(
        "MSL_C.PPCEABI.H",
        ["-str pool", "-opt level=0, peephole, schedule, nospace", "-inline off", "-sym on"],
        [
            Object(Matching, "MSL_C/PPC_EABI/abort_exit.c"),
            Object(Matching, "MSL_C/MSL_Common/alloc.c"),
            Object(Matching, "MSL_C/MSL_Common/ansi_files.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/ansi_fp.c"),
            Object(Matching, "MSL_C/MSL_Common/arith.c"),
            Object(Matching, "MSL_C/MSL_Common/bsearch.c"),
            Object(Matching, "MSL_C/MSL_Common/buffer_io.c"),
            Object(Matching, "MSL_C/PPC_EABI/critical_regions.gamecube.c"),
            Object(Matching, "MSL_C/MSL_Common/ctype.c"),
            Object(Matching, "MSL_C/MSL_Common/direct_io.c"),
            Object(Matching, "MSL_C/MSL_Common/errno.c"),
            Object(Matching, "MSL_C/MSL_Common/file_io.c"),
            Object(Matching, "MSL_C/MSL_Common/FILE_POS.C"),
            Object(Matching, "MSL_C/MSL_Common/locale.c"),
            Object(Matching, "MSL_C/MSL_Common/mbstring.c"),
            Object(Matching, "MSL_C/MSL_Common/mem.c"),
            Object(Matching, "MSL_C/MSL_Common/mem_funcs.c"),
            Object(Matching, "MSL_C/MSL_Common/misc_io.c"),
            Object(Matching, "MSL_C/MSL_Common/printf.c"),
            Object(Matching, "MSL_C/MSL_Common/scanf.c"),
            Object(Matching, "MSL_C/MSL_Common/signal.c"),
            Object(Matching, "MSL_C/MSL_Common/string.c"),
            Object(Matching, "MSL_C/MSL_Common/strtold.c"),
            Object(Matching, "MSL_C/MSL_Common/strtoul.c"),
            Object(Matching, "MSL_C/MSL_Common/float.c"),
            Object(Matching, "MSL_C/MSL_Common/wchar_io.c"),  
            Object(Matching, "MSL_C/MSL_Common_Embedded/uart_console_io_gcn.c")
        ]
    ),
    mslLib(
        "fdlibm.PPCEABI.H",
        [],          
        [
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_asin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_atan2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_exp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_pow.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_rem_pio2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_cos.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_rem_pio2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_sin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_tan.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_atan.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_ceil.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_copysign.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_cos.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_floor.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_frexp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_ldexp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_modf.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_sin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_tan.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_asin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_atan2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_exp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_pow.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_sqrt.c"),
            Object(Matching, "MSL_C/PPC_EABI/math_ppc.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_sqrt.c")
        ]
    ),
    mslLib(
        "MSL_C.PPCEABI.bare.H",
        [],
        [
            Object(Matching, "MSL_C/MSL_Common/extras.c")
        ]
    ),
    trkLib(
        "TRK_MINNOW_DOLPHIN",
        [
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/mainloop.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/nubevent.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/nubassrt.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/nubinit.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/msg.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/msgbuf.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/serpoll.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/dispatch.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/msghndlr.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/support.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/mutex_TRK.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/notify.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/main_TRK.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/mem_TRK.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Portable/string_TRK.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Processor/ppc/Generic/flush_cache.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Processor/ppc/Generic/__exception.s"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Processor/ppc/Generic/mpc_7xx_603e.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c"),   
            Object(NonMatching, "debugger/embedded/MetroTRK/Os/dolphin/usr_put.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk_glue.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Os/dolphin/targcont.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Os/dolphin/target_options.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Os/dolphin/UDP_Stubs.c"),
            Object(NonMatching, "debugger/embedded/MetroTRK/Export/mslsupp.c"),

            Object(NonMatching, "gamedev/cust_connection/cc/exi2/GCN/EXI2_DDH_GCN/main.c"),
            Object(NonMatching, "gamedev/cust_connection/utils/common/CircleBuffer.c"),
            Object(NonMatching, "gamedev/cust_connection/cc/exi2/GCN/EXI2_GDEV_GCN/main.c"),
            Object(NonMatching, "gamedev/cust_connection/utils/common/MWTrace.c"),
            Object(NonMatching, "gamedev/cust_connection/utils/gc/MWCriticalSection_gc.cpp"),
        ]
    ),
    DolphinLib(
        "ai",
        [
            Object(Matching, "dolphin/ai/ai.c")
        ]        
    ),
    DolphinLib(
        "amcstubs",
        [
            Object(NonMatching, "dolphin/amcstubs/AmcExi2Stubs.c")
        ]        
    ),
    DolphinLib(
        "ar",
        [
            Object(Matching, "dolphin/ar/ar.c"),
            Object(Matching, "dolphin/ar/arq.c")
        ]        
    ),
    DolphinLib(
        "base",
        [
            Object(Matching, "dolphin/base/PPCArch.c")
        ]        
    ),
    DolphinLib(
        "card",
        [
            Object(Matching, "dolphin/card/CARDBios.c"),
            Object(Matching, "dolphin/card/CARDUnlock.c"),
            Object(Matching, "dolphin/card/CARDRdwr.c"),
            Object(Matching, "dolphin/card/CARDBlock.c"),
            Object(Matching, "dolphin/card/CARDDir.c"),
            Object(Matching, "dolphin/card/CARDCheck.c"),
            Object(Matching, "dolphin/card/CARDMount.c"),
            Object(Matching, "dolphin/card/CARDFormat.c"),
            Object(Matching, "dolphin/card/CARDOpen.c"),
            Object(Matching, "dolphin/card/CARDNet.c"),
            Object(Matching, "dolphin/card/CARDCreate.c"),
            Object(Matching, "dolphin/card/CARDRead.c"),
            Object(Matching, "dolphin/card/CARDWrite.c"),
            Object(Matching, "dolphin/card/CARDDelete.c"),
            Object(Matching, "dolphin/card/CARDStat.c"),
            Object(Matching, "dolphin/card/CARDRename.c")
        ]
    ),
    DolphinLib(
        "db",
        [
            Object(Matching, "dolphin/db/db.c"),
        ]
    ),
    DolphinLib(
        "dsp",
        [
            Object(Matching, "dolphin/dsp/dsp.c"),
            Object(Matching, "dolphin/dsp/dsp_debug.c"),
            Object(Matching, "dolphin/dsp/dsp_task.c")
        ]        
    ),
    DolphinLib(
        "dvd",
        [
            Object(Matching, "dolphin/dvd/dvdlow.c"),
            Object(Matching, "dolphin/dvd/dvdfs.c"),
            Object(Matching, "dolphin/dvd/dvd.c"),
            Object(Matching, "dolphin/dvd/dvdqueue.c"),
            Object(Matching, "dolphin/dvd/dvderror.c"),
            Object(Matching, "dolphin/dvd/dvdidutils.c"),
            Object(Matching, "dolphin/dvd/dvdFatal.c"),
            Object(Matching, "dolphin/dvd/emu_level2/fstload.c")
        ]
    ),
    DolphinLib(
        "eth",
        [
            Object(Matching, "dolphin/eth/eth.c"),
            Object(Matching, "dolphin/eth/ethsec.c"),
            Object(Matching, "dolphin/eth/md5.c"),
            Object(Matching, "dolphin/eth/base64.c")
        ]
    ),
    DolphinLib(
        "exi",
        [
            Object(Matching, "dolphin/exi/EXIBios.c"),
            Object(Matching, "dolphin/exi/EXIUart.c")
        ]
    ),
    DolphinLib(
        "gd",
        [
            Object(NonMatching, "dolphin/gd/GDBase.c"),
            Object(NonMatching, "dolphin/gd/GDGeometry.c")
        ]
    ),
    DolphinLib(
        "gx",
        [
            Object(NonMatching, "dolphin/gx/GXInit.c"),
            Object(NonMatching, "dolphin/gx/GXFifo.c"),
            Object(NonMatching, "dolphin/gx/GXAttr.c"),
            Object(NonMatching, "dolphin/gx/GXMisc.c"),
            Object(NonMatching, "dolphin/gx/GXGeometry.c"),
            Object(NonMatching, "dolphin/gx/GXFrameBuf.c"),
            Object(NonMatching, "dolphin/gx/GXLight.c"),
            Object(NonMatching, "dolphin/gx/GXTexture.c"),
            Object(NonMatching, "dolphin/gx/GXBump.c"),
            Object(NonMatching, "dolphin/gx/GXTev.c"),
            Object(NonMatching, "dolphin/gx/GXPixel.c"),
            Object(NonMatching, "dolphin/gx/GXDraw.c"),
            Object(NonMatching, "dolphin/gx/GXDisplayList.c"),
            Object(NonMatching, "dolphin/gx/GXTransform.c"),
            Object(NonMatching, "dolphin/gx/GXPerf.c")
        ]
    ),
    DolphinLib(
        "hio",
        [
            Object(NonMatching, "dolphin/hio/hio.c")
        ]
    ),
    DolphinLib(
        "ip",
        [
            Object(NonMatching, "dolphin/ip/IP.c"),
            Object(NonMatching, "dolphin/ip/IPArp.c"),
            Object(NonMatching, "dolphin/ip/IPIcmp.c"),
            Object(NonMatching, "dolphin/ip/IPRoute.c"),
            Object(NonMatching, "dolphin/ip/IPUdp.c"),
            Object(NonMatching, "dolphin/ip/IPFrag.c"),
            Object(NonMatching, "dolphin/ip/IPEther.c"),
            Object(NonMatching, "dolphin/ip/IFFifo.c"),
            Object(NonMatching, "dolphin/ip/IFRing.c"),
            Object(NonMatching, "dolphin/ip/IPTcp.c"),
            Object(NonMatching, "dolphin/ip/IPTcpOutput.c"),
            Object(NonMatching, "dolphin/ip/IPTcpTimer.c"),
            Object(NonMatching, "dolphin/ip/IPTcpUser.c"),
            Object(NonMatching, "dolphin/ip/IPTcpTimeWait.c"),
            Object(NonMatching, "dolphin/ip/IPDns.c"),
            Object(NonMatching, "dolphin/ip/IPDhcp.c"),
            Object(NonMatching, "dolphin/ip/IPZero.c"),
            Object(NonMatching, "dolphin/ip/IPOpt.c"),
            Object(NonMatching, "dolphin/ip/IPSocket.c"),
            Object(NonMatching, "dolphin/ip/IPPPP.c"),
            Object(NonMatching, "dolphin/ip/IPPPPoE.c"),
            Object(NonMatching, "dolphin/ip/IPLcp.c"),
            Object(NonMatching, "dolphin/ip/IPIpcp.c"),
            Object(NonMatching, "dolphin/ip/IPPap.c"),
            Object(NonMatching, "dolphin/ip/IPChap.c"),
            Object(NonMatching, "dolphin/ip/IPIgmp.c"),
            Object(NonMatching, "dolphin/ip/IPUuid.c")
        ]
    ),
    DolphinLib(
        "lg",  # unofficial name
        [
            Object(NonMatching, "dolphin/lg/allsrc.c")
        ]
    ),
    DolphinLib(
        "mtx",
        [
            Object(NonMatching, "dolphin/mtx/mtx.c"),
            Object(NonMatching, "dolphin/mtx/mtxvec.c"),
            Object(NonMatching, "dolphin/mtx/mtx44.c"),
            Object(Matching, "dolphin/mtx/mtx44vec.c"),
            Object(Matching, "dolphin/mtx/vec.c"),
            Object(NonMatching, "dolphin/mtx/quat.c"),
        ]
    ),
    DolphinLib(
        "OdemuExi2",
        [
            Object(NonMatching, "dolphin/OdemuExi2/DebuggerDriver.c")
        ]
    ),
    DolphinLib(
        "odenotstub",
        [
            Object(NonMatching, "dolphin/odenotstub/odenotstub.c")
        ]
    ),
    DolphinLib(
        "os",
        [
            Object(Matching, "dolphin/os/OS.c"),
            Object(Matching, "dolphin/os/OSAlarm.c"),
            Object(Matching, "dolphin/os/OSAlloc.c"),
            Object(Matching, "dolphin/os/OSArena.c"),
            Object(Matching, "dolphin/os/OSAudioSystem.c"),
            Object(Matching, "dolphin/os/OSCache.c"),
            Object(Matching, "dolphin/os/OSContext.c"),
            Object(Matching, "dolphin/os/OSError.c"),
            Object(Matching, "dolphin/os/OSFont.c"),
            Object(Matching, "dolphin/os/OSInterrupt.c"),
            Object(Matching, "dolphin/os/OSLink.c"),
            Object(Matching, "dolphin/os/OSMessage.c"),
            Object(Matching, "dolphin/os/OSMemory.c"),
            Object(Matching, "dolphin/os/OSMutex.c"),
            Object(Matching, "dolphin/os/OSReboot.c"),
            Object(Matching, "dolphin/os/OSReset.c"),
            Object(Matching, "dolphin/os/OSResetSW.c"),
            Object(Matching, "dolphin/os/OSRtc.c"),
            Object(Matching, "dolphin/os/OSThread.c"),
            Object(Matching, "dolphin/os/OSTime.c"),
            Object(Matching, "dolphin/os/OSSync.c"),
            Object(Matching, "dolphin/os/init/__start.c"),
            Object(Matching, "dolphin/os/init/__ppc_eabi_init.cpp")
        ]
    ),
    DolphinLib(
        "pad",
        [
            Object(Matching, "dolphin/pad/Padclamp.c"),
            Object(Matching, "dolphin/pad/Pad.c")
        ]
    ),
    DolphinLib(
        "si",
        [
            Object(Matching, "dolphin/si/SIBios.c"),
            Object(Matching, "dolphin/si/SISamplingRate.c"),
            Object(NonMatching, "dolphin/si/SISteering.c"),
            Object(NonMatching, "dolphin/si/SISteeringXfer.c"),
            Object(NonMatching, "dolphin/si/SISteeringAuto.c")
        ]
    ),
    DolphinLib(
        "upnp",
        [
            Object(NonMatching, "dolphin/upnp/UPnP.c"),
            Object(NonMatching, "dolphin/upnp/UPnPHttp.c"),
            Object(NonMatching, "dolphin/upnp/UPnPSsdp.c"),
            Object(NonMatching, "dolphin/upnp/UPnPUuid.c"),
            Object(NonMatching, "dolphin/upnp/UPnPUri.c"),
            Object(NonMatching, "dolphin/upnp/UPnPHttpd.c"),
            Object(NonMatching, "dolphin/upnp/UPnPHttpdResponse.c")
        ]
    ),
    DolphinLib(
        "vi",
        [
            Object(Matching, "dolphin/vi/vi.c")
        ]
    ),
    DolphinLib(
        "thp",
        [
            Object(Matching, "dolphin/thp/THPDec.c"),
            Object(Matching, "dolphin/thp/THPAudio.c")
        ]
    ),
    JSystemLib(
        "J2DGraph",
        ["-O4,s"],
        [
            Object(Matching, "JSystem/J2DGraph/J2DAnimation.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DAnmLoader.cpp"),
            Object(Equivalent, "JSystem/J2DGraph/J2DBloSaver.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DGrafContext.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DManage.cpp"),
            Object(Equivalent, "JSystem/J2DGraph/J2DMatBlock.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DMaterial.cpp"),
            Object(Equivalent, "JSystem/J2DGraph/J2DMaterialFactory.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DOrthoGraph.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DPane.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DPerspGraph.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DPicture.cpp"),
            Object(Equivalent, "JSystem/J2DGraph/J2DPictureEx.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DPrint.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DScreen.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DTevs.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DTextBox.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DTextBoxEx.cpp"),
            Object(Equivalent, "JSystem/J2DGraph/J2DWindow.cpp"),
            Object(Matching, "JSystem/J2DGraph/J2DWindowEx.cpp")
        ]
    ),
    JSystemLib(
        "J3D",
        ["-O4,p"],
        [
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DAnimation.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DCluster.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DJoint.cpp"), 
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DJointTree.cpp"), 
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DMaterialAnm.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DMaterialAttach.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DModel.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DModelData.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DMtxBuffer.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DShapeTable.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphAnimator/J3DSkinDeform.cpp"),                    
            
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DDrawBuffer.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DGD.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DMatBlock.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DMaterial.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DPacket.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DShape.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DShapeDraw.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DShapeMtx.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DStruct.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DSys.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DTevs.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DTexture.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DTransform.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphBase/J3DVertex.cpp"),                     

            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DAnmLoader.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DBinaryFormat.cpp"),  # might not be in GraphLoader
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DClusterLoader.cpp"),            
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DJointFactory.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DMaterialFactory.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DMaterialFactory_v21.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DModelLoader.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DModelLoaderCalcSize.cpp"),
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DModelSaver.cpp"),  # might not be in GraphLoader
            Object(NonMatching, "JSystem/J3D/GraphLoader/J3DShapeFactory.cpp")
        ]
    ),
    JSystemLib(
        "J3DU",
        ["-O4,p"],
        [
            Object(NonMatching, "JSystem/J3DU/J3DUPerf.cpp"),
            Object(NonMatching, "JSystem/J3DU/J3DUDL.cpp"),
            Object(NonMatching, "JSystem/J3DU/J3DUMotion.cpp"),
            Object(Matching, "JSystem/J3DU/J3DUClipper.cpp"),
        ]
    ),
    JSystemLib(
        "JAudio",
        ["-O4,p"],
        [
            Object(Matching, "JSystem/JAudio/Interface/JAIAudience.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISe.cpp"),
            Object(Equivalent, "JSystem/JAudio/Interface/JAISeMgr.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISeq.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISeqDataMgr.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISeqMgr.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISound.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISoundChild.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISoundHandles.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISoundInfo.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISoundParams.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAISoundStarter.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAIStream.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAIStreamDataMgr.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAIStreamMgr.cpp"),
            Object(Matching, "JSystem/JAudio/Interface/JAIAudible.cpp"),

            # System/?
            Object(Matching, "JSystem/JAudio/System/JASCalc.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASCallback.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASCmdStack.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASDvdThread.cpp"),
            Object(Equivalent, "JSystem/JAudio/System/JASHeapCtrl.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASProbe.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASReport.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASResArcLoader.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASTaskThread.cpp"),
            # System/?
            Object(Matching, "JSystem/JAudio/System/JASAiCtrl.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASAudioReseter.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASAudioThread.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASChannel.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASDriverIF.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASDSPChannel.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASDSPInterface.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASLfo.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASOscillator.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASSoundParams.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASWSParser.cpp"),
            # System/?
            Object(Matching, "JSystem/JAudio/System/JASBasicBank.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASBasicInst.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASBasicWaveBank.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASBNKParser.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASDrumSet.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASInstRand.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASInstSense.cpp"),            
            Object(Matching, "JSystem/JAudio/System/JASSimpleWaveBank.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASWaveArcLoader.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASWaveBank.cpp"),
            # System/ 
            Object(Matching, "JSystem/JAudio/System/JASBank.cpp"),
            # System/Player/src
            Object(Matching, "JSystem/JAudio/System/JASAramStream.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASRegisterParam.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASSeqCtrl.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASSeqParser.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASSeqReader.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASTrack.cpp"),
            Object(Matching, "JSystem/JAudio/System/JASTrackPort.cpp"),

            Object(Matching, "JSystem/JAudio/Task/dspproc.c", extra_cflags=["-O4,s", "-inline noauto", "-use_lmw_stmw off", "-func_align 32"]),
            Object(Matching, "JSystem/JAudio/Task/dsptask.c", extra_cflags=["-O4,s", "-inline noauto", "-use_lmw_stmw off", "-func_align 32"]),
            Object(Matching, "JSystem/JAudio/Task/osdsp.c", extra_cflags=["-O4,s", "-inline noauto", "-use_lmw_stmw off", "-func_align 32"]),
            Object(Matching, "JSystem/JAudio/Task/osdsp_task.c", extra_cflags=["-O4,s", "-inline noauto", "-use_lmw_stmw off", "-func_align 32"]),

            Object(Matching, "JSystem/JAudio/Utility/JAUAudioArcInterpreter.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUAudioArcLoader.cpp"),
            Object(NonMatching, "JSystem/JAudio/Utility/JAUAudioMgr.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUBankTable.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUClusterSound.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUInitializer.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUSeqCollection.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUSeqDataBlockMgr.cpp"),
            Object(Equivalent, "JSystem/JAudio/Utility/JAUSoundAnimator.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUSoundMgr.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUSoundTable.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUStreamFileTable.cpp"),
            Object(NonMatching, "JSystem/JAudio/Utility/JAUAudience.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUSectionHeap.cpp"),
            Object(Matching, "JSystem/JAudio/Utility/JAUSoundObject.cpp")
        ]
    ),
    JSystemLib(
        "JFramework",
        ["-O4,p"],
        [
            Object(Matching, "JSystem/JFramework/JFWSystem.cpp"),
            Object(Matching, "JSystem/JFramework/JFWDisplay.cpp")
        ]
    ),
    JSystemLib(
        "JGadget",
        ["-O4,p"],
        [
            Object(Matching, "JSystem/JGadget/hashcode.cpp"),
            Object(Matching, "JSystem/JGadget/linklist.cpp")
        ]
    ),
    JSystemLib(
        "JKernel",
        ["-O4,s"],
        [
            Object(Matching, "JSystem/JKernel/JKRAram.cpp"),
            Object(Matching, "JSystem/JKernel/JKRAramArchive.cpp"),
            Object(Matching, "JSystem/JKernel/JKRAramBlock.cpp"),
            Object(Matching, "JSystem/JKernel/JKRAramHeap.cpp"),
            Object(Matching, "JSystem/JKernel/JKRAramPiece.cpp"),
            Object(Matching, "JSystem/JKernel/JKRAramStream.cpp"),
            Object(Matching, "JSystem/JKernel/JKRArchivePri.cpp"),
            Object(Matching, "JSystem/JKernel/JKRArchivePub.cpp"),
            Object(Matching, "JSystem/JKernel/JKRCompArchive.cpp"),
            Object(Matching, "JSystem/JKernel/JKRDecomp.cpp"),
            Object(Matching, "JSystem/JKernel/JKRDisposer.cpp"),
            Object(Matching, "JSystem/JKernel/JKRDvdFile.cpp"),
            Object(Matching, "JSystem/JKernel/JKRDvdAramRipper.cpp"),
            Object(Matching, "JSystem/JKernel/JKRDvdArchive.cpp"),
            Object(Matching, "JSystem/JKernel/JKRDvdRipper.cpp"),
            Object(Matching, "JSystem/JKernel/JKRExpHeap.cpp"),
            Object(Matching, "JSystem/JKernel/JKRFileCache.cpp"),
            Object(Matching, "JSystem/JKernel/JKRFileFinder.cpp"),
            Object(Matching, "JSystem/JKernel/JKRFileLoader.cpp"),
            Object(Matching, "JSystem/JKernel/JKRHeap.cpp"),
            Object(Matching, "JSystem/JKernel/JKRMemArchive.cpp"),
            Object(Matching, "JSystem/JKernel/JKRSolidHeap.cpp"),
            Object(Matching, "JSystem/JKernel/JKRThread.cpp"),
            Object(Matching, "JSystem/JKernel/JKRAssertHeap.cpp")
        ]
    ),
    JSystemLib(
        "JMath",
        ["-O4,p"],
        [
            Object(Matching, "JSystem/JMath/JMath.cpp"),
            Object(Matching, "JSystem/JMath/random.cpp"),
            Object(Matching, "JSystem/JMath/JMATrigonometric.cpp")
        ]
    ),
    JSystemLib(
        "JParticle",
        ["-O4,p"],
        [
            Object(Equivalent, "JSystem/JParticle/JPABaseShape.cpp"),
            Object(Matching, "JSystem/JParticle/JPAChildShape.cpp"),
            Object(Matching, "JSystem/JParticle/JPADynamicsBlock.cpp"),
            Object(Matching, "JSystem/JParticle/JPAEmitter.cpp"),
            Object(Matching, "JSystem/JParticle/JPAEmitterManager.cpp"),
            Object(Matching, "JSystem/JParticle/JPAExTexShape.cpp"),
            Object(Matching, "JSystem/JParticle/JPAExtraShape.cpp"),
            Object(Matching, "JSystem/JParticle/JPAFieldBlock.cpp"),
            Object(Matching, "JSystem/JParticle/JPAKeyBlock.cpp"),
            Object(Matching, "JSystem/JParticle/JPAMath.cpp"),
            Object(Matching, "JSystem/JParticle/JPAParticle.cpp"),
            Object(Matching, "JSystem/JParticle/JPAResource.cpp"),
            Object(Matching, "JSystem/JParticle/JPAResourceLoader.cpp"),
            Object(Matching, "JSystem/JParticle/JPAResourceManager.cpp"),
            Object(Matching, "JSystem/JParticle/JPATexture.cpp")
        ]
    ),
    JSystemLib(
        "JSupport",
        ["-O4,p"],
        [
            Object(Matching, "JSystem/JSupport/JSUInputStream.cpp"),
            Object(Matching, "JSystem/JSupport/JSUList.cpp"),
            Object(Matching, "JSystem/JSupport/JSUMemoryStream.cpp"),
            Object(Matching, "JSystem/JSupport/JSUFileStream.cpp")
        ]
    ),
    JSystemLib(
        "JUtility",
        ["-O4,s"],
        [
            Object(Matching, "JSystem/JUtility/JUTAssert.cpp"),
            Object(Matching, "JSystem/JUtility/JUTConsole.cpp"),
            Object(Matching, "JSystem/JUtility/JUTDbPrint.cpp"),
            Object(Matching, "JSystem/JUtility/JUTDirectFile.cpp"),
            Object(Matching, "JSystem/JUtility/JUTDirectPrint.cpp"),
            Object(Matching, "JSystem/JUtility/JUTException.cpp"),
            Object(Matching, "JSystem/JUtility/JUTFader.cpp"),
            Object(Matching, "JSystem/JUtility/JUTFont.cpp"),
            Object(Matching, "JSystem/JUtility/JUTGamePad.cpp"),
            Object(Matching, "JSystem/JUtility/JUTGraphFifo.cpp"),
            Object(Matching, "JSystem/JUtility/JUTNameTab.cpp"),
            Object(Matching, "JSystem/JUtility/JUTPalette.cpp"),
            Object(Matching, "JSystem/JUtility/JUTProcBar.cpp"),
            Object(Matching, "JSystem/JUtility/JUTResFont.cpp"),
            Object(Matching, "JSystem/JUtility/JUTResource.cpp"),
            Object(Matching, "JSystem/JUtility/JUTTexture.cpp"),
            Object(Matching, "JSystem/JUtility/JUTVideo.cpp"),
            Object(Matching, "JSystem/JUtility/JUTXfb.cpp"),
            Object(Matching, "JSystem/JUtility/JUTFontData_Ascfont_fix12.s", extra_asflags=[f"-I build/{config.version}/bin"],)
        ]
    ),
    JSystemLib(
        "Logitech",
        ["-O4,p"],
        [
            Object(NonMatching, "JSystem/Logitech/Conditon.cpp"),
            Object(NonMatching, "JSystem/Logitech/Constant.cpp"),
            Object(NonMatching, "JSystem/Logitech/Force.cpp"),
            Object(NonMatching, "JSystem/Logitech/LGWheels.cpp"),
            Object(NonMatching, "JSystem/Logitech/Periodic.cpp"),
            Object(NonMatching, "JSystem/Logitech/Ramp.cpp"),
            Object(NonMatching, "JSystem/Logitech/Wheels.cpp")
        ]
    ),
    GameLib(
        "Bando",
        [],
        [
            Object(Matching, "Bando/EngineSound.cpp")
        ]
    ),
    GameLib(
        "Inagaki",
        [],
        [
            Object(Equivalent, "Inagaki/CharacterSoundMgr.cpp"),
            Object(Matching, "Inagaki/GameAudioCamera.cpp"),
            Object(Matching, "Inagaki/GameAudioCommon.cpp"),
            Object(Matching, "Inagaki/GameAudioHostIO.cpp"),
            Object(NonMatching, "Inagaki/GameAudioMain.cpp"),
            Object(Matching, "Inagaki/GameAudioMgr.cpp"),
            Object(NonMatching, "Inagaki/GameAudioParam.cpp"),
            Object(NonMatching, "Inagaki/KartSoundMgr.cpp"),
            Object(Matching, "Inagaki/NpcCarSoundMgr.cpp"),
            Object(Matching, "Inagaki/ObjectSoundMgr.cpp"),
            Object(Matching, "Inagaki/RockSoundMgr.cpp"),
            Object(NonMatching, "Inagaki/GameSoundTable.cpp"),
            Object(NonMatching, "Inagaki/ShoreSoundMgr.cpp"),
            Object(NonMatching, "Inagaki/CircleSoundMgr.cpp"),
            Object(Matching, "Inagaki/CoasterSoundMgr.cpp"),
            Object(Matching, "Inagaki/MapSoundObjectMgr.cpp"),
            Object(Matching, "Inagaki/GameAudioFxConfig.cpp"),
            Object(Matching, "Inagaki/GameAudioArcLoader.cpp")
        ]
    ),
    GameLib(
        "Kameda",
        [],
        [
            Object(NonMatching, "Kameda/Motor.cpp"),
            Object(NonMatching, "Kameda/MotorManager.cpp"),
            Object(NonMatching, "Kameda/J2DManager.cpp"),
            Object(Matching, "Kameda/Task.cpp"),
            Object(NonMatching, "Kameda/Goal2D.cpp"),
            Object(NonMatching, "Kameda/PauseManager.cpp"),
            Object(NonMatching, "Kameda/Pause2D.cpp"),
            Object(Matching, "Kameda/WipeManager.cpp"),
            Object(NonMatching, "Kameda/Fade.cpp"),
            Object(NonMatching, "Kameda/K2DPicture.cpp"),
            Object(Matching, "Kameda/Scene.cpp"),
            Object(Matching, "Kameda/SceneApp.cpp"),
            Object(NonMatching, "Kameda/SceneTitle.cpp"),
            Object(NonMatching, "Kameda/SceneOption.cpp"),
            Object(NonMatching, "Kameda/SceneRecord.cpp"),
            Object(NonMatching, "Kameda/SceneMap.cpp"),
            Object(NonMatching, "Kameda/Result2D.cpp"),
            Object(NonMatching, "Kameda/SceneFactory.cpp"),
            Object(Matching, "Kameda/SequenceInfo.cpp"),
            Object(Matching, "Kameda/PrintDvdError.cpp"),
            Object(NonMatching, "Kameda/Race2D.cpp"),
            Object(Matching, "Kameda/ReadPrintMessage.cpp"),
            Object(NonMatching, "Kameda/PrintProgressive.cpp"),
            Object(NonMatching, "Kameda/PrintMemoryCard.cpp"),
            Object(NonMatching, "Kameda/PreRace2D.cpp"),
            Object(NonMatching, "Kameda/Go2D.cpp"),
            Object(NonMatching, "Kameda/Mario.cpp"),
            Object(NonMatching, "Kameda/SceneMenu.cpp"),
            Object(Matching, "Kameda/Demo2D.cpp"),
            Object(NonMatching, "Kameda/Go3212D.cpp"),
            Object(NonMatching, "Kameda/CharacterSelect3D.cpp"),
            Object(NonMatching, "Kameda/SceneCourseSelect.cpp"),
            Object(NonMatching, "Kameda/SceneGhostLoadSave.cpp"),
            Object(NonMatching, "Kameda/PrintWindow.cpp"),
            Object(NonMatching, "Kameda/CourseName2D.cpp"),
            Object(NonMatching, "Kameda/SceneLanEntry.cpp"),
            Object(NonMatching, "Kameda/SceneMapSelect.cpp"),
            Object(Matching, "Kameda/BattleName2D.cpp"),
            Object(NonMatching, "Kameda/LapGhost2D.cpp"),
            Object(NonMatching, "Kameda/CanNotSaveG2D.cpp"),
            Object(NonMatching, "Kameda/LANResult2D.cpp"),
            Object(NonMatching, "Kameda/LANNum2D.cpp"),
            Object(NonMatching, "Kameda/Cup2D.cpp"),
            Object(NonMatching, "Kameda/BombStart2D.cpp")
        ]
    ),
    GameLib(
        "Kaneshige",
        ["-inline off"],
        [
            Object(Matching, "Kaneshige/DrawBuffer.cpp"),
            Object(Matching, "Kaneshige/Course/Course.cpp"),
            Object(Matching, "Kaneshige/Course/CrsData.cpp"),
            Object(Matching, "Kaneshige/Course/CrsGround.cpp"),
            Object(Equivalent, "Kaneshige/ExModel.cpp"),
            Object(Matching, "Kaneshige/HioMgr.cpp"),
            Object(Matching, "Kaneshige/KartLoader.cpp"),
            Object(Matching, "Kaneshige/KartInfo.cpp"),
            Object(Matching, "Kaneshige/KartChecker.cpp"),
            Object(Matching, "Kaneshige/RaceMgr.cpp"),
            Object(Matching, "Kaneshige/SysDebug.cpp"),
            Object(Matching, "Kaneshige/CrsArea.cpp"),
            Object(Matching, "Kaneshige/LightMgr.cpp"),
            Object(Matching, "Kaneshige/ScrnShot.cpp"),
            Object(Matching, "Kaneshige/GeoStartLine.cpp"),
            Object(Matching, "Kaneshige/CenterLine.cpp"),
            Object(Matching, "Kaneshige/RaceInfo.cpp"),
            Object(Matching, "Kaneshige/RaceLight.cpp"),
            Object(Matching, "Kaneshige/RaceDrawBuffer.cpp"),
            Object(Matching, "Kaneshige/KartDrawer.cpp"),
            Object(Matching, "Kaneshige/TexLODControl.cpp"),
            Object(Matching, "Kaneshige/ShapePktControl.cpp"),
            Object(Matching, "Kaneshige/SimpleDrawer.cpp"),
            Object(Matching, "Kaneshige/JugemPoint.cpp"),
            Object(Matching, "Kaneshige/GeoCar.cpp"),
            Object(Matching, "Kaneshige/GeoBus.cpp"),
            Object(Matching, "Kaneshige/GeoTruck.cpp"),
            Object(Matching, "Kaneshige/GeoSignal.cpp"),
            Object(Matching, "Kaneshige/GeoNormCar.cpp"),
            Object(Matching, "Kaneshige/GeoCannon.cpp"),
            Object(Matching, "Kaneshige/GeoBombCar.cpp"),
            Object(Matching, "Kaneshige/GeoKinoCar.cpp"),
            Object(Matching, "Kaneshige/GeoKuriCar.cpp"),
            Object(Matching, "Kaneshige/GeoItemCar.cpp"),
            Object(Matching, "Kaneshige/GeoHanaCar.cpp"),
            Object(Matching, "Kaneshige/GeoHanaBody.cpp"),
            Object(Matching, "Kaneshige/GeoCarShadow.cpp"),
            Object(Matching, "Kaneshige/GeoWater.cpp"),
            Object(Matching, "Kaneshige/GeoItemGen.cpp"),
            Object(Matching, "Kaneshige/GeoVision.cpp"),
            Object(Matching, "Kaneshige/GeoSubWater.cpp"),
            Object(Matching, "Kaneshige/RaceBGMPlayer.cpp"),
            Object(Matching, "Kaneshige/RaceDirector.cpp"),
            Object(Matching, "Kaneshige/RaceDrawer.cpp"),
            Object(Matching, "Kaneshige/GeoDkCloud.cpp"),
            Object(Matching, "Kaneshige/KartMotion.cpp"),
            Object(Matching, "Kaneshige/GeoKuribo.cpp"),
            Object(Matching, "Kaneshige/GeoAirJet.cpp"),
            Object(Matching, "Kaneshige/BlurScreen.cpp"),
            Object(Matching, "Kaneshige/GeoRabbitMark.cpp"),
            Object(Matching, "Kaneshige/KartMtx.cpp"),
            Object(Matching, "Kaneshige/DemoTimeKeeper.cpp"),
            Object(Matching, "Kaneshige/GeoKinojii.cpp"),
            Object(Matching, "Kaneshige/GeoSplash.cpp"),
            Object(Matching, "Kaneshige/GeoTideSensor.cpp"),
            Object(Matching, "Kaneshige/GhostScreen.cpp"),
            Object(Matching, "Kaneshige/ShimmerScreen.cpp"),
            Object(Matching, "Kaneshige/GeoPuller.cpp"),
            Object(Matching, "Kaneshige/GeoShimmer.cpp"),
            Object(Matching, "Kaneshige/GeoSoundGen.cpp"),
            Object(Matching, "Kaneshige/GeoShine.cpp"),
            Object(Matching, "Kaneshige/DarkAnmMgr.cpp"),
            Object(Matching, "Kaneshige/Movie/MovieApp.cpp"),
            Object(Matching, "Kaneshige/Movie/MoviePlayer.cpp"),
            Object(Matching, "Kaneshige/Movie/THPDraw.c"),
            Object(Matching, "Kaneshige/MdlViewer.cpp"),
            Object(Matching, "Kaneshige/RaceTime.cpp"),
            Object(Matching, "Kaneshige/MaskScreen.cpp"),
            Object(Matching, "Kaneshige/THP/THPPlayer.c"),
            Object(Matching, "Kaneshige/THP/THPVideoDecode.c"),
            Object(Matching, "Kaneshige/THP/THPRead.c"),
            Object(Matching, "Kaneshige/THP/THPAudioDecode.c")
        ]
    ),
    GameLib(
        "Kawano",
        [],
        [
            Object(NonMatching, "Kawano/driverData.cpp"),
            Object(NonMatching, "Kawano/driver.cpp"),
            Object(NonMatching, "Kawano/driverMgr.cpp"),
            Object(NonMatching, "Kawano/animation.cpp"),
            Object(NonMatching, "Kawano/driverAnim.cpp"),
            Object(NonMatching, "Kawano/driverAnimBuf.cpp"),
            Object(NonMatching, "Kawano/driver_ik.cpp"),
            Object(NonMatching, "Kawano/driverTPAnim.cpp"),
            Object(NonMatching, "Kawano/driverState.cpp"),
            Object(NonMatching, "Kawano/driverOffset.cpp"),
            Object(NonMatching, "Kawano/feeling.cpp"),
            Object(NonMatching, "Kawano/osage.cpp"),
            Object(NonMatching, "Kawano/accessory.cpp"),
            Object(NonMatching, "Kawano/laurel.cpp"),
            Object(NonMatching, "Kawano/driverObj.cpp"),
            Object(Matching, "Kawano/SceneTitleLine.cpp"),
            Object(NonMatching, "Kawano/StaffRoll2D.cpp")
        ]
    ),
    GameLib(
        "Osako",
        [],
        [
            Object(Matching, "Osako/clock.cpp"),
            Object(NonMatching, "Osako/animator.cpp"),
            Object(Matching, "Osako/testApp.cpp"),
            Object(Matching, "Osako/shadowModel.cpp"),
            Object(Matching, "Osako/GameApp.cpp"),
            Object(Matching, "Osako/main.cpp"),
            Object(Matching, "Osako/MainMenuApp.cpp"),
            Object(Matching, "Osako/PlayerSelectApp.cpp"),
            Object(Matching, "Osako/MapSelectApp.cpp"),
            Object(Matching, "Osako/RaceApp.cpp"),
            Object(Matching, "Osako/ResMgr.cpp"),
            Object(Matching, "Osako/LogoApp.cpp"),
            Object(Matching, "Osako/system.cpp"),
            Object(Matching, "Osako/kartPad.cpp"),
            Object(Matching, "Osako/AppMgr.cpp"),
            Object(Matching, "Osako/FontMgr.cpp"),
            Object(Matching, "Osako/ErrorViewApp.cpp"),
            Object(Matching, "Osako/CardMgr.cpp"),
            Object(Matching, "Osako/CardAgent.cpp"),
            Object(Matching, "Osako/SystemFile.cpp"),
            Object(Matching, "Osako/systemData.cpp"),
            Object(NonMatching, "Osako/NetGameMgr.cpp"),
            Object(Matching, "Osako/SequenceApp.cpp"),
            Object(Matching, "Osako/NetGateApp.cpp"),
            Object(Matching, "Osako/TestViewApp.cpp"),
            Object(NonMatching, "Osako/shadowMgr.cpp"),
            Object(Matching, "Osako/shadowScr.cpp"),
            Object(Matching, "Osako/TARecord.cpp"),
            Object(Matching, "Osako/SystemRecord.cpp"),
            Object(Matching, "Osako/GPRecord.cpp"),
            Object(Matching, "Osako/SaveFile.cpp"),
            Object(Matching, "Osako/PadMgr.cpp"),
            Object(Matching, "Osako/PadRecorder.cpp"),
            Object(Matching, "Osako/ForceEffectMgr.cpp"),
            Object(NonMatching, "Osako/BBAMgr.cpp"),
            Object(Matching, "Osako/AwardApp.cpp"),
            Object(Matching, "Osako/GhostFile.cpp"),
            Object(Matching, "Osako/Kart2DCommon.cpp"),
            Object(Matching, "Osako/ResetAgent.cpp"),
            Object(NonMatching, "Osako/Award2D.cpp"),
            Object(Matching, "Osako/NameEntryHelper.cpp"),
            Object(NonMatching, "Osako/LANBackground.cpp"),
            Object(NonMatching, "Osako/LANSelectMode.cpp"),
            Object(NonMatching, "Osako/LANTitle.cpp"),
            Object(NonMatching, "Osako/LANEntry.cpp"),
            Object(NonMatching, "Osako/LANPlayInfo.cpp"),
            Object(Matching, "Osako/MenuBackground.cpp"),
            Object(Matching, "Osako/MenuTitleLine.cpp"),
            Object(Matching, "Osako/GIRecord.cpp"),
            Object(Matching, "Osako/password.cpp"),
            Object(Matching, "Osako/screenshot.c")
        ]
    ),
    GameLib(
        "Sato",
        [],
        [
            Object(NonMatching, "Sato/logbridge.cpp"),
            Object(Matching, "Sato/stMath.cpp"),
            Object(NonMatching, "Sato/GeographyObjMgr.cpp"),
            Object(Matching, "Sato/GeographyObj.cpp"),
            Object(Matching, "Sato/GeoTree.cpp"),
            Object(Matching, "Sato/ObjCollision.cpp"),
            Object(Matching, "Sato/GeoRFlower.cpp"),
            Object(Matching, "Sato/GeoJumpNarrow.cpp"),
            Object(Matching, "Sato/GeoItemBox.cpp"),
            Object(NonMatching, "Sato/JPEffectMgr.cpp"),
            Object(NonMatching, "Sato/stParticle.cpp"),
            Object(NonMatching, "Sato/stEmitter.cpp"),
            Object(NonMatching, "Sato/stEffectMgr.cpp"),
            Object(NonMatching, "Sato/stEfctExhaust.cpp"),
            Object(NonMatching, "Sato/ItemObjMgr.cpp"),
            Object(Matching, "Sato/ItemGTurtle.cpp"),
            Object(NonMatching, "Sato/ItemObj.cpp"),
            Object(NonMatching, "Sato/ItemBanana.cpp"),
            Object(Matching, "Sato/stEfctWeed.cpp"),
            Object(Matching, "Sato/ItemKinoko.cpp"),
            Object(NonMatching, "Sato/ItemStar.cpp"),
            Object(NonMatching, "Sato/J3DEffectMgr.cpp"),
            Object(Matching, "Sato/StringObj.cpp"),
            Object(NonMatching, "Sato/ItemWanWan.cpp"),
            Object(NonMatching, "Sato/ItemBomb.cpp"),
            Object(NonMatching, "Sato/ObjUtility.cpp"),
            Object(NonMatching, "Sato/JPEffectPerformer.cpp"),
            Object(NonMatching, "Sato/ItemSuccession.cpp"),
            Object(NonMatching, "Sato/J3DEfctBomb.cpp"),
            Object(NonMatching, "Sato/J3DAnmObject.cpp"),
            Object(Matching, "Sato/GeoObjPathMove.cpp"),
            Object(NonMatching, "Sato/EffectScreen.cpp"),
            Object(NonMatching, "Sato/stEfctRoad.cpp"),
            Object(Matching, "Sato/AnmController.cpp"),
            Object(Matching, "Sato/RivalKart.cpp"),
            Object(Matching, "Sato/StateObserver.cpp"),
            Object(NonMatching, "Sato/NeckCtrl.cpp"),
            Object(NonMatching, "Sato/LumbarCtrl.cpp"),
            Object(NonMatching, "Sato/ArmCtrl.cpp"),
            Object(NonMatching, "Sato/ItemThunder.cpp"),
            Object(Matching, "Sato/ItemFlyTurtle.cpp"),
            Object(NonMatching, "Sato/stEfctWater.cpp"),
            Object(NonMatching, "Sato/J3DEfctSplashDown.cpp"),
            Object(Matching, "Sato/ItemYoshiEgg.cpp"),
            Object(NonMatching, "Sato/RivalItemCtrl.cpp"),
            Object(NonMatching, "Sato/RivalBodyCtrl.cpp"),
            Object(NonMatching, "Sato/RivalSpeedCtrl.cpp"),
            Object(NonMatching, "Sato/J3DEfctIceRock.cpp"),
            Object(Matching, "Sato/ItemHeart.cpp"),
            Object(NonMatching, "Sato/J3DEfctHeart.cpp")
        ]
    ),
    GameLib(
        "Shiraiwa",
        [],
        [
            Object(NonMatching, "Shiraiwa/Coord3D.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjDossun.cpp"),
            Object(NonMatching, "Shiraiwa/JugemMain.cpp"),
            Object(Matching, "Shiraiwa/JugemLap.cpp"),
            Object(NonMatching, "Shiraiwa/Interp.cpp"),
            Object(Equivalent, "Shiraiwa/JugemReverse.cpp"),
            Object(Matching, "Shiraiwa/JugemRescue.cpp"),
            Object(Matching, "Shiraiwa/JugemVoidRod.cpp"),
            Object(Matching, "Shiraiwa/JugemStart.cpp"),
            Object(Matching, "Shiraiwa/JugemRodBoard.cpp"),
            Object(Matching, "Shiraiwa/JugemRodSignal.cpp"),
            Object(Matching, "Shiraiwa/JugemGoal.cpp"),
            Object(Matching, "Shiraiwa/JugemFlag.cpp"),
            Object(Equivalent, "Shiraiwa/MapObjWanwan.cpp"),
            Object(Matching, "Shiraiwa/MapObjWanwanChain.cpp"),
            Object(Matching, "Shiraiwa/AnmPlayer.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjSkyShip.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjDonkyRockGen.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjDonkyRock.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjNoMove.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjPeachTree.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjFireBar.cpp"),
            Object(Matching, "Shiraiwa/MapObjHioNode.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjFireObj.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjPoihana.cpp"),
            Object(NonMatching, "Shiraiwa/Coord3DUtil.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjWall.cpp"),
            Object(NonMatching, "Shiraiwa/SmallAnimalGen.cpp"),
            Object(NonMatching, "Shiraiwa/SMAButterfly.cpp"),
            Object(Matching, "Shiraiwa/SiUtil.cpp"),
            Object(NonMatching, "Shiraiwa/SMABirdGroup.cpp"),
            Object(NonMatching, "Shiraiwa/SMABird2.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjGeyser.cpp"),
            Object(NonMatching, "Shiraiwa/TestString.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjDonkyTree.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjSnowRock.cpp"),
            Object(NonMatching, "Shiraiwa/LensFlare.cpp"),
            Object(NonMatching, "Shiraiwa/TestRollObj.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjFountain.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjHeyho.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjSanbo.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjWluArrow.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjSnowMan.cpp"),
            Object(NonMatching, "Shiraiwa/DistSnow.cpp"),
            Object(Matching, "Shiraiwa/MapObjIceBlock.cpp"),
            Object(Matching, "Shiraiwa/WipeSlide.cpp"),
            Object(Matching, "Shiraiwa/WipeBase.cpp"),
            Object(Matching, "Shiraiwa/WipeCircle.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjSun.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjBubble.cpp"),
            Object(Matching, "Shiraiwa/JugemHeadLamp.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjDesun.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjSandPillar.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjFerriswheel.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjMoveItemBox.cpp"),
            Object(NonMatching, "Shiraiwa/Balloon.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjJetCoaster.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjPool.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjKpFire.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjRollTable.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjNossie.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjMare.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjMonte.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjMeteor.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjDemoObj.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjAwardItemBox.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjHanabi.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjAntLion.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjPakkun.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjKpGear.cpp"),
            Object(NonMatching, "Shiraiwa/SMABoidBird.cpp"),
            Object(NonMatching, "Shiraiwa/BoidsManager.cpp"),
            Object(Matching, "Shiraiwa/WipeCheck.cpp"),
            Object(Matching, "Shiraiwa/WipeShut.cpp"),
            Object(Matching, "Shiraiwa/WipeLine.cpp"),
            Object(Matching, "Shiraiwa/WipeWave.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjVentilator.cpp"),
            Object(NonMatching, "Shiraiwa/MapObjYoshiHeli.cpp"),
            Object(NonMatching, "Shiraiwa/ShineArrow.cpp"),
            Object(Matching, "Shiraiwa/ZCaptureMgr.cpp"),
            Object(Matching, "Shiraiwa/WipeCurtain.cpp"),
            Object(Matching, "Shiraiwa/JugemDemo.cpp"),
            Object(Matching, "Shiraiwa/SecretScene.cpp")
        ]
    ),
    GameLib(
        "Yamamoto",
        ["-inline off"],
        [
            Object(Equivalent, "Yamamoto/kartBody.cpp"),
            Object(Matching, "Yamamoto/kartCamera.cpp"),
            Object(Matching, "Yamamoto/kartCtrl.cpp"),
            Object(NonMatching, "Yamamoto/kartSus.cpp"),
            Object(Matching, "Yamamoto/kartvec.cpp"),
            Object(NonMatching, "Yamamoto/kartParams.cpp"),
            Object(NonMatching, "Yamamoto/kartBodyStrat.cpp"),
            Object(NonMatching, "Yamamoto/kartCtrlStrat.cpp"),
            Object(Matching, "Yamamoto/kartLine.cpp"),
            Object(Matching, "Yamamoto/kartCtrlInfo.cpp"),
            Object(NonMatching, "Yamamoto/kartCamStrat.cpp"),
            Object(NonMatching, "Yamamoto/KartAnime.cpp"),
            Object(NonMatching, "Yamamoto/KartItemStrat.cpp"),
            Object(Matching, "Yamamoto/KartSound.cpp"),
            Object(Matching, "Yamamoto/KartTaeget.cpp"),
            Object(NonMatching, "Yamamoto/KartRescue.cpp"),
            Object(NonMatching, "Yamamoto/KartCannon.cpp"),
            Object(Matching, "Yamamoto/KartDossin.cpp"),
            Object(NonMatching, "Yamamoto/KartTumble.cpp"),
            Object(Matching, "Yamamoto/KartChecker.cpp"),
            Object(Matching, "Yamamoto/KartDamage.cpp"),
            Object(Matching, "Yamamoto/KartStar.cpp"),
            Object(Matching, "Yamamoto/KartTire.cpp"),
            Object(Matching, "Yamamoto/KartThunder.cpp"),
            Object(NonMatching, "Yamamoto/KartCrash.cpp"),
            Object(NonMatching, "Yamamoto/KartDemoCam.cpp"),
            Object(NonMatching, "Yamamoto/KartFitParam.cpp"),
            Object(Matching, "Yamamoto/KartDisp.cpp"),
            Object(Matching, "Yamamoto/KartHandle.cpp"),
            Object(Matching, "Yamamoto/KartApeendix.cpp"),
            Object(Matching, "Yamamoto/KartPipe.cpp"),
            Object(Matching, "Yamamoto/KartAnt.cpp"),
            Object(NonMatching, "Yamamoto/kartChapter.cpp"),
            Object(Matching, "Yamamoto/KartPerCam.cpp")
        ]
    ),
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("jsys", "JSystem"),
    ProgressCategory("sdk", "Dolphin SDK"),
    ProgressCategory("msl", "MSL"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress and write progress.json
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
