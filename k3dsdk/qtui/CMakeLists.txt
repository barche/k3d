project(k3dsdk-qtui)

add_definitions(-DQT_QML_DEBUG)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

qt5_add_resources(RESOURCES templates.qrc)

set(HEADERS
  application.h
  convert.h
  document.h
  dynamic_gui_item.h
	dynamic_gui_root.h
	events.h
  glew_context.h
	gui_element.h
  node_wrapper.h
  property_wrapper.h
	split_view.h
	utility.h
  viewport.h
)

set(SOURCES
  application.cpp
  document.cpp
  dynamic_gui_item.cpp
	dynamic_gui_root.cpp
	events.cpp
  glew_context.cpp
	gui_element.cpp
  node_wrapper.cpp
	property_wrapper.cpp
	split_view.cpp
	utility.cpp
  viewport.cpp
)

k3d_add_library(k3dsdk-qtui SHARED ${RESOURCES} ${HEADERS} ${SOURCES} ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
k3d_generate_def_file(k3dsdk-qtui)

target_link_libraries(k3dsdk-qtui k3dsdk k3dsdk-opengl Qt5::Core Qt5::Widgets Qt5::Quick)

install(TARGETS k3dsdk-qtui
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR}
	)
