#FIND_PROGRAM(ASSISTANT_COMMAND NAMES assistant assistant-mac)

#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_definitions(-DQT_QML_DEBUG)

qt5_add_resources(RESOURCES qtui.qrc)

k3d_build_qtui_module(k3d-qtui EXTRA_SOURCES ${RESOURCES} ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
k3d_create_module_proxy(k3d-qtui)

target_link_libraries(k3d-qtui k3dsdk ${K3D_BOOST_PROGRAM_OPTIONS_LIBS} Qt5::Core Qt5::Quick Qt5::Widgets)
