PROJECT(Tamandua)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

FIND_PACKAGE(Qt4 REQUIRED)
#FIND_PACKAGE(GTK2)

include(${QT_USE_FILE})

set(td_core_src
    td_core/td_core.c
   )

set(td_base_src
    td_base/td_base.c
   )

SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_library(td_base SHARED ${td_base_src})
add_library(td_core SHARED ${td_core_src})

if (APPLE)
  SET_TARGET_PROPERTIES(td_base td_core PROPERTIES COMPILE_FLAGS "-fno-common")
endif (APPLE)

target_link_libraries(td_core td_base)

include_directories(td_base td_core)

add_library(dispatch MODULE problems/dispatch/dispatch.c)
add_library(smith MODULE problems/smith/smith.c)
add_library(flowshop2 MODULE problems/flowshop2/flowshop2.c)
add_library(flowshop3 MODULE problems/flowshop3/flowshop3.c)


if (NOT APPLE)
	INSTALL(TARGETS td_core td_base
			LIBRARY DESTINATION /usr/local/lib
			LIBRARY DESTINATION /usr/local/lib
			)
	INSTALL(TARGETS dispatch smith flowshop2 flowshop3 
			LIBRARY DESTINATION /usr/local/share/tamandua/problems
			LIBRARY DESTINATION /usr/local/share/tamandua/problems
			LIBRARY DESTINATION /usr/local/share/tamandua/problems
			LIBRARY DESTINATION /usr/local/share/tamandua/problems
			)
endif (NOT APPLE)

subdirs(
	gamandua
	myrmidon
	)
