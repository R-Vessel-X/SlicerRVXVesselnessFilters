# RVesselX Slicer Vesselness Filters plugin

<div style="text-align:center">
<img class="center" src="https://github.com/R-Vessel-X/SlicerRVXVesselnessFilters/raw/main/Screenshots/RVX_vesselness_example.png"/>
</div>

This repository contains several vesselness filters tested in the context of liver vessel segmentation.

The original implementations come from : 
* [https://github.com/JonasLamy/LiverVesselness](https://github.com/JonasLamy/LiverVesselness)
* [https://github.com/path-openings/RORPO](https://github.com/path-openings/RORPO)

An installation-free online demonstration is also available
here: http://ipol-geometry.loria.fr/~kerautre/ipol_demo/LiverVesselnessIPOLDemo

A benchmark comparison of the different filters and details regarding their equations can be found
here : https://hal.science/hal-02544493/document

## Table of contents

* [Introduction](#Introduction)
* [How to cite](#How-to-cite)
* [Using the Plugin](#Using-the-Plugin)
    * [Installing the plugin](#Installing-the-plugin)
    * [Tutorials](#Tutorials)
    * [Plugin Overview](#Plugin-Overview)
* [Contributing](#Contributing)

## Introduction

<div style="text-align:center">
<img class="center" src="https://github.com/R-Vessel-X/SlicerRVXVesselnessFilters/raw/main/Screenshots/RVXLogo.png"/>
</div>

The RVesselX slicer vesselness plugin is an extension providing several vesselness filters as command line interface extensions.
The filters take as input a volume and generate a new volume with the processed vesselness.

For more information on the R-Vessel-X project, please visit :  
https://anr.fr/Projet-ANR-18-CE45-0018

## How to cite

Please cite the reference of this work if you use any of it :
```
@ARTICLE{9833530,
  author={Lamy, Jonas and Merveille, Odyssée and Kerautret, Bertrand and Passat, Nicolas},
  journal={IEEE Transactions on Medical Imaging}, 
  title={A Benchmark Framework for Multiregion Analysis of Vesselness Filters}, 
  year={2022},
  volume={41},
  number={12},
  pages={3649-3662},
  doi={10.1109/TMI.2022.3192679}}
```

## Using the Plugin

### Installing the plugin

The plugin can be installed in Slicer3D using
the [extension manager](https://slicer.readthedocs.io/en/latest/user_guide/extensions_manager.html#install-extensions).
It can be found using the search bar by typing "RVesselX".

After the installation is complete, the different filters will be available from the search bar by typing in their
names or by navigating to the `Filtering > Vesselness` category.

### Tutorials

1. In the `Sample Data` module, load the `CTLiver` volume
2. Switch to the `Frangi Vesselness Filter` module located in `Filtering > Vesselness`
2. In the IO section, select the `CTLiver` in the `Input Volume` drop down menu
3. In the IO section, select `Create New Volume` in the `Output Volume` drop down menu
4. Click On the `Apply` button
5. After processing the following volume will be displayed in the UI

<div style="text-align:center">
<img class="center" src="https://github.com/R-Vessel-X/SlicerRVXVesselnessFilters/raw/main/Screenshots/RVX_vesselness_tutorial_01.png"/>
</div>

### Plugin Overview

This repository provides the following, hessian-based and morphological vesselness filters implementations :

- Antiga Vesselness Filter : A. F. Frangi, W. J. Niessen, K. L. Vincken, and M. A. Viergever, “Multiscale vessel
  enhancement filtering,” in MICCAI, 1998, pp. 130–137.
- OOF Vesselness Filter : M. W. K. Law and A. C. S. Chung, “Three dimensional curvilinear structure detection using
  optimally oriented flux,” in ECCV, 2008, pp. 368–382.
- Jerman Vesselness Filter : T. Jerman, F. Pernus, B. Likar, and Z. Spiclin, “Enhancement of vascular structures in 3D
  and 2D angiographic images,” IEEE T Med Imaging, vol. 35, pp. 2107–2118, 2016.
- Meijering Vesselness Filter : E. Meijering, M. Jacob, J.-C. Sarria, P. Steiner, H. Hirling, and M. Unser, “Neurite
  tracing in fluorescence microscopy images using ridge filtering and graph searching: Principles and validation,” in
  ISBI, 2004, pp. 1219–1222.
- Sato Vesselness Filter : Y. Sato, S. Nakajima, H. Atsumi, T. Koller, G. Gerig, S. Yoshida, and R. Kikinis, “3D
  multi-scale line filter for segmentation and visualization of curvilinear structures in medical images,” in
  CVRMed-MRCAS, 1997, pp. 213–222.
- Zhang Vesselness Filter : D. Li, L. Zhang, C. Sun, T. Yin, C. Liu, and J. Yang, “Robust retinal image enhancement via
  dual-tree complex wavelet transform and morphology-based method,” IEEE Access, vol. 7, pp. 47 303–47 316, 2019.
- RORPO Vesselness Filter : O. M ERVEILLE , H. T ALBOT , L. N AJMAN et N. P ASSAT . “Curvilinear structure analysis by
ranking the orientation responses of path operators”. In : IEEE Transactions in Pattern
Analysis 40 (2018), p. 304-317 (cf. p. 57, 68)

#### Common parameters

Hessian filters share a common configuration as well as their Input/Output definition.
RORPO has a different configuration, however it shares Input/Output definition with hessian based filters.

##### Scale space parameters

The scale space parameters define the parameters for the Hessian matrix processing.
They influence the min / max scale of the vessels which will be detected in the input volumes.

* min / max Sigma : Sets the Hessian gaussian filter min / max size in voxels (multiply by the image resolution to get the size in mm).
* Scales : Number of scales between mininum sigma and max sigma (higher number involves longer processing time)

RORPO's algorithm is parametrised only by scales parameters.
They define the size of the morphological paths corresponding to the min size of the vessels which will be detected in the input volume.

* min Scale : Set the size of the first path
* Factor : Growth factor of the path for the next scale
* Scales : Number of scales tested by the algorithm.
* Dilation : make the algorithm more robust to noise. This parameter is embeded into the scale computation. 

* (optionnal) core : Number of CPU core (max 7), used by RORPO. Single core processing is slow.

##### IO

* Input Volume : Source volume on which to process the Vesselness. The volume should ideally have isotropic spacing.
* Mask volume : (Optional) Binary volume to limit the area on which the Vesselness is processed
* Output Volume : Output volume containing the Vesselness values

# Contributing

This project welcomes contributions. If you want more information about how you can contribute, please refer to
the [CONTRIBUTING.md file](CONTRIBUTING.md).

