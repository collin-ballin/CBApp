//
//  my_demo.cpp
//  CBApp
//
//  Created by Collin Bond on 4/15/25.
//
//***************************************************************************//
//***************************************************************************//
#include "app/my_demo.h"
#include "imgui.h"



namespace cb {//     BEGINNING NAMESPACE "cb"...
//***************************************************************************//
//***************************************************************************//



//  "ShowCBDemoWindow"
//
void ShowCBDemoWindow(bool * p_open, ImGuiIO & io)
{
    static float    f               = 0.0f;
    static int      counter         = 0;
    ImVec4          clear_color     = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    
    ImGui::Begin("My Demo Window", p_open);                     //  Create a window called "Hello, world!" and append into it.


    ImGui::Text("Custom demo content here.");
    ImGui::Checkbox("Demo Window", p_open);                     //  Edit bools storing our window open/close state

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);                //  Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color);     //  Edit 3 floats representing a color

    if (ImGui::Button("Button"))                                //  Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);



    ImGui::End();
    
    
    return;
}



//***************************************************************************//
//
//
//
//***************************************************************************//
//***************************************************************************//
}//     END OF "cb" NAMESPACE.







//***************************************************************************//
//***************************************************************************//
//
//  END.
