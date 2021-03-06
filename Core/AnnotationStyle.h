#ifndef _ANNOTATIONSTYLE_
#define _ANNOTATIONSTYLE_
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

#include "AnnotationWidget.h"
#include "Blob.h"

#include "DataLoader.h"

class Display;



class AnnotationStyle: public vtkInteractorStyleRubberBandPick{
    public:
        vtkTypeMacro(AnnotationStyle, vtkInteractorStyleRubberBandPick);
        static AnnotationStyle* New(){
            auto *result = new AnnotationStyle;
            result->InitializeObjectBase();
            return result;
        }

        void InitAnnotationWidget(){
            // Test();
            // used for debuging
            this->InitAnnotationWidgetFromLabel();

            this->SelectedWidget = vtkSmartPointer<AnnotationWidget>::New();
            this->SelectedWidget->Initialize(this);
        }

        void SwitchFocalPoints(){
            auto renderer = this->PointCloudRenderer;
            auto camera = renderer->GetActiveCamera();
            SwitchFocalPoints(this->it, camera);
        }

        void SwitchFocalPoints(std::set<vtkSmartPointer<AnnotationWidget>>::iterator iterator, vtkCamera* camera){
            double position[3];
            auto widget = *iterator;
            widget->GetPosition(position);
            camera->SetFocalPoint(position);
            this->ResetVerticalView(camera);
        }

        void InitAnnotationWidgetFromLabel(){
            int numOfFeatures = this->SelectedWidget->GetNumOfFeatures();
            Blob blob(numOfFeatures);
            if(!this->AnnotationDataloader->LoadAnnotation(&blob)){
                // no annotation file
                return;
            }
            double info[numOfFeatures];
            int imageShape[2];
            AnnotationDataloader->GetImageShape(imageShape);
            // get points
            AnnotationDataloader->Update();
            for(int i=0;i<blob.GetNumOfSamples();i++){
                // init it
                this->SelectedWidget = vtkSmartPointer<AnnotationWidget>::New();
                this->SelectedWidget->Initialize(this);

                // set info
                blob.GetSample(i, info);

                this->SelectedWidget->SetInfo(info, imageShape);
                auto planes = vtkSmartPointer<vtkPlanes>::New();
                this->SelectedWidget->GetPlanes(planes);
                this->ExtractPoints(this->Points, planes);

                // add it
                this->AddBox();
                // disable this line, otherwise box will disappear when no points selected
                // this->SelectedWidget->PlaceAnnotationBoxWidget();
                this->SelectedWidget->PlaceArrowActor();
                this->SelectedWidget->SetEnabled(true);
                this->SelectedWidget->SetDeactiveColor();
                this->SelectedWidget->On();
            }
        }
        // void WriteDataToDisk();

        void SwitchFocus(std::set<vtkSmartPointer<AnnotationWidget>>::iterator iterator);

        void SwitchFocus();
        void ResetCurrentSelection();

        void ClearCurrentSelection(){
            this->RemoveBox();
            this->SelectedWidget->Clear();
        }

        void ClearCurrentFrame();

        void OffAllBoxes();

        void Save(){
            // if(this->AnnotationWidgets.size()==0){
                // return ;
            // }
            auto it = this->AnnotationWidgets.begin();
            auto widget = *it;
            int numOfFeatures = widget->GetNumOfFeatures();
            Blob blob(numOfFeatures);
            int imageShape[2];
            AnnotationDataloader->GetImageShape(imageShape);
            for(it = this->AnnotationWidgets.begin();it!=this->AnnotationWidgets.end();it++){
                // for each boxwidget in storage
                widget = *it;
                double info[numOfFeatures];
                widget->GetInfo(info, imageShape);
                blob.AddSample(info);
            }
            this->AnnotationDataloader->Save(&blob);
        }

        void ResetHorizontalView(vtkCamera* camera);
        void ResetVerticalView(vtkCamera* camera);

        void PlaceBoxWidget();

        void ComputeBorderWidgetPosition(double *position, double* position2);

        void PlaceBorderWidget();

        virtual void  OnRightButtonUp() override;

        void SetDisplay(vtkSmartPointer<Display> d){
            Displayer = d;
        }

        void AddBox();

        bool IncreaseIteratorRecurrent();

        void RemoveBox();

        void HandleCurrentSelection();

        void PrintCurrentVisibleBox(std::string callerStack);

        void ToggleDisplayAllBoxes();



        virtual void OnChar()override;
        void SetTextActor(vtkTextActor* textActor);

        void RemoveAllFromPointCloudRenderer(){
            // remove all other than itself
            this->PointCloudRenderer->RemoveAllViewProps();
            this->PointCloudRenderer->AddActor(this->AnnotationDataloader->GetPointCloudActor());
            this->PointCloudRenderer->AddActor(this->textActor);
        }

        void RemoveAllFromImageRenderer(){
            // remove all other than itself
            this->ImageRenderer->RemoveAllViewProps();
            this->ImageRenderer->AddActor(this->AnnotationDataloader->GetImageActor());
        }

        void ToggleText();
        void ExtractPoints(vtkPolyData* polyData, vtkImplicitFunction* selectedRegion);

        void PickActor();

        bool IsSelectPoint();

        void UpdateRender();

        virtual void OnLeftButtonUp() override;

        void SetPoints(vtkSmartPointer<vtkPolyData> points){
            this->Points = points;
        }
        void SetImageRenderer(vtkRenderer* renderer){
            this->ImageRenderer = renderer;
        }
        void SetPointCloudRenderer(vtkRenderer* renderer){
            this->PointCloudRenderer = renderer;
        }

        vtkRenderer* GetPointCloudRenderer(){
            return PointCloudRenderer;
        }

        vtkRenderer* GetImageRenderer(){
            return ImageRenderer;
        }

        void SetDataLoader(DataLoader* dataloader){
            AnnotationDataloader = dataloader;
        }
    private:
        vtkSmartPointer<vtkPolyData> Points;
        std::set<vtkSmartPointer<AnnotationWidget>> AnnotationWidgets;
        vtkSmartPointer<AnnotationWidget> SelectedWidget;

        vtkSmartPointer<vtkTextActor> textActor;

        std::set<vtkSmartPointer<AnnotationWidget>>::iterator it;
        vtkSmartPointer<vtkRenderer> ImageRenderer;
        vtkSmartPointer<vtkRenderer> PointCloudRenderer;
        vtkSmartPointer<DataLoader> AnnotationDataloader;

        double AngleAdjustPrecision=5;

        vtkSmartPointer<Display> Displayer;
};



#endif
