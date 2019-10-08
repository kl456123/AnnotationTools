#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <cmath>
#include <vtkImageResize.h>
#include <vtkImageSincInterpolator.h>

// vtk library
#include <vtkPNGReader.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkNamedColors.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImplicitBoolean.h>
#include <vtkRendererCollection.h>
#include <vtkCamera.h>
#include <vtkImplicitFunctionCollection.h>
#include <vtkMath.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapper.h>
#include <vtkProperty2D.h>

// boxwidget
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>

#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPropPicker.h>



// vtkSmartPointer<vtkActor2D> ReadImageFromPNGFile(std::string& filename){

// return imageActor;
// }

int main(int, char *[])
{

    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindowInteractor->SetRenderWindow(renderWindow);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    std::string filename = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync/image_02/data/0000000000.png";
    // auto imageActor = ReadImageFromPNGFile(imageFileName);
    auto pngReader = vtkSmartPointer<vtkPNGReader>::New();
    pngReader->SetFileName(filename.c_str());

    // interpolation
    auto interpolate = vtkSmartPointer<vtkImageSincInterpolator>::New();
    interpolate->UseWindowParameterOn();
    auto resize = vtkSmartPointer<vtkImageResize>::New();
    resize->SetInterpolator(interpolate);
    resize->SetInputConnection(pngReader->GetOutputPort());
    double newSize[] = {300,400};
    resize->SetOutputDimensions(newSize[0], newSize[1], 1);
    resize->InterpolateOff();

    auto imageActor = vtkSmartPointer<vtkActor2D>::New();
    auto mapper = vtkSmartPointer<vtkImageMapper>::New();
    mapper->SetColorWindow(255);
    mapper->SetColorLevel(127.5);
    mapper->RenderToRectangleOn();
    imageActor->SetMapper(mapper);
    imageActor->GetMapper()->SetInputConnection(pngReader->GetOutputPort());
    // imageActor->SetWidth(1);
    // imageActor->SetHeight(1);
    renderer->AddActor(imageActor);
    renderWindow->SetSize(1800, 800);
    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    renderer->SetBackground(colors->GetColor3d("Blue").GetData());
    // imageActor->SetPosition(100,100);
    imageActor->SetPosition2(1, 1);
    // imageActor->GetProperty()->SetPointSize(10);
    // imageActor->GetProperty()->Print(std::cout);
    // std::cout<<"width: "<<imageActor->GetWidth();
    // imageActor->SetScale(2);
    // imageActor->SetDisplayPosition(200, 100);

    renderWindow->AddRenderer(renderer);
    imageActor->Print(std::cout);
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
