/*
 * Test program for TH-D75 FO (Frequency/VFO Output) command parsing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hamlib/rig.h>

int main(int argc, char *argv[])
{
    RIG *rig;
    char *device = "/dev/ttyACM0";
    int retval;
    char reply[256];
    struct thd7x_fo_record record;

    if (argc > 1) {
        device = argv[1];
    }

    printf("TH-D75 FO Command Diagnostic\n");
    printf("Device: %s\n\n", device);

    rig_load_all_backends();
    rig = rig_init(RIG_MODEL_THD75);

    if (!rig) {
        fprintf(stderr, "Failed to initialize rig\n");
        return 1;
    }

    rig_set_debug_level(RIG_DEBUG_TRACE);
    rig_set_conf(rig, rig_token_lookup(rig, "rig_pathname"), device);

    retval = rig_open(rig);
    if (retval != RIG_OK) {
        fprintf(stderr, "Failed to open radio: %d (%s)\n", retval, rigerror(retval));
        rig_cleanup(rig);
        return 1;
    }

    printf("Radio opened successfully\n\n");

    printf("=== Test 1: Send FO command and show raw response ===\n");
    retval = kenwood_transaction(rig, "FO 0", reply, sizeof(reply));
    printf("Result: %d\n", retval);
    printf("Reply: '%s' (length %zu)\n\n", reply, strlen(reply));

    printf("=== Test 2: Parse FO response with thd7x_parse_fo ===\n");
    retval = thd7x_parse_fo(reply, strlen(reply), &record);
    printf("Parse result: %d\n", retval);
    if (retval == RIG_OK) {
        printf("  Frequency: %llu Hz\n", (unsigned long long)record.frequency_hz);
        printf("  Band: %u\n", record.band);
        printf("  Mode: %u\n", record.mode);
        printf("  Tone Enabled: %u\n", record.tone_enabled);
        printf("  CTCSs Enabled: %u\n", record.ctcss_enabled);
        printf("  DCS Enabled: %u\n", record.dcs_enabled);
        printf("  Cross Enabled: %u\n", record.cross_enabled);
        printf("  Shift: %u\n", record.shift);
    } else {
        printf("Parse failed with error: %d\n", retval);
    }
    printf("\n");

    printf("=== Test 3: Test FO for Band B (FO 1) ===\n");
    retval = kenwood_transaction(rig, "FO 1", reply, sizeof(reply));
    printf("Result: %d\n", retval);
    printf("Reply: '%s' (length %zu)\n\n", reply, strlen(reply));

    printf("=== Test 4: Test signal strength from FO record ===\n");
    retval = rig_get_level(rig, RIG_VFO_A, RIG_LEVEL_RAWSTR, &((value_t){0}));
    printf("Get RAWSTR result: %d\n", retval);
    
    printf("\n=== Test Complete ===\n");

    rig_close(rig);
    rig_cleanup(rig);

    return 0;
}