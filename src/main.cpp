#include "MainWindow.h"

#include <GL/glut.h>
#include <glu.h>
#include <iostream>

MainWindow* windowPtr;

void doRendering()
{
	windowPtr->display();
}

void doReshape(int w, int h)
{
	windowPtr->reshape(w, h);
}

void handleKeyboard(unsigned char key, int x, int y)
{
	windowPtr->handleKeyboard(key, x, y);
}

void handleKeyboardUp(unsigned char key, int x, int y)
{
    windowPtr->handleKeyboardUp(key, x, y);
}

void idle()
{
	windowPtr->idle();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);

	MainWindow window;
	windowPtr = &window;
	glutDisplayFunc(doRendering);
	glutReshapeFunc(doReshape);
	glutKeyboardFunc(handleKeyboard);
    glutKeyboardUpFunc(handleKeyboardUp);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
