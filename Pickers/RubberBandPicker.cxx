#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <cmath>
#include <utility>

// vtk library

#include "AnnotationStyle.h"
#include "DataLoader.h"
#include "Display.h"


int main(){
    auto display  = vtkSmartPointer<Display>::New();
    std::string root_dir = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync";
    display->Initialize(root_dir);
    display->Start();

    return EXIT_SUCCESS;

}
