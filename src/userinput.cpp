#include <QDebug>

#include "mainview.h"

/**
 * @brief MainView::keyPressEvent Triggered by a key press.
 * @param ev Key event.
 */
void MainView::keyPressEvent(QKeyEvent *ev) {
	switch (ev->key()) {
		case 'W':
			camera.wKeyPressed = true;
			break;

		case 'S':
			camera.sKeyPressed = true;
			break;

		case 'A':
			camera.aKeyPressed = true;
			break;

		case 'D':
			camera.dKeyPressed = true;
			break;

		// case 'Esc':
		// 	mouseCaptured = false;
		// 	setCursor(Qt::ArrowCursor);
		// 	break;

		default:
			// ev->key() is an integer. For alpha numeric characters keys it
			// equivalent with the char value ('A' == 65, '1' == 49) Alternatively,
			// you could use Qt Key enums, see http://doc.qt.io/qt-6/qt.html#Key-enum
			qDebug() << ev->key() << "pressed";
			break;
	}
}

/**
 * @brief MainView::keyReleaseEvent Triggered by a key released.
 * @param ev Key event.
 */
void MainView::keyReleaseEvent(QKeyEvent *ev) {
	switch (ev->key()) {
		case 'W':
			camera.wKeyPressed = false;
			break;

		case 'S':
			camera.sKeyPressed = false;
			break;

		case 'A':
			camera.aKeyPressed = false;
			break;

		case 'D':
			camera.dKeyPressed = false;
			break;

		default:
			qDebug() << ev->key() << "released";
			break;
	}
}

/**
 * @brief MainView::mouseDoubleClickEvent Triggered by clicking two subsequent
 * times on any mouse button. It also fires two mousePress and mouseRelease
 * events.
 * @param ev Mouse events.
 */
void MainView::mouseDoubleClickEvent(QMouseEvent *ev) {
	qDebug() << "Mouse double clicked:" << ev->button();
}

/**
 * @brief MainView::mouseMoveEvent Triggered when moving the mouse inside the
 window (only when the mouse is clicked).
 * @param ev Mouse event.
 */
void MainView::mouseMoveEvent(QMouseEvent *ev) {
	qDebug() << "x" << ev->position().x() << "y" << ev->position().y();

	if (!mouseCaptured) {
		return;
	}

	QPointF globalPos = ev->globalPosition();
    QPointF globalCenter = mapToGlobal(rect().center());

    float dx = globalPos.x() - globalCenter.x();
    float dy = globalPos.y() - globalCenter.y();

	camera.yaw += dx * 0.1f;
	camera.pitch -= dy * 0.1f;

	camera.lastMouseX = dx;
	camera.lastMouseY = dy;

	if (camera.pitch > 89.0f) {
		camera.pitch = 89.0f;
	}
	if (camera.pitch < -89.0f) {
		camera.pitch = -89.0f;
	}

	QCursor::setPos(mapToGlobal(rect().center()));
}

/**
 * @brief MainView::mousePressEvent Triggered when pressing any mouse button.
 * @param ev Mouse event.
 */
void MainView::mousePressEvent(QMouseEvent *ev) {
	qDebug() << "Mouse button pressed:" << ev->button();

	setCursor(Qt::BlankCursor);
	mouseCaptured = true;

	// Do not remove the line below, clicking must focus on this widget!
	setFocus();
}

/**
 * @brief MainView::mouseReleaseEvent Triggered when releasing any mouse button.
 * @param ev Mouse event.
 */
void MainView::mouseReleaseEvent(QMouseEvent *ev) {
	qDebug() << "Mouse button released" << ev->button();
}

/**
 * @brief MainView::wheelEvent Triggered when clicking scrolling with the scroll
 * wheel on the mouse.
 * @param ev Mouse event.
 */
void MainView::wheelEvent(QWheelEvent *ev) {
	qDebug() << "Mouse wheel:" << ev->angleDelta();
}
