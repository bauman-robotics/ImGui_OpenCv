#pragma once

#define CTRL_MODE     (0)
#define COM_PORT_MODE (1)
#define OPENCV_MODE   (2)
//===========================
#define FRAME_SIZE_MIN (0)
#define FRAME_SIZE_640 (1)
#define FRAME_SIZE_MAX (2)


#define SERVER_SOCKET_PORT (18082)

//#define DATA_PREFIX "data"
const std::string DATA_PREFIX = "d";

//#define DEBUG_COUT 

const uint16_t BYNARY_PACKET_KEY = 2255;


//#define MOUSE_CHART_ENABLE

#define SOCKET_MSG_BUF_SIZE (1600)
#define MAX_HEX_VALS_IN_PACKET (700)