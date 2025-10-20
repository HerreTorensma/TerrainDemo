#pragma once

#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>

struct Camera {
	// Kinda bad code but it needs to be finished tomorrow
	bool wKeyPressed = false;
	bool sKeyPressed = false;
	bool aKeyPressed = false;
	bool dKeyPressed = false;

	QVector3D worldUp = QVector3D(0.0f, 1.0f, 0.0f);
	
	QVector3D position = QVector3D(0.0f, 0.0f, 0.0f);
	QVector3D front = QVector3D(0.0f, 0.0f, -1.0f).normalized();
	QVector3D up = QVector3D(0.0f, 1.0f, 0.0f);
	QVector3D right = QVector3D(1.0f, 0.0f, 0.0f);

	int lastMouseX;
	int lastMouseY;

	float yaw = -90.0f;
	float pitch = 0.0f;
};

