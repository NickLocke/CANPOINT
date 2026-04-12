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