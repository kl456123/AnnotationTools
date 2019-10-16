#include <iostream>
#include <string>
#include <vector>
#include "Blob.h"
#include "DataLoader.h"

void DataLoader::Save(Blob* blob){
    auto num_samples = blob->GetNumOfSamples();
    auto num_features = blob->GetNumOfFeatures();
    double sample_data[num_features];
    if(this->fd){
        for(int i=0;i<num_samples;i++){
            blob->GetSample(i, sample_data);
            for(int j=0;j<num_features;j++){
                fprintf(fd, "%.2f", sample_data[j]);
                if(j<num_features-1){
                    fprintf(fd," ");
                }
            }
            if(i<num_samples-1){
                fprintf(fd, "\n");
            }
        }
    }
    std::cout<<"Saving "<<GetCurrentFileName()<<" To Disk."<<std::endl;
}

void DataLoader::InstallPipeline(){
    // pipeline for pointcloud
    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputConnection(TransformFilter->GetOutputPort());

    auto idFilter = vtkSmartPointer<vtkIdFilter>::New();
    idFilter->SetInputConnection(glyphFilter->GetOutputPort());
    idFilter->SetIdsArrayName("OriginalIds");

    auto surfaceFilter=vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());

    auto PointCloudMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    PointCloudMapper->SetInputConnection(surfaceFilter->GetOutputPort());
    PointCloudMapper->ScalarVisibilityOff();

    PointCloudActor = vtkSmartPointer<vtkActor>::New();
    PointCloudActor->SetMapper(PointCloudMapper);

    // pipeline for image
    auto ImageMapper = vtkSmartPointer<vtkImageMapper>::New();
    ImageMapper->SetColorWindow(255);
    ImageMapper->SetColorLevel(127.5);
    ImageMapper->RenderToRectangleOn();
    ImageActor->SetMapper(ImageMapper);
    ImageActor->GetMapper()->SetInputConnection(ImageReader->GetOutputPort());
    ImageActor->SetPosition2(1, 1);
}

void DataLoader::LoadPointCloud(std::string filename){
    int32_t num = 1e6;
    float *data = (float*)malloc(num*sizeof(float));


    float *px = data+0;
    float *py = data+1;
    float *pz = data+2;
    float *pr = data+3;


    auto currFileNameBinary = filename;
    FILE *stream;
    // string color_name = "Reflectance";

    std::cout<<currFileNameBinary<<std::endl;
    stream = fopen(currFileNameBinary.c_str(), "rb");

    num = fread(data,sizeof(float),num,stream)/4;
    if(ferror(stream)){
        return;
    }
    fclose(stream);
    ///////////////////////////////////
    // load data from velodyne bin file
    ///////////////////////////////////

    auto points = vtkSmartPointer<vtkPoints>::New();

    for(int i=0;i<num;i+=1){
        points->InsertNextPoint(*px, *py, *pz);
        px+=4;
        py+=4;
        pz+=4;
        pr+=4;
    }
    // free
    free(data);

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    TransformFilter->SetInputData(polyData);
}
