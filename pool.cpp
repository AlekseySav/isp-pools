#include "pool.h"

Pool::Pool(cap_t capacity, cap_t count)
	: capacity(capacity), count(count), root(this)
{}

Pool* Pool::getroot() const {
	// we are in root anyway
	if (root == this) return root;
	// re-link sub-tree
	root = root->getroot();
	return root;
}

long Pool::measure() const {
	Pool* p = getroot();
	return p->capacity / p->count;
}

void Pool::add(cap_t water) {
	Pool* p = getroot();
	p->capacity += water;
}

void Pool::connect(Pool& pool) {
	Pool* a = getroot();
	Pool* b = pool.getroot();
	// already connected
	if (a == b) return;
	// link root of B to root of A
	b->root = a;
	a->capacity += b->capacity;
	a->count += b->count;
}
