#include <vtkSmartPointer.h>
#include <vtkActor.h>


void Get(vtkActor* pt){

    // return p;
}

int main(){
    auto pt = vtkActor::New();
    {
        auto p = vtkSmartPointer<vtkActor>(pt);
    }

    std::cout<<"Addr: "<<pt<<std::endl;
    return 0;

}
