set( PyInstrumentPlatforms_HEADERS
    include/PyInstrumentPlatforms/InstrumentPlatformsPy.hpp
    )

set( PyInstrumentPlatforms_SOURCES
    src/PyInstrumentPlatforms.cpp
    )

set( PyInstrumentPlatforms_PYTHON_SOURCES
    # Python/__init__.py.in
    )

add_library( PyInstrumentPlatforms MODULE ${PyInstrumentPlatforms_SOURCES} ${PyInstrumentPlatforms_HEADERS} ${PyInstrumentPlatforms_PYTHON_SOURCES} )  


# SET_TARGET_PROPERTIES( PyInstrumentPlatforms PROPERTIES PREFIX "" FOLDER "Python Modules/PRo3D-Extensions" )
# SET_TARGET_PROPERTIES( PyInstrumentPlatforms PROPERTIES PREFIX "" )

if ( WIN32 )
set_target_properties( PyInstrumentPlatforms PROPERTIES SUFFIX ".pyd" )
#                             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
endif ( WIN32 )

# TARGET_LINK_LIBRARIES( PyInstrumentPlatforms
#     InstrumentPlatforms
#     ${BOOST_PYTHON_LIBRARY}
#     ${PYTHON_LIBRARIES}
#     )

# target_link_libraries(PyInstrumentPlatforms PRIVATE InstrumentPlatforms Boost::filesystem Boost::log Boost::python39 optimized Python3::Python)

target_include_directories(PyInstrumentPlatforms PRIVATE include ${CMAKE_CURRENT_BINARY_DIR} ${Python3_INCLUDE_DIR})
target_link_libraries(PyInstrumentPlatforms PRIVATE InstrumentPlatforms Boost::filesystem Boost::log Boost::python39 ${Python3_LIBRARY_RELEASE})

# ADD_DEPENDENCIES( PyInstrumentPlatforms
    # )

# # message(${CMAKE_INSTALL_BINDIR})
# install(TARGETS PyInstrumentPlatforms #RUNTIME DESTINATION ${PRO3D_EXTENSIONS_INSTALL_BINDIR_NAME} COMPONENT runtime
#                                     LIBRARY DESTINATION COMPONENT runtime )
#                                     # ARCHIVE DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME} COMPONENT develop)

# CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/Python/__init__.py.in
#                 ${CMAKE_CURRENT_BINARY_DIR}/__init__.py )

# IF ( WIN32 )
    install( TARGETS PyInstrumentPlatforms
        RUNTIME DESTINATION bin COMPONENT runtime
        LIBRARY DESTINATION bin COMPONENT runtime
        ARCHIVE DESTINATION lib COMPONENT develop
        )
# ELSE ( WIN32 )
#     INSTALL( TARGETS PyInstrumentPlatforms
#         RUNTIME DESTINATION ${PRO3D_EXTENSIONS_INSTALL_BINDIR_NAME} COMPONENT runtime
#         LIBRARY DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME} COMPONENT runtime
#         ARCHIVE DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME} COMPONENT develop
#         )
# ENDIF ( WIN32 )
# INSTALL( DIRECTORY DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME}/InstrumentPlatforms
#     DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT runtime
#     )
# INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
#     DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME}/InstrumentPlatforms COMPONENT runtime
#     )
