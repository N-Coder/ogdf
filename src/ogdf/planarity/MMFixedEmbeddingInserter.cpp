/** \file
 * \brief implementation of MMFixedEmbeddingInserter class
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
#include <ogdf/basic/CombinatorialEmbedding.h>
#include <ogdf/basic/FaceSet.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/GraphList.h>
#include <ogdf/basic/GraphSets.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/Module.h>
#include <ogdf/basic/SList.h>
#include <ogdf/basic/basic.h>
#include <ogdf/basic/graphics.h>
#include <ogdf/basic/tuples.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/planarity/MMFixedEmbeddingInserter.h>
#include <ogdf/planarity/PlanRepExpansion.h>
#include <ogdf/planarity/RemoveReinsertType.h>
#include <ogdf/planarity/embedding_inserter/CrossingsBucket.h>

#include <string>
#include <utility>

namespace ogdf {

static int globalCounter = 0;

// constructor
// sets default values for options
MMFixedEmbeddingInserter::MMFixedEmbeddingInserter() {
	m_rrOption = RemoveReinsertType::None;
	m_percentMostCrossed = 25;
}

#if 0
static void draw(
	const PlanRepExpansion &PG,
	const EdgeArray<bool> *forbiddenEdgeOrig)
{
	static int num = 1;

	GraphAttributes GA(PG,
		GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics |
		GraphAttributes::nodeLabel | GraphAttributes::edgeStyle |
		GraphAttributes::nodeStyle);

	for(node v : PG.nodes) {
		node vOrig = PG.original(v);
		if(vOrig != 0) {
			GA.label(v) = to_string(v->index()) + " | " + to_string(vOrig->index());
			GA.fillColor(v) = "#ffffff";
		} else {
			GA.label(v) = to_string(v->index());
			GA.fillColor(v) = "#22ff22";
		}
		GA.width(v) = 60;
		GA.height(v) = 20;
	}

	if(forbiddenEdgeOrig) {
		for(edge e : PG.edges) {
			edge eOrig = PG.originalEdge(e);
			if (eOrig && (*forbiddenEdgeOrig)[eOrig])
				GA.strokeColor(e) = "#ff0000";
		}
	}

	PlanarStraightLayout pl;
	pl.separation(40);
	pl.callFixEmbed(GA, PG.firstEdge()->adjSource());

	string fileName = string("layout-") + to_string(num++) + ".gml";
	GraphIO::writeGML(GA, fileName);
}
#endif

void MMFixedEmbeddingInserter::drawDual(const PlanRepExpansion& PG,
		const EdgeArray<bool>* forbiddenEdgeOrig) {
	GraphAttributes GA(m_dual,
			GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics | GraphAttributes::nodeLabel
					| GraphAttributes::edgeStyle | GraphAttributes::nodeStyle);

	for (node v : m_dual.nodes) {
		if (m_primalNode[v]) {
			GA.label(v) =
					string("v") + to_string(m_primalNode[v]->index()) + ": " + to_string(v->index());
			GA.fillColor(v) = Color::Name::White;
		} else {
			GA.label(v) = string("f: ") + to_string(v->index());
			GA.fillColor(v) = Color(0x22, 0xff, 0x22);
		}
		GA.width(v) = 50;
		GA.height(v) = 20;
	}

	for (edge e : m_dual.edges) {
		if (origOfDualForbidden(e, PG, forbiddenEdgeOrig)) {
			GA.strokeColor(e) = Color::Name::Red;
		} else {
			GA.strokeColor(e) = Color::Name::Black;
		}
	}

	GraphIO::write(GA, "dual.gml", GraphIO::writeGML);
}

// actual call (called by all variations of call)
//   crossing of generalizations is forbidden if forbidCrossingGens = true
//   edge costs are obeyed if costOrig != 0
Module::ReturnType MMFixedEmbeddingInserter::doCall(PlanRepExpansion& PG,
		const List<edge>& origEdges, const EdgeArray<bool>* forbiddenEdgeOrig) {
	ReturnType retValue = ReturnType::Feasible;

#if 0
	std::cout << "orig edges: ";
	ListConstIterator<edge> itE;
	for(itE = origEdges.begin(); itE.valid(); ++itE)
		std::cout << *itE << " ";
	std::cout << std::endl;
#endif

	PG.embed();
	OGDF_ASSERT(PG.representsCombEmbedding());

	if (origEdges.size() == 0) {
		return ReturnType::Optimal; // nothing to do
	}

	// initialization
	CombinatorialEmbedding E(PG); // embedding of PG

	// construct dual graph
	m_primalAdj.init(m_dual);
	m_dualEdge.init(PG, nullptr);
	m_dualOfFace.init(E);
	m_dualOfNode.init(PG, nullptr);
	m_primalNode.init(m_dual, nullptr);
	m_dualCost.init(m_dual, 0);

	constructDual(PG, E);
	OGDF_ASSERT(checkDualGraph(PG, E));

#if 0
	draw(PG, forbiddenEdgeOrig);

	if(forbiddenEdgeOrig) {
		std::cout << "forbidden edges: ";
		for(edge e : PG.original().edges)
			if((*forbiddenEdgeOrig)[e]) std::cout << e << " ";
		std::cout << std::endl;
	}
#endif

	m_delFaces = new FaceSet(E);

	// m_newFaces and m_mergedNodes are used by removeEdge()
	m_newFaces = nullptr;
	m_mergedNodes = nullptr;
	if (removeReinsert() != RemoveReinsertType::None) {
		m_newFaces = new FaceSet(E);
		m_mergedNodes = new NodeSet(PG);
	}

	SListPure<edge> currentOrigEdges;
	if (removeReinsert() == RemoveReinsertType::Incremental) {
		for (edge e : PG.edges) {
			currentOrigEdges.pushBack(PG.originalEdge(e));
		}
	}

	NodeSet sources(PG), targets(PG);

	// insertion of edges
	for (edge eOrig : origEdges) {
		node srcOrig = eOrig->source();
		node tgtOrig = eOrig->target();

		node oldSrc = (PG.splittableOrig(srcOrig) && PG.expansion(srcOrig).size() == 1)
				? PG.expansion(srcOrig).front()
				: nullptr;
		node oldTgt = (PG.splittableOrig(tgtOrig) && PG.expansion(tgtOrig).size() == 1)
				? PG.expansion(tgtOrig).front()
				: nullptr;

		anchorNodes(eOrig->source(), sources, PG);
		anchorNodes(eOrig->target(), targets, PG);

		List<Tuple2<adjEntry, adjEntry>> crossed;
		findShortestPath(PG, E, sources.nodes(), targets.nodes(), crossed, forbiddenEdgeOrig);
		sources.clear();
		targets.clear();

		insertEdge(PG, E, eOrig, eOrig->source(), eOrig->target(), nullptr, crossed);

		if (oldSrc != nullptr && PG.expansion(srcOrig).size() > 1) {
			contractSplitIfReq(PG, E, oldSrc);
		}
		if (oldTgt != nullptr && PG.expansion(tgtOrig).size() > 1) {
			contractSplitIfReq(PG, E, oldTgt);
		}

		//draw(PG, forbiddenEdgeOrig);

		// THIS OPTION IS NOT YET IMPLEMENTED!
		if (removeReinsert() == RemoveReinsertType::Incremental) {
			currentOrigEdges.pushBack(eOrig);

			bool improved;
			do {
				improved = false;

				SListConstIterator<edge> itRR;
				for (itRR = currentOrigEdges.begin(); itRR.valid(); ++itRR) {
					edge eOrigRR = *itRR;

					int pathLength = PG.chain(eOrigRR).size() - 1;
					if (pathLength == 0) {
						continue; // cannot improve
					}

					node oldSource, oldTarget;
					removeEdge(PG, E, eOrigRR, nullptr, oldSource, oldTarget);

					// try to find a better insertion path
					List<Tuple2<adjEntry, adjEntry>> crossedTuple;
					findShortestPath(PG, E, PG.expansion(eOrigRR->source()),
							PG.expansion(eOrigRR->target()), crossedTuple, forbiddenEdgeOrig);

					// re-insert edge (insertion path cannot be longer)
					insertEdge(PG, E, eOrigRR, eOrigRR->source(), eOrigRR->target(), nullptr,
							crossedTuple);

					int newPathLength = PG.chain(eOrigRR).size() - 1;
					OGDF_ASSERT(newPathLength <= pathLength);

					if (newPathLength < pathLength) {
						improved = true;
					}
				}
			} while (improved);
		}
		OGDF_ASSERT(checkSplitDeg(PG));
	}

#ifdef OGDF_DEBUG
	E.consistencyCheck();
	PG.consistencyCheck();
#endif
	OGDF_ASSERT(checkDualGraph(PG, E));
	OGDF_ASSERT(checkSplitDeg(PG));

	const Graph& G = PG.original();
	if (removeReinsert() != RemoveReinsertType::Incremental) {
		// postprocessing (remove-reinsert heuristc)
		SListPure<edge> rrEdges;

		switch (removeReinsert()) {
		case RemoveReinsertType::All:
		case RemoveReinsertType::MostCrossed: {
			const List<node>& origInCC = PG.nodesInCC();
			ListConstIterator<node> itV;

			for (itV = origInCC.begin(); itV.valid(); ++itV) {
				node vG = *itV;
				for (adjEntry adj : vG->adjEntries) {
					if ((adj->index() & 1) == 0) {
						continue;
					}
					edge eG = adj->theEdge();
					rrEdges.pushBack(eG);
				}
			}
		} break;

		case RemoveReinsertType::Inserted:
			for (edge e : origEdges) {
				rrEdges.pushBack(e);
			}
			break;

		case RemoveReinsertType::None:
		case RemoveReinsertType::Incremental:
			break;
		case RemoveReinsertType::IncInserted:
			OGDF_ASSERT(false);
			break;
		}

		// marks the end of the interval of rrEdges over which we iterate
		// initially set to invalid iterator which means all edges
		SListConstIterator<edge> itStop;

		bool improved;
		do {
			improved = false;

			if (removeReinsert() == RemoveReinsertType::MostCrossed) {
				embedding_inserter::CrossingsBucket<PlanRepExpansion> bucket(&PG);
				rrEdges.bucketSort(bucket);

				const int num = int(0.01 * percentMostCrossed() * G.numberOfEdges());
				itStop = rrEdges.get(num);
			}

			for (SListConstIterator<edge> it = rrEdges.begin(); it != itStop; ++it) {
				edge eOrig = *it;

				int pathLength;
				pathLength = PG.chain(eOrig).size() - 1;
				if (pathLength == 0) {
					continue; // cannot improve
				}

				node oldSrc = nullptr, oldTgt = nullptr;
				removeEdge(PG, E, eOrig, nullptr, oldSrc, oldTgt);
				//draw(PG, forbiddenEdgeOrig);

				// try to find a better insertion path
				anchorNodes(eOrig->source(), sources, PG);
				anchorNodes(eOrig->target(), targets, PG);

				List<Tuple2<adjEntry, adjEntry>> crossed;
				findShortestPath(PG, E, sources.nodes(), targets.nodes(), crossed, forbiddenEdgeOrig);
				sources.clear();
				targets.clear();

				// re-insert edge (insertion path cannot be longer)
				insertEdge(PG, E, eOrig, eOrig->source(), eOrig->target(), nullptr, crossed);

				if (PG.splittable(oldSrc)) {
					contractSplitIfReq(PG, E, oldSrc);
				}
				if (PG.splittable(oldTgt)) {
					contractSplitIfReq(PG, E, oldTgt);
				}

				int newPathLength = PG.chain(eOrig).size() - 1;
				int saved = pathLength - newPathLength;
				OGDF_ASSERT(saved >= 0);

				if (saved > 0) {
					improved = true;
				}
			}

			if (removeReinsert() == RemoveReinsertType::All) {
				// process all node splits
				int nsCount = PG.nodeSplits().size();
				ListIterator<PlanRepExpansion::NodeSplit> itS, itSNext;
				for (itS = PG.nodeSplits().begin(); itS.valid() && nsCount > 0;
						itS = itSNext, --nsCount) {
					++globalCounter;
					PlanRepExpansion::NodeSplit* ns = &(*itS);

					int pathLength;
					pathLength = ns->m_path.size() - 1;
					if (pathLength == 0) {
						continue; // cannot improve
					}

					node vOrig = PG.original(ns->source());

					node oldSrc = nullptr, oldTgt = nullptr;
					removeEdge(PG, E, nullptr, ns, oldSrc, oldTgt);

					// try to find a better insertion path
					findSourcesAndTargets(oldSrc, oldTgt, sources, targets, PG);

					List<Tuple2<adjEntry, adjEntry>> crossed;
					findShortestPath(PG, E, sources.nodes(), targets.nodes(), crossed,
							forbiddenEdgeOrig);

					node vCommon = commonDummy(sources, targets);
					sources.clear();
					targets.clear();

					if (vCommon == nullptr) {
						// re-insert edge (insertion path cannot be longer)
						insertEdge(PG, E, nullptr, vOrig, vOrig, ns, crossed);

						if (PG.splittable(oldSrc)) {
							contractSplitIfReq(PG, E, oldSrc, ns);
						}
						if (PG.splittable(oldTgt)) {
							contractSplitIfReq(PG, E, oldTgt, ns);
						}

						int newPathLength = ns->m_path.size() - 1;
						int saved = pathLength - newPathLength;
						OGDF_ASSERT(saved >= 0);

						if (saved > 0) {
							improved = true;
						}

						itSNext = itS.succ();
						if (newPathLength == 0) {
							contractSplit(PG, E, ns);
						}

					} else {
						improved = true;
						itSNext = itS.succ();

						convertDummy(PG, E, vCommon, vOrig, ns);
					}
				}
			}

#ifdef OGDF_DEBUG
			PG.consistencyCheck();
			E.consistencyCheck();
#endif
			OGDF_ASSERT(checkDualGraph(PG, E));
			OGDF_ASSERT(checkSplitDeg(PG));

		} while (improved); // iterate as long as we improve
	}


	// free resources
	delete m_newFaces;
	delete m_delFaces;
	delete m_mergedNodes;

	m_dualOfFace.init();
	m_dualOfNode.init();
	m_primalNode.init();
	m_primalAdj.init();
	m_dualEdge.init();
	m_dualCost.init();
	m_dual.clear();

	return retValue;
}

// construct dual graph
void MMFixedEmbeddingInserter::constructDual(const PlanRepExpansion& PG,
		const CombinatorialEmbedding& E) {
	// insert a node in the dual graph for each face in E
	for (face f : E.faces) {
		m_dualOfFace[f] = m_dual.newNode();
		//std::cout << "dual of face " << f->index() << ": " << m_dualOfFace[f] << std::endl;
	}

	// insert a node in the dual graph for each splittable node in PG
	for (node v : PG.nodes) {
		if (PG.splittable(v) && v->degree() >= 4) {
			m_primalNode[m_dualOfNode[v] = m_dual.newNode()] = v;
			//std::cout << "dual of node " << v << " :" << m_dualOfNode[v] << std::endl;
		}
	}

	// Insert an edge into the dual graph for each adjacency entry in E.
	// The edges are directed from the left face to the right face.
	for (node v : PG.nodes) {
		node vDual = m_dualOfNode[v];

		for (adjEntry adj : v->adjEntries) {
			node vLeft = m_dualOfFace[E.leftFace(adj)];
			node vRight = m_dualOfFace[E.rightFace(adj)];

			if (vLeft != vRight) {
				edge e = m_dual.newEdge(vLeft, vRight);
				//std::cout << "dual edge " << e << std::endl;
				m_dualEdge[m_primalAdj[e] = adj] = e;
				m_dualCost[e] = 1;
			}

			if (vDual) {
				edge eOut = m_dual.newEdge(vDual, vLeft);
				//std::cout << "dual edge " << eOut << std::endl;
				m_primalAdj[eOut] = adj;
				m_dualCost[eOut] = 0;

				edge eIn = m_dual.newEdge(vLeft, vDual);
				//std::cout << "dual edge " << eIn << std::endl;
				m_primalAdj[eIn] = adj;
				m_dualCost[eIn] = 1;
			}
		}
	}

	// Augment the dual graph by two new vertices. These are used temporarily
	// when searching for a shortest path in the dual graph.
	m_vS = m_dual.newNode();
	m_vT = m_dual.newNode();

	m_maxCost = 2; // we just have 0/1 edge costs at the moment; maximum has
			// to be adjusted when we have more general costs
}

// finds a weighted shortest path in the dual graph augmented by s and t
// (represented by m_vS and m_vT) using edges weights given by costOrig;
// returns list of crossed adjacency entries (corresponding
// to used edges in the dual) in crossed.
//
// running time: O(|dual| + L + C),
//   where L ist the weighted length of the insertion path and C the
//   maximum cost of an edge
void MMFixedEmbeddingInserter::findShortestPath(const PlanRepExpansion& PG,
		const CombinatorialEmbedding& E, const List<node>& sources, const List<node>& targets,
		List<Tuple2<adjEntry, adjEntry>>& crossed, const EdgeArray<bool>* forbiddenEdgeOrig) {
#if 0
	static int count = 1;
	std::cout << "==> " << count++ << std::endl;
	std::cout << "Insert:" << std::endl;
	std::cout << "  sources: ";
	ListConstIterator<node> itV;
	for(itV = sources.begin(); itV.valid(); ++itV)
		std::cout << *itV << " ";
	std::cout << "\n  targets: ";
	for(itV = targets.begin(); itV.valid(); ++itV)
		std::cout << *itV << " ";
	std::cout << std::endl;
	drawDual(PG, forbiddenEdgeOrig);

	std::cout << "forbidden dual edges: ";
	for(edge ed : m_dual.edges) {
		if(origOfDualForbidden(ed,PG,forbiddenEdgeOrig))
			std::cout << ed << " ";
	}
	std::cout << std::endl;
#endif
	Array<SListPure<edge>> nodesAtDist(m_maxCost);
	NodeArray<edge> spPred(m_dual, nullptr);

	int oldIdCount = m_dual.maxEdgeIndex();

	// augment dual by edges from m_vS to all adjacent faces of sources ...
	for (node v : sources) {
		for (adjEntry adj : v->adjEntries) {
			// starting edges of bfs-search are all edges leaving s
			edge eDual = m_dual.newEdge(m_vS, m_dualOfFace[E.rightFace(adj)]);
			m_primalAdj[eDual] = adj;
			nodesAtDist[0].pushBack(eDual);
		}
	}

	// ... and from all adjacent faces of targets to m_vT
	for (node v : targets) {
		for (adjEntry adj : v->adjEntries) {
			edge eDual = m_dual.newEdge(m_dualOfFace[E.rightFace(adj)], m_vT);
			m_primalAdj[eDual] = adj;
		}
	}


	// actual search (using extended bfs on directed dual)
	int currentDist = 0;

	for (;;) {
		// next candidate edge
		while (nodesAtDist[currentDist % m_maxCost].empty()) {
			++currentDist;
		}

		edge eCand = nodesAtDist[currentDist % m_maxCost].popFrontRet();
		node v = eCand->target();

		// leads to an unvisited node?
		if (spPred[v] == nullptr) {
			// yes, then we set v's predecessor in search tree
			spPred[v] = eCand;

			// have we reached t ...
			if (v == m_vT) {
				// ... then search is done.
				// constructed list of used edges (translated to crossed
				// adjacency entries in PG) from t back to s (including first
				// and last!)

				do {
					edge eDual = spPred[v];
					node w = eDual->source();

					if (m_primalNode[w] == nullptr) {
						// w is a face node
						crossed.pushFront(Tuple2<adjEntry, adjEntry>(m_primalAdj[eDual], nullptr));
					} else {
						edge eDual2 = spPred[w];
						w = eDual2->source();
						crossed.pushFront(
								Tuple2<adjEntry, adjEntry>(m_primalAdj[eDual2], m_primalAdj[eDual]));
					}

					v = w;
				} while (v != m_vS);

				break;
			}

			// append next candidate edges to queue
			// (all edges leaving v)
			for (adjEntry adj : v->adjEntries) {
				edge e = adj->theEdge();
				if (v != e->source()) {
					continue;
				}
				if (origOfDualForbidden(e, PG, forbiddenEdgeOrig)) {
					continue;
				}

				int listPos = (currentDist + m_dualCost[e]) % m_maxCost;
				nodesAtDist[listPos].pushBack(e);
			}
		}
	}

	// remove augmented edges again
	adjEntry adj;
	while ((adj = m_vS->firstAdj()) != nullptr) {
		m_dual.delEdge(adj->theEdge());
	}

	while ((adj = m_vT->firstAdj()) != nullptr) {
		m_dual.delEdge(adj->theEdge());
	}

	m_dual.resetEdgeIdCount(oldIdCount);
}

void MMFixedEmbeddingInserter::prepareAnchorNode(PlanRepExpansion& PG, CombinatorialEmbedding& E,
		adjEntry& adjStart, node srcOrig) {
	adjEntry adj = adjStart;
	face f = E.rightFace(adjStart);

	edge eStraight;
	PlanRepExpansion::NodeSplit* nsStraight;
	List<edge>* pathStraight = &PG.setOrigs(adj->theEdge(), eStraight, nsStraight);

	node vStraight = pathStraight->front()->source();
	if (PG.original(vStraight) != srcOrig) {
		vStraight = pathStraight->back()->target();
		if (PG.original(vStraight) != srcOrig) {
			adj = adj->cyclicSucc();
			pathStraight = &PG.setOrigs(adj->theEdge(), eStraight, nsStraight);
			vStraight = pathStraight->front()->source();
			if (PG.original(vStraight) != srcOrig) {
				vStraight = pathStraight->back()->target();
			}
		}
	}
	OGDF_ASSERT(PG.original(vStraight) == srcOrig);

	if (PG.original(adj->twinNode()) == srcOrig) {
		// No need for a split; can just directly go to a split node
		adjStart = (adj == adjStart) ? adj->twin()->cyclicPred() : adj->twin();

	} else {
		edge eNew, e = adj->theEdge();
		if (nsStraight == nullptr) {
			// We split a chain of an original edge
			eNew = PG.enlargeSplit(vStraight, e, E);

		} else {
			// We split a node split
			eNew = PG.splitNodeSplit(e, E);
		}

		adjEntry adj1 = eNew->adjSource();
		node vLeft = m_dualOfFace[E.leftFace(adj1)];
		node vRight = m_dualOfFace[E.rightFace(adj1)];

		edge eDual1 = m_dual.newEdge(vLeft, vRight);
		m_dualEdge[m_primalAdj[eDual1] = adj1] = eDual1;
		m_dualCost[eDual1] = 1;

		adjEntry adj2 = e->adjTarget();
		edge eDual2 = m_dual.newEdge(vRight, vLeft);
		m_dualEdge[m_primalAdj[eDual2] = adj2] = eDual2;
		m_dualCost[eDual2] = 1;

		adjStart = (E.rightFace(adj1) == f) ? adj1 : adj2;
		OGDF_ASSERT(E.rightFace(adjStart) == f);
	}
}

void MMFixedEmbeddingInserter::preprocessInsertionPath(PlanRepExpansion& PG,
		CombinatorialEmbedding& E, node srcOrig, node tgtOrig,
		//PlanRepExpansion::nodeSplit ns,
		List<Tuple2<adjEntry, adjEntry>>& crossed) {
	adjEntry& adjStart = (*crossed.begin()).x1();
	adjEntry& adjEnd = (*crossed.rbegin()).x1();

	// Warning: Potential multi-edges are not considered here!!!

	if (PG.original(adjStart->theNode()) == nullptr) {
		prepareAnchorNode(PG, E, adjStart, srcOrig);
	}

	if (PG.original(adjEnd->theNode()) == nullptr) {
		prepareAnchorNode(PG, E, adjEnd, tgtOrig);
	}
}

// inserts edge e according to insertion path crossed.
// updates embeding and dual graph
void MMFixedEmbeddingInserter::insertEdge(PlanRepExpansion& PG, CombinatorialEmbedding& E,
		edge eOrig, node srcOrig, node tgtOrig, PlanRepExpansion::NodeSplit* nodeSplit,
		List<Tuple2<adjEntry, adjEntry>>& crossed) {
	preprocessInsertionPath(PG, E, srcOrig, tgtOrig, /*nodeSplit,*/ crossed);

	// remove dual nodes of faces on insertion path
	ListConstIterator<Tuple2<adjEntry, adjEntry>> it;
	for (it = crossed.begin(); it.valid() && it.succ().valid(); ++it) {
		adjEntry adj1 = (*it).x1();
		adjEntry adj2 = (*it).x2();
		if (adj2 == nullptr) {
			m_dual.delNode(m_dualOfFace[E.rightFace(adj1)]);
		} else {
			OGDF_ASSERT(adj1->theNode() == adj2->theNode());
			m_dual.delNode(m_dualOfFace[E.leftFace(adj2)]);
		}
	}

	// update primal
	PG.insertEdgePathEmbedded(eOrig, nodeSplit, E, crossed);

	// remove edges at vertices (to be split) on insertion path
	// and insert a new vertex in dual for each split node
	for (it = crossed.begin(); it.succ().valid(); ++it) {
		adjEntry adj1 = (*it).x1();
		adjEntry adj2 = (*it).x2();
		if (adj2 != nullptr) {
			node v = adj1->theNode();
			node vDual = m_dualOfNode[v];
			if (v->degree() >= 4) {
				while (vDual->firstAdj() != nullptr) {
					m_dual.delEdge(vDual->firstAdj()->theEdge());
				}
			} else {
				m_dual.delNode(vDual);
				m_dualOfNode[v] = nullptr;
			}

			node w = adj2->theNode();
			if (w->degree() >= 4) {
				m_primalNode[m_dualOfNode[w] = m_dual.newNode()] = w;
			}
		}
	}

	// insert new face nodes into dual
	OGDF_ASSERT(eOrig != nullptr
			|| nodeSplit != nullptr); // otherwise the path below is not well defined
	const List<edge>& path = (eOrig) ? PG.chain(eOrig) : nodeSplit->m_path;
	ListConstIterator<edge> itEdge;
	for (itEdge = path.begin(); itEdge.valid(); ++itEdge) {
		adjEntry adj = (*itEdge)->adjSource();
		m_dualOfFace[E.leftFace(adj)] = m_dual.newNode();
		m_dualOfFace[E.rightFace(adj)] = m_dual.newNode();
	}

	// insert new edges into dual
	for (itEdge = path.begin(), it = crossed.begin().succ(); itEdge.valid(); ++itEdge, ++it) {
		adjEntry adjSrc = (*itEdge)->adjSource();
		face f = E.rightFace(adjSrc); // face to the right of adj in loop
		node vRight = m_dualOfFace[f];
		m_delFaces->insert(f);

		adjEntry adj1 = f->firstAdj(), adj = adj1;
		do {
			face fAdj = E.leftFace(adj);
			if (!m_delFaces->isMember(fAdj)) // Don't insert edges twice
			{
				node vLeft = m_dualOfFace[fAdj];

				edge eLR = m_dual.newEdge(vLeft, vRight);
				m_dualEdge[m_primalAdj[eLR] = adj] = eLR;
				m_dualCost[eLR] = 1;

				edge eRL = m_dual.newEdge(vRight, vLeft);
				m_dualEdge[m_primalAdj[eRL] = adj->twin()] = eRL;
				m_dualCost[eRL] = 1;

			} else if (f == fAdj) {
				m_dualEdge[adj] = m_dualEdge[adj->twin()] = nullptr;
			}

			node vDual = m_dualOfNode[adj->theNode()];
			if (vDual != nullptr) {
				adjEntry adjSucc = adj->cyclicSucc();
				edge eOut = m_dual.newEdge(vDual, vRight);
				m_primalAdj[eOut] = adjSucc;
				m_dualCost[eOut] = 0;

				edge eIn = m_dual.newEdge(vRight, vDual);
				m_primalAdj[eIn] = adjSucc;
				m_dualCost[eIn] = 1;
			}
		} while ((adj = adj->faceCycleSucc()) != adj1);

		// the other face adjacent to *itEdge ...
		f = E.rightFace(adjSrc->twin());
		vRight = m_dualOfFace[f];
		m_delFaces->insert(f);

		adj1 = f->firstAdj();
		adj = adj1;
		do {
			face fAdj = E.leftFace(adj);
			if (!m_delFaces->isMember(fAdj)) {
				node vLeft = m_dualOfFace[fAdj];

				edge eLR = m_dual.newEdge(vLeft, vRight);
				m_dualEdge[m_primalAdj[eLR] = adj] = eLR;
				m_dualCost[eLR] = 1;

				edge eRL = m_dual.newEdge(vRight, vLeft);
				m_dualEdge[m_primalAdj[eRL] = adj->twin()] = eRL;
				m_dualCost[eRL] = 1;

			} else if (f == fAdj) {
				m_dualEdge[adj] = m_dualEdge[adj->twin()] = nullptr;
			}

			node vDual = m_dualOfNode[adj->theNode()];
			if (vDual != nullptr) {
				adjEntry adjSucc = adj->cyclicSucc();
				edge eOut = m_dual.newEdge(vDual, vRight);
				m_primalAdj[eOut] = adjSucc;
				m_dualCost[eOut] = 0;

				edge eIn = m_dual.newEdge(vRight, vDual);
				m_primalAdj[eIn] = adjSucc;
				m_dualCost[eIn] = 1;
			}

			adjEntry adjS1 = (*it).x1();
			adjEntry adjS2 = (*it).x2();

			if (adjS2 != nullptr) {
				node v = adjS1->theNode();
				node dualV = m_dualOfNode[v];
				if (dualV) {
					face f1 = E.leftFace(adjS1);
					face f2 = E.leftFace(adjS1->cyclicPred());

					for (adjEntry adjE : v->adjEntries) {
						face fL = E.leftFace(adjE);
						if (fL == f1 || fL == f2) {
							continue;
						}

						node vL = m_dualOfFace[fL];
						edge eOut = m_dual.newEdge(dualV, vL);
						m_primalAdj[eOut] = adjE;
						m_dualCost[eOut] = 0;
						edge eIn = m_dual.newEdge(vL, dualV);
						m_primalAdj[eIn] = adjE;
						m_dualCost[eIn] = 1;
					}
				}

				v = adjS2->theNode();
				dualV = m_dualOfNode[v];
				if (dualV) {
					face f1 = E.leftFace(adjS2);
					face f2 = E.leftFace(adjS2->cyclicPred());

					for (adjEntry adjE : v->adjEntries) {
						face fL = E.leftFace(adjE);
						if (fL == f1 || fL == f2) {
							continue;
						}

						node vL = m_dualOfFace[fL];
						edge eOut = m_dual.newEdge(dualV, vL);
						m_primalAdj[eOut] = adjE;
						m_dualCost[eOut] = 0;
						edge eIn = m_dual.newEdge(vL, dualV);
						m_primalAdj[eIn] = adjE;
						m_dualCost[eIn] = 1;
					}
				}
			}
		} while ((adj = adj->faceCycleSucc()) != adj1);
	}

	node s = path.front()->source();
	if (s->degree() == 4 && PG.splittable(s)) {
		m_primalNode[m_dualOfNode[s] = m_dual.newNode()] = s;
		insertDualEdges(s, E);
	}

	node t = path.back()->target();
	if (t->degree() == 4 && PG.splittable(t)) {
		m_primalNode[m_dualOfNode[t] = m_dual.newNode()] = t;
		insertDualEdges(t, E);
	}

	m_delFaces->clear();
}

void MMFixedEmbeddingInserter::insertDualEdge(node vDual, adjEntry adj,
		const CombinatorialEmbedding& E) {
	node vLeft = m_dualOfFace[E.leftFace(adj)];

	edge eOut = m_dual.newEdge(vDual, vLeft);
	m_primalAdj[eOut] = adj;
	m_dualCost[eOut] = 0;

	edge eIn = m_dual.newEdge(vLeft, vDual);
	m_primalAdj[eIn] = adj;
	m_dualCost[eIn] = 1;
}

void MMFixedEmbeddingInserter::insertDualEdges(node v, const CombinatorialEmbedding& E) {
	node vDual = m_dualOfNode[v];
	if (vDual) {
		for (adjEntry adj : v->adjEntries) {
			insertDualEdge(vDual, adj, E);
		}
	}
}

// removes edge eOrig; updates embedding and dual graph
void MMFixedEmbeddingInserter::removeEdge(PlanRepExpansion& PG, CombinatorialEmbedding& E,
		edge eOrig, PlanRepExpansion::NodeSplit* nodeSplit, node& oldSrc, node& oldTgt) {
	OGDF_ASSERT(eOrig != nullptr
			|| nodeSplit != nullptr); // otherwise the path below is not well defined
	const List<edge>& path = (eOrig) ? PG.chain(eOrig) : nodeSplit->m_path;

	ListConstIterator<edge> itEdge;
	for (itEdge = path.begin(); itEdge.valid(); ++itEdge) {
		adjEntry adj = (*itEdge)->adjSource();
		m_delFaces->insert(E.leftFace(adj));
		m_delFaces->insert(E.rightFace(adj));

		// remove dual of nodes that get merged
		if (itEdge != path.begin()) {
			node xl = adj->cyclicSucc()->twinNode();
			node xlDual = m_dualOfNode[xl];
			node xlOrig = PG.original(xl);
			node xr = adj->cyclicPred()->twinNode();
			node xrDual = m_dualOfNode[xr];
			node xrOrig = PG.original(xr);

			if (xlOrig != nullptr && xlOrig == xrOrig) {
				if (xlDual) {
					m_dual.delNode(xlDual);
				}
				if (xrDual) {
					m_dual.delNode(xrDual);
				}
				m_dualOfNode[xl] = m_dualOfNode[xr] = nullptr;
			}
		}
	}

	node vSrc = path.front()->source();
	if (m_dualOfNode[vSrc] != nullptr && vSrc->degree() == 4) {
		m_dual.delNode(m_dualOfNode[vSrc]);
		m_dualOfNode[vSrc] = nullptr;
	}
	node vTgt = path.back()->target();
	if (m_dualOfNode[vTgt] != nullptr && vTgt->degree() == 4) {
		m_dual.delNode(m_dualOfNode[vTgt]);
		m_dualOfNode[vTgt] = nullptr;
	}

	// delete all corresponding nodes in dual
	ListConstIterator<face> itsF;
	for (itsF = m_delFaces->faces().begin(); itsF.valid(); ++itsF) {
		m_dual.delNode(m_dualOfFace[*itsF]);
	}

	m_delFaces->clear();

	// remove edge path from PG
	PG.removeEdgePathEmbedded(E, eOrig, nodeSplit, *m_newFaces, *m_mergedNodes, oldSrc, oldTgt);

	// insert dual nodes for merged nodes
	ListConstIterator<node> itV;
	for (itV = m_mergedNodes->nodes().begin(); itV.valid(); ++itV) {
		node v = *itV;
		if (PG.splittable(v) && v->degree() >= 4) {
			node vDual = m_dualOfNode[v] = m_dual.newNode();
			m_primalNode[vDual] = v;

			for (adjEntry adj : v->adjEntries) {
				if (!m_newFaces->isMember(E.leftFace(adj))) { // other edges are inserted below!
					insertDualEdge(vDual, adj, E);
				}
			}
		}
	}
	m_mergedNodes->clear();

	// insert new nodes for new faces
	ListConstIterator<face> itF;
	for (itF = m_newFaces->faces().begin(); itF.valid(); ++itF) {
		m_dualOfFace[*itF] = m_dual.newNode();
	}

	// insert new edges into dual
	for (itF = m_newFaces->faces().begin(); itF.valid(); ++itF) {
		face f = *itF; // face to the right of adj in loop
		node vRight = m_dualOfFace[f];

		adjEntry adj1 = f->firstAdj(), adj = adj1;
		do {
			face fAdj = E.leftFace(adj);

			if (!m_newFaces->isMember(fAdj) || f->index() < fAdj->index()) {
				node vLeft = m_dualOfFace[E.leftFace(adj)];

				edge eLR = m_dual.newEdge(vLeft, vRight);
				m_dualEdge[m_primalAdj[eLR] = adj] = eLR;
				m_dualCost[eLR] = 1;

				edge eRL = m_dual.newEdge(vRight, vLeft);
				m_dualEdge[m_primalAdj[eRL] = adj->twin()] = eRL;
				m_dualCost[eRL] = 1;

			} else if (f == fAdj) {
				m_dualEdge[adj] = m_dualEdge[adj->twin()] = nullptr;
			}

			node vDual = m_dualOfNode[adj->theNode()];
			if (vDual != nullptr) {
				adjEntry adjSucc = adj->cyclicSucc();
				edge eOut = m_dual.newEdge(vDual, vRight);
				m_primalAdj[eOut] = adjSucc;
				m_dualCost[eOut] = 0;

				edge eIn = m_dual.newEdge(vRight, vDual);
				m_primalAdj[eIn] = adjSucc;
				m_dualCost[eIn] = 1;
			}
		} while ((adj = adj->faceCycleSucc()) != adj1);
	}

	m_newFaces->clear();
}

void MMFixedEmbeddingInserter::contractSplit(PlanRepExpansion& PG, CombinatorialEmbedding& E,
		PlanRepExpansion::NodeSplit* nodeSplit) {
	edge e = nodeSplit->m_path.front();
	node u = e->source();
	node v = e->target();

	if (m_dualOfNode[u]) {
		m_dual.delNode(m_dualOfNode[u]);
	}
	if (m_dualOfNode[v]) {
		m_dual.delNode(m_dualOfNode[v]);
	}

	// remove dual edges connecting dual node of left face of e
	// with dual node of right face of e
	node vl = m_dualOfFace[E.leftFace(e->adjSource())];
	adjEntry adj, adjNext;
	for (adj = vl->firstAdj(); adj != nullptr; adj = adjNext) {
		adjNext = adj->succ();

		adjEntry padj = m_primalAdj[adj->theEdge()];
		if (padj == e->adjSource() || padj == e->adjTarget()) {
			m_dual.delEdge(adj->theEdge());
		}
	}

	PG.contractSplit(nodeSplit, E);
	OGDF_ASSERT(u->degree() >= 4);

	m_primalNode[m_dualOfNode[u] = m_dual.newNode()] = u;
	insertDualEdges(u, E);
}

void MMFixedEmbeddingInserter::contractSplitIfReq(PlanRepExpansion& PG, CombinatorialEmbedding& E,
		node u, const PlanRepExpansion::nodeSplit nsCurrent) {
	edge eContract = u->firstAdj()->theEdge();
	edge eExpand = u->lastAdj()->theEdge();
	if (PG.nodeSplitOf(eContract) == nullptr) {
		std::swap(eContract, eExpand);
	}

	if (u->degree() == 2 && PG.nodeSplitOf(eContract) != nullptr
			&& PG.nodeSplitOf(eContract) != nsCurrent) {
		edge eDCS = m_dualEdge[eContract->adjSource()];
		if (eDCS) {
			m_dual.delEdge(eDCS);
		}
		edge eDCT = m_dualEdge[eContract->adjTarget()];
		if (eDCT) {
			m_dual.delEdge(eDCT);
		}
		edge eDES = m_dualEdge[eExpand->adjSource()];
		if (eDES) {
			m_dual.delEdge(eDES);
		}
		edge eDET = m_dualEdge[eExpand->adjTarget()];
		if (eDET) {
			m_dual.delEdge(eDET);
		}

		edge e = PG.unsplitExpandNode(u, eContract, eExpand, E);

		if (e->isSelfLoop()) {
			for (adjEntry adj : e->source()->adjEntries) {
				if (e == adj->theEdge()) {
					continue;
				}
				edge eDual = m_dualEdge[adj];
				if (eDual) {
					m_dual.delEdge(eDual);
				}
			}

			PG.removeSelfLoop(e, E);

		} else {
			adjEntry adj = e->adjSource();
			node vLeft = m_dualOfFace[E.leftFace(adj)];
			node vRight = m_dualOfFace[E.rightFace(adj)];

			if (vLeft != vRight) {
				edge eLR = m_dual.newEdge(vLeft, vRight);
				m_dualEdge[m_primalAdj[eLR] = adj] = eLR;
				m_dualCost[eLR] = 1;

				edge eRL = m_dual.newEdge(vRight, vLeft);
				m_dualEdge[m_primalAdj[eRL] = adj->twin()] = eRL;
				m_dualCost[eRL] = 1;
			}
		}
	}
}

void MMFixedEmbeddingInserter::collectAnchorNodes(node v, NodeSet& nodes,
		const PlanRepExpansion::NodeSplit* nsParent, const PlanRepExpansion& PG) const {
	if (PG.original(v) != nullptr) {
		nodes.insert(v);
	}

	for (adjEntry adj : v->adjEntries) {
		edge e = adj->theEdge();
		const PlanRepExpansion::NodeSplit* ns = PG.nodeSplitOf(e);
		if (ns == nullptr) {
			// add dummy nodes of non-node-split edge
			ListConstIterator<edge> it = PG.chain(PG.originalEdge(e)).begin();
			for (++it; it.valid(); ++it) {
				nodes.insert((*it)->source());
			}

		} else if (ns != nsParent) {
			// add dummy nodes of node-split edge
			ListConstIterator<edge> it = ns->m_path.begin();
			for (++it; it.valid(); ++it) {
				nodes.insert((*it)->source());
			}

			node w = (v == e->source()) ? ns->target() : ns->source();
			collectAnchorNodes(w, nodes, ns, PG);
		}
	}
}

void MMFixedEmbeddingInserter::findSourcesAndTargets(node src, node tgt, NodeSet& sources,
		NodeSet& targets, const PlanRepExpansion& PG) const {
	collectAnchorNodes(src, sources, nullptr, PG);
	collectAnchorNodes(tgt, targets, nullptr, PG);
}

void MMFixedEmbeddingInserter::anchorNodes(node vOrig, NodeSet& nodes,
		const PlanRepExpansion& PG) const {
	node vFirst = PG.expansion(vOrig).front();
	if (PG.splittableOrig(vOrig)) {
		collectAnchorNodes(vFirst, nodes, nullptr, PG);
	} else {
		nodes.insert(vFirst);
	}
}

node MMFixedEmbeddingInserter::commonDummy(NodeSet& sources, NodeSet& targets) {
	ListConstIterator<node> it;
	for (it = sources.nodes().begin(); it.valid(); ++it) {
		if (targets.isMember(*it)) {
			return *it;
		}
	}

	return nullptr;
}

void MMFixedEmbeddingInserter::convertDummy(PlanRepExpansion& PG, CombinatorialEmbedding& E, node u,
		node vOrig, PlanRepExpansion::nodeSplit ns_0) {
	PlanRepExpansion::nodeSplit ns_1 = PG.convertDummy(u, vOrig, ns_0);

	m_primalNode[m_dualOfNode[u] = m_dual.newNode()] = u;
	insertDualEdges(u, E);

	if (ns_0->m_path.size() == 1) {
		contractSplit(PG, E, ns_0);
	}
	if (ns_1->m_path.size() == 1) {
		contractSplit(PG, E, ns_1);
	}
}

bool MMFixedEmbeddingInserter::checkSplitDeg(PlanRepExpansion& PG) const {
	ListConstIterator<PlanRepExpansion::NodeSplit> it;
	for (it = PG.nodeSplits().begin(); it.valid(); ++it) {
		node src = (*it).source();
		if (src->degree() <= 2) {
			return false;
		}
		node tgt = (*it).target();
		if (tgt->degree() <= 2) {
			return false;
		}
	}

	return true;
}

bool MMFixedEmbeddingInserter::checkDualGraph(PlanRepExpansion& PG,
		const CombinatorialEmbedding& E) const {
	NodeArray<face> af(m_dual, nullptr);
	NodeArray<node> av(m_dual, nullptr);

	for (face f : E.faces) {
		node u = m_dualOfFace[f];
		if (u == nullptr) {
			return false;
		}
		if (af[u] != nullptr || av[u] != nullptr) {
			return false;
		}
		af[u] = f;
	}

	for (node v : PG.nodes) {
		if (PG.splittable(v) && v->degree() >= 4) {
			node u = m_dualOfNode[v];
			if (u == nullptr) {
				return false;
			}
			if (af[u] != nullptr || av[u] != nullptr) {
				return false;
			}
			av[u] = v;
		} else {
			if (m_dualOfNode[v] != nullptr) {
				return false;
			}
		}
	}

	Array<bool> exists(0, m_dual.maxEdgeIndex(), false);

	for (edge e : m_dual.edges) {
		exists[e->index()] = true;

		node v = e->source();
		node w = e->target();
		adjEntry adj = m_primalAdj[e];

		if (af[v] != nullptr && af[w] != nullptr) {
			if (af[v] != E.leftFace(adj)) {
				return false;
			}
			if (af[w] != E.rightFace(adj)) {
				return false;
			}
			if (m_dualEdge[adj] != e) {
				return false;
			}

		} else if (af[v] != nullptr) {
			if (adj->theNode() != av[w]) {
				return false;
			}
			if (af[v] != E.leftFace(adj)) {
				return false;
			}

		} else if (af[w] != nullptr) {
			if (adj->theNode() != av[v]) {
				return false;
			}
			if (af[w] != E.leftFace(adj)) {
				return false;
			}
		} else {
			return false;
		}
	}

	for (edge e : PG.edges) {
		if (E.leftFace(e->adjSource()) == E.rightFace(e->adjSource())) {
			if (m_dualEdge[e->adjSource()] != nullptr) {
				return false;
			}
			if (m_dualEdge[e->adjTarget()] != nullptr) {
				return false;
			}
			continue;
		}

		edge eDual = m_dualEdge[e->adjSource()];
		node srcDual = eDual->source();
		node tgtDual = eDual->target();

		if (af[srcDual] == nullptr) {
			return false;
		}
		if (af[tgtDual] == nullptr) {
			return false;
		}

		if (E.leftFace(e->adjSource()) != af[srcDual]) {
			return false;
		}
		if (E.rightFace(e->adjSource()) != af[tgtDual]) {
			return false;
		}

		if (!exists[eDual->index()]) {
			return false;
		}
#ifdef OGDF_DEBUG
		if (eDual->graphOf() != &m_dual) {
			return false;
		}

		eDual = m_dualEdge[e->adjTarget()];
		if (eDual->graphOf() != &m_dual) {
			return false;
		}
#endif
	}

	return true;
}

}
