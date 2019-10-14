# AnnotationTool

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
* "k"
* "s"
* "Tab", switch among 3d bbox
* "q", quit, note that it will save data before quit
* "h", horizontal view in pointcloud
* "v", bird eye view
* "c", clear current selection
* "i", display all annotation(2d box and 3d box)
* "r", switch between view mode and edit mode
* "Control", intersection select pointcloud, multiple view select and then intersection them

### Configuration
* rawdata path
* storage path
- root_dir
    - image_02
    - velodyne_points
    - calib
    - label_2
* all annotation data will saved into label_2 using txt format,
all filenames of annotation is according to the filenames of data.(the prefix name is the same)


## Development
Welcome to develop it and make it more robust


## TODO
* add GUI using QT
* add configuration file system using protobuf
* add plugin system



