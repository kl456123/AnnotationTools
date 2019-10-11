#include "vtkCustomBorderRepresentation.h"
// vtkStandardNewMacro(vtkCustomBorderRepresentati);
//
vtkCustomBorderRepresentation* vtkCustomBorderRepresentation::New(){
    auto *result = new vtkCustomBorderRepresentation;
    result->InitializeObjectBase();
    return result;

}
