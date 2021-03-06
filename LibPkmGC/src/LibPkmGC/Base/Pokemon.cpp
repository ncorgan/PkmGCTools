/*
* Copyright (C) TuxSH 2015
* This file is part of LibPkmGC.
*
* LibPkmGC is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* LibPkmGC is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with LibPkmGC.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <LibPkmGC/Base/Pokemon.h>
#include <string>

namespace LibPkmGC {


namespace Base {

void Pokemon::updateNamesLanguage(void) {
	OTName->setCharset(version.language == Japanese);
	name->setCharset(version.language == Japanese);
}

u16 Pokemon::calculateStat(size_t statIndex, PokemonSpeciesIndex species, PokemonNatureIndex natureIndex, u8 level, u8 IV, u8 EV){
	static const u16 n[3] = { 110, 100, 90 };

	if ((statIndex == 0) && (species == Shedinja)) return 1;
	if (statIndex >= 6) return 0;

	size_t i = statIndex;
	u16 outStat;

	outStat = IV + (2 * getSpeciesData(species).baseStats[i]) + EV / 4;
	
	// We're applying bonuses now
	if(i == 0) return ((outStat + 100) * level / 100) + 10; // HP
	else return (outStat * level / 100 + 5) * n[getNatureStatAffinity(natureIndex, i)] / 100;

}

void Pokemon::calculateStats(PokemonSpeciesIndex species, PokemonNatureIndex natureIndex, u8 level, const u8 IVs[6], const u8 EVs[6], u16 outStats[6]) {
	for (size_t i = 0; i < 6; ++i) 
		outStats[i] = calculateStat(i, species, natureIndex, level, IVs[i], EVs[i]);
}

u8 Pokemon::calculateLevelFromExp(PokemonSpeciesIndex species, u32 experience) {
	const u32* expTable = getSpeciesExpTable(species);
	if (experience >= expTable[100]) return 100;

	u8 i = 0;
	while (i < 100 && experience >= expTable[++i]);
	return i-1;
}

u32 Pokemon::calculateExpFromLevel(PokemonSpeciesIndex species, u8 level) {
	const u32* expTable = getSpeciesExpTable(species);
	if (level >= 100) return expTable[100];
	return expTable[level];
}

u32 Pokemon::fixExperienceProportionally(PokemonSpeciesIndex oldSpecies, u32 oldExp, PokemonSpeciesIndex newSpecies) {
	// Rule of 3
	u8 l = calculateLevelFromExp(oldSpecies, oldExp);
	const u32 *oldT = LibPkmGC::getSpeciesExpTable(oldSpecies), *newT = LibPkmGC::getSpeciesExpTable(newSpecies);
	if (l == 100) return newT[100];
	return newT[l] + (newT[l + 1] - newT[l])*(oldExp - oldT[l]) / (oldT[l + 1] - oldT[l]);

}

Pokemon::Pokemon(size_t inSize, const u8* inData) : DataStruct(inSize, inData)  {
}

Pokemon::~Pokemon(void) {
	Pokemon::deleteFields();
}


u8 Pokemon::calculateLevelFromExp(void) const {
	return calculateLevelFromExp(species, experience);
}

void Pokemon::updateLevelFromExp(void) { partyData.level = calculateLevelFromExp(); }

u32 Pokemon::calculateExpFromLevel(u8 lvl) const {
	return calculateExpFromLevel(species, lvl);
}

void Pokemon::updateExpFromLevel(u8 lvl) { experience = calculateExpFromLevel(lvl); }

u32 Pokemon::fixExperienceProportionally(PokemonSpeciesIndex newSpecies){
	return fixExperienceProportionally(species, experience, newSpecies);
}

PokemonSpeciesData Pokemon::getThisSpeciesData(void) const {
	return LibPkmGC::getSpeciesData(species);
}

const u32 * Pokemon::getExpTable(void) const {
	return LibPkmGC::getSpeciesExpTable(species);
}

void Pokemon::calculateStats(u16 outStats[6]) const {
	calculateStats(species, (PokemonNatureIndex)(PID % 25), calculateLevelFromExp(), IVs, EVs, outStats);
}

void Pokemon::updateStats(void) { calculateStats(partyData.stats); }

bool Pokemon::isShiny(u32 PID, u16 TID, u16 SID) {
	return ((PID >> 16) ^ (PID & 0xffff) ^ SID ^ TID) < 8;
}

PokemonNatureIndex Pokemon::getNature(u32 PID) {
	return (PokemonNatureIndex)(PID % 25);
}

Gender Pokemon::getGender(PokemonSpeciesIndex species, u32 PID) {
	PokemonSpeciesGenderRatio r = getSpeciesData(species).genderRatio;
	if (r == GenderlessOnly) return Genderless;
	else if (r == MaleOnly) return Male;
	else if (r == FemaleOnly) return Female;
	else
		return ((PID & 0xff) >= (u32)r) ? Male : Female;
}

PokemonSpeciesIndex Pokemon::getWurmpleEvolution(u32 PID) {
	return (((PID & 0xffff) % 10) < 5) ? Silcoon : Cascoon;
}

char Pokemon::getUnownForm(u32 PID) {
	char tbl[] = "ABCDEFGHIKLMNOPQRSTUVWXYZ?!";
	u32 n = (((PID >> 24) & 3) << 6) | (((PID >> 16) & 3) << 4) | (((PID >> 8) & 3) << 2) | (PID & 3);
	return tbl[n % 28];
}

std::pair<TypeIndex, u8> Pokemon::getHiddenPowerTypeAndPower(const u8 IVs[6]) {
	const TypeIndex T[] = {
		Fighting,
		Flying,
		Poison,
		Ground,
		Rock,
		Bug,
		Ghost,
		Steel,
		Fire,
		Water,
		Grass,
		Electric,
		Psychic,
		Ice,
		Dragon,
		Dark,
	};
	u32 tval = 0, pval = 0;

	for (size_t i = 0; i < 6; ++i) {
		tval |= (IVs[GBAStatsOrder[i]] & 1) << i;
		pval |= ((IVs[GBAStatsOrder[i]] >> 1) & 1) << i;
	}

	tval = (tval * 15) / 63;
	pval = ((pval * 40) / 63) + 30;
	return std::pair<TypeIndex, u8>(T[tval], (u8) pval);
}

bool Pokemon::isShiny(void) const {
	return isShiny(PID, TID, SID);
}

PokemonNatureIndex Pokemon::getNature(void) const {
	return getNature(PID);
}

Gender Pokemon::getGender(void) const {
	return getGender(species, PID);
}

PokemonSpeciesIndex Pokemon::getWurmpleEvolution(void) const {
	return getWurmpleEvolution(PID);
}

char Pokemon::getUnownForm(void) const {
	return getUnownForm(PID);
}

std::pair<TypeIndex, u8> Pokemon::getHiddenPowerTypeAndPower(void) const {
	return getHiddenPowerTypeAndPower(IVs);
}

void Pokemon::normalizepokerus(void) {
	u8 st = pokerusStatus & 0xf, dr = pokerusStatus >> 4;
	if (st == 0) {
		dr = 0;
		partyData.pokerusDaysRemaining = -1;
	}
	else {
		u8 mx = 1 + (st & 0x3);
		dr = (dr > mx) ? mx : dr;
		partyData.pokerusDaysRemaining = (partyData.pokerusDaysRemaining > mx) ? mx : partyData.pokerusDaysRemaining;
	}

	pokerusStatus = (dr << 4) | st;
}

void Pokemon::normalizeStatus(void) {
	partyData.status = (partyData.status != NoStatus && partyData.status < Poisoned && partyData.status > Asleep) ? NoStatus : partyData.status;
	if (partyData.status != Asleep) partyData.turnsOfSleepRemaining = 0;
	if (partyData.status != BadlyPoisoned) partyData.turnsOfBadPoison = 0;
	partyData.turnsOfSleepRemaining = ((u8)partyData.turnsOfSleepRemaining > 7) ? 7 : partyData.turnsOfSleepRemaining;
	partyData.turnsOfBadPoison = ((u8)partyData.turnsOfBadPoison > 15) ? 15 : partyData.turnsOfBadPoison;
}

void Pokemon::resetPartyData(void) {
	updateStats();
	updateLevelFromExp();
	partyData.status = NoStatus;
	partyData.currentHP = partyData.stats[0];
	partyData.pokerusDaysRemaining = ((pokerusStatus & 0xf0) != 0) ? pokerusStatus & 0xf : -1;
}
Pokemon::Pokemon(Pokemon const & other) : Base::DataStruct(other), OTName(NULL), name(NULL){
	copyNonVirtual(other);
}
void Pokemon::deleteFields(void){
	delete OTName;
	delete name;
}


PokemonAbilityIndex Pokemon::getAbility(void) const {
	const PokemonSpeciesData dt = getThisSpeciesData();
	return (hasSecondAbility()) ? dt.possibleAbilities[1] : dt.possibleAbilities[0];
}

bool Pokemon::isEmptyOrInvalid(void) const {
	return (species >= Bonsly) || (species == NoSpecies) || !getSpeciesData(species).isValid || version.isIncomplete();
}

bool Pokemon::isSecondAbilityDefined(void) const{
	return (getSpeciesData(species).possibleAbilities[1] != NoAbility);
}
void Pokemon::swap(Pokemon& other) {
	DataStruct::swap(other);
	SW(species);
	SW(heldItem);

	SW(friendship);
	SW(locationCaught);
	SW(ballCaughtWith);
	SW(levelMet);
	SW(OTGender);

	SW(version);
	updateNamesLanguage();
	other.updateNamesLanguage();
	if (OTName->isGBA() == other.OTName->isGBA())
		SW(OTName);
	else {
		std::string s1(OTName->toUTF8());
		OTName->fromUTF8(other.OTName->toUTF8());
		other.OTName->fromUTF8(s1.c_str());
	}
	if (name->isGBA() == other.name->isGBA())
		SW(name);
	else {
		std::string s1(name->toUTF8());
		name->fromUTF8(other.name->toUTF8());
		other.name->fromUTF8(s1.c_str());
	}
	SW(contestLuster);
	SW(pokerusStatus);
	SW(markings);

	SW(experience);
	SW(SID);
	SW(TID);
	SW(PID);

	SW(GCUnk);


	SW(partyData);

	SW_ARRAY(specialRibbons, 12);
	SW(unimplementedRibbons);

	SW_ARRAY(moves, 4);
	SW_ARRAY(EVs, 6);
	SW_ARRAY(IVs, 6);

	SW_ARRAY(contestStats, 5);
	SW_ARRAY(contestAchievements, 5);

	SW(obedient);
	SW(unk1);
	SW(unk2);

	bool e1 = isEgg(), e2 = other.isEgg();
	setEggFlag(e2); other.setEggFlag(e1);

	bool a1 = hasSecondAbility(), a2 = other.hasSecondAbility();
	setSecondAbilityFlag(a2); setSecondAbilityFlag(a1);

	bool i1 = isMarkedAsInvalid(), i2 = other.isMarkedAsInvalid();
	setInvalidPokemonFlag(i2); setInvalidPokemonFlag(i1);


}

void Pokemon::copyNonVirtual(Pokemon const& other) {
	//Pokemon::deleteFields(); nope, don't do that
	CP(species);
	CP(heldItem);

	CP(friendship);
	CP(locationCaught);
	CP(ballCaughtWith);
	CP(levelMet);
	CP(OTGender);
	CP(contestLuster);
	CP(pokerusStatus);
	CP(markings);

	CP(experience);
	CP(SID);
	CP(TID);
	CP(PID);

	CP(GCUnk);
	CP(version);

	CP_ARRAY(specialRibbons, 12);
	CP(unimplementedRibbons);

	CP_ARRAY(moves, 4);
	CP_ARRAY(EVs, 6);
	CP_ARRAY(IVs, 6);

	CP(partyData);
	CP_ARRAY(contestStats, 5);
	CP_ARRAY(contestAchievements, 5);

	CP(obedient);
	CP(unk1);
	CP(unk2);
}

Pokemon & Pokemon::operator=(Pokemon const & other){
	if (this != &other) {
		Base::DataStruct::operator=(other);
		copyNonVirtual(other);
		updateNamesLanguage();
		*OTName = *(other.OTName);
		*name = *(other.name);
		setEggFlag(other.isEgg());
		setSecondAbilityFlag(other.hasSecondAbility());
		setInvalidPokemonFlag(other.isMarkedAsInvalid());
	}
	return *this;
}


}
}