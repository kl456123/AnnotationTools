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
    std::string root_dir = "/home/breakpoint/Data/KITTI/object/training";
    display->Initialize(root_dir);
    display->Start();

    return EXIT_SUCCESS;

}
