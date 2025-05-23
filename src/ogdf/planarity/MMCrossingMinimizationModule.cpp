/** \file
 * \brief Implements class MMCrossingMinimizationModule.
 *
 * \author Carsten Gutwenger
 *
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 *
 * \par
 * Copyright (C)<br>
 * See README.md in the OGDF root directory for details.
 *
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation;
 * see the file LICENSE.txt included in the packaging of this file
 * for details.
 *
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * \par
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <ogdf/basic/Array.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphList.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/Module.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/planarity/MMCrossingMinimizationModule.h>
#include <ogdf/planarity/PlanRepExpansion.h>

namespace ogdf {

Module::ReturnType MMCrossingMinimizationModule::call(const Graph& G,
		const List<node>& splittableNodes, int& cr, const EdgeArray<bool>* forbid) {
	cr = 0;
	m_nodeSplits = 0;
	m_splittedNodes = 0;

	NodeArray<bool> splittable(G, false);
	ListConstIterator<node> itV;
	for (itV = splittableNodes.begin(); itV.valid(); ++itV) {
		splittable[*itV] = true;
	}

	EdgeArray<int> comp(G, -1);
	int c = biconnectedComponents(G, comp);

	Array<List<edge>> edges(c);
	for (edge e : G.edges) {
		edges[comp[e]].pushBack(e);
	}

	NodeArray<node> map(G, nullptr);

	for (int i = 0; i < c; ++i) {
		if (edges[i].size() < 9) {
			continue;
		}

		Graph B;
		List<node> nodes;
		List<node> splittableNodesB;

		EdgeArray<bool>* forbidB = nullptr;
		if (forbid) {
			forbidB = new EdgeArray<bool>(B, false);
		}

		ListConstIterator<edge> it;
		for (it = edges[i].begin(); it.valid(); ++it) {
			edge e = *it;
			node v = e->source(), w = e->target();

			if (map[v] == nullptr) {
				map[v] = B.newNode();
				nodes.pushBack(v);
				if (splittable[v]) {
					splittableNodesB.pushBack(map[v]);
				}
			}
			if (map[w] == nullptr) {
				map[w] = B.newNode();
				nodes.pushBack(w);
				if (splittable[w]) {
					splittableNodesB.pushBack(map[w]);
				}
			}

			edge eB = B.newEdge(map[v], map[w]);
			if (forbidB) {
				(*forbidB)[eB] = (*forbid)[e];
			}
		}

		PlanRepExpansion PG(B, splittableNodesB);

		int crcc, numNS = 0, numSN = 0;
		ReturnType ret = doCall(PG, 0, forbidB, crcc, numNS, numSN);
		delete forbidB;
		if (!isSolution(ret)) {
			return ret;
		}
		cr += crcc;
		m_nodeSplits += numNS;
		m_splittedNodes += numSN;

		for (node v : nodes) {
			map[v] = nullptr;
		}
	}

	return ReturnType::Feasible;
}

Module::ReturnType MMCrossingMinimizationModule::call(const Graph& G, int& cr,
		const EdgeArray<bool>* forbid) {
	cr = 0;
	m_nodeSplits = 0;
	m_splittedNodes = 0;

	EdgeArray<int> comp(G, -1);
	int c = biconnectedComponents(G, comp);

	Array<List<edge>> edges(c);
	for (edge e : G.edges) {
		edges[comp[e]].pushBack(e);
	}

	NodeArray<node> map(G, nullptr);

	for (int i = 0; i < c; ++i) {
		if (edges[i].size() < 9) {
			continue;
		}

		Graph B;
		List<node> nodes;

		ListConstIterator<edge> it;
		for (it = edges[i].begin(); it.valid(); ++it) {
			edge e = *it;
			node v = e->source(), w = e->target();

			if (map[v] == nullptr) {
				map[v] = B.newNode();
				nodes.pushBack(v);
			}
			if (map[w] == nullptr) {
				map[w] = B.newNode();
				nodes.pushBack(w);
			}

			B.newEdge(map[v], map[w]);
		}

		PlanRepExpansion PG(B);

		int crcc, numNS = 0, numSN = 0;
		ReturnType ret = doCall(PG, 0, forbid, crcc, numNS, numSN);
		if (!isSolution(ret)) {
			return ret;
		}
		cr += crcc;
		m_nodeSplits += numNS;
		m_splittedNodes += numSN;

		ListConstIterator<node> itV;
		for (itV = nodes.begin(); itV.valid(); ++itV) {
			map[*itV] = nullptr;
		}
	}

	return ReturnType::Feasible;
}

}
