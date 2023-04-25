
#include <criterion/criterion.h>
#include "../src/headers/return_code.h"
#include "../src/lib/return_code.c"


Test(get_return_code_string, test_success_valid_return_code)
{
    // Initialize the return code tree.
    init_return_code_global_struct();

    // Test a valid return code (200).
    const char *res = get_return_code_string(200);
    cr_assert_str_eq(res, "OK");

    // Test a valid return code (201).
    res = get_return_code_string(201);
    cr_assert_str_eq(res, "Created");

}

Test(get_return_code_string, test_fail_invalid_return_code)
{
    // Initialize the return code tree.
    init_return_code_global_struct();

    // Test an invalid return code (203).
    const char *res = get_return_code_string(203);
    cr_assert_null(res);

    // Test an invalid return code (555).
    res = get_return_code_string(555);
    cr_assert_null(res);
}