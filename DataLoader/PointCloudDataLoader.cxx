#include <iostream>
#include <string>

// VTK header
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkElevationFilter.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCylinderSource.h"
#include "vtkNamedColors.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"

using namespace std;




int main(){

    int32_t num = 1e6;
    float *data = (float*)malloc(num*sizeof(float));


    float *px = data+0;
    float *py = data+1;
    float *pz = data+2;
    float *pr = data+3;


    string currFileNameBinary = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync/velodyne_points/data/0000000000.bin";
    FILE *stream;
    double low_z = 0;
    double high_z = 0;
    string color_name = "Reflectance";

    cout<<currFileNameBinary<<endl;
    stream = fopen(currFileNameBinary.c_str(), "rb");

    num = fread(data,sizeof(float),num,stream)/4;
    if(ferror(stream)){
        return -1;
    }
    fclose(stream);
    ///////////////////////////////////
    // load data from velodyne bin file
    ///////////////////////////////////

    auto points = vtkSmartPointer<vtkPoints>::New();
    auto colors = vtkSmartPointer<vtkDoubleArray>::New();
    colors->SetName(color_name.c_str());

    for(int i=0;i<num;i+=1){
        //insert point into Points
        points->InsertNextPoint(*px, *py, *pz);
        // 0-1
        colors->InsertNextValue(*pr);
        px+=4;
        py+=4;
        pz+=4;
        pr+=4;
    }

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->GetPointData()->SetScalars(colors);
    polyData->GetPointData()->SetActiveScalars(color_name.c_str());

    // filter
    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputData(polyData);
    // glyphFilter->Update();


    // auto elevationFilter = vtkSmartPointer<vtkElevationFilter>::New();
    // elevationFilter->SetInputConnection(glyphFilter->GetOutputPort());
    // elevationFilter->SetLowPoint(0, 0, low_z);
    // elevationFilter->SetHighPoint(0, 0, high_z);
    auto elevationFilter = glyphFilter;

    auto dataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    dataMapper->SetInputConnection(elevationFilter->GetOutputPort());
    dataMapper->SetScalarRange(0, 1);

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(dataMapper);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0,0,0);

    // camera
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Zoom(1.5);

    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(300, 300);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("PointCloud");

    // style
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

    // interactor
    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
