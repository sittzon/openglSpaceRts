#ifndef OBJ_HANDLER_CPP
#define OBJ_HANDLER_CPP

#include <sstream>

#include "ObjHandler.h"
#include "Globals.h"
#include "TGALoader.h"

using namespace std;

//-----------------------------------------
//FACE CODE
//-----------------------------------------

//Face::Face()
//{
////    static int i = 0;
////    nr = ++i;
////    cout  << "Number: "<< nr << " Created!" << endl;
////    vertIndex   = Indice();
////    normIndex   = Indice();
////    texIndex    = Indice();
//}
//Face::~Face()
//{
////    cout << "Number: "<< nr <<" Deleted!!" << endl;
//
//}
//
//
//Face::Face(const Face &in)
//{
////    vertIndex   = Indice(in.vertIndex.vert[0], in.vertIndex.vert[1], in.vertIndex.vert[2]);
////    normIndex   = Indice(in.normIndex.vert[0], in.normIndex.vert[1], in.normIndex.vert[2]);
////    texIndex    = Indice(in.texIndex.vert[0], in.texIndex.vert[1], in.texIndex.vert[2]);
//
//    cout << "Face copied!" << endl;
//}

//-----------------------------------------
//OBJHANDLER CODE
//-----------------------------------------

ObjHandler::ObjHandler()
{
}

ObjHandler::~ObjHandler()
{
}

Model* ObjHandler::loadObj(const string filename, const bool useTexture, const GLint rgbMode)
{
    //Create new model object
    Model *m = new Model();

    //Use standard values
    m = loadObj(filename, useTexture, rgbMode, Point(0.05, 0.8, 0.9, 200.0));

	//Return model object
	return m;
}


Model* ObjHandler::loadObj(const string filename, const bool useTexture, const GLint rgbMode, Point adss)
{
	//Initiate loading of file and connect to shader program
	//------------------------------------------------------
	init(filename);

	//Create new Model object
	//-----------------------
	Model* m = new Model();

	//Set model properties based on LoadObj variables
	//-----------------------------------------------

    //Convert to GL format for easy displaying
	float* vertToModel = new float[faces.size()*9];
	float* texToModel = new float[faces.size()*9];
	float* normToModel = new float[faces.size()*9];
    Face* tempFace = new Face();
	for (unsigned int i = 0; i < faces.size(); i++)
	{
	    tempFace = faces[i];
	    for(unsigned int j = 0; j < 9; j++)
	    {
	        vertToModel[i*9 + j] = tempFace->vertPos[j];
	        texToModel[i*9 + j] = tempFace->texPos[j];
	        normToModel[i*9 + j] = tempFace->normDir[j];
	    }
	}
	tempFace = NULL;
	delete tempFace;

    ///Set to model property
	m->vertices = vertToModel;
	m->texCoords = texToModel;
	m->normals = normToModel;
    m->faces = faces;

	//Load texture data to model
	unsigned int width, height;
	if(useTexture)
    {
        string textureFileName = filename.substr(0, filename.size() - 4);
        textureFileName = textureFileName.append(".tga");

        unsigned char* tempTexture = NULL;
        tempTexture = TGALoader::loadImage(textureFileName.c_str(), &width, &height);
        if (tempTexture != (unsigned char*)false)
        {
            m->texture = tempTexture;
        }
        else    //Load default checkboard pattern texture
        {
            cout << "Texture not found: " << textureFileName << endl;
            m->texture = TGALoader::loadImage("./models/Checkerboard_Pattern.tga", &width, &height);
        }
    }

    GLuint glErr = glGetError();
    if(glErr != 0)
        cout << "OBJHandler GL error 1: " << glErr << endl;

	//Sets some parameters that where previously hidden
    m->verticesSize = faces.size()*3;
    m->normalsSize = faces.size()*3;
	m->texCoordsSize = faces.size()*3;

    int index = filename.find_last_of("/") + 1;
	m->name = filename.substr(index, filename.length() - index - 4);

	//Allocate and activate Vertex Array Object (VAO)
	glGenVertexArrays(1, &m->vao);
	glGenBuffers(1, &m->vboVertex);
	glGenBuffers(1, &m->vboNormal);
	glGenBuffers(1, &m->vboTexture);
    if(useTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m->textureId);
    	glBindTexture(GL_TEXTURE_2D, m->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, rgbMode, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m->texture);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    //Set ambient, diffuse and specular components
    m->kAmb = adss[0];
    m->kDiff = adss[1];
    m->kSpec = adss[2];
    m->shininess = adss[3];

    //GL error check
    glErr = glGetError();
    if(glErr != 0)
        cout << "OBJHandler GL error 3: " << glErr << endl;

	//Clear data from class, to make room for loading of a new model
	//--------------------------------------------------------------
	clearAllData();

	//Return Model object
	//-------------------
	return m;
}

//PRIVATE DECLARATIONS
int ObjHandler::init(const string filename)
{
	//Initialize local variables
	string file = "";
	int numVertices = 0;
	int numTexCoords = 0;
	int numNorms = 0;

	//Reads info from .obj file
	//-------------------------
	ifstream temp(filename.c_str());

	if (temp.is_open() == true)
	{
		while(temp.good() == true)
		{
			getline(temp, file);

			//Empty line - skip
			if (file.length() == 0){}

			//Vertex texture coordinate
			else if (file[0] == 'v' && file[1] == 't')
			{
				numTexCoords++;
				texCoords.push_back(readTexCoord(file));
			}
			//Vertex normal
			else if (file[0] == 'v' && file[1] == 'n')
			{
				numNorms++;
				normals.push_back(readNormal(file));
			}
			//Vertex coordinates
			else if (file[0] == 'v')
			{
				numVertices++;
				vertices.push_back(readVertex(file));
			}
			//Face
			else if (file[0] == 'f')
			{
				createFace(file, numNorms, numTexCoords);
			}
		}
		temp.close();
	}
	else
	{
		cerr << "Error: .obj file load error (wrong filename?)" << endl;
		return 1;
	}

/*
	//If no normals exists, create from vertex data
	if (numNorms == 0)
	{
        cout << endl << "---------------------" << endl;
	    cout << "! No normals found, estimating normals for model: " << filename << " !" << endl;
        cout << "---------------------" << endl << endl;

        // 1 = one normal per vertex, 0 = 3 normals per vertex
	    createNormals();
	    numNorms = normals.size();
	}
*/

	cout << filename << endl;
	cout << "---------------------" << endl;
	cout << "Verts: " << numVertices << endl;
	cout << "Tex: " << numTexCoords << endl;
	cout << "Normals: " << numNorms << endl;
	cout << "Faces: " << faces.size() << endl;
	cout << "---------------------" << endl << endl;

	return 0;
}

Point ObjHandler::readVertex(string file)
{
	Point tempPoint;// = *new Point();//= {0,0,0};

    size_t start = 0;
	size_t end = 0;
	string cutOut = "";

    for (int i = 0; i < 3; i++)
    {
        start = file.find(' ',end);
        end = file.find(' ', start+2);
        cutOut = file.substr(start+1, end-start);

        tempPoint[i] = (float)atof(cutOut.c_str()); //Vertex
//        tempPoint.coord[i] = (float)atof(cutOut.c_str()); //Vertex

        start = end;
    }

    //cout << tempPoint.coord[0] << " " << tempPoint.coord[1] << " " << tempPoint.coord[2] << endl;
	return tempPoint;
}

Point ObjHandler::readNormal(string file)
{
	Point tempPoint;// = *new Point();// = {0,0,0};

    size_t start = 0;
	size_t end = 0;
	string cutOut = "";

    for (int i = 0; i < 3; i++)
    {
        start = file.find(' ',end);
        end = file.find(' ', start+2);
        cutOut = file.substr(start+1, end-start);

        tempPoint[i] = (float)atof(cutOut.c_str()); //Normal
//        tempPoint.coord[i] = (float)atof(cutOut.c_str()); //Normal

        start = end;
    }

	//Normalize
	float divide = sqrt(tempPoint[0]*tempPoint[0]+tempPoint[1]*tempPoint[1]+tempPoint[2]*tempPoint[2]);
	tempPoint[0] = tempPoint[0]/divide;
	tempPoint[1] = tempPoint[1]/divide;
	tempPoint[2] = tempPoint[2]/divide;

	//cout << tempPoint.coord[0] << ' ' << tempPoint.coord[1] << ' ' << tempPoint.coord[2] << endl;

	return tempPoint;
}

Point ObjHandler::readTexCoord(string file)
{
	Point tempPoint;// = *new Point();// = {0,0,0};

    size_t start = 0;
	size_t end = 0;
	string cutOut = "";

    for (int i = 0; i < 3; i++)
    {
        start = file.find(' ',end);
        end = file.find(' ', start+2);
        cutOut = file.substr(start+1, end-start);


        tempPoint[i] = (float)atof(cutOut.c_str()); //Texcoord
//        tempPoint.coord[i] = (float)atof(cutOut.c_str()); //Texcoord

        start = end;
    }

//    cout << tempPoint.coord[0] << ' ' << tempPoint.coord[1] << endl;
	return tempPoint;
}

void ObjHandler::createFace(string file, int numNormals, int numTexCoords)
{
	//--------------------------------------------
	//indices can be defined in four ways:
	//vertex					-> f v1 v2 v3
	//vertex/texcoord			-> f v1/vt1 v2/vt2 v3/vt3
	//vertex/texcoord/normal	-> f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
	//vertex/normal				-> f v1//vn1 v2//vn2 v3//vn3
	//--------------------------------------------
	size_t start = 0;
	size_t end = 0;
	string cutOut = file;

	float tempVertPos[12];
    float tempNormDir[12];
    float tempTexPos[12];

    //Quads or triangles?
    stringstream fileStream(file);  //Convert to stringstream
    unsigned int components = 0;
    unsigned int nrElements = 3;
    string doNotUseThisString;
    while (fileStream >> doNotUseThisString) components++;
    if(components == 5)     //Quads
        nrElements = 4;

	//vertex					-> f v1 v2 v3
	if (numNormals == 0 && numTexCoords == 0)
	{
	    for (unsigned int i = 0; i < nrElements; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            int number = (int)atoi(cutOut.c_str()) - 1;
            Point currentVert = vertices[number];
            tempVertPos[3*i + 0] =  vertices[number][0];
            tempVertPos[3*i + 1] =  vertices[number][1];
            tempVertPos[3*i + 2] =  vertices[number][2];

            start = end;
	    }
	}

	//vertex/texcoord			-> f v1/vt1 v2/vt2 v3/vt3
	else if (numNormals == 0 && numTexCoords > 0)
	{
        for (unsigned int i = 0; i < nrElements; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            int tempStart = 0;
            int tempEnd = 0;

            tempEnd = cutOut.find('/',tempStart);
            int number = (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1;
            tempVertPos[3*i + 0] =  vertices[number][0];
            tempVertPos[3*i + 1] =  vertices[number][1];
            tempVertPos[3*i + 2] =  vertices[number][2];

            tempStart = tempEnd+1;
            number = (int)atoi((cutOut.substr(tempStart)).c_str()) - 1;
            tempTexPos[3*i + 0] =  texCoords[number][0];
            tempTexPos[3*i + 1] =  texCoords[number][1];
            tempTexPos[3*i + 2] =  texCoords[number][2];

            start = end;
	    }
	}

	//vertex/texcoord/normal	-> f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
	else if (numNormals > 0 && numTexCoords > 0)
	{
	    for (unsigned int i = 0; i < nrElements; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            int tempStart = 0;
            int tempEnd = 0;

            tempEnd = cutOut.find('/',tempStart);
            int number = (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1;
            tempVertPos[3*i + 0] =  vertices[number][0];
            tempVertPos[3*i + 1] =  vertices[number][1];
            tempVertPos[3*i + 2] =  vertices[number][2];

            tempStart = tempEnd+1;
            tempEnd = cutOut.find('/',tempStart);
            number = (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1;
            tempTexPos[3*i + 0] =  texCoords[number][0];
            tempTexPos[3*i + 1] =  texCoords[number][1];
            tempTexPos[3*i + 2] =  texCoords[number][2];

            tempStart = tempEnd+1;
            number = (int)atoi((cutOut.substr(tempStart)).c_str()) - 1;
            tempNormDir[3*i + 0] =  normals[number][0];
            tempNormDir[3*i + 1] =  normals[number][1];
            tempNormDir[3*i + 2] =  normals[number][2];
            start = end;
	    }
	}

	//vertex/normal				-> f v1//vn1 v2//vn2 v3//vn3
	else if (numNormals > 0 && numTexCoords == 0)
	{
	    for (unsigned int i = 0; i < nrElements; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            int tempStart = 0;
            int tempEnd = 0;

            tempEnd = cutOut.find("//",tempStart);
            int number = (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1;
            tempVertPos[3*i + 0] =  vertices[number][0];
            tempVertPos[3*i + 1] =  vertices[number][1];
            tempVertPos[3*i + 2] =  vertices[number][2];

            tempStart = tempEnd+2;
            number = (int)atoi((cutOut.substr(tempStart)).c_str()) - 1;
            tempNormDir[3*i + 0] =  normals[number][0];
            tempNormDir[3*i + 1] =  normals[number][1];
            tempNormDir[3*i + 2] =  normals[number][2];

            start = end;
	    }
	}

	Face* face1 = new Face();

	if(nrElements == 4) //Quads
	{
	    Face* face2 = new Face();

        //Copy triangle vertex information, face 1
	    for(unsigned int i = 0; i < 9; i++)
        {
            face1->vertPos[i] = tempVertPos[i];
            face1->texPos[i] = tempTexPos[i];
            face1->normDir[i] = tempNormDir[i];
        }

        //Copy triangle vertex information, face 2
        for(unsigned int i = 0; i < 3; i++)
        {
            face2->vertPos[i] = tempVertPos[i];
            face2->texPos[i] = tempTexPos[i];
            face2->normDir[i] = tempNormDir[i];
        }
        for(unsigned int i = 6; i < 12; i++)
        {
            face2->vertPos[i-3] = tempVertPos[i];
            face2->texPos[i-3] = tempTexPos[i];
            face2->normDir[i-3] = tempNormDir[i];
        }

        //Put into faceslist
        faces.push_back(face1);
        faces.push_back(face2);

        face2 = NULL;
        delete face2;
	}
	else    //Triangles
	{
        //Copy triangle vertex information
	    for(unsigned int i = 0; i < 9; i++)
        {
            face1->vertPos[i] = tempVertPos[i];
            face1->texPos[i] = tempTexPos[i];
            face1->normDir[i] = tempNormDir[i];
        }
        faces.push_back(face1);
	}

    face1 = NULL;
    delete face1;

}

void ObjHandler::createNormals()
{
//    for (unsigned int i = 0; i < vertices.size(); i++)  //One normal per vertex!
//    {
//        normals.push_back(Point(0,0,0));
//    }
//
//	for (unsigned int i = 0; i < faces.size(); i++)
//    {
//		Indice currentFace = faces[i]->vertIndex;
//
//		//Find vertices in currentFace
//		Point vertex0 = vertices[currentFace.vert[0]];
//		Point vertex1 = vertices[currentFace.vert[1]];
//		Point vertex2 = vertices[currentFace.vert[2]];
//
//		//Create vectors between vertices
//		Point v10 = vertex1 - vertex0;
//		Point v20 = vertex2 - vertex0;
//
//		//Crossproduct
//		Point normal = vertex0.crossProduct(v10, v20);
//
//		//Normalize
//		normal = vertex0.normalize(normal);
//
//        // Use same indexing as for vertices
//        normals[currentFace.vert[0]] = normals[currentFace.vert[0]] + normal;
//        normals[currentFace.vert[1]] = normals[currentFace.vert[1]] + normal;
//        normals[currentFace.vert[2]] = normals[currentFace.vert[2]] + normal;
//
//        // Make sure the normIndex in the face-list corresponds to the correct normals
//        faces[i]->normIndex.vert[0] = currentFace.vert[0];
//		faces[i]->normIndex.vert[1] = currentFace.vert[1];
//		faces[i]->normIndex.vert[2] = currentFace.vert[2];
//
//	}
//
//	for (unsigned int i = 0; i < vertices.size(); i++)  //One normal per vertice!
//    {
//        normals[i] = normals[i].normalizeAndGet();
//    }
}

void ObjHandler::clearAllData()
{
	while(vertices.size() > 0)
		vertices.pop_back();
	while(normals.size() > 0)
		normals.pop_back();
	while(texCoords.size() > 0)
		texCoords.pop_back();
	while(faces.size() > 0)
		faces.pop_back();
}

#endif

