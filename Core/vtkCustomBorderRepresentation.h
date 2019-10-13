#ifndef _VTKCUSTOMBORDERREPRESENTATION_
#define _VTKCUSTOMBORDERREPRESENTATION_
#include <vtkBorderRepresentation.h>

class vtkCustomBorderRepresentation: public vtkBorderRepresentation{
    public:
        static vtkCustomBorderRepresentation* New();
        vtkTypeMacro(vtkCustomBorderRepresentation, vtkBorderRepresentation);

        int ComputeInteractionState(int X, int Y, int modify=0){
            vtkBorderRepresentation::ComputeInteractionState(X,Y, modify);
            if(this->InteractionState==vtkBorderRepresentation::Inside){
                this->InteractionState =vtkBorderRepresentation::Outside ;
                this->UpdateShowBorder();
            }
            return this->InteractionState;
        }


};

#endif
