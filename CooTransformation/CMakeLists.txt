include(GenerateExportHeader)

set(CooTransformation_HEADERS
    include/CooTransformation/CooTransformation.hpp
)

set(CooTransformation_SOURCES
    src/CooTransformation.cpp
)



add_library(CooTransformation SHARED ${CooTransformation_SOURCES} ${CooTransformation_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/CooTransformation/CooTransformationExport.hpp)
generate_export_header(CooTransformation PREFIX_NAME  JR_PRO3D_EXTENSIONS_ EXPORT_FILE_NAME CooTransformation/CooTransformationExport.hpp)
target_include_directories(CooTransformation PUBLIC include ${CSPICE_INCLUDE_DIR})
target_include_directories(CooTransformation PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(CooTransformation PRIVATE ${CSPICE_LIBRARY_RELEASE})
set_target_properties(CooTransformation PROPERTIES CXX_STANDARD 17)


install(TARGETS CooTransformation   RUNTIME DESTINATION bin COMPONENT runtime
                                    LIBRARY DESTINATION bin COMPONENT runtime
                                    ARCHIVE DESTINATION lib COMPONENT develop)

install(FILES ${CooTransformation_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/CooTransformation/CooTransformationExport.hpp DESTINATION include/CooTransformation COMPONENT develop)


if(USE_PYTHON)
    add_subdirectory(python)
endif()
