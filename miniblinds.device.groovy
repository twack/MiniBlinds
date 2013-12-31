//*****************************************************************************
/// Sketch:  MiniBlinds.groovy
/// Author:  twack@wackware.net
/// Version: 0.1
/// Date:    2013-10-11
/// Purpose: Operate Mini Blinds using Arduino and SmartThings Shield
///
/// Use License: Non-Profit Open Software License version 3.0 (NPOSL-3.0)
///              http://opensource.org/licenses/NPOSL-3.0
///
//*****************************************************************************
///                                Change Log
/// Version:    0.2
///   Date:     20131013
///   Change1:  Fixed null error when parsing returned message from device
///   Change2:  Added change log
///
/// Version:    0.1
///   Date:     20131012
///   Change1:  Initial Release
///
//*****************************************************************************
///
/// Device Type:	Switch
///
/// Commands:		openBlinds
///					closeInside
///					closeOutside
///
/// Attributes:		openedState
///					closedInsideState
///					closedOutsideState
///
/// To-Do's:		* Change tiles to miniblinds when ST has them.
///					* Change sketch to be able to get angles from app and
///					  store in memory. Update via ST app install UI then.
//*****************************************************************************


metadata {
	// Simulator metadata


	// UI tile definitions
	tiles {
		standardTile("open", "device.openedState", width: 2, height: 2, canChangeIcon: true, canChangeBackground: true) {
			state "no", label: "Closed", action: "openBlinds", icon: "st.Home.home9", backgroundColor: "#8B8989"
			state "yes", label: "Open", action: "closeInside", icon: "st.Home.home9", backgroundColor: "#ffa81e"
		}
        standardTile("closeInside", "device.closedInsideState", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
            state "no", label: 'Inward', action: "closeInside", icon: "st.Home.home9", backgroundColor: "#8B8989"
            state "yes", label: 'Inward', action: "closeInside", icon: "st.Home.home9", backgroundColor: "#79b821"
		}
        standardTile("closeOutside", "device.closedOutsideState", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
            state "no", label: 'Outward', action: "closeOutside", icon: "st.Home.home9", backgroundColor: "#8B8989"
            state "yes", label: 'Outward', action: "closeOutside", icon: "st.Home.home9", backgroundColor: "#79b821"
		}

		main "open"
        details(["open", "closeInside", "closeOutside"])
	}
}

// Parse incoming device messages to generate events
def parse(String description) {

	log.debug "Parsing '${description}'"

	def arduinoMsg = zigbee.parse(description)?.text

	def parts = arduinoMsg.split(":");
    
    def myState = "${parts[0]}"
    
    if (myState == "openedState" || myState == "closedInsideState" || myState == "closedOutsideState"){
	  def myValue = "${parts[1]}" //Bug, can't use the parts[1] array in the createEvent value
      return createEvent(name: myState, value: myValue) 
    }
}

// Commands sent to the device
def openBlinds() {
	zigbee.smartShield(text: "openBlinds").format()
}

def closeInside() {
	zigbee.smartShield(text: "closeInside").format()
}

def closeOutside() {
	zigbee.smartShield(text: "closeOutside").format()
}
