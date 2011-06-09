#include <timeout_queue.h>
#include <unistd.h>

#include <check.h>

/** Test Cases for TMQ Element Create
 */

/** NULL key
 */
START_TEST(test_tmq_element_create_null_size)
{
    struct tmq_element *ret;
    ret = tmq_element_create (NULL, 4);
    fail_unless ((ret == NULL), "tmq_element_create (NULL, 4)");
}
END_TEST


/** Zero Sized Key 
 */
START_TEST(test_tmq_element_create_key_zero)
{
    struct tmq_element *ret;
    int key = 0;
    ret = tmq_element_create (&key, 0);
    fail_unless ((ret == NULL), "tmq_element_create (&key, 0)");
}
END_TEST

/** NULL key AND Zero Sized Key 
 */
START_TEST(test_tmq_element_create_null_zero)
{
    struct tmq_element *ret;
    ret = tmq_element_create (NULL, 0);
    fail_unless ((ret == NULL), "tmq_element_create (NULL, 0)");
}
END_TEST

/** Build the TMQ Test Suite
 */
Suite *
make_tmq_insert_suite (void)
{
    Suite *s = suite_create ("timeout queue element create");
    TCase *tc = tcase_create ("Core");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_element_create_null_size);
    tcase_add_test (tc, test_tmq_element_create_key_zero);
    tcase_add_test (tc, test_tmq_element_create_null_zero);

    return s;
}

/** Build the TMQ Test Suite
 */
int
main (void)
{
    int n;
    Suite *s;
    SRunner *sr;

    s = make_tmq_insert_suite ();
    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    n = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (n == 0) ? 0 : 1;
}
