#ifndef _DATALOADER_
#define _DATALOADER_
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <vtkPNGReader.h>
#include <vtkTransform.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkIdFilter.h>
#include <vtkActor2D.h>
#include <vtkImageMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkActor2D.h>

class Blob;

using namespace std;
class DataLoader: public vtkObjectBase{
    public:
        static DataLoader* New(){
            auto *result = new DataLoader;
            result->InitializeObjectBase();
            return result;
        }
        void Initialize(string root_dir){
            this->root_dir =root_dir;
            this->ReadFilesNamesFromDir(GetImageDir());

            this->TransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            this->ImageActor = vtkSmartPointer<vtkActor2D>::New();
            this->PointCloudActor = vtkSmartPointer<vtkActor>::New();

            // this->PolyData = vtkSmartPointer<vtkPolyData>::New();
            this->ImageReader = vtkSmartPointer<vtkPNGReader>::New();

            // some variables
            this->FileIndex = 0;

            // setup pipeline
            this->InstallPipeline();

            this->Load(FileIndex);
        }
        string GetImageDir(){
            return this->root_dir+"/image_02/data";
        }
        string GetPointCloudDir(){
            return this->root_dir+"/velodyne_points/data";
        }

        string GetCalibDir(){
            return this->root_dir + "/calib";
        }
        string GetAnnotationDir(){
            return this->root_dir + "/label_2";
        }

        string GetImagePath(int fileIndex){
            return GetImageDir()+"/" + filenames[fileIndex]+".png";
        }
        string GetPointCloudPath(int fileIndex){
            return GetPointCloudDir()+"/"+filenames[fileIndex]+".bin";
        }

        string GetCalibPath(int fileIndex){
            return GetCalibDir()+ "/"+filenames[fileIndex] + ".txt";
        }
        string GetAnnotationPath(int fileIndex){
            return GetAnnotationDir()+"/"+filenames[fileIndex]+".txt";
        }



        void ReadFileNamesFromTXT(string txt){
            FILE* txt_fd = fopen(txt.c_str(), "r");
            fclose(txt_fd);
        }

        bool CheckFileIndex(int fileIndex){
            return fileIndex<GetNumOfFiles() and fileIndex>=0;
        }

        int GetNumOfFiles(){
            return filenames.size();
        }

        void LoadNext(){
            FileIndex +=1;
            Load(FileIndex);
        }

        void Update(){
        }

        void LoadImage(string fn){
            ImageReader->SetFileName(fn.c_str());
        }



        void LoadPointCloud(string fn);

        void LoadAnnotation(string fn){
            this->fd = fopen(fn.c_str(), "w");
        }
        void Load(int fileIndex){

            if(!CheckFileIndex(FileIndex)){return;}
            // load calib first due to that velodyne needs it
            LoadCalibration(GetCalibPath(fileIndex));
            // image
            LoadImage(GetImagePath(fileIndex));
            //velodyne
            LoadPointCloud(GetPointCloudPath(fileIndex));

            // load annotation if any
            LoadAnnotation(GetAnnotationPath(fileIndex));

        }


        void Save(Blob* blob);

        void LoadPrev(){
            FileIndex-=1;
            Load(FileIndex);
        }

        void ReadFilesNamesFromDir(string root_dir){
            auto dir = opendir(root_dir.c_str());
            if(dir==NULL){
                std::cout<<"Error when Opening directory"<<std::endl;
                return;
            }else{
                std::cout<<"Opening "<<root_dir<<std::endl;
            }
            struct dirent *ptr;
            while((ptr=readdir(dir))!=NULL)
            {
                // ignore '.' and '..'
                if(ptr->d_name[0] == '.')
                    continue;
                auto name_str = string(ptr->d_name);
                int pos = name_str.find(".");
                auto sample_str = name_str.substr(0, pos);
                filenames.push_back(sample_str);
            }

        }

        void InstallPipeline();

        void LoadCalibration(string calib_fn){
            double R0_rect[16] = {9.999128000000e-01, 1.009263000000e-02, -8.511932000000e-03,0,
                -1.012729000000e-02, 9.999406000000e-01, -4.037671000000e-03,0,
                8.470675000000e-03, 4.123522000000e-03, 9.999556000000e-01, 0,
                0,0,0,1};
            double VeloToCam[16] = {6.927964000000e-03, -9.999722000000e-01, -2.757829000000e-03, -2.457729000000e-02,
                -1.162982000000e-03, 2.749836000000e-03, -9.999955000000e-01, -6.127237000000e-02,
                9.999753000000e-01, 6.931141000000e-03, -1.143899000000e-03, -3.321029000000e-01,
                0.0,0.0,0.0,1.0};
            auto transform = vtkSmartPointer<vtkTransform>::New();
            transform->SetMatrix(R0_rect);
            transform->Concatenate(VeloToCam);
            // TransformFilter->Print(std::cout);
            TransformFilter->SetTransform(transform);
        }

        ~DataLoader(){
            if(fd){
                fclose(fd);
            }
        }

        vtkActor2D* GetImageActor(){
            return ImageActor;
        }
        vtkActor* GetPointCloudActor(){
            return PointCloudActor;
        }

        vtkPolyData* GetPoints(){
            return TransformFilter->GetOutput();
        }

    private:
        string root_dir;
        vector<string> filenames;
        vtkSmartPointer<vtkTransformPolyDataFilter> TransformFilter;
        FILE* fd;
        int FileIndex;
        vtkSmartPointer<vtkPNGReader> ImageReader;
        // vtkSmartPointer<vtkPolyData> PolyData;
        vtkSmartPointer<vtkActor2D> ImageActor;
        vtkSmartPointer<vtkActor> PointCloudActor;

};
#endif
