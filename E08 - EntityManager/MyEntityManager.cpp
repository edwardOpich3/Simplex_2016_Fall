#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	// Set the entity count to 0, as there shouldn't be anything in the list
	m_uEntityCount = 0;
}
void MyEntityManager::Release(void)
{
	// Iterate through the list of entities, deleting each one and setting its pointer to the null poitner
	for (std::vector<MyEntity*>::iterator i = m_entityList.begin(); i != m_entityList.end();)
	{
		delete *i;
		*i = nullptr;
		i = m_entityList.erase(i);
	}

	// There are no more entities in the list; set the count to 0
	m_uEntityCount = 0;
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	// Check of the instance is already null pointer first to avoid exceptions
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// Iterate through the list to find the entity that has an ID matching the one provided
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return i;
		}
	}

	// If we've reached this point the ID doesn't match anything.
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), return the model of the last entry in the list
	if (a_uIndex == -1)
	{
		return m_entityList[m_uEntityCount - 1]->GetModel();
	}

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	// Return the model of the entity that's ID matches the one provided
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetModel();
		}
	}

	// Return nothing if the ID matches nothing
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), return the rigidbody of the last entry in the list
	if (a_uIndex == -1)
	{
		return m_entityList[m_uEntityCount - 1]->GetRigidBody();
	}

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	// Return the rigidbody of the entity that's ID matches the one provided
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetRigidBody();
		}
	}

	// Return nothing if the ID matches nothing
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), return the model matrix of the last entry in the list
	if (a_uIndex == -1)
	{
		return m_entityList[m_uEntityCount - 1]->GetModelMatrix();
	}

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	// Return the model matrix of the entity that's ID matches the one provided
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetModelMatrix();
		}
	}

	// Return the identity matrix if the ID matches nothing
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	// Set the model matrix of the entity that's ID matches the one provided, to the matrix provided
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			m_entityList[i]->SetModelMatrix(a_m4ToWorld);
			return;
		}
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), set the model matrix of the last entry in the list to the one provided
	if (a_uIndex == -1)
	{
		m_entityList[m_uEntityCount - 1]->SetModelMatrix(a_m4ToWorld);
		return;
	}

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	// Check to see if any of the models are colliding, and if so mark their rigidbodies accordingly
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		// j is initialized to i + 1 because there's no need to check collisions already checked, or collisions with oneself
		for (unsigned int j = i + 1; j < m_uEntityCount; j++)
		{
			if (m_entityList[i]->IsColliding(m_entityList[j]))
			{
				m_entityList[i]->GetRigidBody()->AddCollisionWith(m_entityList[j]->GetRigidBody());
			}
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	m_entityList.push_back(new MyEntity(a_sFileName, a_sUniqueID));
	m_uEntityCount++;

	// The axis is visible in the provided demo exe, so why not set it visible here?
	m_entityList[m_uEntityCount - 1]->SetAxisVisible(true);
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), delete the last entity in the list, set its pointer to null, and delete its entry
	if (a_uIndex == -1)
	{
		delete m_entityList[m_uEntityCount - 1];
		m_entityList[m_uEntityCount - 1] = nullptr;
		m_entityList.pop_back();

		m_uEntityCount--;
		return;
	}

	// Make a vector iterator that's set to the index given, delete the entity at that index, set its pointer to null, and remove its entry in the list
	std::vector<MyEntity*>::iterator myIterator = m_entityList.begin() + a_uIndex;
	delete m_entityList[a_uIndex];
	m_entityList[a_uIndex] = nullptr;
	m_entityList.erase(myIterator);

	m_uEntityCount--;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	// Iterate through the list until the entity with the ID matching the one provided is found
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			// Create a vector iterator set to the index of our entity, delete the entity, set its pointer to null, and delete its entry in the list
			std::vector<MyEntity*>::iterator myIterator = m_entityList.begin() + i;
			delete m_entityList[i];
			m_entityList[i] = nullptr;
			m_entityList.erase(myIterator);

			m_uEntityCount--;
			return;
		}
	}
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), return the ID of the last entry in the list
	if (a_uIndex == -1)
	{
		return m_entityList[m_uEntityCount - 1]->GetUniqueID();
	}

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), return the last entry in the list
	if (a_uIndex == -1)
	{
		return m_entityList[m_uEntityCount - 1];
	}

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// If a_uIndex == -1 (except it's unsigned, so whatever the max value for uint is), add every entity in the list to the render list
	if (a_uIndex == -1)
	{
		for (unsigned int i = 0; i < m_uEntityCount; i++)
		{
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		}
		return;
	}

	m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	// Iterate through the list until we find the entity whose ID matches the one provided, then add it to the render list
	for (unsigned int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			m_entityList[i]->AddToRenderList(a_bRigidBody);
			return;
		}
	}
}