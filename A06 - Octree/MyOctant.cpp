#include "MyOctant.h"

using namespace Simplex;

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
}

MyOctant::MyOctant(MyOctant const & other)
{
}

MyOctant & MyOctant::operator=(MyOctant const & other)
{
	// TODO: insert return statement here
}

MyOctant::~MyOctant()
{
}

void MyOctant::Swap(MyOctant & other)
{
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
	return false;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	
}

void MyOctant::Display(vector3 a_v3Color)
{
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center), a_v3Color, RENDER_SOLID);
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
	if (m_uLevel == m_uMaxLevel)
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
	}

	// If we've made it this far, we're at a leaf; delete the current octant
	Release();
}

void MyOctant::ConstructTree(uint a_nMaxLevel = 3)
{

}

void MyOctant::AssignIDtoEntity()
{

}

uint MyOctant::GetOctantCount()
{

}

void MyOctant::Release()
{

}

void MyOctant::Init()
{

}

void MyOctant::ConstructList()
{

}
