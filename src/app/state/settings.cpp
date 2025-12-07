/***********************************************************************************
*
*       ********************************************************************
*       ****           S E T T I N G S . C P P  ____  F I L E           ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      November 29, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/state/_settings.h"
//
#include <filesystem>
#include <system_error>
#include <chrono>
#include <format>

#ifndef _WIN32
    # include <unistd.h>        // getpid
    # include <sys/types.h>     // pid_t
#else
    # include <windows.h>       // pid_t
#endif  //  _WIN32  //



namespace cb { namespace app { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      0.      STATIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "CBApp_Settings_ReadLine"
//
static void CBApp_Settings_ReadLine(ImGuiContext* , ImGuiSettingsHandler* , void * entry, const char * line)
{
	static_cast<CBAppSettings*>(entry)->LoadFromIniLine(line);
    return;
}


//  "CBApp_Settings_WriteAll"
//
static void CBApp_Settings_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer * buf)
{
	buf->appendf("[%s][Settings]\n", handler->TypeName);
	CBAppSettings::instance().SaveToIniLines(*buf);
    
	buf->append("\n");
    return;
}




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 0.  "STATIC FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
//      1.      PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //


//  "LoadFromIniLine"
//
void CBAppSettings::LoadFromIniLine(const char * line)
{
    char    key[128] {};
    float   v[4]     {};
    int     iv       = 0;
    float   fv       = 0.0f;

    //  Try color first (4 floats)
    if ( sscanf(line, "%127[^=]=%f %f %f %f", key, &v[0], &v[1], &v[2], &v[3]) == 5 )
    {
        for (size_t i = 0; i < static_cast<size_t>(UIColor::COUNT); ++i)
        {
            const UIColor e = static_cast<UIColor>(i);
            if ( strcmp(key, CBAppSettings::ms_UI_COLOR_NAMES[e]) == 0 )
            {
                this->ms_UI_COLORS[e] = ImVec4(v[0], v[1], v[2], v[3]);
                break;
            }
        }
        return;
    }

    //  Try int/bool
    if ( sscanf(line, "%127[^=]=%d", key, &iv) == 2 )
    {
        if ( strcmp(key, "ShowDebugPanel") == 0 )       { m_show_debug_panel = (iv != 0);   }
        if ( strcmp(key, "SelectedApplet") == 0 )       { m_selected_applet  = iv;          }
        return;
    }
    //  Try float
    if ( sscanf(line, "%127[^=]=%f", key, &fv) == 2 )
    {
        if (strcmp(key, "MasterVolume") == 0)   m_master_volume = fv;
        return;
    }
    return;
}


//  "SaveToIniLines"
//
void CBAppSettings::SaveToIniLines(ImGuiTextBuffer & buf) const
{
	//	SECTION: SCALAR SETTINGS
	buf.appendf("ShowDebugPanel=%d\n",      m_show_debug_panel);
	buf.appendf("SelectedApplet=%d\n",      m_selected_applet);
	buf.appendf("MasterVolume=%f\n",        m_master_volume);
	buf.append("\n");

	//	SECTION: UI COLORS (ENUM-INDEXED VIA EnumArray)
	size_t      i           = 0ULL;
	UIColor     color_id    = static_cast<UIColor>(0);

	for (i = 0ULL; i < static_cast<size_t>(UIColor::COUNT); ++i, color_id = static_cast<UIColor>(i))
	{
		const ImVec4&   c   = CBAppSettings::ms_UI_COLORS[color_id];

		buf.appendf("%s=%f %f %f %f\n",
		            ms_UI_COLOR_NAMES[color_id],
		            c.x, c.y, c.z, c.w);
	}
    return;
}





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "PUBLIC API" ]].












// *************************************************************************** //
//
//
//
//      2.      STATIC HANDLER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_settings_read_open"
//
void * CBAppSettings::_settings_read_open(ImGuiContext * , ImGuiSettingsHandler * , const char * name) noexcept
{
	if (strcmp(name, cv_SUBSECTION_SETTINGS) == 0 || strcmp(name, cv_SUBSECTION_COLORS) == 0)
		return &instance();

	return nullptr;
}


//  "_settings_read_line"
//
void CBAppSettings::_settings_read_line(ImGuiContext * , ImGuiSettingsHandler * , void * entry, const char * line) noexcept
{
	if (entry)
		static_cast<CBAppSettings*>(entry)->_load_from_ini_line(line);

	return;
}


//  "_settings_write_all"
//
void CBAppSettings::_settings_write_all(ImGuiContext * , ImGuiSettingsHandler * handler, ImGuiTextBuffer * buffer) noexcept
{
    CBAppSettings &     obj         = CBAppSettings::instance();
	size_t              i           = 0ULL;
	UIColor             color_id    = static_cast<UIColor>(0);



	//	SECTION:    SCALAR SETTINGS
	buffer->appendf("[%s][%s]\n", handler->TypeName, CBAppSettings::cv_SUBSECTION_SETTINGS);
	obj._save_to_ini_lines(*buffer);
	buffer->append("\n");


	//	SECTION:    UI---COLORS
	buffer->appendf("[%s][%s]\n", handler->TypeName, CBAppSettings::cv_SUBSECTION_COLORS);



	for (i = 0ULL; i < static_cast<size_t>(UIColor::COUNT); ++i, color_id = static_cast<UIColor>(i))
	{
		const ImVec4 &      c       = obj.ms_UI_COLORS[color_id];

		buffer->appendf(
              "%s=%f %f %f %f\n"
            , ms_UI_COLOR_NAMES[color_id]
            , c.x, c.y, c.z, c.w);
	}
	buffer->append("\n");

	return;
}





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "HANDLER FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
//      3.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "_load_from_ini_line"
//
void CBAppSettings::_load_from_ini_line(const char * line) noexcept
{
	//	SECTION: PARSING LOGIC – FORGIVING, ORDER-INDEPENDENT
	char    key[128]    {};
	float   v[4]        {};
	int     iv          = 0;
	float   fv          = 0.0f;

	//	COLORS FIRST (4 FLOATS)
	if (sscanf(line, "%127[^=]=%f %f %f %f", key, &v[0], &v[1], &v[2], &v[3]) == 5)
	{
		for (size_t i = 0; i < static_cast<size_t>(UIColor::COUNT); ++i)
		{
			const UIColor   e   = static_cast<UIColor>(i);

			if (strcmp(key, ms_UI_COLOR_NAMES[e]) == 0)
			{
				ms_UI_COLORS[e] = ImVec4(v[0], v[1], v[2], v[3]);
				break;
			}
		}
		return;
	}

	//	INT / BOOL
	if (sscanf(line, "%127[^=]=%d", key, &iv) == 2)
	{
		if      (strcmp(key, "ShowDebugPanel") == 0)    m_show_debug_panel = (iv != 0);
		else if (strcmp(key, "SelectedApplet") == 0)    m_selected_applet   = iv;
		return;
	}

	//	FLOAT
	if (sscanf(line, "%127[^=]=%f", key, &fv) == 2)
	{
		if (strcmp(key, "MasterVolume") == 0)          m_master_volume     = fv;
		return;
	}
 
    return;
}


//  "_save_to_ini_lines"
//
void CBAppSettings::_save_to_ini_lines(ImGuiTextBuffer & buf) const noexcept
{
	//	SECTION: SCALAR SETTINGS ONLY (COLORS ARE HANDLED DIRECTLY IN _settings_write_all)
	buf.appendf("ShowDebugPanel=%d\n",      m_show_debug_panel);
	buf.appendf("SelectedApplet=%d\n",      m_selected_applet);
	buf.appendf("MasterVolume=%f\n",        m_master_volume);
 
    return;
}





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "UTILITY FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.
