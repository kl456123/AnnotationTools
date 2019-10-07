#include <vtkActor.h>



namespace{
    class MouseInteractorHighLightActor: public vtkInteractorStyleTrackballCamera{
        public:
            static MouseInteractorHighLightActor* New();
            vtkTypeMacro(MouseInteractorHighLightActor,
                    vtkInteractorStyleTrackballCamera);
            MouseInteractorHighLightActor(){
                LastPickedActor = NULL;
                LastPickedProperty = vtkProperty::New();
            }

            virtual ~MouseInteractorHighLightActor(){
                LastPickedProperty->Delete();
            }

            virtual void OnLeftButtonDown()override{
                auto colors = vtkSmartPointer<vtkNamedColors>::New();
                int* clickPos = this->GetInteractor()->GetEventPosition();

                auto picker = vtkSmartPointer<vtkPropPicker>::New();

                picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

                if(this->LastPickedActor){
                    // recover the last prop
                    this->LastPickedActor->GetProperty()->DeepCopy(this->LastPickedProperty());
                }

                this->LastPickedActor = picker->GetActor();
                if(this->LastPickedActor){
                    this->LastPickedProperty->DeepCopy(this->LastPickedActor->GetProperty());
                    // highlight
                    this->LastPickedActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
                    this->LastPickedActor->GetProperty()->SetDiffuse(1.0);
                    this->LastPickedActor->GetProperty()->SetSpecular(0.0);
                    this->LastPickedActor->GetProperty()->EdgeVisibilityOn();
                }
                // forward
                vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
            }
        private:
            vtkActor* LastPickedActor;
            vtkProperty* LastPickedProperty;
    };

}
