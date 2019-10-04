#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkCellArray.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {void PickCallbackFunction(vtkObject* caller,
        long unsigned int eventId, void* clientData, void* callData);}


int main(){
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType pid[1];
    pid[0] = points->InsertNextPoint(1.0, 0.0, 0.0);
    vertices->InsertNextCell(1, pid);
    pid[0] = points->InsertNextPoint(0.0, 0.0, 0.0);
    vertices->InsertNextCell(1, pid);
    pid[0] = points->InsertNextPoint(0.0, 1.0, 0.0);
    vertices->InsertNextCell(1, pid);

    auto polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetVerts(vertices);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polydata);

    auto actor = vtkSmartPointer<vtkActor>::New();

    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(5);
    actor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    auto areaPicker = vtkSmartPointer<vtkAreaPicker>::New();

    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetPicker(areaPicker);

    renderer->SetBackground(colors->GetColor3d("Gold").GetData());

    auto style = vtkSmartPointer<vtkInteractorStyleRubberBandPick>::New();

    style->SetCurrentRenderer(renderer);
    renderWindowInteractor->SetInteractorStyle(style);


    auto pickerCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    pickerCallback->SetCallback(PickCallbackFunction);

    // picker callback
    areaPicker->AddObserver(vtkCommand::EndPickEvent, pickerCallback);

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;


}

namespace{
    void PickCallbackFunction(vtkObject* caller,
            long unsigned int eventId, void* clientData, void* callData){
        std::cout<<"PIck."<<std::endl;
        auto areaPicker = static_cast<vtkAreaPicker*>(caller);
        auto props = areaPicker->GetProp3Ds();
        props->InitTraversal();
        for(vtkIdType i=0;i<props->GetNumberOfItems();i++){
            vtkProp3D* prop = props->GetNextProp3D();
            std::cout<< "Picked prop: "<<prop<<std::endl;

        }
    }
}

