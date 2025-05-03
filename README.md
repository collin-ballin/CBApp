# *"CBApp" – Real-Time C++ Application Framework*

![License](https://img.shields.io/badge/license-MIT-green)
![C++17](https://img.shields.io/badge/C%2B%2B-17%2B-blue)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey)

---

### **CBApp is a cross-platform desktop framework built in C++17+ that uses** `Dear ImGui` **(docking branch),** `ImPlot`, **GLFW, and OpenGL**.

The system is designed for data visualization and simulation—ideal for computational physics and real-time analysis tools.

> * Multi-viewport docking UI (Dear ImGui)
> * Real-time plotting (ImPlot)
> * Native performance (GLFW + OpenGL)
> * JSON-driven theming and font configuration
> * Modular C++ architecture with clean CMake/Xcode integration

---

## 1. OVERVIEW

This project wraps ImGui and OpenGL into a custom `App` class, bootstraps multi-window support via GLFW, and renders dynamically using a real-time frame loop.

The app layout, fonts, and color scheme are JSON-configured via files in `assets/`.

<p align="center">
  <img src="assets/images/imgui_dock_example.png"
       alt="Docking layout example"
       width="92%" />
  <br>
  <em>Figure 1. Example docking layout with real-time plotting and sidebar UI.</em>
</p>

<p align="center">
  <img src="assets/images/implot_plotting.png"
       alt="ImPlot demo"
       width="46%" />
  <img src="assets/images/light_dark_toggle.png"
       alt="Appearance toggle"
       width="46%" />
</p>

<em style="display:block; text-align:center;">
  <strong>Figure 2.</strong> Plotting and style toggles using ImPlot and custom JSON themes.
</em>

---

## 2. BUILD INSTRUCTIONS

### 2.1 macOS / Linux

```bash
git clone https://github.com/your-username/CBApp.git
cd CBApp && mkdir build && cd build
cmake -G "Xcode" ..             # or use "Unix Makefiles"
cmake --build .
./CBApp                         # launch the app
```

### 2.2 Windows (Visual Studio)

```powershell
git clone https://github.com/your-username/CBApp.git
cd CBApp
cmake -G "Visual Studio 17 2022" .
cmake --build . --config Release
.\Release\CBApp.exe
```

---

## 3. STRUCTURE

```text
CBApp/
├── CMakeLists.txt           # CMake entry
├── src/                     # App core & render loop
├── include/                 # Public headers (app/, utility/)
├── imgui/                   # ImGui + ImPlot + backends
├── assets/                  # JSON config, TTF fonts
├── libs/                    # Third-party libs (GLFW, JSON, cblib)
├── docs/                    # Markdown + change logs
```

---

## 4. LICENSE

Distributed under the **MIT License**.
See `LICENSE.txt` for details.

---

## 5. AUTHOR

> **Collin Bond**
> Graduate Research – Computational Physics
> *Portland State University*
