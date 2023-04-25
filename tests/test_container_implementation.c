#include <criterion/criterion.h>
#include "../src/headers/container_implementation.h"
#include "../src/lib/container_implementation.c"

Test(tree_create, create_tree)
{
    tree_t* result = tree_create("html", "text/html");

    cr_assert_str_eq(result->extension, "html");
    cr_assert_str_eq(result->value, "text/html");
    cr_assert_null(result->fils_gauche);
    cr_assert_null(result->fils_droite);

    free(result->extension);
    free(result->value);
    free(result);
}

Test(tree_add, add_tree)
{
    const char* ext = ".txt";
    const char* val = "text/plain";

    tree_t* tree = NULL;

    tree_add(tree, ext, val);

    cr_assert_null(tree);
}

Test(tree_delete, delete_tree) 
{
    tree_t *t = tree_create("txt", "text/plain");

    tree_delete(&t);

    cr_assert_null(t);
}

Test(tree_delete, delete_treewith_multiple_nodes)
{
    tree_t *t = tree_create("txt", "text/plain");
    tree_add(t, "html", "text/html");
    tree_add(t, "css", "text/css");

    tree_delete(&t);

    cr_assert_null(t);
}

Test(tree_delete_v1, null_tree) 
{
    tree_t* root = NULL;

    tree_delete_v1(root);

    cr_assert_null(root);
}