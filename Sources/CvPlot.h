#pragma once

// CvPlot.h

#ifndef CIV4_PLOT_H
#define CIV4_PLOT_H

//#include "CvStructs.h"
#include "LinkedList.h"
#include <bitset>
#include <vector>
#include "CvGameObject.h"
#include "CvUnit.h"
#include "idinfo_iterator.h"

class CvSelectionGroup;

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvArea;
class CvMap;
class CvPlotBuilder;
class CvRoute;
class CvRiver;
class CvCity;
class CvPlotGroup;
class CvFeature;
class CvUnit;
class CvSymbol;
class CvFlagEntity;
class CvPathGeneratorPlotInfo;
class CvPathPlotInfoStore;

typedef bool (*ConstPlotUnitFunc)( const CvUnit* pUnit, int iData1, int iData2, const CvUnit* eUnit);
typedef bool (*PlotUnitFunc)(CvUnit* pUnit, int iData1, int iData2, const CvUnit* eUnit);

//	Koshling - add caching to canBuild calculations
#define CAN_BUILD_VALUE_CACHING
#ifdef CAN_BUILD_VALUE_CACHING
struct canBuildCacheEntry
{
	canBuildCacheEntry()
		: iPlotX(-1)
		, iPlotY(-1)
		, eBuild(NO_BUILD)
		, ePlayer(NO_PLAYER)
		, lResult(-1)
		, iLastUseCount(0)
	{}

	int iPlotX;
	int iPlotY;
	BuildTypes eBuild;
	PlayerTypes ePlayer;
	long lResult;
	int iLastUseCount;
};

#define CAN_BUILD_CACHE_SIZE 64

class canBuildCache
{
public:
	canBuildCache() : currentUseCounter(0) {}

	struct canBuildCacheEntry entries[CAN_BUILD_CACHE_SIZE];
	int currentUseCounter;
};
#endif

class CvPlot
{
friend CvPathPlotInfoStore;
public:
	CvPlot();
	virtual ~CvPlot();


	CvGameObjectPlot* getGameObject() {return &m_GameObject;};

protected:
	CvGameObjectPlot m_GameObject;

public:
/************************************************************************************************/
/* DCM	                  Start		 05/31/10                        Johnny Smith               */
/*                                                                   Afforess                   */
/* Battle Effects                                                                               */
/************************************************************************************************/
	// Dale - BE: Battle Effect START
	void changeBattleCountdown(int iValue);
	void setBattleCountdown(int iValue);
	int getBattleCountdown() const;
	bool isBattle() const;
	bool canHaveBattleEffect(const CvUnit* pAttacker = NULL, const CvUnit* pDefender = NULL) const;
	EffectTypes getBattleEffect();
	void setBattleEffect();
protected:
	int m_iBattleCountdown;
	EffectTypes m_eBattleEffect;
public:
	// Dale - BE: Battle Effect END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	void init(int iX, int iY);
	void uninit();
	void reset(int iX = 0, int iY = 0, bool bConstructorCall=false);
	void setupGraphical();
	void updateGraphicEra();

	void erase();																																								// Exposed to Python
																																							// Exposed to Python
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	void destroyGraphics();
	bool isNull() const;
	void setNull(bool bNull);
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/	

	DllExport float getPointX() const;														
	DllExport float getPointY() const;														
	DllExport NiPoint3 getPoint() const;																																	// Exposed to Python

	float getSymbolSize() const;
	DllExport float getSymbolOffsetX(int iID) const;
	DllExport float getSymbolOffsetY(int iID) const;

	TeamTypes getTeam() const;																																	// Exposed to Python

	void doTurn();

	void doImprovement();

	void updateCulture(bool bBumpUnits, bool bUpdatePlotGroups);

	void updateFog();
	void updateVisibility();

	void updateSymbolDisplay();
	void updateSymbolVisibility();
	void updateSymbols();

	void updateMinimapColor();

	bool unitHere(const CvUnit* pUnit) const;
	void updateCenterUnit();
	void enableCenterUnitRecalc(bool bEnable);

	void verifyUnitValidPlot();

	void nukeExplosion(int iRange, CvUnit* pNukeUnit = NULL);																							// Exposed to Python

	bool isConnectedTo( const CvCity* pCity) const;																												// Exposed to Python
	bool isConnectedToCapital(PlayerTypes ePlayer = NO_PLAYER) const;																			// Exposed to Python
	int getPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;													// Exposed to Python
	bool isPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;								// Exposed to Python
	bool isAdjacentPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;				// Exposed to Python
	void updatePlotGroupBonus(bool bAdd);

	bool isAdjacentToArea(int iAreaID) const;
	bool isAdjacentToArea(const CvArea* pArea) const;																						// Exposed to Python
	bool shareAdjacentArea( const CvPlot* pPlot) const;																					// Exposed to Python
	bool isAdjacentToLand() const;																															// Exposed to Python 
	bool isCoastalLand(int iMinWaterSize = -1) const;																																	// Exposed to Python

	bool isVisibleWorked() const;
	bool isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer = NO_PLAYER) const;	// Exposed to Python

	DllExport bool isLake() const;																															// Exposed to Python
/************************************************************************************************/
/* Afforess	                  Start		 07/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	bool isFreshWater() const;
*/
	bool isFreshWater(bool bIgnoreJungle = false) const;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
																													// Exposed to Python
	bool isPotentialIrrigation() const;																													// Exposed to Python
	bool canHavePotentialIrrigation() const;																										// Exposed to Python
	DllExport bool isIrrigationAvailable(bool bIgnoreSelf = false) const;												// Exposed to Python

	bool isRiverMask() const;
	DllExport bool isRiverCrossingFlowClockwise(DirectionTypes eDirection) const;
	bool isRiverSide() const;																																		// Exposed to Python
	bool isRiver() const;																																				// Exposed to Python
	bool isRiverConnection(DirectionTypes eDirection) const;																		// Exposed to Python

	CvPlot* getNearestLandPlotInternal(int iDistance) const;
	int getNearestLandArea() const;																															// Exposed to Python
	CvPlot* getNearestLandPlot() const;																													// Exposed to Python

	int seeFromLevel(TeamTypes eTeam) const;																										// Exposed to Python  
	int seeThroughLevel() const;																																// Exposed to Python
	void changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, CvUnit* pUnit, bool bUpdatePlotGroups);
	bool canSeePlot(CvPlot *plot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection) const;
	bool canSeeDisplacementPlot(TeamTypes eTeam, int dx, int dy, int originalDX, int originalDY, bool firstPlot, bool outerRing) const;
	bool shouldProcessDisplacementPlot(int dx, int dy, int range, DirectionTypes eFacingDirection) const;
	void updateSight(bool bIncrement, bool bUpdatePlotGroups);
	void updateSeeFromSight(bool bIncrement, bool bUpdatePlotGroups);

	bool canHaveBonus(BonusTypes eBonus, bool bIgnoreLatitude = false) const;

	bool canBuildImprovement(ImprovementTypes eImprovement, TeamTypes eTeam) const;
	bool canHaveImprovementAsUpgrade(ImprovementTypes eImprovement, TeamTypes eTeam = NO_TEAM, bool bPotential = false, bool bOver = true) const;
/************************************************************************************************/
/* Afforess	                  Start		 05/24/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam = NO_TEAM, bool bPotential = false, bool bOver = true, bool bUpgradeCheck = false) const;		// Exposed to Python
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	bool canBuild(BuildTypes eBuild, PlayerTypes ePlayer = NO_PLAYER, bool bTestVisible = false, bool bIncludePythonOverrides = true) const;						// Exposed to Python												// Exposed to Python
	static bool hasCachedCanBuildEntry(int iX, int iY, BuildTypes eBuild, PlayerTypes ePlayer, struct canBuildCacheEntry*& entry);
	long canBuildFromPython(BuildTypes eBuild, PlayerTypes ePlayer) const;
	long canBuildFromPythonInternal(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTime(BuildTypes eBuild) const;																																										// Exposed to Python
// BUG - Partial Builds - start
	int getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTurnsLeft(BuildTypes eBuild, int iNowExtra, int iThenExtra, bool bIncludeUnits = true) const;																			// Exposed to Python
// BUG - Partial Builds - end
	int getFeatureProduction(BuildTypes eBuild, TeamTypes eTeam, CvCity** ppCity) const;																// Exposed to Python

	DllExport CvUnit* getBestDefenderExternal(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false) const;
	CvUnit* getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false, bool bAssassinate = false, bool bClearCache = false) const;		// Exposed to Python
	CvUnit* getFirstDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false) const;
	int AI_sumStrength(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, DomainTypes eDomainType = NO_DOMAIN, bool bDefensiveBonuses = true, bool bTestAtWar = false, bool bTestPotentialEnemy = false, int iRange = 0) const;	
	CvUnit* getSelectedUnit() const;																																// Exposed to Python				
	int getUnitPower(PlayerTypes eOwner = NO_PLAYER) const;																					// Exposed to Python	
	/*int getUnitNukeIntercept(PlayerTypes eOwner) const;*/

	// Super Forts begin *bombard*
	bool isBombardable(const CvUnit* pUnit) const;
	bool isBombarded() const;
	void setBombarded(bool bNewValue);
	int getDefenseDamage() const;
	void changeDefenseDamage(int iChange);
	// Super Forts end
	// Super Forts begin *culture*
	//	Koshling - due to the implementatino of fort culture inheritted from AND we need
	//	to handle fort culture a bit differently in C2C and some of these methods are
	//	not needed
	//int getCultureRangeForts(PlayerTypes ePlayer) const;
	//void setCultureRangeForts(PlayerTypes ePlayer, int iNewValue);
	//void changeCultureRangeForts(PlayerTypes ePlayer, int iChange);
	//bool isWithinFortCultureRange(PlayerTypes ePlayer) const;
	void changeCultureRangeFortsWithinRange(PlayerTypes ePlayer, int iChange, int iRange, bool bUpdate);
	void doImprovementCulture();
	// Super Forts end
	// Super Forts begin *canal* *choke*
	int countRegionPlots(const CvPlot* pInvalidPlot = NULL) const;
	int countAdjacentPassableSections(bool bWater) const;
	int countImpassableCardinalDirections() const;
	int getCanalValue() const;
	void setCanalValue(int iNewValue);
	void calculateCanalValue();
	int getChokeValue() const;
	void setChokeValue(int iNewValue);
	void calculateChokeValue();
	// Super Forts end

	int defenseModifier(TeamTypes eDefender, bool bIgnoreBuilding, bool bHelp = false) const;									// Exposed to Python				
	int movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot) const;														// Exposed to Python				
	static void flushMovementCostCache();

	int getExtraMovePathCost() const;																																// Exposed to Python
	void changeExtraMovePathCost(int iChange);		
																														// Exposed to Python
	//	Koshling - count of mountain leaders present per team maintained for efficiency of movement calculations
	// TB: This was not working properly so has been changed to a plotcount method.
	int getHasMountainLeader(TeamTypes eTeam) const;
	
	bool isAdjacentOwned() const;																																		// Exposed to Python
	bool isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly = false) const;												// Exposed to Python
	bool isAdjacentTeam(TeamTypes eTeam, bool bLandOnly = false) const;															// Exposed to Python
	bool isWithinCultureRange(PlayerTypes ePlayer, int* iFoundRange = NULL) const;																						// Exposed to Python
	int getNumCultureRangeCities(PlayerTypes ePlayer) const;																				// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/30/08                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
	bool isHasPathToEnemyCity( TeamTypes eAttackerTeam, bool bIgnoreBarb = true );
	bool isHasPathToPlayerCity( TeamTypes eMoveTeam, PlayerTypes eOtherPlayer = NO_PLAYER );
	int calculatePathDistanceToPlot( TeamTypes eTeam, CvPlot* pTargetPlot );
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	int getDistancePlottoPlot(const CvPlot* pTargetPlot) const;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// Plot danger cache
	bool isActivePlayerNoDangerCache() const;
	bool isActivePlayerHasDangerCache() const;
	bool isTeamBorderCache( TeamTypes eTeam ) const;
	void setIsActivePlayerNoDangerCache( bool bNewValue );
	void setIsActivePlayerHasDangerCache( bool bNewValue );
	void setIsTeamBorderCache( TeamTypes eTeam, bool bNewValue );
	void invalidateIsTeamBorderCache();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 01/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	CvCity* getAdjacentCity(PlayerTypes ePlayer = NO_PLAYER) const;
	bool isDepletedMine() const;
	void setIsDepletedMine(bool bNewValue);
	ImprovementTypes findDepletedMine();
	bool changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer = NO_PLAYER);
	void changeCultureRangeCities(PlayerTypes eOwnerIndex, int iRangeIndex, int iChange, bool bUpdatePlotGroups, bool bUpdateCulture = true);
	bool isHasValidBonus() const;
	int getOccupationCultureRangeCities(PlayerTypes eOwnerIndex) const;
	bool isWithinOccupationRange(PlayerTypes eOwnerIndex) const;
	void changeOccupationCultureRangeCities(PlayerTypes eOwnerIndex,int iChange);
	PlayerTypes getClaimingOwner() const;
	void setClaimingOwner(PlayerTypes eNewValue);
#ifdef OLD_VERSION_PRE_SUPER_FORTS
	void changeActsAsCity(PlayerTypes ePlayer, int iChange);
#endif
	bool isActsAsCity() const;
	void doResourceDepletion();
	bool isCanMoveLandUnits() const;
	bool isCanMoveSeaUnits() const;
	bool isCanMoveAllUnits() const;
	bool isCanUseRouteLandUnits() const;
	bool isCanUseRouteSeaUnits() const;
	bool isSeaTunnel() const;
	int getRevoltProtection();
	int getAverageEnemyStrength(TeamTypes eTeam);
	int getAverageEnemyDamage(TeamTypes eTeam);
	LandmarkTypes getLandmarkType() const;
	void setLandmarkType(LandmarkTypes eLandmark);
	CvWString getLandmarkName() const;
	void setLandmarkName(CvWString szName);
	CvWString getLandmarkMessage() const;
	void setLandmarkMessage(CvWString szName);
	bool isCountedPlot() const;
	void setCountedPlot(bool bNewVal);
	const CvPlot* isInFortControl(bool bIgnoreObstructions, PlayerTypes eDefendingPlayer, PlayerTypes eAttackingPlayer, const CvPlot* pFort = NULL) const;
	PlayerTypes controlsAdjacentZOCSource(TeamTypes eAttackingTeam) const;
	bool isInCityZoneOfControl(PlayerTypes ePlayer) const;
	bool isInUnitZoneOfControl(PlayerTypes ePlayer) const;
	int getTotalTurnDamage(const CvSelectionGroup* pGroup) const;
	int getTerrainTurnDamage(const CvUnit* pUnit = NULL) const;
	int getFeatureTurnDamage() const;
	int getTotalTurnDamage(const CvUnit* pUnit = NULL) const;
	CvUnit* getWorstDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false) const;
	bool isBorder(bool bIgnoreWater = false) const;
	
	int getNumVisibleAdjacentEnemyDefenders(const CvUnit* pUnit) const;		
	
	void addSign(PlayerTypes ePlayer, CvWString szMessage);
	void removeSign(PlayerTypes ePlayer);
	void removeSignForAllPlayers();
	
	int getNumVisibleEnemyUnits(const CvUnit* pUnit) const;
	int getNumVisibleEnemyTargetUnits(const CvUnit* pUnit) const;
	int getNumVisibleEnemyUnits(PlayerTypes ePlayer) const;
	int getNumVisibleEnemyCombatUnits(PlayerTypes ePlayer) const;
	int getBorderPlotCount() const;
	int getEnemyBorderPlotCount(PlayerTypes ePlayer) const;
	int getVisibleEnemyStrength(PlayerTypes ePlayer, int iRange = 0) const;
	int getVisibleNonAllyStrength(PlayerTypes ePlayer) const;

	protected:
		bool m_bDepletedMine;
		char /*PlayerTypes*/ m_eClaimingOwner;
		char* m_aiOccupationCultureRangeCities;
		void doTerritoryClaiming();
		CvWString m_szLandmarkMessage;
		CvWString m_szLandmarkName;
		LandmarkTypes m_eLandmarkType;
		bool m_bCounted;
		static stdext::hash_map<int,int>* m_resultHashMap;
#ifdef SUPPORT_MULTITHREADED_PATHING
		static CRITICAL_SECTION m_resultHashAccessSection;
#endif

	public:
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	PlayerTypes calculateCulturalOwner() const;

	void plotAction(PlotUnitFunc func, int iData1 = -1, int iData2 = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM);
	int plotCount(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, const CvUnit* pUnit = NULL, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1, int iRange = 0) const;
	CvUnit* plotCheck(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, const CvUnit* pUnit = NULL, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;
	int plotStrength(UnitValueFlags eFlags, ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1, int iRange = 0) const;
	int plotStrengthTimes100(UnitValueFlags eFlags, ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1, int iRange = 0) const;

	bool isOwned() const;																																							// Exposed to Python
	bool isBarbarian() const;																																						// Exposed to Python
	bool isNPC() const;																																					// Exposed to Python
	bool isHominid() const;																																					// Exposed to Python
	bool isRevealedBarbarian() const;																																	// Exposed to Python

	bool isVisible(TeamTypes eTeam, bool bDebug) const;																			// Exposed to Python
	DllExport bool isActiveVisible(bool bDebug) const;																								// Exposed to Python
	bool isVisibleToCivTeam() const;																																	// Exposed to Python
	bool isVisibleToWatchingHuman() const;																														// Exposed to Python
	bool isAdjacentVisible(TeamTypes eTeam, bool bDebug) const;																				// Exposed to Python
	bool isAdjacentNonvisible(TeamTypes eTeam) const;																				// Exposed to Python

	DllExport bool isGoody(TeamTypes eTeam = NO_TEAM) const;																					// Exposed to Python
	bool isRevealedGoody(TeamTypes eTeam = NO_TEAM) const;																						// Exposed to Python
	void removeGoody();																																								// Exposed to Python

	DllExport bool isCity(bool bCheckImprovement = false, TeamTypes eForTeam = NO_TEAM) const;																																		// Exposed to Python
	bool isFriendlyCity(const CvUnit& kUnit, bool bCheckImprovement) const;																												// Exposed to Python
	bool isEnemyCity(const CvUnit& kUnit, bool bOnlyRealCities = false) const;																													// Exposed to Python

	bool isOccupation() const;																																				// Exposed to Python
	bool isBeingWorked() const;																															// Exposed to Python

	bool isUnit() const;																																							// Exposed to Python
	bool isInvestigate(TeamTypes eTeam) const;																												// Exposed to Python
	bool isVisibleEnemyDefender(const CvUnit* pUnit) const;																						// Exposed to Python
	CvUnit *getVisibleEnemyDefender(PlayerTypes ePlayer) const;
	int getNumDefenders(PlayerTypes ePlayer) const;
	int getNumAttackers(PlayerTypes ePlayer) const;																													// Exposed to Python
	int getNumVisibleEnemyDefenders(const CvUnit* pUnit) const;
	int getNumCriminals() const;
	int getNumPlayerUnitAI(UnitAITypes eUnitAI, PlayerTypes ePlayer) const;
	int getNumVisibleWantedCriminals(PlayerTypes ePlayer) const;
	int getNumVisiblePotentialEnemyDefenders(const CvUnit* pUnit) const;
	int getNumVisiblePotentialEnemyDefenderless(const CvUnit* pUnit) const;
	bool isVisiblePotentialEnemyDefender(const CvUnit* pUnit) const;
	bool isVisiblePotentialEnemyDefenderless(const CvUnit* pUnit) const;
	DllExport bool isVisibleEnemyUnit(PlayerTypes ePlayer) const;																			// Exposed to Python
	DllExport int getNumVisibleUnits(PlayerTypes ePlayer) const;
	bool isVisibleEnemyUnit(const CvUnit* pUnit) const;
	bool isVisibleOtherUnit(PlayerTypes ePlayer) const;																								// Exposed to Python
	DllExport bool isFighting() const;																																// Exposed to Python

/************************************************************************************************/
/* Afforess	                  Start		 01/31/10                                               */
/*                                                                                              */
/*   New parameter                                                                              */
/************************************************************************************************/
	bool canHaveFeature(FeatureTypes eFeature, bool bOverExistingFeature = false) const;																				// Exposed to Python
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/																			// Exposed to Python

	DllExport bool isRoute() const;																																		// Exposed to Python
	bool isValidRoute(const CvUnit* pUnit) const;																											// Exposed to Python
	bool isTradeNetworkImpassable(TeamTypes eTeam) const;																														// Exposed to Python
	bool isNetworkTerrain(TeamTypes eTeam) const;																											// Exposed to Python
	bool isBonusNetwork(TeamTypes eTeam) const;																												// Exposed to Python
	bool isTradeNetwork(TeamTypes eTeam) const;																												// Exposed to Python
	bool isTradeNetworkConnected(const CvPlot * pPlot, TeamTypes eTeam) const;												// Exposed to Python
	bool isRiverNetwork(TeamTypes eTeam) const;

	bool isValidDomainForLocation(const CvUnit& unit) const;																					// Exposed to Python
	bool isValidDomainForAction(const CvUnit& unit) const;																						// Exposed to Python
/************************************************************************************************/
/* Afforess	Mountains Start		 09/18/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	//bool isImpassable() const;																														// Exposed to Python
	bool isImpassable(TeamTypes eTeam = NO_TEAM) const;
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/

	DllExport int getViewportX() const;
	inline int getX() const
	{
		return m_iX;
	}
	inline int getX_INLINE() const
	{
		return m_iX;
	}
	DllExport int getViewportY() const;																																				// Exposed to Python
	inline int getY() const
	{
		return m_iY;
	}
	inline int getY_INLINE() const
	{
		return m_iY;
	}
	bool isInViewport(int comfortBorderSize = 0) const;
	static void EvictGraphicsIfNecessary();
	void setShouldHaveFullGraphics(bool bShouldHaveFullGraphics);
	bool shouldHaveFullGraphics() const;
	bool shouldHaveGraphics() const;
	bool at(int iX, int iY) const;																																		// Exposed to Python
// BUG - Lat/Long Coordinates - start
	int calculateMinutes(int iPlotIndex, int iPlotCount, bool bWrap, int iDegreeMin, int iDegreeMax) const;
	int getLongitudeMinutes() const;																																		// Exposed to Python
	int getLatitudeMinutes() const;																																		// Exposed to Python
// BUG - Lat/Long Coordinates - end
	int getLatitude() const;																																					// Exposed to Python  
	int getLatitudeRaw() const;
	int getLongitude() const;																																					// Exposed to Python  
	int getLongitudeRaw() const;
	int getFOWIndex() const;

	CvArea* area() const;																																							// Exposed to Python
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						01/02/09		jdog5000		*/
/* 																			*/
/* 	General AI																*/
/********************************************************************************/
/* original BTS code
	CvArea* waterArea() const;
*/
	CvArea* waterArea(bool bNoImpassable = false) const;
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/	

	CvArea* secondWaterArea() const;
	int getArea() const;																																		// Exposed to Python
	void setArea(int iNewValue);			

	DllExport int getFeatureVariety() const;																													// Exposed to Python

	int getOwnershipDuration() const;																																	// Exposed to Python
	bool isOwnershipScore() const;																																		// Exposed to Python
	void setOwnershipDuration(int iNewValue);																													// Exposed to Python
	void changeOwnershipDuration(int iChange);																												// Exposed to Python

	int getImprovementDuration() const;																																// Exposed to Python
	void setImprovementDuration(int iNewValue);																												// Exposed to Python
	void changeImprovementDuration(int iChange);																											// Exposed to Python

	int getUpgradeProgressHundredths() const;																													// Exposed to Python
	int getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const;				// Exposed to Python
	void setUpgradeProgressHundredths(int iNewValue);																														// Exposed to Python
	void changeUpgradeProgressHundredths(int iChange);																													// Exposed to Python

	int getForceUnownedTimer() const;																																	// Exposed to Python
	bool isForceUnowned() const;																																			// Exposed to Python
	void setForceUnownedTimer(int iNewValue);																													// Exposed to Python
	void changeForceUnownedTimer(int iChange);																												// Exposed to Python

	int getCityRadiusCount() const;																																		// Exposed to Python
	int isCityRadius() const;																																					// Exposed to Python
	void changeCityRadiusCount(int iChange);

	bool isStartingPlot() const;																																			// Exposed to Python
	void setStartingPlot(bool bNewValue);																															// Exposed to Python
	
	DllExport bool isNOfRiver() const;																																// Exposed to Python					
	void setNOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);											// Exposed to Python					
																																																		
	DllExport bool isWOfRiver() const;																																// Exposed to Python					
	void setWOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);											// Exposed to Python					
																																																		
	DllExport CardinalDirectionTypes getRiverNSDirection() const;																			// Exposed to Python					
	DllExport CardinalDirectionTypes getRiverWEDirection() const;																			// Exposed to Python					

	CvPlot* getInlandCorner() const;																																	// Exposed to Python
	bool hasCoastAtSECorner() const;

	bool isIrrigated() const;																																					// Exposed to Python
	void setIrrigated(bool bNewValue);
	void updateIrrigated();

	bool isPotentialCityWork() const;																																						// Exposed to Python
	bool isPotentialCityWorkForArea(CvArea* pArea) const;																												// Exposed to Python
	void updatePotentialCityWork();

	bool isShowCitySymbols() const;
	void updateShowCitySymbols();

	bool isFlagDirty() const;																																										// Exposed to Python
	void setFlagDirty(bool bNewValue);																																					// Exposed to Python

	DllExport PlayerTypes getOwner() const;																																			// Exposed to Python
#ifdef _USRDLL
	inline PlayerTypes getOwnerINLINE() const
	{
		return (PlayerTypes)m_eOwner;
	}
#endif
	void setOwner(PlayerTypes eNewValue, bool bCheckUnits, bool bUpdatePlotGroup);
	static void	setDeferredPlotGroupRecalculationMode(bool bDefer);

	PlotTypes getPlotType() const;																																			// Exposed to Python
	bool isWater() const;																																								// Exposed to Python
	DllExport bool isWaterExternal() const;																																								// Exposed to Python
	bool isFlatlands() const;																																											// Exposed to Python
	DllExport bool isHills() const;																																			// Exposed to Python
	DllExport bool isPeak() const;																																							// Exposed to Python
	bool isPeak2(bool bCountsAs = false) const;																																								// Exposed to Python
	void setPlotType(PlotTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);			// Exposed to Python

	DllExport TerrainTypes getTerrainTypeExternal() const;																																	// Exposed to Python
	TerrainTypes getTerrainType() const;																																	// Exposed to Python
	void setTerrainType(TerrainTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);	// Exposed to Python

	DllExport FeatureTypes getFeatureTypeExternal() const;																																	// Exposed to Python
	FeatureTypes getFeatureType() const;																																	// Exposed to Python
	void setFeatureType(FeatureTypes eNewValue, int iVariety = -1, bool bImprovementSet = false);																				// Exposed to Python
	void setFeatureDummyVisibility(const char *dummyTag, bool show);																				// Exposed to Python
	void addFeatureDummyModel(const char *dummyTag, const char *modelTag);
	void setFeatureDummyTexture(const char *dummyTag, const char *textureTag);
	CvString pickFeatureDummyTag(int mouseX, int mouseY);
	void resetFeatureModel();

	DllExport BonusTypes getBonusType(TeamTypes eTeam = NO_TEAM) const;																							// Exposed to Python
	BonusTypes getNonObsoleteBonusType(TeamTypes eTeam = NO_TEAM) const;																	// Exposed to Python
	void setBonusType(BonusTypes eNewValue);																															// Exposed to Python

	void setImprovementCurrentValue();
	int getImprovementCurrentValue() const;

	DllExport ImprovementTypes getImprovementType() const;																													// Exposed to Python
	void setImprovementType(ImprovementTypes eNewValue);																									// Exposed to Python

	RouteTypes getRouteType() const;																																			// Exposed to Python
	void setRouteType(RouteTypes eNewValue, bool bUpdatePlotGroup);																															// Exposed to Python
	void updateCityRoute(bool bUpdatePlotGroup);

	DllExport CvCity* getPlotCity() const;																																					// Exposed to Python
	void setPlotCity(CvCity* pNewValue);

	CvCity* getWorkingCity() const;																																				// Exposed to Python
	void updateWorkingCity();

	CvCity* getWorkingCityOverride() const;																															// Exposed to Python
	void setWorkingCityOverride( const CvCity* pNewValue);

	int getRiverID() const;																																							// Exposed to Python
	void setRiverID(int iNewValue);																																			// Exposed to Python

	int getMinOriginalStartDist() const;																																// Exposed to Python
	void setMinOriginalStartDist(int iNewValue);

	int getReconCount() const;																																					// Exposed to Python
	void changeReconCount(int iChange);

	int getRiverCrossingCount() const;																																	// Exposed to Python
	void changeRiverCrossingCount(int iChange);

	short* getYield();
	DllExport int getYield(YieldTypes eIndex) const;																										// Exposed to Python
	int calculateNatureYield(YieldTypes eIndex, TeamTypes eTeam, bool bIgnoreFeature = false) const;		// Exposed to Python
	int calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const;															// Exposed to Python
	int calculateTotalBestNatureYield(TeamTypes eTeam) const;																						// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/06/09                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
	int calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal = false, bool bBestRoute = false) const;	// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	int calculateYield(YieldTypes eIndex, bool bDisplay = false) const;												// Exposed to Python
	bool hasYield() const;																																		// Exposed to Python
	void updateYield();
	int calculateMaxYield(YieldTypes eYield) const;
	int getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade) const;

	int getCulture(PlayerTypes eIndex) const;																									// Exposed to Python
	int countTotalCulture() const;																														// Exposed to Python
	int countFriendlyCulture(TeamTypes eTeam) const;
	TeamTypes findHighestCultureTeam() const;																														// Exposed to Python
	PlayerTypes findHighestCulturePlayer() const;
	int calculateCulturePercent(PlayerTypes eIndex) const;																		// Exposed to Python
	int calculateTeamCulturePercent(TeamTypes eIndex) const;																						// Exposed to Python
	void setCulture(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups);																		// Exposed to Python
	void changeCulture(PlayerTypes eIndex, int iChange, bool bUpdate);																	// Exposed to Python
	int countNumAirUnits(TeamTypes eTeam) const;
	int countNumAirUnitCargoVolume(TeamTypes eTeam) const;
	int airUnitSpaceAvailable(TeamTypes eTeam) const;
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						10/17/08		jdog5000		*/
/* 																			*/
/* 	Air AI																	*/
/********************************************************************************/
	int countAirInterceptorsActive(TeamTypes eTeam) const;
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/

	int getFoundValue(PlayerTypes eIndex);																															// Exposed to Python
	bool isBestAdjacentFound(PlayerTypes eIndex);																												// Exposed to Python
	void clearFoundValue(PlayerTypes eIndex);
	void setFoundValue(PlayerTypes eIndex, int iNewValue);

	int getPlayerCityRadiusCount(PlayerTypes eIndex) const;																							// Exposed to Python
	bool isPlayerCityRadius(PlayerTypes eIndex) const;																									// Exposed to Python
	void changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange);

	int getPlotGroupId(PlayerTypes ePlayer) const;
	CvPlotGroup* getPlotGroup(PlayerTypes ePlayer) const;
	CvPlotGroup* getOwnerPlotGroup() const;
	void setPlotGroup(PlayerTypes ePlayer, CvPlotGroup* pNewValue, bool bRecalculateBonuses = true);
	void updatePlotGroup();
	void updatePlotGroup(PlayerTypes ePlayer, bool bRecalculate = true, bool bRecalculateBonuses = true);

	void clearVisibilityCounts();
	int getVisibilityCount(TeamTypes eTeam) const;																											// Exposed to Python
	void changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bUpdatePlotGroups, int iIntensity = 0, int iUnitID = 0);							// Exposed to Python

	int getDangerCount(int /*PlayerTypes*/ ePlayer);
	void setDangerCount(int /*PlayerTypes*/ ePlayer, int iNewCount);

	int getStolenVisibilityCount(TeamTypes eTeam) const;																								// Exposed to Python
	void changeStolenVisibilityCount(TeamTypes eTeam, int iChange);

	int getLastVisibleTurn(TeamTypes eTeam) const;
	void setLastVisibleTurn(TeamTypes eTeam, short turn);

	int getBlockadedCount(TeamTypes eTeam) const;																								// Exposed to Python
	void changeBlockadedCount(TeamTypes eTeam, int iChange);
	void resetBlockadedCounts();

	DllExport PlayerTypes getRevealedOwner(TeamTypes eTeam, bool bDebug) const;													// Exposed to Python
	TeamTypes getRevealedTeam(TeamTypes eTeam, bool bDebug) const;														// Exposed to Python
	void setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue);
	void updateRevealedOwner(TeamTypes eTeam);

	DllExport bool isRiverCrossing(DirectionTypes eIndex) const;																				// Exposed to Python
	void updateRiverCrossing(DirectionTypes eIndex);
	void updateRiverCrossing();

	DllExport bool isRevealed(TeamTypes eTeam, bool bDebug) const;																								// Exposed to Python
	void setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly, TeamTypes eFromTeam, bool bUpdatePlotGroup);	// Exposed to Python
	bool isAdjacentRevealed(TeamTypes eTeam, bool bDebug = false) const;																				// Exposed to Python
	bool isAdjacentNonrevealed(TeamTypes eTeam) const;																				// Exposed to Python

	ImprovementTypes getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const;					// Exposed to Python
	void setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue);			

	RouteTypes getRevealedRouteType(TeamTypes eTeam, bool bDebug) const;											// Exposed to Python
	void setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue);							

	int getBuildProgress(BuildTypes eBuild) const;																											// Exposed to Python  
	bool changeBuildProgress(BuildTypes eBuild, int iChange, TeamTypes eTeam = NO_TEAM);								// Exposed to Python 

// BUG - Partial Builds - start
	bool hasAnyBuildProgress() const;
// BUG - Partial Builds - end

	void updateFeatureSymbolVisibility(); 
	void updateFeatureSymbol(bool bForce = false);

	DllExport bool isLayoutDirty() const;							// The plot layout contains bonuses and improvements --- it is, like the city layout, passively computed by LSystems
	DllExport void setLayoutDirty(bool bDirty);
	DllExport bool isLayoutStateDifferent() const;
	DllExport void setLayoutStateToCurrent();
	bool updatePlotBuilder();

	DllExport void getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked);				// determines how the improvement state is shown in the engine
	DllExport void getVisibleBonusState(BonusTypes& eType, bool& bImproved, bool& bWorked);		// determines how the bonus state is shown in the engine
	bool shouldUsePlotBuilder();
	CvPlotBuilder* getPlotBuilder() { return m_pPlotBuilder; }

	DllExport CvRoute* getRouteSymbol() const;
	void updateRouteSymbol(bool bForce = false, bool bAdjacent = false);

	DllExport CvRiver* getRiverSymbol() const;
	void updateRiverSymbol(bool bForce = false, bool bAdjacent = false);
	void updateRiverSymbolArt(bool bAdjacent = true);

	CvFeature* getFeatureSymbol() const;

	DllExport CvFlagEntity* getFlagSymbol() const;
	CvFlagEntity* getFlagSymbolOffset() const;
	DllExport void updateFlagSymbol();

	DllExport CvUnit* getCenterUnit() const;
	DllExport CvUnit* getDebugCenterUnit() const;
	void setCenterUnit(CvUnit* pNewValue);

	int getCultureRangeCities(PlayerTypes eOwnerIndex, int iRangeIndex) const;														// Exposed to Python
	bool isCultureRangeCity(PlayerTypes eOwnerIndex, int iRangeIndex) const;															// Exposed to Python
	//void changeCultureRangeCities(PlayerTypes eOwnerIndex, int iRangeIndex, int iChange, bool bUpdatePlotGroups);

	int getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const;										// Exposed to Python
	bool isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible) const;														// Exposed to Python
	void changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange, int iIntensity, int iUnitID = 0);					// Exposed to Python

	int getNumPlotTeamVisibilityIntensity() const;
	PlotTeamVisibilityIntensity& getPlotTeamVisibilityIntensity(int iIndex);
	int getNumPlotTeamVisibilityIntensityCount(InvisibleTypes eInvisibility, TeamTypes eTeam, int iIntensity) const;
	bool hasPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam, int iIntensity) const;
	int getHighestPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam) const;
	//void setHighestPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam);

	static void	NextCachePathEpoch();
	bool HaveCachedPathValidityResult(void* entity, bool bIsAlternateResult, bool& cachedResult);
	void CachePathValidityResult(void* entity, bool bIsAlternateResult, bool cachedResult);

	int getNumUnits() const;																																		// Exposed to Python
	CvUnit* getUnitByIndex(int iIndex) const;																													// Exposed to Python
	void addUnit(CvUnit* pUnit, bool bUpdate = true);
	void removeUnit(CvUnit* pUnit, bool bUpdate = true);
	DllExport CLLNode<IDInfo>* nextUnitNode(CLLNode<IDInfo>* pNode) const;
	CLLNode<IDInfo>* prevUnitNode(CLLNode<IDInfo>* pNode) const;
	DllExport CLLNode<IDInfo>* headUnitNode() const;
	CLLNode<IDInfo>* tailUnitNode() const;

	// For iterating over units on a plot, optionally skipping invalid (NULL) ones
	class unit_iterator : public idinfo_iterator<unit_iterator, CvUnit>
	{
	public:
		typedef idinfo_iterator<unit_iterator, CvUnit> base_type;
		unit_iterator() {}
		explicit unit_iterator(const CLinkList<IDInfo>* list, bool skip_invalid = false) : base_type(list, skip_invalid) {}

	private:
		friend class core_access;
		CvUnit* resolve(const IDInfo& info) const;
	};

	unit_iterator beginValidUnits() const { return unit_iterator(&m_units, true); }
	unit_iterator endValidUnits() const { return unit_iterator(); } // Same as endUnits() currently

	unit_iterator beginUnits() const { return unit_iterator(&m_units); }
	unit_iterator endUnits() const { return unit_iterator(); }

	int getNumSymbols() const;
	CvSymbol* getSymbol(int iID) const;
	CvSymbol* addSymbol();

	void deleteSymbol(int iID);
	void deleteAllSymbols();

	// Script data needs to be a narrow string for pickling in Python
	CvString getScriptData() const;																											// Exposed to Python
	void setScriptData(const char* szNewValue);																					// Exposed to Python

	bool canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const;
	bool canApplyEvent(EventTypes eEvent) const;
	void applyEvent(EventTypes eEvent);

	bool canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible) const;

	bool isEspionageCounterSpy(TeamTypes eTeam) const;
	
	DllExport int getAreaIdForGreatWall() const;
	DllExport int getSoundScriptId() const;
	DllExport int get3DAudioScriptFootstepIndex(int iFootstepTag) const;
	DllExport float getAqueductSourceWeight() const;  // used to place aqueducts on the map
	DllExport bool shouldDisplayBridge(CvPlot* pToPlot, PlayerTypes ePlayer) const;
	DllExport bool checkLateEra() const;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

protected:
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	bool m_bNull;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/	
	short m_iX;
	short m_iY;
	int m_iArea;
	mutable CvArea *m_pPlotArea;
	short m_iFeatureVariety;
	short m_iOwnershipDuration;
	short m_iImprovementDuration;
	short m_iUpgradeProgress;
	short m_iForceUnownedTimer;
	short m_iCityRadiusCount;
	int m_iRiverID;
	short m_iMinOriginalStartDist;
	short m_iReconCount;
	short m_iRiverCrossingCount;
	mutable int m_iCanHaveImprovementAsUpgradeCache;
	mutable int m_iCurrentRoundofUpgradeCache;
	int m_iImprovementCurrentValue;

	// Super Forts begin *canal* *choke*
	int m_iCanalValue;
	int m_iChokeValue;
	// Super Forts end
	// Super Forts begin *bombard*
	int m_iDefenseDamage;
	bool m_bBombarded;
	// Super Forts end

	bool m_bStartingPlot:1;
	bool m_bHills:1;
	bool m_bNOfRiver:1;
	bool m_bWOfRiver:1;
	bool m_bIrrigated:1;
	bool m_bPotentialCityWork:1;
	bool m_bShowCitySymbols:1;
	bool m_bFlagDirty:1;
	bool m_bPlotLayoutDirty:1;
	bool m_bLayoutStateWorked:1;

	char /*PlayerTypes*/ m_eOwner;
	short /*PlotTypes*/ m_ePlotType;
	short /*TerrainTypes*/ m_eTerrainType;
	short /*FeatureTypes*/ m_eFeatureType;
	short /*BonusTypes*/ m_eBonusType;
	short /*ImprovementTypes*/ m_eImprovementType;
	short /*RouteTypes*/ m_eRouteType;
	char /*CardinalDirectionTypes*/ m_eRiverNSDirection;
	char /*CardinalDirectionTypes*/ m_eRiverWEDirection;

	IDInfo m_plotCity;
	IDInfo m_workingCity;
	IDInfo m_workingCityOverride;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// Plot danger cache
	bool m_bIsActivePlayerHasDangerCache;
	bool m_bIsActivePlayerNoDangerCache;
	bool* m_abIsTeamBorderCache;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	static	int m_iGlobalCachePathEpoch;
	int		m_iCachePathEpoch;
	void*	m_cachedPathValidityEntity;
	void*	m_cachedPathAlternateValidityEntity;
	bool	m_cachedPathAlternateValidity;
	bool	m_cachedPathValidity;

	// Super Forts begin *culture*
	//	Koshling - not needed in the C2C implemenattion due to AND's existing
	//	ellision of fort and city culture
	//short* m_aiCultureRangeForts;
	// Super Forts end

	short* m_aiYield;
	std::vector<std::pair<PlayerTypes,int> > m_aiCulture;
	std::vector<PlotTeamVisibilityIntensity> m_aPlotTeamVisibilityIntensity;
	unsigned int* m_aiFoundValue;
	char* m_aiPlayerCityRadiusCount;
	int* m_aiPlotGroup;			// IDs - keep as int
	short* m_aiVisibilityCount;
	short* m_aiStolenVisibilityCount;
	short* m_aiBlockadedCount;
	char* m_aiRevealedOwner;

	bool* m_abRiverCrossing;	// bit vector
	bool* m_abRevealed;

	short* /*ImprovementTypes*/ m_aeRevealedImprovementType;
	short* /*RouteTypes*/ m_aeRevealedRouteType;

	char* m_szScriptData;

	int* m_paiBuildProgress;

	short* m_aiLastSeenTurn;

	//	This array will be sparsely populated on most plots so not worth
	//	an indirection, but look to find a way to use the sparseness to reduce memory usage
	//	at some point
	short* m_aiDangerCount;

	CvFeature* m_pFeatureSymbol;
	CvRoute* m_pRouteSymbol;
	CvRiver* m_pRiverSymbol;
	CvFlagEntity* m_pFlagSymbol;
	CvFlagEntity* m_pFlagSymbolOffset;
	CvUnit* m_pCenterUnit;

	CvPlotBuilder* m_pPlotBuilder;		// builds bonuses and improvements

	char** m_apaiCultureRangeCities;
	short** m_apaiInvisibleVisibilityCount;
	//short** m_apaiCachedHighestTeamInvisibilityIntensity;

	//	Koshling - need to cache presence of mountain leaders in mountain
	//	plots so that CanMoveThrough calculations don't get bogged down
	//	searching unit stacks.  This is a count of mountain leader units
	//	in the plot for each team.  The array is only created if the plot is
	//	a mountain plot and at least one team has a mountain leader present.
	//	The array is cleared when the last leader leaves, so the memory overhead is
	//	low
	short*	m_aiMountainLeaderCount;

	CLinkList<IDInfo> m_units;

	std::vector<CvSymbol*> m_symbols;

	void doFeature();
	void doCulture();

	void processArea(CvArea* pArea, int iChange);
	void doImprovementUpgrade();

	ColorTypes plotMinimapColor();

	// added so under cheat mode we can access protected stuff
	friend class CvGameTextMgr;

// From Lead From Behind by UncutDragon
public:
	bool hasDefender(bool bCheckCanAttack, PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false, bool bTestCanFight = false) const;
	bool hasStealthDefender(const CvUnit* pAttacker) const;
	void revealBestStealthDefender(const CvUnit* pAttacker);
	void doPreAttackTraps(CvUnit* pAttacker);


#ifdef CAN_BUILD_VALUE_CACHING
public:
	static void ClearCanBuildCache();

private:
	static canBuildCache g_canBuildCache;
	static int canBuildCacheHits;
	static int canBuildCacheReads;
#endif

	//	Koshling - add Zobrist hashing of plotGroups to reduce recalculation.
	//	Each plot has a contribution value to any hash it is included in
private:
	int	m_zobristContribution;
	int m_movementCharacteristicsHash;
	bool m_bPlotGroupsDirty;

	static bool bDeferPlotGroupRecalculation;

	bool updateSymbolsInternal();

protected:
	//	Directly accessed cached pathing info per plot for performance reasons
	volatile mutable int				m_pathGenerationSeq;
	mutable CvPathGeneratorPlotInfo*	m_currentPathInfo;

public:
	int m_groupGenerationNumber;
	//	Toggle plot in/out of contribution
	void ToggleInPlotGroupsZobristContributors();

	inline int getZobristContribution() const
	{
		return m_zobristContribution;
	}

	inline int getMovementCharacteristicsHash() const { return m_movementCharacteristicsHash; }

	//TB Combat Mod AI
	int getNumAfflictedUnits(PlayerTypes eOwner, PromotionLineTypes eAfflictionLine) const;

	bool isPlotIgnoringImprovementUpgrade() const;
	void setPlotIgnoringImprovementUpgrade(bool bNewValue);
	void setImprovementUpgrade();

	int getInjuredUnitCombatsUnsupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain = NO_DOMAIN) const;
	int getUnitCombatsUnsupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain = NO_DOMAIN) const;
	int getOverloadedUnitCombatsSupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain = NO_DOMAIN) const;

	void unitGameStateCorrections();

	bool isMapCategoryType(MapCategoryTypes eIndex) const;

	int countSeeInvisibleActive(PlayerTypes ePlayer, InvisibleTypes eVisible) const;

	int getCommunicability(PromotionLineTypes ePromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume) const;

protected:
	// AIAndy: Properties
	CvProperties m_Properties;
public:
	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

	void pageGraphicsOut();
	static void notePageRenderStart(int iRenderArea);

protected:
	bool m_bInhibitCenterUnitCalculation;
	bool m_bIgnoringImprovementUpgrade;
	short m_iGraphicsPageIndex;
};

#endif
