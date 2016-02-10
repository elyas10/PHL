#pragma once

#include "../PHLMemory.h"

const enum PlayerStatCode
{
	LEVEL = 0xC,
	ARMOR = 0xF4,
	HEALTH = 0xF8,
	HEALTH_REGEN = 0xFB, // Divide by 60 to get actual regen
	MANA_REGEN = 0xFC, // Divide by 60 to get actual regen
	EVASION = 0x11D,
	MAIN_HAND_FIRE_DAMAGE_LOW = 0x10E,
	MAIN_HAND_FIRE_DAMAGE_HIGH = 0x10F,
	MAIN_HAND_LIGHTNING_DAMAGE_LOW = 0x110,
	MAIN_HAND_LIGHTNING_DAMAGE_HIGH = 0x111,
	MAIN_HAND_PHYSICAL_DAMAGE_LOW = 0x118,
	MAIN_HAND_PHYSICAL_DAMAGE_HIGH = 0x119,
	MAIN_HAND_ACCURACY_RATING_LOW = 0x125,
	MAIN_HAND_ACCURACY_RATING_HIGH = 0x126,
	MAIN_HAND_ATTACK_SPEED_MODIFIER = 0x127,
	STRENGTH = 0x243,
	INTELLIGENCE = 0x246,
	DEXTERITY = 0x249,
	MAIN_HAND_CRITICAL_STRIKE_MULTIPLIER_LOW = 0x267,
	MAIN_HAND_CRITICAL_STRIKE_MULTIPLIER_HIGH = 0x268,
	MAIN_HAND_TOTAL_COMBINED_DAMAGE_LOW = 0x2A7,
	MAIN_HAND_TOTAL_COMBINED_DAMAGE_HIGH = 0x2A8,
	CHANCE_TO_HIT = 0x2AD,
	FIRE_RESISTANCE = 0x83E,
	COLD_RESISTANCE = 0x83F,
	LIGHTNING_RESISTANCE = 0x840,

	PLAYER_STAT_CODE_END
};

class PHLPlayer : public PHLHack<PHLPlayer>
{
	PHLPlayer ();
public:
	friend class PHLHack<PHLPlayer>;
	void printAddr () override;

	// Get basic player info
	int * getPlayerStatsPtr () const;
	int getPlayerStat (PlayerStatCode code) const;

	Addr getPlayerDefenseAddr () const;
	int getPlayerDefense (int offset) const;
	int getCurHealth () const;
	int getMaxHealth () const;
	int getCurMana () const;
	int getMaxMana () const;
	int getCurES () const;
	int getMaxES () const;

	// Cool
	Addr mouseX;
	Addr mouseY;
	Addr baseStruct;
	Addr gameStruct;
	Addr playerStruct;
};