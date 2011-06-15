#include <timeout_queue.h>
#include <unistd.h>
#include <check.h>

#define DEFAULT_TIME 1

/** Test Cases for TMQ Timeout
 */

/** NULL struct tmq *
 */
START_TEST(test_tmq_timeout_null)
{
    int ret = tmq_timeout (NULL);
    fail_unless ((ret == -1), "tmq_timeout (NULL)");
}
END_TEST

/** Test Case for TMQ Timeout Single Element 
 */
START_TEST(test_tmq_timeout_one)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    int key = 100;

    elem = tmq_element_create (&key, sizeof(key));
    tmq_insert (tmq, elem);

    sleep (2);

    tmq_timeout (tmq);

    fail_unless ((tmq->size == 0), "Failed to timeout single element");

    tmq_destroy (tmq);
}
END_TEST

/** Test Case for TMQ Insert a Bunch of Elements 
 */
START_TEST(test_tmq_timeout_a_bunch)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    int key;

    for (key = 0; key < 9000; key++)
    {
        elem = tmq_element_create (&key, sizeof(key));
        tmq_insert (tmq, elem);
    }

    sleep (2);

    tmq_timeout (tmq);

    fail_unless ((tmq->size == 0), "Failed to timeout a bunch");

    tmq_destroy (tmq);
}
END_TEST

/** Positive Tests
 */
Suite *
positive_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Positive - tmq_timeout");
    TCase *tc = tcase_create ("Positive");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_timeout_one);
    tcase_add_test (tc, test_tmq_timeout_a_bunch);

    return s;
}

/** Negative Tests 
 */
Suite *
negative_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Negatvie - tmq_timeout");
    TCase *tc = tcase_create ("Negative");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_timeout_null);

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

    s = negative_suite ();
    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    n = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (n == 0) ? 0 : 1;
}
