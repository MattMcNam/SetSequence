cmake_minimum_required(VERSION 2.6)

project(PortalSTV)

set(PACKAGE "PortalSTV")
set(PACKAGE_VERSION "a1")
set(PACKAGE_BUGREPORT "https://github.com/SBlue/portalstv/issues")
set(PACKAGE_NAME "${PACKAGE}")
set(PACKAGE_STRING "${PACKAGE} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE}_${PACKAGE_VERSION}")
set(PACKAGE_URL "https://github.com/SBlue/portalstv")
set(VERSION "${PACKAGE_VERSION}")

include_directories(${PROJECT_BINARY_DIR})
# Put final product in 'bin' folder
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Include source and hl2sdk
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${HL2SDK}/common)
include_directories(${HL2SDK}/public)
include_directories(${HL2SDK}/public/tier0)
include_directories(${HL2SDK}/public/tier1)
include_directories(${HL2SDK}/game/server)
include_directories(${HL2SDK}/game/shared)

set(HDR_PUBLIC
	src/portalstv.h
	src/stdafx.h
	)

set(SOURCES
	src/portalstv.cpp
	)

source_group("Header Files" FILES ${HDR_PUBLIC})
source_group("Source FIles" FILES ${SOURCES})

# Setup defines, compiler options and linker options for each platform
#if(UNIX) # Common Unix settings
#	add_definitions(-Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf -DHAVE_STDINT_H -DGNUC -DNDEBUG)
#	add_definitions(-Usprintf -Ustrncpy -UPROTECTED_THINGS_ENABLE)
#	set(COMPILER_FLAGS "-fno-strict-aliasing -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Werror -Wno-uninitialized -Wno-unused -Wno-switch -msse -m32 -fno-exceptions -fno-threadsafe-statics -Wno-non-virtual-dtor -Wno-overloaded-virtual")
#	set(LINKER_FLAGS "-m32")
#	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#		add_definitions(-DOSX -D_OSX)
#		set(LINKER_FLAGS "${LINKER_FLAGS} -mmacosx-version-min=10.5 -arch i386 -dynamiclib")
#	else() # Assume Linux
#		add_definitions(-DLINUX -D_LINUX)
#		set(COMPILER_FLAGS "${COMPILER_FLAGS} -mfpmath=sse")
#		set(LINKER_FLAGS "${LINKER_FLAGS} -shared")
#	endif()
#else() # Windows
	# Force use of static libs
	# Credit SteveL & brofield on StackOverflow
	foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINOFO)
		string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
	endforeach()
	
	add_definitions(/D_CRT_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_DEPRECATE)
	set(COMPILER_FLAGS "/EHsc /DR- /W3 /nologo /Zi /TP")
	set(LINKER_FLAGS "/MACHINE:X86 /subsystem:windows /NODEFAULTLIB:libc /NODEFAULTLIB:libcd")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmt")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmtd")
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")

# Create the plugin
add_library(PortalSTV SHARED
	${HDR_PUBLIC}
	${SOURCES})

# Remove 'lib' prefix
set_target_properties(PortalSTV PROPERTIES PREFIX "")

# Link sdk libraries
#if(WIN32)
	target_link_libraries(PortalSTV ${HL2SDK}/lib/public/tier0.lib)
	target_link_libraries(PortalSTV ${HL2SDK}/lib/public/tier1.lib)
#else()
#	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#		target_link_libraries(PortalSTV ${HL2SDK}/lib/osx32/libtier0.dylib)
#	else()
#		target_link_libraries(PortalSTV ${HL2SDK}/lib/public/linux32/libtier0.so)
#	endif()
#endif()
