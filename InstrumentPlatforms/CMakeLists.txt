include(GenerateExportHeader)

set(InstrumentPlatforms_HEADERS
    include/InstrumentPlatforms/InstrumentPlatforms.hpp
)

set(InstrumentPlatforms_SOURCES
    src/InstrumentPlatforms.cpp
)

add_library(InstrumentPlatforms SHARED ${InstrumentPlatforms_SOURCES} ${InstrumentPlatforms_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/InstrumentPlatforms/InstrumentPlatformsExport.hpp)
generate_export_header(InstrumentPlatforms PREFIX_NAME  JR_PRO3D_EXTENSIONS_ EXPORT_FILE_NAME InstrumentPlatforms/InstrumentPlatformsExport.hpp)
target_include_directories(InstrumentPlatforms PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(InstrumentPlatforms PRIVATE ${CSPICE_LIBRARY_RELEASE})

install(TARGETS InstrumentPlatforms RUNTIME DESTINATION bin COMPONENT runtime
                                    LIBRARY DESTINATION bin COMPONENT runtime
                                    ARCHIVE DESTINATION lib COMPONENT develop)

install(FILES ${InstrumentPlatforms_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/InstrumentPlatforms/InstrumentPlatformsExport.hpp DESTINATION include/InstrumentPlatforms COMPONENT develop)


if(USE_PYTHON)
    add_subdirectory(python)
endif()
