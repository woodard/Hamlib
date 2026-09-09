/*
 *  Hamlib TH-D75 Dual Band (DL command) hardware test
 *  Copyright (c) 2026 by Ben Woodard AE6BC
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * Hardware test program for TH-D75 Dual Band (DL command) functionality.
 * Tests the RIG_FUNC_DUAL_BAND implementation via set_func/get_func.
 *
 * This test is designed to be run with an actual TH-D75 radio connected
 * to /dev/ttyACM0. It enables trace debugging to help diagnose any issues.
 *
 * Usage: ./test_thd75_dual_band_hardware /dev/ttyACM0
 *
 * Compile: gcc -o test_thd75_dual_band_hardware test_thd75_dual_band_hardware.c \
 *          $(pkg-config --cflags --libs hamlib)
 */

#ifdef HAVE_CONFIG_H
#  include "hamlib/config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hamlib/rig.h>

/* Print usage information */
static void usage(const char *progname)
{
    fprintf(stderr, "Usage: %s <serial_device>\n", progname);
    fprintf(stderr, "Example: %s /dev/ttyACM0\n", progname);
}

/* Test the RIG_FUNC_DUAL_BAND feature */
static int test_dual_band_feature(RIG *rig)
{
    int retval;
    int status;
    int test_result = 0;

    printf("\n=== Testing RIG_FUNC_DUAL_BAND Feature ===\n\n");

    /* Step 1: Get current state */
    printf("Step 1: Getting current Dual Band state...\n");
    retval = rig_get_func(rig, RIG_VFO_A, RIG_FUNC_DUAL_BAND, &status);
    if (retval != RIG_OK) {
        printf("  ERROR: Failed to get func: %d (%s)\n", retval, rigerror(retval));
        test_result = 1;
    } else {
        printf("  Current Dual Band state: %d\n", status);
    }

    /* Step 2: Enable Dual Band */
    printf("\nStep 2: Enabling Dual Band...\n");
    rig_debug(RIG_DEBUG_TRACE, "Setting RIG_FUNC_DUAL_BAND = 1\n");
    retval = rig_set_func(rig, RIG_VFO_A, RIG_FUNC_DUAL_BAND, 1);
    if (retval != RIG_OK) {
        printf("  ERROR: Failed to set func (enable): %d (%s)\n", retval, rigerror(retval));
        test_result = 1;
    } else {
        printf("  Successfully enabled Dual Band\n");
    }

    /* Step 3: Verify it's enabled */
    printf("\nStep 3: Verifying Dual Band is enabled...\n");
    retval = rig_get_func(rig, RIG_VFO_A, RIG_FUNC_DUAL_BAND, &status);
    if (retval != RIG_OK) {
        printf("  ERROR: Failed to get func: %d (%s)\n", retval, rigerror(retval));
        test_result = 1;
    } else {
        printf("  Dual Band state: %d\n", status);
        if (status == 1) {
            printf("  SUCCESS: Dual Band is enabled as expected\n");
        } else {
            printf("  FAIL: Dual Band is not enabled (got %d, expected 1)\n", status);
            test_result = 1;
        }
    }

    /* Step 4: Disable Dual Band */
    printf("\nStep 4: Disabling Dual Band...\n");
    rig_debug(RIG_DEBUG_TRACE, "Setting RIG_FUNC_DUAL_BAND = 0\n");
    retval = rig_set_func(rig, RIG_VFO_A, RIG_FUNC_DUAL_BAND, 0);
    if (retval != RIG_OK) {
        printf("  ERROR: Failed to set func (disable): %d (%s)\n", retval, rigerror(retval));
        test_result = 1;
    } else {
        printf("  Successfully disabled Dual Band\n");
    }

    /* Step 5: Verify it's disabled */
    printf("\nStep 5: Verifying Dual Band is disabled...\n");
    retval = rig_get_func(rig, RIG_VFO_A, RIG_FUNC_DUAL_BAND, &status);
    if (retval != RIG_OK) {
        printf("  ERROR: Failed to get func: %d (%s)\n", retval, rigerror(retval));
        test_result = 1;
    } else {
        printf("  Dual Band state: %d\n", status);
        if (status == 0) {
            printf("  SUCCESS: Dual Band is disabled as expected\n");
        } else {
            printf("  FAIL: Dual Band is not disabled (got %d, expected 0)\n", status);
            test_result = 1;
        }
    }

    /* Step 6: Toggle multiple times to test stability */
    printf("\nStep 6: Testing stability with multiple toggles...\n");
    for (int i = 0; i < 3; i++) {
        printf("  Toggle %d: ", i + 1);
        
        /* Enable */
        retval = rig_set_func(rig, RIG_VFO_A, RIG_FUNC_DUAL_BAND, 1);
        if (retval != RIG_OK) {
            printf("FAIL (enable)\n");
            test_result = 1;
            continue;
        }

        /* Disable */
        retval = rig_set_func(rig, RIG_VFO_A, RIG_FUNC_DUAL_BAND, 0);
        if (retval != RIG_OK) {
            printf("FAIL (disable)\n");
            test_result = 1;
            continue;
        }

        printf("OK\n");
    }

    return test_result;
}

int main(int argc, char *argv[])
{
    RIG *rig;
    char *device;
    int retval;

    printf("TH-D75 Dual Band Hardware Test\n");
    printf("================================\n");

    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    device = argv[1];
    printf("Device: %s\n", device);

    /* Initialize hamlib */
    rig_load_all_backends();

    /* Initialize the rig */
    rig = rig_init(RIG_MODEL_THD75);
    if (!rig) {
        fprintf(stderr, "Failed to initialize rig\n");
        return EXIT_FAILURE;
    }

    /* Enable trace debugging as requested */
    rig_set_debug_level(RIG_DEBUG_TRACE);
    printf("Debug level set to RIG_DEBUG_TRACE\n");

    /* Set the serial port device */
    rig_set_conf(rig, rig_token_lookup(rig, "rig_pathname"), device);

    /* Open the radio */
    printf("\nOpening radio on %s...\n", device);
    rig_debug(RIG_DEBUG_TRACE, "Attempting to open radio on %s\n", device);
    retval = rig_open(rig);
    if (retval != RIG_OK) {
        fprintf(stderr, "Failed to open radio: %d (%s)\n", retval, rigerror(retval));
        rig_cleanup(rig);
        return EXIT_FAILURE;
    }
    printf("Radio opened successfully\n");

    /* Run the test */
    retval = test_dual_band_feature(rig);

    /* Close the radio */
    printf("\nClosing radio...\n");
    rig_close(rig);
    rig_cleanup(rig);

    /* Print results */
    printf("\n=== Test Results ===\n");
    if (retval == 0) {
        printf("All tests PASSED\n");
    } else {
        printf("Some tests FAILED\n");
    }

    return retval ? EXIT_FAILURE : EXIT_SUCCESS;
}
