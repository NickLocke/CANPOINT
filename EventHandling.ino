void ProcessSwitchNormalOn(int pointNumber) {
  // The control panel switch has been moved to the normal position
  point* point = GetPointFromInternalNumber(pointNumber);
  point->switch_normal = true;
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" switch moved to normal position.") << endl;

  AttemptToMovePoint(*point, 'N');



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

  AttemptToMovePoint(*point, 'R');
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
  point->normal_route_called = true;
  Serial << F("A route which requires ") << GetPointNumberDisplay(*point) << F(" to be in the normal position has been called.") << endl;
}

void ProcessRouteRequiringNormalCleared(int pointNumber) {
  // A route which requires the points to be locked normal has been cleared
  point* point = GetPointFromInternalNumber(pointNumber);
  point->normal_route_called = false;
  Serial << F("A route which required ") << GetPointNumberDisplay(*point) << F(" to be in the normal position has been cleared.") << endl;
}

void ProcessRouteRequiringReverseCalled(int pointNumber) {
  // A route which requires the points to be locked reverse has been called
  point* point = GetPointFromInternalNumber(pointNumber);
  point->reverse_route_called = true;
  Serial << F("A route which requires ") << GetPointNumberDisplay(*point) << F(" to be in the reverse position has been called.") << endl;
}

void ProcessRouteRequiringReverseCleared(int pointNumber) {
  // A route which requires the points to be locked reverse has been cleared
  point* point = GetPointFromInternalNumber(pointNumber);
  point->reverse_route_called = false;
  Serial << F("A route which required ") << GetPointNumberDisplay(*point) << F(" to be in the reverse position has been cleared.") << endl;
}

void ProcessDetectedNormal(int pointNumber, char pointEnd) {
  // The points have been detected normal
  point* point = GetPointFromInternalNumber(pointNumber);
  (pointEnd == 'A' ? point->detected_normal_A_end : point->detected_normal_B_end) = true;
  EvaluatePointSwitchDetectionIndications(*point);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" ") << pointEnd << F(" end detected normal.") << endl;
}

void ProcessNormalDetectionLost(int pointNumber, char pointEnd) {
  // The points are no longer detected normal
  point* point = GetPointFromInternalNumber(pointNumber);
  (pointEnd == 'A' ? point->detected_normal_A_end : point->detected_normal_B_end) = false;
  EvaluatePointSwitchDetectionIndications(*point);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" ") << pointEnd << F(" end normal detection lost.") << endl;
}

void ProcessDetectedReverse(int pointNumber, char pointEnd) {
  // The points have been detected reverse
  point* point = GetPointFromInternalNumber(pointNumber);
  (pointEnd == 'A' ? point->detected_reverse_A_end : point->detected_reverse_B_end) = true;
  EvaluatePointSwitchDetectionIndications(*point);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" ") << pointEnd << F(" end detected reverse.") << endl;
}

void ProcessReverseDetectionLost(int pointNumber, char pointEnd) {
  // The points are no longer detected reverse
  point* point = GetPointFromInternalNumber(pointNumber);
  (pointEnd == 'A' ? point->detected_reverse_A_end : point->detected_reverse_B_end) = false;
  EvaluatePointSwitchDetectionIndications(*point);
  Serial << F("> Point ") << GetPointNumberDisplay(*point) << F(" ") << pointEnd << F(" end reverse detection lost.") << endl;
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