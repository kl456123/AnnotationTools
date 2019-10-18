# AnnotationTool(c++ version)

## Introduction

## OverView

### Some kinds of files
* ConfigFile
    used to open project

* DataFile
    used to record labels

* CacheFile
    used to resume or save the tmp results

## Installation

* install vtk first
compile vtk from source and then make install to /usr/local/include/vtk

* compile project
```bash
cd ${DIR}
mkdir build
cmake ..
make -j8
# run it
./RubberBandPicker
```



## How to
### Some Keybindings
* "Tab", switch among 3d bbox
* "q", quit, note that it will save data before quit
* "h", horizontal view in pointcloud
* "v", bird eye view
* "c", clear current selection
* "i", display all annotation(2d box and 3d box)
* "r", switch between view mode and edit mode
* "Control", intersection select pointcloud, multiple view select and then intersection them
* "No Control", select from total points and add it
* "Left" and "Right", rotate highlight box(selected box) clockwise or unclockwise
* "Up" and "Down", go to the next frame
* "n", rotate arrow 90 degree, to determine which face is the front face

### Configuration
* rawdata path
* storage path
- root_dir
    - image_2
    - velodyne
    - calib
    - anno_2
* all annotation data will saved into anno_2 using txt format,
all filenames of annotation is according to the filenames of data.(the prefix name is the same)

### Demo Image
![](https://github.com/kl456123/AnnotationTools/blob/master/Images/Screenshot%20from%202019-10-14%2016-02-39.png)
![](https://github.com/kl456123/AnnotationTools/blob/master/Images/Screenshot%20from%202019-10-14%2016-02-58.png)
![](https://github.com/kl456123/AnnotationTools/blob/master/Images/Screenshot%20from%202019-10-14%2016-21-44.png)
![](https://github.com/kl456123/AnnotationTools/blob/master/Images/gifhome_512x512.gif)



## Development
Welcome to develop it and make it more robust


## TODO
* add GUI using QT
* add configuration file system using protobuf
* add plugin system



