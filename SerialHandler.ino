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
  Serial << F("Number   Switch  Route          Tracks   CanMoveN CanMoveR DetNA DetNB DetRA DetRB") << endl;

  for (counter = 0; counter < items; counter++) {
    Serial << GetPointNumberDisplay(points[counter]);
    Serial << F(" ");
    Serial << GetPointSwitchPositionDisplay(points[counter]);
    Serial << F(" ");
    Serial << GetRouteCalledDisplay(points[counter]);
    Serial << F(" ");
    Serial << F(IsTrackCircuitOverPointOccupied(points[counter]) ? "Occupied" : "Clear   ");
    Serial << F(" ");
    Serial << F(CanPointMoveToNormal(points[counter]) ? "Yes" : "No ");
    Serial << F("      ");
    Serial << F(CanPointMoveToReverse(points[counter]) ? "Yes" : "No ");


    Serial << F("      ");
    Serial << F(points[counter].detected_normal_A_end ? "Yes" : "No ");
    Serial << F("   ");
    Serial << F(points[counter].detected_normal_B_end ? "Yes" : "No ");
    Serial << F("   ");
    Serial << F(points[counter].detected_reverse_A_end ? "Yes" : "No ");
    Serial << F("   ");
    Serial << F(points[counter].detected_reverse_B_end ? "Yes" : "No ");

    Serial << endl;
  }
  Serial << endl;
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
  Serial << endl;
}