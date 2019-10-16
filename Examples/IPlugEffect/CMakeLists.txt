cmake_minimum_required (VERSION 3.15)

# add your source and header files here
set( PLUGIN_SRC
  ${CMAKE_SOURCE_DIR}/IPlugEffect.cpp
  ${CMAKE_SOURCE_DIR}/config.h
)

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
set( IPLUG2_ROOT /Users/oli/Dev/MyPlugins )

#set( IPLUG2_ROOT $ENV{IPLUG2} )
# need the quotes to compare with empty string
#if ( "${IPLUG2_ROOT}" STREQUAL "")
#  message( FATAL_ERROR
#    "Please set the environment variable IPLUG2 to the location of your iPlug2 install."
#    "\n"
#    "If it is already set, if this is Windows please make sure that the path uses forward slashes "
#	"- CMake doesn't use backslashes."
#  )
#endif()
#message( "iPlug2 root dir: ${IPLUG2_ROOT}" )


# store config.h in variable for reparsing with getDefineFromConfig function
file(STRINGS "${CMAKE_SOURCE_DIR}/config.h" _CONFIG_CONTENT)
if ( "${_CONFIG_CONTENT}" STREQUAL "")
    message( FATAL_ERROR "Error: config.h not found. It should be in the same directory as CMakeLists.txt.")
endif()

function( getDefineFromConfig  def_name _retval )
  foreach (_LINE ${_CONFIG_CONTENT})
    if ("${_LINE}" MATCHES "^[ \t]*#[ \t]*define[ \t]*${def_name}[ \t]*[<\"]([^>\"]*)[>\"]")
      set( ${_retval} ${CMAKE_MATCH_1} PARENT_SCOPE )
	  return()
    endif()
  endforeach()
endfunction()

set( retval )
getDefineFromConfig("PLUG_NAME" retval)
set( PLUG_NAME ${retval} )

if ("${PLUG_NAME}" STREQUAL "")
  message( FATAL_ERROR "Error: PLUG_NAME not defined in config.h." )
endif()

set( GRAPHICS_BACKEND "AGG" ) 

# graphics backend options are NO_IGRAPHICS, IGRAPHICS_LICE, IGRAPHICS_CAIRO etc
#getDefineFromConfig("GRAPHICS_BACKEND" retval)
#set( GRAPHICS_BACKEND ${retval} ) 
#if ("${GRAPHICS_BACKEND}" STREQUAL "")
#  message( FATAL_ERROR "Error: GRAPHICS_BACKEND not defined in config.h." )
#endif()

# TODO: probably check that it's a valid value

message( "Creating build files for '${PLUG_NAME}' using '${GRAPHICS_BACKEND}' graphics backend..." )

if (GRAPHICS_BACKEND STREQUAL "AGG")
  message( "  - Using AGG graphics backend" )
  add_compile_options( -DIGRAPHICS_AGG )
  add_library( GRAPHICS_LIB 
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsAGG.h
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsAGG_src.cpp
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsAGG_src.h
  )

elseif (GRAPHICS_BACKEND STREQUAL "CAIRO")
  message( "  - Using Cairo graphics backend" )
  add_compile_options( -DIGRAPHICS_CAIRO )
  include_directories(
    ${IPLUG2_ROOT}/Dependencies/IGraphics/glad_GL2/include
    ${IPLUG2_ROOT}/Dependencies/IGraphics/glad_GL2/src
  )
  add_library( GRAPHICS_LIB 
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsCairo.h
  )

elseif (GRAPHICS_BACKEND STREQUAL "LICE")
  message( "  - Using LICE graphics backend" )
  add_compile_options( -DIGRAPHICS_LICE )
  add_library( GRAPHICS_LIB 
    #${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsLice.cpp
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsLice.h   
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsLice_src.cpp
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsLice_src.h
  )
  
  include_directories(
    ${IPLUG2_ROOT}/WDL/lice
    ${IPLUG2_ROOT}/IGraphics/Drawing
  )

elseif (GRAPHICS_BACKEND STREQUAL "NANOVG")  
  message( "  - Using NanoVG graphics backend" )
  add_compile_options( -DIGRAPHICS_NANOVG -DIGRAPHICS_GL2 )
  include_directories(
    ${IPLUG2_ROOT}/Dependencies/IGraphics/glad_GL2/include
    ${IPLUG2_ROOT}/Dependencies/IGraphics/glad_GL2/src
  )
endif()

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )


project( ${PLUG_NAME} CXX ) 
set( APP_TARGET "${PLUG_NAME}-app" )
add_executable( ${APP_TARGET} )
target_compile_features( ${APP_TARGET} PUBLIC cxx_std_14 )

set( VST2_TARGET "${PLUG_NAME}-vst2" )
add_library( ${VST2_TARGET} SHARED )
target_compile_features( ${VST2_TARGET} PUBLIC cxx_std_14 )

set( VST3_TARGET "${PLUG_NAME}-vst3" )
add_library( ${VST3_TARGET} SHARED )
target_compile_features( ${VST3_TARGET} PUBLIC cxx_std_14 )
#set_target_properties(${VST3_TARGET} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

# set( AU_TARGET "${PLUG_NAME}-au" )
# add_library( ${AU_TARGET} SHARED )
# set_target_properties(${AU_TARGET} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
# 
# set( AAX_TARGET "${PLUG_NAME}-aax" )
# add_library( ${AAX_TARGET} SHARED )
# set_target_properties(${AAX_TARGET} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

project( ${PLUG_NAME} CXX ) 

message( "Adding files for standalone target..." )
  target_include_directories( ${APP_TARGET} PUBLIC
    ${CMAKE_SOURCE_DIR}/resources
    ${IPLUG2_ROOT}/IPlug/APP
	${IPLUG2_ROOT}/Dependencies/IPlug/RTAudio
	${IPLUG2_ROOT}/Dependencies/IPlug/RTAudio/include
	${IPLUG2_ROOT}/Dependencies/IPlug/RTMidi
  )
  target_sources( ${APP_TARGET} PUBLIC
    ${IPLUG2_ROOT}/IPlug/APP/iPlugAPP.cpp
    ${IPLUG2_ROOT}/IPlug/APP/iPlugAPP_dialog.cpp
    ${IPLUG2_ROOT}/IPlug/APP/iPlugAPP_host.cpp
    ${IPLUG2_ROOT}/IPlug/APP/iPlugAPP_main.cpp
	${IPLUG2_ROOT}/Dependencies/IPlug/RTMidi/RtMidi.cpp
	${IPLUG2_ROOT}/Dependencies/IPlug/RTAudio/RtAudio.cpp
  )

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

message( "Adding files for VST2 target..." )
  target_include_directories( ${VST2_TARGET} PUBLIC
    ${IPLUG2_ROOT}/IPlug/VST2
    ${IPLUG2_ROOT}/Dependencies/IPlug/VST2_SDK
  )
  target_sources( ${VST2_TARGET} PUBLIC
    ${IPLUG2_ROOT}/Dependencies/IPlug/VST2_SDK/aeffect.h
    ${IPLUG2_ROOT}/Dependencies/IPlug/VST2_SDK/aeffectx.h
    ${IPLUG2_ROOT}/IPlug/VST2/IPlugVST2.cpp
    ${IPLUG2_ROOT}/IPlug/VST2/IPlugVST2.h
  )

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

message( "Adding files for VST3 target . . ." )
  set( VST3_DIR ${IPLUG2_ROOT}/Dependencies/IPlug/VST3_SDK )

  target_include_directories( ${VST3_TARGET} PUBLIC
    ${IPLUG2_ROOT}/IPlug/VST3
    ${VST3_DIR}
  )
  
  target_sources( ${VST3_TARGET} PUBLIC
    ${VST3_DIR}/base/source/baseiids.cpp
    ${VST3_DIR}/base/source/classfactoryhelpers.h
    ${VST3_DIR}/base/source/fbuffer.h
    ${VST3_DIR}/base/source/fbuffer.cpp
    ${VST3_DIR}/base/source/fcleanup.h
    ${VST3_DIR}/base/source/fcommandline.h
    ${VST3_DIR}/base/source/fdebug.h
    ${VST3_DIR}/base/source/fdebug.cpp
    ${VST3_DIR}/base/source/fdynlib.cpp
    ${VST3_DIR}/base/source/fdynlib.h
    ${VST3_DIR}/base/source/fobject.h
    ${VST3_DIR}/base/source/fobject.cpp
    ${VST3_DIR}/base/source/fstdmethods.h
    ${VST3_DIR}/base/source/fstreamer.cpp
    ${VST3_DIR}/base/source/fstreamer.h
    ${VST3_DIR}/base/source/fstring.h
    ${VST3_DIR}/base/source/fstring.cpp
    ${VST3_DIR}/base/source/hexbinary.h
    ${VST3_DIR}/base/source/timer.h
    ${VST3_DIR}/base/source/updatehandler.cpp
    ${VST3_DIR}/base/source/updatehandler.h

    #${VST3_DIR}/base/thread/source/fcondition.cpp
    ${VST3_DIR}/base/thread/source/flock.cpp

    ${VST3_DIR}/pluginterfaces/base/conststringtable.h
    ${VST3_DIR}/pluginterfaces/base/conststringtable.cpp
    ${VST3_DIR}/pluginterfaces/base/coreiids.cpp
    ${VST3_DIR}/pluginterfaces/base/funknown.cpp
    ${VST3_DIR}/pluginterfaces/base/funknown.h
    ${VST3_DIR}/pluginterfaces/base/ustring.cpp
    ${VST3_DIR}/pluginterfaces/base/ustring.h

    ${VST3_DIR}/public.sdk/source/common/commoniids.cpp
    ${VST3_DIR}/public.sdk/source/common/memorystream.h
    ${VST3_DIR}/public.sdk/source/common/memorystream.cpp 
    ${VST3_DIR}/public.sdk/source/common/pluginview.h
    ${VST3_DIR}/public.sdk/source/common/pluginview.cpp

    ${VST3_DIR}/public.sdk/source/main/pluginfactory.cpp
    ${VST3_DIR}/public.sdk/source/main/pluginfactory.h

    ${VST3_DIR}/public.sdk/source/vst/vstaudioeffect.h
    ${VST3_DIR}/public.sdk/source/vst/vstaudioeffect.cpp
    ${VST3_DIR}/public.sdk/source/vst/vstbus.h
    ${VST3_DIR}/public.sdk/source/vst/vstbus.cpp
    ${VST3_DIR}/public.sdk/source/vst/vstcomponent.h
    ${VST3_DIR}/public.sdk/source/vst/vstcomponent.cpp
    ${VST3_DIR}/public.sdk/source/vst/vstcomponentbase.h
    ${VST3_DIR}/public.sdk/source/vst/vstcomponentbase.cpp
    ${VST3_DIR}/public.sdk/source/vst/vstinitiids.cpp
    ${VST3_DIR}/public.sdk/source/vst/vstparameters.h
    ${VST3_DIR}/public.sdk/source/vst/vstparameters.cpp
    ${VST3_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.h
    ${VST3_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp

    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3_Common.h
    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3_ControllerBase.h
    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3_View.h
    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3.h
    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3.cpp
    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3_ProcessorBase.h
    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3_ProcessorBase.cpp
    ${IPLUG2_ROOT}/IPlug/VST3/IPlugVST3.h
  )


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

message( "Adding iPlug include directories..." )

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${IPLUG2_ROOT}/WDL/libpng
  ${IPLUG2_ROOT}/WDL/zlib
  ${IPLUG2_ROOT}/IPlug
  ${IPLUG2_ROOT}/IGraphics
  ${IPLUG2_ROOT}/IGraphics/Drawing
  ${IPLUG2_ROOT}/IGraphics/Controls
  ${IPLUG2_ROOT}/Dependencies/IGraphics/STB
  ${IPLUG2_ROOT}/Dependencies/IPlug/SWELL
  ${IPLUG2_ROOT}/WDL
  ${IPLUG2_ROOT}/WDL/swell
  ${IPLUG2_ROOT}/IPlug/Extras
  ${IPLUG2_ROOT}/Dependencies/IGraphics/NanoSVG/src  # needed for IGraphicsStructs.h
  ${IPLUG2_ROOT}/Dependencies/IGraphics/NanoVG/src
)

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

message( "Adding iPlug base source files..." )

set( IPLUG_SRC
  ${IPLUG2_ROOT}/IPlug/IPlugAPIBase.cpp
  ${IPLUG2_ROOT}/IPlug/IPlugAPIBase.h
  ${IPLUG2_ROOT}/IPlug/IPlugConstants.h
  ${IPLUG2_ROOT}/IPlug/IPlugDelegate_select.h
  ${IPLUG2_ROOT}/IPlug/IPlugEditorDelegate.h
  ${IPLUG2_ROOT}/IPlug/IPlugLogger.h
  ${IPLUG2_ROOT}/IPlug/IPlugMidi.h
  ${IPLUG2_ROOT}/IPlug/IPlugParameter.cpp
  ${IPLUG2_ROOT}/IPlug/IPlugParameter.h
  ${IPLUG2_ROOT}/IPlug/IPlugPaths.h
  ${IPLUG2_ROOT}/IPlug/IPlugPlatform.h
  ${IPLUG2_ROOT}/IPlug/IPlugProcessor.h
  ${IPLUG2_ROOT}/IPlug/IPlugProcessor.cpp
  ${IPLUG2_ROOT}/IPlug/IPlugPluginBase.cpp
  ${IPLUG2_ROOT}/IPlug/IPlugPluginBase.h
  ${IPLUG2_ROOT}/IPlug/IPlugQueue.h
  ${IPLUG2_ROOT}/IPlug/IPlugStructs.h
  ${IPLUG2_ROOT}/IPlug/IPlugTimer.cpp
  ${IPLUG2_ROOT}/IPlug/IPlugTimer.h
  ${IPLUG2_ROOT}/IPlug/IPlugUtilities.h
  ${IPLUG2_ROOT}/IPlug/IPlug_include_in_plug_hdr.h
  ${IPLUG2_ROOT}/IPlug/IPlug_include_in_plug_src.h
)

set( IPLUG_PLATFORM "" )

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set( IPLUG_PLATFORM
    ${IPLUG2_ROOT}/IPlug/IPlugPaths.cpp
  )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set( IPLUG_PLATFORM
    ${IPLUG2_ROOT}/IPlug/IPlugPaths.mm
  )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set( IPLUG_PLATFORM
    ${IPLUG2_ROOT}/IPlug/IPlugPaths.cpp
  )
endif()

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

message( "Adding graphics backend files..." )

set( IGRAPHICS_PLATFORM "" )

include_directories( ${IPLUG2_ROOT}/IGraphics/Platforms/ )

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set( IGRAPHICS_PLATFORM
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsWin.cpp
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsWin.h
  )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set( IGRAPHICS_PLATFORM
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsMac.mm
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsMac.h
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsMac_view.mm
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsMac_view.h
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsCoreText.mm
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsCoreText.h
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsLice_src.mm
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsAGG_src.mm
    ${IPLUG2_ROOT}/IGraphics/Drawing/IGraphicsNanoVG_src.m
  )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set( IGRAPHICS_PLATFORM
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsLinux.cpp
    ${IPLUG2_ROOT}/IGraphics/Platforms/IGraphicsLinux.h
  )
endif()

set( IGRAPHICS_BASE
  ${IPLUG2_ROOT}/IGraphics/IControl.cpp 
  ${IPLUG2_ROOT}/IGraphics/IControl.h
  ${IPLUG2_ROOT}/IGraphics/IGraphics.cpp
  ${IPLUG2_ROOT}/IGraphics/IGraphics.h
  ${IPLUG2_ROOT}/IGraphics/IGraphicsConstants.h
  ${IPLUG2_ROOT}/IGraphics/IGraphicsEditorDelegate.cpp
  ${IPLUG2_ROOT}/IGraphics/IGraphicsEditorDelegate.h
  ${IPLUG2_ROOT}/IGraphics/IGraphicsLiveEdit.h
  ${IPLUG2_ROOT}/IGraphics/IGraphicsPathBase.h
  ${IPLUG2_ROOT}/IGraphics/IGraphicsPopupMenu.h
  ${IPLUG2_ROOT}/IGraphics/IGraphicsStructs.h
  ${IPLUG2_ROOT}/IGraphics/IGraphicsUtilities.h
  ${IPLUG2_ROOT}/IGraphics/IGraphics_include_in_plug_hdr.h
  ${IPLUG2_ROOT}/IGraphics/IGraphics_include_in_plug_src.h
  ${IPLUG2_ROOT}/IGraphics/IGraphics_select.h

  ${IPLUG2_ROOT}/IGraphics/Controls/IColorPickerControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IControls.cpp
  ${IPLUG2_ROOT}/IGraphics/Controls/IControls.h
  ${IPLUG2_ROOT}/IGraphics/Controls/ICornerResizerControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IFPSDisplayControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IPopupMenuControl.cpp
  ${IPLUG2_ROOT}/IGraphics/Controls/IPopupMenuControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IRTTextControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/ITextEntryControl.cpp
  ${IPLUG2_ROOT}/IGraphics/Controls/ITextEntryControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IVKeyboardControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IVMeterControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IVMultiSliderControl.h
  ${IPLUG2_ROOT}/IGraphics/Controls/IVScopeControl.h 
)

set( SOURCES
  ${PLUGIN_SRC}
  ${IPLUG_SRC}
  ${IPLUG_PLATFORM}
  ${IGRAPHICS_PLATFORM}
  ${IGRAPHICS_BASE}
  ${HEADER_FILES}
  ${CMAKE_SOURCE_DIR}/resources/IPlugEffect.ico 
  ${CMAKE_SOURCE_DIR}/resources/main.rc
)

include_directories( ${CMAKE_SOURCE_DIR}/resources/fonts )
set_source_files_properties( IPlugEffect.ico main.rc PROPERTIES LANGUAGE RC )

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
#     platform specifics: 
#	    
#		COMMON_DEFS and COMMON_LIBS are, unsurprisingly, common between all targets on a platform
#
#
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set( CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll )
    set( COMMON_DEFS
        -DNOMINMAX -D_WIN32 -DIPLUG_EDITOR=1 -DIPLUG_DSP=1 -D_CRT_SECURE_NO_WARNINGS
    )
	message ( "GB = '${COMMON_DEFS}'")
    target_sources( ${APP_TARGET} PUBLIC
        ${IPLUG2_ROOT}/Dependencies/IPlug/RTAudio/include/asio.cpp
      	${IPLUG2_ROOT}/Dependencies/IPlug/RTAudio/include/asiodrivers.cpp
      	${IPLUG2_ROOT}/Dependencies/IPlug/RTAudio/include/asiolist.cpp
      	${IPLUG2_ROOT}/Dependencies/IPlug/RTAudio/include/iasiothiscallresolver.cpp
    )
    target_sources( ${VST3_TARGET} PUBLIC
        ${VST3_DIR}/public.sdk/source/main/dllmain.cpp
    )
    set_property( TARGET ${APP_TARGET} APPEND_STRING PROPERTY LINK_FLAGS 
        "/MANIFEST /NXCOMPAT /DYNAMICBASE /SUBSYSTEM:WINDOWS /NOLOGO /MACHINE:X64 /Zc:inline /fp:precise" 
    )
    # set in COMMON_LIBS: shlwapi comctl32 wininet 
    target_link_libraries( ${APP_TARGET} PUBLIC
        winmm		dsound		odbc32		odbccp32 
        ole32		oleaut32	kernel32	user32 
        uuid		gdi32		winspool	advapi32
        shell32
    )
    target_compile_definitions( ${APP_TARGET} PUBLIC
        -DWIN32 -DAPP_API -D__WINDOWS_DS__ -D__WINDOWS_MM__ -D__WINDOWS_ASIO__ -D_MBCS 
        -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE #-D_DEBUG
    )
  
    if (MSVC)
        message( "Visual Studio..." )
    	# not global for all targets?
    	add_compile_options( /wd4068 /wd4996 )  # disable unrecognised pragma warnings + POSIX deprecations
    	target_compile_options( ${APP_TARGET}  PUBLIC /wd4068 /wd4996 )
    	target_compile_options( ${VST2_TARGET} PUBLIC /wd4068 /wd4996 )
    	target_compile_options( ${VST3_TARGET} PUBLIC /wd4068 /wd4996 )
    	set( COMMON_LIBS shlwapi comctl32 wininet )
    else()
        message( FATAL_ERROR "This CMake file only supports MSVC on Windows at the moment." )
    endif()
  
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set( CMAKE_FIND_LIBRARY_SUFFIXES .a .dylib ) 
    set( COMMON_LIBS "-framework Cocoa" "-framework Carbon" "-framework CoreFoundation" "-framework CoreData" "-framework Foundation" "-framework CoreServices" "-framework Metal" "-framework MetalKit" "-framework QuartzCore" "-framework WebKit" "-framework OpenGL" )
    set( COMMON_DEFS )
    #target_precompile_headers( ${IPLUG2_ROOT}/IPlug/IPlugOBJCPrefix.pch ) #TODO

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set( CMAKE_FIND_LIBRARY_SUFFIXES .a .so )
    set( COMMON_LIBS ) # TODO
    set( COMMON_DEFS )
endif()


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
#                                  standalone
#
    message( "Adding libs and compile/link flags for standalone" )
    set_target_properties( ${APP_TARGET} PROPERTIES OUTPUT_NAME ${PLUG_NAME} )
	target_compile_definitions( ${APP_TARGET} PUBLIC ${COMMON_DEFS} )
    target_sources( ${APP_TARGET} PUBLIC ${SOURCES} ${HEADER_FILES} )
    target_link_libraries( ${APP_TARGET} PUBLIC ${COMMON_LIBS} ${GRAPHICS_LIB} ${APP_LINK_LIBS} )


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
#                                     VST2
#                                                                                      
    message( "Adding libs and compile/link flags for VST2" )
    set_target_properties( ${VST2_TARGET} PROPERTIES OUTPUT_NAME ${PLUG_NAME} )
    target_compile_definitions( ${VST2_TARGET} PUBLIC ${COMMON_DEFS} -DVST2_API -DVST_FORCE_DEPRECATED )
    target_sources( ${VST2_TARGET} PUBLIC ${SOURCES} ${HEADER_FILES} )
    target_link_libraries( ${VST2_TARGET} PUBLIC ${COMMON_LIBS} ${GRAPHICS_LIB} )
  

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# 
#                                     VST3
#
    message( "Adding libs and compile/link flags for VST3" )
	set_target_properties( ${VST3_TARGET} PROPERTIES SUFFIX ".vst3" )
    #set_target_properties( ${VST3_TARGET} PROPERTIES OUTPUT_NAME ${PLUG_NAME} )
    target_compile_definitions(  ${VST3_TARGET} PUBLIC ${COMMON_DEFS} -DVST3_API -DVST_FORCE_DEPRECATED -D_DEBUG )
    target_sources( ${VST3_TARGET} PUBLIC ${SOURCES} ${HEADER_FILES} )
    target_link_libraries( ${VST3_TARGET} PUBLIC ${COMMON_LIBS} ${GRAPHICS_LIB} )
  


#add_custom_command(TARGET ${VST_3} POST_BUILD
#		COMMAND 
#		#CALL ${CMAKE_SOURCE_DIR}/scripts/postbuild-win.bat "$(TargetExt)" "$(BINARY_NAME)" "$(Platform)" "$(COPY_VST2)" "$(TargetPath)" "$(VST2_32_PATH)" "$(VST2_64_PATH)" "$(VST3_32_PATH)" "$(VST3_64_PATH)" "$(AAX_32_PATH)" "$(AAX_64_PATH)" "$(BUILD_DIR)" "$(VST_ICON)" "$(AAX_ICON)" "$(CREATE_BUNDLE_SCRIPT)"
#	)
#)