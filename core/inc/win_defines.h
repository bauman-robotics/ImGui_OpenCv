#pragma once
// #include "win_defines.h"

//=== ALL_WIN_SIZE =====
#define ALL_WIN_WIDTH  (880)
#define ALL_WIN_HIGH   (736)


//=== MAIN_MENU =====
#define MAIN_MENU_POS_W (0)
#define MAIN_MENU_POS_H (0)
#define MAIN_MENU_W     (MAIN_WIN_WIDTH)
#define MAIN_MENU_H     (38)

//=== MAIN_WIN =====
#define MAIN_WIN_X_POS  (MAIN_MENU_POS_W)
#define MAIN_WIN_Y_POS  (MAIN_MENU_H)
#define MAIN_WIN_WIDTH  (ALL_WIN_WIDTH)
#define MAIN_WIN_HIGH   (ALL_WIN_HIGH - MAIN_MENU_H)
//===================

#define EMPTY_W (8)
#define EMPTY_H (8)

//=== LINE 1 ========
#define L1_W (230)
#define L1_H (150)

#define L1_P1_SOCKET_PORT_CTRL_W  (L1_W)
#define L1_P1_SOCKET_PORT_CTRL_H  (L1_H)

#define L1_P2_SOCKET_DATA_CTRL_W  (L1_W)
#define L1_P2_SOCKET_DATA_CTRL_H  (L1_H)

#define L1_P3_SOCKET_LOGS_CTRL_W  (L1_W) 
// MAIN_WIN_WIDTH - EMPTY_W * 4 - L1_W * 2)
#define L1_P3_SOCKET_LOGS_CTRL_H  (L1_H)

#define L1_P4_SOCKET_FILTER_CTRL_W (MAIN_WIN_WIDTH - EMPTY_W * 5 - L1_W * 3)
#define L1_P4_SOCKET_FILTER_CTRL_H (L1_H)


//====================
#define L2_P1_SOCKET_PLOT_W       (MAIN_WIN_WIDTH - EMPTY_W *2)
#define L2_P1_SOCKET_PLOT_H       (MAIN_WIN_HIGH - L1_H - EMPTY_H * 3)

#define TEXT_LINE_H               (50)
#define L2_PLOT_HEIGHT            (L2_P1_SOCKET_PLOT_H - TEXT_LINE_H * 2 - 20)  // 400


//====================
#define SERIAL_CTRL_W     (280)
#define SERIAL_CTRL_H     (270)

#define SERIAL_PLOT_W     (575)
#define SERIAL_PLOT_H     (270)

#define MOUSE_PLOT_W      (865)
#define MOUSE_PLOT_H_ON   (270)
#define MOUSE_PLOT_H_OFF  (70)

#define MOUSE_CTRL_W      (250)
#define MOUSE_CTRL_H      (150)

#define TEST_GROUP_W      (300)
#define TEST_GROUP_H      (150)

//====================

#define CV1_GROUP_W      (300)
#define CV1_GROUP_H      (150)

#define CV2_GROUP_W      (300)
#define CV2_GROUP_H      (150)