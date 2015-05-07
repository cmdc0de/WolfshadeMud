//////////////////////////////////////////////////////////////
//Wolfshade MUD server
//Copyright (C) 1999 Demetrius and John Comes
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////
//	guiding: if you practice a skill the bit type will be set in the m_nSkillSphere variable

//CProficiency will be a static member of all PC-Classes
#ifndef _PROFICIENCY_H_
#define _PROFICIENCY_H_
#define TOTAL_SKILL_SPHERES 15

class CProficiency
{
private:
	struct sProficiencyInfo
	{
	public:
		CString m_strSkillSetName;
		short m_nTier,
			m_nBonus,
			m_nPenalty;
		int m_nValue,
			m_nParentValue;
	public:
		sProficiencyInfo(CString strName, short nTier, short nBonus, short nPenalty, UINT nValue, UINT nParentValue)
		{
			m_strSkillSetName = strName;
			m_nTier = nTier;
			m_nBonus = nBonus;
			m_nPenalty = nPenalty;
			m_nValue = nValue;
			m_nParentValue = nParentValue;
		}
	};
protected:
	CLinkList<sProficiencyInfo> m_ProficiencyLL;
	short m_nTotalTiers;
public:
	CString GetSingleProficiencyName(int nValueOfTier)
	{
		POSITION pos = m_ProficiencyLL.GetStartPosition();
		sProficiencyInfo *pProf;
		while(pos)
		{
			pProf = m_ProficiencyLL.GetNext(pos);
			if((pProf->m_nValue & nValueOfTier)==nValueOfTier)
			{
				return pProf->m_strSkillSetName;
			}
		}
		return "Invalid";
	}
	void GetProficiencyNames(CString &str)
	{
		POSITION pos = m_ProficiencyLL.GetStartPosition();
		sProficiencyInfo *pProf;
		while(pos)
		{
			pProf = m_ProficiencyLL.GetNext(pos);
			str.Format("%s %-30s %-2d\r\n",
				(char *)str,
				(char *)pProf->m_strSkillSetName,
				pProf->m_nTier);
		}
	}
	void GetSkillSets(int nSkillSphere, CString &str, bool bAddReturn=true)
	{
		POSITION pos = m_ProficiencyLL.GetStartPosition();
		sProficiencyInfo *pProf;
		while(pos)
		{
			pProf = m_ProficiencyLL.GetNext(pos);
			if(pProf->m_nValue & nSkillSphere)
			{
				str.Format("%s %s%s",
					(char *)str,
					(char *)pProf->m_strSkillSetName,
					(bAddReturn ? "\r\n" : ""));
			}
		}
	}
	int GetNeededPriorSkillKnowledge(const CSkillInfo *pSkillYouWantToLearn)
	{
		POSITION pos = m_ProficiencyLL.GetStartPosition();
		sProficiencyInfo *pProf;
		while(pos)
		{
			pProf = m_ProficiencyLL.GetNext(pos);
			if(pSkillYouWantToLearn->IsOfSet(pProf->m_nValue))
			{
				return pProf->m_nParentValue;
			}
		}
		assert(0);
		return -1;
	}
	CProficiency() 
	{
		m_nTotalTiers = 0;
		m_ProficiencyLL.SetDeleteData();
	}
	virtual ~CProficiency(){;}
	void Add(CString strName, short nTier, short nBonus, short nPenatly, UINT nValue, UINT nParentValue)
	{
		m_nTotalTiers = (nTier > m_nTotalTiers) ? nTier : m_nTotalTiers;
		m_ProficiencyLL.Add(new sProficiencyInfo(strName,nTier,nBonus,nPenatly,nValue,nParentValue));
	}
	//nSpheres should be m_nSkillSpheres!
	short CalcBonus(UINT nSpheres)
	{
		short TotalBonus = 0;
		bool TierUsed[TOTAL_SKILL_SPHERES] = {false};
		sProficiencyInfo *pProf;
		POSITION pos = m_ProficiencyLL.GetStartPosition();
		while(pos)
		{
			pProf = m_ProficiencyLL.GetNext(pos);
			if((nSpheres & pProf->m_nValue) 
				&& !TierUsed[pProf->m_nTier])
			{
				TierUsed[pProf->m_nTier] = true;
				TotalBonus += pProf->m_nBonus;
			}
			else if((nSpheres & pProf->m_nValue) 
				&& TierUsed[pProf->m_nTier])
			{
				TotalBonus -= pProf->m_nPenalty;
			}
		}
		return TotalBonus;
	}
};
#endif