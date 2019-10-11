#ifndef _ANNOTATIONWIDGET_
#define _ANNOTATIONWIDGET_
#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkBoxWidget.h>
#include <vtkBorderWidget.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkArrowSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include "vtkCustomBorderRepresentation.h"

class AnnotationStyle;


class AnnotationWidget: public vtkObjectBase{
    public:
        static AnnotationWidget* New(){
            auto *result = new AnnotationWidget;
            result->InitializeObjectBase();
            return result;
        }

        AnnotationWidget();
        void Initialize(AnnotationStyle* style);

        void SetInteractorStyle(vtkInteractorStyle* style);

        void SetColor(int colorType){
            if(colorType==GREEN){
                PointsActor->GetProperty()->SetColor(Colors->GetColor3d("Green").GetData());
            }else if(colorType==RED){
                PointsActor->GetProperty()->SetColor(Colors->GetColor3d("Red").GetData());
            }else{
                // handle error here
            }
        }

        void SetActiveColor(){
            this->SetColor(RED);
        }
        void SetDeactiveColor(){
            this->SetColor(GREEN);
        }

        vtkPolyData* GetPolyData(){
            return reinterpret_cast<vtkPolyData*>(this->PointsActor->GetMapper()->GetInput());
        }

        void SetPolyData(vtkPolyData* polyData){
            reinterpret_cast<vtkDataSetMapper*>(this->PointsActor->GetMapper())->SetInputData(polyData);
        }

        void AddToRenderer(){
            this->PointCloudRenderer->AddActor(this->PointsActor);
            this->PointCloudRenderer->AddActor(this->ArrowActor);
        }

        vtkActor* GetActor(){
            return this->PointsActor;
        }

        void RemoveFromRenderer(){
            PointCloudRenderer->RemoveActor(this->PointsActor);
            PointCloudRenderer->RemoveActor(this->ArrowActor);
        }


        void Clear();

        void PlaceAnnotationBorderWidget(double* position, double* position2);
        void PlaceArrowActor();
        void PlaceAnnotationBoxWidget();
        bool IsValidBox();
        void Off();
        void On();
        bool GetEnabled();

    private:
        // points color
        vtkSmartPointer<vtkNamedColors> Colors;
        // actor of inner points
        vtkSmartPointer<vtkActor> PointsActor;
        // box_3d
        vtkSmartPointer<vtkBoxWidget> AnnotationBoxWidget;
        // box_2d
        vtkSmartPointer<vtkBorderWidget> AnnotationBorderWidget;
        // arrow actor(orientation)
        vtkSmartPointer<vtkActor> ArrowActor;

        // renderer
        vtkSmartPointer<vtkRenderer> PointCloudRenderer;
        vtkSmartPointer<vtkRenderer> ImageRenderer;

        // mapper
        vtkSmartPointer<vtkPolyDataMapper> ArrowMapper;
        vtkSmartPointer<vtkDataSetMapper>PointsMapper;

        // interactor
        vtkSmartPointer<vtkRenderWindowInteractor> Interactor;

        enum {GREEN=0, RED};
};
#endif
