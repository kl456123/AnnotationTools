#include <vtkSmartPointer.h>
#include <vtkBoxWidget.h>
#include <vtkBorderWidget.h>
#include <vtkBorderRepresentation.h>
#include <iostream>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>


vtkBorderWidget* Get(){
    vtkBorderWidget* box_widget = vtkSmartPointer<vtkBorderWidget>::New();
    return box_widget;
}

int main(){
    // auto box_widget = Get();
    // box_widget->GetBorderRepresentation()->Print(std::cout);
    double R0_rect[16] = {9.999128000000e-01, 1.009263000000e-02, -8.511932000000e-03,0,
        -1.012729000000e-02, 9.999406000000e-01, -4.037671000000e-03,0,
        8.470675000000e-03, 4.123522000000e-03, 9.999556000000e-01, 0,
        0,0,0,1};
    double VeloToCam[16] = {6.927964000000e-03, -9.999722000000e-01, -2.757829000000e-03, -2.457729000000e-02,
        -1.162982000000e-03, 2.749836000000e-03, -9.999955000000e-01, -6.127237000000e-02,
        9.999753000000e-01, 6.931141000000e-03, -1.143899000000e-03, -3.321029000000e-01,
        0.0,0.0,0.0,1.0};
    auto transform = vtkSmartPointer<vtkTransform>::New();
    auto TransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transform->SetMatrix(R0_rect);
    transform->Concatenate(VeloToCam);
    TransformFilter->Print(std::cout);
    TransformFilter->SetTransform(transform);
    return 0;
}
