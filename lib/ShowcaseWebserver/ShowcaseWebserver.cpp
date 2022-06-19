#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include "ShowcaseWebserver.h"

#include "FRTTransceiver.h"
#include "Global_FRTTSetup.h"
#include "SmartDoorUtilities.h"
#include "Global_WebPage.h"
#include "SensorUtilities.h"

/*! \brief Creates server object*/
WebServer server(80);

/*! \brief SSID for the Accesspoint */
const char *ssidAP = "--------";
/*! \brief Password for the Accesspoint */
const char *passAP = "--------";

#define WEBSERVER_PRINTSUMMARY_PAIRS (1U)
 
const char * SMARTDOOR_1 = "<!doctype html> <html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>DAPI FRTTransceiver Showcase</title> <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor\" crossorigin=\"anonymous\"> <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script> </head> <body style=\"background-color: #0F0F0F;\"> <form type=\"submit\" action=\"/\" method=\"get\" id=\"formToSmartDoor\" name=\"formToSmartDoor\"></form> <!-- FORM-GET SmartDoor webpage --> <form type=\"submit\" action=\"/printSummary\" method=\"get\" id=\"formToSummary\" name=\"formToSummary\"></form> <!-- FORM PRINT SUMMARY --> <form type=\"submit\" action=\"/sensor\" method=\"get\" id=\"formToSensor\" name=\"formToSensor\"></form> <!-- FORM-GET Sensor webpage --> <nav class=\"navbar navbar-expand-lg navbar-dark bg-dark\"> <div class=\"container-fluid\"> <a class=\"navbar-brand\" onclick=\"document.getElementById('formToSmartDoor').submit()\" >FRTTransceiver Showcase</a> <button class=\"navbar-toggler\" type=\"button\" data-bs-toggle=\"collapse\" data-bs-target=\"#navbarNavDropdown\" aria-controls=\"navbarNavDropdown\" aria-expanded=\"false\" aria-label=\"Toggle navigation\"> <span class=\"navbar-toggler-icon\"></span> </button> <div class=\"collapse navbar-collapse\" id=\"navbarNavDropdown\"> <ul class=\"navbar-nav\"> <li class=\"nav-item\"> <a class=\"nav-link\" onclick=\"document.getElementById('formToSmartDoor').submit()\" >SmartDoor</a> </li> <li class=\"nav-item\"> <a class=\"nav-link\" onclick=\"document.getElementById('formToSensor').submit()\" >Sensor</a> </li> <li class=\"nav-item dropdown\"> <a class=\"nav-link dropdown-toggle\" id=\"navbarDropdownMenuLink\" role=\"button\" data-bs-toggle=\"dropdown\" aria-expanded=\"false\"> Summary </a> <ul class=\"dropdown-menu\" aria-labelledby=\"navbarDropdownMenuLink\"> <li><button type=\"submit\" class=\"dropdown-item\" name=\"task\" value=\"webserver\" form=\"formToSummary\">Webserver</button></li> <li><button type=\"submit\" class=\"dropdown-item\" name=\"task\" value=\"sensor\" form=\"formToSummary\">Sensor</button></li> <li><button type=\"submit\" class=\"dropdown-item\" name=\"task\" value=\"smartDoor\" form=\"formToSummary\">SmartDoor</button></li> </ul> </li> </ul> </div> </div> </nav> <div class=\"container-fluid mx-auto mt-4 min-vh-100\"> <div class=\"row justify-content-center\"> <div class=\"col-7 shadow bg-secondary text-white rounded-4 border border-2 border-secondary p-3 m-3\"> <p class=\"col-12 h3 p-2 m-2\">Doorbell</p> <div class=\"col-12 shadow-lgs rounded-4 m-2 p-4\" style=\"background-color: #8F9BA6;\"> <p class=\"lead fs-6 p-2\"> Press buttons to turn on the 10 second silent ring. <br> Color of button will be the same as on the led strip </p> </div> <form action=\"/ring\" id=\"formRingtype\" method=\"get\"></form> <!-- Das ist das Form für die Türklingel --> <div class=\"col-12 shadow-lg rounded-4 m-2 p-4\" style=\"background-color: #8F9BA6;\"> <div class=\"row\"> <div class=\"col-6\"> <p class=\"lead fs-6 p-2\"> Select a ringtype: </p> </div> <div class=\"col-6\"> <select name=\"ringtype\" class=\"form-select form-select-sm p-2 mt-2 bg-secondary text-white border-secondary\" form=\"formRingtype\" aria-label=\".form-select-lg\"> <!-- ###################### HERE SPLIT ####################################-->";
const char * SMARTDOOR_2 = "</select> </div> </div> </div> <div class=\"col-12 shadow-lg rounded-4 m-2 p-4\" style=\"background-color: #8F9BA6;\"> <div class=\"row\"> <div class=\"col-6 mb-3\"><button type=\"submit\" name=\"ccode\" value=\"0\" form=\"formRingtype\" class=\"btn bg-primary text-white\">Hast du Lust auf einen Spaziergang?</button></div> <div class=\"col-6 mb-3\"><button type=\"submit\" name=\"ccode\" value=\"1\" form=\"formRingtype\" class=\"btn btn-danger\">Muss dich kurz etwas Fragen?<br>Kannst du rauskommen?</button></div> <div class=\"col-6 mb-3\"><button type=\"submit\" name=\"ccode\" value=\"2\" form=\"formRingtype\" class=\"btn btn-success\">Gehe einkaufen, brauchst was?</button></div> <div class=\"col-6\"><button type=\"submit\" name=\"ccode\" value=\"3\" form=\"formRingtype\" class=\"btn btn-warning\">Bist du noch wach?</button></div> </div> </div> </div> <form action=\"/mailbox\" method=\"get\" id=\"formMailbox\"></form> <!-- Das ist das Form für die Mailbox--> <div class=\"col-7 shadow bg-secondary text-white rounded-4 border border-2 border-secondary p-3 m-3\"> <p class=\"col-12 h3 p-2 m-2\">Mailbox</p> <div class=\"col-12 shadow-lg rounded-4 m-2 p-4\" style=\"background-color: #8F9BA6;\"> <!-- ###################### HERE SPLIT ####################################-->";
const char * SMARTDOOR_3 = "</div> </div> </div> </div> <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2\" crossorigin=\"anonymous\"></script> </body> </html>";

const char * SENSOR_1 = "<!doctype html> <html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>DAPI FRTTransceiver Showcase</title> <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor\" crossorigin=\"anonymous\"> </head> <body style=\"background-color: #0F0F0F;\"> <form type=\"submit\" action=\"/\" method=\"get\" id=\"formToSmartDoor\" name=\"formToSmartDoor\"></form> <!-- FORM-GET SmartDoor webpage --> <form type=\"submit\" action=\"/printSummary\" method=\"get\" id=\"formToSummary\" name=\"formToSummary\"></form> <!-- FORM PRINT SUMMARY --> <form type=\"submit\" action=\"/sensor\" method=\"get\" id=\"formToSensor\" name=\"formToSensor\"></form> <!-- FORM-GET Sensor webpage --> <nav class=\"navbar navbar-expand-lg navbar-dark bg-dark\"> <div class=\"container-fluid\"> <a class=\"navbar-brand\" onclick=\"document.getElementById('formToSmartDoor').submit()\" >FRTTransceiver Showcase</a> <button class=\"navbar-toggler\" type=\"button\" data-bs-toggle=\"collapse\" data-bs-target=\"#navbarNavDropdown\" aria-controls=\"navbarNavDropdown\" aria-expanded=\"false\" aria-label=\"Toggle navigation\"> <span class=\"navbar-toggler-icon\"></span> </button> <div class=\"collapse navbar-collapse\" id=\"navbarNavDropdown\"> <ul class=\"navbar-nav\"> <li class=\"nav-item\"> <a class=\"nav-link\" onclick=\"document.getElementById('formToSmartDoor').submit()\" >SmartDoor</a> </li> <li class=\"nav-item\"> <a class=\"nav-link\" onclick=\"document.getElementById('formToSensor').submit()\" >Sensor</a> </li> <li class=\"nav-item dropdown\"> <a class=\"nav-link dropdown-toggle\" id=\"navbarDropdownMenuLink\" role=\"button\" data-bs-toggle=\"dropdown\" aria-expanded=\"false\"> Summary </a> <ul class=\"dropdown-menu\" aria-labelledby=\"navbarDropdownMenuLink\"> <li><button type=\"submit\" class=\"dropdown-item\" name=\"task\" value=\"webserver\" form=\"formToSummary\">Webserver</button></li> <li><button type=\"submit\" class=\"dropdown-item\" name=\"task\" value=\"sensor\" form=\"formToSummary\">Sensor</button></li> <li><button type=\"submit\" class=\"dropdown-item\" name=\"task\" value=\"smartDoor\" form=\"formToSummary\">SmartDoor</button></li> </ul> </li> </ul> </div> </div> </nav> <div class=\"container-fluid mx-auto mt-4 min-vh-100\"> <div class=\"row justify-content-center\"> <div class=\"col-7 shadow bg-secondary text-white rounded-4 border border-2 border-secondary p-3 m-3\"> <p class=\"col-12 h3 p-2 m-2\">Sensor data measurment</p> <div class=\"col-12 shadow-lgs rounded-4 m-2 p-4\" style=\"background-color: #8F9BA6;\"> <p class=\"lead fs-6 p-2\"> Press 'Measure' to get new sensor data. <br> Press 'Clear' to flush sensor data buffer. </p> </div> <form action=\"/measure\" id=\"formLightSensor\" method=\"get\"></form> <!-- Das ist für lightsensor --> <form action=\"/measure\" id=\"formSensorUS\" method=\"get\"></form> <!-- Das ist für ultrasonic sensor --> <div class=\"col-12 shadow-lg rounded-4 m-2 p-4\" style=\"background-color: #8F9BA6;\"> <div class=\"row justify-content-center\"> <div class=\"col-5 m-3 p-3\"> <div class=\"col-12 p-3 mb-3 text-center border rounded-4 text-danger \">Lightsensor</div> <!--<div class=\"col-12 p-3 mb-3 text-center border rounded-4 \"></div>-->";
const char * SENSOR_2 = "<div class=\"col-12 d-grid gap-2\"> <button name=\"action\" type=\"submit\" value=\"measureLS\" form=\"formLightSensor\" class=\"btn btn-success btn-sm\">Measure new sensor data</button> <button name=\"action\" type=\"submit\" value=\"clearLS\" form=\"formLightSensor\" class=\"btn btn-danger btn-sm\">Clear buffer</button> </div> </div> <div class=\"col-5 m-3 p-3\"> <div class=\"col-12 p-3 mb-3 text-center border rounded-4 text-danger\">Ultrasonicsensor</div>";
const char * SENSOR_3 = "<div class=\"col-12 d-grid gap-2\"> <button name=\"action\" type=\"submit\" value=\"measureUS\" form=\"formSensorUS\" class=\"btn btn-success btn-sm\">Measure new sensor data</button> <button name=\"action\" type=\"submit\" value=\"clearUS\" form=\"formSensorUS\" class=\"btn btn-danger btn-sm\">Clear buffer</button> </div> </div> </div> </div> </div> </div> </div> <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2\" crossorigin=\"anonymous\"></script> </body> </html>";

void setupWifi();
void printKeyVal(std::map<String,String> &);
String prepareSmartDoorWebpage(bool,bool);
String prepareSensorWebpage();
void copyKeyVals(std::map<String,String> &);

std::map<String,String> keyValPairs;

FRTT::FRTTransceiver comm(WEBSERVER_TASKHANDLE,2);

bool bSmartdoorWaveClicked = true;
bool bMailboxOpen = false;
bool bErrorOccured = false;
SensorProject::SensorData sensordata;

void WebserverTask(void * ptr)
{	
	while(SMARTDOOR_MAIN_TASKHANDLE == NULL || SENSOR_MAIN_TASKHANDLE == NULL) vTaskDelay(pdMS_TO_TICKS(1));

	delay(1000); // to stabilize logging output
    setupWifi();

	comm.addDataAllocateCallback(dataAllocator);
	comm.addDataFreeCallback(deleter);

	comm.addCommPartner(SMARTDOOR_MAIN_TASKHANDLE,SMARTDOOR_TX_QUEUE,1,SMARTDOOR_TX_SEM,SMARTDOOR_RX_QUEUE,1,SMARTDOOR_RX_SEM,"SMARTDOOR");
	comm.addCommPartner(SENSOR_MAIN_TASKHANDLE,SENSOR_TX_QUEUE,1,SENSOR_TX_SEM,SENSOR_RX_QUEUE,1,SENSOR_RX_SEM,"SENSOR");

    server.on(PATH_ROOT,[](){
		server.send(200,"text/html",prepareSmartDoorWebpage(bSmartdoorWaveClicked,bMailboxOpen));
	});

	server.on(PATH_PRINT_SUMMARY,[](){
		copyKeyVals(keyValPairs);

		if(SmartDoorProject::parsePrintSummarySig(keyValPairs))
		{
			comm.NotifyBasic(SMARTDOOR_MAIN_TASKHANDLE);
		}
		else if(SensorProject::parsePrintSummarySig(keyValPairs))
		{
			comm.NotifyBasic(SENSOR_MAIN_TASKHANDLE);
		}
		else if(parsePrintSummarySig(keyValPairs))
		{
			comm.printCommunicationsSummary();
		}
		else
		{
			/* Errror */
			printf("Error occured...\n\n");
		}

		server.send(200,"text/html",prepareSmartDoorWebpage(bSmartdoorWaveClicked,bMailboxOpen));
	});

	server.on(PATH_RING,[](){
		copyKeyVals(keyValPairs);

		comm.writeToQueue(SMARTDOOR_MAIN_TASKHANDLE,(uint8_t)Datatypes::eKEYVAL,&keyValPairs,FRTTRANSCEIVER_WAITMAX,FRTTRANSCEIVER_WAITMAX,(uint32_t)SmartDoorProject::eSigErr::eRING);
		comm.readFromQueue(SMARTDOOR_MAIN_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true,FRTTRANSCEIVER_WAITMAX,FRTTRANSCEIVER_WAITMAX);

		const FRTT::FRTTTempDataContainer * t = comm.getNewestBufferedDataFrom(SMARTDOOR_MAIN_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true);
		
		if(t != NULL)
		{
			if((Datatypes)t->u8DataType == Datatypes::eDUMMYDATA){
				switch((SmartDoorProject::eSigErr)t->u32AdditionalData)
				{
					case SmartDoorProject::eSigErr::eBLINKSUCCEEDED:
						bSmartdoorWaveClicked = false;
						break;
					case SmartDoorProject::eSigErr::eWAVESUCCEEDED:
						bSmartdoorWaveClicked = true;
						break;
					case SmartDoorProject::eSigErr::eERROR:
						bErrorOccured = true;
						printf("Error occured..\n\n");
						break;
				}
			}
		}
		
        server.send(200,"text/html",prepareSmartDoorWebpage(bSmartdoorWaveClicked,bMailboxOpen));
	});

	server.on(PATH_MAILBOX,[](){
		copyKeyVals(keyValPairs);

		comm.writeToQueue(SMARTDOOR_MAIN_TASKHANDLE,(uint8_t)Datatypes::eKEYVAL,&keyValPairs,100,100,(uint32_t)SmartDoorProject::eSigErr::eMAILBOX);
		comm.readFromQueue(SMARTDOOR_MAIN_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true,FRTTRANSCEIVER_WAITMAX,FRTTRANSCEIVER_WAITMAX);

		const FRTT::FRTTTempDataContainer * t = comm.getNewestBufferedDataFrom(SMARTDOOR_MAIN_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true);
		
		if(t != NULL)
		{	
			if((Datatypes)t->u8DataType == Datatypes::eDUMMYDATA)
			{
				switch((SmartDoorProject::eSigErr)t->u32AdditionalData)
				{
					case SmartDoorProject::eSigErr::eCLOSESUCCEEDED:
						bMailboxOpen = false;
						break;
					case SmartDoorProject::eSigErr::eOPENSUCCEEDED:
						bMailboxOpen = true;
						break;
					case SmartDoorProject::eSigErr::eERROR:
						bErrorOccured = true;
						printf("Error occured..\n\n");
						break;
				}
			}
		}

		server.send(200,"text/html",prepareSmartDoorWebpage(bSmartdoorWaveClicked,bMailboxOpen));
	});
	
	
	server.on(PATH_SENSOR_MEASURE,[](){
		copyKeyVals(keyValPairs);

		comm.writeToQueue(SENSOR_MAIN_TASKHANDLE,(uint8_t)Datatypes::eKEYVAL,&keyValPairs,100,100,0);
		comm.readFromQueue(SENSOR_MAIN_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true,FRTTRANSCEIVER_WAITMAX,FRTTRANSCEIVER_WAITMAX);
		
		const FRTT::FRTTTempDataContainer * t = comm.getNewestBufferedDataFrom(SENSOR_MAIN_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true);

		if(t != NULL)
		{	
			if((Datatypes)t->u8DataType == Datatypes::eSENSORDATA)
			{
				SensorProject::eSigErr signal = (SensorProject::eSigErr)t->u32AdditionalData;
				if(signal == SensorProject::eSigErr::eERROR)
				{
					printf("Error occured..\n\n");
				}
				sensordata = *((SensorProject::SensorData *)t->data);
			}
		}

		server.send(200,"text/html",prepareSensorWebpage());
		
	});

	server.on(PATH_SENSOR,[](){
		server.send(200,"text/html",prepareSensorWebpage());
	});

	server.begin();

    for(;;)
    {
        server.handleClient();
	    delay(2);
    }
}

void copyKeyVals(std::map<String,String> & keyValPairs)
{
	keyValPairs.clear();
	for(int i = 0; i < server.args();i++)
	{
		keyValPairs.insert({server.argName(i),server.arg(i)});
	}
}

void printKeyVal(std::map<String,String> & keyValPairs)
{
	std::map<String, String>::iterator iterStart = keyValPairs.begin(), iterEnd = keyValPairs.end();

	while(iterStart != iterEnd)
	{   
		String key = iterStart->first;
		String value = iterStart->second;

		printf("KEY: %s - VAL: %s\n",key,value);
		
		iterStart++;
	}
}
bool parsePrintSummarySig(std::map<String,String> & keyValPairs)
{
	std::map<String, String>::iterator iterStart = keyValPairs.begin(), iterEnd = keyValPairs.end();

	if(iterStart == iterEnd || keyValPairs.size() < WEBSERVER_PRINTSUMMARY_PAIRS || keyValPairs.size() > WEBSERVER_PRINTSUMMARY_PAIRS) return false;

	while(iterStart != iterEnd)
	{   
		String key = iterStart->first;
		String value = iterStart->second;

		if(key == WEB_KEY_TASK && value == WEB_VAL_WEBSERVER)
		{
			return true;
		}
		
		iterStart++;
	}
        
    return false;
}
String prepareSmartDoorWebpage(bool bSmartdoorWavedClicked,bool bMailboxOpen)
{
	String s("");

	s += SMARTDOOR_1;

	
	if(bSmartdoorWavedClicked)
	{
		s += "<option value=\"wave\" selected>Wave</option> <option value=\"blink\">Blink</option>";
	}
	else
	{
		s += "<option value=\"wave\">Wave</option> <option value=\"blink\" selected>Blink</option>";
	}
	s += SMARTDOOR_2;

	if(bMailboxOpen)
	{
		s += "<div class=\"d-grid gap-2\"> <button name=\"action\" type=\"submit\" value=\"close\" form=\"formMailbox\" class=\"btn btn-danger\">Please close the mailbox and press this button to lock it</button> </div>";
	}
	else
	{
		s += "<div class=\"d-grid gap-2\"> <button name=\"action\" type=\"submit\" value=\"open\" form=\"formMailbox\" class=\"btn btn-success\">Press to open the mailbox</button> </div>";
	}
	s += SMARTDOOR_3;

	return s;
}

String prepareSensorWebpage()
{
	String s("");

	s += SENSOR_1;

	for(uint8_t u8I = 0;u8I < sensordata.u8LightSensor;u8I++)
	{
		s += "<div class=\"col-12 p-3 mb-3 text-center border rounded-4 \">";
		s += "Lightsensor[";
		if(sensordata.lightSensor[u8I] < 50)
		{
			s += "Dunkel]: ";
		}
		else if(sensordata.lightSensor[u8I] < 1000)
		{
			s += "Leicht hell]: ";
		}
		else if(sensordata.lightSensor[u8I] < 3200)
		{
			s += "Hell]: ";
		}
		else
		{
			s += "Sehr hell]: ";
		}
		s += sensordata.lightSensor[u8I];
		s += "</div>";
	}

	for(uint8_t u8I = sensordata.u8LightSensor; u8I < SENSOR_MAX_DATABUFFER;u8I++)
	{
		s += "<div class=\"col-12 p-3 mb-3 text-center border rounded-4 \">";
		s += "NO DATA";
		s += "</div>";
	}

	s += SENSOR_2;

	for(uint8_t u8I = 0;u8I < sensordata.u8BuffLenUS;u8I++)
	{
		s += "<div class=\"col-12 p-3 mb-3 text-center border rounded-4 \">";
		s += "Distance[in cm]: ";
		s += sensordata.ultrasonicSensordata[u8I];
		s += "</div>";
	}

	for(uint8_t u8I = sensordata.u8BuffLenUS; u8I < SENSOR_MAX_DATABUFFER;u8I++)
	{
		s += "<div class=\"col-12 p-3 mb-3 text-center border rounded-4 \">";
		s += "NO DATA";
		s += "</div>";
	}

	s += SENSOR_3;

	return s;

}


void setupWifi()
{
    // put your setup code here, to run once:
	WiFi.begin(ssidAP,passAP);
	while(WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		printf("Connecting to wifi...\n");
	}

	printf("Connected to wifi....\n");
	
}
