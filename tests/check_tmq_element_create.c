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

/** Positive test 
 */
START_TEST(test_tmq_element_create)
{
    struct tmq_element *ret;
    struct key
    { 
        int host;
        int id;
    } key;

    key.host = 0x0A040A20;
    key.id = 1;

    ret = tmq_element_create (&key, sizeof (key));
    fail_unless ((ret != NULL), "tmq_element_create (&key, sizeof (key))");
}
END_TEST

/** Negative Tests 
 */
Suite *
negative_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Negative - tmq_element_create");
    TCase *tc = tcase_create ("Negative");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_element_create_null_size);
    tcase_add_test (tc, test_tmq_element_create_key_zero);
    tcase_add_test (tc, test_tmq_element_create_null_zero);

    return s;
}

/** Positive Tests 
 */
Suite *
positive_suite (void)
{
    Suite *s = suite_create ("Timeout Queue - Positive - tmq_element_create");
    TCase *tc = tcase_create ("Positive");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_element_create);

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
