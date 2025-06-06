/** \file
 * \brief Declaration of class PlanarStraightLayout which represents
 *        a planar straight-line drawing algorithm.
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

#pragma once

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/basic.h>
#include <ogdf/basic/geometry.h>
#include <ogdf/augmentation/AugmentationModule.h>
#include <ogdf/planarity/EmbedderModule.h>
#include <ogdf/planarlayout/GridLayoutModule.h>
#include <ogdf/planarlayout/ShellingOrderModule.h>

#include <memory>

namespace ogdf {
class GridLayout;
class ShellingOrder;

/**
 * \brief Implementation of the Planar-Straight layout algorithm.
 *
 * The class PlanarStraightLayout implements a straight-line drawing algorithm
 * for planar graphs. It draws a planar graph on a grid of size at
 * most (2<i>n</i>-4) * (<i>n</i>-2) without edge crossings.
 *
 * The algorithm runs in several phases. In the first phase,
 * the graph is augmented by adding edges to achieve a certain connectivity
 * (e.g., biconnected or triconnected). Then, a shelling order of the
 * the resulting graph is computed. Both phases are implemented by using
 * module options, so you can replace them with different implementations.
 * However, you have to make sure, that the connectivity achieved by the
 * augmentation module is sufficient for the shelling order module (or the
 * input graph already has the required connectivity).
 *
 * The implementation used in PlanarStraightLayout is an improved version of
 * an algorithm presented in:
 *
 * Goos Kant: <i>Drawing Planar Graphs Using the Canonical Ordering</i>.
 * Algorithmica 16(1), pp. 4-32, 1996.
 *
 * <H3>Precondition</H3>
 * The input graph needs to be planar and simple (i.e., no self-loops and no
 * multiple edges).
 *
 * <H3>Optional parameters</H3>
 *
 * <table>
 *   <tr>
 *     <th><i>Option</i><th><i>Type</i><th><i>Default</i><th><i>Description</i>
 *   </tr><tr>
 *     <td><i>sizeOptimization</i><td>bool<td>true
 *     <td>If this option is set to true, the algorithm tries to reduce the
 *     size of the resulting grid layout.
 *   </tr><tr>
 *     <td><i>baseRatio</i><td>double<td>0.33
 *     <td>This option specifies the maximal number of nodes placed on the base line.
 *     The algorithm tries to place as many nodes as possible on the base
 *     line, but takes at most max(2, \a baseRatio * size of external face) many.
 *   </tr>
 * </table>
 *
 * <H3>%Module options</H3>
 * Instances of type PlanarDrawLayout provide the following module options:
 *
 * <table>
 *   <tr>
 *     <th><i>Option</i><th><i>Type</i><th><i>Default</i><th><i>Description</i>
 *   </tr><tr>
 *     <td><i>augmenter</i><td>AugmentationModule<td>PlanarAugmentation
 *     <td>Augments the graph by adding edges to obtain a planar graph with
 *     a certain connectivity (e.g., biconnected or triconnected).
 *   </tr><tr>
 *     <td><i>embedder</i><td>EmbedderModule<td>SimpleEmbedder
 *     <td>Planar embedding algorithm applied after planar augmentation.
 *   </tr><tr>
 *     <td><i>shellingOrder</i><td>ShellingOrderModule<td>BiconnectedShellingOrder
 *     <td>The algorithm to compute a shelling order. The connectivity assured
 *     by the planar augmentation module has to be sufficient for the shelling
 *     order module!
 *   </tr>
 * </table>
 *
 * <H3>Running Time</H3>
 * The computation of the layout takes time O(<I>n</I>), where <I>n</I> is
 * the number of nodes of the input graph.
 */
class OGDF_EXPORT PlanarStraightLayout : public PlanarGridLayoutModule {
public:
	//! Constructs an instance of the Planar-Straight layout algorithm.
	PlanarStraightLayout();

	~PlanarStraightLayout() { }

	/**
	 *  @name Optional parameters
	 *  @{
	 */

	/**
	 * \brief Returns the current setting of option sizeOptimization.
	 *
	 * If this option is set to true, the algorithm tries to reduce the
	 * size of the resulting grid layout.
	 */
	bool sizeOptimization() const { return m_sizeOptimization; }

	//! Sets the option sizeOptimization to \p opt.
	void sizeOptimization(bool opt) { m_sizeOptimization = opt; }

	/**
	 * \brief Returns the current setting of option baseRatio.
	 *
	 * This option specifies the maximal number of nodes placed on the base line.
	 * The algorithm tries to place as many nodes as possible on the base
	 * line, but takes at most max(2, #m_baseRatio * size of external face) many.
	 */
	double baseRatio() const { return m_baseRatio; }

	//! Sets the option baseRatio to \p ratio.
	void baseRatio(double ratio) { m_baseRatio = ratio; }

	/** @}
	 *  @name Module options
	 *  @{
	 */

	/**
	 * \brief Sets the augmentation module.
	 *
	 * The augmentation module needs to make sure that the graph gets the
	 * connectivity required for calling the shelling order module.
	 */
	void setAugmenter(AugmentationModule* pAugmenter) { m_augmenter.reset(pAugmenter); }

	//! Sets the shelling order module.
	void setShellingOrder(ShellingOrderModule* pOrder) { m_computeOrder.reset(pOrder); }

	//! Sets the module option for the graph embedding algorithm.
	void setEmbedder(EmbedderModule* pEmbedder) { m_embedder.reset(pEmbedder); }

	//! @}

private:
	bool m_sizeOptimization; //!< The option for size optimization.
	double m_baseRatio; //!< The option for specifying the base ratio.

	std::unique_ptr<EmbedderModule> m_embedder; //!< The planar embedder module.
	std::unique_ptr<AugmentationModule> m_augmenter; //!< The augmentation module.
	std::unique_ptr<ShellingOrderModule> m_computeOrder; //!< The shelling order module.

	virtual void doCall(const Graph& G, adjEntry adjExternal, GridLayout& gridLayout,
			IPoint& boundingBox, bool fixEmbedding) override;

	void computeCoordinates(const Graph& G, ShellingOrder& lmc, NodeArray<int>& x, NodeArray<int>& y);
};

}
