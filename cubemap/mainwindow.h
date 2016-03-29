#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "glwindow.h"
#include "viewer3d.h"
#include "shader.h"
#include "skybox.h"
#include "actor.h"

class MainWindow : public GLWindow
{
public:
	MainWindow();
private:
	Viewer3D *viewer;
	ProgramObject *program;
	Skybox *skybox[2];
	Actor *models[2];
	int current;

	void ChangeModel(int i);

	void OnCreate();
	void OnDisplay();
	void OnSize(int w, int h);
	void OnKeyDown(UINT keyCode);
	void OnMouseDown(MouseButton b, int x, int y);
	void OnMouseMove(UINT keysPressed, int x, int y);
	void OnMouseWheel(short delta, UINT keysPressed, int x, int y);
	void OnDestroy();
};

#endif // _MAINWINDOW_H_