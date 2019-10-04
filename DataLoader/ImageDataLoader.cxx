#include <iostream>
#include <string>

#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
// #include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[]){

    if(argc!=2){
        std::cout<< "Usage: "<<argv[0]<<" Filename(.jpeg/jpg) e.g. Pileated.jpg" <<std::endl;
        return EXIT_FAILURE;
    }
    // auto jpegReader = vtkSmartPointer<vtkJPEGReader>::New();
    auto pngReader = vtkSmartPointer<vtkPNGReader>::New();
    std::string filename = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync/image_00/data/0000000000.png";
    // jpegReader->SetFileName(filename.c_str());
    pngReader->SetFileName(argv[1]);

    auto imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    imageViewer->SetInputConnection(pngReader->GetOutputPort());
    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    imageViewer->SetupInteractor(renderWindowInteractor);
    imageViewer->Render();
    imageViewer->GetRenderer()->ResetCamera();
    imageViewer->Render();

    renderWindowInteractor->Start();
    return EXIT_SUCCESS;
}
