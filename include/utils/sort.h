/*
 *      sort.h
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

#ifndef __sort_h__
#define __sort_h__

#include "data_structures/node.h"

void swap(node **heap, int count);
void heapify(node **heap, int count, int root);
void heapsort(node **heap);

/* Sort an array of node pointers by weight (ascending) */
void sort_nodes_by_weight(node **nodes, int count);

#endif

