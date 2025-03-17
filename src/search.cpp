#include "search.hpp"
#include "trace.hpp"


void sm_search(
	bptr_t const& root,
	SearchIO& ops,
	FifoPair& readFifos
) {
	static Tracer t;
	static enum {
		IDLE,
		TRAVERSE
	} state = IDLE;
	static bkey_t key;
	static bstatusval_t result;

	switch(state) {
		case IDLE:
			if (!ops.input.empty()) {
				ops.input.read(key);
				t.reset(root, key);
				state = TRAVERSE;
			}
			break;
		case TRAVERSE:
			//! If reached a leaf
			if (t.sm_step(readFifos)) {
				result = t.get_result();
				ops.output.write(result);
				state = IDLE;
			}
			break;
	}
}
