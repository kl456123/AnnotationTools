aux_source_directory(. Module_SRCS)

find_package(VTK COMPONENTS
    vtkCommonColor
    vtkCommonCore
    vtkCommonDataModel
    vtkFiltersSources
    vtkIOImage
    vtkInteractionImage
    vtkInteractionStyle
    vtkRenderingContextOpenGL2
    vtkRenderingCore
    vtkRenderingFreeType
    vtkRenderingGL2PSOpenGL2
    vtkRenderingOpenGL2 QUIET)

if (NOT VTK_FOUND)
    message("Skipping AnnotationTools: ${VTK_NO_FOUND_MESSAGE}")
    return ()
endif()
message(STATUS "VTK_VERSION: ${VTK_VERSION}")
message(STATUS "VTK_LIBRARIES: ${VTK_LIBRARIES}")


include_directories(../Core)
add_library(DataLoader ${Module_SRCS})
target_link_libraries(DataLoader ${VTK_LIBRARIES})
