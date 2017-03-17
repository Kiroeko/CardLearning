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

	//������ţ����ã��������ͣ������������������������飬ս����������
	Card(int num,int fe,bool type,int att,int def, const std::vector<std::string> &dama, std::vector<bool> &tac);

	Card(const Card &C);

	std::string getCardRes();

	//�������
	int number = 0;
	//��Ӧ����
	Sprite *cardSprite = nullptr;

	//���Ʒ���
	int fee = 0;
	//�������ͣ���-��ӣ���-����
	bool cardType = true;

	//������
	int attackPower = 0;
	//������
	int defensePower = 0;

	//���Ƶ����飬��ʹ���ַ��������¼һ�����߶�����Ƶ�����
	//ÿ���ַ�������5���ַ����
	//��һ���ַ������������ࣺA-����˺���B-����C-��棬D-���ƣ�E-����F-���ƣ�G-�ı乥����
	//�ڶ����ַ���ʾ����˺�/����/����/�ı乥�����Ĳ�����Χ��1-ָ����2-���У�0-��
	//�������ַ���ʾ����˺�/����/����/�ı乥�����ĳ��ܶ�����Ӫ��1-�з���2-�ҷ���3-���У�0-��
	//���ĸ��ַ���ʾ����˺�/����/����/�ı乥�����ĳ��ܶ�����ݣ�1-��ӣ�2-Ӣ�ۣ�3-���У�0-��
	//������ַ���ʾ����˺�/����/����/�ı乥�����ĳ̶ȣ������ֱ�ʾ��
	//
	//����������25�ſ��ƣ������ǡ�Ϊһ��ָ������ӻ�Ӣ�ۻظ�8������������2���ơ������������ھ���������->{"D1338","F0002"}
	std::vector<std::string> kotodama;

	//ս���������飬��������5��Ԫ�أ��������δ������޽ⳡ�����������������������޳�������, ���޳������� �� ������������
	std::vector<bool> tactics;
};

class Attendant
{
public:
	Attendant() = default;

	Attendant(Card &c,int numAtt);

	//���
	int number = 0;
	//������
	int attackPower = 0;
	//��������
	int maxDefensePower = 0;
	//���ڵķ�����
	int nowDefensePower = 0;

	//�ж�����
	bool canUse = false;
	//���޳���
	bool isShield = false;

	//��Ӧ����
	Sprite *attSprite = nullptr;

	//��������ʶ
	Label *lLabel = nullptr;
	//��������ʶ
	Label *rLabel = nullptr;
};

#endif