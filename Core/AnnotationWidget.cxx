#include "AnnotationWidget.h"
#include "AnnotationStyle.h"

AnnotationWidget::AnnotationWidget(){
    // set state first
    BorderWidgetEnabled = false;
    BoxWidgetEnabled = false;

    // points actor
    PointsMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    // important !
    PointsMapper->ScalarVisibilityOff();
    PointsActor = vtkSmartPointer<vtkActor>::New();
    PointsActor->SetMapper(PointsMapper);

    // arrow actor
    auto arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    ArrowMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    ArrowActor = vtkSmartPointer<vtkActor>::New();
    ArrowActor->SetScale(3);
    // ArrowActor->RotateY(90);
    ArrowMapper->SetInputConnection(arrowSource->GetOutputPort());
    ArrowActor->SetMapper(ArrowMapper);

    // box_3d
    AnnotationBoxWidget = vtkSmartPointer<vtkBoxWidget>::New();
    AnnotationBoxWidget->KeyPressActivationOff();
    AnnotationBoxWidget->SetPlaceFactor(1);
    // AnnotationBoxWidget->SetEnabled(false);
    AnnotationBoxWidget->SetRotationEnabled(false);
    // AnnotationBoxWidget->SetTranslationEnabled(false);
    // AnnotationBoxWidget->SetScalingEnabled(false);

    // box_2d
    AnnotationBorderWidget = vtkSmartPointer<vtkBorderWidget>::New();
    AnnotationBorderWidget->KeyPressActivationOff();

    // set color
    Colors =  vtkSmartPointer<vtkNamedColors>::New();
}

void AnnotationWidget::Initialize(AnnotationStyle* style){
    Interactor = style->GetInteractor();
    PointCloudRenderer = style->GetPointCloudRenderer();
    ImageRenderer = style->GetImageRenderer();

    // SetInteractor
    AnnotationBoxWidget->SetInteractor(Interactor);
    AnnotationBorderWidget->SetInteractor(Interactor);

    // set default renderer
    AnnotationBoxWidget->SetDefaultRenderer(PointCloudRenderer);
    AnnotationBorderWidget->SetDefaultRenderer(ImageRenderer);
}


void AnnotationWidget::Off(){
    // box 3d
    // if(BoxWidgetEnabled){
    this->AnnotationBoxWidget->Off();
    // }

    // box 2d
    // if(BorderWidgetEnabled){
    this->AnnotationBorderWidget->Off();
    // }
    // arrow
    this->PointCloudRenderer->RemoveActor(this->ArrowActor);
}

void AnnotationWidget::On(){
    if(BoxWidgetEnabled){
        this->AnnotationBoxWidget->On();
        // arrow
        this->PointCloudRenderer->AddActor(this->ArrowActor);
    }

    if(BorderWidgetEnabled){
        this->AnnotationBorderWidget->On();
    }
}

void AnnotationWidget::Clear(){
    auto polyData = this->PointsActor->GetMapper()->GetInput();
    if(polyData){
        polyData->Initialize();
        this->PointCloudRenderer->RemoveActor(PointsActor);
    }

    polyData = this->ArrowActor->GetMapper()->GetInput();
    if(polyData){
        // no used for arrow actor
        polyData->Initialize();
        this->PointCloudRenderer->RemoveActor(this->ArrowActor);
    }
    //set off to widgets
    BoxWidgetEnabled = false;
    BorderWidgetEnabled = false;
    this->Off();
}

bool AnnotationWidget::GetEnabled(){
    return this->AnnotationBoxWidget->GetEnabled() or this->AnnotationBorderWidget->GetEnabled();
}

bool AnnotationWidget::IsValidBox(){
    auto polyData = reinterpret_cast<vtkPolyData*>(this->PointsActor->GetMapper()->GetInput());
    return polyData and polyData->GetNumberOfPoints()>0;
}

void AnnotationWidget::PlaceAnnotationBoxWidget(){
    // Box

    BoxWidgetEnabled = true;
    this->AnnotationBoxWidget->SetProp3D(this->PointsActor);
    this->AnnotationBoxWidget->PlaceWidget();
    this->AnnotationBoxWidget->Off();


}

void AnnotationWidget::PlaceArrowActor(){
    // set position
    double position[3];
    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    this->AnnotationBoxWidget->GetPolyData(polyData);
    polyData->GetPoint(polyData->GetNumberOfPoints()-1, position);
    ArrowActor->SetPosition(position);
}

void AnnotationWidget::PlaceAnnotationBorderWidget(double* position, double* position2){
    BorderWidgetEnabled = true;
    this->AnnotationBorderWidget->CreateDefaultRepresentation();
    auto rep = vtkSmartPointer<vtkCustomBorderRepresentation>::New();
    this->AnnotationBorderWidget->SetRepresentation(rep);

    // set position
    rep->SetPosition(position);
    rep->SetPosition2(position2);
    this->AnnotationBorderWidget->SelectableOff();
    this->AnnotationBorderWidget->KeyPressActivationOff();
    this->AnnotationBorderWidget->On();
}

