PROJECT(uuidgen)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3dsdk_BINARY_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(k3d-uuidgen
  main.cpp
  )

SET_TARGET_PROPERTIES(k3d-uuidgen PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${K3D_RUNTIME_OUTPUT_DIRECTORY}
  )

TARGET_LINK_LIBRARIES(k3d-uuidgen
  k3dsdk
  )

INSTALL(TARGETS k3d-uuidgen DESTINATION bin)

