//------------------------------------------------------------------------------
//
//  SimConnect FaciliitesData sample
//  
//	Description:
//				Ctrl F1 displays the Get Facilities menu on the screen
//				Ctrl F2 displays the Subscribe to Faciliites menu on the screen
//------------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SimConnect.h"

static enum GROUP_ID 
{
    GROUP0,
};

static enum EVENT_ID 
{
    EVENT0,
    EVENT1,
    
    EVENT_MENU_1,
    EVENT_MENU_2,
};

static enum INPUT_ID 
{
    INPUT0,
};

static enum REQUEST_ID 
{
    REQUEST_0,
    REQUEST_1
};

#pragma pack(push, 1)
#pragma pack(pop)


bool gQuit = false;
HANDLE hSimConnect = NULL;

static const char szTitle[] = "Facilities Data";
static const char szHelp[] = "Press Ctrl-F1 for Get Facilities, Ctrl-F2 for Subscribe to Facilities";

static const char GetFacilitiesMenu[] = \
    "SimConnect Facilities Test\0" 
    "Choose which item:\0"
        "Get airport facilities\0"
        "Get waypoints\0"
        "Get NDB\0"
        "Get VOR\0"
        "Close menu\0";

static const char SubscribeFacilitiesMenu[] = \
    "SimConnect Facilities Test\0" 
    "Choose which item:\0"
        "Subscribe to airports\0"
        "Subscribe to waypoints \0"
        "Subscribe to NDB\0"
        "Subscribe to VOR\0"
        "Unsubscribe to airports\0"
        "Unsubscribe to waypoints \0"
        "Unsubscribe to NDB\0"
        "Unsubscribe to VOR\0"
        "Close menu\0";


char const * MenuText(SIMCONNECT_TEXT_RESULT result)
{
    switch (result)
    {
        case SIMCONNECT_TEXT_RESULT_DISPLAYED:
            return "Displayed";
            break;
        case SIMCONNECT_TEXT_RESULT_QUEUED:
            return "Queued";
            break;
        case SIMCONNECT_TEXT_RESULT_REMOVED:
            return "Removed from Queue";
            break;
        case SIMCONNECT_TEXT_RESULT_REPLACED:
            return "Replaced in Queue";
            break;
        case SIMCONNECT_TEXT_RESULT_TIMEOUT:
            return "Timeout";
            break;
        default:
            if (SIMCONNECT_TEXT_RESULT_MENU_SELECT_1 <= result && result <= SIMCONNECT_TEXT_RESULT_MENU_SELECT_10)
            {
                static char text[] = "10 Selected";
                text[1] = result - SIMCONNECT_TEXT_RESULT_MENU_SELECT_1 + '1';
                return text + (result != SIMCONNECT_TEXT_RESULT_MENU_SELECT_10);
            }
            return "<unknown>";
    }
}

// Dump facility list head
void Dump(SIMCONNECT_RECV_FACILITIES_LIST * pList)
{
    printf("RequestID: %d  dwArraySize: %d  dwEntryNumber: %d  dwOutOf: %d", 
           pList->dwRequestID, pList->dwArraySize, pList->dwEntryNumber, pList->dwOutOf);
}

// Dump various facility elements
void Dump(SIMCONNECT_DATA_FACILITY_AIRPORT * pFac)
{
    printf("  Icao: %s  Latitude: %lg  Longitude: %lg  Altitude: %lg", 
           pFac->Icao, pFac->Latitude, pFac->Longitude, pFac->Altitude);
}

void Dump(SIMCONNECT_DATA_FACILITY_WAYPOINT * pFac)
{
    Dump((SIMCONNECT_DATA_FACILITY_AIRPORT*) pFac);
    printf("  fMagVar: %g", pFac->fMagVar);
}

void Dump(SIMCONNECT_DATA_FACILITY_NDB * pFac)
{
    Dump((SIMCONNECT_DATA_FACILITY_WAYPOINT*) pFac);
    printf("  fFrequency: %d", pFac->fFrequency);
}

void Dump(SIMCONNECT_DATA_FACILITY_VOR * pFac)
{
    Dump((SIMCONNECT_DATA_FACILITY_NDB*) pFac);
    printf("  Flags: %x  fLocalizer: %f  GlideLat: %lg  GlideLon: %lg  GlideAlt: %lg  fGlideSlopeAngle: %f", 
           pFac->Flags, pFac->fLocalizer, pFac->GlideLat, pFac->GlideLon, pFac->GlideAlt,  pFac->fGlideSlopeAngle);
}

template <class T>
void DumpAll(T * pList)
{
    printf("\n");
    Dump(pList);
    for (unsigned i = 0; i < pList->dwArraySize; ++i)
    {
        printf("\n");
        Dump(pList->rgData + i);
    }
}

template <class T>
void RespondTo(T * pList, SIMCONNECT_FACILITY_LIST_TYPE type)
{
	DumpAll(pList);
}

void CALLBACK MySignalProc(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
    switch(pData->dwID)
    {
        HRESULT hr;

        case SIMCONNECT_RECV_ID_OPEN:
        {
            SIMCONNECT_RECV_OPEN *pOpen = (SIMCONNECT_RECV_OPEN*)pData;
            printf("Open: AppName=\"%s\"  AppVersion=%d.%d.%d.%d  SimConnectVersion=%d.%d.%d.%d\n", pOpen->szApplicationName,
                    pOpen->dwApplicationVersionMajor, pOpen->dwApplicationVersionMinor, pOpen->dwApplicationBuildMajor, pOpen->dwApplicationBuildMinor,
                    pOpen->dwSimConnectVersionMajor, pOpen->dwSimConnectVersionMinor, pOpen->dwSimConnectBuildMajor, pOpen->dwSimConnectBuildMinor
                  );
            break;
        }

        case SIMCONNECT_RECV_ID_EVENT:
        {
            SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;
            switch (evt->uEventID)
            {
                case EVENT0:
                {
                    printf("\nEVENT0: %d", evt->dwData);
                    SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, EVENT_MENU_1, sizeof(GetFacilitiesMenu), (void*)GetFacilitiesMenu);
                    break;
                }

                case EVENT1:
                {
                    printf("\nEVENT1: %d", evt->dwData);
                    SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, EVENT_MENU_2, sizeof(SubscribeFacilitiesMenu), (void*)SubscribeFacilitiesMenu);
                    break;
                }

                case EVENT_MENU_1:
                {
                    printf("\nMENU #1 Event: dwData=%d (%s)", evt->dwData, MenuText(SIMCONNECT_TEXT_RESULT(evt->dwData)));
                    unsigned const item = evt->dwData - SIMCONNECT_TEXT_RESULT_MENU_SELECT_1;
                    if (item < SIMCONNECT_FACILITY_LIST_TYPE_COUNT)
                    {
                        // Get the current cached list of airports, waypoints, etc, as the item indicates
                        hr =  SimConnect_RequestFacilitiesList(hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE(item), REQUEST_0);
                    }
                    break;
                }

                case EVENT_MENU_2:
                {
                    printf("\nMENU #2 Event: dwData=%d (%s)", evt->dwData, MenuText(SIMCONNECT_TEXT_RESULT(evt->dwData)));
                    unsigned const item = evt->dwData - SIMCONNECT_TEXT_RESULT_MENU_SELECT_1;
                    if (item < SIMCONNECT_FACILITY_LIST_TYPE_COUNT)
                    {
                        hr = SimConnect_SubscribeToFacilities(hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE(item), REQUEST_1);
                    }
                    else if (SIMCONNECT_FACILITY_LIST_TYPE_COUNT <= item && item < 2 * SIMCONNECT_FACILITY_LIST_TYPE_COUNT)
                    {
                        hr = SimConnect_UnsubscribeToFacilities(hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE(item - SIMCONNECT_FACILITY_LIST_TYPE_COUNT));
                    }
                    break;
                }

                default:
                    printf("\nSIMCONNECT_RECV_EVENT: 0x%08X 0x%08X 0x%X", evt->uEventID, evt->dwData, cbData);
                    break;
            }
            break;
        }

        case SIMCONNECT_RECV_ID_AIRPORT_LIST:
        {
            RespondTo((SIMCONNECT_RECV_AIRPORT_LIST*)pData, SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT);
            break;
        }

        case SIMCONNECT_RECV_ID_WAYPOINT_LIST:
        {
            RespondTo((SIMCONNECT_RECV_WAYPOINT_LIST*)pData, SIMCONNECT_FACILITY_LIST_TYPE_WAYPOINT);
            break;
        }

        case SIMCONNECT_RECV_ID_NDB_LIST:
        {
            RespondTo((SIMCONNECT_RECV_NDB_LIST*)pData, SIMCONNECT_FACILITY_LIST_TYPE_NDB);
            break;
        }

        case SIMCONNECT_RECV_ID_VOR_LIST:
        {
            RespondTo((SIMCONNECT_RECV_VOR_LIST*)pData, SIMCONNECT_FACILITY_LIST_TYPE_VOR);
            break;
        }

        case SIMCONNECT_RECV_ID_QUIT:
        {
            printf("\n***** SIMCONNECT_RECV_ID_QUIT *****\n");
            gQuit = true;
            break;
        }

        case SIMCONNECT_RECV_ID_EXCEPTION:
        {
            SIMCONNECT_RECV_EXCEPTION *except = (SIMCONNECT_RECV_EXCEPTION*)pData;
            printf("\n\n***** EXCEPTION=%d  SendID=%d  uOffset=%d  cbData=%d\n", except->dwException, except->dwSendID, except->dwIndex, cbData);
            break;
        }

        default:
            printf("\nUNKNOWN DATA RECEIVED: pData=%p cbData=%d\n", pData, cbData);
            break;
    }
}

//------------------------------------------------------------------------------
//  main
//------------------------------------------------------------------------------
int __cdecl main(int argc, char* argv[])
{
    HANDLE hEventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    if(hEventHandle == NULL)
    {
        printf("Error: Event creation failed!  Exiting");
        return GetLastError();
    }

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "FacilitiesData", NULL, 0, hEventHandle, 0)))
    {
       
		printf("\nConnected to Flight Simulator!");

	    HRESULT hr;

		hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT0);
		hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT1);

	    hr = SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP0, EVENT0, TRUE);
		hr = SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP0, EVENT1, TRUE);
		hr = SimConnect_SetNotificationGroupPriority(hSimConnect, GROUP0, SIMCONNECT_GROUP_PRIORITY_HIGHEST);

	    hr = SimConnect_MapInputEventToClientEvent(hSimConnect, INPUT0, "Ctrl+F1", EVENT0);
		hr = SimConnect_MapInputEventToClientEvent(hSimConnect, INPUT0, "Ctrl+F2", EVENT1);
		hr = SimConnect_SetInputGroupState(hSimConnect, INPUT0, SIMCONNECT_STATE_ON);

	    SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_PRINT_RED, 15, 0, sizeof(szTitle), (void*)szTitle);
		SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_PRINT_RED, 15, 0, sizeof(szHelp), (void*)szHelp);
    
	    while (!gQuit && ::WaitForSingleObject(hEventHandle, INFINITE) == WAIT_OBJECT_0)
		{
			hr = SimConnect_CallDispatch(hSimConnect, MySignalProc, NULL);
			if (FAILED(hr))
			{
				break;
			}
		} 

	    hr = SimConnect_Close(hSimConnect);
		CloseHandle(hEventHandle);
	}
    return 0; 
}

