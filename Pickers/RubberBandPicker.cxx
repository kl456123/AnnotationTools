#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <cmath>
#include <utility>

// vtk library
#include <vtkArrowSource.h>
#include <vtkPlane.h>
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
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkAxesActor.h>
#include <vtkArrowSource.h>

// boxwidget
#include <vtkBorderWidget.h>
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkBorderRepresentation.h>

#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPropPicker.h>

#include "AnnotationStyle.h"

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif


vtkSmartPointer<vtkVertexGlyphFilter> ReadPointCloudFromBin(std::string& filename){
    int32_t num = 1e6;
    float *data = (float*)malloc(num*sizeof(float));


    float *px = data+0;
    float *py = data+1;
    float *pz = data+2;
    float *pr = data+3;


    auto currFileNameBinary = filename;
    FILE *stream;
    // string color_name = "Reflectance";

    std::cout<<currFileNameBinary<<std::endl;
    stream = fopen(currFileNameBinary.c_str(), "rb");

    num = fread(data,sizeof(float),num,stream)/4;
    if(ferror(stream)){
        return NULL;
    }
    fclose(stream);
    ///////////////////////////////////
    // load data from velodyne bin file
    ///////////////////////////////////

    auto points = vtkSmartPointer<vtkPoints>::New();
    // auto colors = vtkSmartPointer<vtkDoubleArray>::New();
    // colors->SetName(color_name.c_str());

    for(int i=0;i<num;i+=1){
        //insert point into Points
        points->InsertNextPoint(*px, *py, *pz);
        // 0-1
        // colors->InsertNextValue(*pr);
        px+=4;
        py+=4;
        pz+=4;
        pr+=4;
    }

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    // polyData->GetPointData()->SetScalars(colors);
    // polyData->GetPointData()->SetActiveScalars(color_name.c_str());
    //
    // transform polydata
    auto transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    auto transform = vtkSmartPointer<vtkTransform>::New();
    double R0_rect[16] = {9.999128000000e-01, 1.009263000000e-02, -8.511932000000e-03,0,
        -1.012729000000e-02, 9.999406000000e-01, -4.037671000000e-03,0,
        8.470675000000e-03, 4.123522000000e-03, 9.999556000000e-01, 0,
        0,0,0,1};
    double VeloToCam[16] = {6.927964000000e-03, -9.999722000000e-01, -2.757829000000e-03, -2.457729000000e-02,
        -1.162982000000e-03, 2.749836000000e-03, -9.999955000000e-01, -6.127237000000e-02,
        9.999753000000e-01, 6.931141000000e-03, -1.143899000000e-03, -3.321029000000e-01,
        0.0,0.0,0.0,1.0};
    transform->SetMatrix(R0_rect);
    transform->Concatenate(VeloToCam);
    transform_filter->SetInputData(polyData);
    transform_filter->SetTransform(transform);
    transform_filter->Update();

    // filter
    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputData(transform_filter->GetOutput());

    return glyphFilter;
}

vtkSmartPointer<vtkActor2D> ReadImageFromPNGFile(std::string& filename){
    auto pngReader = vtkSmartPointer<vtkPNGReader>::New();
    pngReader->SetFileName(filename.c_str());

    auto imageActor = vtkSmartPointer<vtkActor2D>::New();
    auto mapper = vtkSmartPointer<vtkImageMapper>::New();
    mapper->SetColorWindow(255);
    mapper->SetColorLevel(127.5);
    mapper->RenderToRectangleOn();
    imageActor->SetMapper(mapper);
    imageActor->GetMapper()->SetInputConnection(pngReader->GetOutputPort());
    // support to mapper
    imageActor->SetPosition2(1, 1);
    // color it first

    return imageActor;
}




int main(){
    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    std::string filename = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync/velodyne_points/data/0000000000.bin";

    auto glyphFilter = ReadPointCloudFromBin(filename);

    auto idFilter = vtkSmartPointer<vtkIdFilter>::New();
    idFilter->SetInputConnection(glyphFilter->GetOutputPort());
#if VTK890
    idFilter->SetCellIdsArrayName("OriginalIds");
    idFilter->SetPointIdsArrayName("OriginalIds");
#else
    idFilter->SetIdsArrayName("OriginalIds");
#endif
    idFilter->Update();

    auto surfaceFilter=vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(surfaceFilter->GetOutputPort());
    // mapper->SetInputConnection(glyphFilter->GetOutputPort());
    mapper->ScalarVisibilityOff();

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

    // image actor
    std::string imageFileName = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync/image_02/data/0000000000.png";
    auto imageActor = ReadImageFromPNGFile(imageFileName);
    auto imageRenderer = vtkSmartPointer<vtkRenderer>::New();
    imageRenderer->AddActor(imageActor);
    imageRenderer->SetViewport(0,0.5, 1,1);
    renderer->SetViewport(0,0, 1,0.5);


    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->AddRenderer(imageRenderer);
    renderWindow->SetSize(1600, 800);

    auto areaPicker = vtkSmartPointer<vtkAreaPicker>::New();

    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetPicker(areaPicker);

    renderer->SetBackground(colors->GetColor3d("Black").GetData());
    // imageRenderer->SetBackground(colors->GetColor3d("Red").GetData());

    std::cout<<"image renderer: "<<imageRenderer<<std::endl;
    std::cout<<"pc renderer: "<<renderer<<std::endl;

    // customs style
    auto style = vtkSmartPointer<AnnotationStyle>::New();
    style->SetPoints(surfaceFilter->GetOutput());
    renderWindowInteractor->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);
    style->SetImageRenderer(imageRenderer);
    style->SetPointCloudRenderer(renderer);
    // set the first
    style->InitAnnotationWidget();

    // text actor
    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("VIEW");
    textActor->SetPosition2( 10, 40 );
    textActor->GetTextProperty()->SetFontSize ( 24 );
    textActor->GetTextProperty()->SetColor ( 1.0, 0.0, 0.0 );
    renderer->AddActor2D(textActor);
    style->SetTextActor(textActor);

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

}
