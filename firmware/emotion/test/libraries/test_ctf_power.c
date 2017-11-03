
#include <stdint.h>
#include "CUnit/Basic.h"
#include "ctf_power.h"
#include "ctf_offset.h"

static void init_ctf_offset()
{
    // Send 10 calibration pages which sets the Offset to 590.
    static const uint8_t page[] = { 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x02, 0x4E };
    for (int i = 0; i < 10; i++)
        ctf_set_calibration_page((ant_bp_ctf_calibration_t*)page);
}

static int init()
{
    init_ctf_offset();

    // Power must be calculated based on current message and a previous message.
    static const uint8_t previous_page[] = { 0x20, 0xD5, 0x00, 0xD5, 0xD6, 0x61, 0x79, 0xAE };
    static const uint8_t current_page[] = { 0x20, 0xD6, 0x00, 0xD5, 0xDC, 0x84, 0x7C, 0x36 };
    
    ctf_set_main_page((ant_bp_ctf_t*)previous_page);
    ctf_set_main_page((ant_bp_ctf_t*)current_page);
    
    return 0;
}

static void test_ctf_get_power()
{
    uint16_t watts = ctf_get_power();
    CU_ASSERT(watts == 260);
}

static void test_ctf_get_offset()
{
    CU_ASSERT(getCtfOffset() == 590);
}

void tests_ctf_power()
{
    // Add suite.
    CU_pSuite pSuite = CU_add_suite("ctf_power_tests", init, NULL);

    // Add tests.
    CU_ADD_TEST(pSuite, test_ctf_get_offset);
    CU_ADD_TEST(pSuite, test_ctf_get_power);
}