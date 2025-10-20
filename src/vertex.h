#pragma once

#include <QVector3D>

/**
 * @brief Represents a single vertex with coordinates and an rgb color.
 */
struct Vertex {
	QVector3D coords;
	QVector3D normal;
	QVector2D texCoord;

	Vertex() = default;
	Vertex(QVector3D coords, QVector3D normal, QVector2D texCoord) : coords(coords), normal(normal), texCoord(texCoord){}
};
