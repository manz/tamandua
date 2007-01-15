#Myrmidon
set(myrmidon_src
		wrap/Context.cc
		wrap/Problem.cc
		wrap/Strategy.cc
		wrap/Wrap.cc
		wrap/Generator.cc
		wrap/Job.cc
		wrap/Task.cc
		ProblemDialog.cc
		MainWindow.cc
		main.cc
   )
 
set(moc_hdr
		ProblemDialog.h
		MainWindow.h
		wrap/Wrap.h
		)

QT4_WRAP_CPP(moc_src ${moc_hdr})

add_executable(myrmidon MACOSX_BUNDLE ${myrmidon_src} ${moc_src})

if (APPLE)
	set_target_properties(myrmidon PROPERTIES LINK_FLAGS "-bind_at_load")
endif (APPLE)

target_link_libraries(myrmidon ${QT_LIBRARIES} td_base td_core)

