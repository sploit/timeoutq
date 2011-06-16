#include <timeout_queue.h>

#include <check.h>

/** Test Cases for TMQ Start
 */

/** NULL struct tmq *
 */
START_TEST(test_tmq_start_null)
{
    int ret = tmq_start (NULL);

    fail_unless ((ret == -1), "tmq_start (NULL)");
}
END_TEST

/** Negative Tests 
 */
Suite *
negative_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Negatvie - tmq_start");
    TCase *tc = tcase_create ("Negative");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_start_null);

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

    s = negative_suite ();
    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    n = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (n == 0) ? 0 : 1;
}
