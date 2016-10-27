/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "random.h"

CRandom::CRandom() {
   m_data = 0;
   srand((unsigned) time(NULL));
}

int CRandom::Roll(unsigned int dice) {
   return(rand() % dice) + 1;
}

int CRandom::Roll(unsigned int dice, unsigned int Times) {
   m_data = 0;
   for (unsigned short int i = 0; i < Times; i++) {
	m_data += (rand() % dice) + 1;
   }
   return m_data;
}
