/** \file
 * \brief declaration of class FixedEmbeddingInserterOld
 *
 * \author Hoi-Ming Wong
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

#pragma once

#include <ogdf/basic/CombinatorialEmbedding.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/basic.h>
#include <ogdf/upward/UpwardEdgeInserterModule.h>
#include <ogdf/upward/UpwardPlanarity.h>

namespace ogdf {
class UpwardPlanRep;
template<class E>
class List;
template<class E>
class SList;

//! Edge insertion module that inserts each edge optimally into a fixed embedding.
class OGDF_EXPORT FixedEmbeddingUpwardEdgeInserter : public UpwardEdgeInserterModule {
public:
	//! Creates an instance of fixed-embedding edge inserter.
	FixedEmbeddingUpwardEdgeInserter() { }

	~FixedEmbeddingUpwardEdgeInserter() { }


private:
	bool isUpwardPlanar(Graph& G) const { return UpwardPlanarity::isUpwardPlanar_singleSource(G); }

	/**
	 * @param UPR is the input upward planarized representation of a FUPS and will also receive the result.
	 * @param origEdges is the list of original edges (edges in the original graph
	 *        of \p UPR) that have to be inserted.
	 * @param costOrig points to an edge array containing the costs of original edges; edges in
	 *        \p UPR without an original edge have zero costs.
	 * @param forbiddenEdgeOrig points to an edge array indicating if an original edge is
	 *        forbidden to be crossed.
	 */
	virtual ReturnType doCall(UpwardPlanRep& UPR, const List<edge>& origEdges,
			const EdgeArray<int>* costOrig = nullptr,
			const EdgeArray<bool>* forbiddenEdgeOrig = nullptr) override;


	ReturnType insertAll(UpwardPlanRep& UPR, List<edge>& toInsert, EdgeArray<int>& cost);


	//! compute a list of static locked edges, i.e. eges which a priory cannot included in a feasible insertion path.
	void staticLock(UpwardPlanRep& UPR, EdgeArray<bool>& locked, const List<edge>& origEdges,
			edge e_orig);

	//! compute a list of dynamic locked edges
	void dynamicLock(UpwardPlanRep& UPR, EdgeArray<bool>& locked, face f, adjEntry e_cur);

	void nextFeasibleEdges(UpwardPlanRep& UPR, List<adjEntry>& nextEdges, face f, adjEntry e_cur,
			EdgeArray<bool>& locked, bool heuristic);

	//! compute the minimal feasible insertion path
	void minFIP(UpwardPlanRep& UPR, List<edge>& origEdges, EdgeArray<int>& cost, edge e_orig,
			SList<adjEntry>& path) {
		getPath(UPR, origEdges, cost, e_orig, path, false);
	}

	//! compute a constraint feasible insertion path usig heuristic.
	void constraintFIP(UpwardPlanRep& UPR, List<edge>& origEdges, EdgeArray<int>& cost, edge e_orig,
			SList<adjEntry>& path) {
		getPath(UPR, origEdges, cost, e_orig, path, true);
	}

	//! compute an insertion path
	void getPath(UpwardPlanRep& UPR, List<edge>& origEdges, EdgeArray<int>& cost, edge e_orig,
			SList<adjEntry>& path, bool heuristic);


	//! mark the edges which are dominates by node v
	void markUp(const Graph& G, node v, EdgeArray<bool>& markedEdges);


	//! mark the edges which dominate node v
	void markDown(const Graph& G, node v, EdgeArray<bool>& markedEdges);

	//! compute the feasible edges of the face f with respect to e
	void feasibleEdges(UpwardPlanRep& UPR,
			face f, // current face
			adjEntry adj, // current adjEntry, right face muss be f
			EdgeArray<bool>& locked, // we compute the dyn. locked edges on the fly with respect to e
			List<adjEntry>& feasible, // the list of feasible edges in f with respect to e
			bool heuristic);

	//! return true if current insertion path is contraint feasible
	bool isConstraintFeasible(UpwardPlanRep& UPR, const List<edge>& orig_edges, edge e_orig,
			adjEntry adjCurrent,
			adjEntry adjNext, // the next adjEntry of the current insertion path
			EdgeArray<adjEntry>& predAdj //Array to reconstruction the insertion path
	);


	//! return true if current insertion path is contraint feasible
	bool isConstraintFeasible(UpwardPlanRep& UPR, List<edge>& origEdges, edge e_orig,
			SList<adjEntry>& path);
};

}
