#include <timeout_queue.h>

#include <check.h>

/** Test Case for TMQ Create
 */
START_TEST (test_positive_tmq_create)
{
    struct tmq *tmq = tmq_create ();
    fail_unless ((tmq != NULL), "tmq_create () failed");
}
END_TEST

/** Build the TMQ Test Suite
 */
Suite *
positive_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Positive - tmq_create");
    TCase *tc = tcase_create ("Positive");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_positive_tmq_create);

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

    s = positive_suite ();
    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    n = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (n == 0) ? 0 : 1;
}
