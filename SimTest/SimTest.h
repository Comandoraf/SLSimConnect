/************************************************************************************************************************************************
*                                                                                                                                              *
*                                                    Flight Simulators Laboratory 2018                                                         *
*                                                                                                                                              *
*           SimTest.h header file                                                                                                                                   *
*                                                                                                                                              *
************************************************************************************************************************************************/


#pragma once

#define FEET_TO_METER   0.3048    // feet to meters coeficient
#define METER_TO_FEET   3.2808399 // meters to feet coeficient

#ifndef PI
#define PI        (3.1415926535897932384626433832795)    // better value
#endif

#ifndef TWOPI
#define TWOPI     (6.283185307179586476925286766559)     // 2.0*PI
#endif

#ifndef HALFPI
#define HALFPI    (1.5707963267948966192313216916398)    // PI/2.0
#endif

#ifdef QUARTERPI
#define QUARTERPI (0.78539816339744830961566084581988)   // PI/4.0
#endif

#ifndef DEG2RAD
#define DEG2RAD   (0.017453292519943295769236907684886)  // PI/180.0
#endif

#ifndef RAD2DEG
#define RAD2DEG   (57.295779513082320876798154814105)    // 180.0/PI
#endif



static enum DATA_DEFINE_ID {
  DEFINITION_1,
  DEFINITION_2,
  DEFINITION_3,
  DEFINITION_4,
  DEFINITION_5,
  DEFINITION_6,
  DEFINITION_INIT_POS,
};

static enum DATA_REQUEST_ID {
  REQUEST_1,
  REQUEST_2,
  REQUEST_6,
};

static enum EVENT_ID {
  KEY_FREEZE_LATITUDE_LONGITUDE_TOGGLE,
  KEY_FREEZE_LATITUDE_LONGITUDE_SET,
  KEY_FREEZE_ALTITUDE_SET,
  KEY_FREEZE_ATTITUDE_SET,
  KEY_FREEZE_ALTITUDE_TOGGLE,
  KEY_FREEZE_ATTITUDE_TOGGLE,
  EVENT_SIM_START,
};

struct PLANE_NAV {
  char    title[256];
  double  kohlsmann;
  double  altitude;
  double  latitude;
  double  longitude;
  double  vertspeed;
  double  gspeed;
  double  heading;
  double  pitch;
  double  bank;
  double  xvelocity;
  double  yvelocity;
  double  zvelocity;
};

