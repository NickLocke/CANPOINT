// 3rd party libraries
#include <Streaming.h>

// VLCB library header files
#include <VLCB.h>
#include <VCAN2040.h>    

// Points configuration
#include "points.h"

// forward function declarations
void eventhandler(byte, const VLCB::VlcbMessage *);
void printConfig();

// constants
const byte VER_MAJ = 1;             // code major version
const char VER_MIN = 'a';           // code minor version
const byte VER_BETA = 0;            // code beta sub-version
const byte MANUFACTURER = MANU_DEV; // for boards in development.
const byte MODULE_ID = 102;         // VLCB module type

// These settings assume a Raspberry Pi Pico running on a CANETHERX borad, without a separate CAN interface.
const byte LED_GRN = 2;             // VLCB green Unitialised LED pin
const byte LED_YLW = 3;             // VLCB yellow Normal LED pin
const byte SWITCH0 = 5;             // VLCB push button switch pin

// module name, must be at most 7 characters.
char mname[] = "POINT";

// CAN transport object
VLCB::VCAN2040 can2040;                  

// Service objects
VLCB::LEDUserInterface ledUserInterface(LED_GRN, LED_YLW, SWITCH0);
VLCB::SerialUserInterface serialUserInterface;
VLCB::MinimumNodeServiceWithDiagnostics mnService;
VLCB::CanServiceWithDiagnostics canService(&can2040);
VLCB::NodeVariableService nvService;
VLCB::EventConsumerService ecService;
VLCB::EventTeachingService etService;
VLCB::EventProducerService epService;

// Event Variable (EV) structure
const int EV_TYPE = 1;
const int EV_POINT_NUMBER = 2;

// Produced event groupings (becomes the high byte of the event number)
enum {  
  PRODUCED_EVENT_POINT_MOTOR,
  PRODUCED_EVENT_NORMAL_INDICATION,
  PRODUCED_EVENT_REVERSE_INDICATION,
  PRODUCED_EVENT_LOCKED_INDICATION,
  PRODUCED_EVENT_OUT_OF_CORRESPONDENCE_INDICATION,
  PRODUCED_EVENT_SET_AND_LOCKED_NORMAL,
  PRODUCED_EVENT_SET_AND_LOCKED_REVERSE
};

// Consumed event groupings (will be received as EV1 in the incoming event)
enum {
  CONSUMED_EVENT_POINT_SWITCH_NORMAL,
  CONSUMED_EVENT_POINT_SWITCH_REVERSE,
  CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_NORMAL,
  CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_REVERSE,
  CONSUMED_EVENT_DETECTED_NORMAL,
  CONSUMED_EVENT_DETECTED_REVERSE,
  CONSUMED_EVENT_TRACK_OCCUPIED
};

// setup - runs once at power on
void setup()
{
  Serial.begin (115200);
  Serial << endl << endl << F("> ** CANPOINT ** ") << __FILE__ << endl;

  setupVLCB();

  // show code version and copyright notice
  printConfig();

  // end of setup
  Serial << F("> ready") << endl << endl;
}


// loop - runs forever
void loop()
{
  // do VLCB message, switch and LED processing
  VLCB::process();

  process_serial_input();

}

// setup VLCB - runs once at power on from setup()
void setupVLCB()
{
  VLCB::checkStartupAction(LED_GRN, LED_YLW, SWITCH0);

  VLCB::setServices({
    &mnService, &ledUserInterface, &serialUserInterface, &canService, &nvService,
    &ecService, &epService, &etService});

  // set config layout parameters
  VLCB::setNumNodeVariables(0);
  VLCB::setMaxEvents(64);
  VLCB::setNumEventVariables(1);

  // set module parameters
  VLCB::setVersion(VER_MAJ, VER_MIN, VER_BETA);
  VLCB::setModuleId(MANUFACTURER, MODULE_ID);

  // set module name
  VLCB::setName(mname);

  // register our VLCB event handler, to receive event messages of learned events
  ecService.setEventHandler(eventhandler);

  // configure and start CAN bus and VLCB message processing
  can2040.setNumBuffers(2, 2);  
  can2040.setPins(9, 1);        

  if (!can2040.begin())
  {
    Serial << F("> error starting VLCB") << endl;
  }

  // initialise and load configuration
  VLCB::begin();

  Serial << F("> mode = (") << _HEX(VLCB::getCurrentMode()) << ") " << VLCB::Configuration::modeString(VLCB::getCurrentMode());
  Serial << F(", CANID = ") << VLCB::getCANID();
  Serial << F(", NN = ") << VLCB::getNodeNum() << endl;

}

// user-defined event processing function
// called from the VLCB library when a learned event is received
// it receives the event table index and the CAN frame
void eventhandler(byte eventIndex, const VLCB::VlcbMessage *msg)
{
  Serial << F("> event handler: index = ") << eventIndex << F(", opcode = 0x") << _HEX(msg->data[0]) << endl;

  // Event Off op-codes have odd numbers. 
  bool actionType = (msg->data[0] & 0x01) == 0; // True for on, false for off

  int eventType = GetEventTypeFromEvent(eventIndex);
  int pointNumber = GetPointNumberFromEvent(eventIndex);

  switch (eventType) {

    case CONSUMED_EVENT_POINT_SWITCH_NORMAL:
      actionType ? ProcessSwitchNormalOn(eventIndex, pointNumber) : ProcessSwitchNormalOff(eventIndex, pointNumber);
      break;
      
    case CONSUMED_EVENT_POINT_SWITCH_REVERSE:
      actionType ? ProcessSwitchReverseOn(eventIndex, pointNumber) : ProcessSwitchReverseOff(eventIndex, pointNumber);
      break;

    case CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_NORMAL:
      actionType ? ProcessRouteRequiringNormalCalled(eventIndex, pointNumber) : ProcessRouteRequiringNormalCleared(eventIndex, pointNumber);
      break;

    case CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_REVERSE:
      actionType ? ProcessRouteRequiringReverseCalled(eventIndex, pointNumber) : ProcessRouteRequiringReverseCleared(eventIndex, pointNumber);
      break;

    case CONSUMED_EVENT_DETECTED_NORMAL:
      actionType ? ProcessDetectedNormal(eventIndex, pointNumber) : ProcessNormalDetectionLost(eventIndex, pointNumber);
      break;

    case CONSUMED_EVENT_DETECTED_REVERSE:
      actionType ? ProcessDetectedReverse(eventIndex, pointNumber) : ProcessReverseDetectionLost(eventIndex, pointNumber);
      break;

    case CONSUMED_EVENT_TRACK_OCCUPIED:
      actionType ? ProcessTrackOccupied(eventIndex, pointNumber) : ProcessTrackCleared(eventIndex, pointNumber);
      break;

    default:
      Serial << F("> *** oops - unknown event type seen in EV1 - ") << eventType << endl;
      break;

  }   

}

void ProcessSwitchNormalOn(byte eventIndex, int pointNumber)
{
    // The control panel switch has been moved to the normal position
    Serial << GetPointNumberDisplay(pointNumber) << F(" switch moved to normal position.");
}

void ProcessSwitchNormalOff(byte eventIndex, int pointNumber)
{
    // The control panel switch has been moved away from the normal position
    Serial << GetPointNumberDisplay(pointNumber) << F(" switch moved from normal to centre position.");
}

void ProcessSwitchReverseOn(byte eventIndex, int pointNumber)
{
    // The control panel switch has been moved to the reverse position
    Serial << GetPointNumberDisplay(pointNumber) << F(" switch moved to reverse position.");
}

void ProcessSwitchReverseOff(byte eventIndex, int pointNumber)
{
    // The control panel switch has been moved away from the reverse position
    Serial << GetPointNumberDisplay(pointNumber) << F(" switch moved from reverse to centre position.");
}

void ProcessRouteRequiringNormalCalled(byte eventIndex, int pointNumber)
{
    // A route which requires the points to be locked normal has been called
    Serial << F("A route which requires ") << GetPointNumberDisplay(pointNumber) << F(" to be in the normal position has been called.");
}

void ProcessRouteRequiringNormalCleared(byte eventIndex, int pointNumber)
{
    // A route which requires the points to be locked normal has been cleared
    Serial << F("A route which required ") << GetPointNumberDisplay(pointNumber) << F(" to be in the normal position has been cleared.");
}

void ProcessRouteRequiringReverseCalled(byte eventIndex, int pointNumber)
{
    // A route which requires the points to be locked reverse has been called 
    Serial << F("A route which requires ") << GetPointNumberDisplay(pointNumber) << F(" to be in the reverse position has been called.");
}

void ProcessRouteRequiringReverseCleared(byte eventIndex, int pointNumber)
{
    // A route which requires the points to be locked reverse has been cleared 
    Serial << F("A route which required ") << GetPointNumberDisplay(pointNumber) << F(" to be in the reverse position has been cleared.");
}

void ProcessDetectedNormal(byte eventIndex, int pointNumber)
{
    // The points have been detected normal
    Serial << GetPointNumberDisplay(pointNumber) << F(" detected normal.");
}

void ProcessNormalDetectionLost(byte eventIndex, int pointNumber)
{
    // The points are no longer detected normal
    Serial << GetPointNumberDisplay(pointNumber) << F(" no longer detected normal.");
}

void ProcessDetectedReverse(byte eventIndex, int pointNumber)
{
    // The points have been detected reverse
    Serial << GetPointNumberDisplay(pointNumber) << F(" detected reverse.");
}

void ProcessReverseDetectionLost(byte eventIndex, int pointNumber)
{
    // The points are no longer detected reverse
    Serial << GetPointNumberDisplay(pointNumber) << F(" no longer detected reverse.");
}

void ProcessTrackOccupied(byte eventIndex, int pointNumber)
{
    // The track circuit over the points has become occupied
    Serial << F("The track over ") << GetPointNumberDisplay(pointNumber) << F(" has become occupied.");
}

void ProcessTrackCleared(byte eventIndex, int pointNumber)
{
    // The track circuit over the points has become clear
    Serial << F("The track over ") << GetPointNumberDisplay(pointNumber) << F(" is no longer occupied.");
}

int GetEventTypeFromEvent(byte eventIndex)
{
  int eventType = VLCB::getEventEVval(eventIndex, EV_TYPE);
  return eventType;
}

int GetPointNumberFromEvent(byte eventIndex)
{
  int pointNumber = VLCB::getEventEVval(eventIndex, EV_POINT_NUMBER);
  return pointNumber;
}

point GetPointFromInternalNumber(int pointNumber)
{
  int items = GetPointCount();
  int counter;

  for(counter = 0; counter < items; counter++)
  {
    if(points[counter].internal_number == pointNumber)
    {
      break;
    }
  }

  return points[counter]; // TODO some error checking

}

// How many points are defined
int GetPointCount()
{
  int itemCount = sizeof(points) / sizeof(points[0]);
  return itemCount;
}

char * GetPointNumberDisplay(int pointNumber)
{
  point point = GetPointFromInternalNumber(pointNumber);

  static char outputBuffer[64]; // Static means that the memory is reserved and reused across multiple calls.
  static char workingBuffer[20];

  outputBuffer[0] = '\0'; 

  int externalNumber = point.external_number;

  strcat(outputBuffer, "Points ");

  snprintf(workingBuffer, sizeof(workingBuffer), "%u", externalNumber);
  strcat(outputBuffer, workingBuffer);

  strcat(outputBuffer, " (");

  snprintf(workingBuffer, sizeof(workingBuffer), "%u", pointNumber);
  strcat(outputBuffer, workingBuffer);

  strcat(outputBuffer, ")");
  return outputBuffer;
}

void sendOnEvent(int eventType, int eventNumber)
{
  VLCB::sendMessageWithNN(OPC_ACON, eventType, eventNumber);
}

void sendOffEvent(int eventType, int eventNumber)
{
  VLCB::sendMessageWithNN(OPC_ACOF, eventType, eventNumber);
}

void process_serial_input() 
{
  if (Serial.available()) 
  {

    char inputChar = Serial.read();

    switch (inputChar) {

      case 'c':
        printConfig();
        break;

      case 'p':
        printPointConfiguration();
        break;

      default:
        // Serial << F("> unknown command ") << c << endl;
        break;
    }
  }
}

// print code version config details and copyright notice
void printConfig()
{
  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;

  // copyright
  Serial << F("> © Nick Locke (MERG 3518) 2026") << endl;
}

void printPointConfiguration()
{
  int items = GetPointCount();
  int counter;

  for(counter = 0; counter < items; counter++)
  {
    int internalNumber = points[counter].internal_number;

    Serial << GetPointNumberDisplay(internalNumber);

    if(counter + 1 < items)
    {
      Serial << F(", ");
    }
  }

  Serial << endl;
}