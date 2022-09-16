/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#define OPENTHREAD_CONFIG_COAP_API_ENABLE 1

#include<stdio.h>
#include <time.h>

#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/platform/logging.h>

#include "openthread-system.h"
#include "cli/cli_config.h"
#include "common/code_utils.hpp"

#include "lib/platform/reset_util.h"
#include <openthread/dataset.h>
#include <openthread/ip6.h>
#include <openthread/thread.h>
#include <string.h>

#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>


#include <openthread/message.h>
#include <openthread/udp.h>
#include "utils/code_utils.h"


#include <openthread/coap.h>
#include <unistd.h> 



#if OPENTHREAD_FTD
#include <openthread/dataset_ftd.h>
#include <openthread/thread_ftd.h>
#endif

/**
 * This function initializes the CLI app.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
#define UDP_PORT 5683

extern void otAppCliInit(otInstance *aInstance);
static void setNetworkConfiguration(otInstance *aInstance);
static const char UDP_DEST_ADDR[] = "fd11:1111:1122:2222:dea6:32ff:fe0d:b3f4";



static const char UDP_PAYLOAD[]   = "The weather outside today is 20 Fahranheit!";

static void initUdp(otInstance *aInstance);
static void sendUdp(otInstance *aInstance);
static void coap_send_data_request(otInstance *aInstance);
void coap_init(otInstance *aInstance);
static void handleButtonInterrupt(otInstance *aInstance);
void handleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
void delay(int number_of_seconds);
unsigned sleep (unsigned int __seconds);
void waitFor(unsigned int secs);


static otUdpSocket sUdpSocket;


void handleNetifStateChanged(uint32_t aFlags, void *aContext)
{
    
}

#if OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    free(aPtr);
}
#endif

#if OPENTHREAD_POSIX && !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
static void ProcessExit(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    OT_UNUSED_VARIABLE(aContext);
    OT_UNUSED_VARIABLE(aArgsLength);
    OT_UNUSED_VARIABLE(aArgs);

    exit(EXIT_SUCCESS);
}
static const otCliCommand kCommands[] = {{"exit", ProcessExit}};
#endif

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

void waitFor(unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

void otTaskletsSignalPending(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

void setNetworkConfiguration(otInstance *aInstance) 
{
    static char          aNetworkName[] = "OpenThreadDemo ";
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Key, PSKc, Security Policy
     */
    //aDataset.mActiveTimestamp.mSeconds             = 1;
    //aDataset.mActiveTimestamp.mTicks               = 0;
    //aDataset.mActiveTimestamp.mAuthoritative       = false;
    //aDataset.mComponents.mIsActiveTimestampPresent = true;

    /* Set Channel to 15 */
    //aDataset.mChannel                      = 15;
    //aDataset.mComponents.mIsChannelPresent = true;

    /* Set Pan ID to 2222 */
    //aDataset.mPanId                      = (otPanId)0x1234;
    //aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID */
    //uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22};
    //memcpy(aDataset.mExtendedPanId.m8, extPanId, sizeof(aDataset.mExtendedPanId));
    //aDataset.mComponents.mIsExtendedPanIdPresent = true;

    /* Set network key */
    uint8_t key[OT_NETWORK_KEY_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    memcpy(aDataset.mNetworkKey.m8, key, sizeof(aDataset.mNetworkKey));
    aDataset.mComponents.mIsNetworkKeyPresent = true;
    

    /* Set Network Name */
    /*size_t length = strlen(aNetworkName);
    assert(length <= OT_NETWORK_NAME_MAX_SIZE);
    memcpy(aDataset.mNetworkName.m8, aNetworkName, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;
    */
    otDatasetSetActive(aInstance, &aDataset);
    
    
}

void handleButtonInterrupt(otInstance *aInstance)
{
    //sendUdp(aInstance);
    otPlatLog(OT_LOG_LEVEL_DEBG, OT_LOG_REGION_PLATFORM, "In button interrupt!"); 

    coap_send_data_request(aInstance);
    otError error = otThreadBecomeRouter(aInstance);
    if (error != OT_ERROR_NONE) {
        otPlatLog(OT_LOG_LEVEL_DEBG, OT_LOG_REGION_PLATFORM, "Remains Child %d\n", error); 
     } else {
        otPlatLog(OT_LOG_LEVEL_DEBG, OT_LOG_REGION_PLATFORM, "Attempts to become Router %d\n", error);
     } 

    otSysLedToggle(1);
    //coap_send_data_request(aInstance);
}

void initUdp(otInstance *aInstance)
{
    otSockAddr  listenSockAddr;

    memset(&sUdpSocket, 0, sizeof(sUdpSocket));
    memset(&listenSockAddr, 0, sizeof(listenSockAddr));
    


    listenSockAddr.mPort    = UDP_PORT;

    otUdpOpen(aInstance, &sUdpSocket, handleUdpReceive, aInstance);
    otUdpBind(aInstance, &sUdpSocket, &listenSockAddr, OT_NETIF_THREAD);
    otThreadGetRouterUpgradeThreshold(aInstance);   
}
void coap_init(otInstance *aInstance)
{
    otError error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
   
}

static void coap_send_data_request(otInstance *aInstance){
  otError       error = OT_ERROR_NONE;
  otMessage   * myMessage;
  otMessageInfo myMessageInfo;
  otIp6Address  destinationAddr;
  const char  * serverIpAddr = "fde8:b952:255b:0:dea6:32ff:fe0d:b3f4";
  const char * myTemperatureJson = "The weather today is 70 degrees celciuss.";

  
  memset(&myMessageInfo, 0, sizeof(myMessageInfo));
  myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
  error = otIp6AddressFromString(serverIpAddr, &myMessageInfo.mPeerAddr);
  do{
	//Create a new message
    myMessage = otCoapNewMessage(aInstance, NULL);
    if (myMessage == NULL) {
      //NRF_LOG_INFO("Failed to allocate message for CoAP Request\r\n");
      //LogInfo("Failed");
      return;

    }
	//Set CoAP type and code in the message
    otCoapMessageInit(myMessage, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT);

	//Add the URI path option in the message
    error = otCoapMessageAppendUriPathOptions(myMessage, "alarm");
    if (error != OT_ERROR_NONE){ break; }

	//Add the content format option in the message
    error = otCoapMessageAppendContentFormatOption(myMessage, OT_COAP_OPTION_CONTENT_FORMAT_TEXT_PLAIN);
    if (error != OT_ERROR_NONE){ break; }

	//Set the payload delimiter in the message
    error = otCoapMessageSetPayloadMarker(myMessage);
    if (error != OT_ERROR_NONE){ break; }

	///Append the payload to the message
    error = otMessageAppend(myMessage, myTemperatureJson, strlen(myTemperatureJson));
    if (error != OT_ERROR_NONE){ break; }

	//Send CoAP-request
    
    error = otCoapSendRequest(aInstance, myMessage, &myMessageInfo, NULL, NULL);
    
  }while(false);

  if (error != OT_ERROR_NONE) {
    //NRF_LOG_INFO("Failed to send CoAP Request: %d\r\n", error);
    //otMessageFree(myMessage);
    otSysLedToggle(2);
  }else{
    //LogInfo("Sent Data succesfull!");
  }
}


/*void sendUdp(otInstance *aInstance)
{
    otError       error = OT_ERROR_NONE;
    otMessage *   message;
    otMessageInfo messageInfo;
    otIp6Address  destinationAddr;

    otError otCoapStart(otInstance *aInstance, uint16_t aPort);

    memset(&messageInfo, 0, sizeof(messageInfo));
    message = otCoapNewMessage(aInstance, NULL);
    


    otIp6AddressFromString(UDP_DEST_ADDR, &destinationAddr);
    messageInfo.mPeerAddr    = destinationAddr;
    messageInfo.mPeerPort    = OT_DEFAULT_COAP_PORT;

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    message = otUdpNewMessage(aInstance, NULL);
    otEXPECT_ACTION(message != NULL, error = OT_ERROR_NO_BUFS);

    error = otMessageAppend(message, UDP_PAYLOAD, sizeof(UDP_PAYLOAD));
    otCoapMessageInit(message, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT);
    

    otEXPECT(error == OT_ERROR_NONE);
    //error = otUdpSend(aInstance, &sUdpSocket, message, &messageInfo);
    error = otCoapSendRequest(aInstance, message, &messageInfo, NULL, NULL);

 exit:
    if (error != OT_ERROR_NONE && message != NULL)
    {
        otMessageFree(message);
    }
} */

void handleUdpReceive(void *aContext, otMessage *aMessage,
                      const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aContext);
    OT_UNUSED_VARIABLE(aMessage);
    OT_UNUSED_VARIABLE(aMessageInfo);
    otSysLedToggle(3);
}

int main(int argc, char *argv[])
{

    otInstance *instance;
    int count = 0;


    OT_SETUP_RESET_JUMP(argv);

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    size_t   otInstanceBufferLength = 0;
    uint8_t *otInstanceBuffer       = NULL;
#endif

pseudo_reset:

    otSysInit(argc, argv);

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    // Call to query the buffer size
    (void)otInstanceInit(NULL, &otInstanceBufferLength);

    // Call to allocate the buffer
    otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
    assert(otInstanceBuffer);

    // Initialize OpenThread with the buffer
    instance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
    instance = otInstanceInitSingle();
#endif
    assert(instance);

    otAppCliInit(instance);
    setNetworkConfiguration(instance);
    /* Start the Thread network interface (CLI cmd > ifconfig up) */
    otIp6SetEnabled(instance, true);
    /* Start the Thread stack (CLI cmd > thread start) */
    otThreadSetEnabled(instance, true);
    otSysLedInit();
    otSysLedSet(2, true);
    otSysButtonInit(handleButtonInterrupt);
    coap_init(instance);
    otThreadBecomeRouter(instance);
  
#if OPENTHREAD_POSIX && !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
    otCliSetUserCommands(kCommands, OT_ARRAY_LENGTH(kCommands), instance);
#endif

    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);
        otSysButtonProcess(instance);
        /*if(count++ > 100000)
        {
            count = 0;
            otSysLedToggle(1);
        }*/
    }

    otInstanceFinalize(instance);
    
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    free(otInstanceBuffer);
#endif

    goto pseudo_reset;

    return 0;
}

#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    va_list ap;

    va_start(ap, aFormat);
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    va_end(ap);
}
#endif
