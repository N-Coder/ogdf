#include <ogdf/basic/precondition.h>

bool ogdf::dfsGenTreeRec(ogdf::UMLGraph& UG, ogdf::EdgeArray<bool, true>& used,
		ogdf::NodeArray<int, true>& hierNumber, int hierNum, ogdf::node v,
		ogdf::List<ogdf::edge>& fakedGens, bool fakeTree) {
	OGDF_ASSERT(hierNumber[v] == 0);
	hierNumber[v] = hierNum;

	bool returnValue = true;

	for (adjEntry adj : v->adjEntries) {
		edge e = adj->theEdge();
		if (e->source() == v) {
			continue;
		}
		if (!(UG.type(e) == Graph::EdgeType::generalization)) {
			continue;
		}
		if (used[e]) {
			continue; //error ??
		}
		used[e] = true;

		node w = e->opposite(v);

		if (hierNumber[w]) {
			//temporarily fake trees
			// if (hierNumber[w] == hierNum) //forward search edge
			if (fakeTree) {
#if 0
				UG.type(e) = Graph::association;
#endif
				fakedGens.pushBack(e);
				continue;
			} else {
				return false; //reached w over unused edge => no tree
			}
		}

		returnValue = dfsGenTreeRec(UG, used, hierNumber, hierNum, w, fakedGens, fakeTree);
		//shortcut
		if (!returnValue) {
			return false;
		}
	}

	return returnValue;
}

ogdf::edge ogdf::firstOutGen(ogdf::UMLGraph& UG, ogdf::node v, ogdf::EdgeArray<bool, true>&) {
	for (adjEntry adj : v->adjEntries) {
		edge e = adj->theEdge();
		if (e->target() == v) {
			continue;
		}
		if (UG.type(e) == Graph::EdgeType::generalization) {
#if 0
			OGDF_ASSERT(!used[e]);
#endif
			return e;
		} else {
			continue;
		}
	}
	return nullptr;
}

bool ogdf::dfsGenTree(ogdf::UMLGraph& UG, ogdf::List<ogdf::edge>& fakedGens, bool fakeTree) {
	EdgeArray<bool> used(UG.constGraph(), false);
#if 0
	NodeArray<bool> visited(UG,false);
#endif
	NodeArray<int> hierNumber(UG.constGraph(), 0);

	int hierNum = 0; //number of hierarchy tree

	const Graph& G = UG.constGraph();
	for (edge e : G.edges) {
		//descent in the hierarchy containing e
		if (!used[e] && UG.type(e) == Graph::EdgeType::generalization) {
			hierNum++; //current hierarchy tree
			//first we search for the sink
			node sink = e->target();
			edge sinkPath = firstOutGen(UG, e->target(), used);
			int cycleCounter = 0;
			while (sinkPath) {
				sink = sinkPath->target();
				sinkPath = firstOutGen(UG, sinkPath->target(), used);
				cycleCounter++;
				//if there is no sink, convert Generalizations to Associations and draw
				if (cycleCounter > G.numberOfEdges()) {
					UG.type(sinkPath) = Graph::EdgeType::association;
					fakedGens.pushBack(sinkPath);
					sink = sinkPath->source();
					sinkPath = nullptr;
				}
			}

			//now sink is the hierarchy sink

			//used is set in dfsGenTreeRec
			bool isTree = dfsGenTreeRec(UG, used, hierNumber, hierNum, sink, fakedGens, fakeTree);
			if (!isTree) {
				return false;
			}
		}
	}

	return true;
}
