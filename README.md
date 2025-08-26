# *[**\*CBApp:**]()* *a cross-platform desktop application for computational electrodynamic modeling with C++*



<details open>
  <summary>Note:</summary>

  > *CBApp is just a generic, placeholder name I've given to this project for the purpose of having an identifier in the build system.*
</details>



[![CBAppSlideshow](docs/CBApp/gifs/slideshow-0.gif)](https://github.com/collin-ballin/CBApp/releases)






---


[![Last commit](https://img.shields.io/github/last-commit/collin-ballin/CBApp.svg)](https://github.com/collin-ballin/CBApp/commits/main)
\
![Windows](https://img.shields.io/badge/Windows-supported-green)
![macOS](https://img.shields.io/badge/macOS-supported-green)
\
![ImGui Docking](https://img.shields.io/badge/imgui-docking-green)
![GLFW](https://img.shields.io/badge/GLFW-3.3-success)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3+-success)
\
![C++20](https://img.shields.io/badge/C%2B%2B-20%2B-blue)
![Apple Clang](https://img.shields.io/badge/Compiler-Apple%20Clang%20%3E%3D16.0.2-blue)
![GNU GCC](https://img.shields.io/badge/Compiler-GNU%20GCC%20%3E%3D15.1.0-blue)
![CMake](https://img.shields.io/badge/cmake-%3E%3D3.16-blue)
![MinGW-w64 x86_64 Toolchain](https://img.shields.io/badge/toolchain-x86__64--w64--mingw32-blue)



---

---

- [1. SYNOPSIS](#1-synopsis)
- [2. PURPOSE & DESIGN](#2-purpose--design)
- [3. INTERFACE DESIGN](#3-interface-design)
  - [User Interface](#user-interface)
    - [Overview](#overview)
  - [Theoretical Aims](#theoretical-aims)
  - [Implementation Aims](#implementation-aims)
- [4. APPLICATION FEATURES](#4-application-features)
  - [4A. Main Applications](#4A-main-applications)
    - [4A.1 The `Editor` Application](#4A.1-the-editor-application)
    - [4A.2 The `Mimic` Application](#4A.2-the-mimic-application)
    - [4A.3 The `Graph` Application](#4A.3-the-graph-application)
  - [4B. Secondary Applications](#4B-secondary-applications)
    - [4B.1 The `CCounter` Application](#4B.1-the-ccounter-application)
    - [4B.2 `SPR Calculator` Application](#4B.2-spr-calculator-application)
  - [4C. Additional Tools and Utilities](#4C-additional-tools-and-utilities)
    - [4C.1 Color Tool](#4C.1-color-tool)
    - [4C.2 ImGui Tools](#4C.2-imgui-tools)
- [5. IMPLEMENTATION DETAILS](#5-implementation-details)
  - [Build System](#build-system)
    - [macOS (Xcode)](#macos-xcode)
    - [Windows (VSCode)](#windows-vscode)
- [6. CURRENT PROJECT STATUS](#6-current-project-status)
- [7. ADDITIONAL INFORMATION](#7-additional-information)
  - [Author](#author)
  - [Acknowledgements](#acknowledgements)
  - [License](#license)
  - [Legacy & Archive](#legacy--archive)









## 1. SYNOPSIS


### [**This project seeks to develop a cross-platform, GUI-based desktop application for performing computational physics simulations.  Our project is built using modern C++ (cpp-20+), the *docking branch* of `Dear ImGui`, `OpenGL`, and `GLFW`.**]()


<p align="center">
  <img src="docs/CBApp/gifs/fdtd-1d-1.gif"
       alt="fdtd-video-1"
       width="97.5%" />
  <br>
  <em>Video 1. 1D FDTD Simulation; work was taken from an FDTD engine I developed for a recent paper and migrated into the CBApp interface.</em>
</p>


### Our application is built using `CMake`; `minGW` is used as a cross-platform toolchain to build the project for Windows.  The set of third-party dependencies is kept to a minimum to maximize the compatibility our application for use across a variety of different machines that may/may-not have access to certain installations.  Currently, these dependencies are limited to the `ImPlot` extension for Dear ImGui and `nlohmann`'s JSON for modern C++.


### The scope of this project is primarily focused on simulations of electrodynamic systems and specifically those using using the Finite-Difference Time-Domain method (FDTD). 








---
---

## 2. PURPOSE & OBJECTIVES

*Fundamentally, our application must facilitate each of the following tasks:*
- (1) Create and design an electrodynamic system along with the relevant parameters for each item within,
- (2) Perform a simulation of the conditions we specified within the system we created, 
- (3) Present a visualization of the simulation results and allow the user to interact with and analyze the data. 






---

---

## 3. INTERFACE DESIGN

*In this pursuit, our design has been structured in a manner as to adhere to the following goals:*


### ***USER INTERFACE.***
>
> Implementing a highly funcitonal user-interface is often an extremely time consuming and potentially frustrating task.  What's more---this is further pronounced when maintaining, refactoring, or extending an existing UI that is poorly thought-out.  At the penalty of increased complexity and initial development time, our goal has been to establish a robust and extensible UI that is accomodating to change, additions, or extensions.
>
>
>
>  #### UI Overview:
> >
> > *The overal layout of the UI design is inspired by the desigon of the Ableton Live application:*
> >   - **Menu Bar:**         The bar at the very top of the application containing the "File", "Window", "Help", etc, drop-down menus.
> >   - **Control Bar:**      The 'banner-style' panel that is found below the Menu Bar and spans the entire width of the main window. 
> >   - **Browser:**      The collapsible window pane found on the left hand side of the main window and spanning its entire height.
> >   - **Main Section:**     The primary window pane located beneath the Control Bar and to the right of the Browser. 
> >   - **Detail View:**      A second collapsible window pane located below the Main Section. 
> >
> >
> 
>
>
> > #### UI Features:
> >
> >  - Taking advantage of the `viewports` and `docking` features of Dear ImGui---the user is able to rearrange and customize the appearance and window layout of each part of the main application to suit their needs and preferences.  
> > 
> > 
> > - Each window of the application can be undocked and moved to a different docing location or even set aside to serve as an entirely seperate, stand-alone window. 
> >
> > 
> > - The colors, geometry, and fonts of the application can be customized, saved/loaded to an external file, overriden as the default settings of the app, and so forth.  
> >
> >
> >
> 
>
>
> #### Delegator Classes:
> > - Each major responsibility of the application is allocated to a `delegator` class in order to prevent the central `App` class from becoming bloated and cumbersome and, instead, delegating away certain responsibilities, as the name suggests.
> >
> > - This is a modular system that allows for additional windows, tools, and other features to be easily integrated into the core application services.
>
>






---

---

## 4. APPLICATION FEATURES

<p align="center">
  <img src="docs/CBApp/images/tools.png"
       alt="tools"
       width="25.0%" />
  <br>
  <em>Figure 1. Menubar list displaying each tool that is available in the application.</em>
</p>



### ***THE [EDITOR]() APPLICATION***
>
> The `Editor` Application is designed to be a CAD or Vector-Artwork style tool that will be used to create the system that will ultimately be simulated by the FDTD engine (see "`Graph` Application" below).
>
>  #### Object Creation:
> > This tool provides a host of different tools to create objects and customize their geometry in a manner following that of a classic, solid-body modeling program.  This includes the ability to create exotic curvature in each object with a Bezier curve editing tool whose design was heavily inspired by that of the "Pen Tool" in Adobe Illustrator.  
>
>  #### Payloads:
> > Each object carries a `Payload`---which is used to carry the data that categorizes what the role of this object is within the FDTD simulation.  That is---by default, each object is simply an element of pure geometry that has no significance to the simulation on its own.  By assigning the `Payloads` for each object---we create the additional information that is needed to define each element of the system.  The host of different `Payloads` includes things like the following: 
> > - `Sources:` 
> >   - E or B sources such as Gaussian Pulse, Time-Harmonic, Ricker Wavelets, Monochromatic Plane Waves, and so forth, which are responsible for stimulating the field of the simulation.
> >   - Each source may have a variety of additional properties depending on its particular type.  This includes, but is not limitied to, its frequency, the length of time it will be active for, its amplitude or intensity, whether or not it is a Hard or Soft source, descriptions of its dispersion or coherence properties, and so on.
> > - `Materials:`  
> >   - This describes the various media that are contained within the simulated domain, such as dielectrics, conductors, and so on. 
> > - `Boundary Conditions:` 
> >   - of the simulation (such as a Murr ABC, TF/SF, PML, etc)
> > - `Monitors and Measurements:` 
> >   - Monitors are used to indicate certain points or regions of interest within the simulation.  Placing a monitor at a certain region allows us to explicitly dictate what specific quantities of data we wish to record from the simulation.



<mark>critical</mark>
![Status](https://img.shields.io/badge/FDTD-ready-brightgreen)

<details><summary>Show rationale</summary>
Short justification goes here.
</details>



### ***THE [MIMIC]() APPLICATION***
>
> Text
>
>  #### Sub-Item 1:
> > Some more text...



### ***THE [GRAPH]() APPLICATION (THIS WILL LATER BE RENAMED...)***
>
> This application is the actual FDTD Engine that will compute the simulation of the system that was designed with the `Editor` application.  Note that this name is just temporarly and will evelntually be replaced with a more-appropriate name once development proceeds further.  
>
>  #### Sub-Item 1:
> > Some more text...



### ***THE [CCOUNTER]() APPLICATION***
>
> Designed to facilitate our work in the laboratory on Spontaneous Parametric Down-Conversion (SPDC) due to a pump laser that is incident upon a Birifringent, Anisotropic BBO crystal.
>
>  #### The Coincidence Counter:
> > The Coincidence Counter device in our lab is responsible for detecting and counting single-photons which are produced from the SPDC process.  The way this is achieved is via a series of Avalanche Photodiode Detectors (APD) which are precicely placed at a positions derived by the Frensel elipse of the system.  The signal of each APD is monitored by a National Instruments Field Programmable Gate Array (FPGA) device.  The CCounter application runs a Python script inside a child process to interface with the FPGA using NI's Python API.  Hence---the ImGui application is responsible for fetching the data that is returned from the FPGA, displaying it in real-time, and allowing the user to perform data analysis and/or modifications to the FPGA.



### ***ADDITIONAL [TOOLS]() AND UTILITIES***
>
> *A number of smaller utility tools have been created in order to solve certain problems that have arisen over the development of this project.  These tools remain accessible to the end user in the event that they should perhaps ever find them to be useful.*
>
>
>
>  #### The Color Tool:
> > Some more text...
>
>
>
>  #### Dear ImGui Tools
> > Each of the tools provided by the Dear ImGui demos can be accessed from the "Window > Show" menu in the Menu Bar.  This includes the Dear ImGui Demo and Style Editor, the ImPlot Demo and Style Editor, and etc.  Using these tools, the user can customize the appearance of the paplication to their requirements and subequently save these settings for future useage. 






---


This project wraps ImGui and OpenGL into a custom `App` class, bootstraps multi-window support via GLFW, and renders dynamically using a real-time frame loop.

The app layout, fonts, and color scheme are JSON-configured via files in `assets/`.










<p align="center">
  <img src="docs/CBApp/heatmap_2.png"
       alt="Dark-mode dashboard"
       width="45%" />
  <img src="docs/CBApp/heatmap_2.png"
       alt="Light-mode dashboard"
       width="45%" />
</p>

<em style="display:block; text-align:center;">
  <em>Figure 3. [Placeholder Images].</em>
</em>






---

---

## 5. IMPLEMENTATION DETAILS

>
> ### *3.1.  Build System (CMake)*
> > - Some information here.
> >
> > - Even more information over here.
> >
>
>
> 



>
> ### *5.2 macOS (Xcode)*
> > 
> > ```bash
> > #   OUT-OF-DATE   ---   I NEED TO UPDATE THIS INFORMATION (July 25, 2025)...
> > 
> > git clone https://github.com/your-username/CBApp.git
> > cd CBApp && mkdir build && cd build
> > cmake -G "Xcode" ..             # or use "Unix Makefiles"
> > cmake --build .
> > ./CBApp                         # launch the app
> > ```
>
>
>



>
> ### *5.3 Windows (Visual Studio)*
> > 
> > ```powershell
> > #   OUT-OF-DATE   ---   I NEED TO UPDATE THIS INFORMATION (July 25, 2025)...
> >
> > git clone https://github.com/your-username/CBApp.git
> > cd CBApp
> > cmake -G "Visual Studio 17 2022" .
> > cmake --build . --config Release
> > .\Release\CBApp.exe
> > ```
>
>
>



>
> ### *5.4 Project Directory Structure*
> > 
> > ```text
> > #   OUT-OF-DATE   ---   I NEED TO UPDATE THIS INFORMATION (July 25, 2025)...
> >
> > CBApp/
> >   ├── src/                      # *MY* Source Code...
> >   ├── include/                  # *MY* Header Files...
> >   ├── imgui/                    # Source Code for ImGui, ImPlot, and Backends...
> >   ├
> >   ├── assets/                   # User Data, ".ini" and ".json" for ImGui, TTF Font FIles, etc... 
> >   ├── libs/                     # Third-party libs (GLFW, JSON, cblib)
> >   ├── docs/                     # Markdown + change logs
> > ```






---
---

## 6. CURRENT PROJECT STATUS

<p align="center">
  <img src="docs/CBApp/gifs/slideshow_1.gif"
       alt="slideshow-2"
       width="97.5%" />
  <br>
  <em>SLIDESHOW 2.  Assorted images taken across various points within the project's development.</em>
</p>









---

---

## 7. ADDITIONAL INFORMATION



### Author
> - **Collin A. Bond**
>   - A Recent [*Conference Paper*](https://iopscience.iop.org/article/10.1088/1742-6596/3052/1/012002) on FDTD.
>   - Student of Physics and Computer Science.
>   - *Portland State University*.






### License
Distributed under the **MIT License**.  See `LICENSE.txt` for details.






### Acknowledgements
> A special thanks goes out to the following individuals:
>
> - Dr. Erik J. Sánchez
>   - Department of Physics, Portland State University.
> - Dr. Andres H. La Rosa
>   - Department of Physics, Portland State University.
> - Prof. John B. Schneider
>   - School of Electrical Engineering and Computer Science, Washington State University.
>   - His *outstanding* book, [*Understanding the Finite-Difference Time-Domain Method*](https://eecs.wsu.edu/~schneidj/ufdtd/index.php).
> - Dr. Jeroen Vleggaar
>   - The [*Huygens Optics*](https://www.youtube.com/@HuygensOptics) YouTube Channel.



























---
---

### MISC
and data analysis, and data visualization.  Our ultimate goal is for this application to serve in a similar capacity to the commercial products developed by companies such as Ansys Lumerical and others.





> ## This application is being designed with the intention to serve several purposes. 
> ## 1. First, this application will be used for real-time data analysis with a National Instruments FPGA-based coincidence counter that is used for the purpose of conducting experiments in quantum mechanics involving spontaneous parametric down-conversion via a BBO birifringent, isotropic crystal as well as single, individual photons and entanglement. 

The system is designed for data visualization and simulation—ideal for computational physics and real-time analysis tools.

> * Multi-viewport docking UI (Dear ImGui)
> * Real-time plotting (ImPlot)
> * Native performance (GLFW + OpenGL)
> * JSON-driven theming and font configuration
> * Modular C++ architecture with clean CMake/Xcode integration
