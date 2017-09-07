#include <Arduino.h>
#include <OSButtons.h>

#define NO_PIN -1

void OSButtons::setup(int clickTimeout) {
  _clickTimeout = clickTimeout;

  for (int i = 0; i < MAX_BUTTONS; i++) {
    _buttonMap[i]         = NO_PIN;
    _buttonActiveState[i] = LOW;
    _buttonLastState[i]   = 0;
  }
}

void OSButtons::registerButton(int pin, int activeState)
{
  for (int i = 0; i < MAX_BUTTONS; i++) {
    if (_buttonMap[i] == NO_PIN) {
      _buttonMap[i]         = pin;
      _buttonActiveState[i] = activeState;
      break;
    }
  }
}

void OSButtons::handleClick(int pin, int index) {
  bool consumed = false;

  // button is active
  if (digitalRead(pin) == _buttonActiveState[index]) {
    // click is enabled (no timeout)
    if (_buttonLastState[index] == 0) {
      // set timeout
      _buttonLastState[index] = _clickTimeout;

      // send click event
      for (int i = 0; i < MAX_BUTTON_CALLBACKS; i++) {
        if ((_buttonCallbacks[index][i] != NULL) && !consumed) {
          consumed = _buttonCallbacks[index][i](pin);
        }
      }
    } else if (_buttonLastState[index] > 0) {
      // reduce timeout
      _buttonLastState[index]--;
    }
  }
}

void OSButtons::handleButtonClicks() {
  for (int i = 0; i < MAX_BUTTONS; i++) {
    if (_buttonMap[i] != NO_PIN) {
      handleClick(_buttonMap[i], i);
    }
  }
}

int OSButtons::registerButtonClick(int pin, bool (*callback)(int)) {
  int index = -1;

  for (int i = 0; i < MAX_BUTTONS; i++) {
    if (_buttonMap[i] == pin) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    return -1; // pin not found in map
  }

  for (int i = 0; i < MAX_BUTTON_CALLBACKS; i++) {
    if (_buttonCallbacks[index][i] == NULL) {
      _buttonCallbacks[index][i] = callback;
      return i;
    }
  }

  return -2; // no callbacks found
}
