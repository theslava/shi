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

#ifndef __node_h__
#define __node_h__

typedef struct _node {
	int byte;
		/* if byte <0, then this is a parent node (left/right pointers valid)
		 * if byte >=0, then this is a leaf node (left/right pointers NOT valid)
		 */
	long long unsigned int weight;
	struct _node *left;
	struct _node *right;
} node;

long long unsigned int get_weight(node *n) {
	return n->weight;
}

int compare_nodes (node *a, node *b) {
	int result = a->weight - b->weight;
	if (result < 0) return -1;
	else if (result > 0) return 1;
	else return 0;
}

#endif
