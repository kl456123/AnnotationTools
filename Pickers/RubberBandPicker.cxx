#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <cmath>

// vtk library
#include <vtkPlane.h>
#include <vtkPNGReader.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkNamedColors.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImplicitBoolean.h>
#include <vtkRendererCollection.h>
#include <vtkCamera.h>
#include <vtkImplicitFunctionCollection.h>
#include <vtkMath.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapper.h>

// boxwidget
#include <vtkBorderWidget.h>
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>

#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPropPicker.h>


#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

vtkSmartPointer<vtkVertexGlyphFilter> ReadPointCloudFromBin(std::string& filename){
    int32_t num = 1e6;
    float *data = (float*)malloc(num*sizeof(float));


    float *px = data+0;
    float *py = data+1;
    float *pz = data+2;
    float *pr = data+3;


    auto currFileNameBinary = filename;
    FILE *stream;
    double low_z = 0;
    double high_z = 0;
    // string color_name = "Reflectance";

    std::cout<<currFileNameBinary<<std::endl;
    stream = fopen(currFileNameBinary.c_str(), "rb");

    num = fread(data,sizeof(float),num,stream)/4;
    if(ferror(stream)){
        return NULL;
    }
    fclose(stream);
    ///////////////////////////////////
    // load data from velodyne bin file
    ///////////////////////////////////

    auto points = vtkSmartPointer<vtkPoints>::New();
    // auto colors = vtkSmartPointer<vtkDoubleArray>::New();
    // colors->SetName(color_name.c_str());

    for(int i=0;i<num;i+=1){
        //insert point into Points
        points->InsertNextPoint(*px, *py, *pz);
        // 0-1
        // colors->InsertNextValue(*pr);
        px+=4;
        py+=4;
        pz+=4;
        pr+=4;
    }

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    // polyData->GetPointData()->SetScalars(colors);
    // polyData->GetPointData()->SetActiveScalars(color_name.c_str());
    //
    // transform polydata
    auto transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    auto transform = vtkSmartPointer<vtkTransform>::New();
    double R0_rect[16] = {9.999128000000e-01, 1.009263000000e-02, -8.511932000000e-03,0,
        -1.012729000000e-02, 9.999406000000e-01, -4.037671000000e-03,0,
        8.470675000000e-03, 4.123522000000e-03, 9.999556000000e-01, 0,
        0,0,0,1};
    double VeloToCam[16] = {6.927964000000e-03, -9.999722000000e-01, -2.757829000000e-03, -2.457729000000e-02,
        -1.162982000000e-03, 2.749836000000e-03, -9.999955000000e-01, -6.127237000000e-02,
        9.999753000000e-01, 6.931141000000e-03, -1.143899000000e-03, -3.321029000000e-01,
        0.0,0.0,0.0,1.0};
    transform->SetMatrix(R0_rect);
    transform->Concatenate(VeloToCam);
    transform_filter->SetInputData(polyData);
    transform_filter->SetTransform(transform);
    transform_filter->Update();

    // filter
    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputData(transform_filter->GetOutput());

    return glyphFilter;
}

vtkSmartPointer<vtkActor2D> ReadImageFromPNGFile(std::string& filename){
    auto pngReader = vtkSmartPointer<vtkPNGReader>::New();
    pngReader->SetFileName(filename.c_str());

    auto imageActor = vtkSmartPointer<vtkActor2D>::New();
    auto mapper = vtkSmartPointer<vtkImageMapper>::New();
    mapper->SetColorWindow(255);
    mapper->SetColorLevel(127.5);
    mapper->RenderToRectangleOn();
    imageActor->SetMapper(mapper);
    imageActor->GetMapper()->SetInputConnection(pngReader->GetOutputPort());
    // support to mapper
    imageActor->SetPosition2(1, 1);
    // color it first

    // imageActor->GetMapper()->SliceFacesCameraOn();
    // imageActor->GetMapper()->SliceAtFocalPointOn();
    return imageActor;
}

// class Storage{
// public:
// Storage(const std::string storage_dir){
// fd = fopen(storage_dir+"/gt.txt", "w");

// }

// void WriteDataToDisK(){
// for(int i=0;i<this->BoxWidgets.size();i++){
// // for each boxwidget in storage
// this->BoxWidgets[i]->On();
// }
// fwrite(fd, );
// }
// ~Storage(){
// fclose(fd);
// }
// private:
// std::vector<vtkSmartPointer<vtkBoxWidget>> BoxWidgets;
// FILE* fd;

// };
//
//
void MatrixMultiply(double A[4][3], double B[3], double C[4]){
    for(int i=0;i<4;i++){
        C[i] = A[i][0]*B[0] + A[i][1]*B[1] + A[i][2]*B[2];
    }
}

vtkSmartPointer <vtkImplicitBoolean> GenerateFrustum(double PT[4][3], double box[]){
    double l_left[] = {box[0], box[1], box[0], box[3]};
    double l_right[] = {box[0], box[1], box[2], box[1]};
    double l_top[] = {box[2], box[1], box[2], box[3]};
    double l_bottom[] = {box[0], box[3], box[2], box[3]};
    double lines[4][4];
    auto planes = vtkSmartPointer<vtkImplicitBoolean>::New();
    planes->SetOperationTypeToIntersection();
    for(int i=0;i<4;i++){
        lines[0][i] = l_left[i];
        lines[1][i] = l_right[i];
        lines[2][i] = l_top[i];
        lines[3][i] = l_bottom[i];
    }
    for(int i=0;i<4;i++){
        double a = lines[i][3] - lines[i][1];
        double b = lines[i][0] - lines[i][2];
        double c = lines[i][2] * lines[i][1]-lines[i][0]* lines[i][3];
        double B[3] = {a,b,c};
        double C[4];
        MatrixMultiply(PT, B, C);
        auto plane = vtkSmartPointer<vtkPlane>::New();
        if(i==0 or i==3){
            plane->SetNormal(-C[0],-C[0],-C[2]);
        }else{
            plane->SetNormal(C[0],C[1],C[2]);
        }
        // get origin
        double origin[3]={0};
        if(C[2]){
            origin[2]= -C[3]/C[2];
        }else if(C[1]){
            origin[1] = C[3]/C[1];
        }else{
            origin[0] = C[3]/C[0];
        }
        std::cout<<"Origin: "<<origin[0]<<" "<<origin[1]<<" "<<origin[2]<<std::endl;
        std::cout<<"Normal: "<<C[0]<<" "<< C[1]<<" "<<C[2]<<std::endl;
        plane->SetOrigin(origin);
        planes->AddFunction(plane);
    }
    return planes;

}

class InteractorStyle: public vtkInteractorStyleRubberBandPick{
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);
        InteractorStyle(){
            // init selected
            this->ResetCurrentSelection();
            // init colors
            this->Colors = vtkSmartPointer<vtkNamedColors>::New();
            this->fd = fopen("./gt.txt", "w");
        }
        ~InteractorStyle(){
            fclose(this->fd);
        }
        void WriteDataToDisK(){
            auto polyData = vtkSmartPointer<vtkPolyData>::New();
            int num_boxes = this->BoxWidgets.size();
            // dims, center and ry
            double boxes_info[num_boxes][3+3+1];
            std::cout<<"Write "<<num_boxes<<" data to disk in total. "<<std::endl;
            int i = 0;
            for (auto iterator=this->BoxWidgets.begin();iterator!=this->BoxWidgets.end();++iterator, ++i){
                // for each boxwidget in storage
                (*it)->GetPolyData(polyData);
                int num_points = polyData->GetNumberOfPoints();
                std::cout<<"num_points: "<<num_points<<std::endl;
                double coords[num_points][3];
                for(int j=0;j<num_points;j++){
                    polyData->GetPoint(j, coords[j]);
                }
                float l = sqrt(vtkMath::Distance2BetweenPoints(coords[9], coords[8]));
                float h = sqrt(vtkMath::Distance2BetweenPoints(coords[11], coords[10]));
                float w = sqrt(vtkMath::Distance2BetweenPoints(coords[13], coords[12]));
                boxes_info[i][0] = l;
                boxes_info[i][1] = h;
                boxes_info[i][2] = w;
                boxes_info[i][3] = coords[14][0];
                boxes_info[i][4] = coords[14][1];
                boxes_info[i][5] = coords[14][2];
                boxes_info[i][6] = 1.57;

            }
            for(int i=0;i<num_boxes;i++){
                for(int j=0;j<7;j++){
                    fprintf(fd, "%.4f", boxes_info[i][j]);
                    if(j<6){
                        fprintf(fd," ");
                    }
                }
                if(i<num_boxes-1){
                    fprintf(fd, "\n");
                }
            }
        }

        void SetFile(FILE* fd){
            this->fd = fd;
        }

        void SwitchFocus(std::set<vtkSmartPointer<vtkBoxWidget>>::iterator iterator){
            if(iterator==BoxWidgets.end())return;
            this->SelectedBoxWidget = *iterator;
            this->SelectedActor = reinterpret_cast<vtkActor*>((this->SelectedBoxWidget->GetProp3D()));
            this->SelectedMapper = reinterpret_cast<vtkDataSetMapper*>(this->SelectedActor->GetMapper());
        }

        void SwitchFocus(){
            if(!IncreaseIteratorRecurrent()){
                // error when empty
                return;
            }
            SwitchFocus(it);
        }
        void ResetCurrentSelection(){
            // if(this->SelectedActor){
            // this->SelectedActor->GetProperty()->SetColor(Colors->GetColor3d("Green").GetData());
            // }
            if(IsValidBox() or !this->SelectedActor){
                // after append the old data to storage, generate newer one to be selected
                this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
                this->SelectedActor = vtkSmartPointer<vtkActor>::New();
                this->SelectedActor->SetMapper(this->SelectedMapper);

                // init boxwidget
                this->SelectedBoxWidget = vtkSmartPointer<vtkBoxWidget>::New();
                this->SelectedBoxWidget->KeyPressActivationOff();
            }
            // otherwise dont need to allocate new memory
        }

        void ClearCurrentSelection(){
            if(!this->SelectedMapper)return;
            auto polyData = this->SelectedMapper->GetInput();
            if(polyData){
                polyData->Initialize();
            }
        }

        bool IsValidBox(){
            if(!this->SelectedActor)return false;
            if(!this->SelectedActor->GetMapper()) return false;
            auto polyData = reinterpret_cast<vtkPolyData*>(this->SelectedActor->GetMapper()->GetInput());
            return polyData and polyData->GetNumberOfPoints()>0;
        }

        void ResetHorizontalView(vtkCamera* camera){
            camera->SetPosition(0.06046166, -0.00176016,  0.00498102);
            camera->SetFocalPoint(0.07551017, 0.02256332, 1.);
            camera->SetViewUp(0.        , -0.96892317, -0.24736186);
            // camera->ComputeViewPlaneNormal();
            double image_size = 375;
            double focal = 707;
            double view_angle = std::atan(image_size/2/focal)*180/3.1415*2;
            camera->SetViewAngle(view_angle);
            camera->SetClippingRange(0.1, 3000);
            double imageShape[2] = {375, 1242};
            double windowCenter[2] = {2*604/imageShape[1]-1, 2*(1-180/imageShape[0])-1};
            std::cout<<"Window Center: "<<windowCenter<<std::endl;
            camera->SetWindowCenter(windowCenter[0], windowCenter[1]);
        }


        void ResetVerticalView(vtkCamera* camera){
            camera->SetPosition(0.0, -100, 0.0);
            camera->SetFocalPoint(0.0, 0.0, 0.0);
            camera->SetViewUp(-1.0,0.0,  0.0);
            camera->ComputeViewPlaneNormal();
            camera->SetClippingRange(0.1, 3000);
        }

        void PlaceBoxWidget(){
            this->SelectedBoxWidget->SetInteractor(this->GetInteractor());
            this->SelectedBoxWidget->SetProp3D(this->SelectedActor);
            this->SelectedBoxWidget->SetPlaceFactor(1.25);
            this->SelectedBoxWidget->PlaceWidget();
            this->SelectedBoxWidget->Off();
        }

        virtual void  OnRightButtonUp()override{
            vtkInteractorStyleRubberBandPick::OnRightButtonUp();

        }

        void AddBox(){
            if(this->BoxWidgets.find(this->SelectedBoxWidget)!=this->BoxWidgets.end()){
                // already inserted
                return ;
            }
            this->BoxWidgets.insert(this->SelectedBoxWidget);
            // update current iterator
            this->it = this->BoxWidgets.find(this->SelectedBoxWidget);
            // add actor
            this->CurrentRenderer->AddActor(this->SelectedActor);
            // color back to the green(deactivate state)
            // this->SelectedActor->GetProperty()->SetColor(Colors->GetColor3d("Green").GetData());
        }

        bool IncreaseIteratorRecurrent(){
            if(this->it==this->BoxWidgets.end()){
                // current cache is empty
                return false;
            }
            this->it++;
            if(it==this->BoxWidgets.end()){
                // current is end, go back to the first one
                it = this->BoxWidgets.begin();
            }
            return true;

        }

        void RemoveBox(){
            auto it = this->BoxWidgets.find(this->SelectedBoxWidget);
            if(it !=this->BoxWidgets.end()){
                // if find it, remove it
                std::cout<<"Removing Box "<<std::endl;
                // update iterator
                if(this->BoxWidgets.size()==1){
                    // after remove, the cache is empty
                    this->it = this->BoxWidgets.end();
                }else{
                    // reset to the first one
                    this->it = this->BoxWidgets.begin();
                }
                this->CurrentRenderer->RemoveActor((*it)->GetProp3D());
                this->BoxWidgets.erase(it);
            }
        }

        void HandleCurrentSelection(){
            // if null
            if(!this->SelectedBoxWidget and !this->SelectedActor){
                return;
            }
            if(IsValidBox()){
                // can add to cache
                this->AddBox();
            }else{
                // remove from cache automaticly
                this->RemoveBox();
            }
        }

        void PrintCurrentVisibleBox(std::string callerStack){
            std::cout<<"["<<callerStack <<"] num of Actor in CurrentRenderer: "
                <<this->CurrentRenderer->VisibleActorCount()<<std::endl;
            std::cout<<"["<<callerStack <<"] num of box in BoxWidgets: "
                <<this->BoxWidgets.size()<<std::endl;

        }

        void ToggleDisplayAllBoxes(){
            bool enabled;
            enabled = this->SelectedBoxWidget->GetEnabled();
            if(enabled){
                this->SelectedBoxWidget->Off();
            }else{
                this->SelectedBoxWidget->On();
            }
            for(auto it = this->BoxWidgets.begin();it!=this->BoxWidgets.end();it++){
                // for each boxwidget in storage
                if(enabled){
                    (*it)->Off();
                }else{
                    (*it)->On();
                }
            }
        }



        virtual void OnChar()override{
            auto renderer = this->GetCurrentRenderer();
            auto camera = renderer->GetActiveCamera();
            auto key = this->Interactor->GetKeyCode();
            std::cout<<"Pressed: "<<key<<std::endl;
            auto keySym = std::string(this->Interactor->GetKeySym());
            if(keySym=="Tab"){
                PrintCurrentVisibleBox("Pressing Tab");
                // handle current box first
                HandleCurrentSelection();

                // switch among boxwigets
                SwitchFocus();
            }
            if(key=='q' or key=='Q'){
                // write data to disk
                this->WriteDataToDisK();
            }
            vtkInteractorStyleRubberBandPick::OnChar();
            switch(key){
                case 'h':
                    this->ResetHorizontalView(camera);
                    break;
                case 'v':
                    this->ResetVerticalView(camera);
                    break;
                case 'c':
                    ClearCurrentSelection();
                    // this->ExtractPoints();
                    break;
                case 'i':
                    ToggleDisplayAllBoxes();
                    break;
                case 'r':
                    // switch text
                    this->ToggleText();
                    break;
            }
            UpdateRender();
        }

        void SetTextActor(vtkTextActor* textActor){
            this->textActor = textActor;
        }

        void ToggleText(){
            std::string flag;
            if(this->CurrentMode){
                flag = "EDIT";
            }else{
                flag = "VIEW";
            }
            textActor->SetInput(flag.c_str());
        }
        // void ExtractPoints(){
        // this->ExtractPoints(this->Points);
        // }

        void ExtractPoints(vtkPolyData* polyData, vtkImplicitFunction* selectedRegion){
            auto extractGeometry = vtkSmartPointer<vtkExtractGeometry>::New();
            extractGeometry->SetImplicitFunction(selectedRegion);
            extractGeometry->SetInputData(polyData);
            extractGeometry->Update();

            auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
            glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
            glyphFilter->Update();

            auto selected = glyphFilter->GetOutput();
            // std::cout<< "Selected "<<selected->GetNumberOfPoints()<<" points."<<std::endl;
            // std::cout<< "Selected "<<selected->GetNumberOfCells()<<" cells."<<std::endl;
            this->SelectedMapper->SetInputData(selected);
            this->SelectedMapper->ScalarVisibilityOff();
            this->SelectedActor->GetProperty()->SetPointSize(3);

            this->AddBox();
            this->PlaceBoxWidget();
        }

        void PickActor(){
            int* clickPos = this->GetInteractor()->GetEventPosition();
            auto picker = vtkSmartPointer<vtkPropPicker>::New();
            picker->Pick(clickPos[0], clickPos[1], 0, this->GetCurrentRenderer());
            auto pickedActor = picker->GetActor();
            if(pickedActor){
                // std::cout<<"Picked Actor: "<<pickedActor<<std::endl;
                // std::cout<<"Current Actor: "<<this->SelectedActor <<std::endl;
                auto pickedIterator = BoxWidgets.end();
                for(auto it=this->BoxWidgets.begin();it!=this->BoxWidgets.end();it++){
                    if((*it)->GetProp3D()==pickedActor){
                        pickedIterator = it;
                        break;
                    }
                }
                SwitchFocus(pickedIterator);
                std::cout<<"[PickActor] Actor's addr: "<<pickedActor<<std::endl;
            }else{
                std::cout<<"[PickActor] "<<"Nothing is picked! "<<std::endl;
            }
        }

        void ContinueSelect(){
        }

        bool IsSelectPoint(){
            bool selectPoint = (this->StartPosition[0]==this->EndPosition[0]) and
                (this->StartPosition[1]==this->EndPosition[1]);
            return selectPoint;
        }

        void UpdateRender(){
            for(auto it=this->BoxWidgets.begin();it!=this->BoxWidgets.end();it++){
                reinterpret_cast<vtkActor*>((*it)->GetProp3D())->GetProperty()
                    ->SetColor(Colors->GetColor3d("Green").GetData());
            }
            this->SelectedActor->GetProperty()->SetColor(Colors->GetColor3d("Red").GetData());
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);
        }

        virtual void OnLeftButtonUp() override{
            std::cout<<"Current renderer "<<this->GetCurrentRenderer()<<std::endl;
            // Forward Event
            vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
            if(!this->CurrentMode or IsSelectPoint()){
                // pick actor as selectedActor, used for modify precisely
                this->PickActor();
            }else{

                vtkSmartPointer<vtkImplicitFunction> frustum_deepcopy;
                if(this->GetCurrentRenderer()==this->ImageRenderer){
                    std::cout<<"Over Image Region "<<std::endl;
                    this->SetCurrentRenderer(this->PointCloudRenderer);
                    int windowSize[2];
                    this->GetInteractor()->GetSize(windowSize);
                    double scales[2] = {windowSize[0]/1242.0,windowSize[1]/2.0/375.0};
                    double box[4] = {double(this->StartPosition[0])/scales[0],
                                    double(windowSize[1]-this->StartPosition[1])/scales[1],
                                    double(this->EndPosition[0])/scales[0],
                                    double(windowSize[1]-this->EndPosition[1])/scales[1]};
                    std::cout<<"Selected Region "<<box[0]<<" "<<box[1]<<" "<<box[2]<<" "<<box[3]<<std::endl;
                    std::cout<<"Window Size: "<<windowSize[0]<<" "<<windowSize[1]<<std::endl;
                    double PT[4][3] = {{7.070493000000e+02,0,0},
                        {0,7.070493000000e+02, 0},
                        {6.040814000000e+02,1.805066000000e+02,1},
                        {4.575831000000e+01,-3.454157000000e-01,4.981016000000e-03}};
                    frustum_deepcopy = GenerateFrustum(PT, box);
                }else{

                    auto frustum  = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();
                    // deepcopy planes
                    vtkSmartPointer<vtkPlanes> frustum_deepcopy1 = vtkSmartPointer<vtkPlanes>::New();
                    frustum_deepcopy1->SetNormals(frustum->GetNormals());
                    frustum_deepcopy1->SetPoints(frustum->GetPoints());
                    frustum_deepcopy = frustum_deepcopy1;
                    std::cout<<"Over PointCloud Region "<<std::endl;
                }



                auto *rwi = this->GetInteractor();
                HandleCurrentSelection();
                if(!rwi->GetControlKey()){
                    // reset first
                    ResetCurrentSelection();

                    // select from the world
                    this->ExtractPoints(this->Points, frustum_deepcopy);
                }else{
                    // select from current polydata
                    auto polyData = reinterpret_cast<vtkPolyData*>(this->SelectedActor->GetMapper()->GetInput());
                    if(IsValidBox()){
                        this->ExtractPoints(polyData, frustum_deepcopy);
                    }else{
                        std::cout<<"Cannot execute union operator due to selection from invalid polydata. "<<std::endl;
                    }
                }
            }
            PrintCurrentVisibleBox("OnLeftButtonUp");
            UpdateRender();

        }
        void SetPoints(vtkSmartPointer<vtkPolyData> points){
            this->Points = points;
        }
        void SetImageRenderer(vtkRenderer* renderer){
            this->ImageRenderer = renderer;
        }
        void SetPointCloudRenderer(vtkRenderer* renderer){
            this->PointCloudRenderer = renderer;
        }
    private:
        vtkSmartPointer<vtkPolyData> Points;
        vtkSmartPointer<vtkActor> SelectedActor;
        vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
        vtkSmartPointer<vtkBoxWidget> SelectedBoxWidget;
        std::set<vtkSmartPointer<vtkBoxWidget>> BoxWidgets;
        std::set<vtkSmartPointer<vtkBorderWidget>> BorderWidgets;

        vtkSmartPointer<vtkTextActor> textActor;
        FILE* fd;

        std::set<vtkSmartPointer<vtkBoxWidget>>::iterator it;
        vtkSmartPointer<vtkNamedColors> Colors;
        vtkSmartPointer<vtkRenderer> ImageRenderer;
        vtkSmartPointer<vtkRenderer> PointCloudRenderer;
};

vtkStandardNewMacro(InteractorStyle);

int main(){
    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    // auto pointSource = vtkSmartPointer<vtkPointSource>::New();
    // pointSource->SetNumberOfPoints(20);
    // pointSource->Update();
    std::string filename = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync/velodyne_points/data/0000000000.bin";

    auto glyphFilter = ReadPointCloudFromBin(filename);

    auto idFilter = vtkSmartPointer<vtkIdFilter>::New();
    idFilter->SetInputConnection(glyphFilter->GetOutputPort());
#if VTK890
    idFilter->SetCellIdsArrayName("OriginalIds");
    idFilter->SetPointIdsArrayName("OriginalIds");
#else
    idFilter->SetIdsArrayName("OriginalIds");
#endif
    idFilter->Update();

    auto surfaceFilter=vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(surfaceFilter->GetOutputPort());
    // mapper->SetInputConnection(glyphFilter->GetOutputPort());
    mapper->ScalarVisibilityOff();

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

    // image actor
    std::string imageFileName = "/home/breakpoint/Data/2011_09_26/2011_09_26_drive_0001_sync/image_02/data/0000000000.png";
    auto imageActor = ReadImageFromPNGFile(imageFileName);
    auto imageRenderer = vtkSmartPointer<vtkRenderer>::New();
    imageRenderer->AddActor(imageActor);
    imageRenderer->SetViewport(0,0.5, 1,1);
    renderer->SetViewport(0,0, 1,0.5);
    // double dpi = 8000;
    // const int extent[6] = {0, int(1242/dpi),0,int(375/dpi),0,0 };
    // double focalpoint[3] = {0.07551017, 0.02256332, 1.};
    // imageActor->SetDisplayExtent(extent);
    // imageActor->SetPosition(focalpoint);
    // imageActor->SetScale(1/dpi);
    // imageActor->RotateX(180);
    // imageActor->GetMapper()->SliceFacesCameraOn();
    // imageActor->GetMapper()->SliceAtFocalPointOn();
    // renderer->AddActor(imageActor);


    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->AddRenderer(imageRenderer);
    renderWindow->SetSize(1600, 800);

    auto areaPicker = vtkSmartPointer<vtkAreaPicker>::New();

    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetPicker(areaPicker);

    renderer->SetBackground(colors->GetColor3d("Black").GetData());
    imageRenderer->SetBackground(colors->GetColor3d("Blue").GetData());

    std::cout<<"image renderer: "<<imageRenderer<<std::endl;
    std::cout<<"pc renderer: "<<renderer<<std::endl;

    // customs style
    auto style = vtkSmartPointer<InteractorStyle>::New();
    style->SetPoints(surfaceFilter->GetOutput());
    renderWindowInteractor->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);
    style->SetImageRenderer(imageRenderer);
    style->SetPointCloudRenderer(renderer);

    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("VIEW");
    textActor->SetPosition2( 10, 40 );
    textActor->GetTextProperty()->SetFontSize ( 24 );
    textActor->GetTextProperty()->SetColor ( 1.0, 0.0, 0.0 );
    renderer->AddActor2D(textActor);
    style->SetTextActor(textActor);

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

}
