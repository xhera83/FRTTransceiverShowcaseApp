#include "SmartDoorUtilities.h"

#define CHECK

namespace SmartDoorProject
{   
    FRTT::FRTTTaskHandle SMARTDOOR_LEDSTRIP_TASKHANDLE = nullptr;
    Adafruit_NeoPixel strip(SMARTDOOR_STRIPLEDS,SMARTDOOR_PIN);
    Servo servo;
    ESP32PWM pwm;
    bool parsePrintSummarySig(std::map<String,String> & keyValPairs)
    {
        std::map<String, String>::iterator iterStart = keyValPairs.begin(), iterEnd = keyValPairs.end();

        if(iterStart == iterEnd || keyValPairs.size() < SMARTDOOR_PRINTSUMMARY_PAIRS || keyValPairs.size() > SMARTDOOR_PRINTSUMMARY_PAIRS) return false;

        while(iterStart != iterEnd)
        {   
            String key = iterStart->first;
            String value = iterStart->second;

            if(key == WEB_KEY_TASK && value == WEB_VAL_SMARTDOOR)
            {
                return true;
            }
            
            iterStart++;
        }
        
        return false;
    }
    
    bool parseWebdata(SmartDoorMailboxControl & mailControl,std::map<String,String> & keyValPairs)
    {   

        std::map<String, String>::iterator iterStart = keyValPairs.begin(), iterEnd = keyValPairs.end();

        if(iterStart == iterEnd || keyValPairs.size() < SMARTDOOR_MAILBOX_PAIRS || keyValPairs.size() > SMARTDOOR_MAILBOX_PAIRS) return false;

        while(iterStart != iterEnd)
        {   
            String key = iterStart->first;
            String value = iterStart->second;

            if(key == WEB_KEY_ACTION && value == WEB_VALUE_CLOSE && mailControl.status != eMailbox::eCLOSED)
            {
                mailControl.status = eMailbox::eCLOSED;
            }
            else if(key == WEB_KEY_ACTION && value == WEB_VALUE_OPEN && mailControl.status != eMailbox::eOPEN)
            {
                mailControl.status = eMailbox::eOPEN;
            }
            else
            {
                return false;
            }
            
            iterStart++;
        }
        
        return true;
    }

    bool parseWebdata(SmartDoorRingControl & ringControl,std::map<String,String> & keyValPairs)
    {
        std::map<String, String>::iterator iterStart = keyValPairs.begin(), iterEnd = keyValPairs.end();

        if(iterStart == iterEnd || (keyValPairs.size() < SMARTDOOR_RINGTYPE_PAIRS ||  keyValPairs.size() > SMARTDOOR_RINGTYPE_PAIRS)){
            return false; 
        }   /* no elements or too many elements */

        eColors tempColor = ringControl.currColor;
        eRingType tempRing = ringControl.currRingType;

        while(iterStart != iterEnd)
        {   
            String key = iterStart->first;
            String value = iterStart->second;

            if(key == WEB_KEY_RINGTYPE && (value == WEB_VAL_BLINK || value == WEB_VAL_WAVE))
            {
                ringControl.currRingType = (value == WEB_VAL_BLINK ? eRingType::eBLINK: eRingType::eWAVE);
            }
            else if(key.equals(WEB_KEY_COLORCODE))
            {   
                char * p;

                int converted = (int)std::strtol(value.c_str(),&p,10);

                eColors colors[SMARTDOOR_COLORS] = {eColors::eBLUE,eColors::eRED,eColors::eGREEN,eColors::eYELLOW};

                if(*p) 
                {   
                    ringControl.currRingType = tempRing;
                    ringControl.currColor = tempColor;
                    return false;
                
                }
                if(converted < 0 || converted >= SMARTDOOR_COLORS)
                {   
                    ringControl.currRingType = tempRing;
                    ringControl.currColor = tempColor;
                    return false;
                }
                ringControl.currColor = colors[converted];
            }
            else
            {
                ringControl.currRingType = tempRing;
                ringControl.currColor = tempColor;

                return false;
            }
            iterStart++;
        }
        return true;
    }


    void LedstripTask(void * param)
    {   
        SDRControl ringControl = *((SDRControl * )param);
        
        bool isOn = false;
        int currWavePos = 0;

        auto do_blink = [](Adafruit_NeoPixel & strip,bool & isOn,uint8_t u8Red,uint8_t u8Green, uint8_t u8Blue)
        {   
            if(isOn)
            {   
                isOn = false;
                strip.clear();
                strip.show();
            }
            else
            {   
                isOn = true;
                for(int i = 0; i < SMARTDOOR_STRIPLEDS;i++)
                {
                    strip.setPixelColor(i,u8Red,u8Green,u8Blue);
                }
                strip.show();
            }
        };

        auto do_wave = [](Adafruit_NeoPixel & strip,int & currPos,uint8_t u8Red,uint8_t u8Green, uint8_t u8Blue)
        {
            
            strip.setPixelColor(currPos,u8Red,u8Green,u8Blue);
            
            currPos++;

            if(currPos == SMARTDOOR_STRIPLEDS)
            {
                currPos = 0;
                strip.clear();
            }
            strip.show();
        };


        int delay = (ringControl.currRingType == eRingType::eBLINK) ? 500:100;

        uint8_t u8Red = 0;
        uint8_t u8Green = 0;
        uint8_t u8Blue = 0;

        switch(ringControl.currColor)
        {
            case eColors::eBLUE:
                u8Red = 0; u8Green = 0;u8Blue = 255;
                break;
            case eColors::eRED:
                u8Red = 255; u8Green = 0;u8Blue = 0;
                break;
            case eColors::eGREEN:
                u8Red = 0; u8Green = 255;u8Blue = 0;
                break;
            case eColors::eYELLOW:
                u8Red = 255; u8Green = 255;u8Blue = 0;
                break;
        }

        for(;;)
        {
            if(ringControl.u8CurrRingTime * delay == SMARTDOOR_RINGTYPE_MS)
            {
                SMARTDOOR_LEDSTRIP_TASKHANDLE = nullptr;
                SmartDoorProject::strip.clear();
                SmartDoorProject::strip.show();
                vTaskDelete(NULL);
            }
            
            if(ringControl.currRingType == eRingType::eBLINK)
            {
                do_blink(SmartDoorProject::strip,isOn,u8Red,u8Green,u8Blue);
            }
            else
            {
                do_wave(SmartDoorProject::strip,currWavePos,u8Red,u8Green,u8Blue);
            }

            vTaskDelay(pdMS_TO_TICKS(delay));
            ringControl.u8CurrRingTime++;
        }
    }

    void ServoLockTask(void * pvParams)
    {   
        SDMControl mailboxControl = *((SDMControl *)pvParams);

        if(mailboxControl.status == SmartDoorProject::eMailbox::eCLOSED)
        {  
            servo.write(0);
        }
        else
        {
            servo.write(90);
        }
        
        vTaskDelete(NULL);
    }

}