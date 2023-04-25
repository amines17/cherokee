#include <criterion/criterion.h>
#include "../src/headers/content_type.h"
#include "../src/lib/content_type.c"


Test(get_content_type_from_filename, valid_filename)
{
    // Test a valid filename with valid extension (".json").
   const char* filename = get_content_type_from_filename("text.json");
   cr_assert_str_eq(filename, "application/json");

}

Test(get_content_type_from_filename, invalid_filename)
{
    // Test an invalid filename with invalid extension (".json").
   const char* result = get_content_type_from_filename("");
   cr_assert_str_eq(result, "text/plain");
   
}

Test(get_content_type_from_extension, valid_extension)
{
    init_global_content_struct();

    // Test a valid valid extension (".html").
   const char* extension = get_content_type_from_extension(".html");
   cr_assert_str_eq(extension, "text/html");

}

Test(get_content_type_from_extension, invalid_extension)
{
    init_global_content_struct();
    // Test an invalid extension (""").
   const char* result = get_content_type_from_extension(".test");
   cr_assert_str_eq(result, "text/plain");
   
}