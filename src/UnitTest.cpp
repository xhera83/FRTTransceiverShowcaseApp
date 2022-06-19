#include "UnitTest.h"
#include "SmartDoorUtilities.h"
#include "SensorUtilities.h"
#include "ShowcaseWebserver.h"
#include "Global_WebPage.h"

#define CAST (uint8_t)

test(printSummaryParsing)
{   
    /* Must return true if 'Summary > Webserver' was clicked on the website */
    std::map<String,String> m;

    assertEqual(parsePrintSummarySig(m),false);                             /* Empty map, too few key-val pairs */
    assertEqual(SmartDoorProject::parsePrintSummarySig(m),false);           /* Empty map, too few key-val pairs */
    assertEqual(SensorProject::parsePrintSummarySig(m),false);              /* Empty map, too few key-val pairs */

    m.insert({"abc","abc"});
    m.insert({"abc2","abc3"});

    assertEqual(parsePrintSummarySig(m),false);                             /* Too many key-val pairs */
    assertEqual(SmartDoorProject::parsePrintSummarySig(m),false);           /* Too many key-val pairs */
    assertEqual(SensorProject::parsePrintSummarySig(m),false);              /* Too many key-val pairs */


    m.clear(); m.insert({WEB_KEY_TASK,"abc"});                         
    assertEqual(parsePrintSummarySig(m),false);                             /* Key valid, value invalid */
    assertEqual(SmartDoorProject::parsePrintSummarySig(m),false);           /* Key valid, value invalid */
    assertEqual(SensorProject::parsePrintSummarySig(m),false);              /* Key valid, value invalid */

    m.clear(); m.insert({"abc",WEB_VAL_WEBSERVER});
    assertEqual(parsePrintSummarySig(m),false);                             /* Key invalid, value valid */

    m.clear(); m.insert({"abc",WEB_VAL_SMARTDOOR});
    assertEqual(SmartDoorProject::parsePrintSummarySig(m),false);           /* Key invalid, value valid */
    
    m.clear(); m.insert({"abc",WEB_VAL_SENSOR});
    assertEqual(SensorProject::parsePrintSummarySig(m),false);              /* Key invalid, value valid */


    m.clear(); m.insert({WEB_KEY_TASK,WEB_VAL_WEBSERVER});          
    assertEqual(parsePrintSummarySig(m),true);                              /* CORRECT */

     m.clear(); m.insert({WEB_KEY_TASK,WEB_VAL_SMARTDOOR});          
    assertEqual(SmartDoorProject::parsePrintSummarySig(m),true);            /* CORRECT */

     m.clear(); m.insert({WEB_KEY_TASK,WEB_VAL_SENSOR});          
    assertEqual(SensorProject::parsePrintSummarySig(m),true);               /* CORRECT */
}

test(smartdoor_parser_1)
{
    SDMControl mailcontrol; /* member variable "closed" at the begin*/
    std::map<String,String> m;

    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Too few arguments */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);    /* Not changed */

    m.insert({"abc","abc"});
    m.insert({"abc2","abc2"});

    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Too many arguments */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);    /* Not changed */

    m.clear();m.insert({"abc","abc"});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Both key and val wrong */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);    /* Not changed */
    
    m.clear();m.insert({WEB_KEY_ACTION,"abc"});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Key correct val wrong */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);    /* Not changed */

    m.clear();m.insert({"abc",WEB_VALUE_CLOSE});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Key incorrect val correct */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);    /* Not changed */

    m.clear();m.insert({"abc",WEB_VALUE_OPEN});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Key incorrect val correct */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);    /* Not changed */

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_CLOSE});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Everything correct but the door is on default closed. Value 'close' doesnt work */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);    /* Not changed */

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_OPEN});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),true);       /* Everything correct, door will be open now*/
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eOPEN);

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_OPEN});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),false);       /* Everything correct, but the door is already open */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eOPEN);

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_CLOSE});
    assertEqual(SmartDoorProject::parseWebdata(mailcontrol,m),true);       /* Everything correct, but the door is already open */
    assertEqual(CAST mailcontrol.status,CAST SmartDoorProject::eMailbox::eCLOSED);

}

test(smartdoor_parser_2)
{   
    /* KEY-VAL PAIRS: 2             */
    /* Colors possible: 0, 1, 2, 3  == blue, red, green, yellow */
    /* Ringtypes: BLINK, WAVE       */

    SDRControl ringcontrol;
    std::map<String,String> m;

    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);                                   /* Too few arguments */
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);              /* Not changed */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);                     /* Not changed */

    m.insert({"abc","abc"}); m.insert({"abc2","abc2"}); m.insert({"abc3","abc3"});
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);                                   /* Too many arguments */
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);              /* Not changed */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);                     /* Not changed */

    m.clear(); m.insert({WEB_KEY_RINGTYPE,"abc"}); m.insert({"abc2","abc2"});                           /* */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_BLINK}); m.insert({"abc2","abc2"});                   /* */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);              /* Stays even when one fails */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);                     /* Stays even when one fails */

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_WAVE}); m.insert({"abc2","abc2"});                    /* */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);              /* Stays even when one fails */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);                     /* Stays even when one fails */

    m.clear(); m.insert({WEB_KEY_COLORCODE,"asdas"}); m.insert({"abc2","abc2"});                        /* */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);              /* Stays even when one fails */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);                     /* Stays even when one fails */

    m.clear(); m.insert({WEB_KEY_COLORCODE,"0"}); m.insert({"abc2","abc2"});                            /* */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);              /* Stays even when one fails */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);                     /* Stays even when one fails */

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_BLINK}); m.insert({WEB_KEY_COLORCODE,"-1"});          /* wrong color */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eNORTYPE);              /* Stays even when one fails */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eNOCLR);                     /* Stays even when one fails */

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_BLINK}); m.insert({WEB_KEY_COLORCODE,"0"});           /* All good*/
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),true);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eBLINK);           
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eBLUE);                 

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_WAVE}); m.insert({WEB_KEY_COLORCODE,"4"});            /* wrong color */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),false);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eBLINK);                /* Stays even when one fails */
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eBLUE);                      /* Stays even when one fails */

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_WAVE}); m.insert({WEB_KEY_COLORCODE,"1"});            /* all good */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),true);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eWAVE);           
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eRED);

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_WAVE}); m.insert({WEB_KEY_COLORCODE,"2"});            /* all good */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),true);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eWAVE);           
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eGREEN);     

    m.clear(); m.insert({WEB_KEY_RINGTYPE,WEB_VAL_WAVE}); m.insert({WEB_KEY_COLORCODE,"3"});            /* all good */
    assertEqual(SmartDoorProject::parseWebdata(ringcontrol,m),true);
    assertEqual(CAST ringcontrol.currRingType,CAST SmartDoorProject::eRingType::eWAVE);           
    assertEqual(CAST ringcontrol.currColor,CAST SmartDoorProject::eColors::eYELLOW);                   
}


test(sensor_parser)
{   
    /*
        #define WEB_VALUE_MEASURELS     ("measureLS")
        #define WEB_VALUE_MEASUREUS     ("measureUS")
        #define WEB_VALUE_CLEARLS       ("clearLS")
        #define WEB_VALUE_CLEARUS       ("clearUS")

    */


    SensorProject::eSigErr signals = SensorProject::eSigErr::eNULL;
    std::map<String,String> m;

    assertEqual(SensorProject::parseWebdata(signals,m),false);                                      /* Too few arguments */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eNULL);                                   /* Not changed */
    

    m.insert({"abc","abc"}); m.insert({"abc2","abc2"});
    assertEqual(SensorProject::parseWebdata(signals,m),false);                                      /* Too many arguments */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eNULL);                                   /* Not changed */

    m.clear(); m.insert({"abc","abc"});
    assertEqual(SensorProject::parseWebdata(signals,m),false);                                      /* Wrong key */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eNULL);                                   /* Not changed */

    m.clear();m.insert({WEB_KEY_ACTION,"abc"});                                                               
    assertEqual(SensorProject::parseWebdata(signals,m),false);                                      /* Unknown value    */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eNULL);                                   /* Not changed      */

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_MEASURELS});                                                               
    assertEqual(SensorProject::parseWebdata(signals,m),true);                                       /* correct value   */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eMEASURELIGHTSENSOR);                     /* Not changed      */

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_MEASUREUS});                                                               
    assertEqual(SensorProject::parseWebdata(signals,m),true);                                       /* correct value   */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eMEASUREULTRASONIC);                      /* Not changed     */

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_CLEARLS});                                                               
    assertEqual(SensorProject::parseWebdata(signals,m),true);                                       /* correct value   */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eCLEARLIGHTSENSOR);                       /* Not changed     */

    m.clear();m.insert({WEB_KEY_ACTION,WEB_VALUE_CLEARUS});                                                               
    assertEqual(SensorProject::parseWebdata(signals,m),true);                                       /* correct value   */
    assertEqual(CAST signals,CAST SensorProject::eSigErr::eCLEARULTRASONIC);                        /* Not changed     */
    
}