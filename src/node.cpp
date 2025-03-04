#include "node.hpp"


bstatusval_t Node::find_next(bkey_t key) const {
	bstatusval_t result = {SUCCESS, INVALID};
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		// We overshot the node we were looking for
		// and got an uninitialized key
		if (keys[i] == INVALID) {
			// Empty node, error
			if (i == 0) {
				result.status = NOT_FOUND;
				return result;
			}
			// Save the last node we looked at
			else {
				result.value = values[i-1];
				return result;
			}
		}
		// If this key is the first key greater than what we're looking for
		// then continue down this subtree
		else if (key < keys[i]) {
			result.value = values[i];
			return result;
		}
	}
	// Wasn't in this node, check sibling
	if (next == INVALID) {
		// No siblingg
		result.status = NOT_FOUND;
	} else {
		result.value.ptr = next;
	}
	return result;
}


bstatusval_t Node::find_value(bkey_t key) const {
	bstatusval_t result = {SUCCESS, INVALID};
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		if (keys[i] == key) {
			result.value = values[i];
			return result;
		}
	}
	result.status = NOT_FOUND;
	return result;
}


bool Node::is_valid() const {
	return keys[0] != INVALID;
}

bool Node::is_full() const {
	return keys[TREE_ORDER-1] != INVALID;
}

void Node::clear() {
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		keys[i] = INVALID;
		values[i].data = INVALID;
	}
}

bool AddrNode::is_leaf() const {
	return addr < MAX_LEAVES;
}
