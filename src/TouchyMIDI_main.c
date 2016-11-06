//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8SB1_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"
// $[Generated Includes]
#include "cslib_config.h"
#include "cslib.h"
// [Generated Includes]$

/**
 * define the LED pins
 */
SI_SBIT (LED1, SFR_P0, 7);
SI_SBIT (LED2, SFR_P1, 6);
SI_SBIT (LED3, SFR_P1, 7);
SI_SBIT (LED4, SFR_P1, 0);
SI_SBIT (LED5, SFR_P0, 0);

/// The order of the buttons
///
uint8_t buttonOrder[] = {8, 3, 2, 4, 0, 1};

/// The notes played by each button.
///
uint8_t buttonNotes[] = {60, 62, 64, 65, 67, 69}; // C-major

/// The number of the buttons.
///
const uint8_t buttonCount = 6;

/// The last state of the touch buttons
///
uint8_t lastButtonState = 0;

/// The last pitch bend value sent.
///
uint8_t lastPitchBend = 0;


/// Send a byte to the serial port synchronous.
///
void serialSendByte(uint8_t byte) {
	// 11111101 = clear the TI bit.
	SCON0 &= 0xfd;
	// Start the transmission.
	SBUF0 = byte;
	// Wait until the byte was sent (TI bit set).
	while ((SCON0 & 0x02) == 0) {
		NOP();
	}
}

/// Get a normalized value for a single touch sensor
///
float getNormalizedTouchValue(uint8_t sensorIndex) {
	float result;
	SensorStruct_t *node;
	float relativeValue;

	node = &CSLIB_node[sensorIndex];
	relativeValue = (float)(node->processBuffer[0]);
	relativeValue -= 64.0;
	relativeValue -= node->currentBaseline;
	relativeValue *= 4.0;
	relativeValue /= 65536.0;
	if (relativeValue >= 1.0) {
		result = 1.0;
	} else if (relativeValue < 0.0) {
		result = 0.0;
	} else {
		result = relativeValue;
	}

	return relativeValue;
}


//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
int main(void) {

	// Variables in ancient C-style
	uint8_t currentButtonState; // The current button state.
	uint8_t deltaButtonState; // The delta between the current and the last button state.
	uint8_t i; // An index variable for the for loop.
	uint8_t buttonMask; // A mask with the current tested bit.
	uint8_t pitchBendValue; // The new pitch bend value.
	float sensorValueUp; // The normalized sensor value.
	float sensorValueDown; // The normalized sensor value.

	// Call hardware initialization routine
	enter_DefaultMode_from_RESET();

	while (1) {
// $[Generated Run-time code]
		// -----------------------------------------------------------------------------
		// If low power features are enabled, this will either put the device into a low
		// power state until it is time to take another scan, or put the device into a
		// low-power sleep mode if no touches are active
		// -----------------------------------------------------------------------------
		CSLIB_lowPowerUpdate();

		// -----------------------------------------------------------------------------
		// Performs all scanning and data structure updates
		// -----------------------------------------------------------------------------
		CSLIB_update();

// [Generated Run-time code]$

		// Generate a bit mask with the current button input states.
		currentButtonState = 0;
		for (i = 0; i < buttonCount; ++i) {
			currentButtonState |= (CSLIB_isSensorDebounceActive(buttonOrder[i]) << i);
		}

		// Check if a button was pressed or released.
		deltaButtonState = (lastButtonState ^ currentButtonState);
		// If there is a change, send the changes as MIDI message
		if (deltaButtonState != 0) {
			for (i = 0; i < buttonCount; ++i) {
				buttonMask = (1 << i);
				// Check for the delta.
				if ((deltaButtonState & buttonMask) != 0) {
					// See if it was a touch down or up.
					if ((currentButtonState & buttonMask) != 0) {
						// Note On
						//serialSendByte(0x00);
						serialSendByte(0x90); // Note ON - Channel 0
						serialSendByte(buttonNotes[i]); // Middle "C" + i
						serialSendByte(0x7f); // Maximum velocity.
					} else {
						// Note Off
						serialSendByte(0x80); // Note OFF - Channel 0
						serialSendByte(buttonNotes[i]); // Middle "C" + i
						serialSendByte(0x7f); // Maximum velocity.
					}
				}
			}
		}

		// Light the first LED if any button is pressed.
		LED1 = (currentButtonState == 0 ? 1 : 0);
		lastButtonState = currentButtonState;

		// Calculate the pitch bend value.
		sensorValueUp = getNormalizedTouchValue(5);
		sensorValueDown = getNormalizedTouchValue(7);
		if (sensorValueUp > 0.0 && sensorValueUp > sensorValueDown) {
			pitchBendValue = 0x40 + (uint8_t)(sensorValueUp * 63.0);
		} else if (sensorValueDown > 0.0 && sensorValueDown > sensorValueUp) {
			pitchBendValue = 0x40 - (uint8_t)(sensorValueDown * 63.0);
		} else {
			pitchBendValue = 0x40; // No pitch bend
		}

		// Check if there is a difference and send the value.
		if (pitchBendValue != lastPitchBend) {
			serialSendByte(0xE0); // Pitch bend value
			serialSendByte(0x00);
			serialSendByte(pitchBendValue);
			lastPitchBend = pitchBendValue;
		}
	}
}
