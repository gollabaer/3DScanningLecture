
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <qopenglshaderprogram.h>
#include <qmath.h>
extern "C" {
#include "trackball.h"
}

class MainGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	public:
		MainGLWidget(QWidget *parent = 0);
		~MainGLWidget();
		QQuaternion qrotation;
		uint count;
		QMatrix4x4 model, view, projection;
		GLfloat * vertices;
		QVector3D center;
		int rotationMode;

	protected:
		void initializeGL() Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;
		void resizeGL(int width, int height) Q_DECL_OVERRIDE;
		void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
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