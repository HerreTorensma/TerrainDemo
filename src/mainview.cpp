#include "mainview.h"
#include "vertex.h"

#include <QDateTime>
#include <cstddef>
#include <iostream>
#include <QtMath>

/**
 * @brief MainView::MainView Constructs a new main view.
 *
 * @param parent Parent widget.
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
	qDebug() << "MainView constructor";
	connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	
	// Refresh window at 60fps
	timer.start(1000 / 60);
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 */
MainView::~MainView() {
	qDebug() << "MainView destructor";

	makeCurrent();

	destroyModelBuffers();
	glDeleteTextures(1, &texture);
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions.
 */
void MainView::initializeGL() {
	qDebug() << ":: Initializing OpenGL";
	initializeOpenGLFunctions();

	connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

	if (debugLogger.initialize()) {
		qDebug() << ":: Logging initialized";
		debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
	}

	QString glVersion{reinterpret_cast<const char *>(glGetString(GL_VERSION))};
	qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Enable backface culling
	glDisable(GL_CULL_FACE);

	// Default is GL_LESS
	glDepthFunc(GL_LEQUAL);

	// Set the color to be used by glClear. This is, effectively, the background
	// color.
	// glClearColor(0.37f, 0.42f, 0.45f, 0.0f);
	// glClearColor(0.0f, 0.75f, 1.0f, 0.0f);
	// 242, 95, 10
	glClearColor(0.95f, 0.37f, 0.04f, 0.0f);

	createShaderProgram();
	
	// loadHeightMap(":/textures/iceland_lores.png");
	loadHeightMap(":/textures/iceland.png");
	
	modelMatrix.setToIdentity();
	projectionMatrix.setToIdentity();
	viewMatrix.setToIdentity();
	
	// Initialize transformations
	updateProjectionTransform();
	updateModelTransforms();

	// Make the mouse behave for the FPS controller
	setMouseTracking(true);

	lavaPlaneVertices = {
		{ QVector3D(-1.0f, 0.0f, -1.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(0.0f, 0.0f) },
		{ QVector3D( 1.0f, 0.0f, -1.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(1.0f, 0.0f) },
		{ QVector3D( 1.0f, 0.0f,  1.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(1.0f, 1.0f) },
		{ QVector3D(-1.0f, 0.0f,  1.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(0.0f, 1.0f) },
	};

	lavaPlaneIndices = {
		0, 1, 2,
		2, 3, 0
	};

	{
		glGenVertexArrays(1, &lavaPlaneVAO);
		glBindVertexArray(lavaPlaneVAO);

		glGenBuffers(1, &lavaPlaneVBO);
		glBindBuffer(GL_ARRAY_BUFFER, lavaPlaneVBO);
		glBufferData(GL_ARRAY_BUFFER, lavaPlaneVertices.size() * sizeof(Vertex), lavaPlaneVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, coords));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &lavaPlaneEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lavaPlaneEBO);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			lavaPlaneIndices.size() * sizeof(unsigned int),
			lavaPlaneIndices.data(),
			GL_STATIC_DRAW
		);
	}

	{
		QImage lavaImage = QImage(":/textures/lava.png");

		QVector<quint8> data = imageToBytes(lavaImage);

		glGenTextures(1, &lavaTexture);
		glBindTexture(GL_TEXTURE_2D, lavaTexture);
		
		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Upload data
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			lavaImage.width(),
			lavaImage.height(),
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data.data()
		);

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	loadSkyBox();
}

/**
 * @brief MainView::createShaderProgram Creates a new shader program with a
 * vertex and fragment shader.
 */
void MainView::createShaderProgram() {
	terrainShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader_terrain.glsl");
	terrainShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_terrain.glsl");
	terrainShader.link();

	lavaShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader_lava.glsl");
	lavaShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_lava.glsl");
	lavaShader.link();

	skyboxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader_skybox.glsl");
	skyboxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_skybox.glsl");
	skyboxShader.link();
}

void MainView::loadSkyBox() {
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	
	// Load each texture
	QImage backImage = QImage(":/textures/skybox/back.png");
	backImage = backImage.mirrored(false, true);
	QVector<quint8> backImageData = imageToBytes(backImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, backImage.width(), backImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, backImageData.data());

	QImage bottomImage = QImage(":/textures/skybox/bottom.png");
	bottomImage = bottomImage.mirrored(false, true);
	QVector<quint8> bottomImageData = imageToBytes(bottomImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, backImage.width(), backImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bottomImageData.data());

	QImage frontImage = QImage(":/textures/skybox/front.png");
	frontImage = frontImage.mirrored(false, true);
	QVector<quint8> frontImageData = imageToBytes(frontImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, backImage.width(), backImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, frontImageData.data());

	QImage leftImage = QImage(":/textures/skybox/left.png");
	leftImage = leftImage.mirrored(false, true);
	QVector<quint8> leftImageData = imageToBytes(leftImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, backImage.width(), backImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, leftImageData.data());

	QImage rightImage = QImage(":/textures/skybox/right.png");
	rightImage = rightImage.mirrored(false, true);
	QVector<quint8> rightImageData = imageToBytes(rightImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, backImage.width(), backImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, rightImageData.data());

	QImage topImage = QImage(":/textures/skybox/top.png");
	topImage = topImage.mirrored(false, true);
	QVector<quint8> topImageData = imageToBytes(topImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, backImage.width(), backImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, topImageData.data());

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	{
		float s = 1.0f * 0.5f;

		QVector<QVector3D> cubePositions = {
			// Front face
			{ -s, -s,  s }, {  s, -s,  s }, {  s,  s,  s },
			{ -s, -s,  s }, {  s,  s,  s }, { -s,  s,  s },

			// Back face
			{  s, -s, -s }, { -s, -s, -s }, { -s,  s, -s },
			{  s, -s, -s }, { -s,  s, -s }, {  s,  s, -s },

			// Left face
			{ -s, -s, -s }, { -s, -s,  s }, { -s,  s,  s },
			{ -s, -s, -s }, { -s,  s,  s }, { -s,  s, -s },

			// Right face
			{  s, -s,  s }, {  s, -s, -s }, {  s,  s, -s },
			{  s, -s,  s }, {  s,  s, -s }, {  s,  s,  s },

			// Top face
			{ -s,  s,  s }, {  s,  s,  s }, {  s,  s, -s },
			{ -s,  s,  s }, {  s,  s, -s }, { -s,  s, -s },

			// Bottom face
			{ -s, -s, -s }, {  s, -s, -s }, {  s, -s,  s },
			{ -s, -s, -s }, {  s, -s,  s }, { -s, -s,  s }
		};

		glGenVertexArrays(1, &skyboxVAO);
		glBindVertexArray(skyboxVAO);

		glGenBuffers(1, &skyboxVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, cubePositions.size() * sizeof(QVector3D), cubePositions.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*)0);
		glEnableVertexAttribArray(0);
	}
}

void MainView::loadHeightMap(const QString &filename) {
	QImage image = QImage(filename);
 
	QVector<Vertex> vertices;

	int width = image.width();
	int height = image.height();
	
	// float yScale = 0.1f;
	// float yScale = 0.05f;
	float yScale = 0.5f;
	float xzScale = 1.0f;

	// float yScale = 0.05f;
	// float xzScale = 1.0f;

	float maxHeight = 255.0f * yScale;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			QColor color = image.pixelColor(j, i);
			unsigned char y = color.red();
			
			QVector3D position(
				(-height/2.0f + i) * xzScale,
				-(maxHeight / 2.0f) + (int)y * yScale,
				(-width/2.0f + j) * xzScale
			);

			float hl = (i > 0) ? image.pixelColor(j, i-1).red() * yScale : y * yScale;
			float hr = (i < height-1) ? image.pixelColor(j, i+1).red() * yScale : y * yScale;
			float hd = (j > 0) ? image.pixelColor(j-1, i).red() * yScale : y * yScale;
			float hu = (j < width-1) ? image.pixelColor(j+1, i).red() * yScale : y * yScale;

			QVector3D normal(hl - hr, 2.0f, hd - hu);
			normal.normalize();

			QVector2D uv(float(j) / (width - 1) * 128 , float(i) / (height - 1) * 128);

			Vertex vertex(position, normal, uv);

			vertices.push_back(vertex);
		}
	}

	lavaPlaneModelMatrix.setToIdentity();
	// lavaPlaneModelMatrix.translate(QVector3D(0.0f, -maxHeight/2.0f + yScale, 0.0f));
	lavaPlaneModelMatrix.translate(QVector3D(0.0f, -maxHeight/2.0f, 0.0f));
	// lavaPlaneModelMatrix.translate(QVector3D(0.0f, -maxHeight/2.0f - 1000, 0.0f));
	lavaPlaneModelMatrix.scale(10000.0f);

	terrainYScale = yScale;

	QVector<unsigned int> indices;
	
	for (unsigned int i = 0; i < height-1; i++) {
		for (unsigned int j = 0; j < width; j++) {
			for (unsigned int k = 0; k < 2; k++) {
				indices.push_back(j + width * (i + k));
			}
		}
	}

	terrainStripsAmount = height - 1;
	terrainVerticesPerStrip = width * 2;

	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, coords));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &terrainEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int),
		indices.data(),
		GL_STATIC_DRAW
	);

	// Textures
	QImage flatImage = QImage(":/textures/grass.png");

	QVector<quint8> flatImageData = imageToBytes(flatImage);

	glGenTextures(1, &flatTexture);
	glBindTexture(GL_TEXTURE_2D, flatTexture);
	
	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload data
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		flatImage.width(),
		flatImage.height(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		flatImageData.data()
	);

	glGenerateMipmap(GL_TEXTURE_2D);

	QImage steepImage = QImage(":/textures/ground.png");

	QVector<quint8> steepImageData = imageToBytes(steepImage);

	glGenTextures(1, &steepTexture);
	glBindTexture(GL_TEXTURE_2D, steepTexture);
	
	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload data
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		steepImage.width(),
		steepImage.height(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		steepImageData.data()
	);

	glGenerateMipmap(GL_TEXTURE_2D);
}

void MainView::updateCamera(Camera& camera) {
	const float speed = 0.5f;

	int forward = camera.wKeyPressed - camera.sKeyPressed;
	camera.position += camera.front * forward * speed;

	int right = camera.dKeyPressed - camera.aKeyPressed;
	camera.position += QVector3D::crossProduct(camera.front, camera.up) * right * speed;
	
	QVector3D direction(
		cos(qDegreesToRadians(camera.yaw)) * cos(qDegreesToRadians(camera.pitch)),
		sin(qDegreesToRadians(camera.pitch)),
		sin(qDegreesToRadians(camera.yaw)) * cos(qDegreesToRadians(camera.pitch))
	);

	camera.front = direction.normalized();
	
	viewMatrix.setToIdentity();
	viewMatrix.lookAt(camera.position, camera.position + camera.front, camera.up);

	updateProjectionTransform();
	updateModelTransforms();
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
 void MainView::paintGL() {
	updateCamera(camera);
	time += 1.0f;
	
	// Clear the screen before rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	{
		skyboxShader.bind();
		glDepthMask(GL_FALSE);

		// QMatrix4x4 view;
		// view.setToIdentity();
		// view.lookAt();

		QMatrix4x4 viewNoTranslation = viewMatrix;
		viewNoTranslation.setColumn(3, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));

		skyboxShader.setUniformValue("viewMatrix", viewNoTranslation);
		skyboxShader.setUniformValue("projectionMatrix", projectionMatrix);

		glBindVertexArray(skyboxVAO);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		terrainShader.setUniformValue("skyboxTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthMask(GL_TRUE);
		skyboxShader.release();
	}
	
	{
		lavaShader.bind();

		lavaShader.setUniformValue("modelMatrix", lavaPlaneModelMatrix);
		lavaShader.setUniformValue("viewMatrix", viewMatrix);
		lavaShader.setUniformValue("projectionMatrix", projectionMatrix);
		lavaShader.setUniformValue("time", time);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lavaTexture);
		terrainShader.setUniformValue("lavaTexture", 0);

		glBindVertexArray(lavaPlaneVAO);
		glDrawElements(GL_TRIANGLES, lavaPlaneIndices.size(), GL_UNSIGNED_INT, 0);

		lavaShader.release();
	}

	{
		terrainShader.bind();
		terrainShader.setUniformValue("modelMatrix", modelMatrix);
		terrainShader.setUniformValue("viewMatrix", viewMatrix);
		terrainShader.setUniformValue("projectionMatrix", projectionMatrix);
		terrainShader.setUniformValue("yScale", terrainYScale);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, flatTexture);
		terrainShader.setUniformValue("flatTexture", 0);
	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, steepTexture);
		terrainShader.setUniformValue("steepTexture", 1);
	
		glBindVertexArray(terrainVAO);
		for (unsigned int strip = 0; strip < terrainStripsAmount; strip++) {
			glDrawElements(
				GL_TRIANGLE_STRIP,
				terrainVerticesPerStrip,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * terrainVerticesPerStrip * strip)
			);
		}
		terrainShader.release();
	}
}

/**
 * @brief MainView::resizeGL Called upon resizing of the screen.
 *
 * @param newWidth The new width of the screen in pixels.
 * @param newHeight The new height of the screen in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight) {
	Q_UNUSED(newWidth)
	Q_UNUSED(newHeight)
	updateProjectionTransform();
}

/**
 * @brief MainView::updateProjectionTransform Updates the projection transform
 * matrix taking into consideration the current aspect ratio.
 */
void MainView::updateProjectionTransform() {
	float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());
	projectionMatrix.setToIdentity();
	projectionMatrix.perspective(60.0f, aspectRatio, 0.1f, 10000.0f);
}

/**
 * @brief MainView::updateModelTransforms Updates the model transform matrix of
 * the mesh to reflect the current rotation and scale values.
 */
void MainView::updateModelTransforms() {
	modelMatrix.setToIdentity();
}

/**
 * @brief MainView::destroyModelBuffers Cleans up the memory used by OpenGL.
 */
void MainView::destroyModelBuffers() {
	// TODO: delete terrain stuff
}

/**
 * @brief MainView::setRotation Changes the rotation of the displayed objects.
 * @param rotateX Number of degrees to rotate around the x axis.
 * @param rotateY Number of degrees to rotate around the y axis.
 * @param rotateZ Number of degrees to rotate around the z axis.
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
	rotation = {static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ)};
	updateModelTransforms();
}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
void MainView::setScale(float newScale) {
	scale = newScale;
	updateModelTransforms();
}

/**
 * @brief MainView::onMessageLogged OpenGL logging function, do not change.
 *
 * @param Message The message to be logged.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
	qDebug() << " → Log:" << Message;
}
