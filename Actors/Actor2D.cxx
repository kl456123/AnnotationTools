#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include "vtkRenderWindowInteractor.h"
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>


int main(){
    auto points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(10,10,0);
    points->InsertNextPoint(100,100, 0);
    points->InsertNextPoint(200,200, 0);

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);

    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputData(polyData);

    auto dataMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    dataMapper->SetInputConnection(glyphFilter->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(dataMapper);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0,0,0);

    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(300, 300);
    renderWindow->AddRenderer(renderer);

    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    actor->GetProperty()->SetColor(0,1,0);
    actor->GetProperty()->SetPointSize(3);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

}



