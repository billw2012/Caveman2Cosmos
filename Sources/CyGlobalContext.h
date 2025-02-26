#pragma once

#ifndef CyGlobalContext_h
#define CyGlobalContext_h

//
// Python wrapper class for global vars and fxns
// Passed to Python
//

#include "CvGlobals.h"
#include "CvArtFileMgr.h"

class CyGame;
class CyMap;
class CyPlayer;
class CvRandom;
class CyEngine;
class CyTeam;
class CyArtFileMgr;
class CyUserProfile;
class CyVariableSystem;

class CyGlobalContext
{
public:
	CyGlobalContext();
	virtual ~CyGlobalContext();

	static CyGlobalContext& getInstance();		// singleton accessor

	bool isDebugBuild() const;
	CyGame* getCyGame() const;
	CyMap* getCyMap() const;

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	void switchMap(int iMap);
	int getNumMapInfos() const;
	CvMapInfo* getMapInfo(int iMap) const;
	int getNumMapSwitchInfos() const;
	CvMapSwitchInfo* getMapSwitchInfo(int iMapSwitch) const;
	CyMap* getMapByIndex(int iIndex);
	void initializeMap(int iMap);
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/	
	void setIsInPedia(bool isInPedia);
	bool isShiftDown() const;
	bool isAltDown() const;
	bool isCtrlDown() const;
	CyPlayer* getCyPlayer(int idx);
	CyPlayer* getCyActivePlayer();
	CvRandom& getCyASyncRand() const;
	CyTeam* getCyTeam(int i);
	CyArtFileMgr* getCyArtFileMgr() const;

	CvEffectInfo* getEffectInfo(int i) const;
	CvTerrainInfo* getTerrainInfo(int i) const;
	CvBonusClassInfo* getBonusClassInfo(int i) const;
	CvBonusInfo* getBonusInfo(int i) const;
	CvFeatureInfo* getFeatureInfo(int i) const;
	CvCivilizationInfo* getCivilizationInfo(int idx) const;
	CvLeaderHeadInfo* getLeaderHeadInfo(int i) const;
	CvTraitInfo* getTraitInfo(int i) const;
	CvUnitInfo* getUnitInfo(int i) const;
	CvSpecialUnitInfo* getSpecialUnitInfo(int i) const;
	CvYieldInfo* getYieldInfo(int i) const;
	CvCommerceInfo* getCommerceInfo(int i) const;
	CvRouteInfo* getRouteInfo(int i) const;
	CvImprovementInfo* getImprovementInfo(int i) const;
	CvGoodyInfo* getGoodyInfo(int i) const;
	CvBuildInfo* getBuildInfo(int i) const;
	CvHandicapInfo* getHandicapInfo(int i) const;
	CvGameSpeedInfo* getGameSpeedInfo(int i) const;
	CvTurnTimerInfo* getTurnTimerInfo(int i) const;
	CvBuildingClassInfo* getBuildingClassInfo(int i) const;
	CvMissionInfo* getMissionInfo(int i) const;
	CvCommandInfo* getCommandInfo(int i) const;
	CvAutomateInfo* getAutomateInfo(int i) const;
	CvActionInfo* getActionInfo(int i) const;
	CvUnitClassInfo* getUnitClassInfo(int i) const;
	CvInfoBase* getUnitCombatInfo(int i) const;
	//TB Promotion Line Mod begin
	CvPromotionLineInfo* getPromotionLineInfo(int i) const;
	//TB Promotion Line Mod end
	CvMapCategoryInfo* getMapCategoryInfo(int i) const;
	CvIdeaClassInfo* getIdeaClassInfo(int i) const;
	CvIdeaInfo* getIdeaInfo(int i) const;
	//CvTraitOptionEditsInfo* getTraitOptionEditsInfo(int i) const;
	CvInfoBase* getDomainInfo(int i) const;
	CvBuildingInfo* getBuildingInfo(int i) const;
	CvCivicOptionInfo* getCivicOptionInfo(int i) const;
	CvCivicInfo* getCivicInfo(int i) const;
	CvDiplomacyInfo* getDiplomacyInfo(int i) const;
	CvProjectInfo* getProjectInfo(int i) const;
	CvVoteInfo* getVoteInfo(int i) const;
	CvProcessInfo* getProcessInfo(int i) const;
	CvSpecialistInfo* getSpecialistInfo(int i) const;
	CvReligionInfo* getReligionInfo(int i) const;
	CvCorporationInfo* getCorporationInfo(int i) const;
	CvControlInfo* getControlInfo(int i) const;
	CvTechInfo* getTechInfo(int i) const;
	CvSpecialBuildingInfo* getSpecialBuildingInfo(int i) const;
	CvPromotionInfo* getPromotionInfo(int i) const;
	CvAnimationPathInfo * getAnimationPathInfo(int i) const;
	CvEmphasizeInfo * getEmphasizeInfo(int i) const;
	CvUpkeepInfo * getUpkeepInfo(int i) const;
	CvCultureLevelInfo * getCultureLevelInfo(int i) const;
	CvEraInfo * getEraInfo(int i) const;
	CvVictoryInfo * getVictoryInfo(int i) const;
	CvWorldInfo * getWorldInfo(int i) const;
	CvClimateInfo * getClimateInfo(int i) const;
	CvSeaLevelInfo * getSeaLevelInfo(int i) const;
	CvInfoBase * getUnitAIInfo(int i) const;
	CvColorInfo* getColorInfo(int i) const;
    CvUnitArtStyleTypeInfo* getUnitArtStyleTypeInfo(int i) const;
	CvPropertyInfo* getPropertyInfo(int i) const;

	int getInfoTypeForString(const char* szInfoType) const;
/************************************************************************************************/
/* Afforess	                  Start		 03/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getInfoTypeForStringWithHiddenAssert(const char* szInfoType) const;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	int getTypesEnum(const char* szType) const;

	int getNumPlayerColorInfos() const { return GC.getNumPlayerColorInfos(); }
	CvPlayerColorInfo* getPlayerColorInfo(int i) const;

	CvInfoBase* getHints(int i) const;
	CvMainMenuInfo* getMainMenus(int i) const;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 02/19/08                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// Python Modular Loading
	CvPythonModulesInfo* getPythonModulesInfo(int i) const;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	CvInvisibleInfo* getInvisibleInfo(int i) const;
	CvVoteSourceInfo* getVoteSourceInfo(int i) const;
	CvInfoBase* getAttitudeInfo(int i) const;
	CvInfoBase* getMemoryInfo(int i) const;
	CvInfoBase* getConceptInfo(int i) const;
	CvInfoBase* getNewConceptInfo(int i) const;
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
	// Dale - DCM: Pedia Concepts START
	CvInfoBase* getDCMConceptInfo(int i) const;
	// Dale - DCM: Pedia Concepts END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/*Afforess                                     11/13/09                                         */
/************************************************************************************************/
	CvInfoBase* getANDConceptInfo(int i) const;
/************************************************************************************************/
/* Afforess                                END                                                  */
/************************************************************************************************/
	CvInfoBase* getCityTabInfo(int i) const;
	CvInfoBase* getCalendarInfo(int i) const;
	CvInfoBase* getGameOptionInfo(int i) const;
	CvInfoBase* getMPOptionInfo(int i) const;
	CvInfoBase* getForceControlInfo(int i) const;
	CvInfoBase* getSeasonInfo(int i) const;
	CvInfoBase* getMonthInfo(int i) const;
	CvInfoBase* getDenialInfo(int i) const;
	CvQuestInfo* getQuestInfo(int i) const;
	CvTutorialInfo* getTutorialInfo(int i) const;
	CvEventTriggerInfo* getEventTriggerInfo(int i) const;
	CvEventInfo* getEventInfo(int i) const;
	CvEspionageMissionInfo* getEspionageMissionInfo(int i) const;
	CvHurryInfo* getHurryInfo(int i) const;
	CvPlayerOptionInfo* getPlayerOptionInfo(int i) const;
	CvPlayerOptionInfo* getPlayerOptionsInfoByIndex(int i) const;

	CvGraphicOptionInfo* getGraphicOptionInfo(int i) const;
	CvGraphicOptionInfo* getGraphicOptionsInfoByIndex(int i) const;

	// ArtInfos
	CvArtInfoInterface* getInterfaceArtInfo(int i) const;
	CvArtInfoMovie* getMovieArtInfo(int i) const;
	CvArtInfoMisc* getMiscArtInfo(int i) const;
	CvArtInfoUnit* getUnitArtInfo(int i) const;
	CvArtInfoBuilding* getBuildingArtInfo(int i) const;
	CvArtInfoCivilization* getCivilizationArtInfo(int i) const;
	CvArtInfoLeaderhead* getLeaderheadArtInfo(int i) const;
	CvArtInfoBonus* getBonusArtInfo(int i) const;
	CvArtInfoImprovement* getImprovementArtInfo(int i) const;
	CvArtInfoTerrain* getTerrainArtInfo(int i) const;
	CvArtInfoFeature* getFeatureArtInfo(int i) const;


	// Structs

	const char* getEntityEventTypes(int i) const { return GC.getEntityEventTypes((EntityEventTypes) i); }
	const char* getAnimationOperatorTypes(int i) const { return GC.getAnimationOperatorTypes((AnimationOperatorTypes) i); }
	const char* getFunctionTypes(int i) const { return GC.getFunctionTypes((FunctionTypes) i); }
	const char* getFlavorTypes(int i) const { return GC.getFlavorTypes((FlavorTypes) i); }
	const char* getArtStyleTypes(int i) const { return GC.getArtStyleTypes((ArtStyleTypes) i); }
	const char* getCitySizeTypes(int i) const { return GC.getCitySizeTypes(i); }
	const char* getContactTypes(int i) const { return GC.getContactTypes((ContactTypes) i); }
	const char* getDiplomacyPowerTypes(int i) const { return GC.getDiplomacyPowerTypes((DiplomacyPowerTypes) i); }
	const char *getFootstepAudioTypes(int i) { return GC.getFootstepAudioTypes(i); }
	const char *getFootstepAudioTags(int i) { return GC.getFootstepAudioTags(i); }

	int getNumEffectInfos() const { return GC.getNumEffectInfos(); }
	int getNumTerrainInfos() const { return GC.getNumTerrainInfos(); }
	int getNumSpecialBuildingInfos() const { return GC.getNumSpecialBuildingInfos(); }
	int getNumBonusInfos() const { return GC.getNumBonusInfos(); };
	int getNumPlayableCivilizationInfos() const { return GC.getNumPlayableCivilizationInfos(); }
	int getNumCivilizatonInfos() const { return GC.getNumCivilizationInfos(); }
	int getNumLeaderHeadInfos() const { return GC.getNumLeaderHeadInfos(); }
	int getNumTraitInfos() const { return GC.getNumTraitInfos(); }
	int getNumUnitInfos() const { return GC.getNumUnitInfos(); }
	int getNumSpecialUnitInfos() const { return GC.getNumSpecialUnitInfos(); }
	int getNumRouteInfos() const { return GC.getNumRouteInfos(); }
	int getNumFeatureInfos() const { return GC.getNumFeatureInfos(); }
	int getNumImprovementInfos() const { return GC.getNumImprovementInfos(); }
	int getNumGoodyInfos() const { return GC.getNumGoodyInfos(); }
	int getNumBuildInfos() const { return GC.getNumBuildInfos(); }
	int getNumHandicapInfos() const { return GC.getNumHandicapInfos(); }
	int getNumGameSpeedInfos() const { return GC.getNumGameSpeedInfos(); }
	int getNumTurnTimerInfos() const { return GC.getNumTurnTimerInfos(); }
	int getNumBuildingClassInfos() const { return GC.getNumBuildingClassInfos(); }
	int getNumBuildingInfos() const { return GC.getNumBuildingInfos(); }
	int getNumUnitClassInfos() const { return GC.getNumUnitClassInfos(); }
	int getNumUnitCombatInfos() const { return GC.getNumUnitCombatInfos(); }
	//TB Promotion Line Mod begin
	int getNumPromotionLineInfos() const { return GC.getNumPromotionLineInfos(); }
	//TB Promotion Line Mod end
	int getNumMapCategoryInfos() const { return GC.getNumMapCategoryInfos(); }
	int getNumIdeaClassInfos() const { return GC.getNumIdeaClassInfos(); }
	int getNumIdeaInfos() const { return GC.getNumIdeaInfos(); }
	//int getNumTraitOptionEditsInfos() const { return GC.getNumTraitOptionEditsInfos(); }
	int getNumAutomateInfos() const { return GC.getNumAutomateInfos(); }
	int getNumCommandInfos() const { return GC.getNumCommandInfos(); }
	int getNumControlInfos() const { return GC.getNumControlInfos(); }
	int getNumMissionInfos() const { return GC.getNumMissionInfos(); }
	int getNumActionInfos() const { return GC.getNumActionInfos(); }
	int getNumPromotionInfos() const { return GC.getNumPromotionInfos(); }
	int getNumTechInfos() const { return GC.getNumTechInfos(); }
	int getNumReligionInfos() const { return GC.getNumReligionInfos(); }
	int getNumCorporationInfos() const { return GC.getNumCorporationInfos(); }
	int getNumSpecialistInfos() const { return GC.getNumSpecialistInfos(); }
	int getNumCivicInfos() const { return GC.getNumCivicInfos(); }
	int getNumDiplomacyInfos() const { return GC.getNumDiplomacyInfos(); }
	int getNumCivicOptionInfos() const { return GC.getNumCivicOptionInfos(); }
	int getNumProjectInfos() const { return GC.getNumProjectInfos(); }
	int getNumVoteInfos() const { return GC.getNumVoteInfos(); }
	int getNumProcessInfos() const { return GC.getNumProcessInfos(); }
	int getNumEmphasizeInfos() const { return GC.getNumEmphasizeInfos(); }
	int getNumHurryInfos() const { return GC.getNumHurryInfos(); }
	int getNumUpkeepInfos() const { return GC.getNumUpkeepInfos(); }
	int getNumCultureLevelInfos() const { return GC.getNumCultureLevelInfos(); }
	int getNumEraInfos() const { return GC.getNumEraInfos(); }
	int getNumVictoryInfos() const { return GC.getNumVictoryInfos(); }
	int getNumWorldInfos() const { return GC.getNumWorldInfos(); }
	int getNumSeaLevelInfos() const { return GC.getNumSeaLevelInfos(); }
	int getNumClimateInfos() const { return GC.getNumClimateInfos(); }
	int getNumConceptInfos() const { return GC.getNumConceptInfos(); }
	int getNumNewConceptInfos() const { return GC.getNumNewConceptInfos(); }
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
	// Dale - DCM: Pedia Concepts START
	int getNumDCMConceptInfos() const { return GC.getNumDCMConceptInfos(); }
	// Dale - DCM: Pedia Concepts END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/*Afforess                                     11/13/09                                         */
/************************************************************************************************/
	int getNumANDConceptInfos() const { return GC.getNumANDConceptInfos(); }
/************************************************************************************************/
/* Afforess                                END                                                  */
/************************************************************************************************/
	int getNumCityTabInfos() const { return GC.getNumCityTabInfos(); }
	int getNumCalendarInfos() const { return GC.getNumCalendarInfos(); }
	int getNumPlayerOptionInfos() const { return GC.getNumPlayerOptionInfos(); }
	int getNumGameOptionInfos() const { return GC.getNumGameOptionInfos(); }
	int getNumMPOptionInfos() const { return GC.getNumMPOptionInfos(); }
	int getNumForceControlInfos() const { return GC.getNumForceControlInfos(); }
	int getNumSeasonInfos() const { return GC.getNumSeasonInfos(); }
	int getNumMonthInfos() const { return GC.getNumMonthInfos(); }
	int getNumDenialInfos() const { return GC.getNumDenialInfos(); }
	int getNumQuestInfos() const { return GC.getNumQuestInfos(); }
	int getNumTutorialInfos() const { return GC.getNumTutorialInfos(); }
	int getNumEventTriggerInfos() const { return GC.getNumEventTriggerInfos(); }
	int getNumEventInfos() const { return GC.getNumEventInfos(); }
	int getNumEspionageMissionInfos() const { return GC.getNumEspionageMissionInfos(); }
	int getNumHints() const { return GC.getNumHints(); }
	int getNumMainMenus() const { return GC.getNumMainMenus(); }
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 02/19/08                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// Python Modular Loading
	int getNumPythonModulesInfos() const { return GC.getNumPythonModulesInfos(); }
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	int getNumInvisibleInfos() const { return GC.getNumInvisibleInfos(); }
	int getNumVoteSourceInfos() const { return GC.getNumVoteSourceInfos(); }

	// ArtInfos
	int getNumInterfaceArtInfos() const { return ARTFILEMGR.getNumInterfaceArtInfos(); }
	int getNumMovieArtInfos() const { return ARTFILEMGR.getNumMovieArtInfos(); }
	int getNumMiscArtInfos() const { return ARTFILEMGR.getNumMiscArtInfos(); }
	int getNumUnitArtInfos() const { return ARTFILEMGR.getNumUnitArtInfos(); }
	int getNumBuildingArtInfos() const { return ARTFILEMGR.getNumBuildingArtInfos(); }
	int getNumCivilizationArtInfos() const { return ARTFILEMGR.getNumCivilizationArtInfos(); }
	int getNumLeaderheadArtInfos() const { return ARTFILEMGR.getNumLeaderheadArtInfos(); }
	int getNumImprovementArtInfos() const { return ARTFILEMGR.getNumImprovementArtInfos(); }
	int getNumBonusArtInfos() const { return ARTFILEMGR.getNumBonusArtInfos(); }
	int getNumTerrainArtInfos() const { return ARTFILEMGR.getNumTerrainArtInfos(); }
	int getNumFeatureArtInfos() const { return ARTFILEMGR.getNumFeatureArtInfos(); }
	int getNumAnimationPathInfos() const { return GC.getNumAnimationPathInfos(); }
	int getNumAnimationCategoryInfos() const { return GC.getNumAnimationCategoryInfos(); }
    int getNumUnitArtStyleTypeInfos() const { return GC.getNumUnitArtStyleTypeInfos(); }


	int getNumEntityEventTypes() const { return GC.getNumEntityEventTypes(); }
	int getNumAnimationOperatorTypes() const { return GC.getNumAnimationOperatorTypes(); }
	int getNumArtStyleTypes() const { return GC.getNumArtStyleTypes(); }
	int getNumFlavorTypes() const { return GC.getNumFlavorTypes(); }
	int getNumCitySizeTypes() const { return GC.getNumCitySizeTypes(); }
	int getNumFootstepAudioTypes() const { return GC.getNumFootstepAudioTypes(); }

	int getNumPropertyInfos() const { return GC.getNumPropertyInfos(); }

	//////////////////////
	// Globals Defines
	//////////////////////

	CyVariableSystem* getCyDefinesVarSystem();
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   08/02/07                            MRGENIE          */
/*                                                                                              */
/* Return true/false from                                                                       */
/************************************************************************************************/
	bool getDefineBOOL( const char * szName ) const { return GC.getDefineBOOL( szName ); }
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   END                                                  */
/************************************************************************************************/
	int getDefineINT( const char * szName ) const { return GC.getDefineINT( szName ); }
	float getDefineFLOAT( const char * szName ) const { return GC.getDefineFLOAT( szName ); }
	const char * getDefineSTRING( const char * szName ) const { return GC.getDefineSTRING( szName ); }
	void setDefineINT( const char * szName, int iValue ) { return GC.setDefineINT( szName, iValue ); }
	void setDefineFLOAT( const char * szName, float fValue ) { return GC.setDefineFLOAT( szName, fValue ); }
	void setDefineSTRING( const char * szName, const char * szValue ) { return GC.setDefineSTRING( szName, szValue ); }
/************************************************************************************************/
/* Mod Globals    Start                          09/13/10                           phungus420  */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool isDCM_BATTLE_EFFECTS() const { return GC.isDCM_BATTLE_EFFECTS(); }
	int getBATTLE_EFFECT_LESS_FOOD() const { return GC.getBATTLE_EFFECT_LESS_FOOD(); }
	int getBATTLE_EFFECT_LESS_PRODUCTION() const { return GC.getBATTLE_EFFECT_LESS_PRODUCTION(); }
	int getBATTLE_EFFECT_LESS_COMMERCE() const { return GC.getBATTLE_EFFECT_LESS_COMMERCE(); }
	int getBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS() const { return GC.getBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS(); }
	int getMAX_BATTLE_TURNS() const { return GC.getMAX_BATTLE_TURNS(); }

	bool isDCM_AIR_BOMBING() const { return GC.isDCM_AIR_BOMBING(); }
	bool isDCM_RANGE_BOMBARD() const { return GC.isDCM_RANGE_BOMBARD(); }
	int getDCM_RB_CITY_INACCURACY() const { return GC.getDCM_RB_CITY_INACCURACY(); }
	int getDCM_RB_CITYBOMBARD_CHANCE() const { return GC.getDCM_RB_CITYBOMBARD_CHANCE(); }
	bool isDCM_ATTACK_SUPPORT() const { return GC.isDCM_ATTACK_SUPPORT(); }
	bool isDCM_STACK_ATTACK() const { return GC.isDCM_STACK_ATTACK(); }
	bool isDCM_OPP_FIRE() const { return GC.isDCM_OPP_FIRE(); }
	bool isDCM_ACTIVE_DEFENSE() const { return GC.isDCM_ACTIVE_DEFENSE(); }
	bool isDCM_ARCHER_BOMBARD() const { return GC.isDCM_ARCHER_BOMBARD(); }
	bool isDCM_FIGHTER_ENGAGE() const { return GC.isDCM_FIGHTER_ENGAGE(); }

	bool isDYNAMIC_CIV_NAMES() const { return GC.isDYNAMIC_CIV_NAMES(); }

	bool isIDW_ENABLED() const { return GC.isIDW_ENABLED(); }
	float getIDW_BASE_COMBAT_INFLUENCE() const { return GC.getIDW_BASE_COMBAT_INFLUENCE(); }
	float getIDW_NO_CITY_DEFENDER_MULTIPLIER() const { return GC.getIDW_NO_CITY_DEFENDER_MULTIPLIER(); }
	float getIDW_FORT_CAPTURE_MULTIPLIER() const { return GC.getIDW_FORT_CAPTURE_MULTIPLIER(); }
	float getIDW_EXPERIENCE_FACTOR() const { return GC.getIDW_EXPERIENCE_FACTOR(); }
	float getIDW_WARLORD_MULTIPLIER() const { return GC.getIDW_WARLORD_MULTIPLIER(); }
	int getIDW_INFLUENCE_RADIUS() const { return GC.getIDW_INFLUENCE_RADIUS(); }
	float getIDW_PLOT_DISTANCE_FACTOR() const { return GC.getIDW_PLOT_DISTANCE_FACTOR(); }
	float getIDW_WINNER_PLOT_MULTIPLIER() const { return GC.getIDW_WINNER_PLOT_MULTIPLIER(); }
	float getIDW_LOSER_PLOT_MULTIPLIER() const { return GC.getIDW_LOSER_PLOT_MULTIPLIER(); }
	bool isIDW_EMERGENCY_DRAFT_ENABLED() const { return GC.isIDW_EMERGENCY_DRAFT_ENABLED(); }
	int getIDW_EMERGENCY_DRAFT_MIN_POPULATION() const { return GC.getIDW_EMERGENCY_DRAFT_MIN_POPULATION(); }
	float getIDW_EMERGENCY_DRAFT_STRENGTH() const { return GC.getIDW_EMERGENCY_DRAFT_STRENGTH(); }
	float getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER() const { return GC.getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER(); }
	bool isIDW_NO_BARBARIAN_INFLUENCE() const { return GC.isIDW_NO_BARBARIAN_INFLUENCE(); }
	bool isIDW_NO_NAVAL_INFLUENCE() const { return GC.isIDW_NO_NAVAL_INFLUENCE(); }
	bool isIDW_PILLAGE_INFLUENCE_ENABLED() const { return GC.isIDW_PILLAGE_INFLUENCE_ENABLED(); }
	float getIDW_BASE_PILLAGE_INFLUENCE() const { return GC.getIDW_BASE_PILLAGE_INFLUENCE(); }
	float getIDW_CITY_TILE_MULTIPLIER() const { return GC.getIDW_CITY_TILE_MULTIPLIER(); }

	bool isSS_ENABLED() const { return GC.isSS_ENABLED(); }
	bool isSS_BRIBE() const { return GC.isSS_BRIBE(); }
	bool isSS_ASSASSINATE() const { return GC.isSS_ASSASSINATE(); }
/************************************************************************************************/
/* Mod Globals                        END                                           phungus420  */
/************************************************************************************************/

	int getMOVE_DENOMINATOR() const { return GC.getMOVE_DENOMINATOR(); }
	int getNUM_UNIT_PREREQ_OR_BONUSES() const { return GC.getNUM_UNIT_PREREQ_OR_BONUSES(); }
	int getNUM_BUILDING_PREREQ_OR_BONUSES() const { return GC.getNUM_BUILDING_PREREQ_OR_BONUSES(); }
	int getFOOD_CONSUMPTION_PER_POPULATION() const { return GC.getFOOD_CONSUMPTION_PER_POPULATION(); }
	int getMAX_HIT_POINTS() const { return GC.getMAX_HIT_POINTS(); }
	int getHILLS_EXTRA_DEFENSE() const { return GC.getHILLS_EXTRA_DEFENSE(); }
	int getRIVER_ATTACK_MODIFIER() const { return GC.getRIVER_ATTACK_MODIFIER(); }
	int getAMPHIB_ATTACK_MODIFIER() const { return GC.getAMPHIB_ATTACK_MODIFIER(); }
	int getHILLS_EXTRA_MOVEMENT() const { return GC.getHILLS_EXTRA_MOVEMENT(); }
	int getMAX_PLOT_LIST_ROWS() const { return GC.getMAX_PLOT_LIST_ROWS(); }
	int getUNIT_MULTISELECT_MAX() const { return GC.getUNIT_MULTISELECT_MAX(); }
	int getPERCENT_ANGER_DIVISOR() const { return GC.getPERCENT_ANGER_DIVISOR(); }
	int getEVENT_MESSAGE_TIME() const { return GC.getEVENT_MESSAGE_TIME(); }
	int getROUTE_FEATURE_GROWTH_MODIFIER() const { return GC.getROUTE_FEATURE_GROWTH_MODIFIER(); }
	int getFEATURE_GROWTH_MODIFIER() const { return GC.getFEATURE_GROWTH_MODIFIER(); }
	int getMIN_CITY_RANGE() const { return GC.getMIN_CITY_RANGE(); }
	int getCITY_MAX_NUM_BUILDINGS() const { return GC.getCITY_MAX_NUM_BUILDINGS(); }
	int getNUM_UNIT_AND_TECH_PREREQS() const { return GC.getNUM_UNIT_AND_TECH_PREREQS(); }
	int getNUM_AND_TECH_PREREQS() const { return GC.getNUM_AND_TECH_PREREQS(); }
	int getNUM_OR_TECH_PREREQS() const { return GC.getNUM_OR_TECH_PREREQS(); }
	int getLAKE_MAX_AREA_SIZE() const { return GC.getLAKE_MAX_AREA_SIZE(); }
	int getNUM_ROUTE_PREREQ_OR_BONUSES() const { return GC.getNUM_ROUTE_PREREQ_OR_BONUSES(); }
	int getNUM_BUILDING_AND_TECH_PREREQS() const { return GC.getNUM_BUILDING_AND_TECH_PREREQS(); }
	int getMIN_WATER_SIZE_FOR_OCEAN() const { return GC.getMIN_WATER_SIZE_FOR_OCEAN(); }
	int getFORTIFY_MODIFIER_PER_TURN() const { return GC.getFORTIFY_MODIFIER_PER_TURN(); }
	int getMAX_CITY_DEFENSE_DAMAGE() const { return GC.getMAX_CITY_DEFENSE_DAMAGE(); }
	int getNUM_CORPORATION_PREREQ_BONUSES() const { return GC.getNUM_CORPORATION_PREREQ_BONUSES(); }
	int getPEAK_SEE_THROUGH_CHANGE() const { return GC.getPEAK_SEE_THROUGH_CHANGE(); }
	int getHILLS_SEE_THROUGH_CHANGE() const { return GC.getHILLS_SEE_THROUGH_CHANGE(); }
	int getSEAWATER_SEE_FROM_CHANGE() const { return GC.getSEAWATER_SEE_FROM_CHANGE(); }
	int getPEAK_SEE_FROM_CHANGE() const { return GC.getPEAK_SEE_FROM_CHANGE(); }
	int getHILLS_SEE_FROM_CHANGE() const { return GC.getHILLS_SEE_FROM_CHANGE(); }
	int getUSE_SPIES_NO_ENTER_BORDERS() const { return GC.getUSE_SPIES_NO_ENTER_BORDERS(); }

	float getCAMERA_MIN_YAW() const { return GC.getCAMERA_MIN_YAW(); }
	float getCAMERA_MAX_YAW() const { return GC.getCAMERA_MAX_YAW(); }
	float getCAMERA_FAR_CLIP_Z_HEIGHT() const { return GC.getCAMERA_FAR_CLIP_Z_HEIGHT(); }
	float getCAMERA_MAX_TRAVEL_DISTANCE() const { return GC.getCAMERA_MAX_TRAVEL_DISTANCE(); }
	float getCAMERA_START_DISTANCE() const { return GC.getCAMERA_START_DISTANCE(); }
	float getAIR_BOMB_HEIGHT() const { return GC.getAIR_BOMB_HEIGHT(); }
	float getPLOT_SIZE() const { return GC.getPLOT_SIZE(); }
	float getCAMERA_SPECIAL_PITCH() const { return GC.getCAMERA_SPECIAL_PITCH(); }
	float getCAMERA_MAX_TURN_OFFSET() const { return GC.getCAMERA_MAX_TURN_OFFSET(); }
	float getCAMERA_MIN_DISTANCE() const { return GC.getCAMERA_MIN_DISTANCE(); }
	float getCAMERA_UPPER_PITCH() const { return GC.getCAMERA_UPPER_PITCH(); }
	float getCAMERA_LOWER_PITCH() const { return GC.getCAMERA_LOWER_PITCH(); }
	float getFIELD_OF_VIEW() const { return GC.getFIELD_OF_VIEW(); }
	float getSHADOW_SCALE() const { return GC.getSHADOW_SCALE(); }
	float getUNIT_MULTISELECT_DISTANCE() const { return GC.getUNIT_MULTISELECT_DISTANCE(); }

	int getMAX_CIV_PLAYERS() const { return GC.getMAX_CIV_PLAYERS(); }
	int getMAX_PC_PLAYERS() const { return GC.getMAX_PC_PLAYERS(); }
	int getMAX_PLAYERS() const { return GC.getMAX_PLAYERS(); }
	int getMAX_CIV_TEAMS() const { return GC.getMAX_CIV_TEAMS(); }
	int getMAX_PC_TEAMS() const { return GC.getMAX_PC_TEAMS(); }
	int getMAX_TEAMS() const { return GC.getMAX_TEAMS(); }
	int getLAST_PLAYER() const { return GC.getLAST_PLAYER(); }
	int getBARBARIAN_PLAYER() const { return GC.getBARBARIAN_PLAYER(); }
	int getBARBARIAN_TEAM() const { return GC.getBARBARIAN_TEAM(); }
	int getAGGRESSIVE_ANIMAL_PLAYER() const { return GC.getAGGRESSIVE_ANIMAL_PLAYER(); }
	int getAGGRESSIVE_ANIMAL_TEAM() const { return GC.getAGGRESSIVE_ANIMAL_TEAM(); }
	int getPASSIVE_ANIMAL_PLAYER() const { return GC.getPASSIVE_ANIMAL_PLAYER(); }
	int getPASSIVE_ANIMAL_TEAM() const { return GC.getPASSIVE_ANIMAL_TEAM(); }
	int getNPC1_PLAYER() const { return GC.getNPC1_PLAYER(); }
	int getNPC1_TEAM() const { return GC.getNPC1_TEAM(); }
	int getNPC2_PLAYER() const { return GC.getNPC2_PLAYER(); }
	int getNPC2_TEAM() const { return GC.getNPC2_TEAM(); }
	int getNPC3_PLAYER() const { return GC.getNPC3_PLAYER(); }
	int getNPC3_TEAM() const { return GC.getNPC3_TEAM(); }
	int getNPC4_PLAYER() const { return GC.getNPC4_PLAYER(); }
	int getNPC4_TEAM() const { return GC.getNPC4_TEAM(); }
	int getNPC5_PLAYER() const { return GC.getNPC5_PLAYER(); }
	int getNPC5_TEAM() const { return GC.getNPC5_TEAM(); }
	int getNPC6_PLAYER() const { return GC.getNPC6_PLAYER(); }
	int getNPC6_TEAM() const { return GC.getNPC6_TEAM(); }
	int getNPC7_PLAYER() const { return GC.getNPC7_PLAYER(); }
	int getNPC7_TEAM() const { return GC.getNPC7_TEAM(); }
	int getNPC8_PLAYER() const { return GC.getNPC8_PLAYER(); }
	int getNPC8_TEAM() const { return GC.getNPC8_TEAM(); }
	int getINVALID_PLOT_COORD() const { return GC.getINVALID_PLOT_COORD(); }
	int getNUM_CITY_PLOTS() const { return GC.getNUM_CITY_PLOTS(); }
	int getCITY_HOME_PLOT() const { return GC.getCITY_HOME_PLOT(); }

// BUG - DLL Info - start
	bool isBull() const { return GC.isBull(); }
	int getBullApiVersion() const { return GC.getBullApiVersion(); }

	const wchar* getBullName() const { return GC.getBullName(); }
	const wchar* getBullVersion() const { return GC.getBullVersion(); }
	
	std::wstring pyGetBullName() { return getBullName(); }									// Exposed to Python
	std::wstring pyGetBullVersion() { return getBullVersion(); }							// Exposed to Python
// BUG - DLL Info - end

// BUG - BUG Info - start
	void setIsBug(bool bIsBug) { GC.setIsBug(bIsBug); }										// Exposed to Python
// BUG - BUG Info - end

// BUFFY - DLL Info - start
#ifdef _BUFFY
	bool isBuffy() const { return GC.isBuffy(); }
	int getBuffyApiVersion() const { return GC.getBuffyApiVersion(); }

	const wchar* getBuffyName() const { return GC.getBuffyName(); }
	const wchar* getBuffyVersion() const { return GC.getBuffyVersion(); }
	
	std::wstring pyGetBuffyName() { return getBuffyName(); }								// Exposed to Python
	std::wstring pyGetBuffyVersion() { return getBuffyVersion(); }							// Exposed to Python
#endif
// BUFFY - DLL Info - end
/************************************************************************************************/
/* Afforess	                  Start		 06/13/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getPEAK_EXTRA_DEFENSE() const { return GC.getPEAK_EXTRA_DEFENSE(); }
	int getPEAK_EXTRA_MOVEMENT() const { return GC.getPEAK_EXTRA_MOVEMENT(); }
	bool isXMLLogging() const { return GC.isXMLLogging(); }
	void setXMLLogging(bool bNewVal) { GC.setXMLLogging(bNewVal); }
	
	void setNoUpdateDefineINT( const char * szName, int iValue ) { return GC.setDefineINT( szName, iValue, false ); }
	void setNoUpdateDefineFLOAT( const char * szName, float fValue ) { return GC.setDefineFLOAT( szName, fValue, false ); }
	void setNoUpdateDefineSTRING( const char * szName, const char * szValue ) { return GC.setDefineSTRING( szName, szValue, false ); }
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	// UI functions
	POINT getCursorPos() const;
	POINT screenToClient(POINT screenPos) const;
};

#endif	// CyGlobalContext_h
