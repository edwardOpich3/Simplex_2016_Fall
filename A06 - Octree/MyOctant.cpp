#include "MyOctant.h"

using namespace Simplex;

uint MyOctant::m_uIdealEntityCount;
uint MyOctant::m_uMaxLevel;
uint MyOctant::m_uOctantCount;

// For the root octant
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	// Since this is the root octant, we can safely assume the following:
	m_pRoot = this;

	// The root octant should contain every entity in the game!
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		m_EntityList.push_back(i);
	}

	// Initialize max and min
	m_v3Max = m_v3Min = m_pEntityMngr->GetRigidBody(0)->GetCenterGlobal();

	// Have to define the size and center yourself via the entities!
	// Loop through the entity manager's list and determine the two entities whose centers are furthest apart on each axis
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		MyRigidBody* a = m_pEntityMngr->GetRigidBody(i);

		if (a->GetMaxGlobal().x > m_v3Max.x)
		{
			m_v3Max.x = a->GetMaxGlobal().x;
		}
		else if (a->GetMinGlobal().x < m_v3Min.x)
		{
			m_v3Min.x = a->GetMinGlobal().x;
		}

		if (a->GetMaxGlobal().y > m_v3Max.y)
		{
			m_v3Max.y = a->GetMaxGlobal().y;
		}
		else if (a->GetMinGlobal().y < m_v3Min.y)
		{
			m_v3Min.y = a->GetMinGlobal().y;
		}

		if (a->GetMaxGlobal().z > m_v3Max.z)
		{
			m_v3Max.z = a->GetMaxGlobal().z;
		}
		else if (a->GetMinGlobal().z < m_v3Min.z)
		{
			m_v3Min.z = a->GetMinGlobal().z;
		}
	}

	// I'm under the impression that an octant is supposed to be a uniform cube;
	// As such, max and min should all be based on the most extreme coordinate of the max and min entities.

	// Once we have the 6 in question, the our max vector will be determined by the maxes of those 3 entities at the positive extreme

	// m_v3Center will be based on the max and min vectors
	m_v3Center = (m_v3Max + m_v3Min) / 2.0f;

	// The size will be calculated from the length of either the min or max vector
	if (m_v3Max.x - m_v3Min.x > m_v3Max.y - m_v3Min.y)
	{
		if (m_v3Max.x - m_v3Min.x > m_v3Max.z - m_v3Min.z)
		{
			m_fSize = m_v3Max.x - m_v3Min.x;
		}
		else
		{
			m_fSize = m_v3Max.z - m_v3Min.z;
		}
	}
	else
	{
		if (m_v3Max.y - m_v3Min.y > m_v3Max.z - m_v3Min.z)
		{
			m_fSize = m_v3Max.y - m_v3Min.y;
		}
		else
		{
			m_fSize = m_v3Max.z - m_v3Min.z;
		}
	}

	m_lChild.push_back(this);
}

// Should only be called by a parent octant
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	// Calculate the max and min based on size
	float myHalfWidth = m_fSize / 2.0f;
	for (uint i = 0; i < 3; i++)
	{
		m_v3Max[i] = m_v3Center[i] + myHalfWidth;
		m_v3Min[i] = m_v3Center[i] - myHalfWidth;
	}

	// Lastly, check the number of entities contained within this octant!
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		MyRigidBody* curRB = m_pEntityMngr->GetEntity(i)->GetRigidBody();

		// We need to check if the min is less than our max, or if the max is greater than our min, in all dimensions
		// If all 3 dimensions are intersecting, it's in the octant; add it to the list.
		for (uint j = 0; j < 3; j++)
		{
			// If they're separated by a dimension, we can break from the loop, cause they aren't in the octant
			if (curRB->GetMaxGlobal()[j] < m_v3Min[j] || curRB->GetMinGlobal()[j] > m_v3Max[j])
			{
				break;
			}

			// Otherwise, if we've made it through every dimension, they are definitely intersecting, and as such the entity is in the octant.
			else if (j == 2)
			{
				m_EntityList.push_back(i);
			}
		}
	}
}

MyOctant::MyOctant(MyOctant const & other)
{
	// Craftily utilize code we've already written in the operator= overload
	*this = other;
}

MyOctant & MyOctant::operator=(MyOctant const & other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_v3Center = other.m_v3Center;
	m_v3Max = other.m_v3Max;
	m_v3Min = other.m_v3Min;

	m_pParent = other.m_pParent;

	// If this octant has children, they need to be destroyed first, otherwise they will be inaccessible!
	KillBranches();

	for (uint i = 0; i < other.m_uChildren; i++)
	{
		m_pChild[i] = other.m_pChild[i];
		m_uChildren++;
	}

	ClearEntityList();
	m_EntityList = other.m_EntityList;
	
	m_pRoot = other.m_pRoot;

	return *this;
}

MyOctant::~MyOctant()
{
	Release();
}

void MyOctant::Swap(MyOctant & other)
{
	MyOctant temp = *this;

	*this = other;
	other = temp;
}

float MyOctant::GetSize()
{
	return m_fSize;
}

vector3 MyOctant::GetCenterGlobal()
{
	return m_v3Center;
}

vector3 MyOctant::GetMinGlobal()
{
	return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal()
{
	return m_v3Max;
}

bool MyOctant::IsColliding(uint a_uRBIndex)
{
	// Iterate through every entity in our octant and check if they are colliding with the entity provided
	MyEntity* entity = m_pEntityMngr->GetEntity(a_uRBIndex);

	for (uint i = 0; i < m_EntityList.size(); i++)
	{
		// If the two entities being checked aren't the same and they're colliding, return true
		if (m_EntityList[i] != a_uRBIndex && entity->IsColliding(m_pEntityMngr->GetEntity(m_EntityList[i])))
		{
			return true;
		}
	}

	// If we make it here, we've checked every entity in the octant and haven't detected a collision, so there isn't one.
	return false;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (a_nIndex >= m_uOctantCount)
	{
		DisplayLeafs();
	}
	else
	{
		m_pRoot->m_lChild[a_nIndex]->Display();
	}
}

void MyOctant::Display(vector3 a_v3Color)
{
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(IDENTITY_M4, vector3(1.0f) * m_fSize), a_v3Color);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		if (!m_pChild[i]->m_EntityList.empty())
		{
			m_pChild[i]->DisplayLeafs(a_v3Color);
		}
	}

	Display();
}

void MyOctant::ClearEntityList()
{
	m_EntityList.clear();
}

void MyOctant::Subdivide()
{
	// If we're at the maximum level, there's no need to continue
	if (m_uLevel >= m_uMaxLevel)
	{
		return;
	}

	m_uChildren = 8;

	for (uint i = 0; i < m_uChildren; i++)
	{
		// Algorithmically determine the centers of the 8 children; NOTE! Max and Min are GLOBAL, not local!
		vector3 childCenter = vector3((m_v3Center.x - ((m_v3Max.x - m_v3Center.x) / 2.0f)) + ((m_v3Max.x - m_v3Center.x) * (i % 2)),
			(m_v3Center.y - ((m_v3Max.y - m_v3Center.y) / 2.0f)) + ((m_v3Max.y - m_v3Center.y) * ((i / 2) % 2)),
			(m_v3Center.z - ((m_v3Max.z - m_v3Center.z) / 2.0f)) + ((m_v3Max.z - m_v3Center.z) * ((i / 4) % 2)));

		m_pChild[i] = new MyOctant(childCenter, m_fSize / 2.0f);

		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_pRoot = m_pRoot;
	}
}

MyOctant* MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild >= m_uChildren)
	{
		return nullptr;
	}

	return m_pChild[a_nChild];
}

MyOctant* MyOctant::GetParent()
{
	return m_pParent;
}

bool MyOctant::IsLeaf()
{
	return m_uChildren == 0;
}

bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return m_EntityList.size() > a_nEntities;
}

void MyOctant::KillBranches()
{
	// Recursive function; Will traverse through all children, deleting every leaf, and then those leaves' parent (which would then be a leaf)
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();

		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}

	m_uChildren = 0;
}

void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	// If this function has been called, that means that the octant it was called on was the root! Subdivide and conquer!
	Subdivide();
	for (uint i = 0; i < m_uChildren; i++)
	{
		// Only subdivide again if the child contains entities and its level is less than our maximum!
		if (m_pChild[i]->ContainsMoreThan(0) && m_pChild[i]->m_uLevel < a_nMaxLevel)
		{
			m_pChild[i]->ConstructTree(a_nMaxLevel);
		}
	}

	if (this == m_pRoot)
	{
		ConstructList();
	}
}

void MyOctant::AssignIDtoEntity()
{
	// Puts every object in every octant inside of a dimension
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}

	// No sense in registering entities if this octant has children
	if (m_uChildren > 0)
	{
		return;
	}

	// But if it doesn't...
	else
	{
		for (uint i = 0; i < m_EntityList.size(); i++)
		{
			m_pEntityMngr->AddDimension(m_EntityList[i], m_uID);
		}
	}
}

uint MyOctant::GetOctantCount()
{
	return m_uOctantCount;
}

void MyOctant::Release()
{
	m_pParent = nullptr;
	KillBranches();

	m_pRoot = nullptr;

	m_lChild.clear();

	m_uOctantCount--;
}

void MyOctant::Init()
{
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_pMeshMngr = m_pMeshMngr->GetInstance();
	m_pEntityMngr = m_pEntityMngr->GetInstance();

	m_v3Center = vector3();
	m_v3Min = vector3();
	m_v3Max = vector3();

	m_pParent = nullptr;

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}

	m_pRoot = nullptr;

	m_uOctantCount++;
}

void MyOctant::ConstructList()
{
	// Traverses the tree and puts every leaf that contains entities in a list
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}

	// No sense in registering entities if this octant has children
	if (m_uChildren > 0)
	{
		return;
	}

	// But if it doesn't...
	else if(m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
