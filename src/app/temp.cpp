//
//  testing.cpp
//  CBApp
//
//  Created by Collin Bond on 4/21/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "app/app.h"
#include "_config.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  1.  WINDOW FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Test_Basic_Widgets"
//
void App::Test_Basic_Widgets(void)
{
    //  1.  RADIOBUTTON TESTING...
    if (ImGui::TreeNode("Radiobuttons"))
    {
    
        bool        radiobutton_open    = true;
        static int  selection1          = 0;
        static int  selection2          = 0;
        static int  selection3          = 0;


        // Save starting cursor position
        ImVec2 cursor_start = ImGui::GetCursorScreenPos();
        float column_spacing = 120.0f; // Adjust for horizontal distance between columns

        // First column
        ImGui::BeginGroup();
        ImGui::RadioButton("Option A1", selection1 == 0); if (ImGui::IsItemClicked()) selection1 = 0;
        ImGui::RadioButton("Option B1", selection1 == 1); if (ImGui::IsItemClicked()) selection1 = 1;
        ImGui::RadioButton("Option C1", selection1 == 2); if (ImGui::IsItemClicked()) selection1 = 2;
        ImGui::EndGroup();


        // Second column - match top Y of first column, offset X
        ImGui::SameLine();
        ImGui::SetCursorScreenPos(ImVec2(cursor_start.x + column_spacing, cursor_start.y));
        ImGui::BeginGroup();
        ImGui::RadioButton("Option A2", selection2 == 0); if (ImGui::IsItemClicked()) selection2 = 0;
        ImGui::RadioButton("Option B2", selection2 == 1); if (ImGui::IsItemClicked()) selection2 = 1;
        ImGui::RadioButton("Option C2", selection2 == 2); if (ImGui::IsItemClicked()) selection2 = 2;
        ImGui::EndGroup();
        
        
        ImGui::SameLine();
        ImGui::SetCursorScreenPos(ImVec2(cursor_start.x + 2*column_spacing, cursor_start.y));
        ImGui::BeginGroup();
        ImGui::RadioButton("Option A3", selection3 == 0); if (ImGui::IsItemClicked()) selection3 = 0;
        ImGui::RadioButton("Option B3", selection3 == 1); if (ImGui::IsItemClicked()) selection3 = 1;
        ImGui::RadioButton("Option C3", selection3 == 2); if (ImGui::IsItemClicked()) selection3 = 2;
        ImGui::EndGroup();



        ImGui::SeparatorText("Various");
        {
            static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
            ImGui::PushItemWidth(80);
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
            static int item = -1;
            ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); ImGui::SameLine();
            ImGui::SliderFloat("X", &f0, 0.0f, 5.0f); ImGui::SameLine();
            ImGui::SliderFloat("Y", &f1, 0.0f, 5.0f); ImGui::SameLine();
            ImGui::SliderFloat("Z", &f2, 0.0f, 5.0f);
            ImGui::PopItemWidth();

            ImGui::PushItemWidth(80);
            ImGui::Text("Lists:");
            static int selection[4] = { 0, 1, 2, 3 };
            for (int i = 0; i < 4; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
                ImGui::PopID();
                //ImGui::SetItemTooltip("ListBox %d hovered", i);
            }
            ImGui::PopItemWidth();
        }
        ImGui::TreePop();
    }
    


    //  2.  BASIC WIDGETS...
    if (ImGui::TreeNode("Basic Widgets"))
    {
        static int e = 0;
        ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);
        
        
        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
        // See 'Demo->Layout->Text Baseline Alignment' for details.
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Hold to repeat:");
        ImGui::SameLine();

        // Arrow buttons with Repeater
        //IMGUI_DEMO_MARKER("Widgets/Basic/Buttons (Repeating)");
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopItemFlag();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

        ImGui::Button("Tooltip");
        ImGui::SetItemTooltip("I am a tooltip");

        ImGui::LabelText("label", "Value");



        ImGui::SeparatorText("Inputs");
        {
            // To wire InputText() with std::string or any other custom string type,
            // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/imgui_stdlib.h file.
            //IMGUI_DEMO_MARKER("Widgets/Basic/InputText");
            static char str0[128] = "Hello, world!";
            ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));


            static char str1[128] = "";
            ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            //GUI_DEMO_MARKER("Widgets/Basic/InputInt, InputFloat");
            static int i0 = 123;
            ImGui::InputInt("input int", &i0);

            static float f0 = 0.001f;
            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            //ImGui::SameLine(); HelpMarker(
            //    "You can input value using the scientific notation,\n"
            //    "  e.g. \"1e+8\" becomes \"100000000\".");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("input float3", vec4a);
        }
        
        ImGui::TreePop();
    }



    return;
}


//  "ImGui_Demo_Code"
//
void App::ImGui_Demo_Code(void)
{
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    
    
    //IMGUI_DEMO_MARKER("Widgets/Basic");
    if (ImGui::TreeNode("Basic"))
    {
        ImGui::SeparatorText("General");

        //IMGUI_DEMO_MARKER("Widgets/Basic/Button");
        static int clicked = 0;
        if (ImGui::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            ImGui::SameLine();
            ImGui::Text("Thanks for clicking me!");
        }

        //IMGUI_DEMO_MARKER("Widgets/Basic/Checkbox");
        static bool check = true;
        ImGui::Checkbox("checkbox", &check);

        //IMGUI_DEMO_MARKER("Widgets/Basic/RadioButton");
        static int e = 0;
        ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        //IMGUI_DEMO_MARKER("Widgets/Basic/Buttons (Colored)");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            ImGui::Button("Click");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
        // See 'Demo->Layout->Text Baseline Alignment' for details.
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Hold to repeat:");
        ImGui::SameLine();

        // Arrow buttons with Repeater
        //IMGUI_DEMO_MARKER("Widgets/Basic/Buttons (Repeating)");
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopItemFlag();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

        ImGui::Button("Tooltip");
        ImGui::SetItemTooltip("I am a tooltip");

        ImGui::LabelText("label", "Value");



        ImGui::SeparatorText("Inputs");
        {
            // To wire InputText() with std::string or any other custom string type,
            // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/imgui_stdlib.h file.
            //IMGUI_DEMO_MARKER("Widgets/Basic/InputText");
            static char str0[128] = "Hello, world!";
            ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));


            static char str1[128] = "";
            ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            //GUI_DEMO_MARKER("Widgets/Basic/InputInt, InputFloat");
            static int i0 = 123;
            ImGui::InputInt("input int", &i0);

            static float f0 = 0.001f;
            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            //ImGui::SameLine(); HelpMarker(
            //    "You can input value using the scientific notation,\n"
            //    "  e.g. \"1e+8\" becomes \"100000000\".");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("input float3", vec4a);
        }


        ImGui::SeparatorText("Drags");
        {
            //IMGUI_DEMO_MARKER("Widgets/Basic/DragInt, DragFloat");
            static int i1 = 50, i2 = 42, i3 = 128;
            ImGui::DragInt("drag int", &i1, 1);
            //ImGui::SameLine(); HelpMarker(
            //    "Click and drag to edit value.\n"
            //    "Hold SHIFT/ALT for faster/slower edit.\n"
            //    "Double-click or CTRL+click to input value.");
            ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragInt("drag int wrap 100..200", &i3, 1, 100, 200, "%d", ImGuiSliderFlags_WrapAround);

            static float f1 = 1.00f, f2 = 0.0067f;
            ImGui::DragFloat("drag float", &f1, 0.005f);
            ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
            //ImGui::DragFloat("drag wrap -1..1", &f3, 0.005f, -1.0f, 1.0f, NULL, ImGuiSliderFlags_WrapAround);
        }


        ImGui::SeparatorText("Sliders");
        {
            //IMGUI_DEMO_MARKER("Widgets/Basic/SliderInt, SliderFloat");
            static int i1 = 0;
            ImGui::SliderInt("slider int", &i1, -1, 3);
            //ImGui::SameLine(); HelpMarker("CTRL+click to input value.");

            static float f1 = 0.123f, f2 = 0.0f;
            ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            ImGui::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);

            //IMGUI_DEMO_MARKER("Widgets/Basic/SliderAngle");
            static float angle = 0.0f;
            ImGui::SliderAngle("slider angle", &angle);

            // Using the format string to display a name instead of an integer.
            // Here we completely omit '%d' from the format string, so it'll only display a name.
            // This technique can also be used with DragInt().
            //IMGUI_DEMO_MARKER("Widgets/Basic/Slider (enum)");
            enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
            static int elem = Element_Fire;
            const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
            const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
            ImGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name); // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
            //ImGui::SameLine(); HelpMarker("Using the format string parameter to display a name instead of the underlying integer.");
        }


        ImGui::SeparatorText("Selectors/Pickers");
        {
            //IMGUI_DEMO_MARKER("Widgets/Basic/ColorEdit3, ColorEdit4");
            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            ImGui::ColorEdit3("color 1", col1);

            ImGui::ColorEdit4("color 2", col2);
        }


        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more flexible BeginCombo()/EndCombo() api.
            //IMGUI_DEMO_MARKER("Widgets/Basic/Combo");
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
            static int item_current = 0;
            ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            //ImGui::SameLine(); HelpMarker(
            //    "Using the simplified one-liner Combo API here.\n"
            //     "Refer to the \"Combo\" section below for an explanation of how to use the more flexible and general BeginCombo/EndCombo API.");
        }


        {
            // Using the _simplified_ one-liner ListBox() api here
            // See "List boxes" section for examples of how to use the more flexible BeginListBox()/EndListBox() api.
            //IMGUI_DEMO_MARKER("Widgets/Basic/ListBox");
            const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int item_current = 1;
            ImGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 4);
            //ImGui::SameLine(); HelpMarker(
            //    "Using the simplified one-liner ListBox API here.\n"
            //    "Refer to the \"List boxes\" section below for an explanation of how to use the more flexible and general BeginListBox/EndListBox API.");
        }

        // Testing ImGuiOnceUponAFrame helper.
        //static ImGuiOnceUponAFrame once;
        //for (int i = 0; i < 5; i++)
        //    if (once)
        //        ImGui::Text("This will be displayed only once.");

        ImGui::TreePop();
    
    }


    return;
}




// *************************************************************************** //
//
//
//  DEAR IMGUI DEMO APPLICATIONS...
// *************************************************************************** //
// *************************************************************************** //


//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();

        if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (clear)
                Clear();
            if (copy)
                ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char* buf = Buf.begin();
            const char* buf_end = Buf.end();
            if (Filter.IsActive())
            {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have random access to the result of our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of
                // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    if (Filter.PassFilter(line_start, line_end))
                        ImGui::TextUnformatted(line_start, line_end);
                }
            }
            else
            {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
                // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
                // within the visible area.
                // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
                // on your side is recommended. Using ImGuiListClipper requires
                // - A) random access into your data
                // - B) items all being the  same height,
                // both of which we can handle since we have an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to display
                // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
                // it possible (and would be recommended if you want to search through tens of thousands of entries).
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char* line_start = buf + LineOffsets[line_no];
                        const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        ImGui::TextUnformatted(line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::End();
    }
};

// Demonstrate creating a simple log window with basic filtering.
void ShowExampleAppLog(bool* p_open)
{
    static ExampleAppLog log;

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Example: Log", p_open);
    
    if (ImGui::SmallButton("[Debug] Add 5 entries"))
    {
        static int counter = 0;
        const char* categories[3] = { "info", "warn", "error" };
        const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
        for (int n = 0; n < 5; n++)
        {
            const char* category = categories[counter % IM_ARRAYSIZE(categories)];
            const char* word = words[counter % IM_ARRAYSIZE(words)];
            log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
                ImGui::GetFrameCount(), category, ImGui::GetTime(), word);
            counter++;
        }
    }
    ImGui::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Example: Log", p_open);
}






//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear ImGui!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            ImGui::MemFree(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            ImGui::MemFree(Items[i]);
        Items.clear();
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void    Draw(const char* title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Debug Text"))  { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear"))           { ClearLog(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear")) ClearLog();
                ImGui::EndPopup();
            }

            // Display every line as a separate entry so we can change their color or add custom widgets.
            // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
            // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
            // to only process visible items. The clipper will automatically measure the height of your first item and then
            // "seek" to display only items in the visible area.
            // To use the clipper we can replace your standard loop:
            //      for (int i = 0; i < Items.Size; i++)
            //   With:
            //      ImGuiListClipper clipper;
            //      clipper.Begin(Items.Size);
            //      while (clipper.Step())
            //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            // - That your items are evenly spaced (same height)
            // - That you have cheap random access to your elements (you can access them given their index,
            //   without processing all the ones before)
            // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
            // We would need random-access on the post-filtered list.
            // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
            // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
            // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
            // to improve this example code!
            // If your items are of variable height:
            // - Split them into same height items would be simpler and facilitate random-seeking into your list.
            // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            if (copy_to_clipboard)
                ImGui::LogToClipboard();
            for (const char* item : Items)
            {
                if (!Filter.PassFilter(item))
                    continue;

                // Normally you would store more information in your item than just a string.
                // (e.g. make Items[] an array of structure, store color/type etc.)
                ImVec4 color;
                bool has_color = false;
                if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
                else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                if (has_color)
                    ImGui::PopStyleColor();
            }
            if (copy_to_clipboard)
                ImGui::LogFinish();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                ImGui::MemFree(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for (int i = 0; i < Commands.Size; i++)
                    if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                        candidates.push_back(Commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can..
                    // So inputting "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c == 0 || c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    AddLog("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        AddLog("- %s\n", candidates[i]);
                }

                break;
            }
        case ImGuiInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == ImGuiKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == ImGuiKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }
};

void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.



















/*
void Demo_RealtimePlots() {
    ImGui::BulletText("Move your mouse to change the data!");
    ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
    static ScrollingBuffer sdata1, sdata2;
    static RollingBuffer   rdata1, rdata2;
    ImVec2 mouse = ImGui::GetMousePos();
    static float t = 0;
    t += ImGui::GetIO().DeltaTime;
    sdata1.AddPoint(t, mouse.x * 0.0005f);
    rdata1.AddPoint(t, mouse.x * 0.0005f);
    sdata2.AddPoint(t, mouse.y * 0.0005f);
    rdata2.AddPoint(t, mouse.y * 0.0005f);

    static float history = 10.0f;
    ImGui::SliderFloat("History",&history,1,30,"%.1f s");
    rdata1.Span = history;
    rdata2.Span = history;

    static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

    if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1,150))) {
        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1,t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL,0.5f);
        ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, 0, sdata1.Offset, 2 * sizeof(float));
        ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2*sizeof(float));
        ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("##Rolling", ImVec2(-1,150))) {
        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1,0,history, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
        ImPlot::PlotLine("Mouse X", &rdata1.Data[0].x, &rdata1.Data[0].y, rdata1.Data.size(), 0, 0, 2 * sizeof(float));
        ImPlot::PlotLine("Mouse Y", &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(), 0, 0, 2 * sizeof(float));
        ImPlot::EndPlot();
    }
}
*/




/*
void Demo_Heatmaps() {
    static float values1[7][7]  = {{0.8f, 2.4f, 2.5f, 3.9f, 0.0f, 4.0f, 0.0f},
                                    {2.4f, 0.0f, 4.0f, 1.0f, 2.7f, 0.0f, 0.0f},
                                    {1.1f, 2.4f, 0.8f, 4.3f, 1.9f, 4.4f, 0.0f},
                                    {0.6f, 0.0f, 0.3f, 0.0f, 3.1f, 0.0f, 0.0f},
                                    {0.7f, 1.7f, 0.6f, 2.6f, 2.2f, 6.2f, 0.0f},
                                    {1.3f, 1.2f, 0.0f, 0.0f, 0.0f, 3.2f, 5.1f},
                                    {0.1f, 2.0f, 0.0f, 1.4f, 0.0f, 1.9f, 6.3f}};
    static float scale_min       = 0;
    static float scale_max       = 6.3f;
    static const char* xlabels[] = {"C1","C2","C3","C4","C5","C6","C7"};
    static const char* ylabels[] = {"R1","R2","R3","R4","R5","R6","R7"};

    static ImPlotColormap map = ImPlotColormap_Viridis;
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map),ImVec2(225,0),map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        // We bust the color cache of our plots so that item colors will
        // resample the new colormap in the event that they have already
        // been created. See documentation in implot.h.
        BustColorCache("##Heatmap1");
        BustColorCache("##Heatmap2");
    }

    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImGui::SetNextItemWidth(225);
    ImGui::DragFloatRange2("Min / Max",&scale_min, &scale_max, 0.01f, -20, 20);

    static ImPlotHeatmapFlags hm_flags = 0;

    ImGui::CheckboxFlags("Column Major", (unsigned int*)&hm_flags, ImPlotHeatmapFlags_ColMajor);

    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1",ImVec2(225,225),ImPlotFlags_NoLegend|ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::SetupAxisTicks(ImAxis_X1,0 + 1.0/14.0, 1 - 1.0/14.0, 7, xlabels);
        ImPlot::SetupAxisTicks(ImAxis_Y1,1 - 1.0/14.0, 0 + 1.0/14.0, 7, ylabels);
        ImPlot::PlotHeatmap("heat",values1[0],7,7,scale_min,scale_max,"%g",ImPlotPoint(0,0),ImPlotPoint(1,1),hm_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale",scale_min, scale_max, ImVec2(60,225));

    ImGui::SameLine();

    const int size = 80;
    static double values2[size*size];
    srand((unsigned int)(ImGui::GetTime()*1000000));
    for (int i = 0; i < size*size; ++i)
        values2[i] = RandomRange(0.0,1.0);

    if (ImPlot::BeginPlot("##Heatmap2",ImVec2(225,225))) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(-1,1,-1,1);
        ImPlot::PlotHeatmap("heat1",values2,size,size,0,1,nullptr);
        ImPlot::PlotHeatmap("heat2",values2,size,size,0,1,nullptr, ImPlotPoint(-1,-1), ImPlotPoint(0,0));
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();

}
*/



