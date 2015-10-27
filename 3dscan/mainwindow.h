
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMatrix4x4>
#include <QMouseEvent>

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	explicit OpenGLWindow(QWindow *parent = 0);
	~OpenGLWindow();

	void render(QPainter *painter);
	void render();

	void initialize();

	void setAnimating(bool animating);

	GLfloat * vertices;
	uint count;
	QMatrix4x4 model, view, projection;
	QVector3D center;

	public slots:
	void renderLater();
	void renderNow();

protected:
	bool event(QEvent *event) Q_DECL_OVERRIDE;

	void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

private:
	bool m_update_pending;
	bool m_animating;

	GLuint m_posAttr;
	GLuint m_colAttr;
	GLuint m_matrixUniform;
	QPointF oldMousePosition = QPointF(-1,-1);

	QOpenGLShaderProgram *m_program;
	int m_frame;

	QOpenGLContext *m_context;
	QOpenGLPaintDevice *m_device;

	GLuint loadShader(GLenum type, const char *source);
	void  OpenGLWindow::mouseMoveEvent(QMouseEvent* event);
	void  OpenGLWindow::mouseReleaseEvent(QMouseEvent* event);
};