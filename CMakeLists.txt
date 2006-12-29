PROJECT(Tamandua)
FIND_PACKAGE(Qt4 REQUIRED)

include(${QT_USE_FILE})
#Myrmidon
set(myrmidon_src
		td_gui/wrap/Context.cc
		td_gui/wrap/Problem.cc
		td_gui/wrap/Strategy.cc
		td_gui/wrap/Wrap.cc
		td_gui/ProblemDialog.cc
		td_gui/main.cc
   )

set(td_core_src
    td_core/td_core.c
   )

set(td_base_src
    td_base/td_base.c
   )

set(moc_hdr
		td_gui/ProblemDialog.h
		)

SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_library(td_base SHARED ${td_base_src})
add_library(td_core SHARED ${td_core_src})

target_link_libraries(td_core td_base)

if (APPLE)
  SET_TARGET_PROPERTIES(td_base td_core PROPERTIES COMPILE_FLAGS "-fno-common")
endif (APPLE)

include_directories(td_base td_core)
add_library(dispatch MODULE problems/dispatch/dispatch.c)
add_library(smith MODULE problems/smith/smith.c)

QT4_WRAP_CPP(moc_src ${moc_hdr})

add_executable(myrmidon MACOSX_BUNDLE ${myrmidon_src} ${moc_src})

if (APPLE)
	set_target_properties(myrmidon PROPERTIES LINK_FLAGS "-bind_at_load")
endif (APPLE)

target_link_libraries(myrmidon ${QT_LIBRARIES} td_base td_core)
