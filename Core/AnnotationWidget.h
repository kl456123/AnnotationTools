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
#include <vtkPlanes.h>
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
            if(IsValidBox()){
                this->PointCloudRenderer->AddActor(this->PointsActor);
            }
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
            auto polyData = vtkSmartPointer<vtkPolyData>::New();
            this->AnnotationBoxWidget->GetPolyData(polyData);
            int num_points = polyData->GetNumberOfPoints();
            double coords[num_points][3];
            for(int i=0;i<num_points;i++){
                polyData->GetPoint(i, coords[i]);
            }
            position[0] = coords[14][0];
            position[1] = coords[14][1];
            position[2] = coords[14][2];
        }

        void GetDimsAndPosition(double* info){
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

            info[0] = h;
            info[1] = w;
            info[2] = l;
            info[3] = coords[14][0];
            info[4] = coords[14][1] + 0.5*h;
            info[5] = coords[14][2];
        }
        void GetOrientation(double* ry){
            double ori[3];
            this->ArrowActor->GetOrientation(ori);
            if(ori[1]>180){
                ori[1] = ori[1]-360;
            }else if(ori[1]<-180){
                ori[1] +=360;
            }
            // -pi, pi
            ry[0] = ori[1] / 180*3.14;
        }

        void SetInfo(double* info, int imageShape[2]){
            this->Set3DInfo(info);
            this->Set2DInfo(info+7, imageShape);
        }

        void Set2DInfo(double* info, int imageShape[2]){
            auto rep = vtkSmartPointer<vtkCustomBorderRepresentation>::New();
            this->AnnotationBorderWidget->SetRepresentation(rep);
            double position[2];
            double position2[2];


            position2[0]+=position[0];
            position2[1]+=position[1];

            // copy
            position[0] = info[0]/imageShape[1];
            position[1]= 1-info[1]/imageShape[0];
            position2[0] = info[2]/imageShape[1];
            position2[1] = 1-info[3]/imageShape[0];

            double tmp = position[1];
            position[1] = position2[1];
            position2[1] = tmp;
            position2[0] = position2[0] - position[0];
            position2[1] = position2[1] - position[1];
            rep->SetPosition(position);
            rep->SetPosition2(position2);
        }

        void Set3DInfo(double* info){
            // if(info[6]<0){
                // info[6]+=2*3.14;
            // }
            double bounds[6] = {-0.5, 0.5, -0.5, 0.5, -0.5, 0.5};
            this->AnnotationBoxWidget->PlaceWidget(bounds);
            auto trans = vtkSmartPointer<vtkTransform>::New();
            trans->PostMultiply();
            trans->Identity();
            trans->Scale(info[3], info[4], info[5]);
            trans->RotateY(info[6]*180/3.14);
            trans->Translate(info[0], info[1]-0.5*info[4], info[2]);
            this->AnnotationBoxWidget->SetTransform(trans);

            // arrow actor
            this->ArrowActor->RotateY(info[6]*180/3.14);
        }

        void SetVisible(double* info){
            info[0] = 0;
            info[1] = -1;
            info[2] = -1;
            info[3] = -10;
        }

        void Get3DInfo(double* info){
            // kitti format
            // 3d: xyzwhl ry
            double* infoOffset=info;
            // GetPosition(infoOffset);

            GetDimsAndPosition(infoOffset);
            infoOffset+=6;
            GetOrientation(infoOffset);

            // postprocess
            // info[1] +=0.5*info[4];
        }

        void Get2DInfo(double* info, int imageShape[2]){
            // 2d: xyxy
            // double position[2];
            // double position2[2];
            auto rep = this->AnnotationBorderWidget->GetBorderRepresentation();
            if(rep){
                auto position = rep->GetPosition();
                auto position2 = rep->GetPosition2();

                position2[0]+=position[0];
                position2[1]+=position[1];

                // normalized coords
                double tmp = position[1];
                position[1] = position2[1];
                position2[1] = tmp;
                position[1] = 1 - position[1];
                position2[1] = 1- position2[1];

                // copy
                info[0] = position[0] * imageShape[1];
                info[1] = position[1] * imageShape[0];
                info[2] = position2[0] * imageShape[1];
                info[3] = position2[1] * imageShape[0];
            }else{
                info[0] = 0 ;
                info[1] = 0;
                info[2] = 0;
                info[3] = 0;
            }

        }

        void GetInfo(double* info, int imageShape[2]){
            SetVisible(info);
            Get2DInfo(info+4, imageShape);
            Get3DInfo(info+8);
        }

        int GetNumOfFeatures(){
            return 4+7 + 4;
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

        void HorizontalRotate90Arrow(){
            this->ArrowActor->RotateY(90);
        }

        void HorizontalRotateUnClockwise(double angle){
            this->HorizontalRotateClockwise(-angle);
        }


        void Clear();

        void GetPlanes(vtkPlanes* planes){
            return this->AnnotationBoxWidget->GetPlanes(planes);
        }

        void PlaceAnnotationBorderWidget(double* position, double* position2);
        void PlaceArrowActor();
        void PlaceAnnotationBoxWidget();
        bool IsValidBox();
        void Off();
        void On();
        bool GetEnabled();
        void SetEnabled(bool enabled){
            this->BoxWidgetEnabled = enabled;
            this->BorderWidgetEnabled = enabled;
        }

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
