#include "Global_FRTTSetup.h"

void dataAllocator(const FRTT::FRTTDataContainerOnQueue & orig, FRTT::FRTTTempDataContainer & temp)
{
    temp.data = orig.data;
    temp.senderAddress = orig.senderAddress;
    temp.u32AdditionalData = orig.u32AdditionalData;
    temp.u8DataType = orig.u8DataType;
}


void deleter(FRTT::FRTTTempDataContainer & temp)
{
    // nothing to do
}