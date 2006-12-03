PROJECT(Tamandua)
FIND_PACKAGE(Qt4 REQUIRED)

include(${QT_USE_FILE})

set(tamandua_gui_src
    td_gui/main.cc
    td_gui/td_wrap.cc
   )

set(td_core_src
    td_core/td_core.c
   )

set(td_base_src
    td_base/td_base.c
   )

set(moc_hdr
    td_gui/td_wrap.h
   )

add_library(td_base SHARED ${td_base_src})
add_library(td_core SHARED ${td_core_src})

target_link_libraries(td_core td_base)

if (APPLE)
  SET_TARGET_PROPERTIES(td_base td_core PROPERTIES COMPILE_FLAGS "-fno-common")
endif (APPLE)

include_directories(td_base td_core)
add_library(dispatch MODULE problems/dispatch.c)

#QT4_GENERATE_MOC(td_gui/td_wrap.h td_wrap.moc)
#qt4_automoc(td_gui/td_wrap.cc)

QT4_WRAP_CPP(moc_src ${moc_hdr})


add_executable(tamandua MACOSX_BUNDLE ${tamandua_gui_src} ${moc_src})
 
target_link_libraries(tamandua ${QT_LIBRARIES} td_base td_core)
