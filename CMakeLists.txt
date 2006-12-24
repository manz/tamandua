PROJECT(Tamandua)
FIND_PACKAGE(Qt4 REQUIRED)

include(${QT_USE_FILE})
#Myrmidon
set(myrmidon_src
		td_gui/wrap/Context.cc
		td_gui/wrap/Problem.cc
		td_gui/wrap/Strategy.cc
		td_gui/main.cc
#		td_gui/Job.cc
#		td_gui/TaskList.cc
#		td_gui/mainwindow.cc
#		td_gui/main.cc
#		td_gui/td_wrap.cc
   )

set(td_core_src
    td_core/td_core.c
   )

set(td_base_src
    td_base/td_base.c
   )

set(moc_hdr
		td_gui/td_wrap.h
		td_gui/mainwindow.h
		)

SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_library(td_base SHARED ${td_base_src})
add_library(td_core SHARED ${td_core_src})

target_link_libraries(td_core td_base)

if (APPLE)
  SET_TARGET_PROPERTIES(td_base td_core PROPERTIES COMPILE_FLAGS "-fno-common")
endif (APPLE)

include_directories(td_base td_core)
add_library(dispatch MODULE problems/dispatch.c)
#!!! Smith ne compile plus ...
#add_library(smith MODULE problems/smith.c)

#QT4_GENERATE_MOC(td_gui/td_wrap.h td_wrap.moc)
#qt4_automoc(td_gui/td_wrap.cc)

#QT4_WRAP_CPP(moc_src ${moc_hdr})


add_executable(myrmidon MACOSX_BUNDLE ${myrmidon_src} ${moc_src})
 
target_link_libraries(myrmidon ${QT_LIBRARIES} td_base td_core)
