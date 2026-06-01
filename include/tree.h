/*
 *      tree.h
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

#ifndef __tree_h__
#define __tree_h__

#include <malloc.h>
#include "node.h"
#include "sort.h"
#include "metric.h"

typedef struct _tree {
	node * root;
	node nodes[512]; // array of nodes
} tree;

tree * new_tree_from_metric(metric * met);
void delete_tree(tree *del);
#endif

