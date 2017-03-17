#include "Card.h"
#include <sstream>

USING_NS_CC;
using namespace std;

extern double visibleSizeW;
extern double visibleSizeH;

extern double originX;
extern double originY;

Card::Card(int num, int fe, bool type, int att, int def, const vector<string> &dama, vector<bool> &tac)
{
	number = num;
	fee = fe;
	cardType = type;
	attackPower = att;
	defensePower = def;

	kotodama = dama;
	tactics = tac;
}

Card::Card(const Card &C)
{
	number = C.number;
	fee = C.fee;
	cardType = C.cardType;

	attackPower = C.attackPower;
	defensePower = C.defensePower;

	kotodama=C.kotodama;
	tactics=C.tactics;
    
	cardSprite = C.cardSprite;
}

string Card::getCardRes()
{
	stringstream ss;
	string s;
	ss << number; ss >> s; ss.clear(); s += ".png"; s = "Cards/" + s;
	return s;
}

Attendant::Attendant(Card &c,int numAtt)
{
	number = c.number;
	attackPower = c.attackPower;

	maxDefensePower = c.defensePower;
    nowDefensePower = c.defensePower;

	canUse = false;
	isShield = false;

	for (const auto &s:c.kotodama)
	{
		if (s[0]=='C')
		{
			canUse = true;
		}
		else if (s[0]=='B')
		{
			isShield = true;
		}
	}

	string s;
	stringstream ss;
	ss << number; ss >> s; s += ".png"; ss.clear();
	s = "Attendants/" + s;

	//对应精灵
	attSprite = Sprite::create(s.c_str());
	attSprite->setPosition(Vec2(138+numAtt*146,(originY+visibleSizeH)/4+60));

	TTFConfig ttfConfig("Fonts/ARLRDBD.ttf", 50);

	s.clear();
	ss << attackPower; ss >> s; ss.clear();
	//攻击力标识
	lLabel = Label::createWithTTF(ttfConfig, s.c_str(), TextHAlignment::CENTER);
	lLabel->setPosition(Vec2(95 + numAtt * 146, (originY + visibleSizeH) / 4-25));
	lLabel->setColor(Color3B(0, 0, 0));

	s.clear();
	ss << maxDefensePower; ss >> s; ss.clear();
	//防御力标识
	rLabel = Label::createWithTTF(ttfConfig, s.c_str(), TextHAlignment::CENTER);
	rLabel->setPosition(Vec2(180 + numAtt * 146, (originY + visibleSizeH) / 4-25));
	rLabel->setColor(Color3B(0, 0xFF, 0));
}