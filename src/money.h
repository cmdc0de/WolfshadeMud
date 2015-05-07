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
/*
*
*
*  Money
*
*/
#ifndef _MONEY_H_
#define _MONEY_H_

//storage class for money on characters
//must be a struct b/c saved
struct sMoney
{
private:
	base_money Base;
	int Plat, Gold, Silver, Copper;
protected:
	inline void ReCalcBase();
	//static
public:
	static CString m_strPlat;
	static CString m_strGold;
	static CString m_strSilver;
	static CString m_strCopper;
	static const short PLAT_IN_BASE;
	static const short GOLD_IN_BASE;
	static const short SILVER_IN_BASE;
	static bool IsCash(CString &str);
public:
	inline int RemoveCopper(int nCopper);
	inline int RemoveSilver(int nSilver);
	inline int RemoveGold(int nGold);
	inline int RemovePlat(int nPlat);
	inline void AddCopper(int nCopper);
	inline void AddSilver(int nSilver);
	inline void AddGold(int nGold);
	inline void AddPlat(int nPlat);
public:
	sMoney operator +(const sMoney &r);
	sMoney operator /(const int nNum);
	sMoney operator %(const int nNum);
	void FromString(CString &strMoney, sMoney &ToAdd, int nCoins);
	bool IsEmpty() {return Base==0;}
	sMoney & operator+=(const sMoney &money);
	void RoomString(CString &str);
	CString GetMoneyString();
	void IncreaseBy(short nPercent);
	sMoney & operator =(base_money lValue);
	sMoney & operator +=(base_money lValue);
	inline sMoney(sMoney &r);
	bool Take(int nCost) 
	{
		if(Base<nCost)
			return false;
		*this=(Base-=nCost);
		return true;
	}
	bool Take(sMoney &Cost) {return Take(Cost.GetBase());}
	base_money GetBase() {return Base;}
	sMoney(base_money b) {*this=b;}
	sMoney &operator=(const sMoney &r)
	{
		Plat = r.Plat;Gold = r.Gold;Silver = r.Silver;Copper = r.Copper;Base = r.Base;
		return (*this);
	}
	sMoney() {Base = Plat = Gold = Silver = Copper = 0;}
	int GetWeight() const {return Base/PLAT_IN_BASE;} //1 plat is 1 lb
};

inline void sMoney::ReCalcBase()
{
	Base = (Plat*PLAT_IN_BASE) + (Gold*GOLD_IN_BASE) + (Silver*SILVER_IN_BASE) + Copper;
}

inline int sMoney::RemoveCopper(int nCopper)
{
	nCopper = nCopper==-1 ? Copper : nCopper;
	if(Copper < nCopper)
	{
		CError Err(MESSAGE_ONLY,"You don't have that much copper!\r\n");
		throw &Err;
	}
	
	Copper = ((Copper-nCopper)>=0) ? Copper-nCopper : 0;
	ReCalcBase();
	return nCopper;
}

inline int sMoney::RemoveSilver(int nSilver)
{
	nSilver = nSilver==-1 ? Silver : nSilver;
	if(Silver < nSilver)
	{
		CError Err(MESSAGE_ONLY,"You don't have that much silver.\r\n");
		throw &Err;
	}
	Silver = ((Silver-nSilver)>=0) ? Silver-nSilver : 0;
	ReCalcBase();
	return nSilver;
}

inline int sMoney::RemoveGold(int nGold)
{
	nGold = nGold==-1 ? Gold : nGold;
	if(Gold < nGold)
	{
		CError Err(MESSAGE_ONLY,"You don't have that much gold.\r\n");
		throw &Err;
	}
	Gold = ((Gold-nGold)>=0) ? Gold-nGold : 0;
	ReCalcBase();
	return nGold;
}

inline int sMoney::RemovePlat(int nPlat)
{
	nPlat = nPlat==-1 ? Plat : nPlat;
	if(Plat < nPlat)
	{
		CError Err(MESSAGE_ONLY,"You don't have that much platinum.\r\n");
		throw &Err;
	}
	Plat = ((Plat-nPlat)>=0) ? Plat-nPlat : 0;
	ReCalcBase();
	return nPlat;
}

inline void sMoney::AddCopper(int nCopper)
{
	Copper += nCopper;
	ReCalcBase();
}

inline void sMoney::AddSilver(int nSilver)
{
	Silver+=nSilver;
	ReCalcBase();
}

inline void sMoney::AddGold(int nGold)
{
	Gold+=nGold;
	ReCalcBase();
}

inline void sMoney::AddPlat(int nPlat)
{
	Plat+=nPlat;
	ReCalcBase();
}

inline sMoney::sMoney(sMoney &r)
{
	*this = r;
}
#endif
