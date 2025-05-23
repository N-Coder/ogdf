/** \file
 * \brief Declaration of basic page rank.
 *
 * \author Martin Gronemann
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

namespace ogdf {

//! Basic page rank calculation.
/**
 * @ingroup graph-algs
 */
class BasicPageRank {
public:
	BasicPageRank() { initDefaultOptions(); }

	//! main algorithm call
	void call(const Graph& graph, const EdgeArray<double>& edgeWeight,
			NodeArray<double>& pageRankResult);

	//! sets the default options.
	void initDefaultOptions() {
		m_dampingFactor = 0.85;
		m_maxNumIterations = 1000;
		m_threshold = 0.0;
	}

	//! returns the damping factor for each iteration (default is 0.85)
	double dampingFactor() const { return m_dampingFactor; }

	//! sets the damping factor for each iteration (default is 0.85)
	void setDampingFactor(double dampingFactor) { m_dampingFactor = dampingFactor; }

	//! the maximum number of iterations (default is 1000)
	int maxNumIterations() const { return m_maxNumIterations; }

	//! sets the maximum number of iterations (default is 1000)
	void setMaxNumIterations(int maxNumIterations) { m_maxNumIterations = maxNumIterations; }

	/*! returns the threshold/epsilon. After each iteration the result is compared to
	 * to the old one and in case all changes are smaller than threshold the algorithm
	 * stops. Note that the default value is 0.0 resulting in maxNumIterations usually.
	 */
	double threshold() const { return m_threshold; }

	//! sets the threshold to t. See threshold for more information
	void setThreshold(double t) { m_threshold = t; }

private:
	//! the damping factor
	double m_dampingFactor;

	//! maximum number of iterations
	int m_maxNumIterations;

	//! the threshold
	double m_threshold;
};

}
