#pragma once

#define CTRL_MODE     (0)
#define COM_PORT_MODE (1)
#define OPENCV_MODE   (2)
//===========================
#define FRAME_SIZE_MIN (0)
#define FRAME_SIZE_640 (1)
#define FRAME_SIZE_MAX (2)


#define SERVER_SOCKET_PORT (18082)

//const std::string DATA_PREFIX = "d";
const std::string DATA_PREFIX = "f";
#define BINARY_PACKET 

//#define DEBUG_COUT 

const uint16_t BYNARY_PACKET_INT_KEY   = 2255;
//const uint16_t BYNARY_PACKET_FLOAT_KEY = 2233;  // was errors 
const uint16_t BYNARY_PACKET_FLOAT_KEY = 17531;

const uint16_t FLOAT_PRECISION   = 3;

//#define MOUSE_CHART_ENABLE

#define PORT_MSG_BUF_SIZE (1600)
#define MAX_HEX_VALS_IN_PACKET (700)

#define DISABLE_MOUSE_TEAM
#define LOG_ONE_VAL_TO_LINE 