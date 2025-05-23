/** \file
 * \brief Offers coloring of graphs for SimDraw.
 *
 * \author Michael Schulz and Tobias Dehling
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
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/GraphList.h>
#include <ogdf/basic/basic.h>
#include <ogdf/basic/graphics.h>
#include <ogdf/simultaneous/SimDraw.h>
#include <ogdf/simultaneous/SimDrawColorizer.h>

#include <iosfwd>


using namespace std;

namespace ogdf {

// adds some color to the edges and to the nodes
void SimDrawColorizer::addColorNodeVersion() {
	m_SD->addAttribute(GraphAttributes::nodeGraphics);
	m_SD->addAttribute(GraphAttributes::nodeStyle);
	for (node v : m_G->nodes) {
		if (m_SD->isDummy(v)) {
			if (m_SD->isProperDummy(v)) {
				m_GA->fillColor(v) = Color::Name::Darkgray;
			} else {
				m_GA->fillColor(v) = Color::Name::Black;
			}
		} else {
			m_GA->fillColor(v) = Color::Name::Yellow;
		}
	}
	addColor();
}

// adds some color to the edges
void SimDrawColorizer::addColor() {
	m_SD->addAttribute(GraphAttributes::edgeGraphics);
	m_SD->addAttribute(GraphAttributes::edgeStyle);

	SimDrawColorScheme SDCS(m_colorScheme, m_SD->numberOfBasicGraphs());
	for (edge e : m_G->edges) {
		m_GA->strokeColor(e) = SDCS.getColor(m_GA->subGraphBits(e), m_SD->numberOfBasicGraphs());
	}
}

//Implementation of class ColorScheme

// SimDrawColorScheme Constructor
SimDrawColorizer::SimDrawColorScheme::SimDrawColorScheme(colorScheme colorScm, int numberOfGraphs) {
	OGDF_ASSERT(numberOfGraphs > 0);
	OGDF_ASSERT(numberOfGraphs < 31);
	m_intScheme = colorScm;
	red = new int[numberOfGraphs];
	green = new int[numberOfGraphs];
	blue = new int[numberOfGraphs];
	assignColScm(numberOfGraphs);
}

// SimDrawColorScheme Destructor
SimDrawColorizer::SimDrawColorScheme::~SimDrawColorScheme() {
	delete[] red;
	delete[] green;
	delete[] blue;
}

// Calculates the number of overlapping graphs in one edge and gives them
// a color calculated from the choosen colorscheme
Color SimDrawColorizer::SimDrawColorScheme::getColor(int subGraphBits, int numberOfGraphs) {
	int r = 0x00, g = 0x00, b = 0x00;
	int numberOfGraphsInEdge = 0;
	Array<bool> graphs(numberOfGraphs); //Ueberlagerungen von Graphen bei dieser Kante

	/* Loest den Integerwert SubGraphBits in die einzelnen Bits auf und
	findet somit heraus, welche Graphen sich in dieser Kante ueberlagern */
	for (int i = 0; i < numberOfGraphs; i++) {
		graphs[i] = 0;
		if ((subGraphBits & (1 << i)) != 0) {
			graphs[i] = 1;
		}
	}

	/* Bestimmt den Mittelwert der Farben der uebereinanderliegenden Graphen */
	for (int i = 0; i < numberOfGraphs; i++) {
		if (graphs[i] == 1) {
			r += red[i];
			g += green[i];
			b += blue[i];
			numberOfGraphsInEdge++;
		}
	}
	if (numberOfGraphsInEdge == numberOfGraphs) {
		r = 0x00; // Kanten werden schwarz eingefaerbt
		g = 0x00; // wenn sie zu allen Graphen gehoeren
		b = 0x00;
	} else {
		OGDF_ASSERT(numberOfGraphsInEdge > 0);
		r /= numberOfGraphsInEdge; // NOLINT(clang-analyzer-core.DivideZero)
		g /= numberOfGraphsInEdge;
		b /= numberOfGraphsInEdge;
	}

	return Color(r, g, b);
}

// Stores colorscheme colors and assigns them to colorscheme objects
void SimDrawColorizer::SimDrawColorScheme::assignColScm(int numberOfGraphs) {
	// Die einzelnen Farbwerte zu den Farbschemata sind
	// in diesen Arrays hinterlegt, in der Form:
	// {FarbeArray1 R, G, B, FarbeArray2 R, G, B, ... }
	int bluYel_colors[6] = {0x1f, 0x00, 0xfa, 0xfe, 0xff, 0x02};
	int redGre_colors[6] = {0xff, 0x22, 0x18, 0x3a, 0xd1, 0x00};
	int bluOra_colors[6] = {0x00, 0x33, 0xcc, 0xff, 0x99, 0x00};
	int teaLil_colors[6] = {0x48, 0xfd, 0xff, 0xbc, 0x02, 0xbc};
	int redBluYel_colors[9] = {0xff, 0x00, 0x00, 0x34, 0x4e, 0xff, 0xfe, 0xff, 0x19};
	int greLilOra_colors[9] = {0x33, 0xff, 0x00, 0xfa, 0x00, 0x99, 0xff, 0x70, 0x00};
	/* Die Farben werden immer aus dem Array colors genommen, wenn es
	fuer die gegebene Anzahl Graphen kein vorgefertigtes Schema gibt */
	int colors[96] = {0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xa5, 0x00, 0x00,
			0xff, 0x00, 0x8a, 0x2b, 0xe2, 0xdc, 0x14, 0x3c, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff,
			0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0xb8, 0x86, 0x0b, 0xff, 0x14, 0x93, 0x1e, 0x90,
			0xff, 0xff, 0x63, 0x47, 0x80, 0x80, 0x80, 0xa5, 0x2a, 0x2a, 0xff, 0x69, 0x84, 0x20,
			0xb2, 0xaa, 0x00, 0xbf, 0xff, 0xe9, 0x96, 0x7a, 0x64, 0x95, 0xed, 0x00, 0xce, 0xd1,
			0xff, 0xd7, 0x00, 0x32, 0xcd, 0x32, 0x6b, 0x8e, 0x23, 0xde, 0xb8, 0x87, 0x55, 0x6b,
			0x2f, 0x19, 0x19, 0x70, 0xa0, 0x52, 0x2d, 0x69, 0x69, 0x69, 0x4b, 0x00, 0x82};

	/* Hier werden die Farben dem Farbschema entsprechend zugewiesen */
	switch (m_intScheme) {
	case colorScheme::bluYel:
		OGDF_ASSERT(numberOfGraphs <= 2);
		for (int i = 0; i < numberOfGraphs * 3; i += 3) {
			red[i / 3] = bluYel_colors[i];
			green[i / 3] = bluYel_colors[i + 1];
			blue[i / 3] = bluYel_colors[i + 2];
		}
		break;
	case colorScheme::redGre:
		OGDF_ASSERT(numberOfGraphs <= 2);
		for (int i = 0; i < numberOfGraphs * 3; i += 3) {
			red[i / 3] = redGre_colors[i];
			green[i / 3] = redGre_colors[i + 1];
			blue[i / 3] = redGre_colors[i + 2];
		}
		break;
	case colorScheme::bluOra:
		OGDF_ASSERT(numberOfGraphs <= 2);
		for (int i = 0; i < numberOfGraphs * 3; i += 3) {
			red[i / 3] = bluOra_colors[i];
			green[i / 3] = bluOra_colors[i + 1];
			blue[i / 3] = bluOra_colors[i + 2];
		}
		break;
	case colorScheme::teaLil:
		OGDF_ASSERT(numberOfGraphs <= 2);
		for (int i = 0; i < numberOfGraphs * 3; i += 3) {
			red[i / 3] = teaLil_colors[i];
			green[i / 3] = teaLil_colors[i + 1];
			blue[i / 3] = teaLil_colors[i + 2];
		}
		break;
	case colorScheme::redBluYel:
		OGDF_ASSERT(numberOfGraphs <= 3);
		for (int i = 0; i < numberOfGraphs * 3; i += 3) {
			red[i / 3] = redBluYel_colors[i];
			green[i / 3] = redBluYel_colors[i + 1];
			blue[i / 3] = redBluYel_colors[i + 2];
		}
		break;
	case colorScheme::greLilOra:
		OGDF_ASSERT(numberOfGraphs <= 3);
		for (int i = 0; i < numberOfGraphs * 3; i += 3) {
			red[i / 3] = greLilOra_colors[i];
			green[i / 3] = greLilOra_colors[i + 1];
			blue[i / 3] = greLilOra_colors[i + 2];
		}
		break;
	default:
		for (int i = 0; i < numberOfGraphs * 3; i += 3) {
			red[i / 3] = colors[i];
			green[i / 3] = colors[i + 1];
			blue[i / 3] = colors[i + 2];
		}
	}
}
}
