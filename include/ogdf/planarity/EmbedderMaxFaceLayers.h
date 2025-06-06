/** \file
 * \brief Declares ogdf::EmbedderMaxFaceLayers.
 *
 * \author Thorsten Kerkhof
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

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>
#include <ogdf/planarity/EmbedderMaxFace.h>
#include <ogdf/planarity/embedder/LayersBlockEmbedder.h>

namespace ogdf {

//! Embedder that maximizes the external face and optimizes the position of blocks afterwards.
/**
 * @ingroup ga-planembed
 *
 * After computing an embedding just as ogdf::EmbedderMaxFace does,
 * blocks are placed in faces that are closest to the external one.
 *
 * See the paper "Graph Embedding with Minimum Depth and Maximum External Face"
 * by C. Gutwenger and P. Mutzel (2004)
 * and diploma thesis  "Algorithmen zur Bestimmung von guten Graph-Einbettungen für orthogonale Zeichnungen" (in German)
 * by Thorsten Kerkhof (2007) for details.
 */
class OGDF_EXPORT EmbedderMaxFaceLayers : public embedder::LayersBlockEmbedder<EmbedderMaxFace, int> {
protected:
	void embedBlock(const node& bT, const node& cT, ListIterator<adjEntry>& after) override;

	adjEntry trivialInit(Graph& G) override { return EmbedderMaxFace::trivialInit(G)->twin(); }
};

}
