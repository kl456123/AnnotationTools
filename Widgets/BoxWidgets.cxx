#include <vtkSmartPointer.h>
// For the rendering pipeline setup:
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
// // For vtkBoxWidget:
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
class vtkMyCallback:public vtkCommand{
    public:
        static vtkMyCallback *New(){
            return new vtkMyCallback;
        }

        virtual void Execute(vtkObject * caller, unsigned long, void*)override{
            auto  t = vtkSmartPointer<vtkTransform>::New();
            vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
            widget->GetTransform(t);
            // current actor
            widget->GetProp3D()->SetUserTransform(t);
        }

};


int main(){
    auto cone = vtkSmartPointer<vtkConeSource>::New();
    auto coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    coneMapper->SetInputConnection(cone->GetOutputPort());
    auto coneActor = vtkSmartPointer<vtkActor>::New();
    coneActor->SetMapper(coneMapper);
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(coneActor);
    renderer->SetBackground(0.1, 0.2, 0.4);

    auto window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);
    window->SetSize(300, 300);

    auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(window);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactor->SetInteractorStyle( style );
    auto boxWidget = vtkSmartPointer<vtkBoxWidget>::New();
    auto callback = vtkSmartPointer<vtkMyCallback>::New();
    boxWidget->SetInteractor(interactor);

    boxWidget->SetProp3D(coneActor);
    boxWidget->SetPlaceFactor(1.25);
    boxWidget->PlaceWidget();
    boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    // boxWidget->On();
    window->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
