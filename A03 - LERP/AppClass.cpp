#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
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

	// Timer
	static float fTimer = 0;	// The timer variable
	static uint uClock = m_pSystem->GenClock(); // Create a clock to base the timer off of
	fTimer += m_pSystem->GetDeltaTime(uClock); // Get the delta of that clock each frame to update the timer

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		float speed = 0.5f;		// How many seconds does it take to reach the next stop?
		int currentStopIndex = (int)(fTimer * (1.0f / speed)) % (i + 3);		// Which stop the current orb is on its path; the stop should never be greater than the number of sides!;

		//calculate the current position
		// Divide the unit circle into degrees based on how many sides the current torus has
		// Multiply that by the index of the current stop to get our position, then just take the cos/sin to get x/y
		// Multiply the whole vector based on the radius of our torus to place the spheres at the correct distance from the center
		vector3 v3CurrentPos = vector3(cos(((2 * PI) * (1.0f / (i + 3))) * currentStopIndex),
										sin(((2 * PI) * (1.0f / (i + 3))) * currentStopIndex),
										0.0f) * (1.0f + (i * 0.5f));

		// The LERP is calculated and added to the current position on this horridly long line of code
		// Get the distance between the current stop and the next stop
		// Multiply that based on the radius of the current torus so the sphere travels the correct distance
		// Multiply it again by the modulus of the current time (multiplied by the number of seconds it takes to travel between stops) and 1, as the weight
		v3CurrentPos += vector3(cos(((2 * PI) * (1.0f / (i + 3))) * (currentStopIndex + 1)) - cos(((2 * PI) * (1.0f / (i + 3))) * (currentStopIndex)),
								sin(((2 * PI) * (1.0f / (i + 3))) * (currentStopIndex + 1)) - sin(((2 * PI) * (1.0f / (i + 3))) * (currentStopIndex)),
								0.0f) * fmod(fTimer * (1.0f / speed), 1.0f) * (1.0f + (i * 0.5f));

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}