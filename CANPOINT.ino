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
const byte VER_MAJ = 1;              // code major version
const char VER_MIN = 'a';            // code minor version
const byte VER_BETA = 0;             // code beta sub-version
const byte MANUFACTURER = MANU_DEV;  // for boards in development.
const byte MODULE_ID = 103;          // VLCB module type

// These settings assume a Raspberry Pi Pico running on a CANETHERX borad, without a separate CAN interface.
const byte LED_GRN = 2;  // VLCB green Unitialised LED pin
const byte LED_YLW = 3;  // VLCB yellow Normal LED pin
const byte SWITCH0 = 5;  // VLCB push button switch pin

// module name, must be at most 7 characters.
char mname[] = "POINT";

// CAN transport object
VLCB::VCAN2040 can2040;

// Service objects
VLCB::LEDUserInterface ledUserInterface(LED_GRN, LED_YLW, SWITCH0);
// VLCB::SerialUserInterface serialUserInterface;
VLCB::MinimumNodeServiceWithDiagnostics mnService;
VLCB::CanServiceWithDiagnostics canService(&can2040);
VLCB::NodeVariableService nvService;
VLCB::EventConsumerService ecService;
VLCB::EventTeachingService etService;
VLCB::EventProducerService epService;

// Event Variable (EV) structure
const int EV_TYPE = 1;
const int EV_NUMBER = 2;

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
  CONSUMED_EVENT_UNKNOWN,
  CONSUMED_EVENT_POINT_SWITCH_NORMAL,
  CONSUMED_EVENT_POINT_SWITCH_REVERSE,
  CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_NORMAL,
  CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_REVERSE,
  CONSUMED_EVENT_DETECTED_NORMAL,
  CONSUMED_EVENT_DETECTED_REVERSE,
  CONSUMED_EVENT_TRACK_OCCUPIED
};

// setup - runs once at power on
void setup() {
  
  Serial.begin(115200);

  unsigned long serialStartupTimer = millis();

  while (!Serial && millis() - serialStartupTimer < 5000);


  Serial << endl
         << endl
         << F("> ** CANPOINT ** ") << endl;

  setupVLCB();

  // show code version and copyright notice
  printConfig();

  // end of setup
  Serial << F("> ready") << endl
         << endl;
}


// loop - runs forever
void loop() {
  // do VLCB message, switch and LED processing
  VLCB::process();

  process_serial_input();
}

// setup VLCB - runs once at power on from setup()
void setupVLCB() {
  VLCB::checkStartupAction(LED_GRN, LED_YLW, SWITCH0);

  VLCB::setServices({ &mnService, &ledUserInterface, &canService, &nvService,
                      &ecService, &epService, &etService });

  // set config layout parameters
  VLCB::setNumNodeVariables(0);
  VLCB::setMaxEvents(128);
  VLCB::setNumEventVariables(2);

  // set module parameters
  VLCB::setVersion(VER_MAJ, VER_MIN, VER_BETA);
  VLCB::setModuleId(MANUFACTURER, MODULE_ID);

  // set module name
  VLCB::setName(mname);

  // register our VLCB event handler, to receive event messages of learned events
  ecService.setEventHandler(eventhandler);

  // configure and start CAN bus and VLCB message processing
  can2040.setNumBuffers(16, 32);
  can2040.setPins(9, 1);

  if (!can2040.begin()) {
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
void eventhandler(byte eventIndex, const VLCB::VlcbMessage *msg) {
  Serial << F("> event handler: index = ") << eventIndex << F(", opcode = 0x") << _HEX(msg->data[0]) << endl;

  // Event Off op-codes have odd numbers.
  bool actionType = (msg->data[0] & 0x01) == 0;  // True for on, false for off

  int eventType = GetEventTypeFromEvent(eventIndex);
  int number = GetNumberFromEvent(eventIndex);

  switch (eventType) {

    case CONSUMED_EVENT_POINT_SWITCH_NORMAL:
      actionType ? ProcessSwitchNormalOn(number) : ProcessSwitchNormalOff(number);
      break;

    case CONSUMED_EVENT_POINT_SWITCH_REVERSE:
      actionType ? ProcessSwitchReverseOn(number) : ProcessSwitchReverseOff(number);
      break;

    case CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_NORMAL:
      actionType ? ProcessRouteRequiringNormalCalled(number) : ProcessRouteRequiringNormalCleared(number);
      break;

    case CONSUMED_EVENT_ROUTE_REQUIRING_POINTS_REVERSE:
      actionType ? ProcessRouteRequiringReverseCalled(number) : ProcessRouteRequiringReverseCleared(number);
      break;

    case CONSUMED_EVENT_DETECTED_NORMAL:
      actionType ? ProcessDetectedNormal(number) : ProcessNormalDetectionLost(number);
      break;

    case CONSUMED_EVENT_DETECTED_REVERSE:
      actionType ? ProcessDetectedReverse(number) : ProcessReverseDetectionLost(number);
      break;

    case CONSUMED_EVENT_TRACK_OCCUPIED:
      actionType ? ProcessTrackOccupied(number) : ProcessTrackCleared(number);
      break;

    default:
      Serial << F("> *** oops - unknown event type seen in EV1 - ") << eventType << endl;
      break;
  }
}

void ProcessSwitchNormalOn(int pointNumber) {
  // The control panel switch has been moved to the normal position
  point* point = GetPointFromInternalNumber(pointNumber);
  point->switch_normal = true;
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" switch moved to normal position.") << endl;
}

void ProcessSwitchNormalOff(int pointNumber) {
  // The control panel switch has been moved away from the normal position
  point* point = GetPointFromInternalNumber(pointNumber);
  point->switch_normal = false;
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" switch moved from normal to centre position.") << endl;
}

void ProcessSwitchReverseOn(int pointNumber) {
  // The control panel switch has been moved to the reverse position
  point* point = GetPointFromInternalNumber(pointNumber);
  point->switch_reverse = true;
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" switch moved to reverse position.") << endl;
}

void ProcessSwitchReverseOff(int pointNumber) {
  // The control panel switch has been moved away from the reverse position
  point* point = GetPointFromInternalNumber(pointNumber);
  point->switch_reverse = false;
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" switch moved from reverse to centre position.") << endl;
}

void ProcessRouteRequiringNormalCalled(int pointNumber) {
  // A route which requires the points to be locked normal has been called
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << F("A route which requires ") << GetPointNumberDisplay(*point) << F(" to be in the normal position has been called.") << endl;
}

void ProcessRouteRequiringNormalCleared(int pointNumber) {
  // A route which requires the points to be locked normal has been cleared
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << F("A route which required ") << GetPointNumberDisplay(*point) << F(" to be in the normal position has been cleared.") << endl;
}

void ProcessRouteRequiringReverseCalled(int pointNumber) {
  // A route which requires the points to be locked reverse has been called
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << F("A route which requires ") << GetPointNumberDisplay(*point) << F(" to be in the reverse position has been called.") << endl;
}

void ProcessRouteRequiringReverseCleared(int pointNumber) {
  // A route which requires the points to be locked reverse has been cleared
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << F("A route which required ") << GetPointNumberDisplay(*point) << F(" to be in the reverse position has been cleared.") << endl;
}

void ProcessDetectedNormal(int pointNumber) {
  // The points have been detected normal
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" detected normal.") << endl;
}

void ProcessNormalDetectionLost(int pointNumber) {
  // The points are no longer detected normal
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" no longer detected normal.") << endl;
}

void ProcessDetectedReverse(int pointNumber) {
  // The points have been detected reverse
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" detected reverse.") << endl;
}

void ProcessReverseDetectionLost(int pointNumber) {
  // The points are no longer detected reverse
  point* point = GetPointFromInternalNumber(pointNumber);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" no longer detected reverse.") << endl;
}

void ProcessTrackOccupied(int trackCircuitNumber) {
  // The track circuit has become occupied
  trackCircuit* trackCircuit = GetTrackCircuitFromInternalNumber(trackCircuitNumber);
  trackCircuit->occupied = true;
  Serial << F("> Track circuit ") << GetTrackCircuitNumberDisplay(*trackCircuit) << F(" has become occupied.") << endl;
}

void ProcessTrackCleared(int trackCircuitNumber) {
  // The track circuit has become clear
  trackCircuit* trackCircuit = GetTrackCircuitFromInternalNumber(trackCircuitNumber);
  trackCircuit->occupied = false;
  Serial << F("> Track circuit ") << GetTrackCircuitNumberDisplay(*trackCircuit) << F(" is now clear.") << endl;
}

bool CanPointMoveToNormal(point point)
{
  // The point switch is set against the movement
  if(point.switch_reverse) return false;

  // A conflicting route has been requested
  if(point.reverse_route_called) return false;

  return true;
}

bool CanPointMoveToReverse(point point)
{
  // The point switch is set against the movement
  if(point.switch_normal) return false;

  // A conflicting route has been requested
  if(point.normal_route_called) return false;

  return true;
  
}

int GetEventTypeFromEvent(byte eventIndex) {
  int eventType = VLCB::getEventEVval(eventIndex, EV_TYPE);
  return eventType;
}

int GetNumberFromEvent(byte eventIndex) {
  int number = VLCB::getEventEVval(eventIndex, EV_NUMBER);
  return number;
}

point* GetPointFromInternalNumber(int pointNumber) {
  int items = GetPointCount();
  int counter;

  for (counter = 0; counter < items; counter++) {
    if (points[counter].internal_number == pointNumber) {
      return &points[counter];
    }
  }

  Serial << F("< Point number ") << pointNumber << F(" was not found.");
  return NULL;  
}

trackCircuit* GetTrackCircuitFromInternalNumber(int trackCircuitNumber) {
  int items = GetTrackCircuitCount();
  int counter;

  for (counter = 0; counter < items; counter++) {
    if (trackCircuits[counter].internal_number == trackCircuitNumber) {
      return &trackCircuits[counter];
    }
  }

  Serial << F("< Track circuit number ") << trackCircuitNumber << F(" was not found.");
  return NULL;  
}

// How many points are defined
int GetPointCount() {
  int pointCount = sizeof(points) / sizeof(points[0]);
  return pointCount;
}

// How many track circuits are defined
int GetTrackCircuitCount() {
  int trackCircuitCount = sizeof(trackCircuits) / sizeof(trackCircuits[0]);
  return trackCircuitCount;
}

char *GetPointNumberDisplay(point point) {

  static char outputBuffer[64];  // Static means that the memory is reserved and reused across multiple calls.
  static char workingBuffer[20];

  outputBuffer[0] = '\0';

  snprintf(workingBuffer, sizeof(workingBuffer), "%u", point.external_number);
  strcat(outputBuffer, workingBuffer);

  strcat(outputBuffer, " (");

  snprintf(workingBuffer, sizeof(workingBuffer), "%u", point.internal_number);
  strcat(outputBuffer, workingBuffer);

  strcat(outputBuffer, ")");
  return outputBuffer;
}

char *GetTrackCircuitNumberDisplay(trackCircuit trackCircuit) {

  static char outputBuffer[64];  // Static means that the memory is reserved and reused across multiple calls.
  static char workingBuffer[20];

  outputBuffer[0] = '\0';

  snprintf(workingBuffer, sizeof(workingBuffer), "%u", trackCircuit.external_number);
  strcat(outputBuffer, workingBuffer);

  strcat(outputBuffer, " (");

  snprintf(workingBuffer, sizeof(workingBuffer), "%u", trackCircuit.internal_number);
  strcat(outputBuffer, workingBuffer);

  strcat(outputBuffer, ")");
  return outputBuffer;
}

const char *GetPointSwitchPositionDisplay(point point) {

  if(point.switch_normal && !point.switch_reverse)
  {
    return "Normal ";
  }

  if(point.switch_reverse && !point.switch_normal)
  {
    return "Reverse";
  }

  if(!point.switch_normal && !point.switch_reverse)
  {
    return "Centre ";
  }

  return "Error  "; // This means both normal and reverse are on
}

void sendOnEvent(int eventType, int eventNumber) {
  VLCB::sendMessageWithNN(OPC_ACON, eventType, eventNumber);
}

void sendOffEvent(int eventType, int eventNumber) {
  VLCB::sendMessageWithNN(OPC_ACOF, eventType, eventNumber);
}

void process_serial_input() {
  if (Serial.available()) {

    char inputChar = Serial.read();

    switch (inputChar) {

      case 'c':
        printConfig();
        break;

      case 'p':
        printPointConfiguration();
        break;

      case 't':
        printTrackCircuitConfiguration();
        break;

      case '\r':
	    case '\n':
	      break;

      default:
        Serial << F("> Unknown command ") << inputChar << endl;
        break;
    }
  }
}

// print code version config details and copyright notice
void printConfig() {
  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;

  // copyright
  Serial << F("> © Nick Locke (MERG 3518) 2026") << endl;
}

void printPointConfiguration() {
  int items = GetPointCount();
  int counter;

  Serial << endl << F("CURRENT POINT STATUS") << endl;
  Serial << F("Number   Switch  CanMoveN CanMoveR") << endl;

  for (counter = 0; counter < items; counter++) {

    Serial << GetPointNumberDisplay(points[counter]);

    Serial << F(" ");

    Serial << GetPointSwitchPositionDisplay(points[counter]);

    Serial << F(" ");

    Serial << F(CanPointMoveToNormal(points[counter]) ? "Yes" : "No ");

    Serial << F("      ");

    Serial << F(CanPointMoveToReverse(points[counter]) ? "Yes" : "No ");

    Serial << endl;
  }
}

void printTrackCircuitConfiguration() {
  int items = GetTrackCircuitCount();
  int counter;

  Serial << endl << F("CURRENT TRACK CIRCUIT STATUS") << endl;
  Serial << F("Number  State") << endl;

  for (counter = 0; counter < items; counter++) {

    Serial << GetTrackCircuitNumberDisplay(trackCircuits[counter]);

    Serial << F(" ");

    Serial << F(trackCircuits[counter].occupied ? "Occupied" : "Clear");

    Serial << endl;
  }

  
}