#include "MyOctant.h"

using namespace Simplex;

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	Init();
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	Init();
}

MyOctant::MyOctant(MyOctant const & other)
{
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

	// If this octant has children, they need to be destroyed first, otherwise they will be inaccesible!
	KillBranches();

	for (uint i = 0; i < other.m_uChildren; i++)
	{
		m_pChild[i] = other.m_pChild[i];
		m_uChildren++;
	}

	ClearEntityList();
	for (uint i = 0; i < other.m_EntityList.size(); i++)
	{
		m_EntityList.push_back(other.m_EntityList[i]);
	}
	
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
	if (m_uID == a_nIndex)
	{
		Display(a_v3Color);
	}
}

void MyOctant::Display(vector3 a_v3Color)
{
	m_pMeshMngr->AddMeshToRenderList(m_pMeshMngr->GenerateCube(m_fSize, a_v3Color), glm::translate(m_v3Center), RENDER_SOLID);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		if (m_pChild[i]->m_EntityList.size() > 0)
		{
			m_pChild[i]->Display(a_v3Color);
		}
	}
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
		m_pChild[i] = new MyOctant(vector3(), m_fSize / 2.0f);

		m_uOctantCount++;

		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
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
		m_pChild[i]->Release();
	}

	m_uChildren = 0;
}

void MyOctant::ConstructTree(uint a_nMaxLevel = 3)
{
	// If this function has ben called, that means that the octant it was called on was the root! Subdivide and conquer! Look up how an octree works! XD

}

void MyOctant::AssignIDtoEntity()
{

}

uint MyOctant::GetOctantCount()
{
	return m_uOctantCount;
}

void MyOctant::Release()
{
	m_pParent = nullptr;
	KillBranches();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}

	m_pRoot = nullptr;

	m_lChild.clear();

	m_uOctantCount--;
}

void MyOctant::Init()
{
	m_uOctantCount++;
}

void MyOctant::ConstructList()
{

}
