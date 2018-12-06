#pragma once
#include "Engine\Core\Blackboard.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include <string.h>
#include <limits>

extern Blackboard g_gameConfigBlackboard;

// EXPRESSIONS =============================================================================
constexpr float CLIENT_ASPECT = 16/9.f;// We are requesting a 1:1 aspect (square) window area
constexpr float DELTA_TIME = 1.0f/60.0f; //
constexpr int MAX_LIGHTS = 8;

constexpr int MAX_PROFILER_HISTORY = 256;

//profiler globals  =============================================================================
constexpr int PROFILER_SORT_LAYER = 999;
constexpr float PROFILER_TEXT_CELL_HEIGHT = 11.f; //in pixels
constexpr float PROFILER_TEXT_WIDTH_PADDING = 20.f; //in pixels
constexpr float PROFILER_TEXT_HEIGHT_PADDING = 10.f;

//Remote Command Service Globals  =============================================================================
constexpr int REMOTE_SERVICE_DEFAULT_PORT = 29283;
constexpr int REMOTE_SERVICE_MAX_CLIENTS = 16;
constexpr int REMOTE_SERVICE_MAX_BUFFER_SIZE = 256 - 1U;

constexpr char* LOGGER_DATA_FILE = "Data/Log/log.txt";

//Net Globals =============================================================================
constexpr float NET_RELIABLE_RESEND_RATE_PER_SECOND = 0.001f;

constexpr char* DEFAULT_SERVICE_PORT = "12345";

#define MAX_NET_DEFINITION_REGISTRATIONS (UINT8_MAX)
#define INVALID_SENDER_ID (UINT8_MAX)
#define INVALID_PACKET_ACK (UINT16_MAX)
#define MAX_RELIABLES_PER_PACKET (32)
#define MAX_MESSAGE_CHANNELS (8)


//stringf stack local  =============================================================================
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;
const int MAX_MAX_CONSOLE_SIZE = 100;

// DEFINITIONS =============================================================================
#define UNUSED(x) (void)(x);

//todo info
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// the important bits
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
        " --------------------------------------------------------------------------------------\n" \
        "|  TODO :   " ##x "\n" \
        " --------------------------------------------------------------------------------------\n" )


#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" );// ASSERT_RECOVERABLE(0, "") 

//#define PROFILER_ENABLED

// TYPEDEFS =============================================================================
typedef unsigned char byte_t;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;

constexpr uint16_t UNSIGNED_WORD_MAX = 32'767;

// Methods =============================================================================
void EngineStartup();
void EngineShutdown();
const std::string GetCurrentDateTime();