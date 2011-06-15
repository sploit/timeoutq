#include <timeout_queue.h>

#include <check.h>

/** Test Cases for TMQ Pop
 */

/** NULL struct tmq *
 */
START_TEST(test_tmq_pop_null_elem)
{
    int ret, key = 1;
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem = tmq_element_create (&key, sizeof(key));

    tmq_insert (tmq, elem);
    ret = tmq_pop (NULL, elem);

    fail_unless ((ret != 0), "tmq_pop (NULL, elem)");
}
END_TEST


/** NULL struct tmq_elem *
 */
START_TEST(test_tmq_pop_tmq_null)
{
    struct tmq *tmq= tmq_create ();

    int ret;
    ret = tmq_pop (tmq, NULL);

    fail_unless ((ret != 0), "tmq_pop (tmq, NULL)");
}
END_TEST

/** NULL struct tmq *, NULL struct tmq_elem * 
 */
START_TEST(test_tmq_pop_null_null)
{
    int ret;
    ret = tmq_pop (NULL, NULL);

    fail_unless ((ret != 0), "tmq_pop (NULL, NULL)");
}
END_TEST

/** Bump an element that wasn't inserted 
 * TODO: framework has no methodology to validate that an element
 * is part of a given set, i.e. this test will fail till then.
 */
#if 0
START_TEST(test_tmq_pop_element_not_in_list)
{
    int ret, key = 1;
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem = tmq_element_create (&key, sizeof(key));

    ret = tmq_pop (tmq, elem);

    fail_unless ((ret != 0), "tmq_pop (element not in list)");
}
END_TEST
#endif


/** Test Case for TMQ Pop single element out of the list 
 */
START_TEST(test_tmq_pop_one)
{
    struct tmq *tmq;
    struct tmq_element *elem;
    int key = 100;

    tmq = tmq_create ();
    elem = tmq_element_create (&key, sizeof(key));
    tmq_insert (tmq, elem);
    tmq_pop (tmq, elem);

    fail_unless ((tmq->size == 0), "Pop Failed (tmq->size != 0)");
    fail_unless ((elem->next == NULL), "Pop Failed (elem->next != NULL)");
    fail_unless ((elem->prev == NULL), "Pop Failed (elem->prev != NULL)");
    fail_unless ((tmq->head == NULL), "Pop Failed (tmq->head != NULL)");
    fail_unless ((tmq->tail == NULL), "Pop Failed (tmq->pop != NULL)");
}
END_TEST

/** Test Case for TMQ Insert a Bunch of Elements 
 */
START_TEST(test_tmq_pop_from_a_bunch)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    struct tmq_element *saved, *saved_prev, *saved_next;
    int key;

    for (key = 0; key < 9000; key++)
    {
        elem = tmq_element_create (&key, sizeof(key));
        tmq_insert (tmq, elem);

        if (key == 9000/2)
            saved = elem;
    }

    saved_next = saved->next;
    saved_prev = saved->prev;
    tmq_pop (tmq, saved);

    fail_unless ((tmq->size == 9000-1), "Pop Failed (tmq->size != 9000-1)");
    fail_unless ((saved_next->prev == saved_prev), "Pop failed (saved->next->prev != saved->prev)");
    fail_unless ((saved_prev->next == saved_next), "Pop failed (saved->prev->next != saved->next)");
}
END_TEST

/** Positive Tests
 */
Suite *
positive_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Positive - tmq_pop");
    TCase *tc = tcase_create ("Positive");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_pop_one);
    tcase_add_test (tc, test_tmq_pop_from_a_bunch);

    return s;
}

/** Negative Tests 
 */
Suite *
negative_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Negatvie - tmq_pop");
    TCase *tc = tcase_create ("Negative");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_pop_null_elem);
    tcase_add_test (tc, test_tmq_pop_tmq_null);
    tcase_add_test (tc, test_tmq_pop_null_null);

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
