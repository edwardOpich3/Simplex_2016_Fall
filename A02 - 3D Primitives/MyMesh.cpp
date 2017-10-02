#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code

	// Define the points of the cone based on radius, height, and subdivisions (there should be a_nSubdivisons + 2 vertices)
	vector3* vertices = new vector3[a_nSubdivisions + 2];

	vertices[a_nSubdivisions] = vector3(0.0f, -a_fHeight / 2.0f, 0.0f);	// Bottom center point
	vertices[a_nSubdivisions + 1] = vector3(0.0f, a_fHeight / 2.0f, 0.0f);	// Tip of the cone

	// For every subdivision, place a point, starting at (radius, 0, 0)
	for (int i = 0; i < a_nSubdivisions; ++i)
	{
		vertices[i] = vector3(cos((i / (float)a_nSubdivisions) * 2 * PI) * a_fRadius, -a_fHeight / 2.0f, sin((i / (float)a_nSubdivisions) * 2 * PI) * a_fRadius);
	}

	// Define the triangles in the cone based on the number of points
	for (int i = 0; i < a_nSubdivisions; ++i)
	{
		AddTri(vertices[(i + 1) % a_nSubdivisions], vertices[i], vertices[a_nSubdivisions + 1]);
		AddTri(vertices[i], vertices[(i + 1) % a_nSubdivisions], vertices[a_nSubdivisions]);
	}

	// Delete your vertices now that you've defined your triangles
	delete[] vertices;

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code

	// Define the points of the cylinder based on radius, height, and subdivisions (there should be (a_nSubdivisons * 2) + 2 vertices)
	vector3* vertices = new vector3[(a_nSubdivisions * 2) + 2];

	vertices[(a_nSubdivisions * 2)] = vector3(0.0f, -a_fHeight / 2.0f, 0.0f);	// Bottom center point
	vertices[(a_nSubdivisions * 2) + 1] = vector3(0.0f, a_fHeight / 2.0f, 0.0f);	// Top center point

	// For every subdivision, place 2 points, starting at (radius, Y, 0), one at the bottom, one at the top
	for (int i = 0; i < a_nSubdivisions; ++i)
	{
		vertices[i] = vector3(cos((i / (float)a_nSubdivisions) * 2 * PI) * a_fRadius, -a_fHeight / 2.0f, sin((i / (float)a_nSubdivisions) * 2 * PI) * a_fRadius);
		vertices[i + a_nSubdivisions] = vector3(cos((i / (float)a_nSubdivisions) * 2 * PI) * a_fRadius, a_fHeight / 2.0f, sin((i / (float)a_nSubdivisions) * 2 * PI) * a_fRadius);
	}

	// Define the quads and triangles in the cylinder based on the number of points
	for (int i = 0; i < a_nSubdivisions; ++i)
	{
		AddQuad(vertices[(i + 1) % a_nSubdivisions], vertices[i], vertices[((i + 1) % a_nSubdivisions) + a_nSubdivisions], vertices[i + a_nSubdivisions]);
		AddTri(vertices[i], vertices[(i + 1) % a_nSubdivisions], vertices[a_nSubdivisions * 2]);
		AddTri(vertices[((i + 1) % a_nSubdivisions) + a_nSubdivisions], vertices[i + a_nSubdivisions], vertices[(a_nSubdivisions * 2) + 1]);
	}

	// Delete your vertices now that you've defined everything
	delete[] vertices;

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code

	// Define the points of the cylinder based on radius, height, and subdivisions (there should be (a_nSubdivisons * 4) vertices)
	vector3* vertices = new vector3[a_nSubdivisions * 4];

	// For every subdivision, place 4 points, starting at (radius, Y, 0), one at the outside bottom, one at the outside top, one at the inside bottom, and one at the inside top
	for (int i = 0; i < a_nSubdivisions; ++i)
	{
		vertices[i] = vector3(cos((i / (float)a_nSubdivisions) * 2 * PI) * a_fOuterRadius, -a_fHeight / 2.0f, sin((i / (float)a_nSubdivisions) * 2 * PI) * a_fOuterRadius);
		vertices[i + a_nSubdivisions] = vector3(cos((i / (float)a_nSubdivisions) * 2 * PI) * a_fOuterRadius, a_fHeight / 2.0f, sin((i / (float)a_nSubdivisions) * 2 * PI) * a_fOuterRadius);
		vertices[i + (a_nSubdivisions * 2)] = vector3(cos((i / (float)a_nSubdivisions) * 2 * PI) * a_fInnerRadius, -a_fHeight / 2.0f, sin((i / (float)a_nSubdivisions) * 2 * PI) * a_fInnerRadius);
		vertices[i + (a_nSubdivisions * 3)] = vector3(cos((i / (float)a_nSubdivisions) * 2 * PI) * a_fInnerRadius, a_fHeight / 2.0f, sin((i / (float)a_nSubdivisions) * 2 * PI) * a_fInnerRadius);
	}

	// Define the quads in the tube based on the number of points
	for (int i = 0; i < a_nSubdivisions; ++i)
	{
		AddQuad(vertices[(i + 1) % a_nSubdivisions], vertices[i], vertices[((i + 1) % a_nSubdivisions) + a_nSubdivisions], vertices[i + a_nSubdivisions]);
		AddQuad(vertices[i], vertices[(i + 1) % a_nSubdivisions], vertices[i + (a_nSubdivisions * 2)], vertices[((i + 1) % a_nSubdivisions) + (a_nSubdivisions * 2)]);
		AddQuad(vertices[((i + 1) % a_nSubdivisions) + a_nSubdivisions], vertices[i + a_nSubdivisions], vertices[((i + 1) % a_nSubdivisions) + (a_nSubdivisions * 3)], vertices[i + (a_nSubdivisions * 3)]);
		AddQuad(vertices[i + (a_nSubdivisions * 2)], vertices[((i + 1) % a_nSubdivisions) + (a_nSubdivisions * 2)], vertices[i + (a_nSubdivisions * 3)], vertices[((i + 1) % a_nSubdivisions) + (a_nSubdivisions * 3)]);
	}

	// Delete your vertices now that you've defined everything
	delete[] vertices;
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	//GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);

	// Radius of the tube
	float a_fTubeRadius = (a_fOuterRadius - a_fInnerRadius) / 2.0f;

	// Define the points of the cone based on radius, height, and subdivisions (I define subdivisionsA as around the ring, and subdivisionsB as looping through the ring) (there should be a_nSubdivisionsA * a_nSubdivisionsB vertices)
	vector3* vertices = new vector3[a_nSubdivisionsA * a_nSubdivisionsB];

	// For every subdivision, place a point, starting at (radius, 0, 0)
	for (int i = 0; i < a_nSubdivisionsA; ++i)
	{
		vector3 tubeCenter = vector3(cos((i / (float)a_nSubdivisionsA) * 2 * PI), 0, sin((i / (float)a_nSubdivisionsA) * 2 * PI)) * (a_fInnerRadius + a_fTubeRadius);	// The center of the circle making up the tube
		vector3 tubeTrace = (tubeCenter / sqrt(pow(tubeCenter.x, 2) + pow(tubeCenter.y, 2) + pow(tubeCenter.z, 2)));				// Unit vector pointing same direction as tubeCenter
		for (int j = 0; j < a_nSubdivisionsB; ++j)
		{
			vertices[(i * a_nSubdivisionsA) + j] = vector3(
				cos((j / (float)a_nSubdivisionsB) * 2 * PI) * tubeTrace.x,
				sin((j / (float)a_nSubdivisionsB) * 2 * PI),
				cos((j / (float)a_nSubdivisionsB) * 2 * PI) * tubeTrace.z);

			vertices[(i * a_nSubdivisionsA) + j] *= a_fTubeRadius;
			
			vertices[(i * a_nSubdivisionsA) + j] += tubeCenter;
		}
	}

	// Define the quads in the torus based on the number of points
	for (int i = 0; i < a_nSubdivisionsA; ++i)
	{
		for (int j = 0; j < a_nSubdivisionsB; ++j)
		{
			AddQuad(
				vertices[i + (j * a_nSubdivisionsB)],
				vertices[((i + 1) % a_nSubdivisionsA) + (j * a_nSubdivisionsB)],
				vertices[i + (((j + 1) % a_nSubdivisionsB) * a_nSubdivisionsB)],
				vertices[((i + 1) % a_nSubdivisionsA) + (((j + 1) % a_nSubdivisionsB) * a_nSubdivisionsB)]);
		}
	}

	// Delete your vertices now that you've defined your triangles
	delete[] vertices;

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > /*6*/ 12)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code

	// Define the points of the sphere based on radius, height, and subdivisions (there should be (a_nSubdivisons * (a_nSubdivisions - 1)) + 2 vertices)
	vector3* vertices = new vector3[(a_nSubdivisions * (a_nSubdivisions - 1)) + 2];

	vertices[(a_nSubdivisions * (a_nSubdivisions - 1))] = vector3(0.0f, -a_fRadius, 0.0f);	// Bottom center point
	vertices[(a_nSubdivisions * (a_nSubdivisions - 1)) + 1] = vector3(0.0f, a_fRadius, 0.0f);	// Top center point

	// For every subdivision, place a ring of points
	for (int i = 0; i < a_nSubdivisions - 1; ++i)
	{
		// For every subdivision, place a point in a ring
		for (int j = 0; j < a_nSubdivisions; ++j)
		{
			vertices[(i * a_nSubdivisions) + j] = vector3(
				cos((j / (float)a_nSubdivisions) * 2 * PI)  * sin(((i + 1) / (float)a_nSubdivisions) * PI),
				-cos(((i + 1) / (float)a_nSubdivisions) * PI),
				sin((j / (float)a_nSubdivisions) * 2 * PI)  * sin(((i + 1) / (float)a_nSubdivisions) * PI) );

			vertices[(i * a_nSubdivisions) + j] *= a_fRadius;
		}
	}

	// Define the quads in the sphere based on the number of points
	for (int i = 0; i < a_nSubdivisions - 2; ++i)
	{
		for (int j = 0; j < a_nSubdivisions; ++j)
		{
			AddQuad(vertices[((j + 1) % a_nSubdivisions) + (i * a_nSubdivisions)],
				vertices[j + (i * a_nSubdivisions)],
				vertices[((j + 1) % a_nSubdivisions) + ((i + 1) * a_nSubdivisions)],
				vertices[j + ((i + 1) * a_nSubdivisions)]);
		}
	}

	// Connect the outer rings of the sphere to the tips
	for (int i = 0; i < a_nSubdivisions; ++i)
	{
		AddTri(vertices[i],
			vertices[(i + 1) % a_nSubdivisions],
			vertices[(a_nSubdivisions * (a_nSubdivisions - 1))]);

		AddTri(vertices[((i + 1) % a_nSubdivisions) + (a_nSubdivisions * (a_nSubdivisions - 2))],
			vertices[i + (a_nSubdivisions * (a_nSubdivisions - 2))],
			vertices[(a_nSubdivisions * (a_nSubdivisions - 1)) + 1]);
	}

	// Delete your vertices now that you've defined everything
	delete[] vertices;
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}