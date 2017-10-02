#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Make MyMesh object
	m_pMesh = new MyMesh[46];
	for (int i = 0; i < 46; ++i)
	{
		m_pMesh[i].GenerateCube(1.0f, C_BLACK);
	}

	// Initial position for the space invader
	v3Position = vector3(-33.0f, 5.5f, -8.0f);

	// Store the relative positions to keep the boxes at
	positions = new vector2[46];

	// Hardcode all of the necessary positions ( oh boy :( )
	positions[0] = vector2(2.0f, 0.0f);
	positions[1] = vector2(8.0f, 0.0f);

	positions[2] = vector2(3.0f, -1.0f);
	positions[3] = vector2(7.0f, -1.0f);

	positions[4] = vector2(2.0f, -2.0f);
	positions[5] = vector2(3.0f, -2.0f);
	positions[6] = vector2(4.0f, -2.0f);
	positions[7] = vector2(5.0f, -2.0f);
	positions[8] = vector2(6.0f, -2.0f);
	positions[9] = vector2(7.0f, -2.0f);
	positions[10] = vector2(8.0f, -2.0f);

	positions[11] = vector2(1.0f, -3.0f);
	positions[12] = vector2(2.0f, -3.0f);
	positions[13] = vector2(4.0f, -3.0f);
	positions[14] = vector2(5.0f, -3.0f);
	positions[15] = vector2(6.0f, -3.0f);
	positions[16] = vector2(8.0f, -3.0f);
	positions[17] = vector2(9.0f, -3.0f);

	positions[18] = vector2(0.0f, -4.0f);
	positions[19] = vector2(1.0f, -4.0f);
	positions[20] = vector2(2.0f, -4.0f);
	positions[21] = vector2(3.0f, -4.0f);
	positions[22] = vector2(4.0f, -4.0f);
	positions[23] = vector2(5.0f, -4.0f);
	positions[24] = vector2(6.0f, -4.0f);
	positions[25] = vector2(7.0f, -4.0f);
	positions[26] = vector2(8.0f, -4.0f);
	positions[27] = vector2(9.0f, -4.0f);
	positions[28] = vector2(10.0f, -4.0f);

	positions[29] = vector2(0.0f, -5.0f);
	positions[30] = vector2(2.0f, -5.0f);
	positions[31] = vector2(3.0f, -5.0f);
	positions[32] = vector2(4.0f, -5.0f);
	positions[33] = vector2(5.0f, -5.0f);
	positions[34] = vector2(6.0f, -5.0f);
	positions[35] = vector2(7.0f, -5.0f);
	positions[36] = vector2(8.0f, -5.0f);
	positions[37] = vector2(10.0f, -5.0f);

	positions[38] = vector2(0.0f, -6.0f);
	positions[39] = vector2(2.0f, -6.0f);
	positions[40] = vector2(8.0f, -6.0f);
	positions[41] = vector2(10.0f, -6.0f);

	positions[42] = vector2(3.0f, -7.0f);
	positions[43] = vector2(4.0f, -7.0f);
	positions[44] = vector2(6.0f, -7.0f);
	positions[45] = vector2(7.0f, -7.0f);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();		// How to look at object
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();					// From where

	matrix4 m4Model;													// The model

	// Always moving right by 0.1f units per frame
	v3Position.x += 0.1f;

	// Teleport the model back to the starting position if it proceeds too far
	if (v3Position.x > 18.0f)
	{
		v3Position.x = -33.0f;
	}

	// Give it a cool sine wave flight pattern
	v3Position.y = 5.5f - (sin((v3Position.x / 8.0f) * 2 * PI) * 2.0f);

	m4Model = glm::translate(m4Model, v3Position);

	// Render all of the cubes that make up the space invader
	for (int i = 0; i < 46; ++i)
	{
		m_pMesh[i].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(positions[i], 0)));
	}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete[] m_pMesh;
		m_pMesh = nullptr;
	}

	if (positions != nullptr)
	{
		delete[] positions;
		positions = nullptr;
	}

	//release GUI
	ShutdownGUI();
}