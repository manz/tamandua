#include <td_base.h>
#include <td_core.h>

int main
(int argc, char** argv)
{
	struct tdc_context *a, *b;
	/* Initalise tamandua bases
	 * and send log to stderr
	 */
	tdb_init(stderr, &argc, argv);
	/* Initialise tamandua core
	 * with automatic thread selection,
	 * and enable rand init
	 */
	a = tdc_init(TD_PREFIX "/share/tamandua/problems/", 0, 1);
	/* Initialise a second tamadua core
	 * with 3 threads.
	 */
	b = tdc_init(TD_PREFIX "/share/tamandua/problems", 3, 1);
	tdc_setcontext(a);
	tdc_test();
	tdc_setcontext(b);
	tdc_test();
	tdc_exit();
	tdc_setcontext(a);
	tdc_test();
	tdc_test();
	tdc_exit();
	tdb_exit();
}
