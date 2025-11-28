/***********************************************************************************
*
*       ********************************************************************
*       ****       U N I T _ T E S T I N G . C P P  ____  F I L E       ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      November 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/delegators/_detail_view.h"
#include <random>
#include <algorithm>

#include <vector>
#include <string>
#include <stdexcept>
#include <iterator>
#include <sstream>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1A.     PERIPHERAL DEFINITIONS FOR "ORCHID"...
// *************************************************************************** //
// *************************************************************************** //
namespace {


using       namespace       cblib::containers;



//  "IncrementAction"
//      Dummy action: increments an int
//
struct IncrementAction : orchid::ABC
{
    IncrementAction             (int & ref_, int delta_ = 1)
        : target(ref_), delta(delta_)
    {   }
        

    void                undo    (void)  noexcept override           { target -= delta;      }
    void                redo    (void)  noexcept override           { target += delta;      }
    std::string_view    label   (void)  const noexcept override     { return "Increment";   }

private:
    int &       target      ;
    int         delta       ;
};



//  "ToggleFlagAction"
//      flips a bool each time it is redone/undone
//
struct ToggleFlagAction : orchid::ABC
{
    ToggleFlagAction            (bool & ref)
        : target(ref)
    {   }
        

    void                undo    (void)  noexcept override           { target = !target;         }
    void                redo    (void)  noexcept override           { target = !target;         }
    std::string_view    label   (void)  const noexcept override     { return "Toggle flag";     }

private:
    bool &      target      ;
};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1A.  "ORCHID PERIPHERAL" ]].
}   //  END "anonymous" NAMESPACE.












// *************************************************************************** //
//
//
//
//      1B.     UNIT-TESTING FOR "ORCHID"...
// *************************************************************************** //
// *************************************************************************** //

//  "TestOrchid"
//
void CBDebugger::TestOrchid(void) noexcept
{
    static int                      value               = 0;
    static bool                     flag                = false;
    //
    //
    //
    [[maybe_unused]] const bool     is_hovered          = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows      );
    [[maybe_unused]] bool           invoke_undo         = false;
    bool                            invoke_redo         = false;
    
    
    
    //      CASE 0 :    ONLY FETCH INPUT IF WINDOW IS HOVERED...
    if ( is_hovered )
    {
        invoke_undo         = ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z                        );
        invoke_redo         = ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z       );
    }
    
    
    
    //      1.          DATA---PRESENTATION...
    ImGui::SeparatorText("Values");
    ImGui::Indent();
    //
    //
        ImGui::Text( "Value:    %d", value);
        ImGui::Text( "Flag:     %s", (flag) ? "True" : "False");
    //
    //
    ImGui::Unindent();
    
    

    //      2.          DATA---PRESENTATION...
    ImGui::NewLine();
    ImGui::SeparatorText("Orchid Information");
    ImGui::Indent();
    //
    //
        ImGui::Text(
              "Undoable:\t%zu \nRedoable:\t%zu"
             , this->S.Orchid_undo_count()
             , this->S.Orchid_redo_count()
        );
    //
    //
    ImGui::Unindent();



    //      3.          USER---CONTROLS / ACTIONS...
    ImGui::NewLine();
    ImGui::SeparatorText("Actions");
    ImGui::Indent();
    //
    //
        if ( ImGui::Button("Add  +1") )
        {
            this->S.OrchidPush( std::make_unique<IncrementAction>(value, 1) );
        }

        ImGui::SameLine();
        if ( ImGui::Button("Add  +5") )
        {
            this->S.OrchidPush( std::make_unique<IncrementAction>(value, 5) );
        }

        ImGui::SameLine();
        if ( ImGui::Button("Toggle Flag") )
        {
            this->S.OrchidPush( std::make_unique<ToggleFlagAction>(flag) );
        }
    //
    //
    ImGui::Unindent();



    //      X.          QUERY USER INPUT...
    //  if ( invoke_undo )          { this->S.OrchidUndo(); }         //  UNDO.   [ CTRL Z ].
    //  if ( invoke_redo )          { this->S.OrchidRedo(); }         //  REDO.   [ CTRL SHIFT Z ].

    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1B.  "ORCHID UNIT-TESTING" ]].












// *************************************************************************** //
//
//
//
//      2A.     PERIPHERAL DEFINITIONS FOR "UNIT-TESTING"...
// *************************************************************************** //
// *************************************************************************** //
//
namespace tests { //     BEGINNING NAMESPACE "tests"...


// *************************************************************************** //
//      "tests" |    TYPES.
// *************************************************************************** //

template <
      typename T
    , typename Alloc = std::allocator<T>
>
using       ContainerType       = cblib::ndRingBuffer<T,Alloc>;


//  "TestResult"
//
struct TestResult {
    const char *    name        { nullptr };
    bool            pass        { false   };
    std::string     note        {};
};


//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.0.  "TYPES" ].



// *************************************************************************** //
//      "tests" |    FUNCTIONS.
// *************************************************************************** //

/// \brief Convert a sequence to a compact string for notes.
template<typename It>
[[nodiscard]] inline std::string to_string_seq(It first, It last)
{
    std::ostringstream   oss;
    oss << "[";
    bool first_elem = true;
    for (; first != last; ++first) {
        if (!first_elem) { oss << ", "; }
        first_elem = false;
        oss << *first;
    }
    oss << "]";
    return oss.str();
}


/// \brief Collect ndRingBuffer elements into a std::vector in chronological order.
template<typename T, typename Alloc = std::allocator<T>>
[[nodiscard]] inline std::vector<T> snapshot(const ContainerType<T,Alloc> & rb)
{
    std::vector<T>   v;
    v.reserve(rb.size());
    for (const auto & x : rb) { v.push_back(x); }
    return v;
}


/// \brief Compare buffer sequence to an expected initializer_list.
template<typename T, typename Alloc>
[[nodiscard]] inline bool equals_seq(const ContainerType<T,Alloc> & rb,
                                     std::initializer_list<T> expected)
{
    auto v = snapshot(rb);
    return std::equal(v.begin(), v.end(), expected.begin(), expected.end());
}




//  "test_t0_smoke"
//
[[nodiscard]] inline TestResult test_t0_smoke(void)
{
    using           RB      = ContainerType<int>;   //    cblib::containers::ndRingBuffer<int>;

    RB              a       {0};
    RB              b       {8};

    bool            ok_a    = (a.size()==0 && a.capacity()==0 && a.empty() && a.data()==nullptr);
    bool            ok_b    = (b.size()==0 && b.capacity()==8 && b.empty() && b.data()!=nullptr);

    TestResult      r;
    r.name                  = "T0 --- Smoke & invariants";
    r.pass                  = ok_a && ok_b;
    if (!r.pass) {
        r.note              = std::string("a: size=") + std::to_string(a.size())
            + " cap="       + std::to_string(a.capacity())
            + " empty="     + (a.empty()    ? "true"    :"false")
            + " data="      + (a.data()     ? "ptr"     :"null")
            + " | b: size=" + std::to_string(b.size())
            + " cap="       + std::to_string(b.capacity())
            + " empty="     + (b.empty()    ? "true"    :"false")
            + " data="      + (b.data()     ? "ptr"     :"null");
    }
    return r;
}


//  "test_t1_basic_push"
//
[[nodiscard]] inline TestResult test_t1_basic_push()
{
    using           RB      = ContainerType<int>;   //    cblib::containers::ndRingBuffer<int>;
    RB              rb      {4};

    rb.push_back(0);
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    rb.push_back(4);
    rb.push_back(5);

    bool        ok_size = (rb.size()==4);
    bool        ok_seq  = equals_seq(rb, {2,3,4,5});
    bool        ok_fb   = (!rb.empty() && rb.front()==2 && rb.back()==5 && rb.top()==rb.back());

    TestResult  r;
    r.name              = "T1 --- Push & overwrite";
    r.pass              = ok_size && ok_seq && ok_fb;
    if (!r.pass) {
        auto cur        = snapshot(rb);
        r.note          = std::string("got=") + to_string_seq(cur.begin(), cur.end())
                        + " expected=[2, 3, 4, 5]"
                        + " size=" + std::to_string(rb.size());
    }
    return r;
}


//  "test_t2_wrap_mapping"
//
[[nodiscard]] inline TestResult test_t2_wrap_mapping()
{
    using           RB      = ContainerType<int>;   //    cblib::containers::ndRingBuffer<int>;
    RB              rb      {5};

    // Create a wrap: [0,1], then remove newest, then push to wrap head around.
    rb.push_back(0);
    rb.push_back(1);
    rb.push_back(2);
    rb.pop_back();          // remove '2', sequence [0,1]
    rb.push_back(3);
    rb.push_back(4);
    rb.push_back(5);
    rb.push_back(6);        // forces wrap but keeps chronological order

    // Expected logical sequence now [0,1,3,4,5,6] truncated to cap==5 → [1,3,4,5,6]
    bool        ok_seq = equals_seq(rb, {1,3,4,5,6});

    // Cross-check via operator[] view
    std::vector<int>  via_index;
    via_index.reserve(rb.size());
    for (std::size_t i=0; i<rb.size(); ++i) { via_index.push_back(rb[i]); }

    auto        cur   = snapshot(rb);
    bool        ok_agree = std::equal(cur.begin(), cur.end(), via_index.begin(), via_index.end());

    TestResult  r;
    r.name              = "T2 --- Wrap-around mapping";
    r.pass              = ok_seq && ok_agree;
    if (!r.pass) {
        r.note          = std::string("iter=") + to_string_seq(cur.begin(), cur.end())
                        + " idx=" + to_string_seq(via_index.begin(), via_index.end())
                        + " expected=[1, 3, 4, 5, 6]";
    }
    return r;
}


//  "test_t3_at_bounds"
//
[[nodiscard]] inline TestResult test_t3_at_bounds()
{
    using           RB      = ContainerType<int>;   //    cblib::containers::ndRingBuffer<int>;
    RB              rb      {3};

    rb.push_back(10);
    rb.push_back(11);

    bool        ok_in   = (rb.at(0)==10 && rb.at(1)==11);
    bool        threw   = false;

    try {
        (void)rb.at(2);
    } catch (const std::out_of_range &) {
        threw = true;
    } catch (...) {
        threw = false;
    }

    TestResult  r;
    r.name              = "T3 --- at() bounds";
    r.pass              = ok_in && threw;
    if (!r.pass) {
        r.note          = std::string("at(0)=") + std::to_string(rb.at(0))
                        + " at(1)=" + std::to_string(rb.at(1))
                        + " at(2) threw=" + (threw?"true":"false");
    }
    return r;
}


//  "test_t4_clear_semantics"
//
[[nodiscard]] inline TestResult test_t4_clear_semantics()
{
    using RB = ContainerType<int>;
    RB rb{3};
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    rb.clear();
    rb.push_back(7);
    rb.push_back(8);

    const bool ok_size = (rb.size()==2);
    const bool ok_seq  = equals_seq(rb, {7,8});

    TestResult r;
    r.name = "T4 --- clear() semantics";
    r.pass = ok_size && ok_seq;
    if (!r.pass) {
        auto cur = snapshot(rb);
        r.note = std::string("got=") + to_string_seq(cur.begin(), cur.end()) + " expected=[7, 8]";
    }
    return r;
}


//  "test_t5_push_vs_emplace"
//
[[nodiscard]] inline TestResult test_t5_push_vs_emplace()
{
    // --- local counters -------------------------------------------------
    static int ctor        = 0;
    static int copy_ctor   = 0;
    static int move_ctor   = 0;
    static int copy_assign = 0;
    static int move_assign = 0;
    static int dtor        = 0;

    auto reset = [&](){
        ctor = copy_ctor = move_ctor = copy_assign = move_assign = dtor = 0;
    };

    // --- instrumented struct -------------------------------------------
    struct S {
        int v{0};
        S()                          { ++ctor; }
        explicit S(int x) : v{x}     { ++ctor; }
        S(const S& o)  : v{o.v}      { ++copy_ctor; }
        S(S&& o) noexcept : v{o.v}   { ++move_ctor; }
        S& operator=(const S& o)     { v = o.v; ++copy_assign;  return *this; }
        S& operator=(S&& o) noexcept { v = o.v; ++move_assign;  return *this; }
        ~S()                         { ++dtor; }
        bool operator==(int x) const noexcept { return v == x; }
    };

    using RB = ContainerType<S>;
    RB rb{4};

    reset();
    S a{1};                   // ctor++
    rb.push_back(a);          // copy-assign
    rb.push_back(S{2});       // move-assign (temp)
    rb.emplace_back(3);       // in-place construct
    rb.emplace_back(4);

    // --- sequence check -------------------------------------------------
    std::vector<int> seq;
    for (const auto& x : rb) seq.push_back(x.v);
    const bool ok_seq   = (seq == std::vector<int>{1,2,3,4});

    // --- counter sanity -------------------------------------------------
    const bool ok_paths = (copy_assign >= 1) && (move_assign >= 1) && (ctor >= 1);

    TestResult r;
    r.name = "T5 --- push/move/emplace paths";
    r.pass = ok_seq && ok_paths;

    if (!r.pass) {
        std::ostringstream oss;
        oss << "seq=" << to_string_seq(seq.begin(), seq.end())
            << " | ctor="        << ctor
            << " copy_ctor="     << copy_ctor
            << " move_ctor="     << move_ctor
            << " copy_assign="   << copy_assign
            << " move_assign="   << move_assign
            << " dtor="          << dtor;
        r.note = oss.str();
    }
    return r;
}


//  "test_t6_pops"
//
[[nodiscard]] inline TestResult test_t6_pops()
{
    using RB = ContainerType<int>;
    RB rb{4};
    rb.push_back(0);
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);                 // [0,1,2,3]

    rb.pop_front();                  // [1,2,3]
    const bool ok_pf = equals_seq(rb, {1,2,3});

    rb.push_back(4);                 // [1,2,3,4]
    rb.pop_back();                   // [1,2,3]
    const bool ok_pb = equals_seq(rb, {1,2,3});

    TestResult r;
    r.name = "T6 --- pop_front/pop_back";
    r.pass = ok_pf && ok_pb;
    if (!r.pass) {
        auto cur = snapshot(rb);
        r.note = std::string("got=") + to_string_seq(cur.begin(), cur.end()) + " expected=[1, 2, 3]";
    }
    return r;
}


//  "test_t7_iterators"
//
[[nodiscard]] inline TestResult test_t7_iterators()
{
    using RB = ContainerType<int>;
    RB rb{5};
    rb.push_back(0);
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    rb.push_back(4);

    // Forward via range-for
    std::vector<int> fwd;
    for (const auto& x : rb) fwd.push_back(x);

    // Reverse via rbegin/rend
    std::vector<int> rev;
    for (auto it = rb.rbegin(); it != rb.rend(); ++it) rev.push_back(*it);

    // Const iteration
    const RB& crb = rb;
    std::vector<int> fwd_const;
    for (auto it = crb.cbegin(); it != crb.cend(); ++it) fwd_const.push_back(*it);

    const bool ok_fwd  = (fwd == std::vector<int>({0,1,2,3,4}));
    const bool ok_rev  = (rev == std::vector<int>({4,3,2,1,0}));
    const bool ok_c    = (fwd_const == fwd);

    TestResult r;
    r.name = "T7 --- iterators (forward/reverse/const)";
    r.pass = ok_fwd && ok_rev && ok_c;
    if (!r.pass) {
        r.note = std::string("fwd=") + to_string_seq(fwd.begin(), fwd.end())
               + " rev=" + to_string_seq(rev.begin(), rev.end())
               + " c=" + to_string_seq(fwd_const.begin(), fwd_const.end());
    }
    return r;
}


//  "test_t8_invalidation_info"
//
[[nodiscard]] inline TestResult test_t8_invalidation_info()
{
    using RB = ContainerType<int>;
    RB rb{4};
    rb.push_back(0);
    rb.push_back(1);

    auto it = rb.begin();     // save an iterator
    (void)it;

    // Any mutation invalidates iterators by design.
    rb.push_back(2);

    TestResult r;
    r.name = "T8 --- iterator invalidation (info)";
    r.pass = true;
    r.note = "By policy: any mutating op invalidates all iterators; do not deref saved iterators after push/pop/clear.";
    return r;
}


//  "test_t9_zero_capacity"
//
[[nodiscard]] inline TestResult test_t9_zero_capacity()
{
    using RB = ContainerType<int>;
    RB z{0};
    z.push_back(1);
    z.emplace_back(2);
    z.pop_front();
    z.pop_back();

    const bool ok = (z.size()==0 && z.capacity()==0 && z.empty() && z.data()==nullptr);

    TestResult r;
    r.name = "T9 --- zero-capacity edge";
    r.pass = ok;
    if (!r.pass) {
        r.note = "Expected inert container: size=0, cap=0, empty=true, data=nullptr";
    }
    return r;
}






// *************************************************************************** //
//      "tests" |    MAIN TABLE.
// *************************************************************************** //

//  "draw_ndringbuffer_basic_tests"
//
//      Replace your existing draw_ndringbuffer_basic_tests() with this cached version.
//      It runs the tests once, caches the results, and only renders them on subsequent frames.
//
inline void draw_ndringbuffer_basic_tests()
{
    static bool                     s_ran   = false;
    static std::vector<TestResult>  s_results;

    if (!s_ran) {
        s_results = {
              test_t0_smoke()
            , test_t1_basic_push()
            , test_t2_wrap_mapping()
            , test_t3_at_bounds()
            , test_t4_clear_semantics()
            , test_t5_push_vs_emplace()
            , test_t6_pops()
            , test_t7_iterators()
            , test_t8_invalidation_info()
            , test_t9_zero_capacity()
        };
        s_ran = true;
    }

    int passed = 0;
    for (const auto & r : s_results) { if (r.pass) ++passed; }

    ImGui::SeparatorText("Unit-Testing for \"ndRingBuffer\" | T0---T9 (cached)");
    ImGui::Text("Passed %d / %d", passed, (int)s_results.size());

    if (ImGui::BeginTable("ndrb_tests_tbl", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Test");
        ImGui::TableSetupColumn("Result");
        ImGui::TableSetupColumn("Note");
        ImGui::TableHeadersRow();

        for (const auto & r : s_results) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);      ImGui::TextUnformatted(r.name);
            ImGui::TableSetColumnIndex(1);      ImGui::TextUnformatted(r.pass ? "PASS" : "FAIL");
            ImGui::TableSetColumnIndex(2);
            if (!r.note.empty())    { ImGui::TextUnformatted(r.note.c_str()); }
        }
        ImGui::EndTable();
    }
}




//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.1.  "FUNCTIONS" ].






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "tests".

    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2A.  "PERIPHERALS" ]].






// *************************************************************************** //
//
//
//
//      2B.     MISC. "UNIT-TESTING"...
// *************************************************************************** //
// *************************************************************************** //

//  "TestndRingBuffer"
//
void CBDebugger::TestndRingBuffer(void) noexcept
{
    using           namespace       tests;
    static bool     gate            = false;
    
    
    //      CASE 1 :    PROMPT TO PERFORM TESTS...
    if (!gate)
    {
        if ( ImGui::Button("Perform \"ndRingBuffer\" Unit-Tests") ) {
            gate    = true;
        }
    }
    //
    //      CASE 2 :    BEGIN UNIT-TESTS...
    else {
        tests::draw_ndringbuffer_basic_tests();
    }
    
    
    
    
    
    
    return;
}

    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2B.  "UNIT TESTS" ]].












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
