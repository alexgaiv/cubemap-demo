#include "mainwindow.h"
#include "texture.h"
#include "transform.h"

using namespace std;

MainWindow::MainWindow() : current(0)
{
	this->Create("Cubemap (press [SPACE])", CW_USEDEFAULT, CW_USEDEFAULT, 800, 600);
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
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	camera = new TrackballCamera(m_rc);
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
	skybox[0]->GetTexture().BuildMipmaps();
	skybox[1]->GetTexture().BuildMipmaps();
	skybox[0]->GetTexture().SetFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	skybox[1]->GetTexture().SetFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	models[0] = new Model(m_rc);
	models[0]->scale = Vector3f(15.0f);
	models[0]->location = Vector3f(0.0f, -50.0f, 0.0f);

	models[0]->LoadRaw("models/skull.raw");
	models[0]->shader = *program;

	models[1] = new Model(m_rc);
	models[1]->scale = Vector3f(70.0f);
	models[1]->location = Vector3f(0.0f, -30.0f, 0.0f);
	models[1]->LoadRaw("models/teapot.raw");
	models[1]->shader = *program;

	ChangeModel(1);
}

void MainWindow::OnDisplay()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	m_rc->PushModelView();
		camera->ApplyTransform();
		program->UniformMatrix("View", 1, false, m_rc->GetModelView().data);
		skybox[current]->Draw();
		skybox[current]->GetTexture().Bind();
		models[current]->Draw();
	m_rc->PopModelView();
}

void MainWindow::OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	camera->SetPerspective(45.0f, 1.0f, 1e6f, Point3f(0.0f, 0.0f, -200.0f), w, h);
}

void MainWindow::OnKeyDown(UINT keyCode) {
	if (keyCode == 27) DestroyWindow(m_hwnd);
	else if (keyCode == 32) {
		ChangeModel(1 - current);
		Redraw();
	}
	else if (keyCode == 'Z') {
		camera->ResetView();
		Redraw();
	}
}

void MainWindow::OnMouseDown(MouseButton b, int x, int y)
{
	if (b == MouseButton::LBUTTON)
		camera->BeginRotate(x, y);
	else camera->BeginPan(x, y);
}

void MainWindow::OnMouseMove(UINT keysPressed, int x, int y)
{
	if (keysPressed & KM_LBUTTON) {
		camera->Rotate(x, y);
		Redraw();
	}
	else if (keysPressed & (KM_RBUTTON|KM_MBUTTON)) {
		camera->Pan(x, y);
		Redraw();
	}
}

void MainWindow::OnMouseWheel(short delta, UINT keysPressed, int x, int y)
{
	if (delta > 0) camera->Zoom(1.1f);
	else camera->Zoom(0.9f);
	Redraw();
}

void MainWindow::OnDestroy()
{
	delete camera;
	delete program;
	for (int i = 0; i < 2; i++) {
		delete skybox[i];
		delete models[i];
	}
	PostQuitMessage(0);
}