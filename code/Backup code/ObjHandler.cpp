#ifndef OBJ_HANDLER_CPP
#define OBJ_HANDLER_CPP

#include "ObjHandler.h"
#include <cstdlib>

using namespace std;

#define PI 3.1415926535;


// ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ Indice ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Indice::Indice(int i1, int i2, int i3)
{
    vert = new unsigned int[3];

    vert[0] = i1;
    vert[1] = i2;
    vert[2] = i3;

//    cout  << "Number: "<< nr << " Created!" << endl;

}

Indice::~Indice()
{
    delete[] vert;
    vert = NULL;

//    cout << "Number: "<< nr <<" Deleted!!" << endl;
}

Indice::Indice(const Indice &in)
{
    vert = new unsigned int[3];

    vert[0] = in.vert[0];
    vert[1] = in.vert[1];
    vert[2] = in.vert[2];

//    cout << "New Number: " << nr << " Copied from " << in.nr << endl;
}


//-----------------------------------------
//FACE CODE
//-----------------------------------------

face::face()
{
//    static int i = 0;
//    nr = ++i;
//    cout  << "Number: "<< nr << " Created!" << endl;
//    vertIndex   = Indice();
//    normIndex   = Indice();
//    texIndex    = Indice();
}
face::~face()
{
//    cout << "Number: "<< nr <<" Deleted!!" << endl;

}


face::face(const face &in)
{
    vertIndex   = Indice(in.vertIndex.vert[0], in.vertIndex.vert[1], in.vertIndex.vert[2]);
    normIndex   = Indice(in.normIndex.vert[0], in.normIndex.vert[1], in.normIndex.vert[2]);
    texIndex    = Indice(in.texIndex.vert[0], in.texIndex.vert[1], in.texIndex.vert[2]);

    cout << "Face copied!" << endl;
}

void face::setVertValue(int index, int value)
{
    vertIndex.vert[index] = value;
}
void face::setNormValue(int index, int value)
{
    normIndex.vert[index] = value;
}
void face::setTexValue(int index, int value)
{
    texIndex.vert[index] = value;
}


//-----------------------------------------
//OBJHANDLER CODE
//-----------------------------------------


ObjHandler::ObjHandler()
{
    faces = vector<face*>();

    tga = new TGALoader();
}

ObjHandler::ObjHandler(string filename)
{
	init(filename);
}

ObjHandler::~ObjHandler()
{

}

Model* ObjHandler::loadObj(const string filename, const bool useTexture, const GLint rgbMode)
{
	//Initiate loading of file and connect to shader program
	//------------------------------------------------------
	init(filename);

	//Create new Model object
	//-----------------------
	Model *m = new Model();

	//Set model properties based on LoadObj variables
	//-----------------------------------------------
	m->vertices = getVertices();
	m->normals = getNormals();
	m->texCoords = getTexCoords();

	m->indices = getIndices();

	//Load texture data to model
	unsigned int width, height;
	string textureFileName = filename.substr(0, filename.size() - 4);
	textureFileName = textureFileName.append(".tga");
	m->texture = tga->loadImage(textureFileName.c_str(), &width, &height);

	m->faces = faces;

	//Sets some parameters that where previously hidden
    //------------------------------------
    m->facesSize = faces.size();
    m->verticesSize = vertices.size();
    m->normalsSize = normals.size();
	m->texCoordsSize = texCoords.size();

	//m->normalIndicies = getNormalsIndicies(m->normalsIndiciesSize);

    int index = filename.find_last_of("/")+1;
	m->name = filename.substr(index, filename.length() - index - 4);

	//Allocate and activate Vertex Array Object (VAO)
	//-----------------------------------------------
	glGenVertexArrays(1, &m->vao);
	glGenBuffers(1, &m->vboVertex);
	glGenBuffers(1, &m->vboNormal);
	glGenBuffers(1, &m->vboTexture);
	glGenBuffers(1, &m->vboIndicies);
	glGenTextures(1, &m->textureId);
	if(useTexture)
    {
    	glBindTexture(GL_TEXTURE_2D, m->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, rgbMode, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m->texture);
    }

    //GL error check
    GLuint glErr = glGetError();
    if(glErr != 0)
        cout << "GL error: " << glErr << endl;

    glBindTexture(GL_TEXTURE_2D, 0);

	//Allocate and activate Vertex Array Object (VAO) for drawing Normals
	//--------------------------------------------------------------------
	glGenVertexArrays(1, &m->drawNormalsVao);
	glGenBuffers(1, &m->drawNormalsVboVertex);
	glGenBuffers(1, &m->drawNormalsVboNormal);
	glGenBuffers(1, &m->drawNormalsVboIndicies);

    // Upload model to GPU.
    m->reBindBufferObject();

	//Clear data from class, to make room for loading of a new model
	//--------------------------------------------------------------
	clearAllData();

	//Return Model object
	//-------------------
	return m;
}

void ObjHandler::writeObj(string filename, Model *m)
{
    bool normalsExist = m->normalsSize;
    bool texturesExist = m->texCoordsSize;

    //cout << "Normals: " << m->normalsSize << endl;
    //cout << "Texs: " << m->texCoordsSize << endl;

    cout << "Saving " + filename << endl;

    //ifstream fileExists(filename.c_str());
    ofstream outFile;

    //Does file exist?
    /*
    if (fileExists)
    {
        cout << "File exists: ";
        filename.replace(filename.end()-4, filename.end(), "_new.obj");
        cout << "Creating new file " + filename + " due to conflicting name" << endl;
    }

    //File does not exist, create new
    else
    {
        cout << "File does not exist, creating new" << endl;
    }
    */



    outFile.open(filename.c_str());

    outFile << "#u3D obj exporter 0.1 - 2012 u3d crew\n";
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    outFile << "#File created " << asctime(timeinfo) << "\n\n";

    //Save vertices
    for(unsigned int i = 0; i < 3*m->verticesSize; i++)
    {
        if (i%3 == 0)
        {
            outFile << "v";
        }

        outFile << " " << m->vertices[i];

        if ((i+1)%3 == 0 && i != 0)
        {
            outFile << "\n";
        }
    }

    outFile << "\n";

    //Save normals if they exist
    if (normalsExist == true)
    {
        for (unsigned int i = 0; i < 3*m->normalsSize; i++)
        {
            if (i%3 == 0)
            {
                outFile << "vn";
            }

            outFile << " " << m->normals[i];

            if ((i+1)%3 == 0 && i != 0)
            {
                outFile << "\n";
            }
        }
    }

    outFile << "\n";

    //Save texCoords if they exist
    if (texturesExist == true)
    {
        for (unsigned int i = 0; i < 2*m->texCoordsSize; i++)
        {
            if (i%2 == 0)
            {
                outFile << "vt";
            }

            outFile << " " << m->texCoords[i];

            if ((i+1)%2 == 0 && i != 0)
            {
                outFile << "\n";
            }
        }
    }

    outFile << "\n";

    for(vector<face*>::iterator it = m->faces.begin(); it < m->faces.end() ; ++it)
    {
        outFile << "f";

        for(int j = 0; j < 3 ; ++j)
        {

            outFile << " " << (*it)->vertIndex.vert[j]+1;

            if (normalsExist == true && texturesExist == false)
            {
                outFile << "//" << (*it)->normIndex.vert[j]+1;
            }
            else if (normalsExist == true && texturesExist == true)
            {
                outFile << "/" << (*it)->texIndex.vert[j]+1;
                outFile << "/" << (*it)->normIndex.vert[j]+1;
            }
//            else if (normalsExist == false && texturesExist == false)
//            {
//                outFile << m->indices[i]+1;
//            }
        }

        outFile << "\n";
    }

    cout << "File saved" << endl << endl;

    //Close file
    outFile.close();
}

void ObjHandler::drawModel(Model *m)
{
	glBindVertexArray(m->vao);

    // Find current program.
    GLint program;

    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    // Get the attributes pos.
    GLint position = glGetAttribLocation(program, "inPosition");
    GLint normal = glGetAttribLocation(program, "inNormal");
    GLint texture = glGetAttribLocation(program, "inTexCoord");

    // Bind the models VBOs to the current program.
	glBindBuffer(GL_ARRAY_BUFFER, m->vboVertex);
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position);

    if(normal != -1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m->vboNormal);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal);
    }

    if(texture != -1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m->vboTexture);
        glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(texture);
    }

    // Draw the model and texture
    glBindTexture(GL_TEXTURE_2D, m->textureId);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, m->vertices);
	glDrawElements(GL_TRIANGLES, m->facesSize*3, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

//PRIVATE DECLARATIONS

int ObjHandler::init(const string filename)
{
	//Initialize local variables
	string file = "";
	int numVertices = 0;
	int numTexCoords = 0;
	int numNorms = 0;
	int numFaces = 0;

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
				texCoords.push_back(setTexCoord(file));
			}
			//Vertex normal
			else if (file[0] == 'v' && file[1] == 'n')
			{
				numNorms++;
				normals.push_back(setNormal(file));
			}
			//Vertex coordinates
			else if (file[0] == 'v')
			{
				numVertices++;
				vertices.push_back(setVertex(file));
			}
			//Face
			else if (file[0] == 'f')
			{
				numFaces++;
				face* f = createFace(file, numNorms, numTexCoords);
				faces.push_back(f);
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

	//Else, fix normals to correspond to vertex indexes
	else if (numNorms == numVertices)
	{
	    vector<Point> newNormals (vertices.size(), Point(3));
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            face* f = faces[i];
            for (int j = 0; j < 3; j++)
            {
                newNormals[f->vertIndex.vert[j]] = normals[f->normIndex.vert[j]];
                f->normIndex.vert[j] = f->vertIndex.vert[j];
            }
        }
        normals = newNormals;
	}


//FEL PÅ FIX TEXCOOORDS!!!
//-------------------------------
// TODO (Magnus#1#): FIX THIS, VERY WRONG
	if(numTexCoords == 0)
	{
	    //No coords
	    cout << "No texcoords found in model" << endl;
	}
	//Else, fix texcoords to correspond to vertex indexes
	else //if (numTexCoords == numVertices)
	{
//        cout << endl << "---------------------" << endl;
//        cout << "Fixing texcoords..." << endl;
//        cout << "---------------------" << endl;
        vector<Point> newTexCoords (vertices.size(), Point(2));
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            face* f = faces[i];
            for (int j = 0; j < 3; j++)
            {
                newTexCoords[f->vertIndex.vert[j]] = texCoords[f->texIndex.vert[j]];
                f->texIndex.vert[j] = f->vertIndex.vert[j];
            }
        }
        texCoords = newTexCoords;
	}
//-------------------------------
*/

	cout << filename << endl;
	cout << "---------------------" << endl;
	cout << "Verts: " << numVertices << endl;
	cout << "Tex: " << numTexCoords << endl;
	cout << "Normals: " << numNorms << endl;
	cout << "Faces: " << numFaces << endl;
	cout << "---------------------" << endl << endl;

	return 0;
}

Point ObjHandler::setVertex(string file)
{
	Point tempPoint = *new Point();//= {0,0,0};

    size_t start = 0;
	size_t end = 0;
	string cutOut = "";

    for (int i = 0; i < 3; i++)
    {
        start = file.find(' ',end);
        end = file.find(' ', start+2);
        cutOut = file.substr(start+1, end-start);

        tempPoint.coord[i] = (float)atof(cutOut.c_str()); //Vertex

        start = end;
    }

    //cout << tempPoint.coord[0] << " " << tempPoint.coord[1] << " " << tempPoint.coord[2] << endl;
	return tempPoint;
}

Point ObjHandler::setNormal(string file)
{
	Point tempPoint = *new Point();// = {0,0,0};

    size_t start = 0;
	size_t end = 0;
	string cutOut = "";

    for (int i = 0; i < 3; i++)
    {
        start = file.find(' ',end);
        end = file.find(' ', start+2);
        cutOut = file.substr(start+1, end-start);

        tempPoint.coord[i] = (float)atof(cutOut.c_str()); //Normal

        start = end;
    }

	//Normalize
	float divide = sqrt(tempPoint.coord[0]*tempPoint.coord[0]+tempPoint.coord[1]*tempPoint.coord[1]+tempPoint.coord[2]*tempPoint.coord[2]);
	tempPoint.coord[0] = tempPoint.coord[0]/divide;
	tempPoint.coord[1] = tempPoint.coord[1]/divide;
	tempPoint.coord[2] = tempPoint.coord[2]/divide;

	//cout << tempPoint.coord[0] << ' ' << tempPoint.coord[1] << ' ' << tempPoint.coord[2] << endl;

	return tempPoint;
}

Point ObjHandler::setTexCoord(string file)
{
	Point tempPoint = *new Point();// = {0,0,0};

    size_t start = 0;
	size_t end = 0;
	string cutOut = "";

    for (int i = 0; i < 2; i++)
    {
        start = file.find(' ',end);
        end = file.find(' ', start+2);
        cutOut = file.substr(start+1, end-start);

        tempPoint.coord[i] = (float)atof(cutOut.c_str());

        start = end;
    }

//    cout << tempPoint.coord[0] << ' ' << tempPoint.coord[1] << endl;
	return tempPoint;
}

face* ObjHandler::createFace(string file, int numNormals, int numTexCoords)
{
	//--------------------------------------------
	//indices can be defined in four ways:
	//vertex					-> f v1 v2 v3
	//vertex/texcoord			-> f v1/vt1 v2/vt2 v3/vt3
	//vertex/texcoord/normal	-> f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
	//vertex/normal				-> f v1//vn1 v2//vn2 v3//vn3
	//--------------------------------------------

	face* tempFace = new face();

	size_t start = 0;
	size_t end = 0;
	string cutOut = "";

	//vertex					-> f v1 v2 v3
	if (numNormals == 0 && numTexCoords == 0)
	{
	    for (int i = 0; i < 3; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            tempFace->setVertValue(i, (int)atoi(cutOut.c_str()) - 1); //Face indice

            start = end;
	    }
	}

	//vertex/texcoord			-> f v1/vt1 v2/vt2 v3/vt3
	else if (numNormals == 0 && numTexCoords > 0)
	{
        for (int i = 0; i < 3; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            int tempStart = 0;
            int tempEnd = 0;

            tempEnd = cutOut.find('/',tempStart);
            tempFace->setVertValue(i, (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1); //Face indice

            tempStart = tempEnd+1;
            tempFace->setTexValue(i, (int)atoi((cutOut.substr(tempStart)).c_str()) - 1); //Texcoord indice

            start = end;
	    }
	}

	//vertex/texcoord/normal	-> f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
	else if (numNormals > 0 && numTexCoords > 0)
	{
	    for (int i = 0; i < 3; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            int tempStart = 0;
            int tempEnd = 0;

            tempEnd = cutOut.find('/',tempStart);
            tempFace->setVertValue(i, (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1); //Face indice

            tempStart = tempEnd+1;
            tempEnd = cutOut.find('/',tempStart);
            tempFace->setTexValue(i, (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1); //Texcoord indice

            tempStart = tempEnd+1;
            tempFace->setNormValue(i, (int)atoi((cutOut.substr(tempStart)).c_str()) - 1); //Normal indice

            start = end;
	    }
	}

	//vertex/normal				-> f v1//vn1 v2//vn2 v3//vn3
	else if (numNormals > 0 && numTexCoords == 0)
	{
	    for (int i = 0; i < 3; i++)
	    {
            start = file.find(' ',end);
            end = file.find(' ', start+1);
            cutOut = file.substr(start+1, end-start);

            int tempStart = 0;
            int tempEnd = 0;

            tempEnd = cutOut.find("//",tempStart);
            tempFace->setVertValue(i, (int)atoi((cutOut.substr(tempStart, tempEnd-tempStart)).c_str()) - 1); //Face indice

            tempStart = tempEnd+2;
            tempFace->setNormValue(i, (int)atoi((cutOut.substr(tempStart)).c_str()) - 1); //Normal indice

            start = end;
	    }
	}

	return tempFace;
}

void ObjHandler::createNormals()
{
    for (unsigned int i = 0; i < vertices.size(); i++)  //One normal per vertex!
    {
        normals.push_back(Point(0,0,0));
    }

	for (unsigned int i = 0; i < faces.size(); i++)
    {
		Indice currentFace = faces[i]->vertIndex;

		//Find vertices in currentFace
		Point vertex0 = vertices[currentFace.vert[0]];
		Point vertex1 = vertices[currentFace.vert[1]];
		Point vertex2 = vertices[currentFace.vert[2]];

		//Create vectors between vertices
		Point v10 = vertex1 - vertex0;
		Point v20 = vertex2 - vertex0;

		//Crossproduct
		Point normal = vertex0.crossProduct(v10, v20);

		//Normalize
		normal = vertex0.normalize(normal);

        // Use same indexing as for vertices
        normals[currentFace.vert[0]] = normals[currentFace.vert[0]] + normal;
        normals[currentFace.vert[1]] = normals[currentFace.vert[1]] + normal;
        normals[currentFace.vert[2]] = normals[currentFace.vert[2]] + normal;

        // Make sure the normIndex in the face-list corresponds to the correct normals
        faces[i]->normIndex.vert[0] = currentFace.vert[0];
		faces[i]->normIndex.vert[1] = currentFace.vert[1];
		faces[i]->normIndex.vert[2] = currentFace.vert[2];

	}

	for (unsigned int i = 0; i < vertices.size(); i++)  //One normal per vertice!
    {
        normals[i] = normals[i].normalizeAndGet();
    }
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

float* ObjHandler::getVertices()
{
	float* out;
	out = new float[vertices.size()*3];
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Point tempVert = vertices[i];
			out[3*i+j] = tempVert.coord[j];
		}
	}
	return out;
}

float* ObjHandler::getTexCoords()
{
	float* out;
	out = new float[texCoords.size()*2];
	for (unsigned int i = 0; i < texCoords.size(); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Point tempTex = texCoords[i];
			out[2*i+j] = tempTex.coord[j];
		}
	}
	return out;
}

float* ObjHandler::getNormals()
{
	float* out;
	out = new float[normals.size()*3];
	for (unsigned int i = 0; i < normals.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Point tempNormal = normals[i];
			out[3*i+j] = tempNormal.coord[j];
		}
	}
	return out;
}

int* ObjHandler::getIndices()
{
	int* out;
	out = new int[faces.size()*3];

    for (unsigned int i = 0; i < faces.size()*3 ; i++)
    {
        // Should give 0 0 0 1 1 1 2 2 2 etc.
        out[i] = faces[(i-i%3)/3]->vertIndex.vert[i%3]; // faces[i/3]->vertIndex.vert[i%3];
    }
	return out;
}

#endif

