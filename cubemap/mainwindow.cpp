#include "mainwindow.h"
#include "texture.h"
#include "transform.h"

using namespace std;

MainWindow::MainWindow() : current(0)
{
	this->CreateParam("Cubemap (press [SPACE])", CW_USEDEFAULT, CW_USEDEFAULT, 800, 600);
}

void MainWindow::ChangeModel(int i)
{
	current = i;
	program->Uniform("shadeMode", i);
	if (i == 0) {
		float diffuse[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
		program->Uniform("frontMaterial.diffuse", 1, diffuse);
	}
	else {
		float diffuse[4] = { 0.576f, 0.517f, 0.369f, 1.0f };
		program->Uniform("frontMaterial.diffuse", 1, diffuse);
	}
}

void MainWindow::OnCreate()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	
	viewer = new Viewer3D(m_rc);
	program = new ProgramObject(m_rc, "shaders/shader.vert.glsl", "shaders/shader.frag.glsl");
	if (program->IsLinked())
	{
		float specular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
		program->Uniform("cubeTex", 0);
		program->Uniform("specularMode", 1);
		program->Uniform("frontMaterial.specular", 1, specular);
		program->Uniform("frontMaterial.shininess", 128);
	}

	const char *sides1[] =
	{
		"textures/1/ft.tga",
		"textures/1/bk.tga",
		"textures/1/up.tga",
		"textures/1/dn.tga",
		"textures/1/lf.tga",
		"textures/1/rt.tga",
	};
	
	const char *sides2[] =
	{
		"textures/2/ft.tga",
		"textures/2/bk.tga",
		"textures/2/up.tga",
		"textures/2/dn.tga",
		"textures/2/lf.tga",
		"textures/2/rt.tga",
	};

	skybox[0] = new Skybox(m_rc, sides1);
	skybox[1] = new Skybox(m_rc, sides2);

	models[0] = new Actor(m_rc);
	models[0]->scale = 15.0f;
	models[0]->location = Vector3f(0.0f, -50.0f, 0.0f);
	models[0]->mesh.BindTexture(skybox[0]->GetTexture());
	models[0]->mesh.BindShader(*program);
	models[0]->mesh.LoadRaw("models/skull.obj.raw");

	models[1] = new Actor(m_rc);
	models[1]->scale = 70.0f;
	models[1]->location = Vector3f(0.0f, -30.0f, 0.0f);
	models[1]->mesh.BindTexture(skybox[1]->GetTexture());
	models[1]->mesh.BindShader(*program);
	models[1]->mesh.LoadRaw("models/teapot.obj.raw");

	ChangeModel(1);
}

void MainWindow::OnDisplay()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	m_rc->PushModelView();
		viewer->ApplyTransform();
		program->UniformMatrix("View", 1, false, m_rc->GetModelView().data);
		skybox[current]->Draw();
		models[current]->Draw();
	m_rc->PopModelView();
}

void MainWindow::OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	viewer->SetPerspective(45.0f, 1.0f, 1e6f, Point3f(0.0f, 0.0f, -200.0f), w, h);
}

void MainWindow::OnKeyDown(UINT keyCode) {
	if (keyCode == 27) DestroyWindow(m_hwnd);
	else if (keyCode == 32) {
		ChangeModel(1 - current);
		RedrawWindow();
	}
	else if (keyCode == 'Z') {
		viewer->ResetView();
		RedrawWindow();
	}
}

void MainWindow::OnMouseDown(MouseButton b, int x, int y)
{
	if (b == MouseButton::LBUTTON)
		viewer->BeginRotate(x, y);
	else viewer->BeginPan(x, y);
}

void MainWindow::OnMouseMove(UINT keysPressed, int x, int y)
{
	if (keysPressed & KM_LBUTTON) {
		viewer->Rotate(x, y);
		RedrawWindow();
	}
	else if (keysPressed & (KM_RBUTTON|KM_MBUTTON)) {
		viewer->Pan(x, y);
		RedrawWindow();
	}
}

void MainWindow::OnMouseWheel(short delta, UINT keysPressed, int x, int y)
{
	if (delta > 0) viewer->Zoom(1.1f);
	else viewer->Zoom(0.9f);
	RedrawWindow();
}

void MainWindow::OnDestroy()
{
	delete viewer;
	delete program;
	for (int i = 0; i < 2; i++) {
		delete skybox[i];
		delete models[i];
	}
	PostQuitMessage(0);
}