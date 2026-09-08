/*
 *  Hamlib TH-D75 Dual Band (DL command) tests
 *  Copyright (c) 2026 by Ben Woodard AE6BC with the help of Goose AI
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
/* SPDX-License-Identifier: LGPL-2.1-or-later */

/*
 * Unit tests for TH-D75 Dual Band (DL command) functionality.
 * Tests the RIG_FUNC_DUAL_BAND implementation via set_func/get_func.
 */

#ifdef HAVE_CONFIG_H
#  include "hamlib/config.h"
#endif

#include "acutest.h"
#include "test_debug.h"
#include <hamlib/rig.h>
#include <string.h>

/* Helper: initialize and open a TH-D75 rig. Returns NULL on failure. */
static RIG *open_thd75(void)
{
    rig_load_all_backends();

    RIG *rig = rig_init(RIG_MODEL_THD75);

    if (!rig)
    {
        return NULL;
    }

    // No additional setup needed for function tests

    if (rig_open(rig) != RIG_OK)
    {
        rig_cleanup(rig);
        return NULL;
    }

    return rig;
}

/* Helper: close and clean up a rig. */
static void close_thd75(RIG *rig)
{
    if (rig)
    {
        rig_close(rig);
        rig_cleanup(rig);
    }
}

/* ------------------------------------------------------------------ */
/* Tests for RIG_FUNC_DUAL_BAND (DL command)                          */
/* ------------------------------------------------------------------ */

static void test_thd75_dual_band_supported(void)
{
    RIG *rig = open_thd75();
    TEST_ASSERT(rig != NULL);

    /* Verify that RIG_FUNC_DUAL_BAND is in the supported functions */
    int funcs = rig->caps->has_set_func;
    TEST_CHECK((funcs & RIG_FUNC_DUAL_BAND) != 0);

    close_thd75(rig);
}

static void test_thd75_dual_band_disable(void)
{
    RIG *rig = open_thd75();
    TEST_ASSERT(rig != NULL);

    /* Test disabling Dual Band (DL 0;) */
    int retval = rig_set_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, 0);
    TEST_CHECK(retval == RIG_OK);

    /* Verify the state was set */
    int status = 0;
    retval = rig_get_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, &status);
    TEST_CHECK(retval == RIG_OK);
    TEST_CHECK(status == 0);

    close_thd75(rig);
}

static void test_thd75_dual_band_enable(void)
{
    RIG *rig = open_thd75();
    TEST_ASSERT(rig != NULL);

    /* Test enabling Dual Band (DL 1;) */
    int retval = rig_set_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, 1);
    TEST_CHECK(retval == RIG_OK);

    /* Verify the state was set */
    int status = 0;
    retval = rig_get_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, &status);
    TEST_CHECK(retval == RIG_OK);
    TEST_CHECK(status == 1);

    close_thd75(rig);
}

static void test_thd75_dual_band_toggle(void)
{
    RIG *rig = open_thd75();
    TEST_ASSERT(rig != NULL);

    /* Start with Dual Band disabled */
    int retval = rig_set_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, 0);
    TEST_CHECK(retval == RIG_OK);

    /* Enable it */
    retval = rig_set_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, 1);
    TEST_CHECK(retval == RIG_OK);

    int status = 0;
    retval = rig_get_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, &status);
    TEST_CHECK(retval == RIG_OK);
    TEST_CHECK(status == 1);

    /* Disable it again */
    retval = rig_set_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, 0);
    TEST_CHECK(retval == RIG_OK);

    retval = rig_get_func(rig, RIG_VFO_CURR, RIG_FUNC_DUAL_BAND, &status);
    TEST_CHECK(retval == RIG_OK);
    TEST_CHECK(status == 0);

    close_thd75(rig);
}

/* ------------------------------------------------------------------ */
/* Test list for acutest framework                                  */
/* ------------------------------------------------------------------ */

TEST_LIST =
{
    { "thd75_dual_band_supported", test_thd75_dual_band_supported },
    { "thd75_dual_band_disable",   test_thd75_dual_band_disable },
    { "thd75_dual_band_enable",    test_thd75_dual_band_enable },
    { "thd75_dual_band_toggle",    test_thd75_dual_band_toggle },
    { NULL, NULL }
};
