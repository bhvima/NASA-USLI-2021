# NASA-USLI-2021

## Challenge

Teams shall design a payload capable of autonomously locating the launch vehicle upon landing by identifying the launch vehicle’s grid position on an aerial image of the
launch site without the use of a global positioning system (GPS). The method(s)/design(s) utilized to complete the payload mission will be at the teams’ discretion and will be permitted so long as the
designs are deemed safe, obey FAA and legal requirements, and adhere to the intent of the challenge.

## Requirements

* The payload will be capable of remaining in launch-ready configuration on the pad for a minimum of 2 hours without losing the functionality of any critical on-board components, although
the capability to withstand longer delays is highly encouraged.

* GPS shall not be used to aid in any part of the payload mission.
  * GPS coordinates of the launch vehicles landing location shall be known and used solely for the purpose of verification of payload functionality and mission success.

* No external hardware or software is permitted outside the team’s prep area or the launch vehicle itself prior to launch.

## Solution

The payload will be capable of autonomously locating the launch vehicle upon landing and identify its grid position on an aerial image of the launch field (with the dimension of each grid box equal to or less than 250 feet by 250 feet) without the use of a global positioning system (GPS). The payload will consist of an Inertial Navigation System (INS), radio and a power source. Using the measurements from the onboard sensor the payload will compute its relative position to the launch site. There will be a constant downlink of the data via the radio to the ground station. The criterion for success is defined by accurate calculation of the launch vehicles current position within the range of the launch field (2500 feet in every direction), downlink of telemetry data and safe recovery of payload.

## Circuit Diagram

![Schematic](files/circuit.png)

## References

1. <https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf> 
