//  $Header: //depot/main/Civilization4/CvGameCoreDLL/CvStructs.cpp#3 $
//------------------------------------------------------------------------------------------------
//
//  ***************** CIV4 GAME ENGINE   ********************
//
//! \file		CvStructs.cpp
//! \author		Multiple
//! \brief		Implementation of basic Civ4 structures
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"

int EventTriggeredData::getID() const 
{ 
	return m_iId; 
}

void EventTriggeredData::setID(int iID) 
{ 
	m_iId = iID; 
}

void EventTriggeredData::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ(wrapper, "EventTriggeredData",&m_iId);
	WRAPPER_READ(wrapper, "EventTriggeredData",(int*)&m_eTrigger);
	m_eTrigger = (EventTriggerTypes)wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_EVENT_TRIGGERS, m_eTrigger, true);
	WRAPPER_READ(wrapper, "EventTriggeredData",&m_iTurn);
	WRAPPER_READ(wrapper, "EventTriggeredData",(int*)&m_ePlayer);
	WRAPPER_READ(wrapper, "EventTriggeredData",&m_iCityId);
	WRAPPER_READ(wrapper, "EventTriggeredData",&m_iPlotX);
	WRAPPER_READ(wrapper, "EventTriggeredData",&m_iPlotY);
	WRAPPER_READ(wrapper, "EventTriggeredData",&m_iUnitId);
	WRAPPER_READ(wrapper, "EventTriggeredData",(int*)&m_eOtherPlayer);
	WRAPPER_READ(wrapper, "EventTriggeredData",&m_iOtherPlayerCityId);
	
	//	Expiration was not stored in older saves (which didn;t store expired events for replay)
	//	so default to false if absent
	m_bExpired = false;
	WRAPPER_READ(wrapper, "EventTriggeredData",&m_bExpired);

	WRAPPER_READ_CLASS_ENUM(wrapper, "EventTriggeredData",REMAPPED_CLASS_TYPE_RELIGIONS,(int*)&m_eReligion);
	WRAPPER_READ_CLASS_ENUM(wrapper, "EventTriggeredData",REMAPPED_CLASS_TYPE_CORPORATIONS,(int*)&m_eCorporation);
	WRAPPER_READ_CLASS_ENUM(wrapper, "EventTriggeredData",REMAPPED_CLASS_TYPE_BUILDINGS,(int*)&m_eBuilding);
	WRAPPER_READ_STRING(wrapper, "EventTriggeredData",m_szText);
	WRAPPER_READ_STRING(wrapper, "EventTriggeredData",m_szGlobalText);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void EventTriggeredData::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_iId);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "EventTriggeredData", REMAPPED_CLASS_TYPE_EVENT_TRIGGERS, m_eTrigger);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_iTurn);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_ePlayer);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_iCityId);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_iPlotX);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_iPlotY);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_iUnitId);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_eOtherPlayer);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_iOtherPlayerCityId);
	WRAPPER_WRITE(wrapper, "EventTriggeredData", m_bExpired);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "EventTriggeredData", REMAPPED_CLASS_TYPE_RELIGIONS, m_eReligion);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "EventTriggeredData", REMAPPED_CLASS_TYPE_CORPORATIONS, m_eCorporation);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "EventTriggeredData", REMAPPED_CLASS_TYPE_BUILDINGS, m_eBuilding);
	WRAPPER_WRITE_STRING(wrapper, "EventTriggeredData", m_szText);
	WRAPPER_WRITE_STRING(wrapper, "EventTriggeredData", m_szGlobalText);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

int VoteSelectionData::getID() const 
{ 
	return iId; 
}

void VoteSelectionData::setID(int iID) 
{ 
	iId = iID; 
}

void VoteSelectionData::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ(wrapper, "VoteSelectionData",&iId);
	WRAPPER_READ_CLASS_ENUM(wrapper, "VoteSelectionData", REMAPPED_CLASS_TYPE_VOTE_SOURCES,(int*)&eVoteSource);
	int iSize = 0;
	WRAPPER_READ_DECORATED(wrapper, "VoteSelectionData",(unsigned int*)&iSize,"aVoteOption.size");

	if ( iSize < 50 )	//	Old bug left loaded size uninitialized so sanity check and ignore larger
	{
		for (int i = 0; i < iSize; ++i)
		{
			VoteSelectionSubData kData;
			WRAPPER_READ_CLASS_ENUM_DECORATED(wrapper, "VoteSelectionData",REMAPPED_CLASS_TYPE_VOTES,(int*)&kData.eVote, "voteOption.eVote");
			WRAPPER_READ_DECORATED(wrapper, "VoteSelectionData",(int*)&kData.ePlayer, "voteOption.ePlayer");
			WRAPPER_READ_DECORATED(wrapper, "VoteSelectionData",&kData.iCityId, "voteOption.iCityId");
			WRAPPER_READ_DECORATED(wrapper, "VoteSelectionData",(int*)&kData.eOtherPlayer, "voteOption.eOtherPlayer");
			WRAPPER_READ_STRING_DECORATED(wrapper, "VoteSelectionData",kData.szText, "voteOption.szText");
			aVoteOptions.push_back(kData);
		}
	}

	WRAPPER_READ_OBJECT_END(wrapper);
}

void VoteSelectionData::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "VoteSelectionData", iId);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "VoteSelectionData", REMAPPED_CLASS_TYPE_VOTE_SOURCES, eVoteSource);
	WRAPPER_WRITE_DECORATED(wrapper, "VoteSelectionData", aVoteOptions.size(),"aVoteOption.size");
	for (std::vector<VoteSelectionSubData>::iterator it = aVoteOptions.begin(); it != aVoteOptions.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "VoteSelectionData", REMAPPED_CLASS_TYPE_VOTES, (*it).eVote, "voteOption.eVote");
		WRAPPER_WRITE_DECORATED(wrapper, "VoteSelectionData", (*it).ePlayer, "voteOption.ePlayer");
		WRAPPER_WRITE_DECORATED(wrapper, "VoteSelectionData", (*it).iCityId, "voteOption.iCityId");
		WRAPPER_WRITE_DECORATED(wrapper, "VoteSelectionData", (*it).eOtherPlayer, "voteOption.eOtherPlayer");
		WRAPPER_WRITE_STRING_DECORATED(wrapper, "VoteSelectionData", (*it).szText, "voteOption.szText");
	}

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

int VoteTriggeredData::getID() const 
{ 
	return iId; 
}

void VoteTriggeredData::setID(int iID) 
{ 
	iId = iID; 
}

void VoteTriggeredData::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ(wrapper, "VoteTriggeredData",&iId);
	WRAPPER_READ_CLASS_ENUM(wrapper, "VoteTriggeredData", REMAPPED_CLASS_TYPE_VOTE_SOURCES,(int*)&eVoteSource);
	WRAPPER_READ_CLASS_ENUM(wrapper, "VoteTriggeredData", REMAPPED_CLASS_TYPE_VOTES,(int*)&kVoteOption.eVote);
	WRAPPER_READ(wrapper, "VoteTriggeredData",(int*)&kVoteOption.ePlayer);
	WRAPPER_READ(wrapper, "VoteTriggeredData",&kVoteOption.iCityId);
	WRAPPER_READ(wrapper, "VoteTriggeredData",(int*)&kVoteOption.eOtherPlayer);
	WRAPPER_READ_STRING(wrapper, "VoteTriggeredData",kVoteOption.szText);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void VoteTriggeredData::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "VoteTriggeredData", iId);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "VoteTriggeredData", REMAPPED_CLASS_TYPE_VOTE_SOURCES, eVoteSource);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "VoteTriggeredData", REMAPPED_CLASS_TYPE_VOTES, kVoteOption.eVote);
	WRAPPER_WRITE(wrapper, "VoteTriggeredData", kVoteOption.ePlayer);
	WRAPPER_WRITE(wrapper, "VoteTriggeredData", kVoteOption.iCityId);
	WRAPPER_WRITE(wrapper, "VoteTriggeredData", kVoteOption.eOtherPlayer);
	WRAPPER_WRITE_STRING(wrapper, "VoteTriggeredData", kVoteOption.szText);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void PlotExtraYield::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ(wrapper, "PlotExtraYield",&m_iX);
	WRAPPER_READ(wrapper, "PlotExtraYield",&m_iY);
	m_aeExtraYield.clear();
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		int iYield;
		WRAPPER_READ_DECORATED(wrapper, "PlotExtraYield",&iYield, "iYield");
		m_aeExtraYield.push_back(iYield);
	}

	WRAPPER_READ_OBJECT_END(wrapper);
}

void PlotExtraYield::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "PlotExtraYield", m_iX);
	WRAPPER_WRITE(wrapper, "PlotExtraYield", m_iY);
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "PlotExtraYield", m_aeExtraYield[i], "iYield");
	}

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void PlotExtraCost::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ(wrapper, "PlotExtraCost",&m_iX);
	WRAPPER_READ(wrapper, "PlotExtraCost",&m_iY);
	WRAPPER_READ(wrapper, "PlotExtraCost",&m_iCost);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void PlotExtraCost::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "PlotExtraCost", m_iX);
	WRAPPER_WRITE(wrapper, "PlotExtraCost", m_iY);
	WRAPPER_WRITE(wrapper, "PlotExtraCost", m_iCost);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void BuildingYieldChange::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ_CLASS_ENUM(wrapper, "BuildingYieldChange", REMAPPED_CLASS_TYPE_BUILDING_CLASSES,(int*)&eBuildingClass);
	WRAPPER_READ(wrapper, "BuildingYieldChange",(int*)&eYield);
	WRAPPER_READ(wrapper, "BuildingYieldChange",&iChange);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void BuildingYieldChange::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_CLASS_ENUM(wrapper, "BuildingYieldChange", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, eBuildingClass);
	WRAPPER_WRITE(wrapper, "BuildingYieldChange", eYield);
	WRAPPER_WRITE(wrapper, "BuildingYieldChange", iChange);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void BuildingCommerceChange::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ_CLASS_ENUM(wrapper, "BuildingCommerceChange",REMAPPED_CLASS_TYPE_BUILDING_CLASSES,(int*)&eBuildingClass);
	WRAPPER_READ(wrapper, "BuildingCommerceChange",(int*)&eCommerce);
	WRAPPER_READ(wrapper, "BuildingCommerceChange",&iChange);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void PropertySpawns::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// @SAVEBREAK REPLACE 31/8/2018
	// Replace this code at the next save break.
	// Backwards compat loading of prop spawns that weren't being remapped correctly
	WRAPPER_READ(wrapper, "PropertySpawns", (int*)&eProperty);
	if (eProperty == -2)
	{
		PropertyTypes eRemappedProperty;
		WRAPPER_READ_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_PROPERTIES, (int*)&eRemappedProperty);
		eProperty = eRemappedProperty;
		WRAPPER_READ_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_UNIT_CLASSES, (int*)&eUnitClass);
	}
	else
	{
		WRAPPER_READ(wrapper, "PropertySpawns", (int*)&eUnitClass);
	}
	// REPLACE WITH
	// WRAPPER_READ_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_PROPERTIES, (int*)& eProperty);
	// WRAPPER_READ_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_UNIT_CLASSES, (int*)& eUnitClass);
	// SAVEBREAK@

	WRAPPER_READ_OBJECT_END(wrapper);
}

void PropertySpawns::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	// @SAVEBREAK REPLACE 31/8/2018
	// Replace this code at the next save break.
	// Backwards compat loading of prop spawns that weren't being remapped correctly
	WRAPPER_WRITE_DECORATED(wrapper, "PropertySpawns", -2, "eProperty");
	PropertyTypes eRemappedProperty = eProperty;
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_PROPERTIES, eRemappedProperty);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_UNIT_CLASSES, eUnitClass);
	// REPLACE WITH
	// WRAPPER_WRITE_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_PROPERTIES, eRemappedProperty);
	// WRAPPER_WRITE_CLASS_ENUM(wrapper, "PropertySpawns", REMAPPED_CLASS_TYPE_UNIT_CLASSES, eUnitClass);
	// SAVEBREAK@

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void BuildingCommerceChange::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_CLASS_ENUM(wrapper, "BuildingCommerceChange", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, eBuildingClass);
	WRAPPER_WRITE(wrapper, "BuildingCommerceChange", eCommerce);
	WRAPPER_WRITE(wrapper, "BuildingCommerceChange", iChange);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

/************************************************************************************************/
/* Afforess	                  Start		 01/25/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void BuildingYieldModifier::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ_CLASS_ENUM(wrapper, "BuildingYieldModifier",REMAPPED_CLASS_TYPE_BUILDING_CLASSES,(int*)&eBuildingClass);
	WRAPPER_READ(wrapper, "BuildingYieldModifier",(int*)&eYield);
	WRAPPER_READ(wrapper, "BuildingYieldModifier",&iChange);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void BuildingYieldModifier::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_CLASS_ENUM(wrapper, "BuildingYieldModifier", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, eBuildingClass);
	WRAPPER_WRITE(wrapper, "BuildingYieldModifier", eYield);
	WRAPPER_WRITE(wrapper, "BuildingYieldModifier", iChange);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void BuildingCommerceModifier::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ_CLASS_ENUM(wrapper, "BuildingCommerceModifier",REMAPPED_CLASS_TYPE_BUILDING_CLASSES,(int*)&eBuildingClass);
	WRAPPER_READ(wrapper, "BuildingCommerceModifier",(int*)&eCommerce);
	WRAPPER_READ(wrapper, "BuildingCommerceModifier",&iChange);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void BuildingCommerceModifier::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_CLASS_ENUM(wrapper, "BuildingCommerceModifier", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, eBuildingClass);
	WRAPPER_WRITE(wrapper, "BuildingCommerceModifier", eCommerce);
	WRAPPER_WRITE(wrapper, "BuildingCommerceModifier", iChange);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


void checkBattleUnitType(BattleUnitTypes unitType)
{
	FAssertMsg((unitType >= 0) && (unitType < BATTLE_UNIT_COUNT), "[Jason] Invalid battle unit type.");
}

CvBattleRound::CvBattleRound() :
	m_iWaveSize(0),
	m_bRangedRound(false) 
{
	m_aNumKilled[BATTLE_UNIT_ATTACKER] = m_aNumKilled[BATTLE_UNIT_DEFENDER] = 0;
	m_aNumAlive[BATTLE_UNIT_ATTACKER] = m_aNumAlive[BATTLE_UNIT_DEFENDER] = 0;
}

bool CvBattleRound::isValid() const
{
	bool bValid = true;

	// Valid if no more than the wave size was killed, and didn't kill more attackers than were defenders or vv.
	bValid &= (m_aNumKilled[BATTLE_UNIT_ATTACKER] + m_aNumKilled[BATTLE_UNIT_DEFENDER] <= m_iWaveSize);
	bValid &= (m_aNumKilled[BATTLE_UNIT_ATTACKER] <= m_aNumAlive[BATTLE_UNIT_DEFENDER]);
	bValid &= (m_aNumKilled[BATTLE_UNIT_DEFENDER] <= m_aNumAlive[BATTLE_UNIT_ATTACKER]);
	return bValid;
}

bool CvBattleRound::isRangedRound() const
{
	return m_bRangedRound;
}

void CvBattleRound::setRangedRound(bool value)
{
	m_bRangedRound = value;
}

int CvBattleRound::getWaveSize() const
{
	return m_iWaveSize;
}

void CvBattleRound::setWaveSize(int size)
{
	m_iWaveSize = size;
}

int CvBattleRound::getNumKilled(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aNumKilled[unitType];
}

void CvBattleRound::setNumKilled(BattleUnitTypes unitType, int value)
{
	checkBattleUnitType(unitType);
	m_aNumKilled[unitType] = value;
}

void CvBattleRound::addNumKilled(BattleUnitTypes unitType, int increment)
{
	checkBattleUnitType(unitType);
	m_aNumKilled[unitType] += increment;
}

int CvBattleRound::getNumAlive(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aNumAlive[unitType];
}

void CvBattleRound::setNumAlive(BattleUnitTypes unitType, int value)
{
	checkBattleUnitType(unitType);
	m_aNumAlive[unitType] = value;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvMissionDefinition::CvMissionDefinition
//! \brief      Default constructor.
//------------------------------------------------------------------------------------------------
CvMissionDefinition::CvMissionDefinition() :
	m_fMissionTime(0.0f),
	m_eMissionType(NO_MISSION),
	m_pPlot(NULL)
{
	for(int i=0;i<BATTLE_UNIT_COUNT;i++)
		m_aUnits[i] = NULL;
}

MissionTypes CvMissionDefinition::getMissionType() const
{
	return m_eMissionType;
}

void CvMissionDefinition::setMissionType(MissionTypes missionType)
{
	m_eMissionType = missionType;
}

float CvMissionDefinition::getMissionTime() const
{
	return m_fMissionTime;
}

void CvMissionDefinition::setMissionTime(float time)
{
	m_fMissionTime = time;
}

CvUnit *CvMissionDefinition::getUnit(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aUnits[unitType];
}

void CvMissionDefinition::setUnit(BattleUnitTypes unitType, CvUnit *unit)
{
	checkBattleUnitType(unitType);
	m_aUnits[unitType] = unit;
}

const CvPlot *CvMissionDefinition::getPlot() const
{
	return m_pPlot;
}

void CvMissionDefinition::setPlot(const CvPlot *plot)
{
	m_pPlot = plot;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvBattleDefinition::CvBattleDefinition
//! \brief      Constructor.
//------------------------------------------------------------------------------------------------
CvBattleDefinition::CvBattleDefinition() 
	: CvMissionDefinition()
	, m_bAdvanceSquare(false)
	, m_iNumMeleeRounds(0)
	, m_iNumRangedRounds(0)
{
	m_fMissionTime = 0.0f;
	m_eMissionType = MISSION_BEGIN_COMBAT;

	for(int i=0;i<BATTLE_UNIT_COUNT;i++)
	{
		m_aUnits[i] = NULL;
		m_aFirstStrikes[i] = 0;
		for(int j=0;j<BATTLE_TIME_COUNT;j++)
			m_aDamage[i][j] = 0;
	}
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvBattleDefinition::CvBattleDefinition
//! \brief      Copy constructor
//! \param      kCopy The object to copy
//------------------------------------------------------------------------------------------------
CvBattleDefinition::CvBattleDefinition( const CvBattleDefinition & kCopy ) :
	m_bAdvanceSquare( kCopy.m_bAdvanceSquare )
{
	m_fMissionTime = kCopy.m_fMissionTime;
	m_eMissionType = MISSION_BEGIN_COMBAT;
	m_iNumMeleeRounds = kCopy.m_iNumMeleeRounds;
	m_iNumRangedRounds = kCopy.m_iNumRangedRounds;

	for(int i=0;i<BATTLE_UNIT_COUNT;i++)
	{
		m_aUnits[i] = kCopy.m_aUnits[i];
		m_aFirstStrikes[i] = kCopy.m_aFirstStrikes[i];
		for(int j=0;j<BATTLE_TIME_COUNT;j++)
			m_aDamage[i][j] = kCopy.m_aDamage[i][j];
	}

	m_aBattleRounds.assign(kCopy.m_aBattleRounds.begin(), kCopy.m_aBattleRounds.end());
}

CvBattleDefinition::~CvBattleDefinition() {}

int CvBattleDefinition::getDamage(BattleUnitTypes unitType, BattleTimeTypes timeType) const
{
	checkBattleUnitType(unitType);
	checkBattleTimeType(timeType);
	return m_aDamage[unitType][timeType];
}

void CvBattleDefinition::setDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int damage)
{
	checkBattleUnitType(unitType);
	checkBattleTimeType(timeType);
	m_aDamage[unitType][timeType] = damage;
}

void CvBattleDefinition::addDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int increment)
{
	checkBattleUnitType(unitType);
	checkBattleTimeType(timeType);
	m_aDamage[unitType][timeType] += increment;
}

int CvBattleDefinition::getFirstStrikes(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aFirstStrikes[unitType];
}

void CvBattleDefinition::setFirstStrikes(BattleUnitTypes unitType, int firstStrikes)
{
	checkBattleUnitType(unitType);
	m_aFirstStrikes[unitType] = firstStrikes;
}

void CvBattleDefinition::addFirstStrikes(BattleUnitTypes unitType, int increment)
{
	checkBattleUnitType(unitType);
	m_aFirstStrikes[unitType] += increment;
}

bool CvBattleDefinition::isAdvanceSquare() const
{
	return m_bAdvanceSquare;
}

void CvBattleDefinition::setAdvanceSquare(bool advanceSquare)
{
	m_bAdvanceSquare = advanceSquare;
}

int CvBattleDefinition::getNumRangedRounds() const
{
	return m_iNumRangedRounds;
}

void CvBattleDefinition::setNumRangedRounds(int count)
{
	m_iNumRangedRounds = count;
}

void CvBattleDefinition::addNumRangedRounds(int increment)
{
	m_iNumRangedRounds += increment;
}

int CvBattleDefinition::getNumMeleeRounds() const
{
	return m_iNumMeleeRounds;
}

void CvBattleDefinition::setNumMeleeRounds(int count)
{
	m_iNumMeleeRounds = count;
}

void CvBattleDefinition::addNumMeleeRounds(int increment)
{
	m_iNumMeleeRounds += increment;
}

int CvBattleDefinition::getNumBattleRounds() const
{
	return m_aBattleRounds.size();
}

void CvBattleDefinition::clearBattleRounds()
{
	m_aBattleRounds.clear();
}

CvBattleRound &CvBattleDefinition::getBattleRound(int index)
{
	checkBattleRound(index);
	return m_aBattleRounds[index];
}

const CvBattleRound &CvBattleDefinition::getBattleRound(int index) const
{
	checkBattleRound(index);
	return m_aBattleRounds[index];
}

void CvBattleDefinition::addBattleRound(const CvBattleRound &round)
{
	m_aBattleRounds.push_back(round);
}

void CvBattleDefinition::setBattleRound(int index, const CvBattleRound &round)
{
	m_aBattleRounds.assign(index, round);
}

void CvBattleDefinition::checkBattleTimeType(BattleTimeTypes timeType) const
{
	FAssertMsg((timeType >= 0) && (timeType < BATTLE_TIME_COUNT), "[Jason] Invalid battle time type.");
}

void CvBattleDefinition::checkBattleRound(int index) const
{
	FAssertMsg((index >= 0) && (index < (int)m_aBattleRounds.size()), "[Jason] Invalid battle round index.");
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvAirMissionDefinition::CvAirMissionDefinition
//! \brief      Constructor
//------------------------------------------------------------------------------------------------
CvAirMissionDefinition::CvAirMissionDefinition() :
	CvMissionDefinition()
{
	m_fMissionTime = 0.0f;
	m_eMissionType = MISSION_AIRPATROL;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvAirMissionDefinition::CvAirMissionDefinition
//! \brief      Copy constructor
//! \param      kCopy The object to copy
//------------------------------------------------------------------------------------------------
CvAirMissionDefinition::CvAirMissionDefinition( const CvAirMissionDefinition & kCopy )
{
	m_fMissionTime = kCopy.m_fMissionTime;
	m_eMissionType = kCopy.m_eMissionType;
	m_pPlot = kCopy.m_pPlot;

	for(int i=0;i<BATTLE_UNIT_COUNT;i++)
	{
		m_aDamage[i] = kCopy.m_aDamage[i];
		m_aUnits[i] = kCopy.m_aUnits[i];
	}
}

int CvAirMissionDefinition::getDamage(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aDamage[unitType];
}

void CvAirMissionDefinition::setDamage(BattleUnitTypes unitType, int damage)
{
	checkBattleUnitType(unitType);
	m_aDamage[unitType] = damage;
}

bool CvAirMissionDefinition::isDead(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	FAssertMsg(getUnit(unitType) != NULL, "[Jason] Invalid battle unit type.");
	if(getDamage(unitType) >= getUnit(unitType)->maxHitPoints())
		return true;
	else
		return false;
}

PBGameSetupData::PBGameSetupData()
	: iSize(0)
	, iClimate(0)
	, iSeaLevel(0)
	, iSpeed(0)
	, iEra(0)
	, iMaxTurns(0)
	, iCityElimination(0)
	, iAdvancedStartPoints(0)
	, iTurnTime(0)
	, iNumCustomMapOptions(0)
	, aiCustomMapOptions(NULL)
	, iNumVictories(0)
	, abVictories(NULL)
{
	for (int i = 0; i < NUM_GAMEOPTION_TYPES; i++)
	{
		abOptions.push_back(false);
	}
	for (int i = 0; i < NUM_MPOPTION_TYPES; i++)
	{
		abMPOptions.push_back(false);
	}
}

