/** \file
 * \brief Places nodes at the barycenter of his neighbors.
 *
 * \author Gereon Bartel
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

#include <ogdf/basic/basic.h>
#include <ogdf/energybased/multilevel_mixer/InitialPlacer.h>

namespace ogdf {
class MultilevelGraph;

//! The barycenter placer for multilevel layout.
/**
 * @ingroup gd-multi
 */
class OGDF_EXPORT BarycenterPlacer : public InitialPlacer {
private:
	bool m_weightedPositions;

public:
	BarycenterPlacer();

	void placeOneLevel(MultilevelGraph& MLG) override;
	void placeOneNode(MultilevelGraph& MLG);
	void weightedPositionPriority(bool on);
};

}
