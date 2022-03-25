# EMMS_Code_CommandBoard
MPLAB X Project
- Chip - PIC24FV32KA302
- Compiler - XC16 1.61

# ToDo
- nothing at this time

# Changelog
### NEXT
- increased MAX_PARAMETER_LENGTH to account for ModInfo and NULL_CHAR

### v2.6.2
March 20, 2022<br />
Tom Austin
#### Changes
- Ensured any parameters "Set" were immediately available to be "Read"
- AlarmXEnabled since it was directly derived from AlarmXThreshold
  - removed it from the communication commands
- Feature - Meter Name
  - Update communications to handle Meter Name (MName)
  - stored in EEPROM
- Command PwrData - temporary variable names fixed
  - energyEmergencyAdderBuf was wrong -- energyCycleAllocationBuf is correct
- Cleanup - removed some items no longer used
  - PSVersion



### v2.6.1
March 19, 2022<br />
Tom Austin
#### Changes
- New versioning and changelog scheme
## 
### Pre v2.6.1
#### Previous changes are not currently listed
