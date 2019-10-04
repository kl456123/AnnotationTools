#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

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

// boxwidget
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>

#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>


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

    // filter
    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputData(polyData);
    return glyphFilter;
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

class InteractorStyle: public vtkInteractorStyleRubberBandPick{
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);
        InteractorStyle(){
            this->Reset();
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
            for(int i=0;i<num_boxes;i++){
                // for each boxwidget in storage
                this->BoxWidgets[i]->GetPolyData(polyData);
                int num_points = polyData->GetNumberOfPoints();
                double coords[num_points][3];
                for(int j=0;j<num_points;j++){
                    polyData->GetPoint(j, coords[j]);
                }
                float l = vtkMath::Distance2BetweenPoints(coords[9], coords[8]);
                float h = vtkMath::Distance2BetweenPoints(coords[11], coords[10]);
                float w = vtkMath::Distance2BetweenPoints(coords[13], coords[12]);
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
            // fwrite(boxes_info, sizeof(boxes_info[0]),num_boxes, fd);
        }

        void SetFile(FILE* fd){
            this->fd = fd;
        }
        void Reset(){
            // after append the old data to storage, generate newer one to be selected
            this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            this->SelectedActor = vtkSmartPointer<vtkActor>::New();
            this->SelectedActor->SetMapper(this->SelectedMapper);
            this->SelectedRegions = vtkSmartPointer<vtkImplicitBoolean>::New();
            this->SelectedRegions->SetOperationTypeToIntersection();

            // init boxwidget
            this->SelectedBoxWidget = vtkSmartPointer<vtkBoxWidget>::New();
        }
        void ResetHorizontalView(vtkCamera* camera){
            camera->SetPosition(0.0, 0.0, -100);
            camera->SetFocalPoint(0.0, 0.0, 0.0);
            camera->SetViewUp(0.0, -1.0, 0.0);
            camera->ComputeViewPlaneNormal();
            camera->SetClippingRange(0.1, 3000);
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
            // this->GetInteractor()->Print(std::cout);
            // if(this->SelectedBoxWidget->GetInteractor()==this->GetInteractor()){
            // std:cout<<"Equal"<<std::endl;
            // }

            this->SelectedBoxWidget->SetProp3D(this->SelectedActor);
            this->SelectedBoxWidget->SetPlaceFactor(1.25);
            this->SelectedBoxWidget->PlaceWidget();
            this->SelectedBoxWidget->Off();
        }

        virtual void  OnRightButtonUp()override{
            vtkInteractorStyleRubberBandPick::OnRightButtonUp();

        }
        virtual void OnKeyDown()override{
            auto *rwi = this->Interactor;
            std::string key = rwi->GetKeySym();
            // renderers collections
            // std::cout<<"asdg"<<std::endl;
            // auto renderers = this->GetInteractor()->GetRenderWindow()->GetRenderers();
            // std::cout<<"num of renderers: "<<renderers->GetNumberOfItems()<<std::endl;
            // auto renderer = renderers->GetFirstRenderer();
            auto renderer = this->GetCurrentRenderer();
            // std::cout<<this->CurrentRenderer<<std::endl;
            auto camera = renderer->GetActiveCamera();
            // auto camera = this->CurrentRenderer->GetActiveCamera();
            std::cout<<"Pressed: "<<key<<std::endl;
            vtkInteractorStyleRubberBandPick::OnKeyDown();
            if(key=="h"){
                this->ResetHorizontalView(camera);

            }else if(key=="v"){
                this->ResetVerticalView(camera);
            }else if(key=="c"){
                this->SelectedRegions->GetFunction()->RemoveAllItems();
                // this->CurrentRenderer->RemoveActor(this->SelectedActor);
                this->ExtractPoints();
            }else if(key=="a"){
                // append currenct boxwidget to store_lists
                this->BoxWidgets.push_back(this->SelectedBoxWidget);
                this->Reset();
            }else if(key=="j"){
                this->SelectedBoxWidget->On();
                for(int i=0;i<this->BoxWidgets.size();i++){
                    // for each boxwidget in storage
                    this->BoxWidgets[i]->On();
                }
            }else if(key=="m"){
                // write data to disk
                this->WriteDataToDisK();
            }else if(key=="r"){
                // switch text
                this->ToggleText();
            }
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);
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
            // textActor->SetPosition2( 10, 40 );
            // textActor->GetTextProperty()->SetFontSize ( 24 );
            // textActor->GetTextProperty()->SetColor ( 1.0, 0.0, 0.0 );
            // this->CurrentRenderer->AddActor2D(textActor);
        }

        void ExtractPoints(){
            auto extractGeometry = vtkSmartPointer<vtkExtractGeometry>::New();
            extractGeometry->SetImplicitFunction(this->SelectedRegions);
            extractGeometry->SetInputData(this->Points);
            extractGeometry->Update();

            auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
            glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
            glyphFilter->Update();

            auto selected = glyphFilter->GetOutput();
            std::cout<< "Selected "<<selected->GetNumberOfPoints()<<" points."<<std::endl;
            std::cout<< "Selected "<<selected->GetNumberOfCells()<<" cells."<<std::endl;
            this->SelectedMapper->SetInputData(selected);
            this->SelectedMapper->ScalarVisibilityOff();

            auto colors = vtkSmartPointer<vtkNamedColors>::New();
            this->SelectedActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
            this->SelectedActor->GetProperty()->SetPointSize(3);

            this->CurrentRenderer->AddActor(this->SelectedActor);

            this->PlaceBoxWidget();
        }

        virtual void OnLeftButtonUp() override{
            // std::cout<<"OnLeftButtonUp First"<<std::endl;
            std::cout<<"StartPosition: "<<this->StartPosition[0]<<", "<<this->StartPosition[1]<<std::endl;
            std::cout<<"EndPosition: "<<this->EndPosition[0]<<", "<<this->EndPosition[1]<<std::endl;
            bool SelectPoint = (this->StartPosition[0]==this->EndPosition[0]) and
                (this->StartPosition[1]==this->EndPosition[1]);
            // Forward Event
            std::cout<<this->CurrentRenderer<<std::endl;
            vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
            if(!this->CurrentMode or SelectPoint) return;
            vtkPlanes* frustum  = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();
            // deepcopy planes
            vtkSmartPointer<vtkPlanes> frustum_deepcopy = vtkSmartPointer<vtkPlanes>::New();
            frustum_deepcopy->SetNormals(frustum->GetNormals());
            frustum_deepcopy->SetPoints(frustum->GetPoints());

            auto *rwi = this->GetInteractor();
            if(!rwi->GetControlKey()){
                this->SelectedRegions->GetFunction()->RemoveAllItems();
            }
            this->SelectedRegions->AddFunction(frustum_deepcopy);

            this->ExtractPoints();

            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);

            // num of actors
            std::cout<<"num of actor: "<<this->CurrentRenderer->VisibleActorCount()<<std::endl;
            std::cout<<"current mode: "<<this->CurrentMode<<std::endl;
        }
        void SetPoints(vtkSmartPointer<vtkPolyData> points){
            this->Points = points;
        }
    private:
        vtkSmartPointer<vtkPolyData> Points;
        vtkSmartPointer<vtkActor> SelectedActor;
        vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
        vtkSmartPointer<vtkImplicitBoolean> SelectedRegions;
        vtkSmartPointer<vtkBoxWidget> SelectedBoxWidget;
        std::vector<vtkSmartPointer<vtkBoxWidget>> BoxWidgets;

        vtkSmartPointer<vtkTextActor> textActor;
        FILE* fd;
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
    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    auto areaPicker = vtkSmartPointer<vtkAreaPicker>::New();

    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetPicker(areaPicker);

    renderer->SetBackground(colors->GetColor3d("Black").GetData());

    // customs style
    auto style = vtkSmartPointer<InteractorStyle>::New();
    style->SetPoints(surfaceFilter->GetOutput());
    renderWindowInteractor->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);

    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("VIEW");
    textActor->SetPosition2( 10, 40 );
    textActor->GetTextProperty()->SetFontSize ( 24 );
    textActor->GetTextProperty()->SetColor ( 1.0, 0.0, 0.0 );
    renderer->AddActor2D(textActor);
    style->SetTextActor(textActor);


    // fd = fopen(storage_dir+"/gt.txt", "w");
    // std::string storage_dir = ".";
    // style->SetFile(storage_dir);

    // vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
    // textActor->SetInput ( "Hello world" );
    // textActor->SetPosition2 ( 10, 40 );
    // textActor->GetTextProperty()->SetFontSize ( 24 );
    // textActor->GetTextProperty()->SetColor ( 1.0, 0.0, 0.0 );
    // renderer->AddActor2D ( textActor );

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

}
