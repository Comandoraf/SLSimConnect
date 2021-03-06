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
#include <io.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <fstream>
bool doReplay = false;
std::string labelText = "SimAI: Connecting to the FSX simulator...\n";
const char *ch = "SimTest";
LPSTR NazwaKlasy = const_cast<LPSTR>(ch);
MSG Komunikat;
const WORD ID_TIMER = 1;
HWND g_InfoLabel, g_Button;
HRESULT hr;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HANDLE ghSimConnect = NULL;
bool connected = false;
bool subscribed = false;
BOOL gbQuit = false;
bool reading = true;
struct currStats {
	double Alt;
	double Lon;
	double Lat;
	double Head;
	double Bank;
	double Pitch;
};

currStats CURRENT;
SIMCONNECT_DATA_INITPOSITION gInit;
//SIMCONNECT_DATA_FACILITY_VOR gVOR;
SIMCONNECT_DATA_WAYPOINT gVOR;
std::ofstream outFile;
std::ifstream inFile;
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

//void setUpSimObjects()
//{
//	HRESULT hr;
//
//	gInit.Latitude = 47.897044;    //  nuggs Flyig M, Pottsboro, Texas, US
//	gInit.Longitude = -122.285317;
//	gInit.Altitude = 610.230361;
//
//	gInit.Latitude = 35.4371681211; // GMTA_Alhoceima Marocco
//	gInit.Longitude = -3.765129877;
//	gInit.Altitude = 970.788;
//
//	//gInit.Latitude   = 42.3337255977; // LECD_Cerdenya Spain
//	//gInit.Longitude  = 1.6670323867;
//	//gInit.Altitude   = 2306.3835;
//
//	gInit.Pitch = 0.0;
//	gInit.Bank = 0.0;
//	gInit.Heading = 0.0;
//	gInit.OnGround = 0;
//	gInit.Airspeed = 0;
//
//	gVOR.Flags = 0xF;
//	gVOR.Altitude = 610.230361;
//	gVOR.Latitude = 47.897044;
//	gVOR.Longitude = -122.285317;
//
//
//	hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_INIT_POS, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(gInit), &gInit);
//
//} // End of setUpSimObjects



void CALLBACK MyDispatchProcSD(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
	HRESULT hr;

	switch (pData->dwID) {
	case SIMCONNECT_RECV_ID_EVENT: {
		SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;

		switch (evt->uEventID) {
		case EVENT_SIM_START: {
			printf("SimAI: Send first events...");

			//setUpSimObjects();

			hr = SimConnect_UnsubscribeFromSystemEvent(ghSimConnect, EVENT_SIM_START);

			hr = SimConnect_TransmitClientEvent(ghSimConnect, SIMCONNECT_OBJECT_ID_USER, KEY_FREEZE_LATITUDE_LONGITUDE_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST,
				SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			hr = SimConnect_TransmitClientEvent(ghSimConnect, SIMCONNECT_OBJECT_ID_USER, KEY_FREEZE_ATTITUDE_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST,
				SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			hr = SimConnect_TransmitClientEvent(ghSimConnect, SIMCONNECT_OBJECT_ID_USER, KEY_FREEZE_ALTITUDE_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST,
				SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			//SIMCONNECT_DATA_INITPOSITION Init;
			//Init.Altitude = CURRENT.Alt + 1000;
			//Init.Latitude = CURRENT.Lat;
			//Init.Longitude = CURRENT.Lon;
			//Init.Pitch = CURRENT.Pithc;
			//Init.Bank = CURRENT.Bank;
			//Init.Heading = CURRENT.Head;
			//Init.OnGround = 0;
			//Init.Airspeed = 60;
			//hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_INIT_POS, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Init), &Init);
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
		//setUpSimObjects();
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
		printf("SimAI: Receive Quit\n");
		break;
	case SIMCONNECT_RECV_ID_OPEN: {
		HRESULT hr;

		printf("SimAI: Receive Open\n");


		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Vertical Speed", "Feet per second");
		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "GROUND VELOCITY", "Knots");
		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "PLANE HEADING DEGREES TRUE", "degrees");
		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "PLANE PITCH DEGREES", "degrees");
		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "PLANE BANK DEGREES", "degrees");
		////hr = SimConnect_AddToDataDefinition (ghSimConnect,DEFINITION_1,"PLANE BANK DEGREES TRUE",   "radians");
		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "VELOCITY BODY X", "m/s", SIMCONNECT_DATATYPE_FLOAT64);
		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "VELOCITY BODY Y", "m/s", SIMCONNECT_DATATYPE_FLOAT64);
		//hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "VELOCITY BODY Z", "m/s", SIMCONNECT_DATATYPE_FLOAT64);
	}
								  break;
	case SIMCONNECT_RECV_ID_NULL:
		printf("SimAI: NULL ID Received\n");
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
			//double deg;

			if (strlen(&pS->title[0]) != 0) { // security check
											  //deg = pS->heading*180.0 / PI;
				CURRENT.Alt = pS->altitude;
				//CURRENT.Bank = pS->bank;
				//CURRENT.Head = pS->heading;
				CURRENT.Lat = pS->latitude;
				CURRENT.Lon = pS->longitude;
				//CURRENT.Pithc = pS->pitch;
				std::string rawData = std::to_string(pS->latitude)
					+ " " + std::to_string(pS->longitude)
					+ " " + std::to_string(pS->altitude)
					//+ " " + std::to_string(pS->vertspeed)
					//+ " " + std::to_string(pS->gspeed)
					//+ " " + std::to_string(deg)
					+" " + std::to_string(pS->heading)
					+ " " + std::to_string(pS->pitch)
					+ " " + std::to_string(pS->bank);
				outFile << rawData << std::endl;
				std::string info;
				info += "Lat=" + std::to_string(pS->latitude)
					+ "\nLon=" + std::to_string(pS->longitude)
					+ "\nAlt=" + std::to_string(pS->altitude)
					//+ "\nVrt=" + std::to_string(pS->vertspeed)
					//+ "\nSpeed=" + std::to_string(pS->gspeed)
					//+ "\nHead=" + std::to_string(deg)
					+"\nID=" + std::to_string(ObjectID);
				+"\nHead2=" + std::to_string(pS->heading)
					+ "\nPitch=" + std::to_string(pS->pitch)
					+ "\nBank=" + std::to_string(pS->bank);

				SendMessage(g_InfoLabel, WM_SETTEXT, NULL, (LPARAM)info.c_str());
				//printf ("Lat=%f Lon=%f Alt=%f Vrt=%f Speed=%f Head=%f ID=%d\n", pS->latitude,pS->longitude,pS->altitude,pS->vertspeed,pS->gspeed,deg,ObjectID);
				//printf ("Head=%f Pitch=%f Bank=%f\n", pS->heading, pS->pitch, pS->bank);
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
		/*std::string labelText = "SimAI: Received data\n";
		SendMessage(g_InfoLabel, WM_SETTEXT, NULL, (LPARAM)labelText.c_str());*/
		printf("SimAI: Received: %d\n", pData->dwID);
		break;
	}
} // End of MyDispatchProcRD


void SetAircraftPos(double Alt, double Lat, double Lon, double Pitch, double Bank, double Heading)
{
	HRESULT hr;

	/*labelText = "Lat: %lf Lon: %lf Alt: %lf Heading: %lf Pitch: %lf\n", Lat, Lon, Alt, Heading, Pitch;
	SendMessage(g_InfoLabel, WM_SETTEXT, NULL, (LPARAM)labelText.c_str());*/
	//printf("Lat: %lf Lon: %lf Alt: %lf Heading: %lf Pitch: %lf\n", Lat, Lon, Alt, Heading, Pitch);

	std::string send;
	send += "Lat=" + std::to_string(Lat)
		+ "\nLon=" + std::to_string(Lon)
		+ "\nAlt=" + std::to_string(Alt);

	SendMessage(g_InfoLabel, WM_SETTEXT, NULL, (LPARAM)send.c_str());

	hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_2, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Alt), &Alt);
	hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_3, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Lat), &Lat);
	hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_4, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Lon), &Lon);
	hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_5, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Pitch), &Pitch);
	hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_6, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Bank), &Bank);
	hr = SimConnect_SetDataOnSimObject(ghSimConnect, DEFINITION_8, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(Heading), &Heading);

} // SetAircraftPos


  // SimTest Main procedure
int Start()
{
	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	// WYPEŁNIANIE STRUKTURY
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = NazwaKlasy;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// REJESTROWANIE KLASY OKNA
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Blad rejestracji okna", "Niestety...",
			MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	// TWORZENIE OKNA
	HWND hwnd;

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, NazwaKlasy, "Okno", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
	g_InfoLabel = CreateWindowEx(0, "STATIC", "Przycisk", WS_CHILD | WS_VISIBLE,
		0, 0, 400, 480, hwnd, NULL, hInstance, NULL);
	g_Button = CreateWindowEx(0, "BUTTON", "Replay", WS_CHILD | WS_VISIBLE,
		450, 400, 300, 30, hwnd, NULL, hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Brak okna", "error", MB_ICONEXCLAMATION);
		return 1;
	}


	ShowWindow(hwnd, nCmdShow); // Pokaż okienko...
	UpdateWindow(hwnd);


	labelText = "SimAI: Connecting to the FSX simulator...\n";
	printf(labelText.c_str());
	SendMessage(g_InfoLabel, WM_SETTEXT, NULL, (LPARAM)labelText.c_str());
	//printf("SimAI: Connecting to the FSX simulator...\n");

	// Connection refresh time delay 1 second
	if (SetTimer(hwnd, ID_TIMER, 100, NULL) == 0)
		MessageBox(hwnd, "Nie można utworzyć timera!", "error", MB_ICONSTOP);


	//Start();
	// Pętla komunikatów
	while (GetMessage(&Komunikat, NULL, 0, 0))
	{
		TranslateMessage(&Komunikat);
		DispatchMessage(&Komunikat);
	}
	return Komunikat.wParam;
}

void SimConnectBase()
{
	if (reading)
	{
		if (!connected && !subscribed)
			if ((hr = SimConnect_Open(&ghSimConnect, ("ExternalSim"), NULL, 0, NULL, 0)) == S_OK) {   // Connetced to the FSX Flight Simulator?
				labelText = "SimAI: Connected!\n";
				SendMessage(g_InfoLabel, WM_SETTEXT, NULL, (LPARAM)labelText.c_str());
				connected = true;
			}
		if (connected && !subscribed)
		{
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Kohlsman setting hg", "inHg");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Altitude", "feet");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Latitude", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Longitude", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Pitch Degrees", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Bank Degrees", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Heading Degrees True", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_2, "Plane Altitude", "feet");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_3, "Plane Latitude", "degree");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_4, "Plane Longitude", "degree");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_5, "Plane Pitch Degrees", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_6, "Plane Bank Degrees", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_8, "Plane Heading Degrees True", "degrees");
			//hr = SimConnect_SubscribeToSystemEvent(ghSimConnect, EVENT_SIM_START, "1sec");
			subscribed = true;
			hr = SimConnect_SubscribeToSystemEvent(ghSimConnect, EVENT_SIM_START, "SimStart");
			/*hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_INIT_POS, "Initial Position", NULL, SIMCONNECT_DATATYPE_INITPOSITION);
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_1, "Plane Altitude", "feet");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_2, "Plane Latitude", "degree");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_3, "Plane Longitude", "degree");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_4, "Plane Pitch Degrees", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_5, "Plane Bank Degrees", "degrees");
			hr = SimConnect_AddToDataDefinition(ghSimConnect, DEFINITION_6, "Plane Heading Degrees True", "degrees");
			hr = SimConnect_MapClientEventToSimEvent(ghSimConnect, KEY_FREEZE_LATITUDE_LONGITUDE_SET, "FREEZE_LATITUDE_LONGITUDE_SET");
			hr = SimConnect_MapClientEventToSimEvent(ghSimConnect, KEY_FREEZE_LATITUDE_LONGITUDE_TOGGLE, "FREEZE_LATITUDE_LONGITUDE_TOGGLE");
			hr = SimConnect_MapClientEventToSimEvent(ghSimConnect, KEY_FREEZE_ALTITUDE_SET, "FREEZE_ALTITUDE_SET");
			hr = SimConnect_MapClientEventToSimEvent(ghSimConnect, KEY_FREEZE_ALTITUDE_TOGGLE, "FREEZE_ALTITUDE_TOGGLE");
			hr = SimConnect_MapClientEventToSimEvent(ghSimConnect, KEY_FREEZE_ATTITUDE_SET, "FREEZE_ATTITUDE_SET");
			hr = SimConnect_MapClientEventToSimEvent(ghSimConnect, KEY_FREEZE_ATTITUDE_TOGGLE, "FREEZE_ATTITUDE_TOGGLE");*/
			outFile.open("data.txt");
		}
		if (connected && subscribed)
		{
			SimConnect_CallDispatch(ghSimConnect, MyDispatchProcRD, NULL);                              // Dispatch FSX data connection messages
																										// Dispatch FSX data connection messages
																										//SetAircraftPos(gVOR.Altitude, gVOR.Latitude, gVOR.Longitude, gInit.Pitch, gInit.Bank, gInit.Heading);
		}
	}
}
void Replay()
{
	if (!inFile.eof())
	{
		inFile >> CURRENT.Lat;
		inFile >> CURRENT.Lon;
		inFile >> CURRENT.Alt;
		inFile >> CURRENT.Head;
		inFile >> CURRENT.Pitch;
		inFile >> CURRENT.Bank;

		SimConnect_CallDispatch(ghSimConnect, MyDispatchProcSD, NULL);
		SetAircraftPos(CURRENT.Alt, CURRENT.Lat, CURRENT.Lon, CURRENT.Pitch, CURRENT.Bank, CURRENT.Head);
	}
	else
	{
		inFile.close();
		std::string info("Koniec odtwarzania\n");
		SendMessage(g_InfoLabel, WM_SETTEXT, NULL, (LPARAM)info.c_str());
		doReplay = false;
	}
}
// OBSŁUGA ZDARZEŃ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		if (outFile.is_open())
			outFile.close();
		if (connected)
		{
			hr = SimConnect_Close(ghSimConnect);
			connected = false;
		}
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);
		if (outFile.is_open())
			outFile.close();
		if (connected)
		{
			hr = SimConnect_Close(ghSimConnect);
			connected = false;
		}
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		if ((HWND)lParam == g_Button)
		{
			if (outFile.is_open())
				outFile.close();
			inFile.open("data.txt");
			doReplay = true;
			if (SetTimer(hwnd, ID_TIMER, 400, NULL) == 0)
				MessageBox(hwnd, "Nie można utworzyć timera!", "error", MB_ICONSTOP);
			//reading = false;
			/*isReading = true;
			if (OutFile.is_open())
			OutFile.close();
			InFile.open("data.txt");*/
			//SetAircraftPos(gInit.Altitude, gInit.Latitude, gInit.Longitude, gInit.Pitch, gInit.Bank, gInit.Heading);
			//MessageBox(hwnd, "Nacisnąłeś przycisk!", "test", MB_ICONINFORMATION);


		}
	case WM_TIMER:
		if (doReplay)
			Replay();
		else
			SimConnectBase();
		UpdateWindow(hwnd);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}