#include "AnnotationStyle.h"
#include "Display.h"

// vtkStandardNewMacro(AnnotationStyle);
void MatrixMultiply(double A[4][3], double B[3], double C[4]){
    for(int i=0;i<4;i++){
        C[i] = A[i][0]*B[0] + A[i][1]*B[1] + A[i][2]*B[2];
    }
}

void Transpose(double PT[3][4], double PT_trans[4][3]){
    for(int i=0;i<4;i++){
        for(int j=0;j<3;j++){
            PT_trans[i][j] = PT[j][i];
        }
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
// AnnotationStyle::AnnotationStyle(){
    // set dataloader
// }

// void AnnotationStyle::WriteDataToDisK(){
    // auto polyData = vtkSmartPointer<vtkPolyData>::New();
    // int num_boxes = this->AnnotationWidgets.size();
    // // dims, center and ry
    // int cols = 11;
    // double boxes_info[num_boxes][cols] = {0};
    // std::cout<<"Write "<<num_boxes<<" data to disk in total. "<<std::endl;
    // int i = 0;
    // for (auto iterator=this->AnnotationWidgets.begin();iterator!=this->AnnotationWidgets.end();++iterator, ++i){
    // // for each boxwidget in storage
    // // boxes_3d
    // it->AnnotationBoxWidget->GetPolyData(polyData);
    // int num_points = polyData->GetNumberOfPoints();
    // std::cout<<"num_points: "<<num_points<<std::endl;
    // double coords[num_points][3];
    // for(int j=0;j<num_points;j++){
    // polyData->GetPoint(j, coords[j]);
    // }
    // float l = sqrt(vtkMath::Distance2BetweenPoints(coords[9], coords[8]));
    // float h = sqrt(vtkMath::Distance2BetweenPoints(coords[11], coords[10]));
    // float w = sqrt(vtkMath::Distance2BetweenPoints(coords[13], coords[12]));
    // boxes_info[i][0] = l;
    // boxes_info[i][1] = h;
    // boxes_info[i][2] = w;
    // boxes_info[i][3] = coords[14][0];
    // boxes_info[i][4] = coords[14][1];
    // boxes_info[i][5] = coords[14][2];
    // boxes_info[i][6] = 1.57;

    // // boxes_2d
    // auto *rep = it->AnnotationBorderWidget->GetBorderRepresentation();
    // if(rep){
    // double *fpos1 = rep->GetPositionCoordinate()->GetValue();
    // double *fpos2 = rep->GetPosition2Coordinate()->GetValue();
    // boxes_info[i][7] = fpos1[0];
    // boxes_info[i][8] = fpos1[1];
    // boxes_info[i][9] = fpos2[0];
    // boxes_info[i][10] = fpos2[1];
    // }

    // }
    // for(int i=0;i<num_boxes;i++){
    // for(int j=0;j<cols;j++){
    // fprintf(fd, "%.4f", boxes_info[i][j]);
    // if(j<cols-1){
    // fprintf(fd," ");
    // }
    // }
    // if(i<num_boxes-1){
    // fprintf(fd, "\n");
    // }
    // }
// }

void AnnotationStyle::SwitchFocus(std::set<vtkSmartPointer<AnnotationWidget>>::iterator iterator){
    if(iterator==AnnotationWidgets.end())return;
    this->SelectedWidget = *iterator;
}

void AnnotationStyle::SwitchFocus(){
    if(!IncreaseIteratorRecurrent()){
        // error when empty
        return;
    }
    SwitchFocus(it);
}

void AnnotationStyle::ResetCurrentSelection(){
    if(!this->SelectedWidget or this->SelectedWidget->IsValidBox()){
        this->SelectedWidget = vtkSmartPointer<AnnotationWidget>::New();
        this->SelectedWidget->Initialize(this);
        // InitAnnotationWidget();
    }
    // otherwise dont need to allocate new memory
}



void AnnotationStyle::ResetHorizontalView(vtkCamera* camera){
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

void AnnotationStyle::ResetVerticalView(vtkCamera* camera){
    camera->SetPosition(0.0, -100, 0.0);
    camera->SetFocalPoint(0.0, 0.0, 0.0);
    camera->SetViewUp(-1.0,0.0,  0.0);
    camera->ComputeViewPlaneNormal();
    camera->SetClippingRange(0.1, 3000);
}


void AnnotationStyle::ComputeBorderWidgetPosition(double *position, double* position2){
    int X = this->StartPosition[0];
    int Y = this->StartPosition[1];
    double XF = static_cast<double>(X);
    double YF = static_cast<double>(Y);
    this->CurrentRenderer->DisplayToNormalizedDisplay(XF,YF);
    this->CurrentRenderer->NormalizedDisplayToViewport(XF,YF);
    this->CurrentRenderer->ViewportToNormalizedViewport(XF,YF);
    position[0] = XF;
    position[1] = YF;

    X = this->EndPosition[0];
    Y = this->EndPosition[1];
    XF = static_cast<double>(X);
    YF = static_cast<double>(Y);
    this->CurrentRenderer->DisplayToNormalizedDisplay(XF,YF);
    this->CurrentRenderer->NormalizedDisplayToViewport(XF,YF);
    this->CurrentRenderer->ViewportToNormalizedViewport(XF,YF);
    position2[0] = XF;
    position2[1] = YF;

    double tmp = position[1];
    position[1] = position2[1];
    position2[1] = tmp;
    // position2 is offset reference to position
    position2[0] = position2[0] - position[0];
    position2[1] = position2[1] - position[1];
}



void  AnnotationStyle::OnRightButtonUp(){
    vtkInteractorStyleRubberBandPick::OnRightButtonUp();

}

void AnnotationStyle::AddBox(){
    if(this->AnnotationWidgets.find(this->SelectedWidget)!=this->AnnotationWidgets.end()){
        // already inserted
        return ;
    }
    this->AnnotationWidgets.insert(this->SelectedWidget);
    // update current iterator
    this->it = this->AnnotationWidgets.find(this->SelectedWidget);
    // add actor
    this->SelectedWidget->AddToRenderer();
}

bool AnnotationStyle::IncreaseIteratorRecurrent(){
    if(this->it==this->AnnotationWidgets.end()){
        // current cache is empty
        return false;
    }
    this->it++;
    if(it==this->AnnotationWidgets.end()){
        // current is end, go back to the first one
        it = this->AnnotationWidgets.begin();
    }
    return true;

}


void AnnotationStyle::RemoveBox(){
    auto it = this->AnnotationWidgets.find(this->SelectedWidget);
    if(it !=this->AnnotationWidgets.end()){
        // if find it, remove it
        std::cout<<"Removing Box "<<std::endl;
        // update iterator
        if(this->AnnotationWidgets.size()==1){
            // after remove, the cache is empty
            this->it = this->AnnotationWidgets.end();
        }else{
            // reset to the first one
            this->it = this->AnnotationWidgets.begin();
        }
        // this->PointCloudRenderer->RemoveActor(it->PointsActor);
        auto widget = *it;
        widget->RemoveFromRenderer();
        // close first
        widget->Off();
        this->PrintCurrentVisibleBox("RemoveBox");
        // it->AnnotationBorderWidget->Off();
        // them remove them from storage
        this->AnnotationWidgets.erase(it);
    }
}

void AnnotationStyle::HandleCurrentSelection(){
    // if null
    // if(this->SelectedWidget->IsValidBox()){
        // // can add to cache
        // this->AddBox();
    // }else{
        // // remove from cache automaticly
        // this->RemoveBox();
    // }
}

void AnnotationStyle::PrintCurrentVisibleBox(std::string callerStack){
    std::cout<<"["<<callerStack <<"] num of Actor in ImageRenderer: "
        <<this->ImageRenderer->VisibleActorCount()<<std::endl;
    std::cout<<"["<<callerStack <<"] num of Actor in PointCloudRenderer: "
        <<this->PointCloudRenderer->VisibleActorCount()<<std::endl;
    std::cout<<"["<<callerStack <<"] num of box in BoxWidgets: "
        <<this->AnnotationWidgets.size()<<std::endl;

}

void AnnotationStyle::OffAllBoxes(){
    for(auto it = this->AnnotationWidgets.begin();it!=this->AnnotationWidgets.end();it++){
        auto widget = *it;
        widget->Off();
    }
}

void AnnotationStyle::ClearCurrentFrame(){
    // clear image and pointcloud, prepare for loading next frame
    // should off all boxes first otherwise program will crash
    this->OffAllBoxes();
    this->AnnotationWidgets.clear();
    this->RemoveAllFromPointCloudRenderer();
    this->RemoveAllFromImageRenderer();
    // this->ResetCurrentSelection();
}



void AnnotationStyle::OnChar(){
    auto renderer = this->PointCloudRenderer;
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
    }else if(keySym=="Right"){
        this->SelectedWidget->HorizontalRotateClockwise(AngleAdjustPrecision);
    }else if(keySym=="Left"){
        this->SelectedWidget->HorizontalRotateUnClockwise(AngleAdjustPrecision);
    }else if(keySym=="Up"){
        this->Save();
        this->ClearCurrentFrame();
        PrintCurrentVisibleBox("Up");
        this->AnnotationDataloader->LoadPrev();
        // this->InitAnnotationWidgetFromLabel();
        this->InitAnnotationWidget();
        this->Displayer->SetTitle();
    }else if(keySym=="Down"){
        this->Save();
        this->ClearCurrentFrame();
        PrintCurrentVisibleBox("Down");
        this->AnnotationDataloader->LoadNext();
        this->InitAnnotationWidget();
        this->Displayer->SetTitle();
    }
    if(key=='q' or key=='Q'){
        // write data to disk
        // this->WriteDataToDisK();
        this->Save();
    }
    std::cout<<"key: "<<keySym<<std::endl;
    vtkInteractorStyleRubberBandPick::OnChar();
    switch(key){
        case 'n':
            this->SelectedWidget->HorizontalRotate90Arrow();
            break;
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
        case 'a':
            break;
    }
    UpdateRender();
}

void AnnotationStyle::ToggleDisplayAllBoxes(){
    bool enabled;
    enabled = this->SelectedWidget->GetEnabled();
    if(enabled){
        this->SelectedWidget->Off();
    }else{
        this->SelectedWidget->On();
    }
    for(auto it = this->AnnotationWidgets.begin();it!=this->AnnotationWidgets.end();it++){
        // for each boxwidget in storage
        auto widget = *it;
        if(enabled){
            widget->Off();
        }else{
            widget->On();
        }
    }
}

void AnnotationStyle::SetTextActor(vtkTextActor* textActor){
    this->textActor = textActor;
}

void AnnotationStyle::ToggleText(){
    std::string flag;
    if(this->CurrentMode){
        flag = "EDIT";
    }else{
        flag = "VIEW";
    }
    textActor->SetInput(flag.c_str());
}

void AnnotationStyle::ExtractPoints(vtkPolyData* polyData, vtkImplicitFunction* selectedRegion){
    auto extractGeometry = vtkSmartPointer<vtkExtractGeometry>::New();
    extractGeometry->SetImplicitFunction(selectedRegion);
    extractGeometry->SetInputData(polyData);
    extractGeometry->Update();

    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
    glyphFilter->Update();

    auto selected = glyphFilter->GetOutput();
    this->SelectedWidget->SetPolyData(selected);

}



void AnnotationStyle::PickActor(){
    int* clickPos = this->GetInteractor()->GetEventPosition();
    auto picker = vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, this->GetCurrentRenderer());
    auto pickedActor = picker->GetActor();
    if(pickedActor){
        auto pickedIterator = AnnotationWidgets.end();
        for(auto it=this->AnnotationWidgets.begin();it!=this->AnnotationWidgets.end();it++){
            auto widget = *it;
            if(widget->GetActor()==pickedActor){
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

bool AnnotationStyle::IsSelectPoint(){
    bool selectPoint = (this->StartPosition[0]==this->EndPosition[0]) and
        (this->StartPosition[1]==this->EndPosition[1]);
    return selectPoint;
}

void AnnotationStyle::UpdateRender(){
    for(auto it=this->AnnotationWidgets.begin();it!=this->AnnotationWidgets.end();it++){
        auto widget = *it;
        widget->SetDeactiveColor();
    }
    if(this->SelectedWidget){
        this->SelectedWidget->SetActiveColor();
    }
    this->GetInteractor()->GetRenderWindow()->Render();
    this->HighlightProp(NULL);
}

void AnnotationStyle::OnLeftButtonUp(){
    std::cout<<"Current renderer "<<this->GetCurrentRenderer()<<std::endl;
    // Forward Event
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
    if(!this->CurrentMode or IsSelectPoint()){
        // pick actor as selectedActor, used for modify precisely
        this->PickActor();
    }else{
        vtkSmartPointer<vtkImplicitFunction> frustum_deepcopy;
        bool IsImageEvent = this->GetCurrentRenderer()==this->ImageRenderer;
        if(IsImageEvent){
            std::cout<<"Over Image Region "<<std::endl;
            // this->SetCurrentRenderer(this->PointCloudRenderer);
            int windowSize[2];
            this->GetInteractor()->GetSize(windowSize);
            double scales[2] = {windowSize[0]/1242.0,windowSize[1]/2.0/375.0};
            double box[4] = {double(this->StartPosition[0])/scales[0],
                double(windowSize[1]-this->StartPosition[1])/scales[1],
                double(this->EndPosition[0])/scales[0],
                double(windowSize[1]-this->EndPosition[1])/scales[1]};
            std::cout<<"Selected Region "<<box[0]<<" "<<box[1]<<" "<<box[2]<<" "<<box[3]<<std::endl;
            std::cout<<"Window Size: "<<windowSize[0]<<" "<<windowSize[1]<<std::endl;
            // double PT[4][3] = {{7.070493000000e+02,0,0},
                // {0,7.070493000000e+02, 0},
                // {6.040814000000e+02,1.805066000000e+02,1},
                // {4.575831000000e+01,-3.454157000000e-01,4.981016000000e-03}};
            double PT[3][4], PT_trans[4][3];
            this->AnnotationDataloader->GetProjectMatrix(PT);
            Transpose(PT, PT_trans);
            frustum_deepcopy = GenerateFrustum(PT_trans, box);
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
            auto polyData = this->SelectedWidget->GetPolyData();
            if(this->SelectedWidget->IsValidBox()){
                this->ExtractPoints(polyData, frustum_deepcopy);
            }else{
                std::cout<<"Cannot execute union operator due to selection from invalid polydata. "<<std::endl;
            }
        }
        // add points actor and arrow actor
        this->AddBox();

        // place box and arrow
        this->SelectedWidget->PlaceAnnotationBoxWidget();
        this->SelectedWidget->PlaceArrowActor();
        if(IsImageEvent){
            // if it happened in point cloud viewport, not necessary to draw box

            double position[3];
            double position2[3];
            this->ComputeBorderWidgetPosition(position, position2);
            this->SelectedWidget->PlaceAnnotationBorderWidget(position, position2);
        }
    }
    PrintCurrentVisibleBox("OnLeftButtonUp");
    UpdateRender();

}
