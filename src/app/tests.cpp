/***********************************************************************************
*
*       *********************************************************************
*       ****              T E S T S . C P P  ____  F I L E               ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 20, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



namespace ft {  //     BEGINNING NAMESPACE "ft"...
// *************************************************************************** //
// *************************************************************************** //



// ──────────────────────────────────────────────────────────────
//  FunctionalTestRunner
//      • owns its own state
//      • called once per frame from the testing window
// ──────────────────────────────────────────────────────────────
struct FunctionalTestRunner {
    enum class Phase { Idle, Move, Down, Up };
    Phase       phase      = Phase::Idle;
    ImVec2      start_pos  {}, target_pos {};
    float       duration   = 0.0f, elapsed = 0.0f;
    GLFWwindow* window     = nullptr;

    // start a new script -------------------------------------------------------
    void start(GLFWwindow* win, ImVec2 target, float seconds)
    {
        window     = win;
        target_pos = target;
        duration   = std::max(0.001f, seconds);
        elapsed    = 0.0f;

        // **real HW position**
        double cx, cy;
        glfwGetCursorPos(window, &cx, &cy);
        start_pos = ImVec2(static_cast<float>(cx), static_cast<float>(cy));

        // sync ImGui so both views agree
        ImGui::GetIO().AddMousePosEvent(start_pos.x, start_pos.y);

        phase = Phase::Move;
    }

    // abort immediately --------------------------------------------------------
    void abort()
    {
        phase   = Phase::Idle;
        elapsed = 0.0f;
        // also resync ImGui with wherever the OS cursor sits now
        if (window) {
            double cx, cy;  glfwGetCursorPos(window, &cx, &cy);
            ImGui::GetIO().AddMousePosEvent(static_cast<float>(cx),
                                            static_cast<float>(cy));
        }
    }

    // one-frame update ---------------------------------------------------------
    void update()
    {
        if (phase == Phase::Idle || !window) return;

        ImGuiIO& io = ImGui::GetIO();
        elapsed += io.DeltaTime;

        ImVec2 desired = io.MousePos;        // default

        switch (phase) {
            case Phase::Move: {
                float t = std::clamp(elapsed / duration, 0.0f, 1.0f);
                desired = ImLerp(start_pos, target_pos, t);
                if (t >= 1.0f) { phase = Phase::Down; elapsed = 0.0f; }
                break;
            }
            case Phase::Down: io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
                              phase = Phase::Up;
                              break;
            case Phase::Up:   io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
                              phase = Phase::Idle;
                              break;
            default: break;
        }

        // Override hardware + ImGui each frame
        glfwSetCursorPos(window, desired.x, desired.y);
        io.AddMousePosEvent(desired.x, desired.y);
    }

    bool running() const { return phase != Phase::Idle; }
};









// ─────────────────────────────────────────────────────────────
//  ActionComposer  –  minimal two-pane “browser” for test steps
// ─────────────────────────────────────────────────────────────
struct ActionComposer
{
    static constexpr float      ms_SETTINGS_LABEL_WIDTH             = 196.0f;
    static constexpr float      ms_SETTINGS_WIDGET_WIDTH            = 256.0f;
    static constexpr float      ms_DESCRIPTION_FIELD_HEIGHT         = 60.0f;
    
// *************************************************************************** //
    
    inline void                 label                               (const char * text)
    { utl::LeftLabel(text, this->ms_SETTINGS_LABEL_WIDTH, this->ms_SETTINGS_WIDGET_WIDTH); ImGui::SameLine(); };
    
    
    
// *************************************************************************** //
//
//
//      1.  INTERNAL TYPES AND OBJECTS...
// *************************************************************************** //
// *************************************************************************** //
    
    enum class                  ActionType                      { CursorMove, Hotkey, COUNT };
    struct                      CursorMoveParams                { ImVec2 from{}, to{}; float duration{1.f}; };
    struct                      HotkeyParams                    { ImGuiKey key{ImGuiKey_C}; bool ctrl{true}, shift{false}, alt{false}; };
    
    static constexpr std::array<const char *, static_cast<size_t>( ActionType::COUNT )>
                                ms_ACTION_TYPE_NAMES            = { "CursorMove", "Hotkey" };

    struct Action {
        std::string         name        = "new action";
        std::string         descr       = "expect...";
        ActionType          type        = ActionType::CursorMove;
        CursorMoveParams    cursor;
        HotkeyParams        hotkey;
        bool                enabled     = true;
    };

    std::vector<Action>     actions;
    ImGuiTextFilter         filter;
    int                     sel = -1;                 // current selection
    
    
    
// *************************************************************************** //
//
//
//      2.  FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    //  "Begin"
    void Begin(void)
    {
        const float LEFT_W = 260.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
        ImGui::BeginChild("##SeqLeft",  {LEFT_W,0}, ImGuiChildFlags_Borders);
            draw_selector();
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("##SeqRight", {0,0},     ImGuiChildFlags_Borders);
            draw_inspector();
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    
    
private:

    // -----------------------  selector column  ------------------------ //
    void draw_selector()
    {
    
    
        //  Filter -------------------------------------------------------------- //
        if ( ImGui::InputTextWithHint("##flt", "filter", filter.InputBuf, IM_ARRAYSIZE(filter.InputBuf)) )
            { filter.Build(); }
            
        ImGui::Separator();


        //  Toolbar -------------------------------------------------------------   //
        if (ImGui::Button("+ Add")) {                    // Add new action
            actions.emplace_back();
            sel = static_cast<int>(actions.size()) - 1;
        }
        
        ImGui::SameLine();
        ImGui::BeginDisabled(sel <= 0);
        if ( ImGui::Button("Prev") )            { sel = std::max(sel - 1, 0); }     //  Select previous row
        ImGui::EndDisabled();
        
        
        ImGui::SameLine();
        
        
        ImGui::BeginDisabled(sel < 0 || sel >= actions.size() - 1);
        if (ImGui::Button("Next"))                       // select next row
            sel = std::min(sel + 1, (int)actions.size() - 1);
        ImGui::EndDisabled();

        ImGui::Separator();



        //  List ---------------------------------------------------------------- //
        ImGuiListClipper clip;
        clip.Begin(static_cast<int>(actions.size()));
        while (clip.Step())
        {
            for (int i = clip.DisplayStart; i < clip.DisplayEnd; ++i)
            {
                if ( !filter.PassFilter(actions[i].name.c_str()) )
                    continue;

                ImGui::PushID(i);

                // --- drag-handle (ASCII '≡') --------------------------------- //
                ImGui::TextUnformatted("[=]");             // visual hint only
                if (ImGui::IsItemActive() && ImGui::BeginDragDropSource(
                        ImGuiDragDropFlags_SourceNoPreviewTooltip))
                {
                    ImGui::SetDragDropPayload("ACTION_ROW", &i, sizeof i);
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("ACTION_ROW"))
                    {
                        int from = *static_cast<const int*>(p->Data);
                        reorder(from, i);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();

                // --- selectable label --------------------------------------- //
                bool selected = (sel == i);
                if (ImGui::Selectable(actions[i].name.c_str(), selected,
                                      ImGuiSelectableFlags_SpanAllColumns))
                    sel = i;
                ImGui::SameLine();

                // --- delete button ('X') ------------------------------------ //
                if (ImGui::SmallButton("X"))
                {
                    actions.erase(actions.begin() + i);
                    if (sel >= i) sel = std::max(sel - 1, (int)actions.size() - 1);
                    ImGui::PopID();
                    break;                                 // row array changed
                }
                ImGui::PopID();
            }
        }
        clip.End();
        return;
    }
    
    
    // -----------------------  inspector column  ----------------------- //
    void draw_inspector()
    {
        if ( sel < 0 || sel >= actions.size() )         { ImGui::TextDisabled("Select…"); return; }
        
        Action &        a           = actions[sel];
        int             type_int    = static_cast<int>( a.type );

        
        this->label("Name:");
        ImGui::InputText("##name", &a.name);
        this->label("Description:");
        ImGui::InputTextMultiline("description", &a.descr, { -FLT_MIN, ms_DESCRIPTION_FIELD_HEIGHT });

        
        if ( ImGui::Combo("type", &type_int, ms_ACTION_TYPE_NAMES.data(), static_cast<int>( ActionType::COUNT )) )
            { a.type    = static_cast<ActionType>(type_int); }


        ImGui::Separator();
        
        
        switch(a.type)
        {
            case ActionType::CursorMove:        { this->cursor_widgets(a); break;       }
            case ActionType::Hotkey:            { this->hotkey_widgets(a); break;       }
            default:                            { break; }
        }
        
        return;
    }
    
    
    //  "cursor_widgets"
    //
    void cursor_widgets(Action & a)
    {
        this->label("Init:");
        ImGui::DragFloat2   ("##from",          (float*)&a.cursor.from,     1,          0,          FLT_MAX,        "%.f");
        
        this->label("Final:");
        ImGui::DragFloat2   ("##to",            (float*)&a.cursor.to,       1,          0,          FLT_MAX,        "%.f");
        
        this->label("Duration:");
        ImGui::DragFloat    ("##duration",      &a.cursor.duration,         0.05f,      0.0f,       10,             "%.2f s");
        
        return;
    }
    
    
    //  "hotkey_widgets"
    //
    void hotkey_widgets(Action & a)
    {
        ImGui::Text("key: %d",a.hotkey.key);  // quick placeholder
        ImGui::Checkbox("Ctrl",&a.hotkey.ctrl); ImGui::SameLine();
        ImGui::Checkbox("Shift",&a.hotkey.shift); ImGui::SameLine();
        ImGui::Checkbox("Alt",&a.hotkey.alt);
        
        return;
    }
    
    
    
    
    
    //  "reorder"
    //
    void reorder(int from,int to) {
        if(from==to) return;
        Action tmp = std::move(actions[from]);
        actions.erase(actions.begin()+from);
        actions.insert(actions.begin()+to,std::move(tmp));
        sel = to;
        
        return;
    }
    
    
};









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "ft" NAMESPACE.



























// *************************************************************************** //
//
//
//
//      1.      MAIN FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "BeginFunctionalTesting"
//
void App::BeginFunctionalTesting([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    using namespace                     ft;
    static FunctionalTestRunner         s_test;
    static float                        duration_s          = 2.5f;
    static ActionComposer               composer;
    
    
    
    ImGui::Begin("Functional Tests", nullptr, flags);

        composer.Begin();          // draws the new browser
        
        
        
        ImGui::InputFloat("Move duration (s)", &duration_s, 0.1f, 1.0f, "%.2f");

        if (ImGui::Button("Run test: move && click"))
            s_test.start(S.m_glfw_window, ImVec2(400.0f, 300.0f), duration_s);

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            s_test.abort();

        /* live cursor read-out in the same units we feed to glfwSetCursorPos */
        ImVec2 mpos = ImGui::GetMousePos();
        ImGui::Text("MousePos: (%.1f , %.1f)", mpos.x, mpos.y);

        ImGui::TextUnformatted(s_test.running() ? "Running… (Esc aborts)" : "Idle");
    ImGui::End();

    /* drive the script */
    s_test.update();
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
