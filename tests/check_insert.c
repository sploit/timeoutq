#include <timeout_queue.h>
#include <unistd.h>

#include <check.h>

/** Test Cases for TMQ Insert
 */

/** NULL struct tmq *
 */
START_TEST(test_tmq_insert_null_elem)
{
    struct tmq_element *elem;
    int key = 1;

    int ret;

    elem = tmq_element_create (&key, sizeof(key));
    ret = tmq_insert (NULL, elem);

    fail_unless ((ret != 0), "tmq_insert (NULL, elem)");
}
END_TEST


/** NULL struct tmq_elem *
 */
START_TEST(test_tmq_insert_tmq_null)
{
    struct tmq *tmq= tmq_create ();

    int ret;
    ret = tmq_insert (tmq, NULL);

    fail_unless ((ret != 0), "tmq_insert (tmq, NULL)");
}
END_TEST

/** NULL struct tmq *, NULL struct tmq_elem * 
 */
START_TEST(test_tmq_insert_null_null)
{
    int ret;
    ret = tmq_insert (NULL, NULL);

    fail_unless ((ret != 0), "tmq_insert (NULL, NULL)");
}
END_TEST


/** Test Case for TMQ Insert Single Element
 */
START_TEST(test_tmq_insert_one)
{
    struct tmq *tmq;
    struct tmq_element *elem;
    int key = 100;

    tmq = tmq_create ();
    elem = tmq_element_create (&key, sizeof(key));
    tmq_insert (tmq, elem);

    fail_unless ((tmq->size == 1), "Insert one test failed");

    tmq_destroy (tmq);
}
END_TEST

/** Test Case for TMQ Insert a Bunch of Elements 
 */
START_TEST(test_tmq_insert_a_bunch)
{
    struct tmq *tmq = tmq_create ();
    struct tmq_element *elem;
    int key;

    for (key = 0; key < 9000; key++)
    {
        elem = tmq_element_create (&key, sizeof(key));
        tmq_insert (tmq, elem);
    }

    fail_unless ((tmq->size == 9000), "Insert a bunch test failed");

    tmq_destroy (tmq);
}
END_TEST

/** Build the TMQ Test Suite
 */
Suite *
make_tmq_insert_suite (void)
{
    Suite *s = suite_create ("timeout queue insert");
    TCase *tc = tcase_create ("Core");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_insert_null_elem);
    tcase_add_test (tc, test_tmq_insert_tmq_null);
    tcase_add_test (tc, test_tmq_insert_null_null);

    tcase_add_test (tc, test_tmq_insert_one);
    tcase_add_test (tc, test_tmq_insert_a_bunch);

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
