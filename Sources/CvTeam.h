#pragma once

// team.h

#ifndef CIV4_TEAM_H
#define CIV4_TEAM_H

//#include "CvEnums.h"
#include "CvGameObject.h"

class CvArea;
class CvProperties;

class CvTeam
{

public:
	CvTeam();
	virtual ~CvTeam();

	DllExport void init(TeamTypes eID);
	DllExport void reset(TeamTypes eID = NO_TEAM, bool bConstructorCall = false);

	CvGameObjectTeam* getGameObject() {return &m_GameObject;};
protected:
	CvGameObjectTeam m_GameObject;

public:
/************************************************************************************************/
/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void logMsg(char* format, ... );
	bool isRebel() const;
	bool isSingleCityTeam() const;
	bool isRebelAgainst( TeamTypes eTeam ) const;
	void setRebelAgainst( TeamTypes eTeam, bool bNewValue );
	int countRebelAgainst( ) const;
	int getNumMilitaryUnits( ) const;
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

protected:

	void uninit();

public:

/********************************************************************************/
/*		BETTER_BTS_AI_MOD						12/30/08		jdog5000		*/
/*																				*/
/*		     																	*/
/********************************************************************************/
	void resetPlotAndCityData( );
/********************************************************************************/
/*		BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/

	void addTeam(TeamTypes eTeam);																								// Exposed to Python
	void shareItems(TeamTypes eTeam);
	void shareCounters(TeamTypes eTeam);
	//Team Project (5)
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bReligiouslyDisabling = false);

	void doTurn();

	void updateYield();
	void updatePowerHealth();
	void updateCommerce();

	bool canChangeWarPeace(TeamTypes eTeam, bool bAllowVassal = false) const;																			// Exposed to Python
	DllExport bool canDeclareWar(TeamTypes eTeam) const;																// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/16/10                                jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
	bool canEventuallyDeclareWar(TeamTypes eTeam) const;
	void declareWar(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan, bool bCancelPacts = true);										// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	void makePeace(TeamTypes eTeam, bool bBumpUnits = true);																		// Exposed to Python
	bool canContact(TeamTypes eTeam) const;																							// Exposed to Python
	void meet(TeamTypes eTeam, bool bNewDiplo);																		// Exposed to Python
	void signOpenBorders(TeamTypes eTeam);																				// Exposed to Python
	void signDefensivePact(TeamTypes eTeam);																			// Exposed to Python
	bool canSignDefensivePact(TeamTypes eTeam);

	int getAssets() const;																															// Exposed to Python
	int getPower(bool bIncludeVassals) const;																																// Exposed to Python
	int getDefensivePower(TeamTypes eExcludeTeam = NO_TEAM) const;																											// Exposed to Python
	int getEnemyPower() const;
	int getNumNukeUnits() const;																												// Exposed to Python
#if defined QC_MASTERY_VICTORY
//Sevo Begin--VCM
	int getTotalVictoryScore();																												// Exposed to Python
//Sevo End VCM
#endif
	int getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const;
	bool isVotingMember(VoteSourceTypes eVoteSource) const;
	bool isFullMember(VoteSourceTypes eVoteSource) const;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/10/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getCurrentMasterPower( bool bIncludeVassals ) const;
	bool isMasterPlanningLandWar(CvArea* pArea);
	bool isMasterPlanningSeaWar(CvArea* pArea);
	int getAtWarCount(bool bIgnoreMinors, bool bIgnoreVassals = false) const;																				// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	int getWarPlanCount(WarPlanTypes eWarPlan, bool bIgnoreMinors) const;								// Exposed to Python
	int getAnyWarPlanCount(bool bIgnoreMinors) const;																		// Exposed to Python
	int getChosenWarCount(bool bIgnoreMinors) const;																		// Exposed to Python
	int getHasMetCivCount(bool bIgnoreMinors) const;																		// Exposed to Python
	bool hasMetHuman() const;																														// Exposed to Python
	int getDefensivePactCount(TeamTypes eTeam = NO_TEAM) const;																									// Exposed to Python
	int getVassalCount(TeamTypes eTeam = NO_TEAM) const;
	bool isAVassal() const;																							// Exposed to Python
	bool canVassalRevolt(TeamTypes eMaster) const;

	int getUnitClassMaking(UnitClassTypes eUnitClass) const;														// Exposed to Python
	int getUnitClassCountPlusMaking(UnitClassTypes eIndex) const;												// Exposed to Python
	int getBuildingClassMaking(BuildingClassTypes eBuildingClass) const;								// Exposed to Python
	int getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const;								// Exposed to Python
	int getHasReligionCount(ReligionTypes eReligion) const;															// Exposed to Python
	int getHasCorporationCount(CorporationTypes eCorporation) const;															// Exposed to Python

	int processedTeamCulture();
	unsigned long long countTotalCulture();																											// Exposed to Python

	int countNumUnitsByArea(CvArea* pArea) const;																				// Exposed to Python
	int countNumCitiesByArea(CvArea* pArea) const;																			// Exposed to Python
	int countTotalPopulationByArea(CvArea* pArea) const;																// Exposed to Python
	int countPowerByArea(CvArea* pArea) const;																					// Exposed to Python
	int countEnemyPowerByArea(CvArea* pArea) const;																			// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/19/10                                jdog5000      */
/*                                                                                              */
/* War strategy AI                                                                              */
/************************************************************************************************/
	int countEnemyPopulationByArea(CvArea* pArea) const;
	int countNumAIUnitsByArea(CvArea* pArea, UnitAITypes eUnitAI) const;								// Exposed to Python
	int countEnemyDangerByArea(CvArea* pArea, TeamTypes eEnemyTeam = NO_TEAM) const;																		// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	// K-Mod
	int getTypicalUnitValue(UnitAITypes eUnitAI) const;

	int getResearchCost(TechTypes eTech) const;																// Exposed to Python
	int getResearchLeft(TechTypes eTech) const;																// Exposed to Python

	bool hasHolyCity(ReligionTypes eReligion) const;																		// Exposed to Python
	bool hasHeadquarters(CorporationTypes eCorporation) const;																		// Exposed to Python
	bool hasBonus(BonusTypes eBonus) const;
	bool isBonusObsolete(BonusTypes eBonus) const;

	bool isHuman() const;																																// Exposed to Python
	bool isBarbarian() const;																														// Exposed to Python
	bool isNPC() const;																														// Exposed to Python
	bool isHominid() const;																														// Exposed to Python
	bool isMinorCiv() const;																														// Exposed to Python
/************************************************************************************************/
/* REVOLUTION_MOD                         10/22/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void setIsMinorCiv( bool bNewValue, bool bDoBarbCivCheck );																					// Exposed to Python
	void declareWarAsMinor();
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	PlayerTypes getLeaderID() const;																										// Exposed to Python
	PlayerTypes getSecretaryID() const;																									// Exposed to Python
	HandicapTypes getHandicapType() const;																							// Exposed to Python
	CvWString getName() const;																								// Exposed to Python

	DllExport int getNumMembers() const;																								// Exposed to Python
	void changeNumMembers(int iChange);

	int getAliveCount() const;
	int isAlive() const;																											// Exposed to Python
	void changeAliveCount(int iChange);

	int getEverAliveCount() const;
	int isEverAlive() const;																														// Exposed to Python
	void changeEverAliveCount(int iChange);

	int getNumCities() const;																														// Exposed to Python
	void changeNumCities(int iChange);							

	int getTotalPopulation(bool bCheckVassals = true) const;																											// Exposed to Python
	void changeTotalPopulation(int iChange);	

	int getTotalLand(bool bCheckVassals = true) const;																														// Exposed to Python  
	void changeTotalLand(int iChange);														

	int getNukeInterception() const;																										// Exposed to Python
	void changeNukeInterception(int iChange);																			// Exposed to Python

	int getForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource) const;																				// Exposed to Python	
	bool isForceTeamVoteEligible(VoteSourceTypes eVoteSource) const;																								// Exposed to Python	
	void changeForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource, int iChange);												// Exposed to Python	
																																								
	int getExtraWaterSeeFromCount() const;																							// Exposed to Python	
	bool isExtraWaterSeeFrom() const;																										// Exposed to Python	
	void changeExtraWaterSeeFromCount(int iChange);																// Exposed to Python	
																																								
	int getMapTradingCount() const;																											// Exposed to Python	
	bool isMapTrading() const;																													// Exposed to Python	
	void changeMapTradingCount(int iChange);																			// Exposed to Python	

/************************************************************************************************/
/* Afforess	                         01/01/10                                                   */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getCanPassPeaksCount() const;																				
	bool isCanPassPeaks() const;																												
	void changeCanPassPeaksCount(int iChange);	
	
	int getMoveFastPeaksCount() const;																				
	bool isMoveFastPeaks() const;																												
	void changeMoveFastPeaksCount(int iChange);	
	
	int getCanFoundOnPeaksCount() const;																				
	bool isCanFoundOnPeaks() const;																												
	void changeCanFoundOnPeaksCount(int iChange);
	
	int getRebaseAnywhereCount() const;
	bool isRebaseAnywhere() const;
	void changeRebaseAnywhereCount(int iChange);
	
	int getTradeModifier() const;
	void changeTradeModifier(int iChange);
	
	int getForeignTradeModifier() const;
	void changeForeignTradeModifier(int iChange);
	
	int getTradeMissionModifier() const;
	void changeTradeMissionModifier(int iChange);
	
	int getCorporationRevenueModifier() const;
	void changeCorporationRevenueModifier(int iChange);
	
	int getCorporationMaintenanceModifier() const;
	void changeCorporationMaintenanceModifier(int iChange);

	int getTechExtraBuildingHappiness(BuildingTypes eIndex) const;
	void changeTechExtraBuildingHappiness(BuildingTypes eIndex, int iChange);
	void setTechExtraBuildingHappiness(BuildingTypes eIndex, int iNewValue);
	
	int getTechExtraBuildingHealth(BuildingTypes eIndex) const;
	void changeTechExtraBuildingHealth(BuildingTypes eIndex, int iChange);
	void setTechExtraBuildingHealth(BuildingTypes eIndex, int iNewValue);
	
	void ObsoletePromotions(TechTypes eObsoleteTech);
	void ObsoleteCorporations(TechTypes eObsoleteTech);
	
	int getEmbassyTradingCount() const;
	bool isEmbassyTrading() const;
	void changeEmbassyTradingCount(int iChange);
	
    bool isHasEmbassy(TeamTypes eIndex) const;
    void setHasEmbassy(TeamTypes eIndex, bool bNewValue);
	int getBuildingCommerceChange(BuildingTypes eIndex1, CommerceTypes eIndex2) const;							
	void changeBuildingCommerceChange(BuildingTypes eIndex1, CommerceTypes eIndex2, int iChange); 

	int getBuildingYieldChange(BuildingTypes eIndex1, YieldTypes eIndex2) const;			// Exposed to Python
	void changeBuildingYieldChange(BuildingTypes eIndex1, YieldTypes eIndex2, int iChange); 

	int getBuildingSpecialistChange(BuildingTypes eIndex1, SpecialistTypes eIndex2) const;							
	void changeBuildingSpecialistChange(BuildingTypes eIndex1, SpecialistTypes eIndex2, int iChange); 	

	int getBuildingCommerceModifier(BuildingTypes eIndex1, CommerceTypes eIndex2) const;							
	void changeBuildingCommerceModifier(BuildingTypes eIndex1, CommerceTypes eIndex2, int iChange); 

	int getBuildingYieldModifier(BuildingTypes eIndex1, YieldTypes eIndex2) const;			
	void changeBuildingYieldModifier(BuildingTypes eIndex1, YieldTypes eIndex2, int iChange); 
	
	int getLimitedBordersTradingCount() const;	
	bool isLimitedBordersTrading() const;
	void changeLimitedBordersTradingCount(int iChange);
	
	int getCanFarmDesertCount() const;	
	bool isCanFarmDesert() const;
	void changeCanFarmDesertCount(int iChange);
	
	void signLimitedBorders(TeamTypes eTeam);	
	bool canSignOpenBorders(TeamTypes eTeam);
	void sendAmbassador(TeamTypes eTeam);
	
	int getFreeSpecialistCount(SpecialistTypes eIndex) const;
	void setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue);
	void changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange);
	
	bool isAnyVassal() const;
	ImprovementTypes getImprovementUpgrade(ImprovementTypes eImprovement) const;
	ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement) const;
	
	bool isFreeTradeAgreement(TeamTypes eIndex) const;
	void setFreeTradeAgreement(TeamTypes eIndex, bool bNewValue);
	
	bool isLimitedBorders(TeamTypes eIndex) const;
	void setLimitedBorders(TeamTypes eIndex, bool bNewValue);

	void AI_updateBonusValue(BonusTypes eBonus);
	
	int getUnitClassStrengthChange(UnitClassTypes eUnit) const;
	void changeUnitClassStrengthChange(UnitClassTypes eUnit, int iChange);
/************************************************************************************************/
/* Afforess	                     END        		                                            */
/************************************************************************************************/	

	void AI_setAssignWorkDirtyInEveryPlayerCityWithActiveBuilding(BuildingTypes eBuilding);
											
	int getTechTradingCount() const;																										// Exposed to Python	
	bool isTechTrading() const;																													// Exposed to Python	
	void changeTechTradingCount(int iChange);																			// Exposed to Python	
																																								
	int getGoldTradingCount() const;																										// Exposed to Python	
	bool isGoldTrading() const;																													// Exposed to Python	
	void changeGoldTradingCount(int iChange);																			// Exposed to Python	
																																								
	int getOpenBordersTradingCount() const;																							// Exposed to Python	
	bool isOpenBordersTrading() const;																				// Exposed to Python	
	void changeOpenBordersTradingCount(int iChange);															// Exposed to Python	
																																								
	int getDefensivePactTradingCount() const;																						// Exposed to Python	
	bool isDefensivePactTrading() const;																								// Exposed to Python						
	void changeDefensivePactTradingCount(int iChange);														// Exposed to Python	
																																									
	int getPermanentAllianceTradingCount() const;																				// Exposed to Python	
	bool isPermanentAllianceTrading() const;																						// Exposed to Python						
	void changePermanentAllianceTradingCount(int iChange);												// Exposed to Python	
																																									
	int getVassalTradingCount() const;																				// Exposed to Python	
	bool isVassalStateTrading() const;																						// Exposed to Python						
	void changeVassalTradingCount(int iChange);												// Exposed to Python	

	int getBridgeBuildingCount() const;																									// Exposed to Python	
	bool isBridgeBuilding() const;																						// Exposed to Python						
	void changeBridgeBuildingCount(int iChange);																	// Exposed to Python	
																																								
	int getIrrigationCount() const;																											// Exposed to Python	
	bool isIrrigation() const;																								// Exposed to Python	
	void changeIrrigationCount(int iChange);																			// Exposed to Python	
																																								
	int getIgnoreIrrigationCount() const;																								// Exposed to Python	
	bool isIgnoreIrrigation() const;																					// Exposed to Python	
	void changeIgnoreIrrigationCount(int iChange);																// Exposed to Python	
																																								
	int getWaterWorkCount() const;																											// Exposed to Python	
	bool isWaterWork() const;																									// Exposed to Python	
	void changeWaterWorkCount(int iChange);																				// Exposed to Python	

	int getVassalPower() const;																							// Exposed to Python	
	void setVassalPower(int iPower);																					// Exposed to Python	
	int getMasterPower() const;																							// Exposed to Python	
	void setMasterPower(int iPower);																					// Exposed to Python	

	int getEnemyWarWearinessModifier() const;																																			// Exposed to Python
	void changeEnemyWarWearinessModifier(int iChange);						// Exposed to Python
	void changeWarWeariness(TeamTypes eOtherTeam, const CvPlot& kPlot, int iFactor);
	void changeWarWearinessTimes100(TeamTypes eOtherTeam, const CvPlot& kPlot, int iFactor);

	bool isMapCentering() const;																							// Exposed to Python	
	void setMapCentering(bool bNewValue);																					// Exposed to Python	
																																								
	TeamTypes getID() const;																											// Exposed to Python	

	int getStolenVisibilityTimer(TeamTypes eIndex) const;
	bool isStolenVisibility(TeamTypes eIndex) const;																		// Exposed to Python
	void setStolenVisibilityTimer(TeamTypes eIndex, int iNewValue);
	void changeStolenVisibilityTimer(TeamTypes eIndex, int iChange);

	int getWarWeariness(TeamTypes eIndex) const;																				// Exposed to Python
	int getWarWearinessTimes100(TeamTypes eIndex) const;
	void setWarWeariness(TeamTypes eIndex, int iNewValue);												// Exposed to Python
	void setWarWearinessTimes100(TeamTypes eIndex, int iNewValue);
	void changeWarWeariness(TeamTypes eIndex, int iChange);												// Exposed to Python
	void changeWarWearinessTimes100(TeamTypes eIndex, int iChange);

	int getTechShareCount(int iIndex) const;																						// Exposed to Python
	bool isTechShare(int iIndex) const;																									// Exposed to Python
	void changeTechShareCount(int iIndex, int iChange);														// Exposed to Python

	int getCommerceFlexibleCount(CommerceTypes eIndex) const;														// Exposed to Python
	bool isCommerceFlexible(CommerceTypes eIndex) const;																// Exposed to Python
	void changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange);					// Exposed to Python

	int getExtraMoves(DomainTypes eIndex) const;																				// Exposed to Python
	void changeExtraMoves(DomainTypes eIndex, int iChange);							// Exposed to Python

	bool isHasMet(TeamTypes eIndex) const;																		// Exposed to Python
/************************************************************************************************/
/* REVOLUTION_MOD                         02/01/08                                jdog5000      */
/*                                                                                              */
/* For BarbarianCiv Mod and minor civs                                                                 */
/************************************************************************************************/
	void setHasMet( TeamTypes eIndex, bool bNewValue );
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	void makeHasMet(TeamTypes eIndex, bool bNewDiplo);

	DllExport bool isAtWar(TeamTypes eIndex) const;																			// Exposed to Python
	void setAtWar(TeamTypes eIndex, bool bNewValue);

	bool isPermanentWarPeace(TeamTypes eIndex) const;																		// Exposed to Python
	void setPermanentWarPeace(TeamTypes eIndex, bool bNewValue);									// Exposed to Python

	bool isFreeTrade(TeamTypes eIndex) const;																	// Exposed to Python
	bool isOpenBorders(TeamTypes eIndex) const;																// Exposed to Python
	void setOpenBorders(TeamTypes eIndex, bool bNewValue);
	
	bool isDefensivePact(TeamTypes eIndex) const;															// Exposed to Python
	void setDefensivePact(TeamTypes eIndex, bool bNewValue);

	bool isForcePeace(TeamTypes eIndex) const;																// Exposed to Python
	void setForcePeace(TeamTypes eIndex, bool bNewValue);

	bool isVassal(TeamTypes eIndex) const;																// Exposed to Python
	void setVassal(TeamTypes eIndex, bool bNewValue, bool bCapitulated);

	void assignVassal(TeamTypes eVassal, bool bSurrender) const;																// Exposed to Python
	void freeVassal(TeamTypes eVassal) const;																// Exposed to Python

	bool isCapitulated() const;

	int getRouteChange(RouteTypes eIndex) const;																				// Exposed to Python
	void changeRouteChange(RouteTypes eIndex, int iChange);												// Exposed to Python

	int getProjectCount(ProjectTypes eIndex) const;														// Exposed to Python
	DllExport int getProjectDefaultArtType(ProjectTypes eIndex) const;
	DllExport void setProjectDefaultArtType(ProjectTypes eIndex, int value);
	DllExport int getProjectArtType(ProjectTypes eIndex, int number) const;
	DllExport void setProjectArtType(ProjectTypes eIndex, int number, int value);
	bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra = 0) const;									// Exposed to Python
	DllExport bool isProjectAndArtMaxedOut(ProjectTypes eIndex) const;
	void changeProjectCount(ProjectTypes eIndex, int iChange);		// Exposed to Python
	void processProjectChange(ProjectTypes eIndex, int iChange, int iOldProjectCount);
	DllExport void finalizeProjectArtTypes();

	int getProjectMaking(ProjectTypes eIndex) const;																		// Exposed to Python
	void changeProjectMaking(ProjectTypes eIndex, int iChange);

	int getUnitClassCount(UnitClassTypes eIndex) const;																	// Exposed to Python
	bool isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra = 0) const;							// Exposed to Python
	void changeUnitClassCount(UnitClassTypes eIndex, int iChange);

	int getBuildingClassCount(BuildingClassTypes eIndex) const;													// Exposed to Python
	bool isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra = 0) const;			// Exposed to Python
	void changeBuildingClassCount(BuildingClassTypes eIndex, int iChange);

	int getObsoleteBuildingCount(BuildingTypes eIndex) const;
	bool isObsoleteBuilding(BuildingTypes eIndex) const;																// Exposed to Python
	void changeObsoleteBuildingCount(BuildingTypes eIndex, int iChange);

	int getResearchProgress(TechTypes eIndex) const;																						// Exposed to Python
	void setResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);									// Exposed to Python
	void changeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer);								// Exposed to Python
	int changeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer);

	int getTechCount(TechTypes eIndex) const;																										// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      07/27/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
	int getBestKnownTechScorePercent() const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	int getTerrainTradeCount(TerrainTypes eIndex) const;
	bool isTerrainTrade(TerrainTypes eIndex) const;																												// Exposed to Python
	void changeTerrainTradeCount(TerrainTypes eIndex, int iChange);

	int getRiverTradeCount() const;
	bool isRiverTrade() const;																												// Exposed to Python
	void changeRiverTradeCount(int iChange);

	bool isHasTech(TechTypes eIndex) const;																																			// Exposed to Python
	void setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce);	// Exposed to Python

	bool isNoTradeTech(TechTypes eIndex) const;																														// Exposed to Python
	void setNoTradeTech(TechTypes eIndex, bool bNewValue);																					// Exposed to Python

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;										// Exposed to Python 
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);		// Exposed to Python 

	void addPropertiesAllCities(CvProperties* pProp);
	void subtractPropertiesAllCities(CvProperties* pProp);

	bool isFriendlyTerritory(TeamTypes eTeam) const;

	int getVictoryCountdown(VictoryTypes eIndex) const;																							// Exposed to Python
	void setVictoryCountdown(VictoryTypes eIndex, int iTurnsLeft);
	void changeVictoryCountdown(VictoryTypes eIndex, int iChange);
	int getVictoryDelay(VictoryTypes eVictory) const;
	bool canLaunch(VictoryTypes eVictory) const;		// Exposed to Python 
	void setCanLaunch(VictoryTypes eVictory, bool bCan);
	int getLaunchSuccessRate(VictoryTypes eVictory) const;		// Exposed to Python
	void resetVictoryProgress();

	bool isParent(TeamTypes eTeam) const;		// Exposed to Python 

	int getEspionagePointsAgainstTeam(TeamTypes eIndex) const;																							// Exposed to Python
	void setEspionagePointsAgainstTeam(TeamTypes eIndex, int iValue);																							// Exposed to Python
	void changeEspionagePointsAgainstTeam(TeamTypes eIndex, int iChange);																				// Exposed to Python

	int getEspionagePointsEver() const;																							// Exposed to Python
	void setEspionagePointsEver(int iValue);																							// Exposed to Python
	void changeEspionagePointsEver(int iChange);																				// Exposed to Python

	int getCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex) const;																							// Exposed to Python
	void setCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iValue);																		// Exposed to Python
	void changeCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iChange);																// Exposed to Python

	int getCounterespionageModAgainstTeam(TeamTypes eIndex) const;																							// Exposed to Python
	void setCounterespionageModAgainstTeam(TeamTypes eIndex, int iValue);																		// Exposed to Python
	void changeCounterespionageModAgainstTeam(TeamTypes eIndex, int iChange);																// Exposed to Python

	void verifySpyUnitsValidPlot();

	void setForceRevealedBonus(BonusTypes eBonus, bool bRevealed);
	bool isForceRevealedBonus(BonusTypes eBonus) const;

	int countNumHumanGameTurnActive() const;
	void setTurnActive(bool bNewValue, bool bTurn = true);
	bool isTurnActive() const;

	bool hasShrine(ReligionTypes eReligion);

	DllExport void getCompletedSpaceshipProjects(std::map<ProjectTypes, int>& mapProjects) const;
	DllExport int getProjectPartNumber(ProjectTypes projectType, bool bAssert) const;
	DllExport bool hasLaunched() const;

	virtual void AI_init() = 0;
	virtual void AI_reset(bool bConstructor) = 0;
	virtual void AI_doTurnPre() = 0;
	virtual void AI_doTurnPost() = 0;
	virtual void AI_makeAssignWorkDirty() = 0;
	virtual void AI_updateAreaStragies(bool bTargets = true) = 0;
	virtual bool AI_shareWar(TeamTypes eTeam) const = 0;			// Exposed to Python
	virtual void AI_updateWorstEnemy() = 0;
	virtual int AI_getAtWarCounter(TeamTypes eIndex) const = 0;     // Exposed to Python
	virtual void AI_setAtWarCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getAtPeaceCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getHasMetCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setHasMetCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getOpenBordersCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getDefensivePactCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getShareWarCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setShareWarCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getWarSuccess(TeamTypes eIndex) const = 0;    // Exposed to Python
	virtual void AI_setWarSuccess(TeamTypes eIndex, int iNewValue) = 0;
	virtual void AI_changeWarSuccess(TeamTypes eIndex, int iChange) = 0;
	virtual int AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const = 0;
	virtual void AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const = 0;
	virtual void AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue) = 0;
	virtual WarPlanTypes AI_getWarPlan(TeamTypes eIndex) const = 0;
	virtual bool AI_isChosenWar(TeamTypes eIndex) const = 0;
	virtual bool AI_isSneakAttackPreparing(TeamTypes eIndex) const = 0;
	virtual bool AI_isSneakAttackReady(TeamTypes eIndex) const = 0;
	virtual void AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar = true, bool bInFull = true) = 0;

	void recalculateModifiers();

	int getLastRoundOfValidImprovementCacheUpdate() const;
	void setLastRoundOfValidImprovementCacheUpdate();

/********************************************************************************/
/**		REVOLUTION_MOD							6/9/08				jdog5000	*/
/**																				*/
/**		 																		*/
/********************************************************************************/
	//virtual int AI_getBarbarianCivWarVal( TeamTypes eTeam, int iMaxDistance = 10 ) const = 0;
/********************************************************************************/
/**		REVOLUTION_MOD							END								*/
/********************************************************************************/

	int getWinForLosingResearchModifier();

protected:

	int m_iNumMembers;
	int m_iAliveCount;
	int m_iEverAliveCount;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iTotalLand;
	int m_iNukeInterception;
	int m_iExtraWaterSeeFromCount;
	int m_iMapTradingCount;
	int m_iTechTradingCount;
	int m_iGoldTradingCount;
/************************************************************************************************/
/* Afforess	                  Start		 03/8/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int m_iCanPassPeaksCount; 
	int m_iMoveFastPeaksCount; 
	int m_iCanFoundOnPeaksCount; 
	int m_iRebaseAnywhereCount;
	int m_iEmbassyTradingCount;
	int m_iLimitedBordersTradingCount;
	int m_iCanFarmDesertCount;
	int m_iTradeModifier;
	int m_iForeignTradeModifier;
	int m_iTradeMissionModifier;
	int m_iCorporationRevenueModifier;
	int m_iCorporationMaintenanceModifier;
	int m_iLastRoundOfValidImprovementCacheUpdate;
	
	bool* m_abEmbassy;
	bool* m_abLimitedBorders;
	bool* m_abFreeTrade;
	int* m_paiUnitClassStrengthChange;
	
	int** m_ppiBuildingCommerceChange;
	int** m_ppiBuildingYieldChange;
	int** m_ppiBuildingSpecialistChange;
	int** m_ppiBuildingCommerceModifier;
	int** m_ppiBuildingYieldModifier;
	int* m_paiFreeSpecialistCount;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	int m_iOpenBordersTradingCount;
	int m_iDefensivePactTradingCount;
	int m_iPermanentAllianceTradingCount;
	int m_iVassalTradingCount;
	int m_iBridgeBuildingCount;
	int m_iIrrigationCount;
	int m_iIgnoreIrrigationCount;
	int m_iWaterWorkCount;
	int m_iVassalPower;
	int m_iMasterPower;
	int m_iEnemyWarWearinessModifier;
	int m_iRiverTradeCount;
	int m_iEspionagePointsEver;

	bool m_bMapCentering;
	bool m_bCapitulated;

	TeamTypes m_eID;

	int* m_aiStolenVisibilityTimer;
	int* m_aiWarWearinessTimes100;
	int* m_aiTechShareCount;
	int* m_aiCommerceFlexibleCount;
	int* m_aiExtraMoves;
	int* m_aiForceTeamVoteEligibilityCount;

	bool* m_abAtWar;
	bool* m_abHasMet;
	bool* m_abPermanentWarPeace;
	bool* m_abOpenBorders;
	bool* m_abDefensivePact;
	bool* m_abForcePeace;
	bool* m_abVassal;
	bool* m_abCanLaunch;

/************************************************************************************************/
/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool* m_abIsRebelAgainst;
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	int* m_paiRouteChange;
	int* m_paiProjectCount;
	int* m_paiProjectDefaultArtTypes;
	std::vector<int> *m_pavProjectArtTypes;
	int* m_paiProjectMaking;
	int* m_paiUnitClassCount;
	int* m_paiBuildingClassCount;
	int* m_paiObsoleteBuildingCount;
	int* m_paiResearchProgress;
	int* m_paiTechCount;
	int* m_paiTerrainTradeCount;
	int* m_aiVictoryCountdown;
	
	int* m_paiTechExtraBuildingHappiness;
	int* m_paiTechExtraBuildingHealth;

	int* m_aiEspionagePointsAgainstTeam;
	int* m_aiCounterespionageTurnsLeftAgainstTeam;
	int* m_aiCounterespionageModAgainstTeam;

	bool* m_pabHasTech;
	bool* m_pabNoTradeTech;

	int** m_ppaaiImprovementYieldChange;

	std::vector<BonusTypes> m_aeRevealedBonuses;

	void doWarWeariness();

	void updateTechShare(TechTypes eTech);
	void updateTechShare();

	void testCircumnavigated();

	void processTech(TechTypes eTech, int iChange, bool bAnnounce = false);

	void cancelDefensivePacts();
	void announceTechToPlayers(TechTypes eIndex, bool bPartial = false);

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	// AIAndy: Properties
	CvProperties m_Properties;
public:
	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;
};

#endif
