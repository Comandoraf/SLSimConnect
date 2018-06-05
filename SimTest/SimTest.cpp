/************************************************************************************************************************************************
 *                                                                                                                                              *
 *                                                    Flight Simulators Laboratory 2018                                                         *
 *                                                                                                                                              *
 *                                                                                                                                              *
 *   SimTest.cpp: Simple FSX SimConnect console App for demonstration purposes. Receives flight data parameters from the FSX plane movements.   *
 *                                                                                                                                              *
 *    Plane flight parameters:                                                                                                                  *
 *        1. Kohlsman setting               [hg]                                                                                                *
 *        2. Plane Altitude                 [feet]                                                                                              *
 *        3. Plane Latitude                 [degrees]                                                                                           *
 *        4. Plane Longitude                [degrees]                                                                                           *
 *        5. Vertical Speed                 [Feet/second]                                                                                       *
 *        6. GROUND VELOCITY                [Knots]                                                                                             *    
 *        7. PLANE HEADING DEGREES TRUE     [degrees]                                                                                           *
 *        8. PLANE PITCH DEGREES            [degrees]                                                                                           *
 *        9. PLANE BANK DEGREES             [degrees]                                                                                           *
 *       10. PLANE BANK DEGREES TRUE        [radians]                                                                                           *
 *       11. VELOCITY BODY X                [m/s]                                                                                               *
 *       12. VELOCITY BODY Y                [m/s]                                                                                               *
 *       13. VELOCITY BODY Z                [m/s]                                                                                               *
 *                                                                                                                                              *
 ************************************************************************************************************************************************/ 

#include "stdafx.h"


HANDLE ghSimConnect = NULL;
BOOL gbQuit = false;
SIMCONNECT_DATA_INITPOSITION gInit;
//SIMCONNECT_DATA_FACILITY_VOR gVOR;
SIMCONNECT_DATA_WAYPOINT gVOR;

// Callback MyDispatchProc procedure receives requested data type values from the FSX Flight Simulator during successfull SimConnect process.
// Registered FSX communication events:
//    SIMCONNECT_RECV_ID_EVENT
//    SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE
//    SIMCONNECT_RECV_ID_QUIT
//    SIMCONNECT_RECV_ID_OPEN
//    SIMCONNECT_RECV_ID_NULL
//    SIMCONNECT_RECV_ID_EXCEPTION
//    SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE
//    SIMCONNECT_RECV_ID_EVENT_FRAME
//    SIMCONNECT_RECV_ID_SIMOBJECT_DATA
//    SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
//    SIMCONNECT_RECV_ID_SYSTEM_STATE

void setUpSimObjects()
{
  HRESULT hr;

  gInit.Latitude = 47.897044;    //  nuggs Flyig M, Pottsboro, Texas, US
  gInit.Longitude = -122.285317;
  gInit.Altitude = 610.230361;

  gInit.Latitude = 35.4371681211; // GMTA_Alhoceima Marocco
  gInit.Longitude = -3.765129877;
  gInit.Altitude = 970.788;

  //gInit.Latitude   = 42.3337255977; // LECD_Cerdenya Spain
  //gInit.Longitude  = 1.6670323867;
  //gInit.Altitude   = 2306.3835; 

  gInit.Pitch = 0.0;
  gInit.Bank = 0.0;
  gInit.Heading = 0.0;
  gInit.OnGround = 0;
  gInit.Airspeed = 0;

  gVOR.Flags = 0xF;
  gVOR.Altitude = 610.230361;
  gVOR.Latitude = 47.897044;
  gVOR.Longitude = -122.285317;


  hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_INIT_POS, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(gInit), &gInit);

} // End of setUpSimObjects



void CALLBACK MyDispatchProcSD(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
  HRESULT hr;

  switch (pData->dwID) {
  case SIMCONNECT_RECV_ID_EVENT: {
    SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;

    switch (evt->uEventID) {
    case EVENT_SIM_START: {
      printf ("SimAI: Send first events...");

      setUpSimObjects();

      hr = SimConnect_UnsubscribeFromSystemEvent(ghSimConnect, EVENT_SIM_START);

      hr = SimConnect_TransmitClientEvent(ghSimConnect, SIMCONNECT_OBJECT_ID_USER, KEY_FREEZE_LATITUDE_LONGITUDE_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST,
                                          SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
      hr = SimConnect_TransmitClientEvent(ghSimConnect, SIMCONNECT_OBJECT_ID_USER, KEY_FREEZE_ATTITUDE_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST,
                                          SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
      hr = SimConnect_TransmitClientEvent(ghSimConnect, SIMCONNECT_OBJECT_ID_USER, KEY_FREEZE_ALTITUDE_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST,
                                          SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
    }
    break;
    default:
      printf("SimAI: Received:%d\n", evt->uEventID);
      break;
    }
  }
  break;
  case SIMCONNECT_RECV_ID_QUIT:
    gbQuit = TRUE;
    printf("SimAI: Quit Received");
  break;
  case SIMCONNECT_RECV_ID_OPEN:
    printf("SimAI: Receive Open");
    setUpSimObjects();
  break;
  default:
    printf("SimAI: Received:%d\n", pData->dwID);
  break;
  }
} // End of MyDispatchProcSD



void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
{
  HRESULT hr;

  switch (pData->dwID) {
    case SIMCONNECT_RECV_ID_EVENT: {
      SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT *)pData;
      switch (evt->uEventID) {
        case EVENT_SIM_START:
          // Now the sim is running, request information on the user aircraft
          hr = SimConnect_RequestDataOnSimObject(ghSimConnect, REQUEST_2, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND); // refresh each 1sec
          //hr = SimConnect_RequestDataOnSimObject(ghSimConnect, REQUEST_2, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);//refresh 6Hz
          //hr = SimConnect_RequestDataOnSimObject(ghSimConnect, REQUEST_2, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);//refresh each 4sec
        break;
        default:
        break;
      }
    }
    break;
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE: {
      SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *)pData;
    }
    break;
    case SIMCONNECT_RECV_ID_QUIT:
      gbQuit = TRUE;
      printf ("SimAI: Receive Quit\n");
    break;
    case SIMCONNECT_RECV_ID_OPEN: {
      HRESULT hr;

      printf ("SimAI: Receive Open\n");

      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Kohlsman setting hg", "inHg");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Altitude", "feet");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Latitude", "degrees");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Longitude", "degrees");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Vertical Speed", "Feet per second");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "GROUND VELOCITY", "Knots");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "PLANE HEADING DEGREES TRUE", "degrees");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "PLANE PITCH DEGREES", "degrees");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "PLANE BANK DEGREES", "degrees");
      //hr = SimConnect_AddToDataDefinition (ghSimConnect,DEFINITION_1,"PLANE BANK DEGREES TRUE",   "radians");
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "VELOCITY BODY X", "m/s", SIMCONNECT_DATATYPE_FLOAT64);
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "VELOCITY BODY Y", "m/s", SIMCONNECT_DATATYPE_FLOAT64);
      hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "VELOCITY BODY Z", "m/s", SIMCONNECT_DATATYPE_FLOAT64);
    }
    break;
    case SIMCONNECT_RECV_ID_NULL:
      printf ("SimAI: NULL ID Received\n");
    break;
    case SIMCONNECT_RECV_ID_EXCEPTION: {
      SIMCONNECT_RECV_EXCEPTION *Exeption = (SIMCONNECT_RECV_EXCEPTION *)pData;
    }
    break;
    case SIMCONNECT_RECV_ID_EVENT_FRAME: {
      SIMCONNECT_RECV_EVENT_FRAME *Evnt = (SIMCONNECT_RECV_EVENT_FRAME *)pData;
    }
    break;
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
      SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA *)pData;

      switch (pObjData->dwRequestID) {
        case REQUEST_2: {
          PLANE_NAV *pS = (PLANE_NAV *)&pObjData->dwData;
          DWORD ObjectID = pObjData->dwObjectID;
          double deg;

          if (strlen(&pS->title[0]) != 0) { // security check
            deg = pS->heading*180.0 / PI;
            printf ("Lat=%f Lon=%f Alt=%f Vrt=%f Speed=%f Head=%f ID=%d\n", pS->latitude,pS->longitude,pS->altitude,pS->vertspeed,pS->gspeed,deg,ObjectID);
            printf ("Head=%f Pitch=%f Bank=%f\n", pS->heading, pS->pitch, pS->bank);
          }
        }
        break;
      }
    }
    break;
    case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID: {
      SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *ObjID = (SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *)pData;
    }
    break;
    case SIMCONNECT_RECV_ID_SYSTEM_STATE: {
      SIMCONNECT_RECV_SYSTEM_STATE *SysStat = (SIMCONNECT_RECV_SYSTEM_STATE *)pData;
    }
    break;
    default:
      printf ("SimAI: Received: %d\n", pData->dwID);
    break;
  }
} // End of MyDispatchProcRD


void SetAircraftPos(double Alt, double Lat, double Lon, double Pitch, double Bank, double Heading)
{
  HRESULT hr;

  printf ("Lat: %lf Lon: %lf Alt: %lf Heading: %lf Pitch: %lf\n", Lat, Lon, Alt, Heading, Pitch);

  hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Alt),     &Alt);
  hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_2, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Lat),     &Lat);
  hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_3, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Lon),     &Lon);
  hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_4, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Pitch),   &Pitch);
  hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_5, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Bank),    &Bank);
  hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_6, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Heading), &Heading);

} // SetAircraftPos


// SimTest Main procedure
int main()
{
  HRESULT hr;

  printf("SimAI: Connecting to the FSX simulator...\n");
  while (!gbQuit) {
    if ((hr = SimConnect_Open(&ghSimConnect, ("ExternalSim"), NULL, 0, NULL, 0)) == S_OK) {   // Connetced to the FSX Flight Simulator?
      printf ("SimAI: Connection to the FSX simulator!\n");
      break;
    }
    Sleep(1000);  // Wait for FSX connection
  }
  hr = SimConnect_SubscribeToSystemEvent(ghSimConnect, EVENT_SIM_START, "1sec");              // Connection refresh time delay 1 second

  while (!gbQuit) {
    SimConnect_CallDispatch(ghSimConnect, MyDispatchProcRD, NULL);                              // Dispatch FSX data connection messages
    SimConnect_CallDispatch(ghSimConnect, MyDispatchProcSD, NULL);                              // Dispatch FSX data connection messages
    SetAircraftPos(gVOR.Altitude,gVOR.Latitude,gVOR.Longitude,gInit.Pitch, gInit.Bank, gInit.Heading);
    Sleep(1);
  }
  hr = SimConnect_Close(ghSimConnect);

  return 0;
}

