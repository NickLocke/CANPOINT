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

bool IsTrackCircuitOverPointOccupied(point point) {

  for (int counter = 0; counter < MAX_TRACK_CIRCUITS; counter++) {
    int trackCircuitNumber = point.track_locking[counter];
    
    if(trackCircuitNumber == 0) break; // Zero means unused, so we don't need to do any more.

    // Non zero, we need to get the status of the track and any one track occupied is all we need.
    trackCircuit* trackCircuit = GetTrackCircuitFromInternalNumber(trackCircuitNumber);
    
    if(trackCircuit->occupied) return true;
    
  }

  return false;
}

void AttemptToMovePoint(point point, char requestedDirection)
{
  Serial << F("Checking existing setting");
  // If the points are already in the requested direction, get out
  if(point.detected_normal_A_end && point.detected_normal_B_end && requestedDirection == 'N') return;
  if(point.detected_reverse_A_end && point.detected_reverse_B_end && requestedDirection == 'R') return;

  Serial << F("Checking locks");
  // If the points cannot move, get out
  if(requestedDirection == 'N' && !CanPointMoveToNormal(point)) return;
  if(requestedDirection == 'R' && !CanPointMoveToReverse(point)) return;

  Serial << F("OK to move");
  // Request the points move
  requestedDirection == 'N' ? sendOffEvent(PRODUCED_EVENT_POINT_MOTOR, point.internal_number) 
                            : sendOnEvent(PRODUCED_EVENT_POINT_MOTOR, point.internal_number);
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

const char *GetRouteCalledDisplay(point point) {

  if(!point.normal_route_called && !point.reverse_route_called)
  {
    return "Clear         ";
  }

  if(point.normal_route_called && !point.reverse_route_called)
  {
    return "Called Normal ";
  }

  if(point.reverse_route_called && !point.normal_route_called)
  {
    return "Called Reverse";
  }

  return "** Error **   "; // This means both normal and reverse have been called
}

void EvaluatePointSwitchDetectionIndications(point point)
{
  (point.detected_normal_A_end && point.detected_normal_B_end)
    ? sendOnEvent(PRODUCED_EVENT_NORMAL_INDICATION, point.internal_number)
    : sendOffEvent(PRODUCED_EVENT_NORMAL_INDICATION, point.internal_number);

  (point.detected_reverse_A_end && point.detected_reverse_B_end)
    ? sendOnEvent(PRODUCED_EVENT_REVERSE_INDICATION, point.internal_number)
    : sendOffEvent(PRODUCED_EVENT_REVERSE_INDICATION, point.internal_number);
}

void sendOnEvent(int eventType, int eventNumber) {
  VLCB::sendMessageWithNN(OPC_ACON, eventType, eventNumber);
}

void sendOffEvent(int eventType, int eventNumber) {
  VLCB::sendMessageWithNN(OPC_ACOF, eventType, eventNumber);
}

