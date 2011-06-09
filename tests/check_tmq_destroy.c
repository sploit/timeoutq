#include <timeout_queue.h>

#include <check.h>

/** Positive Test Cases for TMQ Destroy
 */
START_TEST (test_positive_tmq_destroy)
{
    struct tmq *tmq = tmq_create ();
    int ret = tmq_destroy (tmq);
    fail_unless ((ret == 0), "tmq_destroy () failed");
}
END_TEST


/** Negative Test Cases for TMQ Destroy
 */
START_TEST (test_negative_tmq_destroy)
{
    int ret = tmq_destroy (NULL);
    fail_unless ((ret < 0), "tmq_destroy (NULL) failed");
}
END_TEST

/** Build the TMQ Test Suite
 */
Suite *
positive_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Positive - tmq_destroy");
    TCase *tc = tcase_create ("Positive");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_positive_tmq_destroy);

    return s;
}

Suite *
negative_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Negative - tmq_destroy");
    TCase *tc = tcase_create ("Negative");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_negative_tmq_destroy);

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

    /** Positive */
    s = positive_suite ();
    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    n = srunner_ntests_failed (sr);
    srunner_free (sr);

    /** Negative */
    s = negative_suite ();
    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    n = srunner_ntests_failed (sr);
    srunner_free (sr);



    return (n == 0) ? 0 : 1;
}
