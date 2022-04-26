#include "../Utilities/models.cpp" 
#include <string>
#include <fstream>
#include <stdlib.h>
#include <list>
#include <map>
#include "tinyxml2/tinyxml2.h"
#include "../Utilities/camera.cpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

using namespace tinyxml2;
using namespace std;


int startX, startY, tracking = 0;
float alpha = 0, beta = 0;
float r = 0;

int timebase = 0;
float frame = 0;
float fps = 0;

camera cam = camera();
group principal_g = group();

GLenum mode = GL_LINE;
bool referential = false;
bool pointer = false;

GLuint vertices, verticeCount;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(cam.fov,
		ratio, //aspect
		cam.near,
		cam.far);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// set the camera
	glLoadIdentity();
	gluLookAt(cam.px,cam.py,cam.pz,
			  cam.lx,cam.ly,cam.lz,
		      cam.ux,cam.uy,cam.uz );

	

	if (referential) {
		//REFERENCIAL
		glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnd();

	}
	if (pointer) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPushMatrix();
		glColor3f(5, 0, 0);
		glTranslatef(cam.lx, cam.ly, cam.lz);
		glutSolidSphere(0.10, 5, 5);
		glColor3f(1, 1, 1);
		glPopMatrix();
	}

	
	glPolygonMode(GL_FRONT_AND_BACK, mode);

	

	principal_g.render();

	frame++;
	float time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;
	}

	char str[80];
	sprintf_s(str, "CG PROJECT [fps: %f]", fps);

	glutSetWindowTitle(str);
	
	// End of frame
	glutSwapBuffers();
}

void remake_lookAt() {
	r = sqrt(pow((cam.px - cam.lx), 2) + pow((cam.py - cam.ly), 2) + pow((cam.pz - cam.lz), 2));
	beta = (180 * asin((cam.py - cam.ly) / r)) / 3.14;
	float aux = (cam.px - cam.lx) / (r * cos((beta * 3.14) / 180));
	alpha = (180 * asin(aux)) / 3.14;
}

void keyboardfunc(unsigned char key, int x, int y) {
	switch (key) {
	case 'm':
		if (mode == GL_LINE) mode = GL_FILL;
		else mode = GL_LINE;
		glutPostRedisplay();
		break;
	case 'r':
		if (referential) referential = false;
		else referential = true;
		glutPostRedisplay();
		break;
	case 'p':
		if (pointer) pointer = false;
		else pointer = true;
		glutPostRedisplay();
		break;
	case 'q':
		cam.lz += 1;
		remake_lookAt();
		glutPostRedisplay();
		break;
	case 'e':
		cam.lz -= 1;
		remake_lookAt();
		glutPostRedisplay();
		break;
	case 'w':
		cam.ly += 1;
		remake_lookAt();
		glutPostRedisplay();
		break;
	case 's':
		cam.ly -= 1;
		remake_lookAt();
		glutPostRedisplay();
		break;
	case 'a':
		cam.lx += 1;
		remake_lookAt();
		glutPostRedisplay();
		break;
	case 'd':
		cam.lx -= 1;
		remake_lookAt();
		glutPostRedisplay();
		break;
	default:
		break;
	}
}




void processMouseButtons(int button, int state, int xx, int yy) {

	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {

			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
	glutPostRedisplay();
}


void processMouseMotion(int xx, int yy) {

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	cam.px = cam.lx + rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	cam.pz = cam.lz + rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	cam.py = cam.ly + rAux * sin(betaAux * 3.14 / 180.0);
	//printf("alpha->%f\n", alphaAux);
	//printf("beta->%f\n", betaAux);
	//printf("raio->%f\n", rAux);
	//cam.print_camera();
	glutPostRedisplay();
}


int glut_main(int argc, char** argv) {
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG_PROJECT");

	timebase = glutGet(GLUT_ELAPSED_TIME);

	// Required callback registry 
	glutDisplayFunc(renderScene);
	//glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);


	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keyboardfunc);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	// init GLEW
#ifndef __APPLE__
	glewInit();
#endif

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnableClientState(GL_VERTEX_ARRAY);

	printf("Preparing data...\n");
	//mods.prepare_data();

	principal_g.prepare_data();
	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}

models xml_models(XMLElement* models_e) {
	models ms = models();
	XMLElement* model_e = models_e->FirstChildElement("model");
	while (model_e) {
		model m = model();
		const char* filename = model_e->Attribute("file");
		ifstream file(filename);
		if (file.is_open()) { //abre o ficheiro
			printf("Loading file model %s ...\n", filename);
			string line;
			while (getline(file, line)) { //l� linha a linha
				if (line[0] == 'i') {
					int idx = stoi(line.substr(1, line.length()));
					m.add_index(idx); //cada indice corresponde a 3 coords.
				}
				else {
					point p = point(line);
					m.add_point(p);
				}
			}
			file.close();
		}
		else {
			printf("ERROR! File model %s does not exist! (IGNORED)", filename);
		}
		printf("File loaded model %s.\n", filename);
		model_e = model_e->NextSiblingElement("model");
		ms.add_model(m);
	}
	return ms;
}

transformations xml_transform(XMLElement* transformations_e) {
	transformations trs;
	XMLElement* transformation_e = transformations_e->FirstChildElement();
	while (transformation_e != NULL) {
		if (strcmp(transformation_e->Name(), "translate") == 0) {
			float x, y, z;
			bool catmullroll = false;
			vector<point> ps;
			float time;
			bool align;
			if (transformation_e->Attribute("time") != NULL && transformation_e->Attribute("align") != NULL) {
				catmullroll = true;
				transformation_e->QueryAttribute("time", &align);
				if (transformation_e->Attribute("align").strcmp("True") == 0) { //corrigir
					align = true;
				}
				else align = false;
				//ler os pontos todos
			}
			else {
				transformation_e->QueryAttribute("x", &x);
				transformation_e->QueryAttribute("y", &y);
				transformation_e->QueryAttribute("z", &z);
			}
			printf("<translate x=%0.f y=%0.f z=%0.f />\n", x, y, z); //DEBUG
			if(catmullroll){
			
			
			}
			else {
				point p = point(x, y, z);
				trs.add_transformation(new translation(p));
			}
		}
		else if (strcmp(transformation_e->Name(), "rotate") == 0) {
			float angle = 0, time = 0, x, y, z;
			if(transformation_e->Attribute("angle") != NULL)
				transformation_e->QueryAttribute("angle", &angle);
			else 
				transformation_e->QueryAttribute("time", &time);
			transformation_e->QueryAttribute("x", &x);
			transformation_e->QueryAttribute("y", &y);
			transformation_e->QueryAttribute("z", &z);
			printf("<rotate angle=%0.f x=%0.f y=%0.f z=%0.f />\n", angle, x, y, z); //DEBUG
			point p = point(x, y, z);
			if (time != 0) {
				trs.add_transformation(new rotation(angle, p));
			}
			else trs.add_transformation(new rotation(p, time));
		}
		else if (strcmp(transformation_e->Name(), "scale") == 0) {
			float x, y, z;
			transformation_e->QueryAttribute("x", &x);
			transformation_e->QueryAttribute("y", &y);
			transformation_e->QueryAttribute("z", &z);
			printf("<scale x=%0.f y=%0.f z=%0.f />\n", x, y, z); //DEBUG
			point p = point(x, y, z);
			trs.add_transformation(new scaling(p));
		}
		transformation_e = transformation_e->NextSiblingElement();
	}
	return trs;
}

group xml_group(XMLElement* group_e) {
	group g = group();
	XMLElement* transform_e = group_e->FirstChildElement("transform");
	if (transform_e) {
		g.add_transformations(xml_transform(transform_e));
	}
	XMLElement* models_e = group_e->FirstChildElement("models");
	if (models_e) {
		g.add_models(xml_models(models_e));
	}
	XMLElement* othergroup_e = group_e->FirstChildElement("group");
	while (othergroup_e) {
		g.add_group(xml_group(othergroup_e));
		othergroup_e = othergroup_e->NextSiblingElement("group");
	}
	return g;
}

void xml_camera(XMLElement* camera_e) {
	XMLElement* position_e = camera_e->FirstChildElement("position");
	if (position_e) {
		position_e->QueryAttribute("x", &cam.px);
		position_e->QueryAttribute("y", &cam.py);
		position_e->QueryAttribute("z", &cam.pz);
		printf("<position x=%0.f y=%0.f z=%0.f />\n", cam.px, cam.py, cam.pz);
	}
	else {
		printf("WARNING: \"position\" (element of \"camera\") not detected. Using default values...");
	}
	XMLElement* lookAt_e = camera_e->FirstChildElement("lookAt");
	if (lookAt_e) {
		lookAt_e->QueryAttribute("x", &cam.lx);
		lookAt_e->QueryAttribute("y", &cam.ly);
		lookAt_e->QueryAttribute("z", &cam.lz);
		printf("<lookAt x=%0.f y=%0.f z=%0.f />\n", cam.lx, cam.ly, cam.lz);
		remake_lookAt();
	}
	else {
		printf("WARNING: \"lookAt\" (element of \"camera\") not detected. Using default values...");
	}
	XMLElement* up_e = camera_e->FirstChildElement("up");
	if (up_e) {
		up_e->QueryAttribute("x", &cam.ux);
		up_e->QueryAttribute("y", &cam.uy);
		up_e->QueryAttribute("z", &cam.uz);
		printf("<up x=%0.f y=%0.f z=%0.f />\n", cam.ux, cam.uy, cam.uz);
	}
	else {
		printf("WARNING: \"up\" (element of \"camera\") not detected. Using default values...");
	}
	XMLElement* projection_e = camera_e->FirstChildElement("projection");
	if (projection_e) {
		projection_e->QueryAttribute("fov", &cam.fov);
		projection_e->QueryAttribute("near", &cam.near);
		projection_e->QueryAttribute("far", &cam.far);
		printf("<projection fov=%0.f near=%0.f fav=%0.f />\n", cam.fov, cam.near, cam.far);
	}
	else {
		printf("WARNING: \"projection\" (element of \"camera\") not detected. Using default values...");
	}
	printf("alpha->%f\n", alpha);
	printf("beta->%f\n", beta);
	printf("raio->%f\n", r);
	cam.print_camera();
}

int xml_world(XMLElement* world_e) {
	XMLElement* camera_e = world_e->FirstChildElement("camera");
	if (camera_e) {
		xml_camera(camera_e);
	}
	else {
		printf("WARNING: \"camera\" not detected. Using default values...");
	}
	XMLElement* group_e = world_e->FirstChildElement("group");
	if (group_e) {
		principal_g = xml_group(group_e);
	}
	else {
		printf("ERROR: \"group\" not detected.");
		return -1;
	}
	return 1;
}


int main(int argc, char** argv) {
	if (argc == 2) {
		if (argv[1]){
			printf("Loading %s\n", argv[1]);
		}

		XMLDocument doc;
		XMLError err = doc.LoadFile(argv[1]);
			
		if (err) {
			fprintf(stderr, "TINYXML FAILURE! Error code: %d\n", err);
			return err;
		}
		
		//world engloba todo o xml
		XMLElement* world_e = doc.FirstChildElement("world");
		if (!world_e) {
			printf("XML needs a field called \"world\"");
			return -1;
		}
		else {
			int err = xml_world(world_e);
			if (err == -1) return -1;
		}
	}
	else printf("Invalid arguments!");
	
	
	glut_main(argc, argv);

	return 0;
}

