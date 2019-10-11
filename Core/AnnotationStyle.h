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



class AnnotationStyle: public vtkInteractorStyleRubberBandPick{
    public:
        vtkTypeMacro(AnnotationStyle, vtkInteractorStyleRubberBandPick);
        AnnotationStyle();
        static AnnotationStyle* New(){
            auto *result = new AnnotationStyle;
            result->InitializeObjectBase();
            return result;
        }

        void InitAnnotationWidget(){
            this->SelectedWidget = vtkSmartPointer<AnnotationWidget>::New();
            this->SelectedWidget->Initialize(this);
        }
        void WriteDataToDisK();
        void SetFile(FILE* fd);

        void SwitchFocus(std::set<vtkSmartPointer<AnnotationWidget>>::iterator iterator);

        void SwitchFocus();
        void ResetCurrentSelection();

        void ClearCurrentSelection(){
            this->SelectedWidget->Clear();
        }

        void ResetHorizontalView(vtkCamera* camera);
        void ResetVerticalView(vtkCamera* camera);

        void PlaceBoxWidget();

        void ComputeBorderWidgetPosition(double *position, double* position2);

        void PlaceBorderWidget();

        virtual void  OnRightButtonUp() override;

        void AddBox();

        bool IncreaseIteratorRecurrent();

        void RemoveBox();

        void HandleCurrentSelection();

        void PrintCurrentVisibleBox(std::string callerStack);

        void ToggleDisplayAllBoxes();



        virtual void OnChar()override;
        void SetTextActor(vtkTextActor* textActor);

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
    private:
        vtkSmartPointer<vtkPolyData> Points;
        std::set<vtkSmartPointer<AnnotationWidget>> AnnotationWidgets;
        vtkSmartPointer<AnnotationWidget> SelectedWidget;

        vtkSmartPointer<vtkTextActor> textActor;

        std::set<vtkSmartPointer<AnnotationWidget>>::iterator it;
        vtkSmartPointer<vtkRenderer> ImageRenderer;
        vtkSmartPointer<vtkRenderer> PointCloudRenderer;

        FILE* fd;
};



#endif
