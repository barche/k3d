project(k3dsdk-qtui)

add_definitions(-DQT_QML_DEBUG)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(HEADERS
  application.h
  convert.h
  document.h
  glew_context.h
  node_wrapper.h
  property_wrapper.h
  viewport.h
)

set(SOURCES
  application.cpp
  document.cpp
  glew_context.cpp
  node_wrapper.cpp
  property_wrapper.cpp
  register_qml_types.cpp
  viewport.cpp
)

k3d_add_library(k3dsdk-qtui SHARED ${HEADERS} ${SOURCES} ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
k3d_generate_def_file(k3dsdk-qtui)

target_link_libraries(k3dsdk-qtui k3dsdk k3dsdk-opengl Qt5::Core Qt5::Widgets Qt5::Quick)

install(TARGETS k3dsdk-qtui
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR}
	)
