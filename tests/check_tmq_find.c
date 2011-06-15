#include <timeout_queue.h>

#include <check.h>

/** Test Cases for TMQ Find
 */

/** XXX Necessary function definition
 */
int compare (const void *p1, const void *p2)
{
    int *val1 = (int *)p1,
        *val2 = (int *)p2;

    if (*val1 == *val2)
        return 0;
    return -1;
}

/** NULL struct tmq *
 */
START_TEST(test_tmq_find_null_elem)
{
    struct tmq_element *elem;
    int key = 1;

    elem = tmq_find (NULL, &key);

    fail_unless ((elem == NULL), "tmq_find (NULL, elem)");
}
END_TEST


/** NULL struct tmq_elem *
 */
START_TEST(test_tmq_find_tmq_null)
{
    struct tmq *tmq = tmq_create ();
    tmq->compare = compare;

    struct tmq_element *elem;
    elem = tmq_find (tmq, NULL);

    fail_unless ((elem == NULL), "tmq_find (tmq, NULL)");
}
END_TEST

/** NULL struct tmq *, NULL struct tmq_elem * 
 */
START_TEST(test_tmq_find_null_null)
{
    struct tmq_element *elem;
    elem = tmq_find (NULL, NULL);

    fail_unless ((elem == NULL), "tmq_find (NULL, NULL)");
}
END_TEST


/** Test Case for TMQ Find Single Element
 */
START_TEST(test_tmq_find_one)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    int key = 100;

    tmq->compare = compare;

    elem = tmq_element_create (&key, sizeof(key));

    tmq_insert (tmq, elem);

    elem = tmq_find (tmq, &key);

    fail_unless ((elem != NULL), "Find failed in a table of 1 element");

    tmq_destroy (tmq);
}
END_TEST

/** Test Case for TMQ Insert a Bunch of Elements 
 */
START_TEST(test_tmq_find_a_bunch)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    int key;

    tmq->compare = compare;

    for (key = 0; key < 9000; key++)
    {
        elem = tmq_element_create (&key, sizeof(key));
        tmq_insert (tmq, elem);
    }

    key = 9000 / 2;
    elem = tmq_find (tmq, &key);

    fail_unless ((elem != NULL), "Find failed in a table of 9000 elements");

    tmq_destroy (tmq);
}
END_TEST

/** Find element not in list 
 */
START_TEST(test_tmq_find_nonexisting)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    int key;

    tmq->compare = compare;

    for (key = 0; key < 9000; key++)
    {
        elem = tmq_element_create (&key, sizeof(key));
        tmq_insert (tmq, elem);
    }

    key =  9999; /* key to element which doesn't exist */
    elem = tmq_find (tmq, &key);

    fail_unless ((elem == NULL), "Found non-existing element");

    tmq_destroy (tmq);
}
END_TEST

/** Positive Tests
 */
Suite *
positive_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Positive - tmq_find");
    TCase *tc = tcase_create ("Positive");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_find_one);
    tcase_add_test (tc, test_tmq_find_a_bunch);
    tcase_add_test (tc, test_tmq_find_nonexisting);

    return s;
}

/** Negative Tests 
 */
Suite *
negative_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Negatvie - tmq_find");
    TCase *tc = tcase_create ("Negative");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_find_null_elem);
    tcase_add_test (tc, test_tmq_find_tmq_null);
    tcase_add_test (tc, test_tmq_find_null_null);

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
