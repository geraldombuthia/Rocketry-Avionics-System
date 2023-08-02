void updateButtonState() {
  // Read the current state of the button
  bool reading = digitalRead(btn);

  // Check if the button state has changed
  if (reading != lastButtonState) {
    // Reset the debounce timer
    lastDebounceTime = millis();
  }

  // Check if the debounce delay has passed
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Update the button state if it has stabilized
    if (reading != buttonState) {
      buttonState = reading;
      // Perform any desired actions based on the button state change
      if (buttonState == false) {
        // Button is pressed, change the variable state
        // For example:
        store = !store;
        Serial.println(store);
        // myVariable = false;
      }
    }
  }

  // Update the last button state
  lastButtonState = reading;
}
