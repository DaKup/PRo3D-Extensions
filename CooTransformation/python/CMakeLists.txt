set( PyCooTransformation_HEADERS
    include/PyCooTransformation/CooTransformationPy.hpp
    )

set( PyCooTransformation_SOURCES
    src/PyCooTransformation.cpp
    )

set( PyCooTransformation_PYTHON_SOURCES
    # Python/__init__.py.in
    )

add_library( PyCooTransformation MODULE ${PyCooTransformation_SOURCES} ${PyCooTransformation_HEADERS} ${PyCooTransformation_PYTHON_SOURCES} )  


# SET_TARGET_PROPERTIES( PyCooTransformation PROPERTIES PREFIX "" FOLDER "Python Modules/PRo3D-Extensions" )
# SET_TARGET_PROPERTIES( PyCooTransformation PROPERTIES PREFIX "" )

if ( WIN32 )
set_target_properties( PyCooTransformation PROPERTIES SUFFIX ".pyd" )
#                             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
endif ( WIN32 )

# TARGET_LINK_LIBRARIES( PyCooTransformation
#     CooTransformation
#     ${BOOST_PYTHON_LIBRARY}
#     ${PYTHON_LIBRARIES}
#     )

# target_link_libraries(PyCooTransformation PRIVATE CooTransformation Boost::filesystem Boost::log Boost::python39 optimized Python3::Python)
target_include_directories(PyCooTransformation PRIVATE include ${CMAKE_CURRENT_BINARY_DIR} ${Python3_INCLUDE_DIR})
target_link_libraries(PyCooTransformation PRIVATE CooTransformation Boost::filesystem Boost::log Boost::python39 ${Python3_LIBRARY_RELEASE})

# ADD_DEPENDENCIES( PyCooTransformation
    # )

# # message(${CMAKE_INSTALL_BINDIR})
# install(TARGETS PyCooTransformation #RUNTIME DESTINATION ${PRO3D_EXTENSIONS_INSTALL_BINDIR_NAME} COMPONENT runtime
#                                     LIBRARY DESTINATION COMPONENT runtime )
#                                     # ARCHIVE DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME} COMPONENT develop)

# CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/Python/__init__.py.in
#                 ${CMAKE_CURRENT_BINARY_DIR}/__init__.py )

# IF ( WIN32 )
    install( TARGETS PyCooTransformation
        RUNTIME DESTINATION bin COMPONENT runtime
        LIBRARY DESTINATION bin COMPONENT runtime
        ARCHIVE DESTINATION lib COMPONENT develop
        )
# ELSE ( WIN32 )
#     INSTALL( TARGETS PyCooTransformation
#         RUNTIME DESTINATION ${PRO3D_EXTENSIONS_INSTALL_BINDIR_NAME} COMPONENT runtime
#         LIBRARY DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME} COMPONENT runtime
#         ARCHIVE DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME} COMPONENT develop
#         )
# ENDIF ( WIN32 )
# INSTALL( DIRECTORY DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME}/CooTransformation
#     DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT runtime
#     )
# INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
#     DESTINATION ${PRO3D_EXTENSIONS_INSTALL_LIBDIR_NAME}/CooTransformation COMPONENT runtime
#     )
