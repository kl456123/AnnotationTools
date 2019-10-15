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
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>

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

        string GetCurrentFileName(){
            return this->filenames[this->FileIndex];
        }
        string GetImageDir(){
            return this->root_dir+"/image_2";
        }
        string GetPointCloudDir(){
            return this->root_dir+"/velodyne";
        }

        string GetCalibDir(){
            return this->root_dir + "/calib";
        }
        string GetAnnotationDir(){
            return this->root_dir + "/anno_2";
        }

        string GetLabelDir(){
            return this->root_dir + "/label_2";
        }
        string GetLabelPath(int fileIndex){
            return GetLabelDir() + "/" + filenames[fileIndex] + ".txt";
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

        void LoadLabel(Blob* blob){
            // used for debuging in kitti dataset
            FILE* f = fopen(GetLabelPath(FileIndex).c_str(), "r");
            double info[14];
            auto numOfFeatures = blob->GetNumOfFeatures();
            double used_info[numOfFeatures];
            char s[100];
            while(fscanf(f, "%s", s)!=EOF){
                if(s[0]=='D' and s[1]=='o' and s[2]=='n'){
                    continue;
                }
                for(int i=0;i<14;i++){
                    fscanf(f, "%lf", info+i);
                }
                // location
                used_info[0] = info[10];
                used_info[1] = info[11];
                used_info[2] = info[12];
                // dims
                used_info[3] = info[9];
                used_info[4] = info[7];
                used_info[5] = info[8];

                // ry
                used_info[6] = info[13];
                blob->AddSample(used_info);
            }
            fclose(f);
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
            // get image shape
            ifstream in(fn);
            in.seekg(16);
            unsigned int width, height;
            in.read((char*)&width, 4);
            in.read((char*)&height, 4);
            ImageShape[1] = ntohl(width);
            ImageShape[0] = ntohl(height);
            std::cout<<"ImageShape: "<<ImageShape[0]<<" "<<ImageShape[1]<<std::endl;
        }



        void LoadPointCloud(string fn);

        void LoadAnnotation(string fn){
            fd = fopen(fn.c_str(), "w");
            if(!fd){
                std::cout<<"Error when opening annotation file."<<fn<<std::endl;
            }else{
                // create directory
                // system;
            }
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

        void GetImageShape(int imageShape[2]){
            imageShape[0] = ImageShape[0];
            imageShape[1] = ImageShape[1];
        }

        void ReadFilesNamesFromDir(string root_dir){
            auto dir = opendir(root_dir.c_str());
            if(dir==NULL){
                std::cout<<"Error when Opening directory "<<root_dir<<std::endl;
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
            // ifstream file;
            // file.open(calib_fn, std::ios::in);
            // double VeloToCam[16]={0};
            // VeloToCam[15] = 1;
            // double R0_rect[16] = {0};
            // R0_rect[15] = 1;
            // if(!file.is_open()){
                // return ;
            // }
            // std::string line;
            // while(getline(file, line)){
                // if(line.empty()){
                    // continue;
                // }
                // if(line.substr(0, 7)=="R0_rect"){
                    // istreamstring s(line);
                    // for(int i=0;i<)
                // }
            // }

            FILE* f = fopen(calib_fn.c_str(), "r");
            char s[100];
            double VeloToCam[16]={0};
            VeloToCam[15] = 1;
            double R0_rect[16] = {0};
            R0_rect[15] = 1;

            while(fscanf(f, "%s", s)!=EOF){
                if(s[0]=='R'){
                    for(int i=0;i<9;i++){
                        fscanf(f, "%lf", R0_rect+i/3*4+i%3);
                    }
                }
                if(s[3]=='v'){
                    for(int i=0;i<12;i++){
                        fscanf(f, "%lf", VeloToCam+i);
                    }
                }
            }
            fclose(f);



            // 9.999239000000e-01 9.837760000000e-03 -7.445048000000e-03 -9.869795000000e-03 9.999421000000e-01 -4.278459000000e-03 7.402527000000e-03 4.351614000000e-03 9.999631000000e-01
            // double VeloToCam[16] = {7.533745000000e-03, -9.999714000000e-01, -6.166020000000e-04, -4.069766000000e-03,
                // 1.480249000000e-02, 7.280733000000e-04, -9.998902000000e-01, -7.631618000000e-02,
                // 9.998621000000e-01, 7.523790000000e-03, 1.480755000000e-02, -2.717806000000e-01,
                // 0,0,0,1};
            // double R0_rect[16] = {9.999128000000e-01, 1.009263000000e-02, -8.511932000000e-03,0,
                // -1.012729000000e-02, 9.999406000000e-01, -4.037671000000e-03,0,
                // 8.470675000000e-03, 4.123522000000e-03, 9.999556000000e-01, 0,
                // 0,0,0,1};
            // double VeloToCam[16] = {6.927964000000e-03, -9.999722000000e-01, -2.757829000000e-03, -2.457729000000e-02,
            // -1.162982000000e-03, 2.749836000000e-03, -9.999955000000e-01, -6.127237000000e-02,
            // 9.999753000000e-01, 6.931141000000e-03, -1.143899000000e-03, -3.321029000000e-01,
            // 0.0,0.0,0.0,1.0};
            auto transform = vtkSmartPointer<vtkTransform>::New();
            transform->Identity();
            transform->PostMultiply();
            transform->Concatenate(R0_rect);
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
        int ImageShape[3];

};
#endif
