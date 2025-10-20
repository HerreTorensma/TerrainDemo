#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class is the main application window. Among other
 * things, it handles the inputs from the screen widgets.
 */
class MainWindow : public QMainWindow {
	Q_OBJECT

 public:
	Ui::MainWindow *ui;

	explicit MainWindow(QWidget *parent = nullptr);
	void renderToFile();
	~MainWindow() override;
};
