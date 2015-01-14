#include <GLEW/glew.h>
#include <SOIL.h>
#include <GLUT/glut.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <vector>
#include <iostream>

using namespace std;

#define ROLL_A  0.06
#define  PITCH_A 0.06
#define  YAW_A 0.06


//MP2 Code used for moving around the scene


// A sturct the defines a vector with 3 coordinates.
typedef struct{

	double x;
	double y;
	double z;

}vec3;

/*A function that takes in two Vectors a
and b and and returns their cross product*/
vec3 cross(vec3 a, vec3 b)
{
	//the return vector
	vec3 ret;
	ret.x=(a.y*b.z)-(a.z*b.y);
	ret.y= -((a.x*b.z)-(a.z*b.x));
	ret.z=(a.x*b.y)-(a.y*b.x);
    return ret;
}

/*
This function takes in a vec3 to represent a point,
a vec3 used as a vector, and a float for the angle

It the rotates that point by the angle around the vector
The logic was point here:
http://inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotation/ 
*/
vec3 rotate(vec3 point, vec3 axis, float angle)
{
    //the length of the vector squared
    double length = (axis.x*axis.x)+(axis.y*axis.y)+(axis.z*axis.z);
	//the return vector
	vec3 ret;
	
    ret.x = ((axis.x*((axis.x*point.x)+(axis.y*point.y)+(axis.z*point.z)))*(1-cos(angle))
		 + (length*point.x*cos(angle))
	     + (sqrt(length)*(-axis.z*point.y+axis.y*point.z)*sin(angle)))/length;
	
    ret.y = ((axis.y*((axis.x*point.x)+(axis.y*point.y)+(axis.z*point.z)))*(1-cos(angle))
		 + (length*point.y*cos(angle))
	     + (sqrt(length)*(axis.z*point.x-axis.x*point.z)*sin(angle)))/length;
    ret.z = ((axis.z*((axis.x*point.x)+(axis.y*point.y)+(axis.z*point.z)))*(1-cos(angle))
		 + (length*point.z*cos(angle))
	     + (sqrt(length)*(-axis.y*point.x+axis.x*point.y)*sin(angle)))/length;
    

    return ret;
}

/*
The class for the plane which holds the planes postion speed
directiona and normal vector, This class also has methods 
for Picth Roll and Yaw. Also there is a method to keep the plane
moving forward at every tick
*/
class Plane{
public:
	//default method for the plane
	Plane(){}
	
	//initialize method to set the initial position direction normal and speed
	void initialize(){
		//initial position set to 0,0,0
		position.x=1.5;
		position.y=1.5;
		position.z=3.5;
		
		//direction set to forward in x direction
		dir.x=-0.4;
		dir.y=0.0;
		dir.z=-1.0;
		
		//normal vector set to be up in z direction
		//this makes sure we are facing up if 
		//z was set to -1 we would start facing down
		normal.x=0.0;
		normal.y=1.0;
		normal.z=0.0;
    	
		//setting the speed of the plane
		//this can be changed via keys
	    speed = 0.004;
	
	}
	/*
		Method used for moving forward position
		multiplying speed times direction gives up the
		velecity for the x,y,z components of the plane
		we know that newposition = oldposition + velocity * time 
		since we have no accerelation.
		We assume time is just 1 unit.
	*/
	void forward() 
	{
		
	    position.x += 0.04 * dir.x;
	    position.y += 0.04 * dir.y;
	    position.z += 0.04 * dir.z;
		
	}
	void backword() 
	{
		
	    position.x -= 0.04 * dir.x;
	    position.y -= 0.04 * dir.y;
	    position.z -= 0.04 * dir.z;
	}
	
	/*
	Method used for rolling the plane
	Takes in an angle to roll by which can be negative
	
	to roll we dont cange the direction vector only the normal vector,
	normal is rotated about the direction vector
	*/
	void roll(double angle) 
	{
	    normal = rotate(normal, dir, angle);
	}
	/*
	Method used for the yaw of plane
	Takes in an angle to yaw by which can be negative
	
	to yaw we dont cange the normal vector only the direction vector,
	direction is rotated about the normal vector
	*/
	void yaw(double angle)
	{
		dir = rotate(dir, normal, angle);
	}
	/*
	Method used for pitching the plane
	Takes in an angle to pitch by which can be negative
	
	to pitch we need to change both direction and normal vector and we need to
	use the cross product.
	We first rotate direction around direction cross normal
	then we do normal around direction cross normal
	*/	
	void pitch(double angle) 
	{
	    dir = rotate(dir, cross(dir, normal), angle);
	    normal = rotate(normal, cross(dir, normal), angle);
	}
	
	//position of plane	
	vec3 position;
	//direction of plane
	vec3 dir;
	//normal vector of plane
	vec3 normal;
	//speed of plane
	double speed;


};
//A instance of the plane class
Plane plane;

//End of camera 

//vertex class used to define the object
//stroes the vertex coordinates the texture coordinates 
//and the per vertex normals
class vertex{
	
public:
	vertex(){}
	//creates a new vertex
	vertex(float xc,float yc,float zc,float uc,float vc){
		
		x=xc;y=yc;z=zc;u=uc;v=vc;
		vnx=0.0f;
		vny=0.0f;
		vnz=0.0f;
		
	}
	//updates the normals of a vertex
	void setnormals(float vnxc,float vnyc,float vnzc){
		vnx+=vnxc;
		vny+=vnyc;
		vnz+=vnzc;
	}
	//position
	float x; 
	float y;
	float z;
	//texture
	float u; 
	float v;
	//normal vector
	float vnx; float vny; float vnz;
};

//The vector of vertices in the model
vector<vertex> vertices;

//class used to define a face in the model
class face{
	
public:
	//creates a new face using three vertices and there locations
	face(vertex* i,vertex* j,vertex* k,int p1, int p2,int p3){
		
		v1=i; v2=j; v3=k;
		//calcutes the normal vector of a face
		float ux = vertices[p2-1].x - vertices[p1-1].x;
		float uy = vertices[p2-1].y - vertices[p1-1].y;
		float uz = vertices[p2-1].z - vertices[p1-1].z;
				  
		float vx = vertices[p3-1].x - vertices[p1-1].x;
		float vy = vertices[p3-1].y - vertices[p1-1].y;
		float vz = vertices[p3-1].z - vertices[p1-1].z;
		
		fnx =(uy * vz)-(uz * vy);
		fny =-((uz * vx)-(ux * vz));
		fnz=(ux * vy)-(uy * vx);
	}
	
	vertex* v1;
	vertex* v2;
	vertex* v3;
	float fnx;
	float fny;
	float fnz;
};

//vector to store all of the faces
vector<face> faces;

//reads a shaders from a file
char * readShader(char * shader, int * len) {
	FILE * file = fopen(shader, "r");
	fseek(file, 0L, SEEK_END);
	*len = ftell(file);
	char* buffer = (char *) malloc(*len);
	fseek(file, 0L, SEEK_SET);
	fread(buffer, *len, 1, file);
	fclose(file);
	return buffer;
}

//loads an obj from a file
void loadobj() {

	//open the file
	FILE * file = fopen("teapot.txt", "r");

	/* Initialize the lists */
	while (!feof(file)) {
		
		//we load the first char since it determines our action
		char temp;
		fscanf(file, "%c ", &temp);
		
		//if the char equals v add a vertex
		if(temp=='v'){
			 	//load the x, y, z coordinates 
				float x, y, z;
				fscanf(file, "%f %f %f\n", &x, &y, &z);
				//calulate u and v coordinates using a cylinder
				float theta = atan2(z,x);
				vertex v(x,y,z,(theta +  3.141592654) / (2* 3.141592654),(y / 2.0));
				//add the vertex
				vertices.push_back(v);
		}
		//if the char equals f add a face
		else if(temp=='f'){
				//load the positions of the vertices
				int p1,p2,p3;
				fscanf(file, "%d %d %d\n", &p1, &p2, &p3);
				//create the face
				face f(&vertices[p1-1],&vertices[p2-1],&vertices[p3-1],p1,p2,p3);
				//push the face back
				faces.push_back(f);
		}else{
			//if the char is anything else just skip
			while(fgetc(file) != '\n'){
			}
		}
			
	
	}
	
	//loop through the faces and set the normals
	for (int i = 0; i < faces.size(); i++) {
		
		faces[i].v1->setnormals(faces[i].fnx,faces[i].fny,faces[i].fnz);
		faces[i].v2->setnormals(faces[i].fnx,faces[i].fny,faces[i].fnz);
		faces[i].v3->setnormals(faces[i].fnx,faces[i].fny,faces[i].fnz);	

	}
	//close the file
	fclose(file);
}

//initialize everything
void init() {
	
	//load the object
	loadobj();
	//initialize glew
	glewInit();
	
	//set the clear color to black
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	//load the textures
	GLuint texture,environment;
	glGenTextures(1, &texture);
	glGenTextures(1, &environment);
	
	int img_width, img_height;
	
	//use soil to load the image
	unsigned char* textimg = SOIL_load_image("bricks2.jpg", &img_width, &img_height, NULL, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, textimg);
	
	//use soil to load the image
	unsigned char* evnimg = SOIL_load_image("env.jpg", &img_width, &img_height, NULL, 0);
 
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, environment); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glTexImage2D(GL_TEXTURE_2D, 0, 3, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, evnimg);


	//load and set up of the vertex shader
	//tutorial https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php
		
	int vlen, flen;
	GLuint vertexShaderObject, fragmentShaderObject, programObject;
	
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexsource = readShader("vertexshader.txt", &vlen);
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentsource = readShader("fragmentshader.txt", &flen); 
	
	const long vsize = (long)vlen;
	const long fsize = (long)flen;
	
	glShaderSource(vertexShaderObject, 1, &vertexsource, &vsize);
	glShaderSource(fragmentShaderObject, 1, &fragmentsource, &fsize);
	
	glCompileShader(vertexShaderObject); 
	glCompileShader(fragmentShaderObject);
	programObject = glCreateProgram();
	glAttachShader(programObject, vertexShaderObject);
	glAttachShader(programObject, fragmentShaderObject);
	glLinkProgram(programObject); 


	glUseProgram(programObject);

	GLint texture0 = glGetUniformLocation(programObject, "colorMap");
	GLint texture1 = glGetUniformLocation(programObject, "envMap");
	glUniform1i(texture0, 0);
	glUniform1i(texture1, 1);
	
	//initialize camera
	plane.initialize();
	
	//initialize lighing
	GLfloat spec[] = {1.0,1.0,1.0};
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, spec);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, spec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	//smooth everything
	glShadeModel(GL_SMOOTH);
}

void display(void) {
	//light position
	GLfloat lpos[] = {3.0 * -.658, 3.0 * -389, 2.0};
	
	glLoadIdentity ();
	
	//We are using the MP2 code to fly around the scene
	//This lets us get any position we want
	//here the glulooAt is called so the we are seeing what the plane is seeing
	
    gluLookAt(plane.position.x, plane.position.y, plane.position.z, // first the position of the plane is passed in
	plane.position.x+plane.dir.x, 									//then the postition where the plane will be is passed in 
	plane.position.y+plane.dir.y, 									//this is what the plane is looking at
	plane.position.z+plane.dir.z, 
	plane.normal.x, plane.normal.y, plane.normal.z);				//then the normal vector is passed in which is the 				

	//clear
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	
	/*Draw the teapot by looping through all the faces and then setting all the per vertex normals 
	the texture coordinates and finally the vertex positions of each vertex.
	*/
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < faces.size(); i++) {
		
		glMultiTexCoord2f(GL_TEXTURE0_ARB,faces[i].v1->u,faces[i].v1->v); 	
		glNormal3f(faces[i].v1->vnx, faces[i].v1->vny, faces[i].v1->vnz);
		glTexCoord2f(faces[i].v1->u,faces[i].v1->v);
		glVertex3f(faces[i].v1->x, faces[i].v1->y, faces[i].v1->z);
		
		glMultiTexCoord2f (GL_TEXTURE0_ARB,faces[i].v2->u,faces[i].v2->v); 	
		glNormal3f(faces[i].v2->vnx, faces[i].v2->vny, faces[i].v2->vnz);
		glTexCoord2f(faces[i].v2->u,faces[i].v2->v);
		glVertex3f(faces[i].v2->x, faces[i].v2->y, faces[i].v2->z);
		
		glMultiTexCoord2f (GL_TEXTURE0_ARB,faces[i].v3->u,faces[i].v3->v); 	
		glNormal3f(faces[i].v3->vnx, faces[i].v3->vny, faces[i].v3->vnz);
		glTexCoord2f(faces[i].v3->u,faces[i].v3->v);
		glVertex3f(faces[i].v3->x, faces[i].v3->y, faces[i].v3->z);
	}
	glEnd();
	
	//swap buffers so no flickering
	glutSwapBuffers();
	glFlush ();
	
	glutPostRedisplay();
}

//reshape function
void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0,(float)w/h,0.01,10.0);
	glMatrixMode (GL_MODELVIEW);
}

//THe keyboardfunction to get keyinput
void keys(unsigned char key, int x, int y) // Keyboard callback function
{
    switch (key) {
		//a key: yaw left	
		case 'a':
			//positive angle passed in to yaw left
			plane.yaw(YAW_A);
			break;
		//d key: yaw right	
		case 'd':
			//negative angle passed in to yaw right
			plane.yaw(-YAW_A);
			break;
		//w key:  speedup
		case 'w':
			plane.forward();
			break;
		//w key:  slowdown / brake
		case 's':
			plane.backword();
			break;		
		//esc key:  Quit program
 		case 27:
 			exit(0);
 			break;
    }
}
//THe special keyboardfunction to get arrow key input
void arrows(int key, int x, int y)
{
    switch (key) {
		//Left key: roll left
        case GLUT_KEY_LEFT: 
			//negative angle rolls left
            plane.roll(-ROLL_A);
            break;
		//right key: roll right
        case GLUT_KEY_RIGHT: 
			//positive angle roll right
            plane.roll(ROLL_A);
            break;
		//down key: pitch down
        case GLUT_KEY_DOWN: 
			//negative angle pitches down
              plane.pitch(-PITCH_A);    
            break;
		//up key: pitch up
        case GLUT_KEY_UP: 
			//positive angle pitches up
            plane.pitch(PITCH_A);        
            break;
    }
}


int main(int argc, char** argv) {
	//initialize the window	
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//window size
    glutInitWindowSize (640, 480);
	//window position
    glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	// call the init method
	init ();
	//set up the display function
    glutDisplayFunc(display);
	//set up the reshape function
	glutReshapeFunc(reshape);
	//set up the keyboard function
    glutKeyboardFunc(keys); 
	//set up the arrowkeys function
    glutSpecialFunc(arrows);

	//start main loop
	glutMainLoop();
	
	return 0;
}
