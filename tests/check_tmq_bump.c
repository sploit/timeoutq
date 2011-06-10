#include <timeout_queue.h>

#include <check.h>

/** Test Cases for TMQ Insert
 */

/** NULL struct tmq *
 */
START_TEST(test_tmq_bump_null_elem)
{
    int ret, key = 1;
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem = tmq_element_create (&key, sizeof(key));

    tmq_insert (tmq, elem);
    ret = tmq_bump (NULL, elem);

    fail_unless ((ret != 0), "tmq_bump (NULL, elem)");
}
END_TEST


/** NULL struct tmq_elem *
 */
START_TEST(test_tmq_bump_tmq_null)
{
    struct tmq *tmq= tmq_create ();

    int ret;
    ret = tmq_bump (tmq, NULL);

    fail_unless ((ret != 0), "tmq_bump (tmq, NULL)");
}
END_TEST

/** NULL struct tmq *, NULL struct tmq_elem * 
 */
START_TEST(test_tmq_bump_null_null)
{
    int ret;
    ret = tmq_bump (NULL, NULL);

    fail_unless ((ret != 0), "tmq_bump (NULL, NULL)");
}
END_TEST

/** Bump an element that wasn't inserted 
 * TODO: framework has no methodology to validate that an element
 * is part of a given set, i.e. this test will fail till then.
 */
#if 0
START_TEST(test_tmq_bump_element_not_in_list)
{
    int ret, key = 1;
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem = tmq_element_create (&key, sizeof(key));

    ret = tmq_bump (tmq, elem);

    fail_unless ((ret != 0), "tmq_bump (element not in list)");
}
END_TEST
#endif


/** Test Case for TMQ Bump Single Element
 */
START_TEST(test_tmq_bump_one)
{
    struct tmq *tmq;
    struct tmq_element *elem;
    int key = 100;

    tmq = tmq_create ();
    elem = tmq_element_create (&key, sizeof(key));
    tmq_insert (tmq, elem);
    tmq_bump (tmq, elem);

    fail_unless ((tmq->size == 1), "Insert one test failed");
    fail_unless ((elem == tmq->head), "Insert one test failed (elem != tmq->head)");
    fail_unless ((elem == tmq->tail), "Insert one test failed (elem != tmq->tail)");
}
END_TEST

/** Test Case for TMQ Insert a Bunch of Elements 
 */
START_TEST(test_tmq_bump_a_bunch)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    int key;

    for (key = 0; key < 9000; key++)
    {
        elem = tmq_element_create (&key, sizeof(key));
        tmq_insert (tmq, elem);
    }

    tmq_bump (tmq, elem);

    fail_unless ((tmq->size == 9000), "Insert a bunch test failed (tmq->size != 9000)");
    fail_unless ((elem == tmq->head), "Insert a bunch test failed (elem != tmq->head)");
    fail_unless ((elem != tmq->tail), "Insert a bunch test failed (elem == tmq->tail)");
}
END_TEST

/** Positive Tests
 */
Suite *
positive_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Positive - tmq_bump");
    TCase *tc = tcase_create ("Positive");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_bump_one);
    tcase_add_test (tc, test_tmq_bump_a_bunch);

    return s;
}

/** Negative Tests 
 */
Suite *
negative_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Negatvie - tmq_bump");
    TCase *tc = tcase_create ("Negative");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_bump_null_elem);
    tcase_add_test (tc, test_tmq_bump_tmq_null);
    tcase_add_test (tc, test_tmq_bump_null_null);

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
