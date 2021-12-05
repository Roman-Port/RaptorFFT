# SET installhelper_name and installhelper_version

# Set versions
include(GenerateExportHeader)
generate_export_header(${installhelper_name})
set_property(TARGET ${installhelper_name} PROPERTY VERSION ${installhelper_version})
set_property(TARGET ${installhelper_name} PROPERTY SOVERSION 3)
set_property(TARGET ${installhelper_name} PROPERTY
  INTERFACE_${installhelper_name}_MAJOR_VERSION 3)
set_property(TARGET ${installhelper_name} APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING ${installhelper_name}_MAJOR_VERSION
)


install(TARGETS ${installhelper_name} EXPORT ${installhelper_name}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/${installhelper_name}
  DESTINATION
    include
  COMPONENT
    Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${installhelper_name}/${installhelper_name}ConfigVersion.cmake"
  VERSION ${installhelper_version}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT ${installhelper_name}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${installhelper_name}/${installhelper_name}Targets.cmake"
)
configure_file(cmake/${installhelper_name}Config.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/${installhelper_name}/${installhelper_name}Config.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/${installhelper_name})
install(EXPORT ${installhelper_name}Targets
  FILE
    ${installhelper_name}Targets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/${installhelper_name}Config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/${installhelper_name}/${installhelper_name}ConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
