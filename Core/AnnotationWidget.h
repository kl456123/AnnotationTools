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
#include <vtkTransform.h>
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

        void GetPosition(double* position){
            double bounds[6];
            this->PointsActor->GetBounds(bounds);
            position[0] = (bounds[0] + bounds[1])/2;
            position[1] = (bounds[2] + bounds[3])/2;
            position[2] = (bounds[4] + bounds[5])/2;
        }

        void GetDims(double* dims){
            auto polyData = vtkSmartPointer<vtkPolyData>::New();
            this->AnnotationBoxWidget->GetPolyData(polyData);
            int num_points = polyData->GetNumberOfPoints();
            double coords[num_points][3];
            for(int i=0;i<num_points;i++){
                polyData->GetPoint(i, coords[i]);
            }
            float l = sqrt(vtkMath::Distance2BetweenPoints(coords[9], coords[8]));
            float h = sqrt(vtkMath::Distance2BetweenPoints(coords[11], coords[10]));
            float w = sqrt(vtkMath::Distance2BetweenPoints(coords[13], coords[12]));
            dims[0] = l;
            dims[1] = h;
            dims[2] = w;
        }
        void GetOrientation(double* ry){
            auto oldTrans = vtkSmartPointer<vtkTransform>::New();
            this->AnnotationBoxWidget->GetTransform(oldTrans);

            double wxyz[4];

            oldTrans->GetOrientationWXYZ(wxyz);
            ry[0] = wxyz[0];
        }

        void Get3DInfo(double* info){
            // kitti format
            // 3d: xyzwhl ry
            double* infoOffset=info;
            GetPosition(info);
            infoOffset+=3;
            GetDims(infoOffset);
            infoOffset+=3;
            GetOrientation(infoOffset);
            infoOffset+=1;
        }

        void Get2DInfo(double* info){
            // 2d: xyxy
            // double position[2];
            // double position2[2];
            auto rep = this->AnnotationBorderWidget->GetBorderRepresentation();
            auto position = rep->GetPosition();
            auto position2 = rep->GetPosition2();

            // normalized coords
            double tmp = position[1];
            position[1] = position2[1];
            position2[1] = tmp;
            position[1] = 1 - position[1];
            position2[1] = 1- position2[1];
        }

        void GetInfo(double* info){
            Get3DInfo(info);
            Get2DInfo(info+7);
        }

        int GetNumOfFeatures(){
            return 4+7;
        }

        void HorizontalRotateClockwise(double angle){
            // rotate box widget
            auto oldTrans = vtkSmartPointer<vtkTransform>::New();
            this->AnnotationBoxWidget->GetTransform(oldTrans);
            auto newTrans = vtkSmartPointer<vtkTransform>::New();
            double position[3];
            GetPosition(position);
            newTrans->PostMultiply();
            newTrans->Identity();
            newTrans->Translate(-position[0], -position[1], -position[2]);
            newTrans->RotateY(angle);
            newTrans->Translate(position[0], position[1], position[2]);
            oldTrans->Concatenate(newTrans);
            this->AnnotationBoxWidget->SetTransform(oldTrans);

            // rotate arrow
            this->ArrowActor->RotateY(angle);
        }

        void HorizontalRotateUnClockwise(double angle){
            this->HorizontalRotateClockwise(-angle);
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

        // border widget state
        bool BorderWidgetEnabled;
        bool BoxWidgetEnabled;

        enum {GREEN=0, RED};
};
#endif
