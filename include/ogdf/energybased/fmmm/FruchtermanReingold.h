/** \file
 * \brief Declaration of class FruchtermanReingold (computation of forces).
 *
 * \author Stefan Hachul
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
#include <ogdf/basic/basic.h>
#include <ogdf/basic/geometry.h>

namespace ogdf::energybased::fmmm {
class NodeAttributes;
} // namespace ogdf::energybased::fmmm

namespace ogdf {
namespace energybased {
namespace fmmm {

class OGDF_EXPORT FruchtermanReingold {
public:
	//! Constructor
	FruchtermanReingold();

	//! Calculate exact rep. forces for each node.
	void calculate_exact_repulsive_forces(const Graph& G, NodeArray<NodeAttributes>& A,
			NodeArray<DPoint>& F_rep);

	//! Grid approximation of rep.forces for each node.
	void calculate_approx_repulsive_forces(const Graph& G, NodeArray<NodeAttributes>& A,
			NodeArray<DPoint>& F_rep);

	//! Make all initialisations that are needed for FruchtermanReingold.
	void make_initialisations(double boxlength, DPoint down_left_corner, int grid_quotient);

	//! Import updated information of the drawing area.
	void update_boxlength_and_cornercoordinate(double b_l, DPoint d_l_c) {
		boxlength = b_l;
		down_left_corner = d_l_c;
	}

private:
	int _grid_quotient; //!< for coarsening the FrRe-grid
	int max_gridindex; //!< maximum index of a grid row/column
	double boxlength; //!< length of drawing box
	DPoint down_left_corner; //!< down left corner of drawing box

	//! The number k of rows and colums of the grid is sqrt(|V|) / frGridQuotient()
	//! (Note that in [FrRe] frGridQuotient() is 2.)
	void grid_quotient(int p) { _grid_quotient = ((0 <= p) ? p : 2); }

	int grid_quotient() const { return _grid_quotient; }
};

}
}
}
