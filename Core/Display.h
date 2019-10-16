#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include <iostream>
#include <string>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkAreaPicker.h>

#include "DataLoader.h"
#include "AnnotationStyle.h"

class Display : public vtkObjectBase{
    public:
        static Display* New(){
            auto *result = new Display;
            result->InitializeObjectBase();
            return result;
        }
        Display(){
            AnnotationDataloader = vtkSmartPointer<DataLoader>::New();
            ImageRenderer = vtkSmartPointer<vtkRenderer>::New();
            PointCloudRenderer = vtkSmartPointer<vtkRenderer>::New();
        }

        void ReadConfigFromFile(){
        }

        void SetTitle(){
            auto title = AnnotationDataloader->GetCurrentFileName();
            this->RenderWindow->SetWindowName(title.c_str());
        }

        void Initialize(std::string root_dir){
            AnnotationDataloader->Initialize(root_dir);
            InitializeRenderer();
            InitializeText();

            InitializeStyle();
            InitializeRenderWindow();
            InitializeInteractor();

            // style need interactor to initialize widget
            Style->InitAnnotationWidget();
            Style->SetDisplay(this);
            SetTitle();
        }

        void InitializeRenderer(){
            // image and pointcloud
            PointCloudRenderer->AddActor(AnnotationDataloader->GetPointCloudActor());
            ImageRenderer->AddActor(AnnotationDataloader->GetImageActor());

            // set viewport
            ImageRenderer->SetViewport(0,0.5, 1,1);
            PointCloudRenderer->SetViewport(0,0, 1,0.5);

            // color them
            auto colors = vtkSmartPointer<vtkNamedColors>::New();
            PointCloudRenderer->SetBackground(colors->GetColor3d("Black").GetData());
        }

        void InitializeInteractor(){
            // setup picker
            AreaPicker = vtkSmartPointer<vtkAreaPicker>::New();
            RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

            RenderWindowInteractor->SetRenderWindow(RenderWindow);
            RenderWindowInteractor->SetPicker(AreaPicker);
            RenderWindowInteractor->SetInteractorStyle(Style);
        }

        void InitializeRenderWindow(){
            RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
            RenderWindow->AddRenderer(PointCloudRenderer);
            RenderWindow->AddRenderer(ImageRenderer);
            // RenderWindow->FullScreenOn();
            // RenderWindow->SetFullScreen(true);
            RenderWindow->SetSize(1920, 1080);
        }

        void InitializeStyle(){
            Style = vtkSmartPointer<AnnotationStyle>::New();
            Style->SetPoints(AnnotationDataloader->GetPoints());
            Style->SetCurrentRenderer(PointCloudRenderer);
            Style->SetImageRenderer(ImageRenderer);
            Style->SetPointCloudRenderer(PointCloudRenderer);
            Style->SetDataLoader(AnnotationDataloader);
            Style->SetTextActor(TextActor);
        }
        void Start(){
            RenderWindowInteractor->Start();
        }

        void InitializeText(){
            TextActor = vtkSmartPointer<vtkTextActor>::New();
            TextActor->SetInput("VIEW");
            TextActor->GetTextProperty()->SetFontSize ( 24 );
            TextActor->GetTextProperty()->SetColor ( 1.0, 0.0, 0.0 );
            PointCloudRenderer->AddActor2D(TextActor);
        }

    private:
        vtkSmartPointer<DataLoader> AnnotationDataloader;
        vtkSmartPointer<vtkRenderer> ImageRenderer;
        vtkSmartPointer<vtkRenderer> PointCloudRenderer;
        vtkSmartPointer<vtkTextActor> TextActor;
        vtkSmartPointer<vtkAreaPicker> AreaPicker;
        vtkSmartPointer<AnnotationStyle> Style;
        vtkSmartPointer<vtkRenderWindow> RenderWindow;
        vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor;

};
#endif
