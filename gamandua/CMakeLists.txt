set(gamandua_src
	gamandua.c
	gamandua_callbacks.c
	)

INCLUDE(UsePkgConfig)

PKGCONFIG(gtk+-2.0 GTK2_INCLUDES GTK2_LIBDIR GTK2_LINKFLAGS GTK2_CFLAGS)


MESSAGE(STATUS "DEBUG: ${GTK2_INCLUDES} ${GTK2_LIBDIR} ${GTK2_LINKFLAGS} ${}")

INCLUDE_DIRECTORIES(${GTK2_INCLUDES})
ADD_EXECUTABLE(gamandua ${gamandua_src})
SET_TARGET_PROPERTIES(gamandua PROPERTIES COMPILE_FLAGS "${GTK2_CFLAGS}")
TARGET_LINK_LIBRARIES(gamandua ${GTK2_LINKFLAGS} td_base td_core)

if (NOT APPLE)
	INSTALL(TARGETS gamandua RUNTIME DESTINATION /usr/bin)
	INSTALL(FILES gamandua_logo.png DESTINATION /usr/share/tamandua/gamandua)
	INSTALL(FILES gamandua_shaded.png DESTINATION /usr/share/tamandua/gamandua)
	INSTALL(FILES grad.png DESTINATION /usr/share/tamandua/gamandua)
	INSTALL(FILES tooltip.png DESTINATION /usr/share/tamandua/gamandua)
endif (NOT APPLE)

if (APPLE)
	set_target_properties(gamandua PROPERTIES LINK_FLAGS "-bind_at_load")
endif (APPLE)
