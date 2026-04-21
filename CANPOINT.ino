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
VLCB::VCAN2040 vcan2040;

// Service objects
VLCB::LEDUserInterface ledUserInterface(LED_GRN, LED_YLW, SWITCH0);
// VLCB::SerialUserInterface serialUserInterface;
VLCB::MinimumNodeServiceWithDiagnostics mnService;
VLCB::CanServiceWithDiagnostics canService(&vcan2040);
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
  CONSUMED_EVENT_DETECTED_NORMAL_A_END,
  CONSUMED_EVENT_DETECTED_NORMAL_B_END,
  CONSUMED_EVENT_DETECTED_REVERSE_A_END,
  CONSUMED_EVENT_DETECTED_REVERSE_B_END,
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
  vcan2040.setNumBuffers(64, 128);
  vcan2040.setPins(9, 1);

  if (!vcan2040.begin()) {
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

    case CONSUMED_EVENT_DETECTED_NORMAL_A_END:
      actionType ? ProcessDetectedNormal(number, 'A') : ProcessNormalDetectionLost(number, 'A');
      break;

    case CONSUMED_EVENT_DETECTED_NORMAL_B_END:
      actionType ? ProcessDetectedNormal(number, 'B') : ProcessNormalDetectionLost(number, 'B');
      break;

    case CONSUMED_EVENT_DETECTED_REVERSE_A_END:
      actionType ? ProcessDetectedReverse(number, 'A') : ProcessReverseDetectionLost(number, 'A');
      break;

    case CONSUMED_EVENT_DETECTED_REVERSE_B_END:
      actionType ? ProcessDetectedReverse(number, 'B') : ProcessReverseDetectionLost(number, 'B');
      break;

    case CONSUMED_EVENT_TRACK_OCCUPIED:
      actionType ? ProcessTrackOccupied(number) : ProcessTrackCleared(number);
      break;

    default:
      Serial << F("> *** oops - unknown event type seen in EV1 - ") << eventType << endl;
      break;
  }
}