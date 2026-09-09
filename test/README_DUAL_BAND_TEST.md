# TH-D75 RIG_FUNC_DUAL_BAND Hardware Test

## Overview

This document describes a test program for verifying the RIG_FUNC_DUAL_BAND feature on the Kenwood TH-D75 radio.

## Test Program

**File**: `test_thd75_dual_band_hardware.c`  
**Location**: `/home/woodard/src/Hamlib/test/`

## Features Tested

The test program verifies:

1. **Getting Dual Band state** - Queries the current DL command status
2. **Setting Dual Band ON** - Enables dual band monitoring via "DL 1"
3. **Verifying ON** - Confirms the state was set correctly
4. **Setting Dual Band OFF** - Disables dual band monitoring via "DL 0"
5. **Verifying OFF** - Confirms the state was cleared
6. **Stability Test** - Multiple toggle cycles to ensure reliability

## Requirements

- TH-D75 radio connected to `/dev/ttyACM0`
- User must have read/write access to the serial port (typically via dialout group)
- Hamlib library compiled with the RIG_FUNC_DUAL_BAND feature

## Compilation

```bash
cd /home/woodard/src/Hamlib
gcc -o test_thd75_dual_band_hardware test/test_thd75_dual_band_hardware.c \
    $(pkg-config --cflags --libs hamlib)
```

## Usage

```bash
./test_thd75_dual_band_hardware /dev/ttyACM0
```

## Debug Mode

The program uses `rig_set_debug_level(RIG_DEBUG_TRACE)` to provide detailed communication tracing between the program and the radio. This is especially useful for diagnosing any protocol issues.

Example output showing the DL command:
```
kenwood_transaction called cmd=DL
0000    44 4c 0d                                            DL.             
0000    44 4c 20 30 0d                                      DL 0.           
kenwood_transaction: read_string len=5 'DL 0\r'
```

## Protocol

**DL Command (Dual Band Monitoring)**:
- **Set**: Send `"DL 0"` or `"DL 1"` to disable/enable dual band mode
- **Get**: Send `"DL"` and expect `"DL 0\r"` or `"DL 1\r"` response
- **Response Format**: 5 bytes (4 printable characters + carriage return)

## Expected Results

When the feature is working correctly, you should see:
```
=== Testing RIG_FUNC_DUAL_BAND Feature ===

Step 1: Getting current Dual Band state...
  Current Dual Band state: 0

Step 2: Enabling Dual Band...
  Successfully enabled Dual Band

Step 3: Verifying Dual Band is enabled...
  Dual Band state: 1
  SUCCESS: Dual Band is enabled as expected

Step 4: Disabling Dual Band...
  Successfully disabled Dual Band

Step 5: Verifying Dual Band is disabled...
  Dual Band state: 0
  SUCCESS: Dual Band is disabled as expected

Step 6: Testing stability with multiple toggles...
  Toggle 1: OK
  Toggle 2: OK
  Toggle 3: OK

=== Test Results ===
All tests PASSED
```

## Troubleshooting

### Permission Denied
- Ensure your user is in the `dialout` group: `groups | grep dialout`
- Add user to dialout group: `sudo usermod -a -G dialout $USER`

### Feature Not Available (Error -11)
- Verify hamlib was compiled with RIG_FUNC_DUAL_BAND support
- Check that the header file has RIG_FUNC_DUAL_BAND defined as bit 49
- Ensure the library is properly installed

### No Radio Response
- Check physical connection to /dev/ttyACM0
- Verify radio is powered on
- Check radio settings (some radios require specific initialization)

### Trace Output Too Verbose
- The test uses RIG_DEBUG_TRACE which produces extensive output
- Filter with grep if needed: `./test_thd75_dual_band_hardware /dev/ttyACM0 2>&1 | grep DL`

## Modifications

If you need to modify the test:
- Change `RIG_VFO_A` to `RIG_VFO_B` to test different VFO behavior
- Modify the number of toggle iterations in Step 6
- Add additional verification steps as needed

## License

This test program is part of Hamlib and is distributed under the GNU Lesser General Public License (LGPL) version 2.1 or later.