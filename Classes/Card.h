#ifndef _CARD_H_
#define _CARD_H_

#include <string>
#include <vector>
#include "cocos2d.h"

using cocos2d::Sprite;
using cocos2d::Label;

class Card
{
friend class Attendant;
public:
	Card() = default;

	//卡牌序号，费用，卡牌类型，攻击力，防御力，言灵数组，战术能力数组
	Card(int num,int fe,bool type,int att,int def, const std::vector<std::string> &dama, std::vector<bool> &tac);

	Card(const Card &C);

	std::string getCardRes();

	//卡牌序号
	int number = 0;
	//对应精灵
	Sprite *cardSprite = nullptr;

	//卡牌费用
	int fee = 0;
	//卡牌类型：真-随从，假-法术
	bool cardType = true;

	//攻击力
	int attackPower = 0;
	//防御力
	int defensePower = 0;

	//卡牌的言灵，它使用字符串数组记录一个或者多个卡牌的能力
	//每个字符串都由5个字符组成
	//第一个字符代表能力种类：A-造成伤害，B-嘲讽，C-冲锋，D-治疗，E-消灭，F-抽牌，G-改变攻击力
	//第二个字符表示造成伤害/治疗/消灭/改变攻击力的波及范围：1-指定，2-所有，0-无
	//第三个字符表示造成伤害/治疗/消灭/改变攻击力的承受对象阵营：1-敌方，2-我方，3-所有，0-无
	//第四个字符表示造成伤害/治疗/消灭/改变攻击力的承受对象身份：1-随从，2-英雄，3-所有，0-无
	//第五个字符表示造成伤害/治疗/抽牌/改变攻击力的程度：用数字表示。
	//
	//举例：比如25号卡牌，技能是“为一个指定的随从或英雄回复8点生命，并抽2张牌”，言灵数组内就是这样的->{"D1338","F0002"}
	std::vector<std::string> kotodama;

	//战术能力数组，数组内有5个元素，它们依次代表：有无解场能力，有无治疗能力，有无抽牌能力, 有无嘲讽能力 和 有无抢脸能力
	std::vector<bool> tactics;
};

class Attendant
{
public:
	Attendant() = default;

	Attendant(Card &c,int numAtt);

	//序号
	int number = 0;
	//攻击力
	int attackPower = 0;
	//最大防御力
	int maxDefensePower = 0;
	//现在的防御力
	int nowDefensePower = 0;

	//行动能力
	bool canUse = false;
	//有无嘲讽
	bool isShield = false;

	//对应精灵
	Sprite *attSprite = nullptr;

	//攻击力标识
	Label *lLabel = nullptr;
	//防御力标识
	Label *rLabel = nullptr;
};

#endif