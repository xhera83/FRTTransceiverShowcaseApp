#include <cstdint>
#include <map>
#include <WString.h>
#include "Adafruit_NeoPixel.h"
#include "FRTTransceiver.h"
#include "ESP32Servo.h"

#include "Global_WebPage.h"

#define SMARTDOOR_RINGTYPE_MS           (10U * 1000U)   /* 10s ringtype */
#define SMARTDOOR_RINGTYPE_PAIRS        (2U)            /* key-val pairs in url */
#define SMARTDOOR_MAILBOX_PAIRS         (1U)            /* key-val pairs in url */
#define SMARTDOOR_PRINTSUMMARY_PAIRS    (1U)            /* key-val pairs in url */ 

#define SMARTDOOR_COLORS          (4U)                  /* Amount of colors used*/
#define SMARTDOOR_STRIPLEDS       (20U)                  /* Amount of leds used on the strip*/
#define SMARTDOOR_PIN             (19U)                 /* PIN FOR ADAFRUIT NEOPIXEL PROTOCOL */
#define SMARTDOOR_LOCK_PIN        (26U)                 /* PIN FOR THE SERVO MOTOR*/

#define SMARTDOOR_CLR_BLUE        (1U)
#define SMARTDOOR_CLR_RED         (2U)
#define SMARTDOOR_CLR_GREEN       (3U)
#define SMARTDOOR_CLR_YELLOW      (4U)

namespace SmartDoorProject {

    enum class eSigErr : uint8_t{
        eNULL = 0,
        eRING,
        eMAILBOX,
        eWAVESUCCEEDED,
        eBLINKSUCCEEDED,
        eCLOSESUCCEEDED,
        eOPENSUCCEEDED,
        eERROR
    };


    enum class eMailbox {
        eOPEN = 0,
        eCLOSED
    };

    enum class eColors
    {
        eNOCLR = -1,
        eBLUE,
        eRED,
        eGREEN,
        eYELLOW
    };

    enum class eRingType
    {
        eNORTYPE = -1,
        eBLINK,
        eWAVE
    };

    struct SmartDoorRingControl
    {
        uint8_t u8CurrRingTime = 0;                 
        eColors currColor = eColors::eNOCLR;
        eRingType currRingType = eRingType::eNORTYPE;
    };


    struct SmartDoorMailboxControl
    {
        eMailbox status = eMailbox::eCLOSED;
    };

    extern FRTT::FRTTTaskHandle SMARTDOOR_LEDSTRIP_TASKHANDLE;
    extern Adafruit_NeoPixel strip;
    extern Servo servo;
    extern ESP32PWM pwm;

    bool parseWebdata(SmartDoorMailboxControl &,std::map<String,String> &);
    bool parseWebdata(SmartDoorRingControl &,std::map<String,String> &);
    bool parsePrintSummarySig(std::map<String,String> &);

    void LedstripTask(void *);
    void ServoLockTask(void *);
}

using SDMControl = SmartDoorProject::SmartDoorMailboxControl;
using SDRControl = SmartDoorProject::SmartDoorRingControl;

