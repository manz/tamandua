set(gamandua_src
	gamandua.c
	gamandua_callbacks.c
	)

IF(GTK2_FOUND)
	INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS} gamandua)
	ADD_EXECUTABLE(gamandua ${gamandua_src})
	TARGET_LINK_LIBRARIES(gamandua ${GTK2_LIBRARIES} td_base td_core)
ENDIF(GTK2_FOUND)

