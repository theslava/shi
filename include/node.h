/*
 *      node.h
 *
 *      Copyright 2007 Vyacheslav Goltser <slavikg@gmail.com>
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NODE_H
#define NODE_H
typedef struct _node {
	int byte;
		/* if byte <0, then this is a parent node (left/right pointers valid)
		 * if byte >=0, then this is a leaf node (left/right pointers NOT valid)
		 */
	long long unsigned int weight;
	struct _node *left;
	struct _node *right;
} node;

static inline long long unsigned int get_weight(const node *n) {
	return n->weight;
}

static inline int compare_nodes (const node *a, const node *b) {
	if (a->weight < b->weight) return -1;
	else if (a->weight > b->weight) return 1;
	else return 0;
}

// Forward declarations for functions defined in main.c
void swap(node **a, node **b);
void headify(node **heap, int size, int i);

#endif /* NODE_H */


