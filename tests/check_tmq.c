#define _GNU_SOURCE
#include <timeout_queue.h>
#include <dlfcn.h>
#include <unistd.h>

#include <check.h>

/** Test Case for TMQ Create
 */
START_TEST (test_tmq_create)
{
    struct tmq *tmq;
    tmq = tmq_create ();

    fail_unless ((tmq != NULL), "tmq_create () failed");

    tmq_destroy (tmq);
}
END_TEST

/** Test Case for TMQ Destroy
 */
START_TEST (test_tmq_destroy)
{
    struct tmq *tmq;
    int ret;

    tmq = tmq_create ();
    ret = tmq_destroy (tmq);

    fail_unless ((ret == 0), "tmq_destroy () failed");
}
END_TEST

/** Build the TMQ Test Suite
 */
Suite *
make_tmq_suite (void)
{
    Suite *s = suite_create ("timeout queue");
    TCase *tc = tcase_create ("Core");

    suite_add_tcase (s, tc);
    tcase_add_test (tc, test_tmq_create);
    tcase_add_test (tc, test_tmq_destroy);

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

    s = make_tmq_suite ();
    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    n = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (n == 0) ? 0 : 1;
}
