#pragma once
#include <cstdint>

class Pool
{
public:
	using cap_t = uint64_t;
public:
	Pool(cap_t capacity = 0, cap_t count = 1);
	long measure() const;
	void add(cap_t water);
	void connect(Pool& pool);

private:
	Pool* getroot() const;
private:
	cap_t capacity;
	cap_t count;
	mutable Pool* root;
};

/*
 * idea:
 * 	connect(A, B) links root(B) to root(A), i.e. B becomes child of A (maybe an indirect child)
 * 	on next access of C (C is a child of B), all nodes between C and B are re-linked to A
 *
 * efficiency:
 * 	let d(n) = *depth of node n*, D=sum(d(n) for each node n)
 * 	each re-link decreases D by 1, each connect increases D by 1.
 * 		so, # re-links <= # connect
 * 	i.e. all operations are done in O*(1)
 */

/*
 * examlpe:
 * 	A B C D
 * 	connect(A, B)
 * 	 A C D
 *  /
 * B
 * connect(C, A)
 *     C D
 *    /
 *   A
 *  /
 * B
 * connect(B, D)
 *   C
 *  / \
 * A   B (B re-linked)
 *      \
 *       D
 */
