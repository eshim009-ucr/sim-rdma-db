#ifndef NODE_HPP
#define NODE_HPP

#include "types.hpp"
#include "lock.hpp"

//! @brief A generic node within the tree
//!
//! Can be a leaf node or an inner node
struct Node {
	//! @brief Keys corresponding to child data at the same indices
	//! @par Inner Nodes
	//! In inner nodes, the keys are the exclusive upper
	//! bounds to ranges of child keys such that
	//! `keys[i-1] <= child[i].keys < keys[i]`
	//! For the 0th key, \f$-\infty\f$ is the bound.
	//! @par Leaf Nodes
	//! In leaf nodes the keys are exact lookup values.
	bkey_t keys[TREE_ORDER];
	//! @brief "Pointer to" (address of) the next largest sibling node
	//!
	//! The @ref bval_t union is used to select how they are interpreted
	//! (as internal nodes that point to other nodes or leaves that hold values)
	bval_t values[TREE_ORDER];
	//! @brief The values corresponding to the keys at the same indices
	//!
	//! These may be leaf data or pointers within the tree.
	bptr_t next;
	//! @brief Used to restrict concurrent modifications to this node
	lock_t lock;
	//! @brief Traverse the tree structure in search of the given key
	//! @param[in] key The key to search for
	//! @return A result containing a status code for success/failure of the
	//!         operation along with the address of the next node to check on
	//!         success. If this is a leaf node, its data will be returned.
	bstatusval_t find_next(bkey_t key) const;
	//! @brief Find the value corresponding to a given key
	bstatusval_t find_value(bkey_t key) const;
};

//! @brief A node that knows the address where it resides in the tree
struct AddrNode : public Node {
	bptr_t addr;
	bool is_leaf() const;
};

#endif
