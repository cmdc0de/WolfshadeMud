/*
*
*  Racial Reference implementation
*
*
*/

#include "stdinclude.h"
#pragma hdrstop
#include "racial.reference.h"

extern CRandom GenNum;
const short CRacialReference::RACE_EVIL = 0;
const short CRacialReference::RACE_GOODIES = 1;
const short CRacialReference::RACE_NEUTRAL = 2;
const short CRacialReference::RACE_ALIEN = 3;
const short CRacialReference::RACE_NO_HASSLE = 4;

const char *CRacialReference::m_strEqPosNames[] = 
{
	"Worn on head       ", //0
	"Worn on horns      ", //1
	"Worn in ear        ", //2
	"Worn on eyes       ", //3
	"Worn on nose       ", //4
	"Worn on face       ", //5
	"Worn on neck       ", //6
	"Worn on body       ", //7
	"Worn on back       ", //8
	"Worn on arms       ", //9
	"Worn on wrist      ", //10
	"Worn on ring       ", //11
	"Wielded            ", //12
	"Dual wielded       ", //13
	"Wielded two handed ",//14 
	"Worn as shield     ", //15
	"Held               ", //16
	"Worn on waist      ", //17
	"Worn on tail       ", //18
	"Worn on legs       ", //19
	"Worn on feet       ", //20
	"Worn on hands      ", //21
	"Worn as a badge    ", //22
	"Worn on wings      ", //23
	"Worn about body    "  //24
};

CRacialReference::CRacialReference()
{
	//m_nTotalWeaponPositions = 0;
	m_nSize = NO_SIZE;
	m_lStartRoom = -1;
}

void CRacialReference::SetAll()
{
	SetAgility();
	SetCharisma();
	SetConstitution();
	SetInnateRace();
	SetPower();
	SetStrength();
	SetWisdom();
	SetIntellegence();
}

short int CRacialReference::GetStartingMoves(const short int *pStats) const
{
	int nMoves;
	switch(m_nSize)
	{
	case SIZE_GIANT:
		nMoves = 80;break;
	case SIZE_HUGE:
		nMoves = 70;break;
	case SIZE_LARGE:
		nMoves = 65;break;
	case SIZE_MEDIUM:
		nMoves = 60;break;
	case SIZE_SMALL:
		nMoves = 55;break;
	case SIZE_TINY:
		nMoves = 45;break;
	default:
		nMoves = -1;
		ErrorLog << "BadSize in GetStarting Moves!\r\n" << endl;
		return nMoves;
		break;
	}
	int nRandom = (pStats[STR] + pStats[STRADD] + pStats[POW])/3;
	return (nMoves + ROLL(nRandom,1));
}

short int CRacialReference::GetStartingMana(const short int *pStats) const 
{
	int nNum = (m_sPower[pStats[POW]/10].m_nManaAdjust + 
		m_sWisdom[pStats[WIS]/10].m_nManaAdjust + 
		m_sIntellegence[pStats[INT]/10].m_nManaAdjust)/3;
	return (nNum + DIE(5)+1);
}

CRacialReference::~CRacialReference()
{

}

short CRacialReference::GetEqPosIndex(UINT nWearBit) const
{
	switch(nWearBit)
	{
	case WEAR_POS_HEAD:
		return 0;
		break;
	case WEAR_POS_HORNS:
		return 1;
		break;
	case WEAR_POS_EAR:
		return 2;
		break;
	case WEAR_POS_EYES:
		return 3;
		break;
	case WEAR_POS_NOSE:
		return 4;
		break;
	case WEAR_POS_FACE:
		return 5;
		break;
	case WEAR_POS_NECK:
		return 6;
		break;
	case WEAR_POS_BODY:
		return 7;
		break;
	case WEAR_POS_ON_BACK:
		return 8;
		break;
	case WEAR_POS_ARMS:
		return 9;
		break;
	case WEAR_POS_WRIST:
		return 10;
		break;
	case WEAR_POS_RING:
		return 11;
		break;
	case WEAR_POS_WIELD:
		return 12;
		break;
	case WEAR_POS_DUAL_WIELD:
		return 13;
		break;
	case WEAR_POS_WIELD_TWO_HANDED:
		return 14;
		break;
	case WEAR_POS_SHIELD:
		return 15;
		break;
	case WEAR_POS_HOLD:
		return 16;
		break;
	case WEAR_POS_WAIST:
		return 17;
		break;
	case WEAR_POS_TAIL:
		return 18;
		break;
	case WEAR_POS_LEGS:
		return 19;
		break;
	case WEAR_POS_FEET:
		return 20;
		break;
	case WEAR_POS_HANDS:
		return 21;
		break;
	case WEAR_POS_BADGE:
		return 22;
		break;
	case WEAR_POS_WINGS:
		return 23;
		break;
	case WEAR_POS_ABOUT_BODY:
		return 24;
		break;
	default:
		return -1;
		break;
	}
}

/////////////////////////////////////
//	PutEqOn
//  Given a character the object and if they
//	are trying to wield or wear
//	Get run the array for the race and check it
//	with the Wearbits of the object
//	if they match we check to see if the character
//	already is wearing/wielding something in that
//	position.  The logic is built to know between wielding
//	and dual wielding.
//	written by: demetrius Comes
//////////////////////////////////////
bool CRacialReference::PutEqOn(CCharacter *pCh, CObject *pObj,bool bSendMsgToRoom) const
{
	CString strToChar;
	register int i;

	if((pObj->GetWearBit() & WEAR_POS_WIELD) 
		|| (pObj->GetWearBit() & WEAR_POS_DUAL_WIELD)
		|| (pObj->GetWearBit() & WEAR_POS_WIELD_TWO_HANDED)
		|| (pObj->GetWearBit() & WEAR_POS_SHIELD)
		|| (pObj->GetWearBit() & WEAR_POS_HOLD))
	{
		CList<int> BadPosLL;
		
		short nMaxWeaponPos = GetMaxWeapons(); //really just returns the number of hands the race has
		int nWeaponPos;
		bool bLastWas2h = false;
		for(i=0;i<nMaxWeaponPos;i++)
		{
			nWeaponPos = GetWeaponPosition(i+1);
			if(!pCh->GetEq(nWeaponPos).IsEmpty())
			{
				BadPosLL.Add(nWeaponPos);
				if(pCh->GetEq(nWeaponPos).GetObject()->GetWearBit() & WEAR_POS_WIELD_TWO_HANDED)
				{
					bLastWas2h = true;
				}
			}
			else
			{
				if(bLastWas2h 
					&& (!(m_nWearPositions[nWeaponPos] & WEAR_POS_WIELD_TWO_HANDED)))
				{
					BadPosLL.Add(nWeaponPos);
				}
				else
				{
					if((pObj->GetWearBit() & WEAR_POS_WIELD_TWO_HANDED)
						&& (i+1<nMaxWeaponPos)
						&& (m_nWearPositions[nWeaponPos] & WEAR_POS_WIELD_TWO_HANDED)
						&& !pCh->GetEq(nWeaponPos+1).IsEmpty())
					{
						BadPosLL.Add(nWeaponPos);
					}
				}
				bLastWas2h = false;
			}
		}
		bool bDual = false;
		for(i=0;i<nMaxWeaponPos;i++)
		{
			nWeaponPos = GetWeaponPosition(i+1);
			if(!pCh->GetEq(nWeaponPos).IsEmpty()
				&& (m_nWearPositions[nWeaponPos] & WEAR_POS_WIELD))
			{
				bDual = !bDual;
			}
			if((pObj->GetWearBit() & m_nWearPositions[nWeaponPos])
				&& !BadPosLL.DoesContain(nWeaponPos))
			{
				CString strHoldOrWield;
				int nWearBit;
				if((pObj->GetWearBit() & WEAR_POS_WIELD)
					&& (pObj->GetWearBit() & m_nWearPositions[nWeaponPos])
					&& !bDual)
				{
					strHoldOrWield = "wield";
					nWearBit = WEAR_POS_WIELD;
				}
				else if((pObj->GetWearBit() & WEAR_POS_DUAL_WIELD)
					&& (pObj->GetWearBit() & m_nWearPositions[nWeaponPos])
					&& bDual)
				{
					strHoldOrWield = "dual wield";
					nWearBit = WEAR_POS_DUAL_WIELD;
				}
				else if((pObj->GetWearBit() & WEAR_POS_WIELD_TWO_HANDED)
					&& (pObj->GetWearBit() & m_nWearPositions[nWeaponPos]))
				{
					strHoldOrWield = "wield two-handed";
					nWearBit = WEAR_POS_WIELD_TWO_HANDED;
				}
				else if((pObj->GetWearBit() & WEAR_POS_HOLD)
					&& (pObj->GetWearBit() & m_nWearPositions[nWeaponPos]))
				{
					strHoldOrWield = "hold";
					nWearBit = WEAR_POS_HOLD;
				}
				else if((pObj->GetWearBit() & WEAR_POS_SHIELD)
					&& (pObj->GetWearBit() & m_nWearPositions[nWeaponPos]))
				{
					strHoldOrWield = "wear";
					nWearBit = WEAR_POS_SHIELD;
				}
				else
				{
					ErrorLog << "CODE ERROR: last else in CRacialReference::puteqon" << endl;
					pCh->SendToChar("Error wearing this piece of eq.\r\n");
					return false;
				}
				pCh->WearEquipment(pObj,nWearBit,nWeaponPos,true);
				strToChar.Format("You %s %s.\r\n",
					(char *)strHoldOrWield,
					(char *)pObj->GetObjName(pCh));
				pCh->SendToChar(strToChar);
				if(bSendMsgToRoom)
				{
					strToChar.Format("%s %s %s.\r\n",
						"%s",
						(char *)strHoldOrWield,
						"%s");
					pCh->GetRoom()->SendToRoom(strToChar,pCh,pObj);
				}
				return true;
			}
		}
		pCh->SendToChar("You don't seem to have enough hands!\r\n");
	}
	else //regular eq
	{
		for(i=0;i<MAX_EQ_POS;i++)
		{
			if(pCh->GetEq(i).IsEmpty()
				&& (pObj->GetWearBit() & m_nWearPositions[i]))
			{
				pCh->WearEquipment(pObj,m_nWearPositions[i],i,true);
				strToChar.Format("You wear %s.\r\n",
					(char *)pObj->GetObjName(pCh));
				pCh->SendToChar(strToChar);
				if(bSendMsgToRoom)
				{
					pCh->GetRoom()->SendToRoom("%s wears %s.\r\n",pCh,pObj);
				}
				return true;
			}
		}
		pCh->SendToChar("How are you going to do that?\r\n");
	}
	return false;
}

////////////////////////////////
//	CalcTotalWeaponPositions()
//	should be called in each race constructor
//	simply count up how many possible weapons we can
//	wield
///////////////////////////////
void CRacialReference::CalcTotalWeaponPositions()
{
	m_nTotalWeaponPositions = 0;
	for(register int i=0;i<MAX_EQ_POS;i++)
	{
		if((m_nWearPositions[i] & WEAR_POS_WIELD) ||
			(m_nWearPositions[i] & WEAR_POS_DUAL_WIELD) ||
			(m_nWearPositions[i] & WEAR_POS_WIELD_TWO_HANDED))
		{
			m_nTotalWeaponPositions++;
		}
	}
}

//////////////////////////////
//	GetWeaponPosition
//	Given a index run though
//	the wear positions and return the
//	weapon position that is given
//	written by: Demetrius Comes
/////////////////////////////
short CRacialReference::GetWeaponPosition(short nPos) const
{
	assert(nPos>0);
	short nCounter = 1;
	for(register int i=0;i<MAX_EQ_POS;i++)
	{
		if((m_nWearPositions[i] & WEAR_POS_WIELD) ||
			(m_nWearPositions[i] & WEAR_POS_DUAL_WIELD) ||
			(m_nWearPositions[i] & WEAR_POS_WIELD_TWO_HANDED))
		{
			if(nCounter==nPos)
			{
				return i;
			}
			else
			{
				nCounter++;
			}
		}
	}
	ErrorLog << "Weapon positions couldn't be found!\r\n" << endl;
	return -1;
}

short int CRacialReference::CalcHeight() const 
{
	return (m_sInnateRace.m_nMinHeight + DIE(m_sInnateRace.m_nMaxHeight-m_sInnateRace.m_nMinHeight)+1);
}
short int CRacialReference::CalcWeight() const 
{
	return (m_sInnateRace.m_nMinWeight + DIE(m_sInnateRace.m_nMaxWeight-m_sInnateRace.m_nMinWeight)+1);
}

///////////////////////////////
//	Given a string we populate it with the innate abilites
void CRacialReference::PopulateInnateString(CString &strInnates)
{
	CString str("Non-Action innates: \r\n");
	if(HasInnate(INNATE_ULTRAVISION))
	{
		str+="Ultravision\r\n";
	}
	if(HasInnate(INNATE_INFRAVISION))
	{
		str+="Infravision\r\n";
	}
	if(HasInnate(INNATE_DAY_SIGHT))
	{
		str+="Day sight\r\n";
	}
	if(HasInnate(INNATE_MAGIC_RESISTANCE))
	{
		str+="Magical Resistance\r\n";
	}
	if(HasInnate(INNATE_REGENERATE))
	{
		str+="Regeneration\r\n";
	}
	str+="\r\nAction-able innates: \r\n";
	if(HasInnate(INNATE_FLY))
	{
		str+="Fly\r\n";
	}
	if(HasInnate(INNATE_STRENGTH))
	{
		str+="Strength\r\n";
	}
	if(HasInnate(INNATE_LARGE))
	{
		str+="Enlarge\r\n";
	}
	if(HasInnate(INNATE_BITE))
	{
		str+="Bite\r\n";
	}
	if(HasInnate(INNATE_FAERIE_FIRE))
	{
		str+="Faerie fire\r\n";
	}
	if(HasInnate(INNATE_INVIS))
	{
		str+="Invisibility\r\n";
	}
	strInnates=str;
}

//////////////////////////////////
//	Starting point for all innates
void CRacialReference::DoInnate(CCharacter *pCh, CString strSub) const
{
	CString strSubCom(strSub.GetWordAfter(0));
	if(strSubCom.IsEmpty())
	{
		pCh->SendToChar(this->GetInnates());
	}
	else if(strSubCom.Compare("fly") && HasInnate(INNATE_FLY))
	{
		if(!pCh->IsAffectedBy(CCharacter::AFFECT_INNATE_FLY_TIRED))
		{
			pCh->AddAffect(CCharacter::AFFECT_INNATE_FLY,((pCh->GetConstitution()/10)+1)*CMudTime::PULSES_PER_MUD_HOUR);
			pCh->AddAffect(CCharacter::AFFECT_INNATE_FLY_TIRED,CMudTime::PULSES_PER_MUD_DAY);
		}
		else
		{
			pCh->SendToChar("You're too tired.\r\n");
		}
	}
	else if(strSubCom.Compare("Strength") && HasInnate(INNATE_STRENGTH))
	{
		if(!pCh->IsAffectedBy(CCharacter::AFFECT_INNATE_STRENGTH_TIRED))
		{
			pCh->AddAffect(CCharacter::AFFECT_STRENGTH,((pCh->GetConstitution()/10)+1)*CMudTime::PULSES_PER_MUD_HOUR,pCh->GetConstitution()/25);
			pCh->AddAffect(CCharacter::AFFECT_INNATE_STRENGTH_TIRED,CMudTime::PULSES_PER_MUD_DAY);
		}
		else
		{
			pCh->SendToChar("You're too tired.\r\n");
		}
	}
	else if(strSubCom.Compare("bite") && HasInnate(INNATE_BITE))
	{
		if(pCh->IsAffectedBy(CCharacter::AFFECT_INNATE_BITE_TIRED))
		{
			pCh->SendToChar("You're too tired.\r\n");
		}
		else if(pCh->IsFighting() && !pCh->IsFighting()->IsDead())
		{
			if(DIE(200)>pCh->IsFighting()->GetAgility())
			{
				pCh->IsFighting()->TakeDamage(pCh,(DIE(pCh->GetLevel()+1)),true);
			}
			else
			{
				pCh->SendToChar("You bite at your opponent but miss.\r\n");
			}
			pCh->AddAffect(CCharacter::AFFECT_INNATE_BITE_TIRED,CMudTime::PULSES_PER_BATTLE_ROUND*2,0,false);
		}
		else
		{
			pCh->SendToChar("You not fighting anyone.\r\n");
		}
	}
	else if(strSubCom.Compare("faerie") && HasInnate(INNATE_FAERIE_FIRE))
	{
		CCharacter *pTarget = pCh->GetTarget(3,true);
		if(pCh->IsAffectedBy(CCharacter::AFFECT_INNATE_FAERIE_FIRE_TIRED))
		{
			pCh->SendToChar("You're too tired.\r\n");
		}
		else if(pTarget==NULL)
		{
			pCh->SendToChar("Who?\r\nsyntax: innate faerie fire <target>");
		}
		else
		{
			pTarget->AddAffect(CCharacter::AFFECT_FAERIE_FIRE,((pCh->GetConstitution()/50)+1)*CMudTime::PULSES_PER_MUD_HOUR,-7);
			pCh->AddFighters(pTarget,false);
			pCh->AddAffect(CCharacter::AFFECT_INNATE_FAERIE_FIRE_TIRED,CMudTime::PULSES_PER_MUD_HOUR*2);
		}
	}
	else if(strSubCom.Compare("invisibility") && HasInnate(INNATE_INVIS))
	{
		if(pCh->IsAffectedBy(CCharacter::AFFECT_INNATE_INVIS_TIRED))
		{
			pCh->SendToChar("You're too tired.\r\n");
		}
		else
		{
			pCh->AddAffect(CCharacter::AFFECT_INNATE_INVIS,((pCh->GetConstitution()/50)+1)*CMudTime::PULSES_PER_MUD_HOUR);
			pCh->AddAffect(CCharacter::AFFECT_INNATE_INVIS_TIRED,CMudTime::PULSES_PER_MUD_DAY);
		}
	}
	else
	{
		pCh->SendToChar("You don't have that innate ability.\r\n");
	}
}
