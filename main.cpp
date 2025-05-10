#include <math.h>
#include <time.h>
#include <thread>
#include <chrono>
#include <dirent.h>

#include "objL/model.h"

using namespace std;

#define WIDTH 600
#define HEIGHT 600

#define INTERVAL 1

int POS_X, POS_Y;

string model_name = "";

GLfloat light_pos[] = {-10.0f, 10.0f, 100.00f, 1.0f};

float pos_x, pos_y, pos_z;
float angle_x = 30.0f, angle_y = 0.0f;

int x_old = 0, y_old = 0;
int current_scroll = 5;
float zoom_per_scroll;

bool is_holding_mouse = false;
bool is_updated = false;


float x_position = -5.0;

int current_vertex = 0;

bool playVertex = true;
bool playEdge = false;
bool playFace = false;
bool reverseVertex = false;
bool reverseEdge = false;
bool reverseFace = false;
bool NowChangeModel = false;

int current_model_index = 0;
int total_models = 0;

int current_face = 0; 
int current_removeface = 0; 
int current_removeedge = 0; 
int current_edge = 0; 
int current_removevertex = 0;

float last_time = 0.0f;
float rotation_speed = 0.4f;

bool keepPlayVertex = true;
bool keepPlayEdge = false;
bool keepPlayFace = false;
bool keepReverseFace = false;
bool keepReverseEdge = false;
bool keepReverseVertex = false;

// controls mesh appeareance speed
int vertex_step = 3; // Number of vertex will appear after previous vertecies that appeared per frame
int face_step = 3; // Number of edge and face will appear after previous edges and faces that appeared per frame 

vector<string> model_list; // List of model names

Model model; // Model object for currently loaded model

// Function prototypes
void init();
vector<string> getModels();
void changeModel();
void resetAnimation();

void init() {

    // Set up the environment
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    // glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // For setting background color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0, 1.0, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // get models deom 'models/' directory
    model_list = getModels();

    // Load the first model
    changeModel();

    // Set the initial position of the model
    pos_x = model.pos_x;
    pos_y = model.pos_y;
    pos_z = model.pos_z - 1.0f;

    //
    zoom_per_scroll = -model.pos_z / 10.0f;
}

/* Reset the animation state
// This function resets the animation parameters to their initial state
// It sets the vertex count, current face, edge, and remove face/edge to zero
// It also resets the play and reverse flags for vertex, edge, and face animations
// The function also sets the NowChangeModel flag to false
// and initializes the keep play and reverse flags for vertex, edge, and face animations
// The function is used to reset the animation state when a new model is loaded
// It ensures that the animation starts from the beginning for the new model
// The function is called when the user changes the model or when the animation needs to be reset
// It is important to call this function after loading a new model
// to ensure that the animation parameters are set correctly for the new model 
*/
void resetAnimation()
{
    current_vertex = 0;
    current_edge = 0; 
    current_face = 0; 
    current_removeface = 0; 
    current_removeedge = 0; 
    current_removevertex = 0;

    playVertex = true;
    playEdge = false;
    playFace = false;
    reverseVertex = false;
    reverseEdge = false;
    reverseFace = false;
    NowChangeModel = false;

    keepPlayVertex = true;
    keepPlayEdge = false;
    keepPlayFace = false;
    keepReverseFace = false;
    keepReverseEdge = false;
    keepReverseVertex = false;
}

/* Changes the model being displayed
// This function changes the model being displayed in the OpenGL window
// It unloads the current model and loads a new one from the list of models
// The new model is selected based on the current model index
// The function also updates the vertex and face step values based on the new model
// It resets the animation state for the new model
*/
void changeModel()
{
    string new_model_name = "models/"+model_list[current_model_index];
    
    model.unload();
    model_name = new_model_name;
    model.load(model_name.c_str());

    vertex_step = model.vertices.size() / 5000;
    if(vertex_step < 1) vertex_step = 1; // In case Vertices are less than 5000

    face_step = model.faces.size() / 1000;
    if(face_step < 1) face_step = 1; // In case Faces are less than 1000

    resetAnimation();

    NowChangeModel = false;

    current_model_index++;
    if (current_model_index >= total_models) {
        current_model_index = 0;
    }
}

/* Get the list of model files from the 'models/' directory
// This function retrieves the list of model files from the 'models/' directory
// It uses the opendir and readdir functions to read the directory contents
// It filters the files to include only those with the '.obj' extension
// The function returns a vector of strings containing the names of the model files
// It also prints the names of the found model files to the console
// The function is used to populate the model_list vector with the available models
// It is called during the initialization of the program to load the available models
// The function is important for allowing the user to select different models to display
// It ensures that only valid model files are included in the list
// The function is also used to count the total number of models available
// It sets the total_models variable to the number of found model files
// The function is called only once during the initialization of the program
// It is not called again during the program execution
// It allows the user to switch between different models easily
*/
vector<string> getModels()
{
    vector<string> model_files;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("models/")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string filename(ent->d_name);
            if (filename.find(".obj") != string::npos) {
                cout << "Found model: " << filename << endl;
                model_files.push_back(filename);
            }
        }
        total_models = model_files.size();
        closedir(dir);
    } else {
        perror("Could not open models directory");
    }
    return model_files;
}

void vertexShow()
{
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < current_vertex && i < model.sorted_vertices.size(); ++i) {
        if (i < model.normals.size()) {
            glNormal3f(model.normals[i][0], model.normals[i][1], model.normals[i][2]);
        }
        if (i < model.texcoords.size()) {
            glTexCoord2f(model.texcoords[i][0], model.texcoords[i][1]);
        }
        glVertex3f(model.sorted_vertices[i][0], model.sorted_vertices[i][1], model.sorted_vertices[i][2]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}


void edgeShow()
{
    for (size_t f = 0; f < current_edge && f < model.faces.size(); ++f) {
        const Face& face = model.faces[f];
        if (face.edge <= 0 || face.vertices == NULL) continue; // Skip material changes or invalid faces
    
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < face.edge; ++i) {
            int vertex_index = face.vertices[i];
            if (vertex_index >= 0 && vertex_index < model.vertices.size()) {
                
                // If the face has a normal, set it
                if (face.normal >= 0 && face.normal < model.normals.size()) {
                    const auto& normal = model.normals[face.normal];
                    glNormal3f(normal[0], normal[1], normal[2]);
                }
    
                // Set the vertex
                glVertex3f(
                    model.vertices[vertex_index][0],
                    model.vertices[vertex_index][1],
                    model.vertices[vertex_index][2]
                );
            }
        }
        glEnd();
    }
}

void faceShow()
{
    for (size_t f = 0; f < current_face; ++f) {
        const Face& face = model.faces[f];

        // Skip invalid faces
        if (face.edge <= 0 || face.vertices == NULL) continue;

        // You can use GL_TRIANGLES or GL_POLYGON (if guaranteed convex)
        glBegin(GL_POLYGON); // Or GL_TRIANGLES if the face is triangular

        // Loop through each vertex of the face
        for (int i = 0; i < face.edge; ++i) {
            int vertex_index = face.vertices[i];
            
            // Make sure the vertex index is valid
            if (vertex_index >= 0 && vertex_index < model.vertices.size()) {
                
                // Check and set the normal if it exists
                if (face.normal >= 0 && face.normal < model.normals.size()) {
                    const auto& normal = model.normals[face.normal];
                    glNormal3f(normal[0], normal[1], normal[2]);
                }
                
                // Set the vertex position
                const auto& vertex = model.vertices[vertex_index];
                glVertex3f(vertex[0], vertex[1], vertex[2]);
            }
        }

        glEnd();
    }
}

void faceRemove()
{
    // Limit the number of faces to draw, based on current_removeface
    size_t limit = model.faces.size() > current_removeface ? model.faces.size() - current_removeface : 0;

    // Loop through the faces that should still be visible
    for (size_t f = 0; f < limit; ++f) {
        const Face& face = model.faces[f];

        if (face.edge <= 0 || face.vertices == NULL) continue;

        glBegin(GL_POLYGON);

        for (int i = 0; i < face.edge; ++i) {
            int vertex_index = face.vertices[i];

            if (vertex_index >= 0 && vertex_index < model.vertices.size()) {
                if (face.normal >= 0 && face.normal < model.normals.size()) {
                    const auto& normal = model.normals[face.normal];
                    glNormal3f(normal[0], normal[1], normal[2]);
                }

                const auto& vertex = model.vertices[vertex_index];
                glVertex3f(vertex[0], vertex[1], vertex[2]);
            }
        }

        glEnd();
    }
}
void edgeRemove()
{
    size_t limit = model.faces.size() > current_removeedge ? model.faces.size() - current_removeedge : 0;

    for (size_t f = 0; f < limit; ++f) {
        const Face& face = model.faces[f];

        if (face.edge <= 0 || face.vertices == NULL) continue;  // Skip invalid faces

        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < face.edge; ++i) {
            int vertex_index = face.vertices[i];
            if (vertex_index >= 0 && vertex_index < model.vertices.size()) {
                
                // If the face has a normal, set it
                if (face.normal >= 0 && face.normal < model.normals.size()) {
                    const auto& normal = model.normals[face.normal];
                    glNormal3f(normal[0], normal[1], normal[2]);
                }
    
                // Set the vertex
                glVertex3f(
                    model.vertices[vertex_index][0],
                    model.vertices[vertex_index][1],
                    model.vertices[vertex_index][2]
                );
            }
        }
        glEnd();
    }
}

void vertexRemove()
{
    size_t limit = model.sorted_vertices.size() > current_removevertex ? model.sorted_vertices.size() - current_removevertex : 0;

    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < limit; ++i) {
        if (i < model.normals.size()) {
            glNormal3f(model.normals[i][0], model.normals[i][1], model.normals[i][2]);
        }
        if (i < model.texcoords.size()) {
            glTexCoord2f(model.texcoords[i][0], model.texcoords[i][1]);
        }
        glVertex3f(model.sorted_vertices[i][0], model.sorted_vertices[i][1], model.sorted_vertices[i][2]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void showAxis()
{
    glLineWidth(2.0);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, -1.0);
    glVertex3d(3.0, 0.0, -1.0);
    glEnd();

    glLineWidth(2.0);
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, -1.0);
    glVertex3d(0.0, 3.0, -1.0);
    glEnd();

    glLineWidth(2.0);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, -1.0);
    glVertex3d(0.0, 0.0, 3.0);
    glEnd();
}

void showMovingBox()
{
    glBegin(GL_POLYGON);
    glVertex3d(x_position, 1.0, -1.0);
    glVertex3d(x_position, -1.0, -1.0);
    glVertex3d(x_position+2.0, -1.0, -1.0);
    glVertex3d(x_position+2.0, 1.0, -1.0);
    glEnd();
}

float camera_distance = 200.0f + (current_scroll - 5) * zoom_per_scroll;  // Distance from origin

float camera_x = sin(angle_y * M_PI / 180.0f) * camera_distance;
float camera_z = cos(angle_y * M_PI / 180.0f) * camera_distance;
float camera_y = 100.0f;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camera_x, camera_y, camera_z,    // Camera position
              0.0f, 0.0f, 0.0f,                // Look at the origin
              0.0f, 1.0f, 0.0f);               // Up is +Y

    glDisable(GL_LIGHTING); // Just so the box and axis are not affected by lighting
    // showAxis();
    // showMovingBox();

    glEnable(GL_LIGHTING); // Re-enable lighting for the model
    glColor3f(1.0f, 1.0f, 1.0f); // Set the color to white
    
    if (playVertex)vertexShow();

    if(playEdge)edgeShow();
    
    if(playFace)faceShow();
    
    if (reverseFace)faceRemove();
    
    if(reverseEdge)edgeRemove();

    if (reverseVertex)vertexRemove();

    if (NowChangeModel)changeModel();
    
    glutSwapBuffers();
}



void timer(int value)
{
    x_position += 0.1;
    if (x_position > 5.0)
        x_position = -5.0;

    float current_time = glutGet(GLUT_ELAPSED_TIME) / 10.0f;
    float delta_time = current_time - last_time;
    last_time = current_time;

    angle_y += rotation_speed * delta_time;
    
    if(angle_y > 360.0f)
        angle_y = 0.0f;
    camera_x = sin(angle_y * M_PI / 180.0f) * camera_distance;
    camera_z = cos(angle_y * M_PI / 180.0f) * camera_distance;

    if (keepPlayVertex)
    {
        // printf("playVertex happening\n");
        if (current_vertex < model.sorted_vertices.size())
        {
            current_vertex += vertex_step; 
            if (current_vertex > model.sorted_vertices.size())
                current_vertex = model.sorted_vertices.size();
        }
        else
        {
            playVertex = true;
            keepPlayVertex = false;
            keepPlayEdge = true;
        }
    }
    else if (keepPlayEdge)
    {
        playEdge = true;
        // printf("playEdge happening\n");
        if (current_edge < model.faces.size())
        {
            current_edge+= face_step; 
            if (current_edge > model.faces.size())
                current_edge = model.faces.size(); 
        }else
        {
            playEdge = true;
            keepPlayEdge = false;
            keepPlayFace = true;
        }
    }else if(keepPlayFace)
    {
        playFace = true;
        // printf("playFace happening\n");
        if (current_face < model.faces.size())
        {
            current_face+= face_step; 
            if (current_face > model.faces.size())
                current_face = model.faces.size();
        }
        else
        {
            playFace = true;
            keepPlayFace = false;
            keepReverseFace = true;
        }
    }else if (keepReverseFace)
    {
        playFace = false;
        reverseFace = true;
        // printf("reverseFace happening\n");
        
        if (current_removeface < model.faces.size()) {
                current_removeface+= face_step; 
        }
        else
        {
            playFace = false;
            keepReverseFace = false;
            keepReverseEdge = true;
        }
    }else if (keepReverseEdge)
    {
        playEdge = false;
        reverseEdge = true;
        // printf("reverseEdge happening\n");
        if (current_removeedge < model.faces.size()) {
            current_removeedge+= face_step; 
        }
        else
        {
            keepReverseEdge = false; 
            keepReverseVertex = true;
        }
    }
    else if (keepReverseVertex)
    {
        playVertex = false;
        reverseVertex = true;
        // printf("reverseVertex happening\n");
        if (current_removevertex < model.sorted_vertices.size()) {
            current_removevertex+= vertex_step; 
        }
        else
        {
            keepReverseVertex = false;  
            NowChangeModel = true;  
        }
    }

    glutPostRedisplay();
    glutTimerFunc(INTERVAL, timer, 0); // Frames are rendering 1ms apart
}

// Mouse callback function
// This function handles mouse events
// It updates the camera position and zoom level based on mouse input
// It also handles mouse button events for zooming in and out
// The function is called whenever a mouse event occurs
// It is used to control the camera position and zoom level
// The function is important for allowing the user to interact with the 3D scene
// It allows the user to zoom in and out using the mouse wheel
// The function is called whenever the mouse is moved or clicked
void mouse(int button, int state, int x, int y) {
    is_updated = true;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            x_old = x;
            y_old = y;
            is_holding_mouse = true;
        } else
            is_holding_mouse = false;
    } else if (state == GLUT_UP) {
        switch (button) {
        case 3: // Scroll up
            if (current_scroll > 0) {
                current_scroll--;
                pos_z += zoom_per_scroll;
                camera_distance = 200.0f + (current_scroll - 5) * zoom_per_scroll; // <--- Add this
            }
            break;
        case 4: // Scroll down
            if (current_scroll < 15) {
                current_scroll++;
                pos_z -= zoom_per_scroll;
                camera_distance = 200.0f + (current_scroll - 5) * zoom_per_scroll; // <--- Add this
            }
            break;
        }
    }
}

// Mouse motion callback function
// This function handles mouse motion events
// It updates the camera rotation based on mouse movement
// The function is called whenever the mouse is moved while a button is pressed
// It is used to control the camera rotation
// The function is important for allowing the user to interact with the 3D scene
// It allows the user to rotate the camera using the mouse
// The function is called whenever the mouse is moved while holding down a button
// It updates the camera rotation based on the mouse movement
void motion(int x, int y) {
    if (is_holding_mouse) {
        is_updated = true;

        // Update Y rotation based on horizontal mouse movement
        angle_y += (x - x_old);  // X-axis movement controls Y-axis rotation
        x_old = x;

        // Make sure the angle is always between 0 and 360 degrees
        if (angle_y > 360.0f)
            angle_y -= 360.0f;
        else if (angle_y < 0.0f)
            angle_y += 360.0f;

        // Lock X rotation to 0 (so it doesn't change)
        angle_x = 0.0f;  // Keep the X rotation fixed at 0 degrees
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    glutSetOption(GLUT_MULTISAMPLE, 8);

    POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
    POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
    glutInitWindowPosition(POS_X, POS_Y);
    glutInitWindowSize(WIDTH, HEIGHT);

    glutCreateWindow("3D OBJ Progressive Mesh Animation");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
