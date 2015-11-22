#ifndef MAINGL_WIDGET_H
#define MAINGL_WIDGET_H

#include <vector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <qopenglshaderprogram.h>
#include "Point3d.h"
#include "Camera.h"



class MainGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	public:
		MainGLWidget(QWidget *parent = 0);
		~MainGLWidget();
		Camera cam;
		uint count;
		GLfloat* vertices;
		GLfloat* colors;

	protected:
		void initializeGL() Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;
		void resizeGL(int width, int height) Q_DECL_OVERRIDE;
		void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
		void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

	private:
		QOpenGLVertexArrayObject m_vao;
		QOpenGLBuffer m_glBuffer;
		QOpenGLShaderProgram *m_program;
		QMatrix4x4 m_proj;
		QMatrix4x4 m_camera;
		QMatrix4x4 m_world;
		QPointF oldMousePosition;
		GLuint m_posAttr;
		GLuint m_colAttr;
		GLuint m_matrixUniform;
};

#endif // MAINGL_WIDGET_H