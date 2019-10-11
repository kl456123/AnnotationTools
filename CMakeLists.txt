cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

project(AnnotationTools)
set(AnnotationTools_VERSION_MAJOR 1)
set(AnnotationTools_VERSION_MINOR 0)


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


include_directories(./Core)
add_subdirectory(Core)
if (VTK_VERSION VERSION_LESS "8.90.0")
    # old system
    include(${VTK_USE_FILE})
    add_executable(AnnotationTools DataLoader/PointCloudDataLoader.cxx)
    add_executable(PNGReader DataLoader/ImageDataLoader.cxx)
    add_executable(Actor2D Actors/Actor2D.cxx)
    add_executable(AreaPicking Pickers/AreaPicking.cxx)
    add_executable(RubberBandPicker Pickers/RubberBandPicker.cxx)
    add_executable(KeypressEvents Callbacks/KeypressEvents.cxx )
    add_executable(BoxWidgets Widgets/BoxWidgets.cxx)
    add_executable(MultipleWindows ViewPorts/MultipleWindows.cxx)
    add_executable(ImageView ViewPorts/ImageView.cxx)
    add_executable(Test Tests/test.cxx)
    target_link_libraries(AnnotationTools PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(PNGReader PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(Actor2D PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(AreaPicking PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(RubberBandPicker PRIVATE ${VTK_LIBRARIES} AnnotationCore)
    target_link_libraries(KeypressEvents PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(BoxWidgets PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(MultipleWindows PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(ImageView PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(Test AnnotationCore)
else()
    add_executable(AnnotationTools DataLoader/PointCloudDataLoader.cxx)
    add_executable(PNGReader DataLoader/ImageDataLoader.cxx)
    add_executable(Actor2D Actors/Actor2D.cxx)
    add_executable(AreaPicking Pickers/AreaPicking.cxx)
    add_executable(RubberBandPicker Pickers/RubberBandPicker.cxx)
    add_executable(KeypressEvents Callbacks/KeypressEvents.cxx )
    add_executable(BoxWidgets Widgets/BoxWidgets.cxx)
    add_executable(MultipleWindows ViewPorts/MultipleWindows.cxx)
    add_executable(ImageView ViewPorts/ImageView.cxx)
    add_executable(Test Tests/test.cxx)
    target_link_libraries(AnnotationTools PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(PNGReader PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(Actor2D PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(AreaPicking PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(RubberBandPicker PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(KeypressEvents PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(BoxWidgets PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(MultipleWindows PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(ImageView PRIVATE ${VTK_LIBRARIES})
    target_link_libraries(Test AnnotationCore)
    vtk_module_autoinit(
        TARGETS AnnotationTools
        MODULES ${VTK_LIBRARIES}
        )
endif()

