/***********************************************************************************
*
*       ********************************************************************
*       ****          _ P Y T H O N _ I P C . H  ____  F I L E          ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      October 31, 2025.
*              MODULE:      CBAPP > CCOUNTER/           | _internal.h
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_COUNTER_PYTHON_IPC_H
#define _CBAPP_COUNTER_PYTHON_IPC_H  1
#include CBAPP_USER_CONFIG



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//      0.1.        ** MY **  HEADERS...
#include "cblib.h"
//  #include "widgets/widgets.h"
//  #include "utility/utility.h"
//  #include "utility/pystream/pystream.h"
//
//  #include "app/c_counter/_utility.h"


//      0.2         STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>
#include <algorithm>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


//      0.3.        "DEAR IMGUI" HEADERS...
#include "json.hpp"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"





namespace cb { namespace ipc { //     BEGINNING NAMESPACE "cb::ipc"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      0.      COMPILE-TIME CONSTANTS FOR INTER-PROCESS COMMUNICATION (IPC)...
// *************************************************************************** //
// *************************************************************************** //

/// @def        _CCOUNTER_IPC_REFACTOR
/// @brief      2026/01/14: Implemented as an emergency roll-back in case Pyton IPC changes are broken.
/// @note       if DEFINED:         Use the NEW versions of `parse_packet` functions.
///             if NOT DEFINED:     Use the OLD, EXISTING versions of `parse_packet`.
/// @todo       TODO
//
#define                         _CCOUNTER_IPC_REFACTOR      1
//
//  static constexpr size_t	        cv_PYTHON_MSG_CAP	        = 512ULL;



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 0.  "COMPILE-TIME CONSTANTS." ]].












// *************************************************************************** //
//
//
//
//      1.      PYTHON INTER-PROCESS COMMUNICATION (IPC) INTERFACE TYPES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1A. TYPES |        IPC ENUM TYPES.
// *************************************************************************** //

//  "PythonCMD"
//
enum class PythonCMD : uint8_t {
      None = 0
    , IntegrationWindow
    , CoincidenceWindow
    , COUNT   //
};
//
//  "DEF_PYTHON_CMD_FMT_STRINGS"
static constexpr cblib::EnumArray< PythonCMD, std::string_view >
DEF_PYTHON_CMD_FMT_STRINGS      = { {
    /*  None                    */    "%s\n"
    /*  IntegrationWindow       */  , "integration_window %.3f\n"
    /*  CoincidenceWindow       */  , "coincidence_window {}\n"
} };




//  "PythonJSONKey"
//
enum class PythonJSONKey : uint8_t {
      Type		= 0
    , Counts
    , Cycles
    , Message
    //  , Detail
    , COUNT
};
//
//  "DEF_PYTHON_JSON_KEY_NAMES"
static constexpr cblib::EnumArray< PythonJSONKey, const char * >
DEF_PYTHON_JSON_KEY_NAMES	= { {
    /*  Type        */	      "type"
    /*  Counts      */	    , "counts"
    /*  Cycles      */	    , "cycles"
    /*  Message     */	    , "message"
    //  /*  Detail      */	    , "details"
} };
    



//  "PythonPacketType"
//
enum class PythonPacketType : uint8_t {
      Unknown   = 0
    , Data
    , Status
    , Error
    , COUNT
};
//
//  "DEF_PYTHON_PACKET_TYPE_NAMES"
static constexpr cblib::EnumArray< PythonPacketType, const char * >
DEF_PYTHON_PACKET_TYPE_NAMES      = { {
    /*  Unknown     */    "unknown"
    /*  Data        */  , "data"
    /*  Status      */  , "status"
    /*  Error       */  , "error"
} };



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "IPC INTERFACE TYPES." ]].












// *************************************************************************** //
//
//
//
//      2.      PYTHON INTER-PROCESS COMMUNICATION (IPC) DATA TYPES...
// *************************************************************************** //
// *************************************************************************** //



//  "PythonIPC_t"
//
//      POD-style state packet for non-data IPC messages from the Python process.
//      - Reserves fixed capacity up-front to avoid repeated allocations.
//      - Enforces a max stored message length (truncates if needed).
//
struct PythonIPC_t
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
	static constexpr size_t	            ms_MESSAGE_BUFFER_SIZE		    = 512ULL;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    STATE VARIABLES.
    // *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
	std::string				            m_status_message			    {   };
	std::string				            m_error_message				    {   };
    
    // *************************************************************************** //
    //      1. |    GENERIC DATA.
    // *************************************************************************** //
	float					            m_status_time				    = 0.0f;
	float					            m_error_time				    = 0.0f;
	uint32_t				            m_status_count				    = 0U;
	uint32_t				            m_error_count				    = 0U;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        PythonIPC_t             (app::AppState & );             //  Def. Constructor.
                                        PythonIPC_t             (void)
	{
		this->m_status_message	.reserve(ms_MESSAGE_BUFFER_SIZE);
		this->m_error_message	.reserve(ms_MESSAGE_BUFFER_SIZE);
		return;
	}
                                        ~PythonIPC_t            (void)                          = default;
                            
                        
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        PythonIPC_t             (const PythonIPC_t &    src)       = delete;   //  Copy. Constructor.
                                        PythonIPC_t             (PythonIPC_t &&         src)       = delete;   //  Move Constructor.
    PythonIPC_t &                       operator =              (const PythonIPC_t &    src)       = delete;   //  Assgn. Operator.
    PythonIPC_t &                       operator =              (PythonIPC_t &&         src)       = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  INLINE OPERATION FUNCTIONS.
    // *************************************************************************** //

    //  "OnStatus"
	inline void                         OnStatus                            (std::string_view msg, float wall_now)
	{
		const size_t	n	= (msg.size() <= ms_MESSAGE_BUFFER_SIZE)    ? msg.size()    : ms_MESSAGE_BUFFER_SIZE;

		this->m_status_message.assign(msg.data(), n);
		this->m_status_time		= wall_now;
		++this->m_status_count;
		return;
	}

    //  "OnError"
	inline void                         OnError                             (std::string_view msg, float wall_now)
	{
		const size_t	n	= (msg.size() <= ms_MESSAGE_BUFFER_SIZE) ? msg.size() : ms_MESSAGE_BUFFER_SIZE;

		this->m_error_message.assign(msg.data(), n);
		this->m_error_time		= wall_now;
		++this->m_error_count;
		return;
	}


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //

    //  "HasStatus"
	inline bool                         HasStatus                           (void) const
	{
		return !this->m_status_message.empty();
	}

    //  "HasError"
	inline bool                         HasError                            (void) const
	{
		return !this->m_error_message.empty();
	}


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "ClearAll"
	inline void                         ClearAll                            (void) {
		this->m_status_message	.clear();
		this->m_error_message	.clear();
		this->m_status_time		= 0.0f;
		this->m_error_time		= 0.0f;
		this->m_status_count	= 0U;
		this->m_error_count		= 0U;
		return;
	}

    //  "ClearStatus"
	inline void                         ClearStatus                         (void) {
		this->m_status_message	.clear();
		this->m_status_time		= 0.0f;
		return;
	}

    //  "ClearError"
	inline void                         ClearError                          (void) {
		this->m_error_message	.clear();
		this->m_error_time		= 0.0f;
		return;
	}
    
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "PythonIPC_t" INLINE STRUCT DEFINITION.


//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "IPC DATA TYPES." ]].












// *************************************************************************** //
//
//
//
//      3.      INTER-PROCESS COMMUNICATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      3B. FUNCTIONS |     IPC UTILITY FUNCTIONS.
// *************************************************************************** //

//  "_packet_type_from_string"
//      Packet type decode (no magic "data"/"status"/"error" strings).
//
inline PythonPacketType _packet_type_from_string(std::string_view s) noexcept
{
	using PkType					= PythonPacketType;

	constexpr size_t	num_types	= static_cast<size_t>(PkType::COUNT);

	size_t				i			= 0ULL;
	PkType				pk_type		= static_cast<PkType>(i);
	const char *		type_name	= nullptr;

	if ( s.empty() )				{ return PkType::Unknown; }

	for ( i = 0ULL, pk_type = static_cast<PkType>(i); i < num_types; ++i, pk_type = static_cast<PkType>(i) )
	{
		type_name = DEF_PYTHON_PACKET_TYPE_NAMES[pk_type];
		if ( (type_name != nullptr) && (s == type_name) )	{ return pk_type; }
	}

	return PkType::Unknown;
}


//  "_extract_message_field"
//      We return `out` as an `out-param` because the `std::string` owned by `CCounter` will be pre-allocated to a fixed size.
//      Status/error message extraction (no magic key names).
//
inline void _extract_message_field(const nlohmann::json & j, std::string & out)
{
	const char *	key_message		= DEF_PYTHON_JSON_KEY_NAMES[PythonJSONKey::Message];
	auto			it				= j.find(key_message);

	out.clear();

	if ( (it != j.end()) && it->is_string() )	{ out = it->get<std::string>(); return; }

	out = j.dump();
	return;
}

 
 
 


// *************************************************************************** //
//      3A. FUNCTIONS |     IPC FUNCTIONS [[ NEW ]].
// *************************************************************************** //

//  "parse_python_message"
//
//      Unified parse: classify message type, parse data packets, or extrac
//      status/error payload. Uses centralized names for ALL keys/types.
//
///     @brief      Parse one JSON line emitted by the Python child.
///     @param      line                A single newline-delimited JSON object (one “packet”).
///     @param      mutual_exclusion    If false, adapt non-mutex FPGA counts into mutex-style totals (legacy behavior).
///     @param[out] out_type            Classified packet type (Data/Status/Error/Unknown).
///     @param[out] out_message         For Status/Error packets: extracted message string; otherwise cleared.
///     @return     If the packet is a Data packet, returns a fully-populated Packet; otherwise returns std::nullopt.
//
template <typename Packet>
inline std::optional< Packet >
parse_python_message( std::string_view line, bool mutual_exclusion, PythonPacketType & out_type, std::string & out_message )
{
	using						json				= nlohmann::json;
	using						value_type			= typename Packet::value_type;
	using						frequency_type		= typename Packet::frequency_type;
	using						Index				= typename Packet::Index;

	const char *				key_type			= DEF_PYTHON_JSON_KEY_NAMES[ PythonJSONKey::Type   ];
	const char *				key_counts			= DEF_PYTHON_JSON_KEY_NAMES[ PythonJSONKey::Counts ];
	const char *				key_cycles			= DEF_PYTHON_JSON_KEY_NAMES[ PythonJSONKey::Cycles ];

	std::optional< Packet >		result				= std::nullopt;
	Packet						packet				{};

	out_type		= PythonPacketType::Unknown;
	out_message		.clear();

	try
	{
		json						j					= json::parse(line);
		auto						it_type				= j.find(key_type);
		auto						it_counts			= j.find(key_counts);
		auto						it_cycles			= j.find(key_cycles);

		//  1) Optional envelope type.
		if ( (it_type != j.end()) && it_type->is_string() )
		{
			const std::string &	type_str				= it_type->get_ref<const std::string &>();
			out_type									= _packet_type_from_string(std::string_view(type_str));
		}

		//  2) Data packet detection (backwards-compatible): counts+cycles => Data.
		if ( (it_counts != j.end()) && (it_cycles != j.end()) )
		{
			if ( !it_counts->is_array() )				{ return std::nullopt; }

			const auto &			arr					= *it_counts;
			const size_t			N					= arr.size();

			if ( N != static_cast<size_t>(Index::COUNT) )	{ return std::nullopt; }

			size_t					i					= 0ULL;
			Index					idx					= static_cast<Index>(i);

			for ( i = 0ULL, idx = static_cast<Index>(i); i < N; ++i, idx = static_cast<Index>(i) )
			{
				packet.counts[idx] = arr[i].get<value_type>();
			}

			packet.frequency	= it_cycles->get<frequency_type>();
			out_type			= PythonPacketType::Data;

			//  Legacy adaptation: non-mutex → mutex
			if ( !mutual_exclusion )
			{
				for ( i = 0ULL, idx = static_cast<Index>(i); i < N; ++i, idx = static_cast<Index>(i) )
				{
					const size_t		val					= packet.counts[idx];
					const uint8_t		mask				= static_cast<uint8_t>(i);

					if ( val == 0 )						{ continue; }
					if ( mask == 0 || mask == 1 || mask == 2 || mask == 4 || mask == 8 )	{ continue; }

					if ( mask & 0x8 )					{ packet.counts[Index::A] += val; }
					if ( mask & 0x4 )					{ packet.counts[Index::B] += val; }
					if ( mask & 0x2 )					{ packet.counts[Index::C] += val; }
					if ( mask & 0x1 )					{ packet.counts[Index::D] += val; }
				}
			}

			result = packet;
			return result;
		}

		//  3) Non-data: extract message only for recognized Status/Error.
		if ( (out_type == PythonPacketType::Status) || (out_type == PythonPacketType::Error) )
		{
			_extract_message_field(j, out_message);
			return std::nullopt;
		}

		out_type = PythonPacketType::Unknown;
	}
	catch (const json::exception &)
	{
		out_type	= PythonPacketType::Unknown;
		out_message	.clear();
		result		= std::nullopt;
	}

	return result;
}



// *************************************************************************** //
//      3B. FUNCTIONS |     IPC FUNCTIONS [[ OLD ]].
// *************************************************************************** //

#ifdef _CCOUNTER_IPC_REFACTOR
// *************************************************************************** //
//
//
/// @brief Data-only wrapper around parse_python_message<> (ignores Status/Error/Unknown packets).
template <typename Packet>
inline std::optional< Packet >
parse_packet(std::string_view line, bool mutual_exclusion)
{
	PythonPacketType		pk_type		= PythonPacketType::Unknown;
	std::string				pk_msg		{   };

	return parse_python_message<Packet>(line, mutual_exclusion, pk_type, pk_msg);
}
//
//
// *************************************************************************** //
# else   //  _CCOUNTER_IPC_REFACTOR  //
// *************************************************************************** //
//
//
//  "parse_packet"
//      Parse one JSON‑line; returns nullopt on format errors.
//
template <typename Packet>
inline std::optional<Packet>
parse_packet(std::string_view line)
{
    using               json                = nlohmann::json;
    using               value_type          = Packet::value_type;
    using               frequency_type      = Packet::frequency_type;
    using               Index               = Packet::Index;
    //
    Packet              packet              {   };

    try
    {
        json            j       = json::parse(line);
        const auto &    arr     = j.at("counts");
        const size_t    N       = arr.size();
        size_t          i       = 0ULL;
        Index           idx     = static_cast<Index>(0);
        
        
        //      CASE 0 :    MORE COUNTER-VALUES RETURNED THAN WE EXPECTED  [ THIS SHOULD NEVER HAPPEN ]...
        if ( N != static_cast<size_t>(Index::COUNT) )       { return std::nullopt; }

        
        //      1.      FETCH THE VALUE OF EACH COUNTER FROM THE DATA-DELIVERY...
        for (i = 0ULL; i < N; idx = static_cast<Index>(++i) ) {
            packet.counts[idx]      = arr[i].get<value_type>();
        }
        packet.frequency    = j.at("cycles").get<frequency_type>();     //  [TO-DO]]:   REPLACE THE KEY-WORD "cycles"!!!
    }
    //
    //      ERROR :     Some type of malformed JSON / JSON-Keys, etc...
    catch (const json::exception & )
    {
        return std::nullopt;
    }
    
    return packet;
}


//  "parse_packet"
//
template <typename Packet>
inline std::optional< Packet >
parse_packet(std::string_view line, bool mutual_exclusion)   // NEW ARG (default = previous behaviour)
{
    using               json                = nlohmann::json;
    using               value_type          = Packet::value_type;
    using               frequency_type      = Packet::frequency_type;
    using               Index               = Packet::Index;
    //
    Packet              packet              {   };


    try
    {
        json            j       = json::parse(line);
        const auto &    arr     = j.at("counts");
        const size_t    N       = arr.size();
        size_t          i       = 0ULL;
        Index           idx     = static_cast<Index>(0);
        
        
        //      CASE 0 :    MORE COUNTER-VALUES RETURNED THAN WE EXPECTED  [ THIS SHOULD NEVER HAPPEN ]...
        if ( N != static_cast<size_t>(Index::COUNT) )       { return std::nullopt; }

        
        //      1.      FETCH THE VALUE OF EACH COUNTER FROM THE DATA-DELIVERY...
        for (i = 0ULL; i < N; idx = static_cast<Index>(++i) ) {
            packet.counts[idx]      = arr[i].get<value_type>();
        }
        packet.frequency    = j.at("cycles").get<frequency_type>();     //  [TO-DO]]:   REPLACE THE KEY-WORD "cycles"!!!


        //      2.      ADAPT GEORGES' FPGA VALUES FROM:  [ NON-MUTEX (Default) ] -- TO -- [ MUTEX ]...
        if ( !mutual_exclusion )
        {
            //      The index value encodes which APD channels participated:
            //          bit3=A,     bit2=B,     bit1=C,     bit0=D      (e.g. 0b1100 == AB)
            //
            for (i = 0ULL, idx = static_cast<Index>(i); i < N; idx = static_cast<Index>(++i) )
            {
                const size_t    val     = packet.counts[idx];
                const uint8_t   mask    = static_cast<uint8_t>(i);
                
                if (val == 0)           { continue; }

                //  skip single channels or UNUSED (they already hold the count)
                if ( mask == 0 || mask == 1 || mask == 2 || mask == 4 || mask == 8 )    { continue; }

                if (mask & 0x8)         { packet.counts[Index::A] += val; }     //  A.
                if (mask & 0x4)         { packet.counts[Index::B] += val; }     //  B.
                if (mask & 0x2)         { packet.counts[Index::C] += val; }     //  C.
                if (mask & 0x1)         { packet.counts[Index::D] += val; }     //  D.
            }
        }
    }
    //
    //      ERROR :     Some type of malformed JSON / JSON-Keys, etc...
    catch (const json::exception & )
    {
        return std::nullopt;
    }
    
    return packet;
}
//
//
#endif  //  _CCOUNTER_IPC_REFACTOR  //
// *************************************************************************** //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "IPC FUNCTIONS." ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb::ipc" NAMESPACE.












#endif      //  _CBAPP_COUNTER_PYTHON_IPC_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.
