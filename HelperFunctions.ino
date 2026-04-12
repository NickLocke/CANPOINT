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

