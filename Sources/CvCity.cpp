// city.cpp

#include "CvGameCoreDLL.h"
#include "CvReachablePlotSet.h"

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
#include "BetterBTSAI.h"
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

#include <boost/bind.hpp>

//Disable this passed in initialization list warning, as it is only stored in the constructor of CvBuildingList and not used
#pragma warning( disable : 4355 )

// Public Functions...

CvCity::CvCity()
: m_GameObject(this),
m_BuildingList(NULL, this),
m_UnitList(NULL, this),
m_Properties(this)
{
	m_aiSeaPlotYield = new int[NUM_YIELD_TYPES];
	m_aiRiverPlotYield = new int[NUM_YIELD_TYPES];
	m_aiBaseYieldRate = new int[NUM_YIELD_TYPES];
	m_aiBaseYieldPerPopRate = new int[NUM_YIELD_TYPES];
	m_aiYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiPowerYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiBonusYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiTradeYield = new int[NUM_YIELD_TYPES];
	m_aiCorporationYield = new int[NUM_YIELD_TYPES];
	m_aiExtraSpecialistYield = new int[NUM_YIELD_TYPES];
	m_aiExtraSpecialistCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRate = new int[NUM_COMMERCE_TYPES];
	m_abCommerceRateDirty = new bool[NUM_COMMERCE_TYPES];
	m_aiProductionToCommerceModifier = new int[NUM_COMMERCE_TYPES];
	m_aiBuildingCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiSpecialistCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiReligionCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCorporationCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceHappinessPer = new int[NUM_COMMERCE_TYPES];
	m_aiDomainFreeExperience = new int[NUM_DOMAIN_TYPES];
	m_aiDomainProductionModifier = new int[NUM_DOMAIN_TYPES];

	m_aiCulture = new int[MAX_PLAYERS];
	m_aiNumRevolts = new int[MAX_PLAYERS];
	m_abEverOwned = new bool[MAX_PLAYERS];
	m_abTradeRoute = new bool[MAX_PLAYERS];
	m_abRevealed = new bool[MAX_TEAMS];
	m_abEspionageVisibility = new bool[MAX_TEAMS];

	m_paiNoBonus = NULL;
	m_paiFreeBonus = NULL;
	m_paiNumBonuses = NULL;
	m_paiNumCorpProducedBonuses = NULL;
	m_paiProjectProduction = NULL;
	m_paiBuildingProduction = NULL;
	m_paiBuildingProductionTime = NULL;
	m_paiBuildingOriginalOwner = NULL;
	m_paiBuildingOriginalTime = NULL;
	m_paiUnitProduction = NULL;
	m_paiUnitProductionTime = NULL;
	m_paiGreatPeopleUnitRate = NULL;
	m_paiGreatPeopleUnitProgress = NULL;
	m_paiSpecialistCount = NULL;
	m_paiMaxSpecialistCount = NULL;
	m_paiForceSpecialistCount = NULL;
	m_paiFreeSpecialistCount = NULL;
	m_paiFreeSpecialistCountUnattributed = NULL;
	m_paiImprovementFreeSpecialists = NULL;
	m_paiReligionInfluence = NULL;
	m_paiStateReligionHappiness = NULL;
	m_paiUnitCombatFreeExperience = NULL;
	m_paiFreePromotionCount = NULL;
	m_paiNumRealBuilding = NULL;
	m_paiNumFreeBuilding = NULL;
	m_paiNumFreeAreaBuilding = NULL;
	m_paiBuildingReplaced = NULL;
	m_bHasCalculatedBuildingReplacement = false;
	m_bPropertyControlBuildingQueued = false;

	m_pabWorkingPlot = NULL;
	m_pabHasReligion = NULL;
	m_pabHasCorporation = NULL;
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_paiUnitClassProductionModifier = NULL;
	m_aiBonusCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiBonusCommercePercentChanges = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceAttacks = new int[NUM_COMMERCE_TYPES];
	m_aiMaxCommerceAttacks = new int[NUM_COMMERCE_TYPES];
	m_paiBuildingClassProductionModifier = NULL;
	m_paiBonusDefenseChanges = NULL;
	m_cachedPropertyNeeds = NULL;
	m_pabHadVicinityBonus = NULL;
	m_pabHasVicinityBonus = NULL;
	m_pabHadRawVicinityBonus = NULL;
	m_pabHasRawVicinityBonus = NULL;
	m_pabHasVicinityBonusCached = NULL;
	m_pabHasRawVicinityBonusCached = NULL;
	m_pabDisabledBuilding = NULL;
	m_paiUnitCombatExtraStrength = NULL;
	m_pabAutomatedCanBuild = NULL;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	m_paTradeCities = NULL;
	//TB Combat Mod (Buildings) begin
	m_paiNewAfflictionTypeCount = NULL;
	m_paiAidRate = NULL;
	m_ppaaiExtraBonusAidModifier = NULL;
	m_paiNewExtraAfflictionOutbreakLevelChange = NULL;
	m_paiNewAfflictionToleranceChange = NULL;
	m_paiNewCurrentOvercomeChange = NULL;
	m_paiUnitCombatProductionModifier = NULL;
	m_paiUnitCombatRepelModifier = NULL;
	m_paiUnitCombatRepelAgainstModifier = NULL;
	m_paiUnitCombatDefenseAgainstModifier = NULL;
	m_paiPromotionLineAfflictionAttackCommunicability = NULL;
	m_paiUnitCombatOngoingTrainingTimeCount = NULL;
	m_paiUnitCombatOngoingTrainingTimeIncrement = NULL;
	//TB Combat Mod (Buildings) end
	//Team Project (1)
	m_paiTechHappiness = NULL;
	m_paiTechSpecialistHappiness = NULL;
	m_paiTechHealth = NULL;
	m_paiTechSpecialistHealth = NULL;
	m_ppaaiTechSpecialistHappinessTypes = NULL;
	m_ppaaiTechSpecialistHealthTypes = NULL;
	m_ppaaiLocalSpecialistExtraYield = NULL;
	m_ppaaiLocalSpecialistExtraCommerce = NULL;
//Team Project (5)
	m_pabReligiouslyDisabledBuilding = NULL;
	m_paiSpecialistBannedCount = NULL;
	m_paiDamageAttackingUnitCombatCount = NULL;
	m_paiBuildingCostPopulationCount = NULL;
	m_paiHealUnitCombatTypeVolume = NULL;

#ifdef CAN_TRAIN_CACHING
	//	KOSHLING - clear canTrain cache validity
	m_canTrainCachePopulated = false;
#endif
	m_bVisibilitySetup = false;
	m_bCanConstruct = NULL;

	//CvDLLEntity::createCityEntity(this);		// create and attach entity to city

	m_aiBaseYieldRank = new int[NUM_YIELD_TYPES];
	m_abBaseYieldRankValid = new bool[NUM_YIELD_TYPES];
	m_aiYieldRank = new int[NUM_YIELD_TYPES];
	m_abYieldRankValid = new bool[NUM_YIELD_TYPES];
	m_aiCommerceRank = new int[NUM_COMMERCE_TYPES];
	m_abCommerceRankValid = new bool[NUM_COMMERCE_TYPES];

	m_deferringBonusProcessingCount = 0;
	m_paiStartDeferredSectionNumBonuses = NULL;
	
	reset(0, NO_PLAYER, 0, 0, true);
}

CvCity::~CvCity()
{
	if ( getEntity() != NULL )
	{
		CvDLLEntity::removeEntity();			// remove entity from engine
		CvDLLEntity::destroyEntity();			// delete CvCityEntity and detach from us
	}

	uninit();

	SAFE_DELETE_ARRAY(m_aiBaseYieldRank);
	SAFE_DELETE_ARRAY(m_abBaseYieldRankValid);
	SAFE_DELETE_ARRAY(m_aiYieldRank);
	SAFE_DELETE_ARRAY(m_abYieldRankValid);
	SAFE_DELETE_ARRAY(m_aiCommerceRank);
	SAFE_DELETE_ARRAY(m_abCommerceRankValid);

	SAFE_DELETE_ARRAY(m_aiSeaPlotYield);
	SAFE_DELETE_ARRAY(m_aiRiverPlotYield);
	SAFE_DELETE_ARRAY(m_aiBaseYieldRate);
	SAFE_DELETE_ARRAY(m_aiBaseYieldPerPopRate);
	SAFE_DELETE_ARRAY(m_aiYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiPowerYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiBonusYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiTradeYield);
	SAFE_DELETE_ARRAY(m_aiCorporationYield);
	SAFE_DELETE_ARRAY(m_aiExtraSpecialistYield);
	SAFE_DELETE_ARRAY(m_aiExtraSpecialistCommerce);
	SAFE_DELETE_ARRAY(m_aiCommerceRate);
	SAFE_DELETE_ARRAY(m_abCommerceRateDirty);
	SAFE_DELETE_ARRAY(m_aiProductionToCommerceModifier);
	SAFE_DELETE_ARRAY(m_aiBuildingCommerce);
	SAFE_DELETE_ARRAY(m_aiSpecialistCommerce);
	SAFE_DELETE_ARRAY(m_aiReligionCommerce);
	SAFE_DELETE_ARRAY(m_aiCorporationCommerce);
	SAFE_DELETE_ARRAY(m_aiCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiCommerceHappinessPer);
	SAFE_DELETE_ARRAY(m_aiDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_aiDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_aiCulture);
	SAFE_DELETE_ARRAY(m_aiNumRevolts);
	SAFE_DELETE_ARRAY(m_abEverOwned);
	SAFE_DELETE_ARRAY(m_abTradeRoute);
	SAFE_DELETE_ARRAY(m_abRevealed);
	SAFE_DELETE_ARRAY(m_abEspionageVisibility);
/************************************************************************************************/
/* Afforess	                  Start		 05/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_aiBonusCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiBonusCommercePercentChanges);
	SAFE_DELETE_ARRAY(m_aiCommerceAttacks);
	SAFE_DELETE_ARRAY(m_aiMaxCommerceAttacks);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

}


void CvCity::init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();


	//--------------------------------
	// Log this event
	if (GC.getLogging())
	{
		if (gDLL->getChtLvl() > 0)
		{
			TCHAR szOut[1024];
			sprintf(szOut, "Player %d City %d built at %d:%d\n", eOwner, iID, iX, iY);
			gDLL->messageControlLog(szOut);
		}
	}

	CvPlot* pPlot = GC.getMapINLINE().plotINLINE(iX, iY);
	//--------------------------------
	// Init saved data
	reset(iID, eOwner, pPlot->getX_INLINE(), pPlot->getY_INLINE());

	//--------------------------------
	// Init non-saved data
	setupGraphical();

	//--------------------------------
	// Init other game data
/************************************************************************************************/
/* Afforess	                  Start		 04/13/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool bFound = false;
	if (GC.getGameINLINE().isOption(GAMEOPTION_PERSONALIZED_MAP) && GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_USE_LANDMARK_NAMES))
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS_2; iI++)
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iI);
			if (!pLoopPlot->getLandmarkName().empty() && pLoopPlot->getLandmarkType() != NO_LANDMARK)
			{
				setName(pLoopPlot->getLandmarkName());
				if (!getName().empty())
				{
					bFound = true;
					break;
				}
			}
		}
	}
	if (!bFound)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	setName(GET_PLAYER(getOwnerINLINE()).getNewCityName());

	setEverOwned(getOwnerINLINE(), true);

	pPlot->setImprovementType(NO_IMPROVEMENT);
	pPlot->setOwner(getOwnerINLINE(), bBumpUnits, false);
	pPlot->setPlotCity(this);

	updateCultureLevel(false);

	if (pPlot->getCulture(getOwnerINLINE()) < GC.getDefineINT("FREE_CITY_CULTURE"))
	{
		pPlot->setCulture(getOwnerINLINE(), GC.getDefineINT("FREE_CITY_CULTURE"), bBumpUnits, false);
	}

	if (!GC.getGameINLINE().isOption(GAMEOPTION_1_CITY_TILE_FOUNDING))
	{
		int iAdjCulture = GC.getDefineINT("FREE_CITY_ADJACENT_CULTURE");
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->getCulture(getOwnerINLINE()) < iAdjCulture)
				{
					pAdjacentPlot->setCulture(getOwnerINLINE(), iAdjCulture, bBumpUnits, false);
				}
				pAdjacentPlot->updateCulture(bBumpUnits, false);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
		{
			pPlot->changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), true, NULL, false);
		}
	}

	long lResult=0;

	CyArgsList argsList;
	argsList.add(iX);
	argsList.add(iY);
	PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "citiesDestroyFeatures", argsList.makeFunctionArgs(), &lResult);

	if (lResult == 1)
	{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       10/30/09                     Mongoose & jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* original bts code
		if (pPlot->getFeatureType() != NO_FEATURE)
*/
		// From Mongoose SDK
		// Don't remove floodplains from tiles when founding city
		if ((pPlot->getFeatureType() != NO_FEATURE) && (pPlot->getFeatureType() != (FeatureTypes)GC.getInfoTypeForString("FEATURE_FLOOD_PLAINS")))
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
		{
/************************************************************************************************/
/* Afforess	                  Start		 02/09/10                                               */
/*                                                                                              */
/*  Extra Hammer from Settling on Forest, Extra Food from Settling on Jungle                    */
/************************************************************************************************/
			BuildTypes eChopBuild = findChopBuild(pPlot->getFeatureType());
			if (eChopBuild != NO_BUILD)
			{
				if ( (GC.getInfoTypeForString("FEATURE_FOREST", true) > 0 &&
					  pPlot->getFeatureType() == (FeatureTypes)GC.getInfoTypeForString("FEATURE_FOREST")) ||
					 (GC.getInfoTypeForString("FEATURE_JUNGLE", true) > 0 &&
					  pPlot->getFeatureType() == (FeatureTypes)GC.getInfoTypeForString("FEATURE_JUNGLE")))
				{
					int iProduction;
					iProduction = GC.getBuildInfo(eChopBuild).getFeatureProduction(pPlot->getFeatureType());
				
					iProduction *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFeatureProductionPercent();
					iProduction /= 100;
					setExtraYieldTurns(iProduction);
				}
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				int iPopDestroys = GC.getFeatureInfo(pPlot->getFeatureType()).getPopDestroys();
				if (iPopDestroys == 0 || iPopDestroys == 1)
				{
					pPlot->setFeatureType(NO_FEATURE);
				}
			}
		}
	}

	pPlot->updateCityRoute(false);

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (pPlot->isVisible(((TeamTypes)iI), false))
			{
				setRevealed(((TeamTypes)iI), true);
			}
		}
	}

	changeMilitaryHappinessUnits(pPlot->plotCount(PUF_isMilitaryHappiness));

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeCommerceHappinessPer(((CommerceTypes)iI), GC.getCommerceInfo((CommerceTypes)iI).getInitialHappiness());
	}

	//TBFREEBUILD
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GET_PLAYER(getOwnerINLINE()).isBuildingFree((BuildingTypes)iI))
		{
			setNumFreeBuilding(((BuildingTypes)iI), 1);
		}
		else if (GET_PLAYER(getOwnerINLINE()).isBuildingFree((BuildingTypes)iI, area()))
		{
			setNumFreeAreaBuilding(((BuildingTypes)iI), 1);
		}
	}

	area()->changeCitiesPerPlayer(getOwnerINLINE(), 1);

	GET_TEAM(getTeam()).changeNumCities(1);

	GC.getGameINLINE().changeNumCities(1);

	setGameTurnFounded(GC.getGameINLINE().getGameTurn());
	setGameTurnAcquired(GC.getGameINLINE().getGameTurn());

	changePopulation(GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFreePopulation());

	changeAirUnitCapacity(GC.getDefineINT("CITY_AIR_UNIT_CAPACITY"));

	updateFreshWaterHealth();
	updateFeatureHealth();
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/19/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	updateImprovementHealth();
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	updateFeatureHappiness();
	updatePowerHealth();

	GET_PLAYER(getOwnerINLINE()).setMaintenanceDirty(true);

	GC.getMapINLINE().updateWorkingCity();

	GC.getGameINLINE().AI_makeAssignWorkDirty();

	GET_PLAYER(getOwnerINLINE()).setFoundedFirstCity(true);

	if (GC.getGameINLINE().isFinalInitialized())
	{
		if (GET_PLAYER(getOwnerINLINE()).getNumCities() == 1)
		{
			for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				if (GC.getCivilizationInfo(getCivilizationType()).isCivilizationFreeBuildingClass(iI))
				{
					BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

					if (eLoopBuilding != NO_BUILDING)
					{
						setNumRealBuilding(eLoopBuilding, 1);
					}
				}
			}

			if (!isHuman())
			{
				changeOverflowProduction(GC.getDefineINT("INITIAL_AI_CITY_PRODUCTION"), 0);
			}
		}
	}

	updateEspionageVisibility(false);

	if (bUpdatePlotGroups)
	{
		GC.getGameINLINE().updatePlotGroups();
	}

/************************************************************************************************/
/* Afforess	                  Start		 01/12/10                                               */
/*                                                                                              */
/* Assimilation                                                                                 */
/************************************************************************************************/
	//	Koshliong  - do this unconditionally - it dopesn; nmatter if assimilation is off because in
	//	that case we're setting the value it would have anyway and on any change of ownership
	//	acquireCity() is called which initializes a new CvCity instance anyway
	setCivilizationType(GET_PLAYER(getOwnerINLINE()).getCivilizationType());
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	m_UnitList.init();

	AI_init();
}


void CvCity::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNoBonus);
	SAFE_DELETE_ARRAY(m_paiFreeBonus);
	SAFE_DELETE_ARRAY(m_paiNumBonuses);
	SAFE_DELETE_ARRAY(m_paiNumCorpProducedBonuses);
	SAFE_DELETE_ARRAY(m_paiProjectProduction);
	SAFE_DELETE_ARRAY(m_paiBuildingProduction);
	SAFE_DELETE_ARRAY(m_paiBuildingProductionTime);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalOwner);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalTime);
	SAFE_DELETE_ARRAY(m_paiUnitProduction);
	SAFE_DELETE_ARRAY(m_paiUnitProductionTime);
	SAFE_DELETE_ARRAY(m_paiGreatPeopleUnitRate);
	SAFE_DELETE_ARRAY(m_paiGreatPeopleUnitProgress);
	SAFE_DELETE_ARRAY(m_paiSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiMaxSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiForceSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiFreeSpecialistCountUnattributed);
	SAFE_DELETE_ARRAY(m_paiImprovementFreeSpecialists);
	SAFE_DELETE_ARRAY(m_paiReligionInfluence);
	SAFE_DELETE_ARRAY(m_paiStateReligionHappiness);
	SAFE_DELETE_ARRAY(m_paiUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_paiFreePromotionCount);
	SAFE_DELETE_ARRAY(m_paiNumRealBuilding);
	SAFE_DELETE_ARRAY(m_paiNumFreeBuilding);
	SAFE_DELETE_ARRAY(m_paiNumFreeAreaBuilding);
	SAFE_DELETE_ARRAY(m_paiBuildingReplaced);
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_paiUnitClassProductionModifier);
	SAFE_DELETE_ARRAY(m_paiBuildingClassProductionModifier);
	SAFE_DELETE_ARRAY(m_paiBonusDefenseChanges);
	SAFE_DELETE_ARRAY(m_cachedPropertyNeeds);
	SAFE_DELETE_ARRAY(m_pabHadVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHasVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHadRawVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHasRawVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHasVicinityBonusCached);
	SAFE_DELETE_ARRAY(m_pabHasRawVicinityBonusCached);
	SAFE_DELETE_ARRAY(m_pabDisabledBuilding);
	SAFE_DELETE_ARRAY(m_paiUnitCombatExtraStrength);
	SAFE_DELETE_ARRAY(m_pabAutomatedCanBuild);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_pabWorkingPlot);
	SAFE_DELETE_ARRAY(m_pabHasReligion);
	SAFE_DELETE_ARRAY(m_pabHasCorporation);

	SAFE_DELETE_ARRAY(m_paTradeCities);
	//TB Combat Mod (Buildings) begin
	SAFE_DELETE_ARRAY(m_paiNewAfflictionTypeCount);
	SAFE_DELETE_ARRAY(m_paiAidRate);
	//SAFE_DELETE_ARRAY(m_paiExtraBonusAidModifier);

	if (m_ppaaiExtraBonusAidModifier != NULL)
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiExtraBonusAidModifier[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiExtraBonusAidModifier);
	}
	SAFE_DELETE_ARRAY(m_paiNewExtraAfflictionOutbreakLevelChange);
	SAFE_DELETE_ARRAY(m_paiNewAfflictionToleranceChange);
	SAFE_DELETE_ARRAY(m_paiNewCurrentOvercomeChange);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifier);
	SAFE_DELETE_ARRAY(m_paiUnitCombatRepelModifier);
	SAFE_DELETE_ARRAY(m_paiUnitCombatRepelAgainstModifier);
	SAFE_DELETE_ARRAY(m_paiUnitCombatDefenseAgainstModifier);
	SAFE_DELETE_ARRAY(m_paiPromotionLineAfflictionAttackCommunicability);
	SAFE_DELETE_ARRAY(m_paiUnitCombatOngoingTrainingTimeCount);
	SAFE_DELETE_ARRAY(m_paiUnitCombatOngoingTrainingTimeIncrement);
	//TB Combat Mod (Buildings) end
//Team Project (5)
	SAFE_DELETE_ARRAY(m_pabReligiouslyDisabledBuilding);

	SAFE_DELETE_ARRAY(m_paiStartDeferredSectionNumBonuses);

	//Team Project (1)
	SAFE_DELETE_ARRAY(m_paiTechHappiness);
	SAFE_DELETE_ARRAY(m_paiTechSpecialistHappiness);
	SAFE_DELETE_ARRAY(m_paiTechHealth);
	SAFE_DELETE_ARRAY(m_paiSpecialistBannedCount);
	SAFE_DELETE_ARRAY(m_paiDamageAttackingUnitCombatCount);
	SAFE_DELETE_ARRAY(m_paiBuildingCostPopulationCount);
	SAFE_DELETE_ARRAY(m_paiHealUnitCombatTypeVolume);
	SAFE_DELETE_ARRAY(m_paiTechSpecialistHealth);

	if (m_ppaaiTechSpecialistHappinessTypes != NULL)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiTechSpecialistHappinessTypes[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiTechSpecialistHappinessTypes);
	}

	if (m_ppaaiTechSpecialistHealthTypes != NULL)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiTechSpecialistHealthTypes[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiTechSpecialistHealthTypes);
	}

	if (m_ppaaiLocalSpecialistExtraYield != NULL)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiLocalSpecialistExtraYield[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiLocalSpecialistExtraYield);
	}

	if (m_ppaaiLocalSpecialistExtraCommerce != NULL)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiLocalSpecialistExtraCommerce[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiLocalSpecialistExtraCommerce);
	}

	m_orderQueue.clear();
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvCity::reset(int iID, PlayerTypes eOwner, int iX, int iY, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	if (!bConstructorCall)
	{
		for(int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			setCommerceModifierDirty((CommerceTypes)iI);
		} 
	}

	m_iID = iID;
	m_iX = iX;
	m_iY = iY;
	m_iRallyX = INVALID_PLOT_COORD;
	m_iRallyY = INVALID_PLOT_COORD;
	m_iGameTurnFounded = 0;
	m_iGameTurnAcquired = 0;
	m_iPopulation = 0;
	m_iHighestPopulation = 0;
	m_iWorkingPopulation = 0;
	m_iSpecialistPopulation = 0;
	m_iNumGreatPeople = 0;
	m_iBaseGreatPeopleRate = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatPeopleProgress = 0;
	m_iNumWorldWonders = 0;
	m_iNumTeamWonders = 0;
	m_iNumNationalWonders = 0;
	m_iNumBuildings = 0;
	m_iGovernmentCenterCount = 0;
	m_iMaintenance = 0;
	m_iMaintenanceModifier = 0;
	m_iWarWearinessModifier = 0;
	m_iHurryAngerModifier = 0;
	m_iHealRate = 0;
	m_iEspionageHealthCounter = 0;
	m_iEspionageHappinessCounter = 0;
	m_iFreshWaterGoodHealth = 0;
	m_iFreshWaterBadHealth = 0;
	m_iFeatureGoodHealth = 0;
	m_iFeatureBadHealth = 0;
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iPowerGoodHealth = 0;
	m_iPowerBadHealth = 0;
	m_iBonusGoodHealth = 0;
	m_iBonusBadHealth = 0;
	m_iHurryAngerTimer = 0;
/************************************************************************************************/
/* REVOLUTION_MOD                         04/19/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_iRevRequestAngerTimer = 0;
	m_iRevSuccessTimer = 0;
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	m_iConscriptAngerTimer = 0;
	m_iDefyResolutionAngerTimer = 0;
	m_iHappinessTimer = 0;
	m_iMilitaryHappinessUnits = 0;
	m_iBuildingGoodHappiness = 0;
	m_iBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHappiness = 0;
	m_iExtraBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHealth = 0;
	m_iExtraBuildingBadHealth = 0;
	m_iFeatureGoodHappiness = 0;
	m_iFeatureBadHappiness = 0;
	m_iBonusGoodHappiness = 0;
	m_iBonusBadHappiness = 0;
	m_iReligionGoodHappiness = 0;
	m_iReligionBadHappiness = 0;
	m_iExtraHappiness = 0;
	m_iExtraHealth = 0;
	m_iNoUnhappinessCount = 0;
	m_iNoUnhealthyPopulationCount = 0;
	m_iBuildingOnlyHealthyCount = 0;
	m_iFood = 0;
	m_iFoodKept = 0;
	m_fMaxFoodKeptMultiplierLog = 0.0;
	m_iOverflowProduction = 0;
	m_iFeatureProduction = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iExtraTradeRoutes = 0;
	m_iTradeRouteModifier = 0;
	m_iForeignTradeRouteModifier = 0;
	m_iBuildingDefense = 0;
	m_iBuildingBombardDefense = 0;
	m_iFreeExperience = 0;
	m_iCurrAirlift = 0;
	m_iMaxAirlift = 0;
	m_iAirModifier = 0;
	m_iAirUnitCapacity = 0;
	m_iNukeModifier = 0;
	m_iFreeSpecialist = 0;
	m_iPowerCount = 0;
	m_iDirtyPowerCount = 0;
	m_iDefenseDamage = 0;
	m_iLastDefenseDamage = 0;
	m_iOccupationTimer = 0;
	m_iCultureUpdateTimer = 0;
	m_iCitySizeBoost = 0;
	m_iSpecialistFreeExperience = 0;
	m_iEspionageDefenseModifier = 0;
	m_iSpecialistInsidiousness = 0;
	m_iSpecialistInvestigation = 0;
	m_icachedPropertyNeedsTurn = 0;

/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_fPopulationgrowthratepercentageLog = 0.0;
	m_iLostProductionBase = 0;
	m_iImprovementGoodHealth = 0;
	m_iImprovementBadHealth = 0;
	m_iSpecialistGoodHealth = 0;
	m_iSpecialistBadHealth = 0;
	m_iSpecialistHappiness = 0;
	m_iSpecialistUnhappiness = 0;
	m_iCiv = NO_CIVILIZATION;
	m_iExtraYieldTurns = 0;
	m_eOccupationCultureLevel = NO_CULTURELEVEL;
	m_iLineOfSight = 0;
	m_iLandmarkAngerTimer = 0;
	m_iInvasionChance = 0;
	m_iInvasionTimer = 0;
	m_iFreshWater = 0;
	m_iAdjacentDamagePercent = 0;
	m_iLostProduction = 0;
	m_iWorkableRadiusOverride = 0;
	m_iProtectedCultureCount = 0;
	m_iNumUnitFullHeal = 0;
	m_iDisabledPowerTimer = 0;
	m_iWarWearinessTimer = 0;
	m_iEventAnger = 0;
	m_iMinimumDefenseLevel = 0;
	m_iNumPopulationEmployed = 0;
	m_iHealthPercentPerPopulation = 0;
	m_iHappinessPercentPerPopulation = 0;
	m_iMADIncoming = 0;
	m_iQuarantinedCount = 0;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	m_bNeverLost = true;
	m_bPropertyControlBuildingQueued = false;
	m_bVisibilitySetup = false;
	m_bBombarded = false;
	m_bDrafted = false;
	m_bAirliftTargeted = false;
	m_bWeLoveTheKingDay = false;
	m_bCitizensAutomated = true;
	m_bProductionAutomated = false;
	m_bWallOverride = false;
	m_bInfoDirty = true;
	m_bLayoutDirty = false;
	m_bMaintenanceDirty = false;
	m_bPlundered = false;

	m_recalcBuilding = MAX_INT;
	m_bPlotWorkingMasked = false;

	m_Properties.clear();

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       12/07/09                         denev & jdog5000     */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
	m_bPopProductionProcess = false;
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/

	m_eOwner = eOwner;
	m_ePreviousOwner = NO_PLAYER;
	m_eOriginalOwner = eOwner;
	m_eCultureLevel = NO_CULTURELEVEL;
/************************************************************************************************/
/* REVOLUTION_MOD                         06/10/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_iRevolutionIndex = 0;
	m_iLocalRevIndex = -1;
	m_iRevIndexAverage = 0;
	m_iRevolutionCounter = 0;
	m_iReinforcementCounter = 0;
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	//TB Combat Mod (Buildings) begin
	m_iAidRate = 0;
	m_iTotalFrontSupportPercentModifier = 0;
	m_iTotalShortRangeSupportPercentModifier = 0;
	m_iTotalMediumRangeSupportPercentModifier = 0;
	m_iTotalLongRangeSupportPercentModifier = 0;
	m_iTotalFlankSupportPercentModifier = 0;
//Team Project (3)
	m_iExtraLocalCaptureProbabilityModifier = 0;
	m_iExtraLocalCaptureResistanceModifier = 0;
	m_iExtraLocalDynamicDefense = 0;
	m_iExtraRiverDefensePenalty = 0;
	m_iExtraLocalRepel = 0;
	m_iExtraMinDefense = 0;
	m_iExtraBuildingDefenseRecoverySpeedModifier = 0;
	m_iModifiedBuildingDefenseRecoverySpeedCap = 0;
	m_iExtraCityDefenseRecoverySpeedModifier = 0;
	m_iExtraTechSpecialistHappiness = 0;
	m_iExtraTechHappiness = 0;
	m_iExtraTechSpecialistHealth = 0;
	m_iExtraTechHealth = 0;
	m_iPrioritySpecialist = NO_SPECIALIST;
	m_iExtraInsidiousness = 0;
	m_iExtraInvestigation = 0;
	m_iSpecialistInsidiousness = 0;
	m_iSpecialistInvestigation = 0;
	m_icachedPropertyNeedsTurn = 0;

	//TB Combat Mod (Buildings) end

	m_iZoCCount = 0;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSeaPlotYield[iI] = 0;
		m_aiRiverPlotYield[iI] = 0;
		m_aiBaseYieldRate[iI] = 0;
		m_aiBaseYieldPerPopRate[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiPowerYieldRateModifier[iI] = 0;
		m_aiBonusYieldRateModifier[iI] = 0;
		m_aiTradeYield[iI] = 0;
		m_aiCorporationYield[iI] = 0;
		m_aiExtraSpecialistYield[iI] = 0;
		m_cachedBuildingYieldModifers[iI] = 0;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiCommerceRate[iI] = 0;
		m_abCommerceRateDirty[iI] = false;
		m_aiProductionToCommerceModifier[iI] = 0;
		m_aiBuildingCommerce[iI] = 0;
		m_aiSpecialistCommerce[iI] = 0;
		m_aiReligionCommerce[iI] = 0;
		m_aiCorporationCommerce[iI] = 0;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCommerceHappinessPer[iI] = 0;
		m_aiBonusCommerceRateModifier[iI] = 0;
		m_aiBonusCommercePercentChanges[iI] = 0;
		m_aiCommerceAttacks[iI] = 0;
		m_aiMaxCommerceAttacks[iI] = 0;
		m_aiExtraSpecialistCommerce[iI] = 0;
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiDomainFreeExperience[iI] = 0;
		m_aiDomainProductionModifier[iI] = 0;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiCulture[iI] = 0;
		m_aiNumRevolts[iI] = 0;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_abEverOwned[iI] = false;
		m_abTradeRoute[iI] = false;
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_abRevealed[iI] = false;
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_abEspionageVisibility[iI] = false;
	}

	m_szName.clear();
	m_szScriptData = "";

	m_bPopulationRankValid = false;
	m_iPopulationRank = -1;


	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_abBaseYieldRankValid[iI] = false;
		m_abYieldRankValid[iI] = false;
		m_aiBaseYieldRank[iI] = -1;
		m_aiYieldRank[iI] = -1;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_abCommerceRankValid[iI] = false;
		m_aiCommerceRank[iI] = -1;
	}

	if (!bConstructorCall)
	{
		//Team Project (1)

		FAssertMsg((0 < GC.getNumTechInfos()),  "GC.getNumTechInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		FAssertMsg(m_ppaaiTechSpecialistHappinessTypes==NULL, "about to leak memory, CvCity::m_ppaaiTechSpecialistHappinessTypes");
		FAssertMsg(m_ppaaiTechSpecialistHealthTypes==NULL, "about to leak memory, CvCity::m_ppaaiTechSpecialistHealthTypes");
		m_paiTechHappiness = new int[GC.getNumTechInfos()];
		m_paiTechSpecialistHappiness = new int[GC.getNumTechInfos()];
		m_paiTechHealth = new int[GC.getNumTechInfos()];
		m_paiTechSpecialistHealth = new int[GC.getNumTechInfos()];
		m_ppaaiTechSpecialistHappinessTypes = new int*[GC.getNumTechInfos()];
		m_ppaaiTechSpecialistHealthTypes = new int*[GC.getNumTechInfos()];
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_ppaaiTechSpecialistHappinessTypes[iI] = new int[GC.getNumSpecialistInfos()];
			m_ppaaiTechSpecialistHealthTypes[iI] = new int[GC.getNumSpecialistInfos()];
			for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
			{
				m_ppaaiTechSpecialistHappinessTypes[iI][iJ] = 0;
				m_ppaaiTechSpecialistHealthTypes[iI][iJ] = 0;
			}
			m_paiTechHappiness[iI] = 0;
			m_paiTechSpecialistHappiness[iI] = 0;
			m_paiTechHealth[iI] = 0;
			m_paiTechSpecialistHealth[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumSpecialistInfos()),  "GC.getNumSpecialistInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		FAssertMsg(m_ppaaiLocalSpecialistExtraYield==NULL, "about to leak memory, CvCity::m_ppaaiLocalSpecialistExtraYield");
		m_ppaaiLocalSpecialistExtraYield = new int*[GC.getNumSpecialistInfos()];
		FAssertMsg(m_ppaaiLocalSpecialistExtraCommerce==NULL, "about to leak memory, CvCity::m_ppaaiLocalSpecialistExtraCommerce");
		m_ppaaiLocalSpecialistExtraCommerce = new int*[GC.getNumSpecialistInfos()];
		m_paiSpecialistBannedCount = new int[GC.getNumSpecialistInfos()];
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiLocalSpecialistExtraYield[iI] = new int[NUM_YIELD_TYPES];
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiLocalSpecialistExtraYield[iI][iJ] = 0;
			}
			m_paiSpecialistBannedCount[iI] = 0;
			m_ppaaiLocalSpecialistExtraCommerce[iI] = new int[NUM_COMMERCE_TYPES];
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaaiLocalSpecialistExtraCommerce[iI][iJ] = 0;
			}
		}

		FAssertMsg((0 < GC.getNumBonusInfos()),  "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiNoBonus = new int[GC.getNumBonusInfos()];
		m_paiFreeBonus = new int[GC.getNumBonusInfos()];
		m_paiNumBonuses = new int[GC.getNumBonusInfos()];
		m_paiNumCorpProducedBonuses = new int[GC.getNumBonusInfos()];
		//TB Combat Mods (Buildings) begin
		m_ppaaiExtraBonusAidModifier = new int*[GC.getNumBonusInfos()];
		//TB Combat Mods (Buildings) end
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_ppaaiExtraBonusAidModifier[iI] = new int[GC.getNumPropertyInfos()];
			for (int iJ = 0; iJ < GC.getNumPropertyInfos(); iJ++)
			{
				m_ppaaiExtraBonusAidModifier[iI][iJ]=0;
			}
			m_paiNoBonus[iI] = 0;
			m_paiFreeBonus[iI] = 0;
			m_paiNumBonuses[iI] = 0;
			m_paiNumCorpProducedBonuses[iI] = 0;
		}

		m_paiProjectProduction = new int[GC.getNumProjectInfos()];
		for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			m_paiProjectProduction[iI] = 0;
		}

		m_paiPromotionLineAfflictionAttackCommunicability = new int[GC.getNumPromotionLineInfos()];
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			m_paiPromotionLineAfflictionAttackCommunicability[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumBuildingInfos()),  "GC.getNumBuildingInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		//m_ppBuildings = new CvBuilding *[GC.getNumBuildingInfos()];
		m_paiBuildingProduction = new int[GC.getNumBuildingInfos()];
		m_paiBuildingProductionTime = new int[GC.getNumBuildingInfos()];
		m_paiBuildingOriginalOwner = new int[GC.getNumBuildingInfos()];
		m_paiBuildingOriginalTime = new int[GC.getNumBuildingInfos()];
		m_paiNumRealBuilding = new int[GC.getNumBuildingInfos()];
		m_paiNumFreeBuilding = new int[GC.getNumBuildingInfos()];
		m_paiNumFreeAreaBuilding = new int[GC.getNumBuildingInfos()];
/************************************************************************************************/
/* Afforess	                  Start		 05/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		m_pabDisabledBuilding = new bool[GC.getNumBuildingInfos()];
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
//Team Project (5)
		m_pabReligiouslyDisabledBuilding = new bool[GC.getNumBuildingInfos()];
		m_paiBuildingCostPopulationCount = new int [GC.getNumBuildingInfos()];
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			//m_ppBuildings[iI] = NULL;
			m_paiBuildingProduction[iI] = 0;
			m_paiBuildingProductionTime[iI] = 0;
			m_paiBuildingOriginalOwner[iI] = -1;
			m_paiBuildingOriginalTime[iI] = MIN_INT;
			m_paiNumRealBuilding[iI] = 0;
			m_paiNumFreeBuilding[iI] = 0;
			m_paiNumFreeAreaBuilding[iI] = 0;
/************************************************************************************************/
/* Afforess	                  Start		 05/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			m_pabDisabledBuilding[iI] = false;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
//Team Project (5)
			m_pabReligiouslyDisabledBuilding[iI] = false;
			m_paiBuildingCostPopulationCount[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumUnitInfos()),  "GC.getNumUnitInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiUnitProduction = new int[GC.getNumUnitInfos()];
		m_paiUnitProductionTime = new int[GC.getNumUnitInfos()];
		m_paiGreatPeopleUnitRate = new int[GC.getNumUnitInfos()];
		m_paiGreatPeopleUnitProgress = new int[GC.getNumUnitInfos()];
		for (int iI = 0;iI < GC.getNumUnitInfos();iI++)
		{
			m_paiUnitProduction[iI] = 0;
			m_paiUnitProductionTime[iI] = 0;
			m_paiGreatPeopleUnitRate[iI] = 0;
			m_paiGreatPeopleUnitProgress[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumSpecialistInfos()),  "GC.getNumSpecialistInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiMaxSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiForceSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiFreeSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiFreeSpecialistCountUnattributed =  new int[GC.getNumSpecialistInfos()];
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_paiSpecialistCount[iI] = 0;
			m_paiMaxSpecialistCount[iI] = 0;
			m_paiForceSpecialistCount[iI] = 0;
			m_paiFreeSpecialistCount[iI] = 0;
			m_paiFreeSpecialistCountUnattributed[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumImprovementInfos()),  "GC.getNumImprovementInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiImprovementFreeSpecialists = new int[GC.getNumImprovementInfos()];
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_paiImprovementFreeSpecialists[iI] = 0;
		}

		m_paiReligionInfluence = new int[GC.getNumReligionInfos()];
		m_paiStateReligionHappiness = new int[GC.getNumReligionInfos()];
		m_pabHasReligion = new bool[GC.getNumReligionInfos()];
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			m_paiReligionInfluence[iI] = 0;
			m_paiStateReligionHappiness[iI] = 0;
			m_pabHasReligion[iI] = false;
		}

		m_pabHasCorporation = new bool[GC.getNumCorporationInfos()];
		for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			m_pabHasCorporation[iI] = false;
		}

		FAssertMsg((0 < GC.getNumUnitCombatInfos()),  "GC.getNumUnitCombatInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiUnitCombatFreeExperience = new int[GC.getNumUnitCombatInfos()];
/************************************************************************************************/
/* Afforess	                  Start		 05/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		m_paiUnitCombatExtraStrength = new int[GC.getNumUnitCombatInfos()];
		//TB Combat Mod (Buildings) begin
		m_paiUnitCombatProductionModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatRepelModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatRepelAgainstModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatDefenseAgainstModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatOngoingTrainingTimeCount = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatOngoingTrainingTimeIncrement = new int[GC.getNumUnitCombatInfos()];
		m_paiDamageAttackingUnitCombatCount = new int [GC.getNumUnitCombatInfos()];
		m_paiHealUnitCombatTypeVolume = new int [GC.getNumUnitCombatInfos()];
		//TB Combat Mod (Buildings) end
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			m_paiUnitCombatFreeExperience[iI] = 0;
/************************************************************************************************/
/* Afforess	                  Start		 05/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			m_paiUnitCombatExtraStrength[iI] = 0;
			//TB Combat Mod (Buildings) begin
			m_paiUnitCombatProductionModifier[iI] = 0;
			m_paiUnitCombatRepelModifier[iI] = 0;
			m_paiUnitCombatRepelAgainstModifier[iI] = 0;
			m_paiUnitCombatDefenseAgainstModifier[iI] = 0;
			m_paiUnitCombatOngoingTrainingTimeCount[iI] = 0;
			m_paiUnitCombatOngoingTrainingTimeIncrement[iI] = 0;
			m_paiDamageAttackingUnitCombatCount[iI] = 0;
			m_paiHealUnitCombatTypeVolume[iI] = 0;
			//TB Combat Mod (Buildings) end
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		}

		FAssertMsg((0 < GC.getNumPromotionInfos()),  "GC.getNumPromotionInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		//TB Combat Mods (Buildings) begin
		m_paiFreePromotionCount = new int[GC.getNumPromotionInfos()];
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			m_paiFreePromotionCount[iI] = 0;
			//TB Combat Mods (Buildings) end
		}

		//TB Combat Mods (Buildings) begin
		FAssertMsg((0 < GC.getNumPromotionLineInfos()),  "GC.getNumPromotionLineInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiNewAfflictionTypeCount = new int[GC.getNumPromotionLineInfos()];
		m_paiNewExtraAfflictionOutbreakLevelChange = new int[GC.getNumPromotionLineInfos()];
		m_paiNewAfflictionToleranceChange = new int[GC.getNumPromotionLineInfos()];
		m_paiNewCurrentOvercomeChange = new int[GC.getNumPromotionLineInfos()];
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			m_paiNewAfflictionTypeCount[iI] = 0;
			m_paiNewExtraAfflictionOutbreakLevelChange[iI] = 0;
			m_paiNewAfflictionToleranceChange[iI] = 0;
			m_paiNewCurrentOvercomeChange[iI] = 0;
			//TB Combat Mods (Buildings) end
		}

		FAssertMsg((0 < GC.getNumPropertyInfos()),  "GC.getNumPropertyInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiAidRate = new int[GC.getNumPropertyInfos()];
		for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
		{
			m_paiAidRate[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumInvisibleInfos()),  "GC.getNumInvisibleInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_aiCachedBestSeeInvisibleUnit = new int[GC.getNumInvisibleInfos()];
		m_abCachedBestSeeInvisibleUnit = new bool[GC.getNumInvisibleInfos()];
		m_aiCachedBestSeeInvisibleUnitSea = new int[GC.getNumInvisibleInfos()];
		m_abCachedBestSeeInvisibleUnitSea = new bool[GC.getNumInvisibleInfos()];
		for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
		{
			m_aiCachedBestSeeInvisibleUnit[iI] = -1;
			m_abCachedBestSeeInvisibleUnit[iI] = false;
			m_aiCachedBestSeeInvisibleUnitSea[iI] = -1;
			m_abCachedBestSeeInvisibleUnitSea[iI] = false;
		}

		FAssertMsg((0 < NUM_CITY_PLOTS),  "NUM_CITY_PLOTS is not greater than zero but an array is being allocated in CvCity::reset");
		m_pabWorkingPlot = new bool[NUM_CITY_PLOTS];
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			m_pabWorkingPlot[iI] = false;
		}
		int iMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES");
		if (getOwnerINLINE() != NO_PLAYER)
		{
			iMaxTradeRoutes += GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();
		}
		FAssertMsg((0 < iMaxTradeRoutes),  "Max Trade Routes is not greater than zero but an array is being allocated in CvCity::reset");
		m_paTradeCities = new IDInfo[iMaxTradeRoutes];
		for (int iI = 0; iI < iMaxTradeRoutes; iI++)
		{
			m_paTradeCities[iI].reset();
		}

		m_aEventsOccured.clear();
		m_aBuildingYieldChange.clear();
		m_aBuildingCommerceChange.clear();
		m_aBuildingHappyChange.clear();
		m_aBuildingHealthChange.clear();
/************************************************************************************************/
/* Afforess	                  Start		 04/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		m_paiUnitClassProductionModifier = new int[GC.getNumUnitClassInfos()];
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			m_paiUnitClassProductionModifier[iI] = 0;
		}
		m_paiBuildingClassProductionModifier = new int[GC.getNumBuildingClassInfos()];
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			m_paiBuildingClassProductionModifier[iI] = 0;
		}
		m_paiBonusDefenseChanges = new int[GC.getNumBonusInfos()];
		m_pabHadVicinityBonus = new bool[GC.getNumBonusInfos()];
		m_pabHadRawVicinityBonus = new bool[GC.getNumBonusInfos()];
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiBonusDefenseChanges[iI] = 0;
			m_pabHadVicinityBonus[iI] = false;
			m_pabHadRawVicinityBonus[iI] = false;
		}

		m_cachedPropertyNeeds = new int[GC.getNumPropertyInfos()];
		for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
		{
			m_cachedPropertyNeeds[iI] = 0;
		}
		
		FAssertMsg(m_pabAutomatedCanBuild==NULL, "about to leak memory, CvCity::m_pabAutomatedCanBuild");
		m_pabAutomatedCanBuild = new bool [GC.getNumBuildInfos()];
		for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			m_pabAutomatedCanBuild[iI] = true;
		}
		m_aBuildingCommerceModifier.clear();
		m_aBuildingYieldModifier.clear();
		m_aPropertySpawns.clear();
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}

	if ( eOwner != NO_PLAYER )
	{
		m_BuildingList.setPlayerToOwner();
		m_UnitList.setPlayerToOwner();
		m_UnitList.init();
	}

	if (!bConstructorCall)
	{
		m_BuildingList.init();
		AI_reset();
	}

	m_bIsGreatWallSeed = false;
	m_deferringBonusProcessingCount = 0;
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvCity::setupGraphical()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	if (!isInViewport())
	{
		return;
	}

	if ( getEntity() == NULL )
	{
		createCityEntity(this);
	}

	CvDLLEntity::setup();

	setInfoDirty(true);
	setLayoutDirty(true);
}

/************************************************************************************************/
/* REVOLUTION_MOD                         03/29/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvCity::getRevolutionIndex() const
{
	return m_iRevolutionIndex;
}

void CvCity::setRevolutionIndex( int iNewValue )
{
	if( iNewValue < 0 )
		iNewValue = 0;

	m_iRevolutionIndex = iNewValue;
}

void CvCity::changeRevolutionIndex( int iChange )
{
	setRevolutionIndex( getRevolutionIndex() + iChange );
}

int CvCity::getLocalRevIndex() const
{
	return m_iLocalRevIndex;
}

void CvCity::setLocalRevIndex( int iNewValue )
{
	m_iLocalRevIndex = iNewValue;
}

void CvCity::changeLocalRevIndex( int iChange )
{
	setLocalRevIndex( getLocalRevIndex() + iChange );
}

int CvCity::getRevIndexAverage() const
{
	return m_iRevIndexAverage;
}

void CvCity::setRevIndexAverage( int iNewValue )
{
	m_iRevIndexAverage = range(iNewValue,0,3400);
}

void CvCity::updateRevIndexAverage( )
{
	setRevIndexAverage( (2*getRevIndexAverage() + getRevolutionIndex())/3 );
}

int CvCity::getRevolutionCounter() const
{
	return m_iRevolutionCounter;
}

void CvCity::setRevolutionCounter( int iNewValue )
{
	if( iNewValue < 0 )
		iNewValue = 0;

	m_iRevolutionCounter = iNewValue;
}

void CvCity::changeRevolutionCounter( int iChange )
{
	setRevolutionCounter( getRevolutionCounter() + iChange );
}

int CvCity::getReinforcementCounter() const
{
	return m_iReinforcementCounter;
}

void CvCity::setReinforcementCounter( int iNewValue )
{
	if( iNewValue < 0 )
		iNewValue = 0;

	m_iReinforcementCounter = iNewValue;
}

void CvCity::changeReinforcementCounter( int iChange )
{
	setReinforcementCounter( getReinforcementCounter() + iChange );
}

// AIAndy: This function seems incomplete and unused
int CvCity::getRevIndexHappinessVal()
{
	FAssertMsg(false, "This function should never have been called!");
	return 0;

#if 0
	int iVal = 0;
	double fHappyMod = 1.0;

	int iUnhappy = unhappyLevel(-1);
	int iHappy = happyLevel();

	if( iUnhappy > iHappy )
	{
		double unhappy = iUnhappy;

		// Lower unhappiness from war weariness
		if( getWarWearinessPercentAnger() > 0 )
		{
			unhappy -= (1.0 * getPopulation() * (double)(getWarWearinessPercentAnger()/2)) / GC.getPERCENT_ANGER_DIVISOR();
		}

		// Lower unhappiness from Rev index (avoid spiral)
		unhappy -= (1.0 * getPopulation() * getRevIndexPercentAnger()) / GC.getPERCENT_ANGER_DIVISOR();

		unhappy -= iHappy;

		if( unhappy > 0 )
		{
			// Lower unhappiness from espionage missions
			if( getEspionageHappinessCounter() )
			{
				unhappy -= std::min( (-unhappy)/3.0, getEspionageHappinessCounter()/3.0 );
			}

			if( getOccupationTimer() > 0 )
			{
				unhappy = unhappy/3.0;
			}
			else if( isRecentlyAcquired() )
			{
				unhappy = unhappy/2.0;
			}

		}
	}
	else
	{
		
	}

	return iVal;
#endif
}

int CvCity::getRevIndexDistanceVal()
{
	int iVal = 0;

	return iVal;
}

int CvCity::getRevIndexColonyVal()
{
	int iVal = 0;

	return iVal;
}

int CvCity::getRevIndexReligionVal()
{
	int iVal = 0;

	return iVal;
}

int CvCity::getRevIndexNationalityVal()
{
	int iVal = 0;

	return iVal;
}

int CvCity::getRevIndexHealthVal()
{
	int iVal = 0;

	return iVal;
}

int CvCity::getRevIndexGarrisonVal()
{
	int iVal = 0;

	return iVal;
}

int CvCity::getRevIndexDisorderVal()
{
	int iVal = 0;

	return iVal;
}

bool CvCity::isRecentlyAcquired()
{
	int iModifier = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
	return ((GC.getGameINLINE().getGameTurn() - getGameTurnAcquired()) < (12*iModifier)/100);
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

void CvCity::kill(bool bUpdatePlotGroups, bool bUpdateCulture)
{
	PROFILE_FUNC();


	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}

	CvPlot* pPlot = plot();

	//	Take this plot out of zobrist hashes for local plot groups
	pPlot->ToggleInPlotGroupsZobristContributors();

	CvPlotGroup* originalTradeNetworkConnectivity[MAX_PLAYERS];
	//	Whose trade networks was this city relevant to prior to razing
	if ( bUpdatePlotGroups )
	{
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			originalTradeNetworkConnectivity[iI] = GET_PLAYER((PlayerTypes)iI).isAlive() ? pPlot->getPlotGroup((PlayerTypes)iI) : NULL;
		}
	}

	// < M.A.D. Nukes Start >
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{

		if(!GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			continue;
		}

		for(int iJ = 0; iJ < GET_PLAYER((PlayerTypes)iI).getNumUnits() ; iJ++)
		{
			if(GET_PLAYER((PlayerTypes)iI).getUnit(iJ) == NULL)
			{
				continue;
			}

			if(!GET_PLAYER((PlayerTypes)iI).getUnit(iJ)->getUnitInfo().getUnitAIType(UNITAI_ICBM))
			{
				continue;
			}
			if(!GET_PLAYER((PlayerTypes)iI).getUnit(iJ)->isMADEnabled())
			{
				continue;
			}
			if(!at(GET_PLAYER((PlayerTypes)iI).getUnit(iJ)->getMADTargetPlotX(), GET_PLAYER((PlayerTypes)iI).getUnit(iJ)->getMADTargetPlotY()))
			{
				continue;
			}

			if(GET_PLAYER((PlayerTypes)iI).getUnit(iJ)->getX_INLINE() == INVALID_PLOT_COORD && 
				GET_PLAYER((PlayerTypes)iI).getUnit(iJ)->getY_INLINE() == INVALID_PLOT_COORD)
			{
				continue;
			}
				
			GET_PLAYER((PlayerTypes)iI).getUnit(iJ)->clearMADTargetPlot();
		}
	}
	// < M.A.D. Nukes End  >

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCityOverride() == this)
			{
				pLoopPlot->setWorkingCityOverride(NULL);
			}
		}
	}

	setCultureLevel(NO_CULTURELEVEL, false);
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	setOccupationCultureLevel(NO_CULTURELEVEL);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		setNumRealBuilding(((BuildingTypes)iI), 0);
		setNumFreeBuilding(((BuildingTypes)iI), 0);
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		setFreeSpecialistCount(((SpecialistTypes)iI), 0);
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		setTradeYield(((YieldTypes)iI), 0);
		setCorporationYield(((YieldTypes) iI), 0);
	}

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		setHasReligion(((ReligionTypes)iI), false, false, true);

		if (isHolyCity((ReligionTypes)iI))
		{
			GC.getGameINLINE().setHolyCity(((ReligionTypes)iI), NULL, false);
		}
	}

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		setHasCorporation(((CorporationTypes)iI), false, false);

		if (isHeadquarters((CorporationTypes)iI))
		{
			GC.getGameINLINE().setHeadquarters(((CorporationTypes)iI), NULL, false);
		}
	}

	setPopulation(0);

	AI_assignWorkingPlots();

	clearOrderQueue();

	// remember the visibility before we take away the city from the plot below
	std::vector<bool> abEspionageVisibility;
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		abEspionageVisibility.push_back(getEspionageVisibility((TeamTypes)iI));
	}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       08/04/09                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
	// Need to clear trade routes of dead city, else they'll be claimed for the owner forever
	clearTradeRoutes();
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/

	pPlot->setPlotCity(NULL);

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/04/10                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
	// Replace floodplains after city is removed
	if (pPlot->getBonusType() == NO_BONUS)
	{
		for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			//Fuyu: don't limit to riverside
			/* 
			if (GC.getFeatureInfo((FeatureTypes)iJ).isRequiresRiver())
			*/
			{
				if (pPlot->canHaveFeature((FeatureTypes)iJ))
				{
					if (GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability() == 10000)
					{
						pPlot->setFeatureType((FeatureTypes)iJ);
						break;
					}
				}
			}
		}
	}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/

	area()->changeCitiesPerPlayer(getOwnerINLINE(), -1);

	GET_TEAM(getTeam()).changeNumCities(-1);

	GC.getGameINLINE().changeNumCities(-1);

	FAssertMsg(getWorkingPopulation() == 0, "getWorkingPopulation is expected to be 0");
	FAssertMsg(!isWorkingPlot(CITY_HOME_PLOT), "isWorkingPlot(CITY_HOME_PLOT) is expected to be false");
	FAssertMsg(getSpecialistPopulation() == 0, "getSpecialistPopulation is expected to be 0");
	FAssertMsg(getNumGreatPeople() == 0, "getNumGreatPeople is expected to be 0");
	FAssertMsg(getBaseYieldRate(YIELD_FOOD) == 0, "getBaseYieldRate(YIELD_FOOD) is expected to be 0");
	FAssertMsg(getBaseYieldRate(YIELD_PRODUCTION) == 0, "getBaseYieldRate(YIELD_PRODUCTION) is expected to be 0");
	FAssertMsg(getBaseYieldRate(YIELD_COMMERCE) == 0, "getBaseYieldRate(YIELD_COMMERCE) is expected to be 0");
	FAssertMsg(!isProduction(), "isProduction is expected to be false");

	PlayerTypes eOwner = getOwnerINLINE();

	bool bCapital = isCapital();

	pPlot->setImprovementType((ImprovementTypes)(GC.getDefineINT("RUINS_IMPROVEMENT")));

	CvPlayer& kOwner = GET_PLAYER(eOwner);

	kOwner.setCommerceDirty(NO_COMMERCE);
	kOwner.updateCommerce(NO_COMMERCE);

	CvEventReporter::getInstance().cityLost(this);

	GET_PLAYER(getOwnerINLINE()).deleteCity(getID());

	if ( bUpdateCulture )
	{
		pPlot->updateCulture(true, false);

		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlot->updateCulture(true, false);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM(kOwner.getTeam()).isVassal((TeamTypes)iI))
		{
			pPlot->changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), false, NULL, false);
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (abEspionageVisibility[iI])
		{
			pPlot->changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), false, NULL, false);
		}
	}

	//ls612: Embassy visibility fix (by Damgo)
	if (bCapital)
	{
		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (GET_TEAM(kOwner.getTeam()).isHasEmbassy((TeamTypes)iI))
			{
				pPlot->changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), false, NULL, false);
			}
		}
	}

	kOwner.setMaintenanceDirty(true);

	GC.getMapINLINE().updateWorkingCity();

	GC.getGameINLINE().AI_makeAssignWorkDirty();

	if (bCapital)
	{
		kOwner.findNewCapital();

		GET_TEAM(kOwner.getTeam()).resetVictoryProgress();
	}
	if (bUpdatePlotGroups)
	{
		PROFILE("CvCity::kill.UpdatePlotGroups");
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if ( GET_PLAYER((PlayerTypes)iI).isAlive() )
			{
				if ( originalTradeNetworkConnectivity[iI] == NULL )
				{
					if ( pPlot->isTradeNetwork(GET_PLAYER((PlayerTypes)iI).getTeam()) )
					{
						GET_PLAYER((PlayerTypes)iI).updatePlotGroups(pPlot->area());
					}
				}
				else
				{
					originalTradeNetworkConnectivity[iI]->recalculatePlots();
				}
			}
		}
	}

	if (eOwner == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}

void CvCity::killTestCheap()
{
	
	//CvPlot* pPlot;
	//CvPlot* pAdjacentPlot;
	//CvPlot* pLoopPlot;
	//PlayerTypes eOwner;
	//bool bCapital;
	// < M.A.D. Nukes Start >
	//int iI, iJ;
	// < M.A.D. Nukes End   >
	
	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}

//	pPlot = plot();


//	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
//	{
//		pLoopPlot = getCityIndexPlot(iI);
//
//		if (pLoopPlot != NULL)
//		{
//			if (pLoopPlot->getWorkingCityOverride() == this)
//			{
//				pLoopPlot->setWorkingCityOverride(NULL);
//			}
//		}
//	}



	//clearOrderQueue();


	//pPlot->setPlotCity(NULL);


//	area()->changeCitiesPerPlayer(getOwnerINLINE(), -1);

//	GET_TEAM(getTeam()).changeNumCities(-1);

//	GC.getGameINLINE().changeNumCities(-1);

//	eOwner = getOwnerINLINE();

//	bCapital = isCapital();

	GET_PLAYER(getOwnerINLINE()).deleteCity(getID());

//	pPlot->updateCulture(true, false);

	//for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	//{
	//	pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

	//	if (pAdjacentPlot != NULL)
	//	{
	//		pAdjacentPlot->updateCulture(true, false);
	//	}
	//}

	
	//GC.getMapINLINE().updateWorkingCity();

	//GC.getGameINLINE().AI_makeAssignWorkDirty();

	//if (bCapital)
	//{
	//	GET_PLAYER(eOwner).findNewCapital();

	//	GET_TEAM(GET_PLAYER(eOwner).getTeam()).resetVictoryProgress();
	//}

	//if (eOwner == GC.getGameINLINE().getActivePlayer())
	//{
	//	gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	//}
}

void CvCity::doTurn()
{
	MEMORY_TRACE_FUNCTION();
	PROFILE("CvCity::doTurn()");

	FAssert(m_deferringBonusProcessingCount == 0);


	flushBestSeeInvisibleUnitCache();
	//	Fail safe
	m_deferringBonusProcessingCount = 0;

	//	New turn.  Generally won't really need a cache flush because
	//	last city of previous turn won't match, but for the sake of edge cases
	//	flush anyway to be safe
	AI_FlushBuildingValueCache();
	FlushCanConstructCache();
	setBuildingListInvalid();
	setUnitListInvalid();
#ifdef CAN_TRAIN_CACHING
	populateCanTrainCache(false);
#endif

	m_unitSourcedPropertyCache.clear();

	//	Slight hack for now - will need to change as and when we go multi-threaded
	CvPlot::setDeferredPlotGroupRecalculationMode(true);

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_cachedBuildingYieldModifers[iI] = -1;
	}

	if (!isBombarded())
	{
		changeDefenseDamage(- cityDefenseRecoveryRate());
	}

	setLastDefenseDamage(getDefenseDamage());
	setBombarded(false);
	setPlundered(false);
	setDrafted(false);
	setAirliftTargeted(false);
	setBuiltFoodProducedUnit(false);
/************************************************************************************************/
/* Afforess	                        		   9/7/10                                           */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	//Promotes Units if there is a building that allows it
	doPromotion();
	//Does vicinity bonus checks
	doVicinityBonus();
	//Checks conditions of buildings, may disable or enable some
	checkBuildings(true, false, true, true, false);
	//Extra Hammer from settling on Forest
	if (getExtraYieldTurns() > 0)
	{
		changeExtraYieldTurns(-1);
	}
	//Extra Food from settling on Jungle
	else if (getExtraYieldTurns() < 0)
	{
		changeExtraYieldTurns(1);
	}
	
	//Checks if enemy troops have found secret entrance into the city
	doInvasion();

	//Damages enemy units around the city, if applicable
	doAttack();
	//Heals friendly units in the city extra, if applicable
	doHeal();
	//Decays the CommerceAttacks
	decayCommerce();
	//Spreads corporations
	doCorporation();
	//Counts down the disable power timer
	doDisabledPower();
	//Recalculate growth thresholds/food storage
	recalculateMaxFoodKeptPercent();
	recalculatePopulationgrowthratepercentage();
/************************************************************************************************/
/* Afforess	                                 END                                                */
/************************************************************************************************/
	doWarWeariness();
	
	AI_doTurn();

	bool bAllowNoProduction = !doCheckProduction();

	doGrowth();

	doCulture();

	doPlotCulture(false, getOwnerINLINE(), getCommerceRate(COMMERCE_CULTURE));

	//	Force deferred plot group recalculation to happen now before we assess production
	CvPlot::setDeferredPlotGroupRecalculationMode(false);

	//	Auto-build any auto-build buildings we can
	doAutobuild();

	doProduction(bAllowNoProduction);

	GET_PLAYER(getOwnerINLINE()).getContractBroker().advertiseTender(this, AI_getBuildPriority());
	
	doDecay();

	doReligion();

	doGreatPeople();

	doMeltdown();

	updateEspionageVisibility(true);

	setCurrAirlift(0);

	doPropertyUnitSpawn();

	//TB Combat Mod (Buildings) begin
	if (GC.getGameINLINE().isOption(GAMEOPTION_OUTBREAKS_AND_AFFLICTIONS))
	{
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			PromotionLineTypes eAfflictionLine = ((PromotionLineTypes)iI);
		
			if (GC.getPromotionLineInfo(eAfflictionLine).isAffliction())
			{
				doOutbreakCheck(eAfflictionLine);

				if (hasAfflictionType(eAfflictionLine))
				{
					changeCurrentOvercomeChange(eAfflictionLine, GC.getPromotionLineInfo(eAfflictionLine).getOvercomeAdjperTurn());
					doOvercomeCheck(eAfflictionLine);
				}

				setPromotionLineAfflictionAttackCommunicability(eAfflictionLine, 0);
				if (getAfflictionToleranceChange(eAfflictionLine) > 0)
				{
					int iDecay = GC.getPromotionLineInfo(eAfflictionLine).getToleranceDecay();
					if (iDecay != 0)
					{
						changeAfflictionToleranceChange(eAfflictionLine, -iDecay);
					}
				}
			}
		}
	}

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		updateOngoingTraining((UnitCombatTypes)iI);
	}
	//TB Combat Mod (Buildings) end

	if (!isDisorder())
	{
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		for (int iI = 0; iI < getNumCityPlots(); iI++)
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->getWorkingCity() == this)
				{
					if (pLoopPlot->isBeingWorked())
					{
						pLoopPlot->doImprovement();
					}
				}
			}
		}
	}

	if (getCultureUpdateTimer() > 0)
	{
		changeCultureUpdateTimer(-1);
	}

	if (getOccupationTimer() > 0)
	{
		changeOccupationTimer(-1);
	}

	if (getHurryAngerTimer() > 0)
	{
		changeHurryAngerTimer(-1);
	}

/************************************************************************************************/
/* REVOLUTION_MOD                         04/28/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if( getRevRequestAngerTimer() > 0 )
	{
		changeRevRequestAngerTimer(-1);
	}

	if (getRevSuccessTimer() > 0)
	{
		changeRevSuccessTimer(-1);
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	if (getConscriptAngerTimer() > 0)
	{
		changeConscriptAngerTimer(-1);
	}

	if (getDefyResolutionAngerTimer() > 0)
	{
		changeDefyResolutionAngerTimer(-1);
	}

	if (getHappinessTimer() > 0)
	{
		changeHappinessTimer(-1);
	}
/************************************************************************************************/
/* Afforess	                  Start		 04/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (getLandmarkAngerTimer() > 0)
	{
		changeLandmarkAngerTimer(-1);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (getEspionageHealthCounter() > 0)
	{
		changeEspionageHealthCounter(-1);
	}

	if (getEspionageHappinessCounter() > 0)
	{
		changeEspionageHappinessCounter(-1);
	}

	if (isOccupation() || (angryPopulation() > 0) || (healthRate() < 0))
	{
		setWeLoveTheKingDay(false);
	}
	else if ((getPopulation() >= GC.getDefineINT("WE_LOVE_THE_KING_POPULATION_MIN_POPULATION")) && (GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("WE_LOVE_THE_KING_RAND"), "Do We Love The King?") < getPopulation()))
	{
		setWeLoveTheKingDay(true);
	}
	else
	{
		setWeLoveTheKingDay(false);
	}
/************************************************************************************************/
/* Afforess	                  Start		 04/30/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		m_pabHadVicinityBonus[iI] = hasVicinityBonus((BonusTypes)iI);
		m_pabHadRawVicinityBonus[iI] = hasRawVicinityBonus((BonusTypes)iI);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

#ifdef CAN_TRAIN_CACHING
	//	Outside the scope of the city's turn where we expect to be using cached values turn
	//	the cache off (we may choose to widden the scope of usability later but for now this is safer)
	clearCanTrainCache();
#endif

	// ONEVENT - Do turn
	CvEventReporter::getInstance().cityDoTurn(this, getOwnerINLINE());

	// XXX
#ifdef _DEBUG
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			//FAssert(getBaseYieldRate((YieldTypes)iI) >= 0);
			//FAssert(getYieldRate((YieldTypes)iI) >= 0);

			int iCount = 0;

			for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
			{
				if (isWorkingPlot(iJ))
				{
					CvPlot* pPlot = getCityIndexPlot(iJ);

					if (pPlot != NULL)
					{
						iCount += pPlot->getYield((YieldTypes)iI);
					}
				}
			}

			for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
			{
			//Team Project (1)
				iCount += specialistYieldTotal((SpecialistTypes)iJ, (YieldTypes)iI);
			}

			for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				//Team Project (5)
				if (!isReligiouslyDisabledBuilding((BuildingTypes)iJ))
				{
					iCount += getNumActiveBuilding((BuildingTypes)iJ) * (GC.getBuildingInfo((BuildingTypes) iJ).getYieldChange(iI) + getBuildingYieldChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes) iJ).getBuildingClassType(), (YieldTypes)iI));
					iCount += getNumActiveBuilding((BuildingTypes)iJ) * ((GC.getBuildingInfo((BuildingTypes) iJ).getYieldPerPopChange(iI)*getPopulation())/100);
				}
			}

			iCount += getTradeYield((YieldTypes)iI);
			iCount += getCorporationYield((YieldTypes)iI);

//			FAssert(iCount == getBaseYieldRate((YieldTypes)iI));
		}

		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
		//	FAssert(getBuildingCommerce((CommerceTypes)iI) >= 0);
		//	FAssert(getSpecialistCommerce((CommerceTypes)iI) >= 0);
		//	FAssert(getReligionCommerce((CommerceTypes)iI) >= 0);
		//	FAssert(getCorporationCommerce((CommerceTypes)iI) >= 0);
		//	FAssert(GET_PLAYER(getOwnerINLINE()).getFreeCityCommerce((CommerceTypes)iI) >= 0);
		}

		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
		//	FAssert(isNoBonus((BonusTypes)iI) || getNumBonuses((BonusTypes)iI) >= ((isConnectedToCapital()) ? (GET_PLAYER(getOwnerINLINE()).getBonusImport((BonusTypes)iI) - GET_PLAYER(getOwnerINLINE()).getBonusExport((BonusTypes)iI)) : 0));
		}
	}
#endif
	// XXX
}

void CvCity::doAutobuild()
{
	//	Auto-build any auto-build buildings we can
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).isAutoBuild() &&
			getNumBuilding((BuildingTypes)iI) <= 0 &&
			canConstruct((BuildingTypes)iI, false, false, true))
		{

			setNumRealBuilding((BuildingTypes)iI, 1);

			CvWString szBuffer = gDLL->getText("TXT_KEY_COMPLETED_AUTO_BUILD", GC.getBuildingInfo((BuildingTypes)iI).getTextKeyWide(), getName().GetCString());
			AddDLLMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
		}
	}
}

bool CvCity::isCitySelected() const
{
	return gDLL->getInterfaceIFace()->isCitySelected(const_cast<CvCity*>(this));
}


bool CvCity::canBeSelected() const
{
	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		return true;
	}

	if (GC.getGameINLINE().getActiveTeam() != NO_TEAM)
	{
		if (plot()->isInvestigate(GC.getGameINLINE().getActiveTeam()))
		{
			return true;
		}
	}

	// EspionageEffect
	for (int iLoop = 0; iLoop < GC.getNumEspionageMissionInfos(); iLoop++)
	{
		// Check the XML
		if (GC.getEspionageMissionInfo((EspionageMissionTypes)iLoop).isPassive() && GC.getEspionageMissionInfo((EspionageMissionTypes)iLoop).isInvestigateCity())
		{
			// Is Mission good?
			if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).canDoEspionageMission((EspionageMissionTypes)iLoop, getOwnerINLINE(), plot(), -1, NULL))
			{
				return true;
			}
		}
	}

	return false;
}


void CvCity::updateSelectedCity(bool bTestProduction)
{
	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			pLoopPlot->updateShowCitySymbols();
		}
	}

	if (bTestProduction)
	{
		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && !isProduction())
		{
			chooseProduction(NO_UNIT, NO_BUILDING, NO_PROJECT, false, true);
		}
	}
}


void CvCity::updateYield()
{
	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			pLoopPlot->updateYield();
		}
	}
}


// XXX kill this?
void CvCity::updateVisibility()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	if (!isInViewport())
	{
		return;
	}

	FAssert(GC.getGameINLINE().getActiveTeam() != NO_TEAM);

	if (isVisibilitySetup())
	{
		CvDLLEntity::setVisible(isInViewport() && isRevealed(GC.getGameINLINE().getActiveTeam(), true));
	}
	else
	{
		setupGraphical();
		m_bVisibilitySetup = true;
	}

}
bool CvCity::isVisibilitySetup()
{
	return m_bVisibilitySetup;
}


void CvCity::createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience)
{
	GET_PLAYER(getOwnerINLINE()).createGreatPeople(eGreatPersonUnit, bIncrementThreshold, bIncrementExperience, getX_INLINE(), getY_INLINE());
}


void CvCity::doTask(TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	switch (eTask)
	{
	case TASK_RAZE:
		GET_PLAYER(getOwnerINLINE()).raze(this);
		break;

	case TASK_DISBAND:
		GET_PLAYER(getOwnerINLINE()).disband(this);
		break;

	case TASK_GIFT:
		if (getLiberationPlayer(false) == iData1)
		{
			liberate(false);
		}
		else
		{
			GET_PLAYER((PlayerTypes)iData1).acquireCity(this, false, true, true);
		}
		break;

	case TASK_KEEP:
		CvEventReporter::getInstance().cityAcquiredAndKept((PlayerTypes)iData1, this);
		break;

	case TASK_LIBERATE:
		liberate(iData1 != 0);
		break;

	case TASK_SET_AUTOMATED_CITIZENS:
		setCitizensAutomated(bOption);
		break;

	case TASK_SET_AUTOMATED_PRODUCTION:
		setProductionAutomated(bOption, bAlt && bShift && bCtrl);
		break;

	case TASK_SET_EMPHASIZE:
		AI_setEmphasize(((EmphasizeTypes)iData1), bOption);
		break;

	case TASK_EMPHASIZE_SPECIALIST:
		AI_setEmphasizeSpecialist((SpecialistTypes)iData1, bOption);
		break;

	case TASK_CHANGE_SPECIALIST:
		alterSpecialistCount(((SpecialistTypes)iData1), iData2);
		break;

	case TASK_CHANGE_WORKING_PLOT:
		alterWorkingPlot(iData1);
		break;

	case TASK_CLEAR_WORKING_OVERRIDE:
		clearWorkingOverride(iData1);
		break;

	case TASK_HURRY:
		hurry((HurryTypes)iData1);
		break;

	case TASK_CONSCRIPT:
		conscript();
		break;

	case TASK_CLEAR_ORDERS:
		clearOrderQueue();
		break;

	case TASK_RALLY_PLOT:
		setRallyPlot(GC.getMapINLINE().plotINLINE(iData1, iData2));
		break;

	case TASK_CLEAR_RALLY_PLOT:
		setRallyPlot(NULL);
		break;
	default:
		FAssertMsg(false, "eTask failed to match a valid option");
		break;
	}
}


void CvCity::chooseProduction(UnitTypes eTrainUnit, BuildingTypes eConstructBuilding, ProjectTypes eCreateProject, bool bFinish, bool bFront)
{
	CvPopupInfo* pPopupInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSEPRODUCTION);
	if (NULL == pPopupInfo)
	{
		return;
	}
	pPopupInfo->setData1(getID());
	pPopupInfo->setOption1(bFinish);

	if (eTrainUnit != NO_UNIT)
	{
		pPopupInfo->setData2(ORDER_TRAIN);
		pPopupInfo->setData3(eTrainUnit);
	}
	else if (eConstructBuilding != NO_BUILDING)
	{
		pPopupInfo->setData2(ORDER_CONSTRUCT);
		pPopupInfo->setData3(eConstructBuilding);
	}
	else if (eCreateProject != NO_PROJECT)
	{
		pPopupInfo->setData2(ORDER_CREATE);
		pPopupInfo->setData3(eCreateProject);
	}
	else
	{
		pPopupInfo->setData2(NO_ORDER);
		pPopupInfo->setData3(NO_UNIT);
	}

	gDLL->getInterfaceIFace()->addPopup(pPopupInfo, getOwnerINLINE(), false, bFront);
}


int CvCity::getCityPlotIndex(const CvPlot* pPlot) const
{
	return plotCityXY(this, pPlot);
}


CvPlot* CvCity::getCityIndexPlot(int iIndex) const
{
	return plotCity(getX_INLINE(), getY_INLINE(), iIndex);
}


bool CvCity::canWork(CvPlot* pPlot) const
{
	if (pPlot->getWorkingCity() != this)
	{
		return false;
	}

	FAssertMsg(getCityPlotIndex(pPlot) != -1, "getCityPlotIndex(pPlot) is expected to be assigned (not -1)");

	if (getCityPlotIndex(pPlot) >= getNumCityPlots()) return false; // Just in case FAssertMsg doesn't end the function.

	//in the rare case that a city ends up with an invisible animal inside the city or something, the city plot should be made immune to the following effect.
	if (pPlot != plot())
	{
		if (pPlot->plotCheck(PUF_canSiege, getOwnerINLINE()) != NULL)
		{
			return false;
		}
	}

	if (pPlot->isWater())
	{
		if (!(GET_TEAM(getTeam()).isWaterWork()))
		{
			return false;
		}

		if (pPlot->getBlockadedCount(getTeam()) > 0)
		{
			return false;
		}

		/* Replaced by blockade mission, above
		if (!(pPlot->plotCheck(PUF_canDefend, -1, -1, NO_PLAYER, getTeam())))
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater())
					{
						if (pLoopPlot->plotCheck(PUF_canSiege, getOwnerINLINE()) != NULL)
						{
							return false;
						}
					}
				}
			}
		}
		*/
	}

	if (!(pPlot->hasYield()))
	{
		return false;
	}

	return true;
}


void CvCity::verifyWorkingPlot(int iIndex)
{
	FAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	if (isWorkingPlot(iIndex))
	{
		CvPlot* pPlot = getCityIndexPlot(iIndex);

		if (pPlot != NULL)
		{
			if (!canWork(pPlot))
			{
				setWorkingPlot(iIndex, false);

				AI_setAssignWorkDirty(true);
			}
		}
	}
}


void CvCity::verifyWorkingPlots()
{
	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		verifyWorkingPlot(iI);
	}
}


void CvCity::clearWorkingOverride(int iIndex)
{
	CvPlot* pPlot = getCityIndexPlot(iIndex);

	if (pPlot != NULL)
	{
		pPlot->setWorkingCityOverride(NULL);
	}
}


int CvCity::countNumImprovedPlots(ImprovementTypes eImprovement, bool bPotential) const
{
	int iCount = 0;

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (eImprovement != NO_IMPROVEMENT)
				{
					if (pLoopPlot->getImprovementType() == eImprovement || 
						(bPotential && pLoopPlot->canHaveImprovement(eImprovement, getTeam())))
					{
						++iCount;
					}
				}
				else if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvCity::countNumWaterPlots() const
{
	int iCount = 0;

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (pLoopPlot->isWater())
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}

int CvCity::countNumRiverPlots() const
{
	int iCount = 0;

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (pLoopPlot->isRiver())
				{
					++iCount;
				}
			}
		}
	}

	return iCount;
}


int CvCity::findPopulationRank() const
{
	if (!m_bPopulationRankValid)
	{
		int iRank = 1;

		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			if ((pLoopCity->getPopulation() > getPopulation()) ||
				((pLoopCity->getPopulation() == getPopulation()) && (pLoopCity->getID() < getID())))
			{
				iRank++;
			}
		}

		// shenanigans are to get around the const check
		m_bPopulationRankValid = true;
		m_iPopulationRank = iRank;
	}

	return m_iPopulationRank;
}


int CvCity::findBaseYieldRateRank(YieldTypes eYield) const
{
	if (!m_abBaseYieldRankValid[eYield])
	{
		//TB Traits begin
		int iRate = getModifiedBaseYieldRate(eYield);
		//TB Traits end

		int iRank = 1;

		int iLoop;
		for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			//TB Traits begin
			if ((pLoopCity->getModifiedBaseYieldRate(eYield) > iRate) ||
				((pLoopCity->getModifiedBaseYieldRate(eYield) == iRate) && (pLoopCity->getID() < getID())))
			//TB Traits end
			{
				iRank++;
			}
		}

		m_abBaseYieldRankValid[eYield] = true;
		m_aiBaseYieldRank[eYield] = iRank;
	}

	return m_aiBaseYieldRank[eYield];
}


int CvCity::findYieldRateRank(YieldTypes eYield) const
{
	PROFILE_FUNC()

	if (!m_abYieldRankValid[eYield])
	{
		int iRate = getYieldRate(eYield);

		int iRank = 1;

		int iLoop;
		for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			if ((pLoopCity->getYieldRate(eYield) > iRate) ||
				((pLoopCity->getYieldRate(eYield) == iRate) && (pLoopCity->getID() < getID())))
			{
				iRank++;
			}
		}

		m_abYieldRankValid[eYield] = true;
		m_aiYieldRank[eYield] = iRank;
	}

	return m_aiYieldRank[eYield];
}


int CvCity::findCommerceRateRank(CommerceTypes eCommerce) const
{
	if (!m_abCommerceRankValid[eCommerce])
	{
		int iRate = getCommerceRateTimes100(eCommerce);

		int iRank = 1;

		int iLoop;
		for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			if ((pLoopCity->getCommerceRateTimes100(eCommerce) > iRate) ||
					((pLoopCity->getCommerceRateTimes100(eCommerce) == iRate) && (pLoopCity->getID() < getID())))
			{
				iRank++;
			}
		}

		m_abCommerceRankValid[eCommerce] = true;
		m_aiCommerceRank[eCommerce] = iRank;
	}

	return m_aiCommerceRank[eCommerce];
}


/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
bool CvCity::isPlotTrainable(UnitTypes eUnit, bool bContinue, bool bTestVisible) const
{
	PROFILE_FUNC();

	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
	CvPlayer& pPlayer = GET_PLAYER(getOwnerINLINE());

	if (!bTestVisible)
	{
		if (kUnit.isStateReligion())
		{
			if (pPlayer.getStateReligion() != NO_RELIGION)
			{
				if(!(isHasReligion(pPlayer.getStateReligion())))
				{
					return false;
				}
			}
		}

		if (kUnit.getPrereqBuilding() != NO_BUILDING)
		{

			if (!getNumBuilding((BuildingTypes)kUnit.getPrereqBuilding()))
			{
				SpecialBuildingTypes eSpecialBuilding = ((SpecialBuildingTypes)(GC.getBuildingInfo((BuildingTypes)(kUnit.getPrereqBuilding())).getSpecialBuildingType()));

				if ((eSpecialBuilding == NO_SPECIALBUILDING) || !(pPlayer.isSpecialBuildingNotRequired(eSpecialBuilding)))
				{
					return false;
				}
			}
		}

		if (kUnit.isPrereqBuildingClass(NO_BUILDINGCLASS))
		{
			CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());
			int const numNumBuildingClassInfos = GC.getNumBuildingClassInfos();

			for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
			{
				if (kUnit.isPrereqBuildingClass(iI))
				{
					if (pPlayer.isBuildingClassRequiredToTrain(BuildingClassTypes(iI), eUnit))
					{
						if (!(getNumBuilding((BuildingTypes)kCivilizationInfo.getCivilizationBuildings(iI))))
						{
							return false;
						}
					}
				}
			}
		}
		
		if (kUnit.getPrereqOrBuildingsNum() > 0)
		{
			bool bFound = false;
			const int iNum = kUnit.getPrereqOrBuildingsNum();
			for (int i = 0; i < iNum; i++)
			{
				if(getNumBuilding(kUnit.getPrereqOrBuilding(i)) > 0)
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
				return false;
		}

		if (kUnit.getTrainCondition())
		{
			if (!kUnit.getTrainCondition()->evaluate(const_cast<CvGameObjectCity*>(getGameObjectConst())))
			{
				return false;
			}
		}
	}

	return true;
}

//Returns true if the city can train a unit, or any upgrade for that unit that forces it obsolete
bool CvCity::isForceObsoleteUnitClassAvailable(UnitTypes eUnit) const
{
	PROFILE_FUNC();

	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());

	FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned (not NO_UNIT)");

	if (kUnit.getForceObsoleteUnitClass(NO_UNITCLASS))
	{
		const int numUnitClassInfos = GC.getNumUnitClassInfos();

		for (int iI = 0; iI < numUnitClassInfos; iI++)
		{
			if (kUnit.getForceObsoleteUnitClass(iI))
			{
				UnitTypes eLoopUnit = (UnitTypes)kCivilization.getCivilizationUnits(iI);
				if(eLoopUnit == NO_UNIT)
				{
					continue;
				}
				CvUnitInfo& kLoopUnit = GC.getUnitInfo(eLoopUnit);

				if (canTrain(eLoopUnit, false, false, false, true))
				{
					return true;
				}
				
		
				for (int iJ = 0; iJ < numUnitClassInfos; iJ++)
				{
					if (kLoopUnit.getUpgradeUnitClass(iJ))
					{
						eLoopUnit = (UnitTypes)kCivilization.getCivilizationUnits(iJ);
						
						if (eLoopUnit != NO_UNIT)
						{
							if (canTrain(eLoopUnit, false, false, false, true))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/


// Returns one of the upgrades...
UnitTypes CvCity::allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount) const
{
	PROFILE_FUNC();

	UnitTypes eResult = NO_UNIT;
	bool bHasCachedResult = false;
	
	//OutputDebugString(CvString::format("allUpgradesAvailable for %d (recursion depth %d)\n", eUnit, iUpgradeCount).c_str());
	if ( iUpgradeCount == 0 )
	{
		stdext::hash_map<UnitTypes,UnitTypes>::const_iterator itr = m_eCachedAllUpgradesResultsRoot.find(eUnit);
		if ( itr != m_eCachedAllUpgradesResultsRoot.end() )
		{
			//OutputDebugString(CvString::format("\t...cached result %d\n", itr->second).c_str());
			eResult = itr->second;
			bHasCachedResult = true;
		}
	}
	else
	{
		stdext::hash_map<UnitTypes,UnitTypes>::const_iterator itr = m_eCachedAllUpgradesResults.find(eUnit);
		if ( itr != m_eCachedAllUpgradesResults.end() )
		{
			//OutputDebugString(CvString::format("\t...cached result %d\n", itr->second).c_str());
			eResult = itr->second;
			bHasCachedResult = true;
		}
	}

	if ( !bHasCachedResult )
	{
	/************************************************************************************************/
	/* REVDCM                                 04/16/10                                phungus420    */
	/*                                                                                              */
	/* CanTrain Performance                                                                         */
	/************************************************************************************************/
		CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
		CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());
		const int numNumUnitClassInfos = GC.getNumUnitClassInfos();

		FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned (not NO_UNIT)");

		if (iUpgradeCount <= numNumUnitClassInfos)
		{
			UnitTypes eUpgradeUnit = NO_UNIT;

			bool bUpgradeFound = false;
			bool bUpgradeAvailable = false;
			bool bUpgradeUnavailable = false;

			for (int iI = 0; iI < numNumUnitClassInfos; iI++)
			{
				if (kUnit.getUpgradeUnitClass(iI))
				{
					UnitTypes eLoopUnit = (UnitTypes)kCivilization.getCivilizationUnits(iI);
		/************************************************************************************************/
		/* REVDCM                                  END Performance                                      */
		/************************************************************************************************/

					if (eLoopUnit != NO_UNIT)
					{
						bUpgradeFound = true;

						UnitTypes eTempUnit = allUpgradesAvailable(eLoopUnit, (iUpgradeCount + 1));

						if (eTempUnit != NO_UNIT)
						{
							eUpgradeUnit = eTempUnit;
							bUpgradeAvailable = true;
						}
						else
						{
							bUpgradeUnavailable = true;
						}
					}
				}
			}

			if (iUpgradeCount > 0)
			{
				if (bUpgradeFound && bUpgradeAvailable)
				{
					FAssertMsg(eUpgradeUnit != NO_UNIT, "eUpgradeUnit is expected to be assigned (not NO_UNIT)");
					eResult = eUpgradeUnit;
				}
				else if (canTrain(eUnit, false, false, false, true))
				{
					eResult = eUnit;
				}

				//	Cache the result so that we don't have to recalculate it multiple times
				//OutputDebugString(CvString::format("\t...caching result %d\n", eResult).c_str());
				m_eCachedAllUpgradesResults[eUnit] = eResult;
			}
			else
			{
				if (bUpgradeFound && !bUpgradeUnavailable)
				{
					eResult = eUpgradeUnit;
				}

				//	Cache the result so that we don't have to recalculate it multiple times
				//OutputDebugString(CvString::format("\t...caching result %d\n", eResult).c_str());
				m_eCachedAllUpgradesResultsRoot[eUnit] = eResult;
			}
		}
	}
	
	return eResult;
}

int CvCity::getMaxNumWorldWonders() const
{
	return GC.getCultureLevelInfo(getCultureLevel()).getMaxWorldWonders();
}

bool CvCity::isWorldWondersMaxed() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	//if (GC.getDefineINT("MAX_WORLD_WONDERS_PER_CITY") == -1)
	//{
	//	return false;
	//}
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_UNLIMITED_WONDERS))
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

	//if (getNumWorldWonders() >= GC.getDefineINT("MAX_WORLD_WONDERS_PER_CITY"))
	if (getNumWorldWonders() >= getMaxNumWorldWonders())
	{
		return true;
	}

	return false;
}

int CvCity::getMaxNumTeamWonders() const
{
	return GC.getCultureLevelInfo(getCultureLevel()).getMaxTeamWonders();
}

bool CvCity::isTeamWondersMaxed() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	//if (GC.getDefineINT("MAX_TEAM_WONDERS_PER_CITY") == -1)
	//{
	//	return false;
	//}
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_UNLIMITED_WONDERS))
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	
	//if (getNumTeamWonders() >= GC.getDefineINT("MAX_TEAM_WONDERS_PER_CITY"))
	if (getNumTeamWonders() >= getMaxNumTeamWonders())
	{
		return true;
	}

	return false;
}


int CvCity::getMaxNumNationalWonders() const
{
	return (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman()) ? GC.getCultureLevelInfo(getCultureLevel()).getMaxNationalWondersOCC() : GC.getCultureLevelInfo(getCultureLevel()).getMaxNationalWonders();
}

bool CvCity::isNationalWondersMaxed() const
{
	//int iMaxNumWonders = (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman()) ? GC.getDefineINT("MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC") : GC.getDefineINT("MAX_NATIONAL_WONDERS_PER_CITY");
	int iMaxNumWonders = getMaxNumNationalWonders();

	if (iMaxNumWonders == -1)
	{
		return false;
	}
	
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_UNLIMITED_WONDERS))
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	if (getNumNationalWonders() >= iMaxNumWonders)
	{
		return true;
	}

	return false;
}


bool CvCity::isBuildingsMaxed() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	if (GC.getDefineINT("MAX_BUILDINGS_PER_CITY") == -1)
	{
		return false;
	}

	if (getNumBuildings() >= GC.getDefineINT("MAX_BUILDINGS_PER_CITY"))
	{
		return true;
	}

	return false;
}

bool CvCity::canTrainInternal(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades) const
{
	PROFILE("CvCity::canTrainInternal (units)");

	if (eUnit == NO_UNIT)
	{
		return false;
	}

	if(GC.getUSE_CAN_TRAIN_CALLBACK(eUnit))
	{
		PROFILE("canTrain.Python");

		CyCity* pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(eUnit);
		argsList.add(bContinue);
		argsList.add(bTestVisible);
		argsList.add(bIgnoreCost);
		argsList.add(bIgnoreUpgrades);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "canTrain", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return true;
		}
	}

	if (!plot()->canTrain(eUnit, bContinue, bTestVisible))
	{
		return false;
	}
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
	if (isForceObsoleteUnitClassAvailable(eUnit))
	{
		return false;
	}

	if (!(isPlotTrainable(eUnit, bContinue, bTestVisible)))
	{
		return false;
	}
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 6/17/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	CvUnitInfo &kUnit = GC.getUnitInfo(eUnit);
	if (kUnit.isForceUpgrade())
	{
		if(canUpgradeUnit(eUnit))
		{
			return false;
		}
	}
	
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());
	bool bQual = false;
	if (kCivilization.isStronglyRestricted() && isNPC())
	{
		for (int iI = 0; iI < kUnit.getNumEnabledCivilizationTypes(); iI++)
		{
			if (getCivilizationType() == kUnit.getEnabledCivilizationType(iI).eCivilization)
			{
				bQual = true;
				break;
			}
		}
	}
	else
	{
		bQual = true;
	}
	if (!bQual)
	{
		return false;
	}

	if (!bTestVisible)
	{
		ReligionTypes eStateReligion = GET_PLAYER(getOwnerINLINE()).getStateReligion();
		if (kUnit.isRequiresStateReligionInCity())
		{	
			if (NO_RELIGION == eStateReligion || !isHasReligion(eStateReligion))
			{
				return false;
			}
		}
	}
	
	if (GC.getGameINLINE().isOption(GAMEOPTION_ASSIMILATION))
	{
		UnitClassTypes eUnitClass = ((UnitClassTypes)(kUnit.getUnitClassType()));

		if (GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass) != eUnit)
		{
			return false;
		}
	}
	if (kUnit.getPrereqVicinityBonus() != NO_BONUS)
	{
		if (!hasVicinityBonus((BonusTypes)kUnit.getPrereqVicinityBonus()))
		{
			return false;
		}
	}
	if (!bTestVisible)
	{
		bool bHasAnyVicinityBonus = false;
		bool bRequiresAnyVicinityBonus = false;
		for (int iI = 0; iI < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); iI++)
		{
			if (kUnit.getPrereqOrVicinityBonuses(iI) != NO_BONUS)
			{
				bRequiresAnyVicinityBonus = true;
				if (hasVicinityBonus((BonusTypes)kUnit.getPrereqOrVicinityBonuses(iI)))
				{
					bHasAnyVicinityBonus = true;
					break;
				}
			}
		}
		if (bRequiresAnyVicinityBonus && !bHasAnyVicinityBonus)
		{
			return false;
		}
	}
	if (GC.getGameINLINE().isOption(GAMEOPTION_REALISTIC_CORPORATIONS))
	{
		if (GET_PLAYER(getOwnerINLINE()).isNoForeignCorporations())
		{
			for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
			{
				if (kUnit.getCorporationSpreads(iI) > 0)
				{
					return false;
				}
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	


	if (!bIgnoreUpgrades)
	{
		if (allUpgradesAvailable(eUnit) != NO_UNIT)
		{
			return false;
		}
	}

	if(GC.getUSE_CANNOT_TRAIN_CALLBACK(eUnit))
	{

		CyCity *pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList2; // XXX
		argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList2.add(eUnit);
		argsList2.add(bContinue);
		argsList2.add(bTestVisible);
		argsList2.add(bIgnoreCost);
		argsList2.add(bIgnoreUpgrades);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "cannotTrain", argsList2.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return false;
		}
	}

	return true;
}

#ifdef CAN_TRAIN_CACHING
#ifdef _DEBUG
//	Uncomment this for consistency testing of the canTrain cache
//#define VALIDATE_CAN_TRAIN_CACHE_CONSISTENCY
#endif

void CvCity::populateCanTrainCache(bool bUnconditional) const
{
	PROFILE_FUNC();
	MEMORY_TRACK_EXEMPT();

	if ( bUnconditional || !m_canTrainCachePopulated )
	{
		clearCanTrainCache();

		int iCount = 0;
		const int numUnitInfos = GC.getNumUnitInfos();
		for (int iI = 0; iI < numUnitInfos; iI++)
		{
			if (canTrain((UnitTypes)iI))
			{
				m_canTrainCacheUnits[(UnitTypes)iI] = true;
				iCount++;
			}
		}

		if ( iCount == 0 )
		{
			OutputDebugString("Nothing trainable!\n");
		}

		m_canTrainCachePopulated = true;
		m_canTrainCacheDirty = false;

	}
}

void CvCity::clearCanTrainCache() const
{
	m_canTrainCachePopulated = false;
	m_canTrainCacheUnits.clear();

	clearUpgradeCache(NO_UNIT);
}
#endif

void CvCity::clearUpgradeCache(UnitTypes eUnit) const
{
	if (eUnit == NO_UNIT)
	{
		m_eCachedAllUpgradesResults.clear();
		m_eCachedAllUpgradesResultsRoot.clear();
	}
	else
	{
		m_eCachedAllUpgradesResults.erase(eUnit);
		m_eCachedAllUpgradesResultsRoot.erase(eUnit);
	}
}

bool CvCity::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades) const
{
	if ( !GET_PLAYER(getOwnerINLINE()).canTrain(eUnit, bContinue, bTestVisible, bIgnoreCost) )
	{
		return false;
	}
	else
#ifdef CAN_TRAIN_CACHING
	if ( !bContinue && !bTestVisible && !bIgnoreCost && !bIgnoreUpgrades )
	{
		if ( m_canTrainCachePopulated )
		{
			bool bHaveCachedResult;
			bool bResult;

			if ( m_canTrainCacheDirty )
			{
				PROFILE("CvCity::canTrain.ProcessDirtyCache");

				//	Needs repopulating
				populateCanTrainCache();
			}

			stdext::hash_map<UnitTypes,bool>::iterator itr = m_canTrainCacheUnits.find(eUnit);
			if ( itr == m_canTrainCacheUnits.end() )
			{
#ifdef VALIDATE_CAN_TRAIN_CACHE_CONSISTENCY
				if ( canTrainInternal(eUnit) )
				{
					FAssertMsg(false, "Consistency check failure in canTrain cache - false negative\n");
				}
#endif
				bResult = false;
				bHaveCachedResult = true;
			}
			else if ( itr->second )
			{
#ifdef VALIDATE_CAN_TRAIN_CACHE_CONSISTENCY
				if ( !canTrainInternal(eUnit) )
				{
					FAssertMsg(false, "Consistency check failure in canTrain cache - false positive\n");
				}
#endif
				bResult = true;
				bHaveCachedResult = true;
			}
			else	//	In map but with false => recalculate
			{
				bHaveCachedResult = false;
			}

			if ( !bHaveCachedResult )
			{
				bResult = canTrainInternal(eUnit);

				if ( bResult )
				{
					m_canTrainCacheUnits[eUnit] = true;
				}
				else
				{
					m_canTrainCacheUnits.erase(eUnit);
				}

			}

			return bResult;
		}
	}
#endif

	PROFILE("canTrain.NonStandard");
	return canTrainInternal(eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades);
}

void CvCity::invalidateCachedCanTrainForUnit(UnitTypes eUnit) const
{
	PROFILE_FUNC();

	if (m_canTrainCachePopulated)
	{
		PROFILE("CvCity::invalidateCachedCanTrainForUnit");

		if (eUnit == NO_UNIT)
		{
			m_canTrainCacheDirty = true;	//	Entire map dirty
		}
		else
		{
			m_canTrainCacheUnits[eUnit] = false;
		}
	}

	clearUpgradeCache(eUnit);

}

bool CvCity::canTrain(UnitCombatTypes eUnitCombat) const
{
	PROFILE_FUNC();
	//TB SubCombat Mod Begin
	const int num = GC.getNumUnitClassInfos();
	CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());

	for (int iI = 0; iI < num; iI++)
	{
		const UnitTypes eUnit = (UnitTypes)kCivilizationInfo.getCivilizationUnits(iI);

		if (NO_UNIT != eUnit &&	GC.getUnitInfo(eUnit).hasUnitCombat(eUnitCombat) &&	canTrain(eUnit))
		{
			return true;
		}
	}
	//TB SubCombat Mod end

	return false;
}

//	KOSHLING - cache can construct values
#ifdef _DEBUG
//	Uncomment to add runtime results checking
//#define VALIDATE_CAN_CONSTRUCT_CACHE
#endif

void CvCity::FlushCanConstructCache(BuildingTypes eBuilding)
{
	//OutputDebugString(CvString::format("[%d] FlushCanConstructCache (%d), workitem priority = %08lx\n", GetCurrentThreadId(), eBuilding, (m_workItem == NULL ? -1 : m_workItem->GetPriority())).c_str());

	if ( eBuilding == NO_BUILDING )
	{
		SAFE_DELETE(m_bCanConstruct);
	}
	else if ( m_bCanConstruct != NULL )
	{
		(*m_bCanConstruct).erase(eBuilding);
	}
}

void CvCity::NoteBuildingNoLongerConstructable(BuildingTypes eBuilding) const
{
	if ( m_bCanConstruct != NULL )
	{
		(*m_bCanConstruct)[eBuilding] = false;
	}
}

bool CvCity::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreAmount, bool bIgnoreBuildings, TechTypes eIgnoreTechReq, int* probabilityEverConstructable, bool bAffliction, bool bExposed) const
{
	PROFILE_FUNC();

	if ( eBuilding == NO_BUILDING )
	{
		return false;
	}
	else if ( !bContinue && !bTestVisible && !bIgnoreCost && !bIgnoreAmount && !bIgnoreBuildings && eIgnoreTechReq == NO_TECH && probabilityEverConstructable == NULL && !bAffliction && !bExposed)
	{
		bool bResult;
		bool bHaveCachedResult;

		if ( m_bCanConstruct == NULL )
		{
			MEMORY_TRACK_EXEMPT()

			m_bCanConstruct = new std::map<int,bool>();
			bHaveCachedResult = false;
		}
		else
		{
			std::map<int,bool>::const_iterator itr = m_bCanConstruct->find(eBuilding);
			if ( itr == m_bCanConstruct->end() )
			{
				bHaveCachedResult = false;
			}
			else
			{
				bResult = itr->second;
				bHaveCachedResult = true;
#ifdef VALIDATE_CAN_CONSTRUCT_CACHE
				//	Verify if required
				if ( bResult != canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost, bIgnoreAmount, NO_BUILDINGCLASS, bIgnoreBuildings, eIgnoreTechReq, NULL, bAffliction, bExposed) )
				{
					MessageBox(NULL,"canConstruct cached result mismatch","cvGameCore",MB_OK);
					FAssertMsg(false, "canConstruct cached result mismatch");
				}
#endif
			}
		}

		if ( !bHaveCachedResult )
		{
			bResult = canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost, bIgnoreAmount, NO_BUILDINGCLASS, bIgnoreBuildings, eIgnoreTechReq, NULL, bAffliction, bExposed);
			{
				MEMORY_TRACK_EXEMPT()

				if ( m_bCanConstruct == NULL )
				{
					MEMORY_TRACK_EXEMPT()

					m_bCanConstruct = new std::map<int,bool>();
				}
				(*m_bCanConstruct)[eBuilding] = bResult;
			}
		}

		return bResult;
	}
	else
	{
		return canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost, bIgnoreAmount, NO_BUILDINGCLASS, bIgnoreBuildings, eIgnoreTechReq, probabilityEverConstructable, bAffliction, bExposed);
	}
}
//	KOSHLING - Can construct cache end

/************************************************************************************************/
/* Afforess	                  Start		 05/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
bool CvCity::canConstructInternal(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreAmount, BuildingClassTypes withExtraBuildingClass, bool bIgnoreBuildings, TechTypes eIgnoreTechReq, int* probabilityEverConstructable, bool bAffliction, bool bExposed) const
{
	PROFILE_FUNC()

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

/************************************************************************************************/
/* REVDCM                                 04/16/10                                phungus420    */
/*                                                                                              */
/* CanConstruct Performance                                                                     */
/************************************************************************************************/
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());

	if ( probabilityEverConstructable != NULL )
	{
		*probabilityEverConstructable = 0;
	}

	if (eBuilding == NO_BUILDING)
	{
		return false;
	}

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	
	bool bQual = false;
	if (kCivilization.isStronglyRestricted() && isNPC())
	{
		for (int iI = 0; iI < kBuilding.getNumEnabledCivilizationTypes(); iI++)
		{
			if (getCivilizationType() == kBuilding.getEnabledCivilizationType(iI).eCivilization)
			{
				bQual = true;
				break;
			}
		}
	}
	else
	{
		bQual = true;
	}
	if (!bQual)
	{
		return false;
	}

	if(GC.getUSE_CAN_CONSTRUCT_CALLBACK())
	{
		CyCity* pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(eBuilding);
		argsList.add(bContinue);
		argsList.add(bTestVisible);
		argsList.add(bIgnoreCost);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "canConstruct", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return true;
		}
	}
	
	//ls612: No Holy City Tag
	if (GC.getBuildingInfo(eBuilding).isNoHolyCity() && isHolyCity() && !bExposed)
	{
		return false;
	}

	if (!(GET_PLAYER(getOwnerINLINE()).canConstruct(eBuilding, bContinue, bTestVisible, bIgnoreCost, eIgnoreTechReq, probabilityEverConstructable, bAffliction, bExposed)))
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	                  Start		 05/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (!bIgnoreAmount)
	{
		if (getNumBuilding(eBuilding) >= GC.getCITY_MAX_NUM_BUILDINGS())
		{
			return false;
		}
	}
	if (isDisabledBuilding(eBuilding))
	{
		return false;
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ASSIMILATION))
	{
		BuildingClassTypes eBuildingClass = ((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType()));
		if (kCivilization.getCivilizationBuildings(eBuildingClass) != eBuilding)
		{
			return false;
		}
	}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	if (!bExposed)
	{
		if (kBuilding.isPrereqReligion())
		{
			if (getReligionCount() > 0)
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 80;
				}
				return false;
			}
		}

		if (kBuilding.isStateReligion())
		{
			ReligionTypes eStateReligion = GET_PLAYER(getOwnerINLINE()).getStateReligion();
			if (NO_RELIGION == eStateReligion || !isHasReligion(eStateReligion))
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 80;
				}
				return false;
			}
		}

		if (kBuilding.getPrereqReligion() != NO_RELIGION)
		{
			if (!(isHasReligion((ReligionTypes)(kBuilding.getPrereqReligion()))))
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 50;
				}
				return false;
			}
		}

		CorporationTypes prereqCorp = (CorporationTypes)kBuilding.getPrereqCorporation();
		if (prereqCorp != NO_CORPORATION)
		{
			if (!isHasCorporation(prereqCorp))
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 30;
				}
				return false;
			}
		}

		CorporationTypes foundsCorp = (CorporationTypes)kBuilding.getFoundsCorporation();
		if (foundsCorp != NO_CORPORATION)
		{
			if (GC.getGameINLINE().isCorporationFounded(foundsCorp))
			{
				return false;
			}

			for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); ++iCorporation)
			{
				if (isHeadquarters((CorporationTypes)iCorporation))
				{
					if (GC.getGameINLINE().isCompetingCorporation((CorporationTypes)iCorporation, foundsCorp))
					{
						return false;
					}
				}
			}
		}

		if (!isValidBuildingLocation(eBuilding))
		{
			return false;
		}

		if (kBuilding.isGovernmentCenter())
		{
			if (isGovernmentCenter())
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 10;
				}
				return false;
			}
		}
	}

	if (!bTestVisible && !bExposed)
	{
		if (!bAffliction)
		{
			if (!bContinue)
			{
				if (getFirstBuildingOrder(eBuilding) != -1)
				{
					return false;
				}
			}

			if (isNPC())
			{
				if (isWorldWonderClass((BuildingClassTypes)(kBuilding.getBuildingClassType())))
				{
					return false;
				}
				else if (isTeamWonderClass((BuildingClassTypes)(kBuilding.getBuildingClassType())))
				{
					return false;
				}
				else if (isNationalWonderClass((BuildingClassTypes)(kBuilding.getBuildingClassType())))
				{
					return false;
				}
			}

			if (!(GC.getBuildingClassInfo((BuildingClassTypes)(kBuilding.getBuildingClassType())).isNoLimit()))
			{
				if (isWorldWonderClass((BuildingClassTypes)(kBuilding.getBuildingClassType())))
				{
					if (isWorldWondersMaxed())
					{
						return false;
					}
				}
				else if (isTeamWonderClass((BuildingClassTypes)(kBuilding.getBuildingClassType())))
				{
					if (isTeamWondersMaxed())
					{
						return false;
					}
				}
				else if (isNationalWonderClass((BuildingClassTypes)(kBuilding.getBuildingClassType())))
				{
					if (isNationalWondersMaxed())
					{
						return false;
					}
				}
				else
				{
					if (isBuildingsMaxed())
					{
						return false;
					}
				}
			}
		}

		if (kBuilding.getHolyCity() != NO_RELIGION)
		{
			if (!isHolyCity(((ReligionTypes)(kBuilding.getHolyCity()))))
			{
				return false;
			}
		}

		if (kBuilding.getPrereqAndBonus() != NO_BONUS)
		{
			if (!hasBonus((BonusTypes)kBuilding.getPrereqAndBonus()))
			{
				if (probabilityEverConstructable != NULL)
				{
					*probabilityEverConstructable = 50;
				}
				return false;
			}
		}

		CorporationTypes foundsCorp = (CorporationTypes)kBuilding.getFoundsCorporation();
		if (foundsCorp != NO_CORPORATION && !bAffliction)
		{
			if (GC.getGameINLINE().isCorporationFounded(foundsCorp))
			{
				return false;
			}

			if (GET_PLAYER(getOwnerINLINE()).isNoCorporations())
			{
				return false;
			}
/************************************************************************************************/
/* Afforess	                  Start		 02/17/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			bool bValid = false;
			bool bRequiresBonus = false;
			for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
			{
				BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(foundsCorp).getPrereqBonus(i);
				if (NO_BONUS != eBonus)
				{
					bRequiresBonus = true;
					if (hasBonus(eBonus))
					{
						bValid = true;
						break;
					}
				}
			}

			if (!bValid && bRequiresBonus)
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 30;
				}
				return false;
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		}

		if (!bAffliction) //bAffliction ONLY applies during the Outbreaks and Afflictions option being on!
		{
			if (!(*getPropertiesConst() <= *(kBuilding.getPrereqMaxProperties())))
					return false;

			if (!(*getPropertiesConst() >= *(kBuilding.getPrereqMinProperties())))
					return false;
		}

		if (plot()->getLatitude() > kBuilding.getMaxLatitude())
		{
			return false;
		}

		if (plot()->getLatitude() < kBuilding.getMinLatitude())
		{
			return false;
		}

		if (kBuilding.isBuildOnlyOnPeaks())
		{
			if (!(plot()->isPeak2(true)))
			{
				return false;
			}
		}

		int iPrereqPopulation = std::max(kBuilding.getPrereqPopulation(), getNumPopulationEmployed() + kBuilding.getNumPopulationEmployed() + 1);
		if (iPrereqPopulation > 0)
		{
			if (getPopulation() < iPrereqPopulation)
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 50;
				}
				return false;
			}
		}

		if (kBuilding.getPrereqCultureLevel() != NO_CULTURELEVEL)
		{
			if (getCultureLevel() < kBuilding.getPrereqCultureLevel())
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 50;
				}
				return false;
			}
		}

		if (kBuilding.getPrereqAnyoneBuildingClass() != NO_BUILDINGCLASS)
		{
			BuildingClassTypes ePrereqBuildingClass = (BuildingClassTypes)kBuilding.getPrereqAnyoneBuildingClass();
			if (GC.getGameINLINE().getBuildingClassCreatedCount(ePrereqBuildingClass) == 0)
			{
				return false;
			}
		}

		bool bRequiresBonus = false;
		bool bNeedsBonus = true;

		for (int iI = 0; iI < kBuilding.getNumPrereqOrBonuses(); iI++)
		{
			if (kBuilding.getPrereqOrBonuses(iI) != NO_BONUS)
			{
				bRequiresBonus = true;

				if (hasBonus((BonusTypes)kBuilding.getPrereqOrBonuses(iI)))
				{
					bNeedsBonus = false;
				}
			}
		}

		if (bRequiresBonus && bNeedsBonus)
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 50;
			}
			return false;
		}
	}
	
	//Hide Buildings that shouldn't appear in the early game and require other buildings
	bool bOldTestVisible = bTestVisible;
	if (kBuilding.getPrereqAndTech() == NO_TECH && kBuilding.getObsoleteTech() == NO_TECH)
	{
		bTestVisible = false;
	}

/************************************************************************************************/
/* Koshling - moved this so non-met vicinity required buildings get hidden in city screen even */
/* when not ordinarily hiding uncosntructable (since you cannot influence your vicinity bonuses */
/* so its not useful information for the most part												*/
/*																								*/
/* Afforess	                  Start		 06/13/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int numNumBuildingClassInfos = GC.getNumBuildingClassInfos();
	if (!bExposed)
	{
		if (kBuilding.getPrereqVicinityBonus() != NO_BONUS)
		{
			if (!hasVicinityBonus((BonusTypes)kBuilding.getPrereqVicinityBonus()))
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 5;
				}
				return false;
			}
		}
		if (kBuilding.getPrereqRawVicinityBonus() != NO_BONUS)
		{
			if (!hasRawVicinityBonus((BonusTypes)kBuilding.getPrereqRawVicinityBonus()))
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 5;
				}
				return false;
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		if (!bTestVisible && !bIgnoreBuildings)
		{
			if (kBuilding.isBuildingClassNeededInCity(NO_BUILDINGCLASS))
			{
				for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
				{
					if (kBuilding.isBuildingClassNeededInCity(iI))
					{
						BuildingTypes ePrereqBuilding = (BuildingTypes)(kCivilization.getCivilizationBuildings(iI));
		/************************************************************************************************/
		/* REVDCM                                  END Performance                                       */
		/************************************************************************************************/
						if (ePrereqBuilding != NO_BUILDING)
						{
							if (0 == getNumBuilding(ePrereqBuilding) /* && (bContinue || (getFirstBuildingOrder(ePrereqBuilding) == -1))*/ &&
								(BuildingClassTypes)iI != withExtraBuildingClass)
							{
								if ( probabilityEverConstructable != NULL )
								{
									*probabilityEverConstructable = 25;
								}
								return false;
							}
						}
					}
				}
			}
		}
	}

	bTestVisible = bOldTestVisible;
	
	if ((!bTestVisible && !bIgnoreBuildings) || bAffliction)
	{
		if (kBuilding.isPrereqNotBuildingClass(NO_BUILDINGCLASS))
		{
			for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
			{
				BuildingTypes eNotPrereqBuilding;
				if (kBuilding.isPrereqNotBuildingClass(iI))
				{
					eNotPrereqBuilding = ((BuildingTypes)(kCivilization.getCivilizationBuildings(iI)));

					if (eNotPrereqBuilding != NO_BUILDING)
					{
						if (getNumActiveBuilding(eNotPrereqBuilding) > 0)
						{
							return false;
						}
					}
				}
			}
		}

		bool bValid = false;
		bool bRequires = false;
		if (kBuilding.isPrereqOrBuildingClass(NO_BUILDINGCLASS))
		{
			for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
			{
				if (kBuilding.isPrereqOrBuildingClass(iI))
				{
					bRequires = true;
					BuildingTypes ePrereqBuilding = ((BuildingTypes)(kCivilization.getCivilizationBuildings(iI)));
					if (ePrereqBuilding != NO_BUILDING)
					{
						if (withExtraBuildingClass == (BuildingClassTypes)iI || getNumActiveBuilding(ePrereqBuilding) > 0)
						{
							bValid = true;
							break;
						}
					}
				}
			}
		}
		if (bRequires && !bValid)
		{
			if ( probabilityEverConstructable != NULL )
			{
				*probabilityEverConstructable = 25;
			}
			return false;
		}
		if (kBuilding.isPrereqPower())
		{
			if(!isPower())
			{
				if ( probabilityEverConstructable != NULL )
				{
					*probabilityEverConstructable = 25;
				}
				return false;
			}
		}
	}
	
	if (!bAffliction)
	{
		//Can not construct replaced buildings.
		if (kBuilding.isReplaceBuildingClass(NO_BUILDINGCLASS))
		{
			BuildingTypes eLoopBuilding;
			for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
			{
				eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
				if (eLoopBuilding != NO_BUILDING)
				{
					if (kBuilding.isReplaceBuildingClass(iI))
					{
						if (getNumActiveBuilding(eLoopBuilding) > 0 || (GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_HIDE_REPLACED_BUILDINGS) && canConstruct(eLoopBuilding, true, false, false, true)))
						{
							return false;
						}
					}
				}
			}
		}
	}

	//	Koshling - always hide things not buildable due to vicinity bonuses - its
	//	not really useful to see them
	//if (!bTestVisible)
	if (!bExposed)
	{
		bool bHasAnyVicinityBonus = false;
		bool bRequiresAnyVicinityBonus = false;
		for (int iI = 0; iI < GC.getNUM_BUILDING_PREREQ_OR_BONUSES(); iI++)
		{
			if (kBuilding.getPrereqOrVicinityBonuses(iI) != NO_BONUS)
			{
				bRequiresAnyVicinityBonus = true;
				if (hasVicinityBonus((BonusTypes)kBuilding.getPrereqOrVicinityBonuses(iI)))
				{
					bHasAnyVicinityBonus = true;
					break;
				}
			}
		}
		if (bRequiresAnyVicinityBonus && !bHasAnyVicinityBonus)
		{
			if ( probabilityEverConstructable != NULL )
			{
				*probabilityEverConstructable = 5;
			}
			return false;
		}
		bool bHasAnyRawVicinityBonus = false;
		bool bRequiresAnyRawVicinityBonus = false;
		for (int iI = 0; iI < GC.getNUM_BUILDING_PREREQ_OR_BONUSES(); iI++)
		{
			if (kBuilding.getPrereqOrRawVicinityBonuses(iI) != NO_BONUS)
			{
				bRequiresAnyRawVicinityBonus = true;
				if (hasRawVicinityBonus((BonusTypes)kBuilding.getPrereqOrRawVicinityBonuses(iI)))
				{
					bHasAnyRawVicinityBonus = true;
					break;
				}
			}
		}
		if (bRequiresAnyRawVicinityBonus && !bHasAnyRawVicinityBonus)
		{
			if ( probabilityEverConstructable != NULL )
			{
				*probabilityEverConstructable = 5;
			}
			return false;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	if (!bTestVisible && kBuilding.getConstructCondition() && !bExposed)
	{
		if (!kBuilding.getConstructCondition()->evaluate(const_cast<CvGameObjectCity*>(getGameObjectConst()))) // Const wegcasten ist hier ok da evaluate nicht wirklich etwas �ndert
		{
			return false;
		}
	}

	if(GC.getUSE_CANNOT_CONSTRUCT_CALLBACK())
	{
		CyCity *pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList2; // XXX
		argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList2.add(eBuilding);
		argsList2.add(bContinue);
		argsList2.add(bTestVisible);
		argsList2.add(bIgnoreCost);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "cannotConstruct", argsList2.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return false;
		}
	}

	return true;
}


bool CvCity::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_CREATE_PROJECT_CALLBACK())
	{
		CyCity* pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(eProject);
		argsList.add(bContinue);
		argsList.add(bTestVisible);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "canCreate", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return true;
		}	
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


	if (!(GET_PLAYER(getOwnerINLINE()).canCreate(eProject, bContinue, bTestVisible)))
	{
		return false;
	}

/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iCount = GC.getProjectInfo(eProject).getNumMapCategoryTypes();
	bool bFound = (iCount < 1);
	for (int iI = 0; iI < iCount; iI++)
	{
		if (plot()->isMapCategoryType((MapCategoryTypes)GC.getProjectInfo(eProject).getMapCategoryType(iI)))
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		return false;
	}

	if(GC.getUSE_CANNOT_CREATE_PROJECT_CALLBACK())
	{
		CyCity* pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList2; // XXX
		argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList2.add(eProject);
		argsList2.add(bContinue);
		argsList2.add(bTestVisible);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "cannotCreate", argsList2.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return false;
		}	
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


	return true;
}


bool CvCity::canMaintain(ProcessTypes eProcess, bool bContinue) const
{
/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_MAINTAIN_PROCESS_CALLBACK())
	{
		CyCity* pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(eProcess);
		argsList.add(bContinue);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "canMaintain", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return true;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (!(GET_PLAYER(getOwnerINLINE()).canMaintain(eProcess, bContinue)))
	{
		return false;
	}

/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CANNOT_MAINTAIN_PROCESS_CALLBACK())
	{
		CyCity* pyCity = new CyCity(const_cast<CvCity*>(this));
		CyArgsList argsList2; // XXX
		argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList2.add(eProcess);
		argsList2.add(bContinue);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "cannotMaintain", argsList2.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return false;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return true;
}


bool CvCity::canJoin() const
{
	return true;
}


int CvCity::getFoodTurnsLeft() const
{
	int iFoodLeft = (growthThreshold() - getFood());

	if (foodDifference() <= 0)
	{
		return iFoodLeft;
	}

	int iTurnsLeft = (iFoodLeft / foodDifference());

	if ((iTurnsLeft * foodDifference()) <  iFoodLeft)
	{
		iTurnsLeft++;
	}

	return std::max(1, iTurnsLeft);
}


bool CvCity::isProduction() const
{
	return (headOrderQueueNode() != NULL);
}


bool CvCity::isProductionLimited() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return isLimitedUnitClass((UnitClassTypes)(GC.getUnitInfo((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)).getUnitClassType()));
			break;

		case ORDER_CONSTRUCT:
			return isLimitedWonderClass((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)(pOrderNode->m_data.iData1)).getBuildingClassType()));
			break;

		case ORDER_CREATE:
			return isLimitedProject((ProjectTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return false;
}

bool CvCity::isProductionUnitCombat(int iIndex) const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getUnitInfo((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)).hasUnitCombat((UnitCombatTypes)iIndex);
			break;

		case ORDER_CONSTRUCT:
			return false;
			break;
		case ORDER_CREATE:
			return false;
			break;
		case ORDER_MAINTAIN:
			return false;
			break;
		case ORDER_LIST:
			return false;
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return false;
}


bool CvCity::isProductionUnit() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.eOrderType == ORDER_TRAIN);
	}

	return false;
}


bool CvCity::isProductionBuilding() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT);
	}

	return false;
}


bool CvCity::isProductionProject() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.eOrderType == ORDER_CREATE);
	}

	return false;
}


bool CvCity::isProductionProcess() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.eOrderType == ORDER_MAINTAIN);
	}

	return false;
}


bool CvCity::canContinueProduction(OrderData order)
{
	switch (order.eOrderType)
	{
	case ORDER_TRAIN:
		return canTrain((UnitTypes)EXTERNAL_ORDER_IDATA(order.iData1), true);
		break;

	case ORDER_CONSTRUCT:
		return canConstruct((BuildingTypes)(order.iData1), true);
		break;

	case ORDER_CREATE:
		return canCreate((ProjectTypes)(order.iData1), true);
		break;

	case ORDER_MAINTAIN:
		return canMaintain((ProcessTypes)(order.iData1), true);
		break;

	case ORDER_LIST:
		return true;
		break;

	default:
		FAssertMsg(false, "order.eOrderType failed to match a valid option");
		break;
	}

	return false;
}


int CvCity::getProductionExperience(UnitTypes eUnit)
{
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());

	int iExperience = getFreeExperience() + kPlayer.getFreeExperience();

	if (eUnit != NO_UNIT)
	{
		CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
		UnitClassTypes eUnitClass = (UnitClassTypes)kUnit.getUnitClassType();

		if (kUnit.isSpy() && !GC.isSS_ENABLED())
		{
			return 0;
		}

		if ((UnitTypes)kCivilization.getCivilizationUnits(eUnitClass) == eUnit && kUnit.canAcquireExperience())
		{
			iExperience += getSpecialistFreeExperience();

			if (kUnit.getUnitCombatType() != NO_UNITCOMBAT)
			{
				iExperience += getUnitCombatFreeExperience((UnitCombatTypes)kUnit.getUnitCombatType());
				iExperience += kPlayer.getUnitCombatFreeExperience((UnitCombatTypes)kUnit.getUnitCombatType());
			}

			for (int iI = 0; iI < kUnit.getNumSubCombatTypes(); iI++)
			{
				UnitCombatTypes eSubCombatType = ((UnitCombatTypes)kUnit.getSubCombatType(iI));
				iExperience += getUnitCombatFreeExperience(eSubCombatType);
				iExperience += kPlayer.getUnitCombatFreeExperience(eSubCombatType);
			}

			iExperience += getDomainFreeExperience((DomainTypes)(kUnit.getDomainType()));

		}
	}

	if (kPlayer.getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(kPlayer.getStateReligion()))
		{
			iExperience += kPlayer.getStateReligionFreeExperience();
		}
	}
//Team Project (6)
	int iExperienceModifier = 0;
	if (kPlayer.getCapitalXPModifier() != 0)
	{
		if (isCapital() || isGovernmentCenter())
		{
			iExperienceModifier += kPlayer.getCapitalXPModifier();
		}
	}

	if (kPlayer.getStateReligionHolyCityXPModifier() != 0)
	{
		if (isHolyCity(kPlayer.getStateReligion()))
		{
			iExperienceModifier += kPlayer.getStateReligionHolyCityXPModifier();
		}
	}

	if (kPlayer.getNonStateReligionHolyCityXPModifier() != 0)
	{
		if (isHolyCity() && !isHolyCity(kPlayer.getStateReligion()))
		{
			iExperienceModifier += kPlayer.getNonStateReligionHolyCityXPModifier();
		}
	}

	if (iExperienceModifier != 0)
	{
		iExperience *= (iExperienceModifier + 100);
		iExperience /= 100;
	}

	return std::max(0, iExperience);
}

void CvCity::addProductionExperience(CvUnit* pUnit, bool bConscript)
{
	PROFILE_FUNC();

	if (pUnit->canAcquirePromotionAny())
	{
		pUnit->changeExperience(getProductionExperience(pUnit->getUnitType()) / ((bConscript) ? 2 : 1));
	}

	const int numNumPromotionInfos = GC.getNumPromotionInfos();
	for (int iI = 0; iI < numNumPromotionInfos; iI++)
	{
		PromotionTypes ePromotion = ((PromotionTypes)iI);

		if (isFreePromotion(ePromotion))
		{
			assignPromotionChecked(ePromotion, pUnit);
		}
	}

	const int numNumBuildingInfos = GC.getNumBuildingInfos();
	for (int iJ = 0; iJ < numNumBuildingInfos; iJ++)
	{
		BuildingTypes eBuilding = ((BuildingTypes)iJ);
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

		if (kBuilding.getNumFreePromoTypes() > 0)
		{
			if (getNumActiveBuilding(eBuilding) > 0)
			{
				assignPromotionsFromBuildingChecked(kBuilding, pUnit);
			}
		}
	}
}

UnitTypes CvCity::getProductionUnit() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNIT;
}


UnitAITypes CvCity::getProductionUnitAI() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitAITypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNITAI;
}


BuildingTypes CvCity::getProductionBuilding() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			break;

		case ORDER_CONSTRUCT:
			return ((BuildingTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_BUILDING;
}


ProjectTypes CvCity::getProductionProject() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
			break;

		case ORDER_CREATE:
			return ((ProjectTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_PROJECT;
}


ProcessTypes CvCity::getProductionProcess() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_LIST:
			break;

		case ORDER_MAINTAIN:
			return ((ProcessTypes)(pOrderNode->m_data.iData1));
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_PROCESS;
}


const wchar* CvCity::getProductionName() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getUnitInfo((UnitTypes) EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)).getDescription(getCivilizationType());
			break;

		case ORDER_CONSTRUCT:
			return GC.getBuildingInfo((BuildingTypes) pOrderNode->m_data.iData1).getDescription();
			break;

		case ORDER_CREATE:
			return GC.getProjectInfo((ProjectTypes) pOrderNode->m_data.iData1).getDescription();
			break;

		case ORDER_MAINTAIN:
			return GC.getProcessInfo((ProcessTypes) pOrderNode->m_data.iData1).getDescription();
			break;

		case ORDER_LIST:
			break; // It is never at the head of the list

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return L"";
}


int CvCity::getGeneralProductionTurnsLeft() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1), 0);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft((BuildingTypes)pOrderNode->m_data.iData1, 0);
			break;

		case ORDER_CREATE:
			return getProductionTurnsLeft((ProjectTypes)pOrderNode->m_data.iData1, 0);
			break;

		case ORDER_MAINTAIN:
		case ORDER_LIST:
			return 0;
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


const wchar* CvCity::getProductionNameKey() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getUnitInfo((UnitTypes) EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)).getTextKeyWide();
			break;

		case ORDER_CONSTRUCT:
			return GC.getBuildingInfo((BuildingTypes) pOrderNode->m_data.iData1).getTextKeyWide();
			break;

		case ORDER_CREATE:
			return GC.getProjectInfo((ProjectTypes) pOrderNode->m_data.iData1).getTextKeyWide();
			break;

		case ORDER_MAINTAIN:
			return GC.getProcessInfo((ProcessTypes) pOrderNode->m_data.iData1).getTextKeyWide();
			break;

		case ORDER_LIST:
			break; // It is never at the head of the list

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return L"";
}


bool CvCity::isFoodProduction() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return isFoodProduction((UnitTypes)(EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return false;
}


bool CvCity::isFoodProduction(UnitTypes eUnit) const
{
	if (GC.getUnitInfo(eUnit).isFoodProduction())
	{
		return true;
	}

	if (GET_PLAYER(getOwnerINLINE()).isMilitaryFoodProduction())
	{
		if (GC.getUnitInfo(eUnit).isMilitaryProduction())
		{
			return true;
		}
	}

	return false;
}


int CvCity::getFirstUnitOrder(UnitTypes eUnit) const
{
	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
		{
			if (EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1) == eUnit)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}


int CvCity::getFirstBuildingOrder(BuildingTypes eBuilding) const
{
	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT)
		{
			if (pOrderNode->m_data.iData1 == eBuilding)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}


int CvCity::getFirstProjectOrder(ProjectTypes eProject) const
{
	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_CREATE)
		{
			if (pOrderNode->m_data.iData1 == eProject)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}


int CvCity::getNumTrainUnitAI(UnitAITypes eUnitAI) const
{
	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
		{
			if (EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2) == eUnitAI)
			{
				iCount++;
			}
		}

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return iCount;
}


int CvCity::getProduction() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getUnitProduction((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
			return getBuildingProduction((BuildingTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CREATE:
			return getProjectProduction((ProjectTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


int CvCity::getProductionNeeded() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionNeeded((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
			return getProductionNeeded((BuildingTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CREATE:
			return getProductionNeeded((ProjectTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return MAX_INT;
}

int CvCity::getProductionNeeded(UnitTypes eUnit) const
{
	return GET_PLAYER(getOwnerINLINE()).getProductionNeeded(eUnit);
}

int CvCity::getProductionNeeded(BuildingTypes eBuilding) const
{
	int iProductionNeeded = GET_PLAYER(getOwnerINLINE()).getProductionNeeded(eBuilding);

	// Python cost modifier
	if (GC.getUSE_GET_BUILDING_COST_MOD_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getOwnerINLINE());	// Player ID
		argsList.add(getID());	// City ID
		argsList.add(eBuilding);	// Building ID
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "getBuildingCostMod", argsList.makeFunctionArgs(), &lResult);

		if (lResult > 1)
		{
			iProductionNeeded *= lResult;
			iProductionNeeded /= 100;
		}
	}

	return iProductionNeeded;
}

int CvCity::getProductionNeeded(ProjectTypes eProject) const
{
	return GET_PLAYER(getOwnerINLINE()).getProductionNeeded(eProject);
}

int CvCity::getProductionTurnsLeft() const
{
	return getQueueNodeProductionTurnsLeft(headOrderQueueNode());
}

int CvCity::getQueueNodeProductionTurnsLeft(CLLNode<OrderData>* pOrderNode, int iIndex) const
{
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft(((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)), iIndex);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft(((BuildingTypes)(pOrderNode->m_data.iData1)), iIndex);
			break;

		case ORDER_CREATE:
			return getProductionTurnsLeft(((ProjectTypes)(pOrderNode->m_data.iData1)), iIndex);
			break;

		case ORDER_MAINTAIN:
			break;

		case ORDER_LIST:
			return 0;
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return MAX_INT;
}

int CvCity::getProductionTurnsLeft(UnitTypes eUnit, int iNum) const
{
	int iProduction;
	int iFirstUnitOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstUnitOrder = getFirstUnitOrder(eUnit);

	if ((iFirstUnitOrder == -1) || (iFirstUnitOrder == iNum))
	{
		iProduction += getUnitProduction(eUnit);
	}

	iProductionNeeded = getProductionNeeded(eUnit);
	iProductionModifier = getProductionModifier(eUnit);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifference(iProductionNeeded, iProduction, iProductionModifier, isFoodProduction(eUnit), (iNum == 0)), getProductionDifference(iProductionNeeded, iProduction, iProductionModifier, isFoodProduction(eUnit), false));
}

int CvCity::getTotalProductionQueueTurnsLeft() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	int iResult = 0;
	int	iIndex = 0;

	while(pOrderNode != NULL)
	{
		iResult += getQueueNodeProductionTurnsLeft( pOrderNode, iIndex++ );

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return iResult;
}

int CvCity::numQueuedUnits(UnitAITypes eUnitAI, CvPlot* pDestPlot)
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	int iResult = 0;

	while(pOrderNode != NULL)
	{
		if ( pOrderNode->m_data.eOrderType == ORDER_TRAIN &&
			 INTERNAL_AUXILIARY_ORDER_IDATA(pOrderNode->m_data.iData1) == GC.getMapINLINE().plotNumINLINE(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()) &&
			 ((INTERNAL_AUXILIARY_ORDER_IDATA(pOrderNode->m_data.iData2) & 0xFF) == (eUnitAI & 0xFF)))
		{
			iResult++;
		}

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return iResult;
}

int CvCity::getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const
{
	int iProduction;
	int iFirstBuildingOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstBuildingOrder = getFirstBuildingOrder(eBuilding);

	if ((iFirstBuildingOrder == -1) || (iFirstBuildingOrder == iNum))
	{
		iProduction += getBuildingProduction(eBuilding);
	}

	iProductionNeeded = getProductionNeeded(eBuilding);

	iProductionModifier = getProductionModifier(eBuilding);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifference(iProductionNeeded, iProduction, iProductionModifier, false, (iNum == 0)), getProductionDifference(iProductionNeeded, iProduction, iProductionModifier, false, false));
}


int CvCity::getProductionTurnsLeft(ProjectTypes eProject, int iNum) const
{
	int iProduction;
	int iFirstProjectOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstProjectOrder = getFirstProjectOrder(eProject);

	if ((iFirstProjectOrder == -1) || (iFirstProjectOrder == iNum))
	{
		iProduction += getProjectProduction(eProject);
	}

	iProductionNeeded = getProductionNeeded(eProject);
	iProductionModifier = getProductionModifier(eProject);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifference(iProductionNeeded, iProduction, iProductionModifier, false, (iNum == 0)), getProductionDifference(iProductionNeeded, iProduction, iProductionModifier, false, false));
}


int CvCity::getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const
{
	int iProductionLeft;
	int iTurnsLeft;

	iProductionLeft = std::max(0, (iProductionNeeded - iProduction - iFirstProductionDifference));

	if (iProductionDifference == 0)
	{
		return iProductionLeft + 1;
	}

	iTurnsLeft = (iProductionLeft / iProductionDifference);

	if ((iTurnsLeft * iProductionDifference) < iProductionLeft)
	{
		iTurnsLeft++;
	}

	iTurnsLeft++;

	return std::max(1, iTurnsLeft);
}


void CvCity::setProduction(int iNewValue)
{
	int iAdaptedNewValue = std::max(0, iNewValue);
	if (isProductionUnit())
	{
		setUnitProduction(getProductionUnit(), iAdaptedNewValue);
	}
	else if (isProductionBuilding())
	{
		setBuildingProduction(getProductionBuilding(), iAdaptedNewValue);
	}
	else if (isProductionProject())
	{
		setProjectProduction(getProductionProject(), iAdaptedNewValue);
	}
}


void CvCity::changeProduction(int iChange)
{
	if (isProductionUnit())
	{
		changeUnitProduction(getProductionUnit(), iChange);
	}
	else if (isProductionBuilding())
	{
		changeBuildingProduction(getProductionBuilding(), iChange);
	}
	else if (isProductionProject())
	{
		changeProjectProduction(getProductionProject(), iChange);
	}
	else if (isProductionProcess())
	{
		CvProcessInfo& kProcess = GC.getProcessInfo(getProductionProcess());

		//	Add gold and espionage directly to player totals
		GET_PLAYER(getOwnerINLINE()).changeGold((kProcess.getProductionToCommerceModifier(COMMERCE_GOLD) * iChange)/100);
		GET_PLAYER(getOwnerINLINE()).doEspionageOneOffPoints((kProcess.getProductionToCommerceModifier(COMMERCE_ESPIONAGE) * iChange)/100);

		//	Research accrues to the team
		TechTypes eCurrentTech = GET_PLAYER(getOwnerINLINE()).getCurrentResearch();
		if (eCurrentTech != NO_TECH)
		{
			GET_TEAM(getTeam()).changeResearchProgress(eCurrentTech, 
													   (kProcess.getProductionToCommerceModifier(COMMERCE_RESEARCH) * iChange)/100,
													   getOwnerINLINE());
		}

		//	Culture to the city itself
		changeCulture(getOwnerINLINE(), (kProcess.getProductionToCommerceModifier(COMMERCE_CULTURE) * iChange)/100, false, false);
	}
}


int CvCity::getProductionModifier() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionModifier((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
			return getProductionModifier((BuildingTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CREATE:
			return getProductionModifier((ProjectTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


int CvCity::getProductionModifier(UnitTypes eUnit) const
{
	int iMultiplier = GET_PLAYER(getOwnerINLINE()).getProductionModifier(eUnit);

	iMultiplier += getUnitClassProductionModifier((UnitClassTypes)(GC.getUnitInfo(eUnit).getUnitClassType()));
	iMultiplier += GET_PLAYER(getOwnerINLINE()).getUnitClassProductionModifier((UnitClassTypes)(GC.getUnitInfo(eUnit).getUnitClassType()));
	if (!GC.getUnitInfo(eUnit).isNoNonTypeProdMods())
	{
		iMultiplier += getDomainProductionModifier((DomainTypes)(GC.getUnitInfo(eUnit).getDomainType()));
		if (GC.getUnitInfo(eUnit).getUnitCombatType() != NO_UNITCOMBAT)
		{
			iMultiplier += GET_PLAYER(getOwnerINLINE()).getUnitCombatProductionModifier((UnitCombatTypes)(GC.getUnitInfo(eUnit).getUnitCombatType()));

			//TB Combat Mod begin TB SubCombat Mod Begin
			iMultiplier += getUnitCombatProductionModifier((UnitCombatTypes)(GC.getUnitInfo(eUnit).getUnitCombatType()));
			for (int iI = 0; iI < GC.getUnitInfo(eUnit).getNumSubCombatTypes(); iI++)
			{
				UnitCombatTypes eSubCombatType = ((UnitCombatTypes)GC.getUnitInfo(eUnit).getSubCombatType(iI));
				iMultiplier += GET_PLAYER(getOwnerINLINE()).getUnitCombatProductionModifier(eSubCombatType);
				iMultiplier += getUnitCombatProductionModifier(eSubCombatType);
			}
			//TB SubCombat Mod endTB SubCombat Mod End
		}

		if (GC.getUnitInfo(eUnit).isMilitaryProduction())
		{
			iMultiplier += getMilitaryProductionModifier();
		}

		if (GET_PLAYER(getOwnerINLINE()).getStateReligion() != NO_RELIGION)
		{
			if (isHasReligion(GET_PLAYER(getOwnerINLINE()).getStateReligion()))
			{
				iMultiplier += GET_PLAYER(getOwnerINLINE()).getStateReligionUnitProductionModifier();
			}
		}
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMultiplier += GC.getUnitInfo(eUnit).getBonusProductionModifier(iI);
		}
	}

	return std::max(0, iMultiplier);
}


int CvCity::getProductionModifier(BuildingTypes eBuilding) const
{
	int iMultiplier = GET_PLAYER(getOwnerINLINE()).getProductionModifier(eBuilding);
	
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iMultiplier += getBuildingClassProductionModifier((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType()));
	
	iMultiplier += GET_PLAYER(getOwnerINLINE()).getBuildingClassProductionModifier((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType()));

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMultiplier += GC.getBuildingInfo(eBuilding).getBonusProductionModifier(iI);
		}
	}

	if (GET_PLAYER(getOwnerINLINE()).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(getOwnerINLINE()).getStateReligion()))
		{
			iMultiplier += GET_PLAYER(getOwnerINLINE()).getStateReligionBuildingProductionModifier();
		}
	}

	return std::max(0, iMultiplier);
}


int CvCity::getProductionModifier(ProjectTypes eProject) const
{
	int iMultiplier = GET_PLAYER(getOwnerINLINE()).getProductionModifier(eProject);

	if (GC.getProjectInfo(eProject).isSpaceship())
	{
		iMultiplier += getSpaceProductionModifier();
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMultiplier += GC.getProjectInfo(eProject).getBonusProductionModifier(iI);
		}
	}

	return std::max(0, iMultiplier);
}

/************************************************************************************************/
/* Afforess	Multiple Production Mod		 08/23/09                                            */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvCity::getOverflowProductionDifference() const
{
	return getProductionDifference(getProductionNeeded(), getProduction(), getProductionModifier(), false, true, false);
}
/*
int CvCity::getProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow) const
*/
int CvCity::getProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow, bool bYield) const
/************************************************************************************************/
/* Afforess	Multiple Production Mod       END                                                */
/************************************************************************************************/

{
	if (isDisorder())
	{
		return 0;
	}

	int iFoodProduction = ((bFoodProduction) ? std::max(0, (getYieldRate(YIELD_FOOD) - foodConsumption(true))) : 0);

	int iOverflow = ((bOverflow) ? (getOverflowProduction() + getFeatureProduction()) : 0);

/************************************************************************************************/
/* Afforess	Multiple Production Mod		 08/23/09                                            */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	//TB Traits begin
	int iYield = ((bYield) ? (getModifiedBaseYieldRate(YIELD_PRODUCTION)) : 0);
	//TB Traits end
/*
	return (((getBaseYieldRate(YIELD_PRODUCTION) + iOverflow) * getBaseYieldRateModifier(YIELD_PRODUCTION, iProductionModifier)) / 100 + iFoodProduction);
*/
	return (((iYield + iOverflow) * getBaseYieldRateModifier(YIELD_PRODUCTION, iProductionModifier)) / 100 + iFoodProduction);
/************************************************************************************************/
/* Afforess	Multiple Production Mod       END                                                */
/************************************************************************************************/

}


int CvCity::getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow) const
{
	return getProductionDifference(getProductionNeeded(), getProduction(), getProductionModifier(), (!bIgnoreFood && isFoodProduction()), bOverflow);
}


int CvCity::getExtraProductionDifference(int iExtra) const
{
	return getExtraProductionDifference(iExtra, getProductionModifier());
}

int CvCity::getExtraProductionDifference(int iExtra, UnitTypes eUnit) const
{
	return getExtraProductionDifference(iExtra, getProductionModifier(eUnit));
}

int CvCity::getExtraProductionDifference(int iExtra, BuildingTypes eBuilding) const
{
	return getExtraProductionDifference(iExtra, getProductionModifier(eBuilding));
}

int CvCity::getExtraProductionDifference(int iExtra, ProjectTypes eProject) const
{
	return getExtraProductionDifference(iExtra, getProductionModifier(eProject));
}

int CvCity::getExtraProductionDifference(int iExtra, int iModifier) const
{
	return ((iExtra * getBaseYieldRateModifier(YIELD_PRODUCTION, iModifier)) / 100);
}


bool CvCity::canHurry(HurryTypes eHurry, bool bTestVisible) const
{
	if (!(GET_PLAYER(getOwnerINLINE()).canHurry(eHurry)))
	{
		return false;
	}

	if (isDisorder())
	{
		return false;
	}

	if (getProduction() >= getProductionNeeded())
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (!isProductionUnit() && !isProductionBuilding())
		{
			return false;
		}

		if (GET_PLAYER(getOwnerINLINE()).getEffectiveGold() < hurryGold(eHurry))
		{
			return false;
		}

		if (maxHurryPopulation() < hurryPopulation(eHurry))
		{
			return false;
		}
	}

	return true;
}

bool CvCity::canHurryUnit(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const
{
	if (!(GET_PLAYER(getOwnerINLINE()).canHurry(eHurry)))
	{
		return false;
	}

	if (isDisorder())
	{
		return false;
	}

	if (getUnitProduction(eUnit) >= getProductionNeeded(eUnit))
	{
		return false;
	}

	int iHurryGold = getHurryGold(eHurry, getHurryCost(false, eUnit, bIgnoreNew));
	
	if (GET_PLAYER(getOwnerINLINE()).getEffectiveGold() < iHurryGold)
	{
		return false;
	}

	if (maxHurryPopulation() < getHurryPopulation(eHurry, getHurryCost(true, eUnit, bIgnoreNew)))
	{
		return false;
	}

	return true;
}

bool CvCity::canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	if (!(GET_PLAYER(getOwnerINLINE()).canHurry(eHurry)))
	{
		return false;
	}

	if (isDisorder())
	{
		return false;
	}

	if (getBuildingProduction(eBuilding) >= getProductionNeeded(eBuilding))
	{
		return false;
	}

	if (GET_PLAYER(getOwnerINLINE()).getEffectiveGold() < getHurryGold(eHurry, getHurryCost(false, eBuilding, bIgnoreNew)))
	{
		return false;
	}

	if (maxHurryPopulation() < getHurryPopulation(eHurry, getHurryCost(true, eBuilding, bIgnoreNew)))
	{
		return false;
	}

	return true;
}


void CvCity::hurry(HurryTypes eHurry)
{
	int iHurryGold = 0;
	int iHurryPopulation = 0;
	int iHurryAngerLength = 0;
	if (!canHurry(eHurry))
	{
		return;
	}

	bool bWhip = (GC.getHurryInfo(eHurry).getProductionPerPopulation() > 0);
	bool bBuy = (GC.getHurryInfo(eHurry).getGoldPerProduction() > 0);

	if (bBuy)
	{
		iHurryGold = hurryGold(eHurry);
		GET_PLAYER(getOwnerINLINE()).changeHurriedCount(1);
		GET_PLAYER(getOwnerINLINE()).changeGold(-(iHurryGold));
	}

	if (bWhip)
	{
		iHurryPopulation = hurryPopulation(eHurry);
		iHurryAngerLength = hurryAngerLength(eHurry);
		changePopulation(-(iHurryPopulation));
		changeHurryAngerTimer(iHurryAngerLength);
	}

	changeProduction(hurryProduction(eHurry));

	if( gCityLogLevel >= 2 )
	{
		CvWStringBuffer szBuffer;
		CvWString szString;
		if (isProductionUnit())
		{
			szString = GC.getUnitInfo(getProductionUnit()).getDescription(getCivilizationType());
		}
		else if (isProductionBuilding())
		{
			szString = GC.getBuildingInfo(getProductionBuilding()).getDescription();
		}
		else if (isProductionProject())
		{
			szString = GC.getProjectInfo(getProductionProject()).getDescription();
		}

		logBBAI("    City %S hurrying production of %S at cost of %d pop, %d gold, %d anger length", getName().GetCString(), szString.GetCString(), iHurryPopulation, iHurryGold, iHurryAngerLength );
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	// Python Event
	CvEventReporter::getInstance().cityHurry(this, eHurry);
}

// BUG - Hurry Assist - start
bool CvCity::hurryOverflow(HurryTypes eHurry, int* iProduction, int* iGold, bool bCountThisTurn) const
{
	if (!canHurry(eHurry))
	{
		return false;
	}

	if (GC.getHurryInfo(eHurry).getProductionPerPopulation() == 0)
	{
		*iProduction = 0;
		*iGold = 0;
		return true;
	}

	int iTotal, iCurrent, iModifier, iGoldPercent;

	if (isProductionUnit())
	{
		UnitTypes eUnit = getProductionUnit();
		FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned a valid unit type");
		iTotal = getProductionNeeded(eUnit);
		iCurrent = getUnitProduction(eUnit);
		iModifier = getProductionModifier(eUnit);
		iGoldPercent = GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT");
	}
	else if (isProductionBuilding())
	{
		BuildingTypes eBuilding = getProductionBuilding();
		FAssertMsg(eBuilding != NO_BUILDING, "eBuilding is expected to be assigned a valid building type");
		iTotal = getProductionNeeded(eBuilding);
		iCurrent = getBuildingProduction(eBuilding);
		iModifier = getProductionModifier(eBuilding);
		iGoldPercent = GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT");
	}
	else if (isProductionProject())
	{
		ProjectTypes eProject = getProductionProject();
		FAssertMsg(eProject != NO_PROJECT, "eProject is expected to be assigned a valid project type");
		iTotal = getProductionNeeded(eProject);
		iCurrent = getProjectProduction(eProject);
		iModifier = getProductionModifier(eProject);
		iGoldPercent = GC.getDefineINT("MAXED_PROJECT_GOLD_PERCENT");
	}
	else
	{
		return false;
	}

	int iHurry = hurryProduction(eHurry);
	int iOverflow = iCurrent + iHurry - iTotal;
	if (bCountThisTurn)
	{
		// include chops and previous overflow here
		iOverflow += getCurrentProductionDifference(false, true);
	}
	int iMaxOverflow = std::max(iTotal, getCurrentProductionDifference(false, false));
	int iLostProduction = std::max(0, iOverflow - iMaxOverflow);
	int iBaseModifier = getBaseYieldRateModifier(YIELD_PRODUCTION);
	int iTotalModifier = getBaseYieldRateModifier(YIELD_PRODUCTION, iModifier);

	iOverflow = std::min(iOverflow, iMaxOverflow);
	iLostProduction *= iBaseModifier;
	iLostProduction /= std::max(1, iTotalModifier);

	*iProduction = (iBaseModifier * iOverflow) / std::max(1, iTotalModifier);
	*iGold = ((iLostProduction * iGoldPercent) / 100);

	return true;
}
// BUG - Hurry Assist - end


UnitTypes CvCity::getConscriptUnit() const
{
	int iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);

		if (eLoopUnit != NO_UNIT)
		{
			if (canTrain(eLoopUnit))
			{
				int iValue = GC.getUnitInfo(eLoopUnit).getConscriptionValue();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestUnit = eLoopUnit;
				}
			}
		}
	}

	// Allow the player to determine the conscripted unit type
	CyArgsList argsList;
	argsList.add(getOwnerINLINE());	// pass in player
	long lConscriptUnit = -1;
	PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "getConscriptUnitType", argsList.makeFunctionArgs(), &lConscriptUnit);

	if (lConscriptUnit != -1)
	{
		eBestUnit = ((UnitTypes)lConscriptUnit);
	}

	return eBestUnit;
}


int CvCity::getConscriptPopulation() const
{
	UnitTypes eConscriptUnit = getConscriptUnit();

	if (eConscriptUnit == NO_UNIT)
	{
		return 0;
	}

	if (GC.getDefineINT("CONSCRIPT_POPULATION_PER_COST") == 0)
	{
		return 0;
	}

	return std::max(1, ((GC.getUnitInfo(eConscriptUnit).getProductionCost()) / GC.getDefineINT("CONSCRIPT_POPULATION_PER_COST")));
}


int CvCity::conscriptMinCityPopulation() const
{
	int iPopulation = GC.getDefineINT("CONSCRIPT_MIN_CITY_POPULATION");

	iPopulation += getConscriptPopulation();

	return iPopulation;
}


int CvCity::flatConscriptAngerLength() const
{
	int iAnger = GC.getDefineINT("CONSCRIPT_ANGER_DIVISOR");

	iAnger *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getHurryConscriptAngerPercent();
	iAnger /= 100;

	return std::max(1, iAnger);
}


//Team Project (6)
bool CvCity::canConscript(bool bOnCapture) const
{
//Team Project (6)
	if (isDisorder() && !bOnCapture)
	{
		return false;
	}

//Team Project (6)
	if (isDrafted() && !bOnCapture)
	{
		return false;
	}

	if (GET_PLAYER(getOwnerINLINE()).getConscriptCount() >= GET_PLAYER(getOwnerINLINE()).getMaxConscript())
	{
		return false;
	}

	if (getPopulation() <= getConscriptPopulation())
	{
		return false;
	}

	if (getPopulation() < conscriptMinCityPopulation())
	{
		return false;
	}

//Team Project (6)
	if (plot()->calculateTeamCulturePercent(getTeam()) < GC.getDefineINT("CONSCRIPT_MIN_CULTURE_PERCENT") && !bOnCapture)
	{
		return false;
	}

	if (getConscriptUnit() == NO_UNIT)
	{
		return false;
	}

	return true;
}

CvUnit* CvCity::initConscriptedUnit()
{
	UnitTypes eConscriptUnit = getConscriptUnit();
	if (NO_UNIT == eConscriptUnit)
	{
		return NULL;
	}

	UnitAITypes eCityAI = NO_UNITAI;
	if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eConscriptUnit, UNITAI_ATTACK, area()) > 0) 
	{ 
		eCityAI = UNITAI_ATTACK; 
	} 
	else if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eConscriptUnit, UNITAI_CITY_DEFENSE, area()) > 0) 
	{ 
		eCityAI = UNITAI_CITY_DEFENSE; 
	} 
	else if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eConscriptUnit, UNITAI_CITY_COUNTER, area()) > 0)
	{
		eCityAI = UNITAI_CITY_COUNTER;
	}
	else if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eConscriptUnit, UNITAI_CITY_SPECIAL, area()) > 0)
	{
		eCityAI = UNITAI_CITY_SPECIAL;
	}
	else
	{
		eCityAI = NO_UNITAI;
	}

	CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eConscriptUnit, getX_INLINE(), getY_INLINE(), eCityAI, NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));
	FAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

	if (NULL != pUnit)
	{
		addProductionExperience(pUnit, true);

		pUnit->setMoves(0);
	}

	return pUnit;
}


//Team Project (6)
void CvCity::conscript(bool bOnCapture)
{
//Team Project (6)
	if (!canConscript(bOnCapture))
	{
		return;
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
	int iPopChange = -(getConscriptPopulation());
	int iAngerLength = flatConscriptAngerLength();
	changePopulation(iPopChange);
	changeConscriptAngerTimer(iAngerLength);

	setDrafted(true);

//Team Project (6)
	if (!bOnCapture)
	{
		GET_PLAYER(getOwnerINLINE()).changeConscriptCount(1);
	}

	CvUnit* pUnit = initConscriptedUnit();
	FAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

	if (NULL != pUnit)
	{
		if (GC.getGameINLINE().getActivePlayer() == getOwnerINLINE() && isInViewport())
		{
			gDLL->getInterfaceIFace()->selectUnit(pUnit, true, false, true);
		}
		if( gCityLogLevel >= 2 )
		{
			logBBAI("      City %S does conscript of a %S at cost of %d pop, %d anger", getName().GetCString(), pUnit->getName().GetCString(), iPopChange, iAngerLength );
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	}
}


int CvCity::getBonusHealth(BonusTypes eBonus) const
{
	int iHealth = GC.getBonusInfo(eBonus).getHealth();

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			iHealth += getNumActiveBuilding((BuildingTypes)iI) * GC.getBuildingInfo((BuildingTypes) iI).getBonusHealthChanges(eBonus);
		}
	}

	return iHealth;
}


int CvCity::getBonusHappiness(BonusTypes eBonus) const
{
	int iHappiness = GC.getBonusInfo(eBonus).getHappiness();

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			iHappiness += getNumActiveBuilding((BuildingTypes)iI) * GC.getBuildingInfo((BuildingTypes) iI).getBonusHappinessChanges(eBonus);
		}
	}
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GET_PLAYER(getOwnerINLINE()).hasTrait((TraitTypes)iI) && hasBonus(eBonus))
		{
			for (int iJ = 0; iJ < GC.getTraitInfo((TraitTypes)iI).getNumBonusHappinessChanges(); iJ++)
			{
				if (GC.getTraitInfo((TraitTypes)iI).getBonusHappinessChange(iJ).eBonus == eBonus)
				{
					iHappiness += GC.getTraitInfo((TraitTypes)iI).getBonusHappinessChange(iJ).iModifier;
				}
			}
		}
	}
	return iHappiness;
}


int CvCity::getBonusPower(BonusTypes eBonus, bool bDirty) const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			if (GC.getBuildingInfo((BuildingTypes) iI).getPowerBonus() == eBonus)
			{
				if (GC.getBuildingInfo((BuildingTypes) iI).isDirtyPower() == bDirty)
				{
				//Team Project (5)
					if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
					{
						iCount += getNumActiveBuilding((BuildingTypes)iI);
					}
				}
			}
		}
	}

	return iCount;
}


int CvCity::getBonusYieldRateModifier(YieldTypes eIndex, BonusTypes eBonus) const
{
	int iModifier = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			iModifier += getNumActiveBuilding((BuildingTypes)iI) * GC.getBuildingInfo((BuildingTypes) iI).getBonusYieldModifier(eBonus, eIndex);
		}
	}

	return iModifier;
}

void CvCity::processBonus(BonusTypes eBonus, int iChange)
{
	PROFILE_FUNC();

	if ( m_paiBuildingReplaced == NULL )
	{
		calculateBuildingReplacements();
	}

	{
		int baseValue = GC.getBonusInfo(eBonus).getHealth();
		int iGoodValue = std::max(0, baseValue);
		int iBadValue = std::min(0, baseValue);

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			//Team Project (5)
			if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				int iValue = GC.getBuildingInfo((BuildingTypes)iI).getBonusHealthChanges(eBonus) * getNumActiveBuilding((BuildingTypes)iI);

				if (iValue >= 0)
				{
					iGoodValue += iValue;
				}
				else
				{
					iBadValue += iValue;
				}
			}
		}

		changeBonusGoodHealth(iGoodValue * iChange);
		changeBonusBadHealth(iBadValue * iChange);
	}

	{
		int baseValue = GC.getBonusInfo(eBonus).getHappiness();
		int iGoodValue = std::max(0, baseValue);
		int iBadValue = std::min(0, baseValue);

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
			//Team Project (5)
		{
			if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				int iValue = getNumActiveBuilding((BuildingTypes)iI) * GC.getBuildingInfo((BuildingTypes)iI).getBonusHappinessChanges(eBonus);

				if (iValue >= 0)
				{
					iGoodValue += iValue;
				}
				else
				{
					iBadValue += iValue;
				}
			}
		}
/************************************************************************************************/
/* Afforess	                  Start		 08/26/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GET_PLAYER(getOwnerINLINE()).hasTrait((TraitTypes)iI))
			{
				TraitTypes eTrait = (TraitTypes)iI;
				int iValue = 0;
				for (int iJ = 0; iJ < GC.getTraitInfo(eTrait).getNumBonusHappinessChanges(); iJ++)
				{
					if (GC.getTraitInfo(eTrait).getBonusHappinessChange(iJ).eBonus == eBonus)
					{
						iValue = GC.getTraitInfo((TraitTypes)iI).getBonusHappinessChange(iJ).iModifier;
					}
				}
				if (iValue >= 0)
				{
					iGoodValue += iValue;
				}
				else
				{
					iBadValue += iValue;
				}
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		changeBonusGoodHappiness(iGoodValue * iChange);
		changeBonusBadHappiness(iBadValue * iChange);
	}

	changePowerCount((getBonusPower(eBonus, true) * iChange), true);
	changePowerCount((getBonusPower(eBonus, false) * iChange), false);

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeBonusYieldRateModifier(((YieldTypes)iI), (getBonusYieldRateModifier(((YieldTypes)iI), eBonus) * iChange));	
	}
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeBonusCommerceRateModifier(((CommerceTypes)iI), (getBonusCommerceRateModifier(((CommerceTypes)iI), eBonus) * iChange));
		changeBonusCommercePercentChanges(((CommerceTypes)iI), (getBonusCommercePercentChanges(((CommerceTypes)iI), eBonus) * iChange));
	}
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			if (GC.getBuildingInfo((BuildingTypes)iI).getVicinityBonusYieldChanges(NO_BONUS, NO_YIELD) != 0 && getNumActiveBuilding((BuildingTypes)iI) > 0)
			{
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					updateYieldRate((BuildingTypes)iI, (YieldTypes)iJ, ((getBuildingYieldChange((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType()), (YieldTypes)iJ)) + (GC.getBuildingInfo((BuildingTypes)iI).getBonusYieldChanges(eBonus, iJ) * getNumActiveBuilding((BuildingTypes)iI) * iChange)));
				}
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
}


//Team Project (5)
void CvCity::processBuilding(BuildingTypes eBuilding, int iChange, bool bObsolete, bool bReplacingNow, bool bReligiouslyDisabling)
{
	PROFILE_FUNC();

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding); //Afforess
	CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	FAssert(iChange == 1 || iChange == -1);
//Team Project (5)
	if (!bReligiouslyDisabling)
	{
		if (kBuilding.isOrbitalInfrastructure())
		{
			GET_PLAYER(getOwnerINLINE()).noteOrbitalInfrastructureCountDirty();
		}
	}
	//TB: Makes it possible to use old built buildings to qualify for x building class built anywhere prerequisites so as to get around
	//Obsoletion and replacement issues.  However, this does still mean that the building needed to have BEEN built previously.
	//Moving here also fixes from other prereqs that may be eliminating or upsetting the count.
	GET_TEAM(getTeam()).changeBuildingClassCount((BuildingClassTypes)kBuilding.getBuildingClassType(), iChange);
	GET_PLAYER(getOwnerINLINE()).changeBuildingClassCount((BuildingClassTypes)kBuilding.getBuildingClassType(), iChange);

	//	We don't need to process the building effects in or out if it is
	//		* obsolete (unless GOING obsolete explicitly now)
	//		* has already been replaced, except in the middle of a modifier recalc where we might be processing it out
	//		  due to that replacement being re-detected
	if ((!(GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding)) && (m_paiBuildingReplaced == NULL || m_paiBuildingReplaced[eBuilding] == 0 || bReplacingNow || (m_recalcBuilding != MAX_INT && iChange < 0))) || bObsolete)
	{
		{
			PROFILE("CvCity::processBuilding.properties");

			if (iChange > 0)
			{
				if (!bReligiouslyDisabling)
				{
					CorporationTypes eCorporation = (CorporationTypes)kBuilding.getFoundsCorporation();
					if (NO_CORPORATION != eCorporation && !GC.getGameINLINE().isCorporationFounded(eCorporation))
					{
						setHeadquarters(eCorporation);
					}

					if (kBuilding.getFreeSpecialTech() != NO_TECH)
					{
						GET_TEAM(getTeam()).setHasTech(kBuilding.getFreeSpecialTech(), true, getOwner(), true, true);
					}
				}

				//	Property manipukators must be serialized since the propagation can mean we're
				//	updating properties on many entities (and granular locking produces too much
				//	overhead)
				CvProperties* pProp = kBuilding.getProperties();
				if (!pProp->isEmpty())
				{
					getProperties()->addProperties(pProp);
				}
				//GET_PLAYER(getOwnerINLINE()).getProperties()->addProperties(kBuilding.getProperties());
				pProp = kBuilding.getPropertiesAllCities();
				if (!pProp->isEmpty())
				{
					GET_TEAM(getTeam()).addPropertiesAllCities(pProp);
				}
			}
			else
			{
				if (bObsolete && !GC.getGame().isOption(GAMEOPTION_UNLIMITED_WONDERS))//When wonders obsolete, give back to the city their slot towards the wonder limit.
				{
					if (!GC.getBuildingClassInfo((BuildingClassTypes)kBuilding.getBuildingClassType()).isNoLimit())
					{
						if (GC.getBuildingClassInfo((BuildingClassTypes)kBuilding.getBuildingClassType()).getMaxGlobalInstances() > 0)
						{
							changeNumWorldWonders(-1);
						}
						if (GC.getBuildingClassInfo((BuildingClassTypes)kBuilding.getBuildingClassType()).getMaxPlayerInstances() > 0)
						{
							changeNumNationalWonders(-1);
						}
						if (GC.getBuildingClassInfo((BuildingClassTypes)kBuilding.getBuildingClassType()).getMaxTeamInstances() > 0)
						{
							changeNumTeamWonders(-1);
						}
					}
				}
				//	Property manipukators must be serialized since the propagation can mean we're
				//	updating properties on many entities (and granular locking produces too much
				//	overhead)
				CvProperties* pProp = kBuilding.getProperties();
				if (!pProp->isEmpty())
				{
					getProperties()->subtractProperties(kBuilding.getProperties());
				}
				//GET_PLAYER(getOwnerINLINE()).getProperties()->subtractProperties(kBuilding.getProperties());
				pProp = kBuilding.getPropertiesAllCities();
				if (!pProp->isEmpty())
				{
					GET_TEAM(getTeam()).subtractPropertiesAllCities(pProp);
				}
			}
		}

		{
			PROFILE("CvCity::processBuilding.part2");

			bool bChange = (iChange == 1);
			for (int iI = 0; iI < kBuilding.getNumFreeTraitTypes(); iI++)
			{
				TraitTypes eTrait = kBuilding.getFreeTraitType(iI).eTrait;
				if (GC.getTraitInfo(eTrait).isCivilizationTrait())
				{
					GET_PLAYER(getOwner()).setHasTrait(eTrait, bChange);
				}
			}

			if (kBuilding.getNoBonus() != NO_BONUS)
			{
				changeNoBonusCount(((BonusTypes)(kBuilding.getNoBonus())), iChange);
			}

			if (kBuilding.getFreeBonus() != NO_BONUS)
			{
				changeFreeBonus(((BonusTypes)(kBuilding.getFreeBonus())), (GC.getGameINLINE().getNumFreeBonuses(eBuilding) * iChange));
				clearVicinityBonusCache((BonusTypes)(kBuilding.getFreeBonus()));
				clearRawVicinityBonusCache((BonusTypes)(kBuilding.getFreeBonus()));
			}

			int iNum = kBuilding.getNumExtraFreeBonuses();
			for (int iI=0; iI<iNum; iI++)
			{
				changeFreeBonus(kBuilding.getExtraFreeBonus(iI), kBuilding.getExtraFreeBonusNum(iI) * iChange);
				clearVicinityBonusCache(kBuilding.getExtraFreeBonus(iI));
				clearRawVicinityBonusCache(kBuilding.getExtraFreeBonus(iI));
			}

			if (kBuilding.getFreePromotion() != NO_PROMOTION)
			{
				changeFreePromotionCount(((PromotionTypes)(kBuilding.getFreePromotion())), iChange);
			}

			if (kBuilding.getPropertySpawnProperty() != NO_PROPERTY && kBuilding.getPropertySpawnUnitClass() != NO_UNITCLASS)
			{
				FAssertMsg(
					GC.getUnitInfo(static_cast<UnitTypes>(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(static_cast<int>(kBuilding.getPropertySpawnUnitClass())))).isBlendIntoCity(),
					CvString::format("Building %s wants to add property spawner with unit class %s, but this unit doesn't have bBlendIntoCity enabled, which is a requirement",
						kBuilding.getType(), 
						GC.getUnitInfo(static_cast<UnitTypes>(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(static_cast<int>(kBuilding.getPropertySpawnUnitClass())))).getType()).c_str());

				changePropertySpawn(iChange, kBuilding.getPropertySpawnProperty(), kBuilding.getPropertySpawnUnitClass());
			}
			
			//TB Nukefix reset nuke validation
			if (kBuilding.isAllowsNukes())
			{//TB Nukefix (changed to GET_PLAYER(getOwnerINLINE() rather than GC.getGameINLINE)
				GET_PLAYER(getOwnerINLINE()).makeNukesValid(true);
			}

	/************************************************************************************************/
	/* Afforess  Meteorology Modmod addon                    10/13/09                               */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
			if (kBuilding.getFreePromotion_2() != NO_PROMOTION)
			{
				changeFreePromotionCount(((PromotionTypes)(kBuilding.getFreePromotion_2())), iChange);
			}
			
			if (kBuilding.getFreePromotion_3() != NO_PROMOTION)
			{
				changeFreePromotionCount(((PromotionTypes)(kBuilding.getFreePromotion_3())), iChange);
			}
	/************************************************************************************************/
	/* Afforess	                         END                                                     */
	/************************************************************************************************/	
			changeEspionageDefenseModifier(kBuilding.getEspionageDefenseModifier() * iChange);
	/************************************************************************************************/
	/* Afforess  Food Threshold Modifier   Start          10/16/09                   		         */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
			changePopulationgrowthratepercentage(kBuilding.getPopulationgrowthratepercentage(),(iChange==1));
	/************************************************************************************************/
	/* Afforess  Food Threshold Modifier                        END                  		         */
	/************************************************************************************************/
			if (!bReligiouslyDisabling)
			{
				changeGreatPeopleRateModifier(kBuilding.getGreatPeopleRateModifier() * iChange);
			}
			changeFreeExperience(kBuilding.getFreeExperience() * iChange);
			changeMaxFoodKeptPercent(kBuilding.getFoodKept(),(iChange==1));
			changeMaxAirlift(kBuilding.getAirlift() * iChange);
			changeAirModifier(kBuilding.getAirModifier() * iChange);
			changeAirUnitCapacity(kBuilding.getAirUnitCapacity() * iChange);
			changeNukeModifier(kBuilding.getNukeModifier() * iChange);
			changeFreeSpecialist(kBuilding.getFreeSpecialist() * iChange);
			changeMaintenanceModifier(kBuilding.getMaintenanceModifier() * iChange);
			changeWarWearinessModifier(kBuilding.getWarWearinessModifier() * iChange);
			changeHurryAngerModifier(kBuilding.getHurryAngerModifier() * iChange);
			changeHealRate(kBuilding.getHealRateChange() * iChange);
			changeQuarantinedCount(kBuilding.isQuarantine() ? iChange : 0);
			if (kBuilding.getHealth() > 0)
			{
				changeBuildingGoodHealth(kBuilding.getHealth() * iChange);
			}
			else
			{
				changeBuildingBadHealth(kBuilding.getHealth() * iChange);
			}
			if (kBuilding.getHappiness() > 0)
			{
				changeBuildingGoodHappiness(kBuilding.getHappiness() * iChange);
			}
			else
			{
				changeBuildingBadHappiness(kBuilding.getHappiness() * iChange);
			}
			if (kBuilding.getReligionType() != NO_RELIGION)
			{
				changeStateReligionHappiness(((ReligionTypes)(kBuilding.getReligionType())), (kBuilding.getStateReligionHappiness() * iChange));
			}
			changeMilitaryProductionModifier(kBuilding.getMilitaryProductionModifier() * iChange);
			changeSpaceProductionModifier(kBuilding.getSpaceProductionModifier() * iChange);
			changeExtraTradeRoutes(kBuilding.getTradeRoutes() * iChange);
			changeTradeRouteModifier(kBuilding.getTradeRouteModifier() * iChange);
			changeForeignTradeRouteModifier(kBuilding.getForeignTradeRouteModifier() * iChange);
			changePowerCount(((kBuilding.isPower()) ? iChange : 0), kBuilding.isDirtyPower());
			changeGovernmentCenterCount((kBuilding.isGovernmentCenter()) ? iChange : 0);
			changeNoUnhappinessCount((kBuilding.isNoUnhappiness()) ? iChange : 0);
			changeNoUnhealthyPopulationCount((kBuilding.isNoUnhealthyPopulation()) ? iChange : 0);
			changeBuildingOnlyHealthyCount((kBuilding.isBuildingOnlyHealthy()) ? iChange : 0);
			if (iChange == 1)
			{
				changePopulation(kBuilding.getPopulationChange() * iChange);
				if (canBuildingCostPopulation(eBuilding))
				{
					int iPopChange = (getPopulation() * kBuilding.getOneTimePopulationPercentLoss())/100;
					changePopulation(-iPopChange);
				}
			}
		}
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			PROFILE("CvCity::processBuilding.Yields");
			changeSeaPlotYield(((YieldTypes)iI), (kBuilding.getSeaPlotYieldChange(iI) * iChange));
			changeRiverPlotYield(((YieldTypes)iI), (kBuilding.getRiverPlotYieldChange(iI) * iChange));
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*			changeBaseYieldRate(((YieldTypes)iI), ((kBuilding.getYieldChange(iI) + getBuildingYieldChange((BuildingClassTypes)kBuilding.getBuildingClassType(), (YieldTypes)iI) + GET_TEAM(getTeam()).getBuildingYieldChange(eBuilding, (YieldTypes)iI))* iChange));
			changeYieldRateModifier(((YieldTypes)iI), ((kBuilding.getYieldModifier(iI) + GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, (YieldTypes)iI))* iChange));
*/			
			changeBaseYieldRate(((YieldTypes)iI), ((kBuilding.getYieldChange(iI) + getBuildingYieldChange((BuildingClassTypes)(kBuilding.getBuildingClassType()), (YieldTypes)iI) + GET_TEAM(getTeam()).getBuildingYieldChange(eBuilding, (YieldTypes)iI))* iChange));
			changeBaseYieldPerPopRate(((YieldTypes)iI), (kBuilding.getYieldPerPopChange(iI) * iChange));
			changeYieldRateModifier(((YieldTypes)iI), (kBuilding.getYieldModifier(iI) * iChange));			

			updateYieldModifierByBuilding(eBuilding, (YieldTypes)iI, GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, (YieldTypes)iI) * iChange);

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/				
			changePowerYieldRateModifier(((YieldTypes)iI), (kBuilding.getPowerYieldModifier(iI) * iChange));
		}

		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			PROFILE("CvCity::processBuilding.Commerces");
/************************************************************************************************/
/* Afforess	                  Start		 5/28/11                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			updateCommerceRateByBuilding(eBuilding, (CommerceTypes)iI, ((GET_TEAM(getTeam()).getBuildingCommerceChange(eBuilding, (CommerceTypes)iI) + GET_PLAYER(getOwnerINLINE()).getBuildingClassCommerceChange((BuildingClassTypes)kBuilding.getBuildingClassType(), (CommerceTypes)iI)) * iChange));

			//TB Debug note: For buildings, apparently the value is being independently calculated during the update routine.  It should NOT need to be processed into this Commerce Rate Modifier which apparently now only tracks Event commerce modifier adjustments.
			//Apparently I was wrong on the above statement...
			changeCommerceRateModifier(((CommerceTypes)iI), (kBuilding.getCommerceModifier(iI) * iChange));

			updateCommerceModifierByBuilding(eBuilding, (CommerceTypes)iI, (GET_TEAM(getTeam()).getBuildingCommerceModifier(eBuilding, (CommerceTypes)iI) + GET_PLAYER(getOwnerINLINE()).getBuildingCommerceModifier(eBuilding, (CommerceTypes)iI)) * iChange);
			changeMaxCommerceAttacks((CommerceTypes)iI, kBuilding.getCommerceAttacks(iI) * iChange);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/					

			changeCommerceHappinessPer(((CommerceTypes)iI), (kBuilding.getCommerceHappiness(iI) * iChange));
		}

//Team Project (5)
		if (!bReligiouslyDisabling)
		{
			for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				PROFILE("CvCity::processBuilding.Religions");
				changeReligionInfluence(((ReligionTypes)iI), (kBuilding.getReligionChange(iI) * iChange));
			}
		}

		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			PROFILE("CvCity::processBuilding.Specialists");
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			changeMaxSpecialistCount(((SpecialistTypes)iI), (kBuilding.getSpecialistCount(iI) + GET_TEAM(getTeam()).getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI)) * iChange);
			/* Was:
			changeMaxSpecialistCount(((SpecialistTypes)iI), kBuilding.getSpecialistCount(iI) * iChange);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/							

			changeFreeSpecialistCount(((SpecialistTypes)iI), kBuilding.getFreeSpecialistCount(iI) * iChange);
		}

		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			if (kBuilding.isMayDamageAttackingUnitCombatType(iI))
			{
				changeDamageAttackingUnitCombatCount(((UnitCombatTypes)iI), iChange);
			}
		}

		if (kBuilding.getNumHealUnitCombatTypes() > 0)
		{
			for (int iI = 0; iI < kBuilding.getNumHealUnitCombatTypes(); iI++)
			{
				changeHealUnitCombatTypeVolume((UnitCombatTypes)kBuilding.getHealUnitCombatType(iI).eUnitCombat, kBuilding.getHealUnitCombatType(iI).iHeal * iChange);
			}
		}


		{
			PROFILE("CvCity::processBuilding.Part3");

/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		int iTechBuildingHealth = GET_TEAM(getTeam()).getTechExtraBuildingHealth(eBuilding);
		if (iTechBuildingHealth > 0)
		{
			changeBuildingGoodHealth(iTechBuildingHealth * iChange);
		}
		else if (iTechBuildingHealth < 0)
		{
			changeBuildingBadHealth(iTechBuildingHealth * iChange);
		}

		int iTechBuildingHappiness = GET_TEAM(getTeam()).getTechExtraBuildingHappiness(eBuilding);
		if (iTechBuildingHappiness > 0)
		{
			changeBuildingGoodHappiness(iTechBuildingHappiness * iChange);
		}
		else if (iTechBuildingHappiness < 0)
		{
			changeBuildingGoodHappiness(iTechBuildingHappiness * iChange);
		}
		
		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			changeUnitCombatExtraStrength((UnitCombatTypes)iI, kBuilding.getUnitCombatExtraStrength(iI) * iChange);
		}

//Team Project (5)
		if (!bReligiouslyDisabling)
		{		
			changeInvasionChance(kBuilding.getInvasionChance() * iChange);
			changeLineOfSight(kBuilding.getLineOfSight() * iChange);
		}
		changeAdjacentDamagePercent(kBuilding.getAdjacentDamagePercent() * iChange);
		changeNumUnitFullHeal(kBuilding.getNumUnitFullHeal() * iChange);
		changeNumPopulationEmployed(kBuilding.getNumPopulationEmployed() * iChange);
		changeHealthPercentPerPopulation(kBuilding.getHealthPercentPerPopulation() * iChange);
		changeHappinessPercentPerPopulation(kBuilding.getHappinessPercentPerPopulation() * iChange);
		//TB Combat Mods (Buildings) begin
		for (int iI = 0; iI < kBuilding.getNumAidRateChanges(); iI++)
		{
			PropertyTypes eProperty = kBuilding.getAidRateChange(iI).ePropertyType;
			changeAidRate(eProperty, kBuilding.getAidRateChange(iI).iChange * iChange);
		}
		changeTotalFrontSupportPercentModifier(kBuilding.getFrontSupportPercentModifier() * iChange);
		changeTotalShortRangeSupportPercentModifier(kBuilding.getShortRangeSupportPercentModifier() * iChange);
		changeTotalMediumRangeSupportPercentModifier(kBuilding.getMediumRangeSupportPercentModifier() * iChange);
		changeTotalLongRangeSupportPercentModifier(kBuilding.getLongRangeSupportPercentModifier() * iChange);
		changeTotalFlankSupportPercentModifier(kBuilding.getFlankSupportPercentModifier() * iChange);
		}

		if (kBuilding.getPromotionLineType() != NO_PROMOTIONLINE && GC.getPromotionLineInfo(kBuilding.getPromotionLineType()).isAffliction())
		{
			changeAfflictionTypeCount(kBuilding.getPromotionLineType(), iChange);
		}

		for (int iI = 0; iI < kBuilding.getNumBonusAidModifiers(); iI++)
		{
			BonusTypes eBonus = kBuilding.getBonusAidModifier(iI).eBonusType;
			PropertyTypes ePropertyType = kBuilding.getBonusAidModifier(iI).ePropertyType;
			changeExtraBonusAidModifier(eBonus, ePropertyType, kBuilding.getBonusAidModifier(iI).iModifier);
		}

		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			PROFILE("CvCity::processBuilding.PromotionLines");
			PromotionLineTypes eAfflictionLine = ((PromotionLineTypes)iI);
			changeExtraAfflictionOutbreakLevelChange(eAfflictionLine, kBuilding.getAfflictionOutbreakLevelChange(iI) * iChange);
			
			if ((iChange == -1) && kBuilding.getPromotionLineType() == eAfflictionLine)
			{
				changeAfflictionToleranceChange(eAfflictionLine, GC.getPromotionLineInfo(eAfflictionLine).getToleranceBuildup());
				if (!hasAfflictionType(eAfflictionLine))
				{
					setCurrentOvercomeChange(eAfflictionLine, 0);
				}
			}
		}

		//int iNum = kBuilding.getNumFreePromoTypes();
		//for (iI=0; iI<iNum; iI++)
		//{
		//	PROFILE("CvCity::processBuilding.FreePromotions");
		//	PromotionTypes eFreePromo = (PromotionTypes)kBuilding.getFreePromoType(iI);
		//	if (eFreePromo != NO_PROMOTION)
		//	{
		//		changeFreePromotionCount(eFreePromo, iChange);
		//	}
		//}

		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			PROFILE("CvCity::processBuilding.CombatInfos");
			changeUnitCombatProductionModifier(((UnitCombatTypes)iI), kBuilding.getUnitCombatProdModifier(iI) * iChange);
			if (!bReligiouslyDisabling)
			{		
				changeUnitCombatRepelModifierTotal(((UnitCombatTypes)iI), kBuilding.getUnitCombatRepelModifier(iI) * iChange);
				//TB Defense Mod
				changeUnitCombatRepelAgainstModifierTotal(((UnitCombatTypes)iI), kBuilding.getUnitCombatRepelAgainstModifier(iI) * iChange);
				changeUnitCombatDefenseAgainstModifierTotal(((UnitCombatTypes)iI), kBuilding.getUnitCombatDefenseAgainstModifier(iI) * iChange);
				//
			}
			if (kBuilding.getUnitCombatOngoingTrainingDuration(iI) > 0)
			{
				if (iChange == 1  && (kBuilding.getUnitCombatOngoingTrainingDuration(iI) < getUnitCombatOngoingTrainingTimeIncrement((UnitCombatTypes)iI)))
				{
					setUnitCombatOngoingTrainingTimeIncrement(((UnitCombatTypes)iI), kBuilding.getUnitCombatOngoingTrainingDuration(iI));
				}
				if (iChange == -1  && (kBuilding.getUnitCombatOngoingTrainingDuration(iI) == getUnitCombatOngoingTrainingTimeIncrement((UnitCombatTypes)iI)))
				{
					int itrain = 0;
					int besttrainer = 0;
					BuildingTypes BestBuilding;
					for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
					{
						if (GC.getBuildingInfo((BuildingTypes)iJ).getUnitCombatOngoingTrainingDuration(iI) > 0)
						{
							BuildingTypes consideredBuilding = ((BuildingTypes)iJ);
							if (getNumActiveBuilding(consideredBuilding) > 0 && (consideredBuilding != ((BuildingTypes)iI)))
							{
								itrain = GC.getBuildingInfo(consideredBuilding).getUnitCombatOngoingTrainingDuration(iI);
								if (itrain < besttrainer)
								{
									besttrainer = itrain;
									BestBuilding = consideredBuilding;
								}
							}
						}
					}
					if (BestBuilding != NULL)
					{
						setUnitCombatOngoingTrainingTimeIncrement(((UnitCombatTypes)iI), GC.getBuildingInfo(BestBuilding).getUnitCombatOngoingTrainingDuration(iI));
					}
					else
					{
						setUnitCombatOngoingTrainingTimeIncrement(((UnitCombatTypes)iI), 0);
					}
				}
			}								
		}
		//TB Combat Mods (Buildings) end
		//TB Building Tags
//Team Project (3)
		changeExtraLocalCaptureProbabilityModifier(kBuilding.getLocalCaptureProbabilityModifier() * iChange);
		changeExtraLocalCaptureResistanceModifier(kBuilding.getLocalCaptureResistanceModifier() * iChange);
		changeExtraInsidiousness(kBuilding.getInsidiousness() * iChange);
		changeExtraInvestigation(kBuilding.getInvestigation() * iChange);
		//TB Defense Mod
		if (!bReligiouslyDisabling)
		{		
			changeExtraLocalDynamicDefense(kBuilding.getLocalDynamicDefense() * iChange);
			changeExtraRiverDefensePenalty(kBuilding.getRiverDefensePenalty() * iChange);
			changeExtraLocalRepel(kBuilding.getLocalRepel() * iChange);
			changeExtraMinDefense(kBuilding.getMinDefense() * iChange);
			if (kBuilding.getBuildingDefenseRecoverySpeedModifier()>0 && kBuilding.getDefenseModifier()>0)
			{
				changeExtraBuildingDefenseRecoverySpeedModifier(kBuilding.getBuildingDefenseRecoverySpeedModifier() * iChange);
				changeModifiedBuildingDefenseRecoverySpeedCap(kBuilding.getDefenseModifier() * iChange);
			}
			changeExtraCityDefenseRecoverySpeedModifier(kBuilding.getCityDefenseRecoverySpeedModifier() * iChange);
		}
		//
//Team Project (1)
		// TODO reform loop to iterate on the mappings
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			changeTechHappiness(((TechTypes)iI), kBuilding.getTechHappinessType((TechTypes)iI) * iChange);
			changeTechHealth(((TechTypes)iI), kBuilding.getTechHealthType((TechTypes)iI) * iChange);
		}
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			for (int iJ = 0; iJ < NUM_YIELD_TYPES ; iJ++)
			{
				if (kBuilding.getLocalSpecialistYieldChange(((SpecialistTypes)iI),((YieldTypes)iJ)) != 0)
				{
					changeLocalSpecialistExtraYield(((SpecialistTypes)iI),((YieldTypes)iJ), kBuilding.getLocalSpecialistYieldChange(((SpecialistTypes)iI),((YieldTypes)iJ)) * iChange);
				}
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES ; iJ++)
			{
				if (kBuilding.getLocalSpecialistCommerceChange(((SpecialistTypes)iI),((CommerceTypes)iJ)) != 0)
				{
					changeLocalSpecialistExtraCommerce(((SpecialistTypes)iI),((CommerceTypes)iJ), kBuilding.getLocalSpecialistCommerceChange(((SpecialistTypes)iI),((CommerceTypes)iJ)) * iChange);
				}
			}
		}

		{
			PROFILE("CvCity::processBuilding.Part4");

		int iMinBuildingDefenseLevel = kBuilding.getNoEntryDefenseLevel();

		if (!GC.getGameINLINE().isOption(GAMEOPTION_REALISTIC_SIEGE))
		{
			iMinBuildingDefenseLevel = 0;
		}

		if (iMinBuildingDefenseLevel > 0)
		{
			int iCurrentMinDefenseLevel = getMinimumDefenseLevel();

			if ( iChange > 0 )
			{
				if ( iMinBuildingDefenseLevel > iCurrentMinDefenseLevel )
				{
					setMinimumDefenseLevel(iMinBuildingDefenseLevel);
				}
			}
			else
			{
				if ( iMinBuildingDefenseLevel == iCurrentMinDefenseLevel )
				{
					int iNewMinDefenseLevel = 0;

					for( int iJ = 0; iJ < GC.getNumBuildingInfos(); ++iJ )
					{
						if ( getNumActiveBuilding((BuildingTypes)iJ) > 0 )
						{
							int iLevel = GC.getBuildingInfo((BuildingTypes)iJ).getNoEntryDefenseLevel();

							if ( iLevel > iNewMinDefenseLevel )
							{
								iNewMinDefenseLevel = iLevel;
							}
						}
					}

					setMinimumDefenseLevel(iNewMinDefenseLevel);
				}
			}
		}
		
		if ( kBuilding.isZoneOfControl() )
		{
			changeZoCCount(iChange);
		}

//Team Project (5)
		if (!bReligiouslyDisabling)
		{
			if (kBuilding.isProtectedCulture())
			{
				changeProtectedCultureCount(iChange > 0 ? 1 : -1 );
			}
			if (kBuilding.getWorkableRadius() > 0)
			{
				setWorkableRadiusOverride(iChange > 0 ? kBuilding.getWorkableRadius() : 0);
			}
			if (kBuilding.isProvidesFreshWater())
			{
				changeFreshWater(iChange);
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
		}
		for (int iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
		{
			PROFILE("CvCity::processBuilding.Improvements");
			changeImprovementFreeSpecialists((ImprovementTypes)iI, kBuilding.getImprovementFreeSpecialist(iI) * iChange);
		}

		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlotGroup::reset");
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			PROFILE("CvCity::processBuilding.Bonuses2");
			if (hasBonus((BonusTypes)iI))
			{
				if (kBuilding.getBonusHealthChanges(iI) > 0)
				{
					changeBonusGoodHealth(kBuilding.getBonusHealthChanges(iI) * iChange);
				}
				else
				{
					changeBonusBadHealth(kBuilding.getBonusHealthChanges(iI) * iChange);
				}
				if (kBuilding.getBonusHappinessChanges(iI) > 0)
				{
					changeBonusGoodHappiness(kBuilding.getBonusHappinessChanges(iI) * iChange);
				}
				else
				{
					changeBonusBadHappiness(kBuilding.getBonusHappinessChanges(iI) * iChange);
				}

				if (kBuilding.getPowerBonus() == iI)
				{
					changePowerCount(iChange, kBuilding.isDirtyPower());
				}

				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					changeBonusYieldRateModifier(((YieldTypes)iJ), (kBuilding.getBonusYieldModifier(iI, iJ) * iChange));
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
					int iBonusChange = kBuilding.getBonusYieldChanges(iI, iJ);
					if (hasVicinityBonus((BonusTypes)iI))
					{
						iBonusChange += kBuilding.getVicinityBonusYieldChanges(iI, iJ);
					}
					updateYieldRate(eBuilding, ((YieldTypes)iJ), ((getBuildingYieldChange((BuildingClassTypes)(kBuilding.getBuildingClassType()), (YieldTypes)iJ)) + (iBonusChange * iChange)));
				}
				for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
				{
					changeBonusCommerceRateModifier(((CommerceTypes)iJ), (kBuilding.getBonusCommerceModifier(iI, iJ) * iChange));
					changeBonusCommercePercentChanges(((CommerceTypes)iJ), (kBuilding.getBonusCommercePercentChanges(iI, iJ) * iChange));
				}
				
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/		
			}
		}

		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			PROFILE("CvCity::processBuilding.UnitCombatFreeExp");
			changeUnitCombatFreeExperience(((UnitCombatTypes)iI), kBuilding.getUnitCombatFreeExperience(iI) * iChange);
		}

		for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
		{
			PROFILE("CvCity::processBuilding.Domains");
			changeDomainFreeExperience(((DomainTypes)iI), kBuilding.getDomainFreeExperience(iI) * iChange);
			changeDomainProductionModifier(((DomainTypes)iI), kBuilding.getDomainProductionModifier(iI) * iChange);
		}
/************************************************************************************************/
/* Afforess           ProductionModifier           10/13/09                                     */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			PROFILE("CvCity::processBuilding.UniClasses");
			changeUnitClassProductionModifier(((UnitClassTypes)iI), kBuilding.getUnitClassProductionModifier(iI) * iChange);
		}
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			PROFILE("CvCity::processBuilding.BuildingClasses");
			changeBuildingClassProductionModifier(((BuildingClassTypes)iI), kBuilding.getBuildingClassProductionModifier(iI) * iChange);

			for(int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; iCommerce++ )
			{
				int iCommerceChange = kBuilding.getGlobalBuildingCommerceChange(iI, iCommerce);

				if ( iCommerceChange != 0 )
				{
					GET_PLAYER(getOwnerINLINE()).changeBuildingClassCommerceChange((BuildingClassTypes)iI, (CommerceTypes)iCommerce, iCommerceChange * iChange);
				}
			}
		}
/*		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (kBuilding.getImprovementYieldChanges(iI, iJ) * iChange));
			}
		}
/************************************************************************************************/
/* Afforess	                         END                                                     */
/************************************************************************************************/	
		{
			PROFILE("CvCity::processBuilding.Part5");
			updateExtraBuildingHappiness();
			updateExtraBuildingHealth();

			GET_PLAYER(getOwnerINLINE()).changeAssets(kBuilding.getAssetValue() * iChange);

			area()->changePower(getOwnerINLINE(), (kBuilding.getPowerValue()/100 * iChange));
			GET_PLAYER(getOwnerINLINE()).changePower(kBuilding.getPowerValue() * iChange);

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
				{
					if (kBuilding.isTeamShare() || (iI == getOwnerINLINE()))
					{
						if ( GET_PLAYER((PlayerTypes)iI).isAlive() )
						{
//Team Project (5)
							GET_PLAYER((PlayerTypes)iI).processBuilding(eBuilding, iChange, area(), bReligiouslyDisabling);
						}
					}
				}
			}
		}

//Team Project (5)
		GET_TEAM(getTeam()).processBuilding(eBuilding, iChange, bReligiouslyDisabling);
		
		if (!bReligiouslyDisabling)
		{
			GC.getGameINLINE().processBuilding(eBuilding, iChange);
		}		
	}

	if (!bObsolete)
	{
		PROFILE("CvCity::processBuilding.NotObsolete");
		//TB DEFENSEBUG:  The following building defense line is allowing buildings that are replaced to continue to function!  
		//We can only assume this entire section therefore gets around replaced buildings, particularly after a recalc.
//Team Project (5)
		//Note: this whole section was pretty well qualified to be ignored on religious disabling.


		if (!bReligiouslyDisabling)
		{
			changeBuildingDefense(kBuilding.getDefenseModifier() * iChange);
			changeBuildingBombardDefense(kBuilding.getBombardDefenseModifier() * iChange);

			changeBaseGreatPeopleRate(kBuilding.getGreatPeopleRateChange() * iChange);

			if (kBuilding.getGreatPeopleUnitClass() != NO_UNITCLASS)
			{
				UnitTypes eGreatPeopleUnit = ((UnitTypes)(kCivilizationInfo.getCivilizationUnits(kBuilding.getGreatPeopleUnitClass())));

				if (eGreatPeopleUnit != NO_UNIT)
				{
					changeGreatPeopleUnitRate(eGreatPeopleUnit, kBuilding.getGreatPeopleRateChange() * iChange);
				}
			}

			SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes)kBuilding.getSpecialBuildingType();
			if (eSpecialBuilding != NO_SPECIALBUILDING)
			{
				GET_PLAYER(getOwnerINLINE()).changeBuildingGroupCount(eSpecialBuilding, iChange);
			}

			GET_PLAYER(getOwnerINLINE()).changeWondersScore(getWonderScore((BuildingClassTypes)(kBuilding.getBuildingClassType())) * iChange);

			for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				if (hasBonus((BonusTypes)iI))
				{
					if (kBuilding.getBonusDefenseChanges(iI) != 0)
					{
						changeBonusDefenseChanges((BonusTypes)iI, kBuilding.getBonusDefenseChanges(iI) * iChange);
					}
				}
			}
		}	
	}
	
	changeBuildingReplacementCount(eBuilding, (iChange > 0));

	setMaintenanceDirty(true);	//	Always assume a chnage in buildings can change maintenance
	updateBuildingCommerce();

	m_buildingSourcedPropertyCache.clear();

	//	New or removed buildings can affect the assessment of the best plot builds
	AI_markBestBuildValuesStale();

//Team Project (5)
	if (!bReligiouslyDisabling && GC.getGameINLINE().isOption(GAMEOPTION_RELIGIOUS_DISABLING))
	{
		checkReligiousDisabling(eBuilding);
	}

	setLayoutDirty(true);
}


void CvCity::processProcess(ProcessTypes eProcess, int iChange)
{
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeProductionToCommerceModifier(((CommerceTypes)iI), (GC.getProcessInfo(eProcess).getProductionToCommerceModifier(iI) * iChange));
	}
}


void CvCity::processSpecialist(SpecialistTypes eSpecialist, int iChange)
{
	if (GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitClass() != NO_UNITCLASS)
	{
		UnitTypes eGreatPeopleUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitClass())));

		if (eGreatPeopleUnit != NO_UNIT)
		{
			changeGreatPeopleUnitRate(eGreatPeopleUnit, GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange() * iChange);
		}
	}

	changeBaseGreatPeopleRate(GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange() * iChange);

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
/************************************************************************************************/
/* Afforess	                  Start		 07/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
		changeBaseYieldRate(((YieldTypes)iI), (GC.getSpecialistInfo(eSpecialist).getYieldChange(iI) * iChange));
*/
		changeBaseYieldRate(((YieldTypes)iI), ((GC.getSpecialistInfo(eSpecialist).getYieldChange(iI) + (GET_PLAYER(getOwnerINLINE()).getSpecialistYieldPercentChanges(eSpecialist, (YieldTypes)iI) / 100)) * iChange));
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}

	int iCommerceChangeTotal = 0;
	int iCommerceChangeModifierTotal = 0;
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		//changeSpecialistCommerce(((CommerceTypes)iI), (GC.getSpecialistInfo(eSpecialist).getCommerceChange(iI) * iChange));
		//TB Adjustment note: this makes the math a bit more accurate to the intention.  However, in this and the previous version
		//there is a potential for small mathematical error that is easily righted via a recalc.  IF a Player's SpecialistCommercePercentChanges adjusts since
		//the specialist was initially processed in and the specialist is then processed out, you will get an incorrect removal because the
		//commerce the specialist added was not updated when the Player's percent modifier to that value changed.  Again, small error and easily righted via a recalc.
		//If this proves to become a bigger issue than I think it will, we'll need to call a mini-recalculation on this value in each city for the player
		//every time an adjustment to SpecialistCommercePercentChanges takes place.  That would be the patient and correct way to address this.
		if (GC.getSpecialistInfo(eSpecialist).getCommerceChange(iI) != 0)
		{
			iCommerceChangeTotal += (100 * GC.getSpecialistInfo(eSpecialist).getCommerceChange(iI));
			iCommerceChangeModifierTotal += (iCommerceChangeTotal * GET_PLAYER(getOwnerINLINE()).getSpecialistCommercePercentChanges(eSpecialist, (CommerceTypes)iI))/100;
			iCommerceChangeTotal += iCommerceChangeModifierTotal;
			changeSpecialistCommerceTimes100(((CommerceTypes)iI), iCommerceChangeTotal * iChange);
		}
		//Without reanalyzing, it looks like the above will omit Specialist Extra Commerce values from % modifiers to commerce percent changes?  hmm...  I think something may be fishy here.
	}

	updateExtraSpecialistYield();
	updateExtraSpecialistCommerce();
//Team Project (1)
	updateTechHappinessandHealth();

/************************************************************************************************/
/* Specialists Enhancements, by Supercheese 10/9/09                                                   */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getSpecialistInfo(eSpecialist).getHealthPercent() > 0)
	{
		changeSpecialistGoodHealth(GC.getSpecialistInfo(eSpecialist).getHealthPercent() * iChange);
	}
	else
	{
		changeSpecialistBadHealth(GC.getSpecialistInfo(eSpecialist).getHealthPercent() * iChange);
	}
	if (GC.getSpecialistInfo(eSpecialist).getHappinessPercent() > 0)
	{
		changeSpecialistHappiness(GC.getSpecialistInfo(eSpecialist).getHappinessPercent() * iChange);
	}
	else
	{
		changeSpecialistUnhappiness(GC.getSpecialistInfo(eSpecialist).getHappinessPercent() * iChange);
	}
/************************************************************************************************/
/* Specialists Enhancements                          END                                              */
/************************************************************************************************/

	changeSpecialistFreeExperience(GC.getSpecialistInfo(eSpecialist).getExperience() * iChange);

	changeSpecialistInsidiousness(GC.getSpecialistInfo(eSpecialist).getInsidiousness() * iChange);
	changeSpecialistInvestigation(GC.getSpecialistInfo(eSpecialist).getInvestigation() * iChange);

	for (int iI = 0; iI < GC.getSpecialistInfo(eSpecialist).getNumUnitCombatExperienceTypes(); iI++)
	{
		if (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI, false).eUnitCombat != NO_UNITCOMBAT)
		{
			changeUnitCombatFreeExperience(GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI, false).eUnitCombat, GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI, false).iModifier * iChange);
		}
	}

}


HandicapTypes CvCity::getHandicapType() const
{
	return GET_PLAYER(getOwnerINLINE()).getHandicapType();
}


CivilizationTypes CvCity::getCivilizationType() const
{
/************************************************************************************************/
/* Afforess	                  Start		 01/14/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (!GC.getGameINLINE().isOption(GAMEOPTION_ASSIMILATION) || m_iCiv == NO_CIVILIZATION)
	{
		return GET_PLAYER(getOwnerINLINE()).getCivilizationType();
	}
	return (CivilizationTypes)m_iCiv;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


LeaderHeadTypes CvCity::getPersonalityType() const
{
	return GET_PLAYER(getOwnerINLINE()).getPersonalityType();
}


ArtStyleTypes CvCity::getArtStyleType() const
{
/************************************************************************************************/
/* Afforess	                  Start		 07/12/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	 if (GC.getGameINLINE().isOption(GAMEOPTION_ASSIMILATION))
	 {
		if (getOriginalOwner() != NO_PLAYER)
		{
			return GET_PLAYER(getOriginalOwner()).getArtStyleType();
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return GET_PLAYER(getOwnerINLINE()).getArtStyleType();
}

CitySizeTypes CvCity::getCitySizeType() const
{
	return ((CitySizeTypes)(range((getPopulation() / 7), 0, (NUM_CITYSIZE_TYPES - 1))));
}

const CvArtInfoBuilding* CvCity::getBuildingArtInfo(BuildingTypes eBuilding) const
{
	return GC.getBuildingInfo(eBuilding).getArtInfo();
}

float CvCity::getBuildingVisibilityPriority(BuildingTypes eBuilding) const
{
	return GC.getBuildingInfo(eBuilding).getVisibilityPriority();
}

bool CvCity::hasTrait(TraitTypes eTrait) const
{
	return GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait);
}

bool CvCity::isBarbarian() const
{
	return GET_PLAYER(getOwnerINLINE()).isBarbarian();
}

bool CvCity::isNPC() const
{
	return GET_PLAYER(getOwnerINLINE()).isNPC();
}

bool CvCity::isHominid() const
{
	return GET_PLAYER(getOwnerINLINE()).isHominid();
}

bool CvCity::isHuman() const
{
	return GET_PLAYER(getOwnerINLINE()).isHuman();
}

bool CvCity::isVisible(TeamTypes eTeam, bool bDebug) const
{
	return plot()->isVisible(eTeam, bDebug);
}

bool CvCity::isCapital() const
{
	return (GET_PLAYER(getOwnerINLINE()).getCapitalCity() == this);
}

bool CvCity::isCoastal(int iMinWaterSize) const
{
	return plot()->isCoastalLand(iMinWaterSize);
}

bool CvCity::isDisorder() const
{
	return (isOccupation() || GET_PLAYER(getOwnerINLINE()).isAnarchy());
}

bool CvCity::isHolyCity(ReligionTypes eIndex) const
{
	if (eIndex == NO_RELIGION)
	{
		return false;
	}
	return (GC.getGameINLINE().getHolyCity(eIndex) == this);
}

bool CvCity::isHolyCity() const
{
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isHolyCity((ReligionTypes)iI))
		{
			return true;
		}
	}

	return false;
}


bool CvCity::isHeadquarters(CorporationTypes eIndex) const
{
	return (GC.getGameINLINE().getHeadquarters(eIndex) == this);
}

void CvCity::setHeadquarters(CorporationTypes eIndex)
{
	if ( !isHeadquarters(eIndex) )
	{
		GC.getGameINLINE().setHeadquarters(eIndex, this, true);

		if (GC.getCorporationInfo(eIndex).getFreeUnitClass() != NO_UNITCLASS)
		{
			UnitTypes eFreeUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getCorporationInfo(eIndex).getFreeUnitClass())));

			if (eFreeUnit != NO_UNIT)
			{
				GET_PLAYER(getOwnerINLINE()).initUnit(eFreeUnit, getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));
			}
		}
	}
}

bool CvCity::isHeadquarters() const
{
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isHeadquarters((CorporationTypes)iI))
		{
			return true;
		}
	}

	return false;
}


int CvCity::getOvercrowdingPercentAnger(int iExtra) const
{
	int iOvercrowding;
	int iAnger;

	iAnger = 0;

	iOvercrowding = (getPopulation() + iExtra);

	if (iOvercrowding > 0)
	{
		iAnger += (((iOvercrowding * GC.getPERCENT_ANGER_DIVISOR()) / std::max(1, (getPopulation() + iExtra))) + 1);
	}

	return iAnger;
}


int CvCity::getNoMilitaryPercentAnger() const
{
	int iAnger;

	iAnger = 0;

	if (getMilitaryHappinessUnits() == 0)
	{
		iAnger += GC.getDefineINT("NO_MILITARY_PERCENT_ANGER");
	}

	return iAnger;
}


int CvCity::getCulturePercentAnger() const
{
	int iTotalCulture = plot()->countTotalCulture();

	if (iTotalCulture == 0)
	{
		return 0;
	}

	int iAngryCulture = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam())
			{
				int iCulture = plot()->getCulture((PlayerTypes)iI);

				if (iCulture > 0)
				{
					if (atWar(GET_PLAYER((PlayerTypes)iI).getTeam(), getTeam()))
					{
						if ( iCulture > MAX_INT/1000 )
						{
							iCulture /= 100;
							iCulture *= std::max(0, (GC.getDefineINT("AT_WAR_CULTURE_ANGER_MODIFIER") + 100));
							iCulture *= 100;
						}
						else
						{
							iCulture *= std::max(0, (GC.getDefineINT("AT_WAR_CULTURE_ANGER_MODIFIER") + 100));
							iCulture /= 100;
						}
					}

					iAngryCulture += iCulture;
				}
			}
		}
	}

	return ((GC.getDefineINT("CULTURE_PERCENT_ANGER") * iAngryCulture) / iTotalCulture);
}


int CvCity::getReligionPercentAnger() const
{
	if (GC.getGameINLINE().getNumCities() == 0)
	{
		return 0;
	}

	if (getReligionCount() == 0)
	{
		return 0;
	}

	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (atWar(GET_PLAYER((PlayerTypes)iI).getTeam(), getTeam()))
			{
				FAssertMsg(GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam(), "Player is at war with himself! :O");

				if (GET_PLAYER((PlayerTypes)iI).getStateReligion() != NO_RELIGION)
				{
					if (isHasReligion(GET_PLAYER((PlayerTypes)iI).getStateReligion()))
					{
						iCount += GET_PLAYER((PlayerTypes)iI).getHasReligionCount(GET_PLAYER((PlayerTypes)iI).getStateReligion());
					}
				}
			}
		}
	}

	int iAnger = GC.getDefineINT("RELIGION_PERCENT_ANGER");

	iAnger *= iCount;
	iAnger /= GC.getGameINLINE().getNumCities();

	iAnger /= getReligionCount();

	return iAnger;
}


int CvCity::getHurryPercentAnger(int iExtra) const
{
	if (getHurryAngerTimer() == 0)
	{
		return 0;
	}

	return ((((((getHurryAngerTimer() - 1) / flatHurryAngerLength()) + 1) * GC.getDefineINT("HURRY_POP_ANGER") * GC.getPERCENT_ANGER_DIVISOR()) / std::max(1, getPopulation() + iExtra)) + 1);
}


int CvCity::getConscriptPercentAnger(int iExtra) const
{
	if (getConscriptAngerTimer() == 0)
	{
		return 0;
	}

	return ((((((getConscriptAngerTimer() - 1) / flatConscriptAngerLength()) + 1) * GC.getDefineINT("CONSCRIPT_POP_ANGER") * GC.getPERCENT_ANGER_DIVISOR()) / std::max(1, getPopulation() + iExtra)) + 1);
}

int CvCity::getDefyResolutionPercentAnger(int iExtra) const
{
	if (getDefyResolutionAngerTimer() == 0)
	{
		return 0;
	}

	return ((((((getDefyResolutionAngerTimer() - 1) / flatDefyResolutionAngerLength()) + 1) * GC.getDefineINT("DEFY_RESOLUTION_POP_ANGER") * GC.getPERCENT_ANGER_DIVISOR()) / std::max(1, getPopulation() + iExtra)) + 1);
}


int CvCity::getWarWearinessPercentAnger() const
{
	int iAnger = GET_PLAYER(getOwnerINLINE()).getWarWearinessPercentAnger();

	iAnger *= std::max(0, (getWarWearinessModifier() + GET_PLAYER(getOwnerINLINE()).getWarWearinessModifier() + 100));
	iAnger /= 100;
/************************************************************************************************/
/* Afforess	                  Start		 06/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iAnger *= std::max(0, (getWarWearinessTimer() + 100));
	iAnger /= 100;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return iAnger;
}

/************************************************************************************************/
/* REVOLUTION_MOD                         04/26/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvCity::getRevRequestPercentAnger(int iExtra) const
{
	if (getRevRequestAngerTimer() == 0)
	{
		return 0;
	}

	int iAnger = GC.getDefineINT("HURRY_ANGER_DIVISOR");
	iAnger *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getHurryConscriptAngerPercent();
	iAnger /= 100;

	iAnger = std::max(1, iAnger);

	int iAngerPercent = 2*((((((getRevRequestAngerTimer() - 1) / iAnger) + 1) * GC.getDefineINT("HURRY_POP_ANGER") * GC.getPERCENT_ANGER_DIVISOR()) / std::max(1, getPopulation() + iExtra)) + 1);

	return iAngerPercent;
}

int CvCity::getRevIndexPercentAnger(int iExtra) const
{
	int iLocalAdjust = std::min((getLocalRevIndex()*3)/4,getLocalRevIndex()/2);
	iLocalAdjust = std::min(iLocalAdjust,10);

	int iAnger = (int)((12.5+iLocalAdjust)*(getRevolutionIndex() - 325))/750;
	iAnger = std::max(0,iAnger);
	iAnger = std::min(iAnger, 40);

	return (GC.getPERCENT_ANGER_DIVISOR()/100)*iAnger;
}

int CvCity::getRevSuccessHappiness() const
{
	if (getRevSuccessTimer() == 0)
	{
		return 0;
	}

	int iHappy = GC.getDefineINT("HURRY_ANGER_DIVISOR");
	iHappy *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getHurryConscriptAngerPercent();
	iHappy /= 100;

	iHappy = std::max(1, iHappy);

	int iHappyPercent = 2*((((((getRevSuccessTimer() - 1) / iHappy) + 1) * GC.getDefineINT("HURRY_POP_ANGER") * GC.getPERCENT_ANGER_DIVISOR()) / std::max(1, getPopulation())) + 1);

	return (iHappyPercent*getPopulation())/GC.getPERCENT_ANGER_DIVISOR();
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

int CvCity::getLargestCityHappiness() const
{
	if (findPopulationRank() <= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities())
	{
		return GET_PLAYER(getOwnerINLINE()).getLargestCityHappiness();
	}

	return 0;
}

int CvCity::getVassalHappiness() const
{
	int iHappy = 0;

	for (int i = 0; i < MAX_TEAMS; i++)
	{
		if (getTeam() != i)
		{
			if (GET_TEAM((TeamTypes)i).isVassal(getTeam()))
			{
				iHappy += GC.getDefineINT("VASSAL_HAPPINESS");
			}
		}
	}

	return iHappy;
}

int CvCity::getVassalUnhappiness() const
{
	int iUnhappy = 0;

	for (int i = 0; i < MAX_TEAMS; i++)
	{
		if (getTeam() != i)
		{
			if (GET_TEAM(getTeam()).isVassal((TeamTypes)i))
			{
				iUnhappy += GC.getDefineINT("VASSAL_HAPPINESS");
			}
		}
	}

	return iUnhappy;
}

/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 20.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Implement iCelebrityHappy                                                        **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
int CvCity::getCelebrityHappiness() const
{
	int iHappy = 0;
	CvPlot* pPlot = plot();
	for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
	{
		iHappy += std::max(0, unitItr->getCelebrityHappy());
	}
	return iHappy;
}

/*****************************************************************************************************/
/**  TheLadiesOgre; 20.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
int CvCity::unhappyLevel(int iExtra) const
{
	PROFILE_FUNC();

	int iUnhappiness = 0;
	if (!isNoUnhappiness() && (!isCapital() || GET_PLAYER(getOwnerINLINE()).getNoCapitalUnhappiness() == 0))
	{
		int  iAngerPercent = 0;

		iAngerPercent += getOvercrowdingPercentAnger(iExtra);
		iAngerPercent += getNoMilitaryPercentAnger();
		iAngerPercent += getCulturePercentAnger();
		iAngerPercent += getReligionPercentAnger();
		iAngerPercent += getHurryPercentAnger(iExtra);
		iAngerPercent += getConscriptPercentAnger(iExtra);
		iAngerPercent += getDefyResolutionPercentAnger(iExtra);
		iAngerPercent += getWarWearinessPercentAnger();
		iAngerPercent += getRevRequestPercentAnger(iExtra);	
		iAngerPercent += getRevIndexPercentAnger(iExtra);

		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			iAngerPercent += GET_PLAYER(getOwnerINLINE()).getCivicPercentAnger((CivicTypes)iI);
		}

		iUnhappiness = ((iAngerPercent * (getPopulation() + iExtra)) / GC.getPERCENT_ANGER_DIVISOR());

		iUnhappiness -= std::min(0, getLargestCityHappiness());
		iUnhappiness -= std::min(0, getMilitaryHappiness());
		iUnhappiness -= std::min(0, getCurrentStateReligionHappiness());
		iUnhappiness -= std::min(0, getBuildingBadHappiness());
		iUnhappiness -= std::min(0, getExtraBuildingBadHappiness());
		iUnhappiness -= std::min(0, getFeatureBadHappiness());
		iUnhappiness -= std::min(0, getBonusBadHappiness());
		iUnhappiness -= std::min(0, getReligionBadHappiness());
		iUnhappiness -= std::min(0, getCommerceHappiness());
		iUnhappiness -= std::min(0, area()->getBuildingHappiness(getOwnerINLINE()));
		iUnhappiness -= std::min(0, GET_PLAYER(getOwnerINLINE()).getBuildingHappiness());
		iUnhappiness -= std::min(0, (getExtraHappiness() + GET_PLAYER(getOwnerINLINE()).getExtraHappiness()));
		iUnhappiness -= std::min(0, GC.getHandicapInfo(getHandicapType()).getHappyBonus());
		iUnhappiness += std::max(0, getVassalUnhappiness());
		iUnhappiness += std::max(0, getEspionageHappinessCounter());
		iUnhappiness -= std::min(0, getCivicHappiness());
		iUnhappiness -= std::min(0, getSpecialistUnhappiness() / 100);
		iUnhappiness -= std::min(0, (GET_PLAYER(getOwnerINLINE()).getWorldHappiness()));
		iUnhappiness -= std::min(0, (GET_PLAYER(getOwnerINLINE()).getProjectHappiness()));
		iUnhappiness += std::max(0, GET_PLAYER(getOwnerINLINE()).calculateTaxRateUnhappiness());
		iUnhappiness -= std::min(0, calculateCorporationHappiness());
		iUnhappiness += std::max(0, getEventAnger());
		iUnhappiness += getExtraTechUnHappinessTotal();

		int iForeignAnger = GET_PLAYER(getOwnerINLINE()).getForeignUnhappyPercent();
		if (iForeignAnger != 0) {
			iForeignAnger = 100 / iForeignAnger;
			iForeignAnger = ((100 - plot()->calculateCulturePercent(getOwnerINLINE())) * iForeignAnger) / 100;
			iUnhappiness += std::max(0, iForeignAnger);
		}
		if (GC.getGameINLINE().isOption(GAMEOPTION_PERSONALIZED_MAP))
		{
			if (!GET_PLAYER(getOwnerINLINE()).isNoLandmarkAnger())
			{
				iUnhappiness += std::max(0, getLandmarkAnger());
			}
			iUnhappiness -= std::min(0, GET_PLAYER(getOwnerINLINE()).getLandmarkHappiness());
		}

		if ( GET_PLAYER(getOwnerINLINE()).getCityLimit() != 0 &&
			 GET_PLAYER(getOwnerINLINE()).getCityOverLimitUnhappy() != 0 )
		{
			int overLimitCities = GET_PLAYER(getOwnerINLINE()).getNumCities() - GET_PLAYER(getOwnerINLINE()).getCityLimit();

			if ( overLimitCities > 0 )
			{
				iUnhappiness += GET_PLAYER(getOwnerINLINE()).getCityOverLimitUnhappy()*overLimitCities;
			}
		}
	}

	return std::max(0, iUnhappiness);
}


int CvCity::happyLevel() const
{
	PROFILE_FUNC();

	int iHappiness = 0;

/************************************************************************************************/
/* REVOLUTION_MOD                         04/28/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iHappiness += std::max(0, getRevSuccessHappiness());
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	iHappiness += std::max(0, getLargestCityHappiness());
	iHappiness += std::max(0, getMilitaryHappiness());
	iHappiness += std::max(0, getCurrentStateReligionHappiness());
	iHappiness += std::max(0, getBuildingGoodHappiness());
	iHappiness += std::max(0, getExtraBuildingGoodHappiness());
	iHappiness += std::max(0, getFeatureGoodHappiness());
	iHappiness += std::max(0, getBonusGoodHappiness());
	iHappiness += std::max(0, getReligionGoodHappiness());
	iHappiness += std::max(0, getCommerceHappiness());
	iHappiness += std::max(0, area()->getBuildingHappiness(getOwnerINLINE()));
	iHappiness += std::max(0, GET_PLAYER(getOwnerINLINE()).getBuildingHappiness());
	iHappiness += std::max(0, (getExtraHappiness() + GET_PLAYER(getOwnerINLINE()).getExtraHappiness()));
	iHappiness += std::max(0, GC.getHandicapInfo(getHandicapType()).getHappyBonus());
	iHappiness += std::max(0, getVassalHappiness());
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iHappiness += std::max(0, getCivicHappiness());
	iHappiness += std::max(0, getSpecialistHappiness() / 100);
	iHappiness += std::max(0, (GET_PLAYER(getOwnerINLINE()).getWorldHappiness()));
	iHappiness += std::max(0, (GET_PLAYER(getOwnerINLINE()).getProjectHappiness()));
	iHappiness += std::max(0, calculateCorporationHappiness());
	if (GC.getGameINLINE().isOption(GAMEOPTION_PERSONALIZED_MAP))
	{
		iHappiness += std::max(0, GET_PLAYER(getOwnerINLINE()).getLandmarkHappiness());
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

	if (getHappinessTimer() > 0)
	{
		iHappiness += GC.getDefineINT("TEMP_HAPPY");
	}
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 09.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Implement iCelebrityHappy                                                        **/
/**  Notes: Added to C2C by TB (Thunderbrd)                                                         **/
/*****************************************************************************************************/
	iHappiness += std::max(0, getCelebrityHappiness());
/*****************************************************************************************************/
/**  TheLadiesOgre; 09.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
//Team Project (1)
	iHappiness += getExtraTechHappinessTotal();

	return std::max(0, iHappiness);
}


int CvCity::angryPopulation(int iExtra) const
{
	PROFILE("CvCityAI::angryPopulation");

	return range((unhappyLevel(iExtra) - happyLevel()), 0, (getPopulation() + iExtra));
}


int CvCity::visiblePopulation() const
{
/************************************************************************************************/
/* Afforess	                  Start		 08/23/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	return (getPopulation() - angryPopulation() - getWorkingPopulation() - getNumPopulationEmployed());
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


int CvCity::totalFreeSpecialists() const
{
	int iCount = 0;
	if (getPopulation() > 0)
	{
		iCount += getFreeSpecialist();
		iCount += area()->getFreeSpecialist(getOwnerINLINE());
		iCount += GET_PLAYER(getOwnerINLINE()).getFreeSpecialist();

		for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); ++iImprovement)
		{
			int iNumSpecialistsPerImprovement = getImprovementFreeSpecialists((ImprovementTypes)iImprovement);
			if (iNumSpecialistsPerImprovement > 0)
			{
				iCount += iNumSpecialistsPerImprovement * countNumImprovedPlots((ImprovementTypes)iImprovement);
			}
		}

//Team Project (6)
		if (GET_PLAYER(getOwnerINLINE()).hasFreeSpecialistperWorldWonder())
		{
			iCount += getNumWorldWonders();
		}

		if (GET_PLAYER(getOwnerINLINE()).hasFreeSpecialistperNationalWonder())
		{
			iCount += getNumNationalWonders();
		}

		if (GET_PLAYER(getOwnerINLINE()).hasFreeSpecialistperTeamProject())
		{
			iCount += getNumTeamWonders();
		}
	}

	return iCount;
}


int CvCity::extraPopulation() const
{
	return (visiblePopulation() + std::min(0, extraFreeSpecialists()));
}


int CvCity::extraSpecialists() const
{
	return (visiblePopulation() + extraFreeSpecialists());
}


int CvCity::extraFreeSpecialists() const
{
	return (totalFreeSpecialists() - getSpecialistPopulation());
}


int CvCity::unhealthyPopulation(bool bNoAngry, int iExtra) const
{
	if (isNoUnhealthyPopulation())
	{
		return 0;
	}

	return std::max(0, (getPopulation() + iExtra - ( bNoAngry ? angryPopulation(iExtra) : 0)));
}


int CvCity::totalGoodBuildingHealth() const
{
/************************************************************************************************/
/* Afforess	                  Start		 08/31/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	return (getBuildingGoodHealth() + area()->getBuildingGoodHealth(getOwnerINLINE()) + GET_PLAYER(getOwnerINLINE()).getBuildingGoodHealth() + getExtraBuildingGoodHealth());
*/
	int iHealth = getBuildingGoodHealth();
	iHealth += area()->getBuildingGoodHealth(getOwnerINLINE());
	iHealth += GET_PLAYER(getOwnerINLINE()).getBuildingGoodHealth();
	iHealth += getExtraBuildingGoodHealth();
	iHealth += std::max(0, calculatePopulationHealth());
	return iHealth;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


int CvCity::totalBadBuildingHealth() const
{
/************************************************************************************************/
/* Afforess	                  Start		 08/31/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	if (!isBuildingOnlyHealthy())
	{
		return (getBuildingBadHealth() + area()->getBuildingBadHealth(getOwnerINLINE()) + GET_PLAYER(getOwnerINLINE()).getBuildingBadHealth() + getExtraBuildingBadHealth());
	}
*/
	if (!isBuildingOnlyHealthy())
	{
		int iHealth = getBuildingBadHealth();
		iHealth += area()->getBuildingBadHealth(getOwnerINLINE());
		iHealth += GET_PLAYER(getOwnerINLINE()).getBuildingBadHealth();
		iHealth += getExtraBuildingBadHealth();
		iHealth += std::min(0, calculatePopulationHealth());
		return iHealth;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return 0;
}


int CvCity::goodHealth() const
{
	PROFILE_FUNC();

	int iTotalHealth = 0;

	iTotalHealth += std::max<int>(0, getFreshWaterGoodHealth());
	iTotalHealth += std::max<int>(0, getFeatureGoodHealth());
	iTotalHealth += std::max<int>(0, getPowerGoodHealth());
	iTotalHealth += std::max<int>(0, getBonusGoodHealth());
	iTotalHealth += std::max<int>(0, totalGoodBuildingHealth());
	iTotalHealth += std::max<int>(0, GET_PLAYER(getOwnerINLINE()).getExtraHealth() + getExtraHealth());
	iTotalHealth += std::max<int>(0, GC.getHandicapInfo(getHandicapType()).getHealthBonus());

/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

	iTotalHealth += std::max<int>(0, getImprovementGoodHealth() / 100);
	iTotalHealth += std::max<int>(0, getSpecialistGoodHealth() / 100);
	iTotalHealth += std::max<int>(0, GET_PLAYER(getOwnerINLINE()).getCivilizationHealth());
	iTotalHealth += std::max<int>(0, calculateCorporationHealth());
	iTotalHealth += std::max<int>(0, GET_PLAYER(getOwnerINLINE()).getWorldHealth());
	iTotalHealth += std::max<int>(0, GET_PLAYER(getOwnerINLINE()).getProjectHealth());

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	//Team Project (1)
	iTotalHealth += std::max<int>(0, getExtraTechHealthTotal());

	return iTotalHealth;
}


int CvCity::badHealth(bool bNoAngry, int iExtra) const
{
	PROFILE_FUNC();

	int iTotalHealth = 0;

	iTotalHealth -= std::max<int>(0, getEspionageHealthCounter());
	iTotalHealth += std::min<int>(0, getFreshWaterBadHealth());
	iTotalHealth += std::min<int>(0, getFeatureBadHealth());
	iTotalHealth += std::min<int>(0, getPowerBadHealth());
	iTotalHealth += std::min<int>(0, getBonusBadHealth());
	iTotalHealth += std::min<int>(0, totalBadBuildingHealth());
	iTotalHealth += std::min<int>(0, GET_PLAYER(getOwnerINLINE()).getExtraHealth() + getExtraHealth());
	iTotalHealth += std::min<int>(0, GC.getHandicapInfo(getHandicapType()).getHealthBonus());
	iTotalHealth += std::min<int>(0, getExtraBuildingBadHealth());

/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

	iTotalHealth += std::min<int>(0, getImprovementBadHealth() / 100);
	iTotalHealth += std::min<int>(0, getSpecialistBadHealth() / 100);
	iTotalHealth += std::min<int>(0, GET_PLAYER(getOwnerINLINE()).getCivilizationHealth());
	iTotalHealth += std::min<int>(0, calculateCorporationHealth());
	iTotalHealth += std::min<int>(0, GET_PLAYER(getOwnerINLINE()).getWorldHealth());
	iTotalHealth += std::min<int>(0, GET_PLAYER(getOwnerINLINE()).getProjectHealth());

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	//Team Project (1)
	iTotalHealth += std::max<int>(0, getExtraTechUnHealthTotal());

	return (unhealthyPopulation(bNoAngry, iExtra) - iTotalHealth);
}


int CvCity::healthRate(bool bNoAngry, int iExtra) const
{
	return std::min(0, (goodHealth() - badHealth(bNoAngry, iExtra)));
}


int CvCity::foodConsumption(bool bNoAngry, int iExtra, bool bIncludeWastage) const
{
	int iPopulationExponent = getPopulation() - 1; // Each pop past the first increases consumption per population by .1, rounded down.  Each point of population means more actual people the higher the amount goes.
	int iConsumptionPerPopulationBase = iPopulationExponent + (GC.getFOOD_CONSUMPTION_PER_POPULATION() * 10);
	int iConsumptionbyPopulation = (((getPopulation() + iExtra) - ((bNoAngry) ? angryPopulation(iExtra) : 0)) * iConsumptionPerPopulationBase) / 10;
	int result = (iConsumptionbyPopulation - healthRate(bNoAngry, iExtra) + (bIncludeWastage ? (int)foodWastage() : 0));

	return result;
}

float CvCity::foodWastage(int surplass) const//Included by Thunderbrd 6/8/2019, code contributed by Sorcdk
{
#define    MAX_SURPLASS    2000
	static    float    calculatedWaste[MAX_SURPLASS];
	static    int calculatedTo = -1;
	int        startWasteAtConsumptionPercent = GC.getDefineINT("WASTAGE_START_CONSUMPTION_PERCENT");
	float    wastageGrowthFactor = GC.getDefineFLOAT("WASTAGE_GROWTH_FACTOR");

	if (wastageGrowthFactor == 0)
	{
		wastageGrowthFactor = (float)0.05;    //    default
	}

	if (startWasteAtConsumptionPercent >= 0)
	{
		if (surplass == -1)
		{
			int iPopulationExponent = getPopulation() - 1; // Each pop past the first increases consumption per population by .1, rounded down.  Each point of population means more actual people the higher the amount goes.
			int iConsumptionPerPopulationBase = iPopulationExponent + (GC.getFOOD_CONSUMPTION_PER_POPULATION() * 10);
			int iConsumptionbyPopulation = (getPopulation() * iConsumptionPerPopulationBase) / 10;
			surplass = foodDifference(true, false) - (iConsumptionbyPopulation * startWasteAtConsumptionPercent) / 100;
		}
	}
	else
	{
		surplass = -1;
	}

	//    Nothing wasted if there is no surplass
	if (surplass <= 0)
	{
		return 0;
	}
	//    Cache what we can as it's not a trivially cheap computation
	else if (surplass <= calculatedTo)
	{
		return calculatedWaste[surplass];
	}
	else
	{
		if (surplass >= MAX_SURPLASS)
		{
			//    After the max we shift to from assymtotic behavior toward the limit to the limit of efficiency
			return foodWastage(MAX_SURPLASS - 1) + (foodWastage(MAX_SURPLASS - 1) - foodWastage(MAX_SURPLASS - 2)) * (surplass - MAX_SURPLASS + 1);
			//return foodWastage(MAX_SURPLASS - 1) + ((float)1.0 - wastageGrowthFactor/((float)1.0+wastageGrowthFactor)) * (surplass - MAX_SURPLASS + 1);
			//    After the max we bother calculating it all gets wasted
			//return calculatedWaste[MAX_SURPLASS-1] + ((float)1.0 - wastageGrowthFactor) * (surplass - MAX_SURPLASS + 1);
		}
		else
		{
			calculatedWaste[surplass] = foodWastage(surplass - 1) + (float)1.0 - (wastageGrowthFactor + ((float)1.0 - wastageGrowthFactor) / ((float)1.0 + (float)0.05 * (float)surplass));
			//calculatedWaste[surplass] = foodWastage(surplass-1) + ((float)1 - (wastageGrowthFactor + pow((float)1.0 - wastageGrowthFactor, surplass))/((float)1.0+wastageGrowthFactor));
			calculatedTo = surplass;

			return calculatedWaste[surplass];
		}
	}
}
//Old version
//float CvCity::foodWastage(int surplass) const
//{
//#define	MAX_SURPLASS	500
//	static	float	calculatedWaste[MAX_SURPLASS];
//	static	int calculatedTo = -1;
//	int		startWasteAtConsumptionPercent = GC.getDefineINT("WASTAGE_START_CONSUMPTION_PERCENT");
//	float	wastageGrowthFactor = GC.getDefineFLOAT("WASTAGE_GROWTH_FACTOR");
//
//	if ( wastageGrowthFactor == 0 )
//	{
//		wastageGrowthFactor = (float)0.05;	//	default
//	}
//
//	if ( startWasteAtConsumptionPercent >= 0 )
//	{
//		if ( surplass == -1 )
//		{
//			int iPopulationExponent = getPopulation() - 1; // Each pop past the first increases consumption per population by .1, rounded down.  Each point of population means more actual people the higher the amount goes.
//			int iConsumptionPerPopulationBase = iPopulationExponent + (GC.getFOOD_CONSUMPTION_PER_POPULATION() * 10);
//			int iConsumptionbyPopulation = (getPopulation() * iConsumptionPerPopulationBase) / 10;
//			surplass = foodDifference(true, false) - (iConsumptionbyPopulation * startWasteAtConsumptionPercent)/100;
//		}
//	}
//	else
//	{
//		surplass = -1;
//	}
//
//	//	Nothing wasted if there is no surplass
//	if ( surplass <= 0 )
//	{
//		return 0;
//	}
//	//	Cache what we can as it's not a trivially cheap computation
//	else if ( surplass <= calculatedTo )
//	{
//		return calculatedWaste[surplass];
//	}
//	else
//	{
//		if ( surplass >= MAX_SURPLASS )
//		{
//			//	After the max we bother calculating it all gets wasted
//			return calculatedWaste[MAX_SURPLASS-1] + (surplass - MAX_SURPLASS + 1);
//		}
//		else
//		{
//			calculatedWaste[surplass] = foodWastage(surplass-1) + ((float)1 - (wastageGrowthFactor + pow((float)1.0 - wastageGrowthFactor, surplass))/((float)1.0+wastageGrowthFactor));
//			calculatedTo = surplass;
//
//			return calculatedWaste[surplass];
//		}
//	}
//}

int CvCity::foodDifference(bool bBottom, bool bIncludeWastage, bool bIgnoreFoodBuildOrRev) const
{
	int iDifference;

	if (!bIgnoreFoodBuildOrRev && isDisorder())
	{
		return 0;
	}

	if (!bIgnoreFoodBuildOrRev && isFoodProduction())
	{
		iDifference = std::min(0, (getYieldRate(YIELD_FOOD) - foodConsumption(false,0,bIncludeWastage)));
	}
	else
	{
		iDifference = (getYieldRate(YIELD_FOOD) - foodConsumption(false,0,bIncludeWastage));
	}

	if (bBottom)
	{
		if ((getPopulation() == 1) && (getFood() == 0))
		{
			iDifference = std::max(0, iDifference);
		}
	}

	return iDifference;
}

/************************************************************************************************/
/* Afforess  Food Threshold Modifier   Start          10/16/09                   		        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvCity::growthThreshold() const
{
	int iThreshold = GET_PLAYER(getOwnerINLINE()).getGrowthThreshold(getPopulation());
	
	iThreshold *= (GET_PLAYER(getOwnerINLINE()).getPopulationgrowthratepercentage() + 100);
	iThreshold /= 100;
	
	iThreshold *= (getPopulationgrowthratepercentage() + 100);
	iThreshold /= 100;
	
	if (getNumCityPlots() == NUM_CITY_PLOTS)
	{
		iThreshold = iThreshold*(100+GC.getDefineINT("CITY_THIRD_RING_EXTRA_GROWTH_THRESHOLD_PERCENT"))/100;
	}

	if ( isHominid() )
	{
		iThreshold /= 2;	//	Those barbarians are just so damned fecund!
	}

	return std::max(1,iThreshold);

}
/************************************************************************************************/
/* Afforess  Food Threshold Modifier                        END                  		        */
/************************************************************************************************/
int CvCity::productionLeft() const
{
	return (getProductionNeeded() - getProduction());
}

int CvCity::getHurryCostModifier(bool bIgnoreNew) const
{
	int iModifier = 100;
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			iModifier = getHurryCostModifier((UnitTypes) EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1), bIgnoreNew);
			break;

		case ORDER_CONSTRUCT:
			iModifier = getHurryCostModifier((BuildingTypes) pOrderNode->m_data.iData1, bIgnoreNew);
			break;

		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType did not match a valid option");
			break;
		}
	}

	return iModifier;
}

int CvCity::getHurryCostModifier(UnitTypes eUnit, bool bIgnoreNew) const
{
	return getHurryCostModifier(GC.getUnitInfo(eUnit).getHurryCostModifier(), getUnitProduction(eUnit), bIgnoreNew);
}

int CvCity::getHurryCostModifier(BuildingTypes eBuilding, bool bIgnoreNew) const
{
	return getHurryCostModifier(GC.getBuildingInfo(eBuilding).getHurryCostModifier(), getBuildingProduction(eBuilding), bIgnoreNew);
}

int CvCity::getHurryCostModifier(int iBaseModifier, int iProduction, bool bIgnoreNew) const
{
	int iModifier = 100;
	iModifier *= std::max(0, iBaseModifier + 100);
	iModifier /= 100;

	if (iProduction == 0 && !bIgnoreNew)
	{
		iModifier *= std::max(0, (GC.getDefineINT("NEW_HURRY_MODIFIER") + 100));
		iModifier /= 100;
	}

	iModifier *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getHurryModifier() + 100));
	iModifier /= 100;
/************************************************************************************************/
/* Afforess	                  Start		 06/27/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_ECONOMY))
	{
		iModifier *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getHurryCostModifier() + 100));
		iModifier /= 100;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return std::max(1,iModifier);	//	Avoid potential divide by 0s
}


int CvCity::hurryCost(bool bExtra) const
{
	return (getHurryCost(bExtra, productionLeft(), getHurryCostModifier(), getProductionModifier()));
}

int CvCity::getHurryCost(bool bExtra, UnitTypes eUnit, bool bIgnoreNew) const
{
	int iProductionLeft = getProductionNeeded(eUnit) - getUnitProduction(eUnit);

	return getHurryCost(bExtra, iProductionLeft, getHurryCostModifier(eUnit, bIgnoreNew), getProductionModifier(eUnit));
}

int CvCity::getHurryCost(bool bExtra, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	int iProductionLeft = getProductionNeeded(eBuilding) - getBuildingProduction(eBuilding);

	return getHurryCost(bExtra, iProductionLeft, getHurryCostModifier(eBuilding, bIgnoreNew), getProductionModifier(eBuilding));
}

int CvCity::getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const
{
	int iProduction = (iProductionLeft * iHurryModifier + 99) / 100; // round up

	if (bExtra)
	{
		int iExtraProduction = getExtraProductionDifference(iProduction, iModifier);
		if (iExtraProduction > 0)
		{
			int iAdjustedProd = iProduction * iProduction;
			
			// round up
			iProduction = (iAdjustedProd + (iExtraProduction - 1)) / iExtraProduction;
		}
	}

	return std::max(0, iProduction);
}

int CvCity::hurryGold(HurryTypes eHurry) const
{
	return getHurryGold(eHurry, hurryCost(false));
}

int CvCity::getHurryGold(HurryTypes eHurry, int iHurryCost) const
{
	int iGold;

	if (GC.getHurryInfo(eHurry).getGoldPerProduction() == 0)
	{
		return 0;
	}

	iGold = (iHurryCost * GC.getHurryInfo(eHurry).getGoldPerProduction());

	FAssert(iGold <= GC.getGREATER_COMMERCE_SWITCH_POINT());//While protected below, if this starts coming up, I'll need to start taking measures to improve this system of hurrying to allow it to start breaking into over 1 million in cost.)

	iGold = std::min(iGold, GC.getGREATER_COMMERCE_SWITCH_POINT());

	return std::max(1, iGold);
}


int CvCity::hurryPopulation(HurryTypes eHurry) const
{
	return (getHurryPopulation(eHurry, hurryCost(true)));
}

int CvCity::getHurryPopulation(HurryTypes eHurry, int iHurryCost) const
{
	if (GC.getHurryInfo(eHurry).getProductionPerPopulation() == 0)
	{
		return 0;
	}

	int iPopulation = (iHurryCost - 1) / GC.getGameINLINE().getProductionPerPopulation(eHurry);

	return std::max(1, (iPopulation + 1));
}

int CvCity::hurryProduction(HurryTypes eHurry) const
{
	int iProduction;

	if (GC.getHurryInfo(eHurry).getProductionPerPopulation() > 0)
	{
		iProduction = (100 * getExtraProductionDifference(hurryPopulation(eHurry) * GC.getGameINLINE().getProductionPerPopulation(eHurry))) / std::max(1, getHurryCostModifier());
		FAssert(iProduction >= productionLeft());
	}
	else
	{
		iProduction = productionLeft();
	}

	return iProduction;
}


int CvCity::flatHurryAngerLength() const
{
	int iAnger;

	iAnger = GC.getDefineINT("HURRY_ANGER_DIVISOR");
	iAnger *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getHurryConscriptAngerPercent();
	iAnger /= 100;
	iAnger *= std::max(0, 100 + getHurryAngerModifier());
	iAnger /= 100;

	return std::max(1, iAnger);
}


int CvCity::hurryAngerLength(HurryTypes eHurry) const
{
	if (GC.getHurryInfo(eHurry).isAnger())
	{
		return flatHurryAngerLength();
	}
	else
	{
		return 0;
	}
}


int CvCity::maxHurryPopulation() const
{
	return (getPopulation() / 2);
}

/************************************************************************************************/
/* phunny_pharmer                Start		 04/20/10                                           */
/*   the goal of this modification is to make it so that difficult tiles (ie hills, mountains)  */
/*   receive less culture per turn; this will make cultural borders grow more slowly on these   */
/*   plots and will lead to cultural borders at mountains and other key features                */
/************************************************************************************************/

#define	MAX_CLOSE_DIST	100	//	Arbitrary to some extent
#define	HASH_RELATIVE_CLOSE_DIST(x,y)	(((x) + MAX_CLOSE_DIST) + 2*MAX_CLOSE_DIST*((y) + MAX_CLOSE_DIST))

void CvCity::recalculateCultureDistances(int iMaxDistance) const
{
	PROFILE_FUNC();
	MEMORY_TRACK_EXEMPT();
	
	// if the point is within one square of the city center
	for(int iDX = -1; iDX <= 1; ++iDX)
	{
		for(int iDY = -1; iDY <= 1; ++iDY)
		{
			int iPlotIndex = HASH_RELATIVE_CLOSE_DIST(iDX,iDY);

			// then the plot distance should be set to one
			//   as all points one away from the city have this default value
			m_aCultureDistances[iPlotIndex] = 1;
		}
	}

	// determine whether or not any value has changed when calculating distances
	//   note that this is initially set to true as long as the maximum distance is not 1
	//   when values cease to change, the final distances have been calculated
	bool bHasChanged = (iMaxDistance != 1);

	// as long as there are changes during the last iteration
	while(bHasChanged)
	{
		// reset the has changed variable to note a new loop cycle has begun
		bHasChanged = false;

		for(int iDX = -iMaxDistance; iDX <= iMaxDistance; ++iDX)
		{
			for(int iDY = -iMaxDistance; iDY <= iMaxDistance; ++iDY)
			{
				int iPlotIndex = HASH_RELATIVE_CLOSE_DIST(iDX,iDY);

				// find the distance to the current plot
				int distance = plotDistance(0, 0, iDX, iDY);

				// if the point is more than one cell away from the city center
				//   then it should be recalculated, as cells closer to the city
				//   will have already been fixed
				if(distance > 1)
				{
					// recalculate the value to determine if it has changed
					int iNewValue = calculateCultureDistance(iDX, iDY, iMaxDistance);

					// if it has changed, save the value and mark that
					//   all values should be recomputed since they
					//   may depend on this value
					if(m_aCultureDistances[iPlotIndex] != iNewValue)
					{
						m_aCultureDistances[iPlotIndex] = iNewValue;
						bHasChanged = true;
					}
				}
			}
		}
	}
}

int CvCity::calculateCultureDistance(int iDX, int iDY, int iMaxDistance) const
{
	PROFILE_FUNC();
	
	// find the current plot and the distance to it
	CvPlot* pPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

	// if the plot distance is greater than the maximum desired plot distance
	//  or if the plot does not exist, then the plot distance is maximal
	if(plotDistance(0, 0, iDX, iDY) > iMaxDistance || pPlot == NULL) return MAX_INT;

	// determine the distance from the square to the city from all directions
	//   it is entirely possible that the shortest distance to a city may come
	//   from an unusual direction, eg if there is a mountain range in the way
	int distance = MAX_INT;

	// check all directions to determine the cultural distance
	//   note 1: all directions are checked
	//   note 2: the distance to the plot is defined as the distance to the
	//     neighbor, plus any penalty for crossing a river, plus an additional
	//     one for reaching that particular square
	//   note 3: if a neighbor has a distance of MAX_INT, then it is ignored
	//     since that means that the neighbor is defined
	int iPlotIndex = HASH_RELATIVE_CLOSE_DIST(iDX+1,iDY);
	int iEastDist = m_aCultureDistances[iPlotIndex];
	if(iEastDist != 0 && iEastDist != MAX_INT)
	{
		iEastDist += pPlot->isRiverCrossing(DIRECTION_EAST);
		distance = std::min(distance, iEastDist+1);
	}
	
	iPlotIndex = HASH_RELATIVE_CLOSE_DIST(iDX,iDY-1);
	int iSouthDist = m_aCultureDistances[iPlotIndex];
	if(iSouthDist != 0 && iSouthDist != MAX_INT)
	{
		iSouthDist += pPlot->isRiverCrossing(DIRECTION_SOUTH);
		distance = std::min(distance, iSouthDist+1);
	}
	
	iPlotIndex = HASH_RELATIVE_CLOSE_DIST(iDX-1,iDY);
	int iWestDist = m_aCultureDistances[iPlotIndex];
	if(iWestDist != 0 && iWestDist != MAX_INT)
	{
		iWestDist += pPlot->isRiverCrossing(DIRECTION_WEST);
		distance = std::min(distance, iWestDist+1);
	}
	
	iPlotIndex = HASH_RELATIVE_CLOSE_DIST(iDX,iDY+1);
	int iNorthDist = m_aCultureDistances[iPlotIndex];
	if(iNorthDist != 0 && iNorthDist != MAX_INT)
	{
		iNorthDist += pPlot->isRiverCrossing(DIRECTION_NORTH);
		distance = std::min(distance, iNorthDist+1);
	}

	// if the distance to the plot is unchanged, perhaps because all the
	//   neighbors have distance MAX_INT, return the value of MAX_INT in
	//   order to ensure that the culture distance is recalculated
	if(distance == MAX_INT) return MAX_INT;

	// increase the cultural distance for the tile if it is difficult terrain
	//   key idea: distant and difficult terrain will accumulate less culture
	//   each turn, making cultural borders grow more slowly on these terrain

	// if the plot is a hills, tundra, or marsh, or coast plot
	//   increase the effective distance by one
	
	bool bBonus = (pPlot->getBonusType(getTeam()) != NO_BONUS);
	int terrainDistance = 0;
	TerrainTypes eTerrain;
	if(pPlot->isHills())
	{
		eTerrain = CvTerrainInfo::getTerrainHill();
	}
	else if(pPlot->isPeak2(true))
	{
		eTerrain = CvTerrainInfo::getTerrainPeak();
	}
	else
	{
		eTerrain = pPlot->getTerrainType();
	}

	if (!bBonus)
	{
		terrainDistance += GC.getTerrainInfo(eTerrain).getCultureDistance();
	}

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		//used for floodplains
		if (GC.getFeatureInfo(pPlot->getFeatureType()).isIgnoreTerrainCulture())
		{
			terrainDistance = 0;
		}
		if (!bBonus)
		{
			terrainDistance += GC.getFeatureInfo(pPlot->getFeatureType()).getCultureDistance();
		}
	}
	distance += std::max(0, terrainDistance);

	// at this point, we are done
	//   save the cached distance in the m_aCultureDistances structure
	//   in order to facilitate the next step of the dynamic programming
	return distance;
}
/************************************************************************************************/
/* phunny_pharmer                    END                                                        */
/************************************************************************************************/
/************************************************************************************************/
/* phunny_pharmer                Start		 05/03/10                                           */
/*   clear all the values in the culture distance cache; these values will have to be recom-    */
/*     puted on the next relevant call to cultureDistance()                                     */
/************************************************************************************************/
void CvCity::clearCultureDistanceCache()
{
	m_aCultureDistances.clear();
}
/************************************************************************************************/
/* phunny_pharmer                    END                                                        */
/************************************************************************************************/
/************************************************************************************************/
/* phunny_pharmer                Start		 04/20/10                                           */
/*   the cache of culture distances precomputed by recalculateCultureDistance is used in order  */
/*     to determine the culture distance from the city center the plot location                 */
/************************************************************************************************/
int CvCity::cultureDistance(int iDX, int iDY, bool bForce) const
{
	PROFILE_FUNC();
	
	if (!bForce && GC.getGameINLINE().isOption(GAMEOPTION_REALISTIC_CULTURE_SPREAD))
	{
		//	This is not a true plot index but we can assume iDX and iDY are less than the total map dimensions so
		//	it is a unique index
		int iPlotIndex = HASH_RELATIVE_CLOSE_DIST(iDX,iDY);

		std::map<int,int>::const_iterator itr = m_aCultureDistances.find(iPlotIndex);

		if ( itr == m_aCultureDistances.end() )
		{
			recalculateCultureDistances(plotDistance(0, 0, iDX, iDY));
			
			return m_aCultureDistances[iPlotIndex];
		}
		else
		{
			return itr->second;
		}
	}
	return plotDistance(0, 0, iDX, iDY);
}
/************************************************************************************************/
/* phunny_pharmer                    END                                                        */
/************************************************************************************************/

int CvCity::cultureStrength(PlayerTypes ePlayer, int &iOriginal) const
{
	int iStrength = 1;

	iStrength += (getHighestPopulation() * 2);

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getOwnerINLINE() == ePlayer)
			{
				iStrength += (GC.getGameINLINE().getCurrentEra() + 1);
			}
		}
	}

	//	Handle culture getting very large
	int	iPlayerCulture = plot()->getCulture(ePlayer);
	int iOwnerCulture = plot()->getCulture(getOwnerINLINE());

	if ( iPlayerCulture > MAX_INT/1000 || iOwnerCulture > MAX_INT/1000 )
	{
		iPlayerCulture /= 1000;
		iOwnerCulture /= 1000;

		FAssert(GC.getDefineINT("REVOLT_TOTAL_CULTURE_MODIFIER") < 1000);
	}

	iStrength *= std::max(0, (((GC.getDefineINT("REVOLT_TOTAL_CULTURE_MODIFIER") * (iPlayerCulture - iOwnerCulture + 1)) / (iPlayerCulture + 1)) + 100));
	iStrength /= 100;

	if (GET_PLAYER(ePlayer).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(ePlayer).getStateReligion()))
		{
			iStrength *= std::max(0, (GC.getDefineINT("REVOLT_OFFENSE_STATE_RELIGION_MODIFIER") + 100));
			iStrength /= 100;
		}
	}

	if (GET_PLAYER(getOwnerINLINE()).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(getOwnerINLINE()).getStateReligion()))
		{
			iStrength *= std::max(0, (GC.getDefineINT("REVOLT_DEFENSE_STATE_RELIGION_MODIFIER") + 100));
			iStrength /= 100;
		}
	}
	iOriginal = iStrength;
	int iGarrisonReduction = (iStrength * cultureGarrison(ePlayer))/100;
	iStrength -= iGarrisonReduction;
	iStrength = std::max(0, iStrength);
	return iStrength;
}


int CvCity::cultureGarrison(PlayerTypes ePlayer) const
{
	int iGarrison = 1;
	
	CvPlot* pPlot = plot();
	for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
	{
		iGarrison += unitItr->revoltProtectionTotal();
	}

	if (atWar(GET_PLAYER(ePlayer).getTeam(), getTeam()))
	{
		iGarrison *= 2;
	}

	return iGarrison;
}

//	This routine is basically used after an old format load, or for a new city
void CvCity::calculateBuildingReplacements() const
{
	PROFILE_FUNC();

	m_paiBuildingReplaced = new int[GC.getNumBuildingInfos()];

	memset(m_paiBuildingReplaced,0,sizeof(int)*GC.getNumBuildingInfos());

	for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if ( getNumActiveBuilding((BuildingTypes)iI) > 0 )
		{
			for(int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				if (GC.getBuildingInfo((BuildingTypes)iJ).isReplaceBuildingClass(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType()))
				{
					//	Cope with old format saves where the calculation will not previously
					//	have been done and so the effects need to be processed
					if ( !m_bHasCalculatedBuildingReplacement && getNumBuilding((BuildingTypes)iJ) > 0 )
					{
						(const_cast<CvCity*>(this))->processBuilding((BuildingTypes)iJ, -1);
					}

					m_paiBuildingReplaced[iJ]++;
				}
			}
		}
	}

	m_bHasCalculatedBuildingReplacement = TRUE;
}

void CvCity::changeBuildingReplacementCount(BuildingTypes eBuilding, bool bAdd)
{
	FAssert(m_paiBuildingReplaced != NULL);

	for(int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iJ).isReplaceBuildingClass(GC.getBuildingInfo(eBuilding).getBuildingClassType()))
		{
			//	During modifier recalculation don't count extant buildings we haven't yet
			//	processed as already being present
			bool bHad = (m_recalcBuilding >= iJ && getNumBuilding((BuildingTypes)iJ) > 0);

			if ( bAdd )
			{
				m_paiBuildingReplaced[iJ]++;
			}
			else
			{
				//	During recalculation after loading an old format save game
				//	this can go negative due to chains that lead from lower numbered
				//	buildings to higher, so just cap at 0 on the bottom
				if ( m_paiBuildingReplaced[iJ] > 0 )
				{
					m_paiBuildingReplaced[iJ]--;
				}
			}

			bool bHas = (m_recalcBuilding >= iJ && getNumBuilding((BuildingTypes)iJ) > 0);

			if ( bHad != bHas )
			{
				processBuilding((BuildingTypes)iJ, bHas ? 1 : -1, false, true);
			}
		}
	}
}

int CvCity::getNumBuilding(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex != NO_BUILDING, "BuildingType eIndex is expected to not be NO_BUILDING");

	if ( m_paiBuildingReplaced == NULL )
	{
		calculateBuildingReplacements();
	}

	if ( m_paiBuildingReplaced[eIndex] > 0 )
	{
		return 0;
	}
	else
	{
		return std::min(GC.getCITY_MAX_NUM_BUILDINGS(), getNumRealBuilding(eIndex) + getNumFreeBuilding(eIndex) + getNumFreeAreaBuilding(eIndex));
	}
}


int CvCity::getNumActiveBuilding(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex != NO_BUILDING, "BuildingType eIndex is expected to not be NO_BUILDING");

	if (GET_TEAM(getTeam()).isObsoleteBuilding(eIndex))
	{
		return 0;
	}

	return (getNumBuilding(eIndex));
}


bool CvCity::hasActiveWorldWonder() const
{
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())))
		{
			if (getNumRealBuilding((BuildingTypes)iI) > 0 && !(GET_TEAM(getTeam()).isObsoleteBuilding((BuildingTypes)iI)))
			{
				return true;
			}
		}
	}

	return false;
}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/04/10                     Mongoose & jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
// From Mongoose SDK
int CvCity::getNumActiveWorldWonders() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())))
		{
			if (getNumRealBuilding((BuildingTypes)iI) > 0 && !(GET_TEAM(getTeam()).isObsoleteBuilding((BuildingTypes)iI)))
			{
				iCount++;
			}
		}
	}

	return iCount;
}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/


int CvCity::getReligionCount() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isHasReligion((ReligionTypes)iI))
		{
			iCount++;
		}
	}

	return iCount;
}

int CvCity::getCorporationCount() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isHasCorporation((CorporationTypes)iI))
		{
			iCount++;
		}
	}

	return iCount;
}


int CvCity::getID() const
{
	return m_iID;
}


int CvCity::getIndex() const
{
	return (getID() & FLTA_INDEX_MASK);
}


IDInfo CvCity::getIDInfo() const
{
	IDInfo city(getOwnerINLINE(), getID());
	return city;
}


void CvCity::setID(int iID)
{
	m_iID = iID;
}


int CvCity::getViewportX() const
{
	CvViewport*	pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);
	FAssert(isInViewport());

	return pCurrentViewPort->getViewportXFromMapX(m_iX);
}


int CvCity::getViewportY() const
{
	CvViewport*	pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);
	FAssert(isInViewport());

	return pCurrentViewPort->getViewportYFromMapY(m_iY);
}

bool CvCity::isInViewport() const
{
	return GC.getCurrentViewport()->isInViewport(m_iX, m_iY);
}

int CvCity::getX() const
{
	return m_iX;
}


int CvCity::getY() const
{
	return m_iY;
}


bool CvCity::at(int iX,  int iY) const
{
	return ((getX_INLINE() == iX) && (getY_INLINE() == iY));
}


bool CvCity::at(CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvCity::plot() const
{
	return GC.getMapINLINE().plotSorenINLINE(getX_INLINE(), getY_INLINE());
}

CvPlot* CvCity::plotExternal() const
{
	FAssert(isInViewport());
	return GC.getMapINLINE().plotSorenINLINE(getX_INLINE(), getY_INLINE());
}


CvPlotGroup* CvCity::plotGroup(PlayerTypes ePlayer) const
{
	return plot()->getPlotGroup(ePlayer);
}


bool CvCity::isConnectedTo(CvCity* pCity) const
{
	return plot()->isConnectedTo(pCity);
}


bool CvCity::isConnectedToCapital(PlayerTypes ePlayer) const
{
	return plot()->isConnectedToCapital(ePlayer);
}


int CvCity::getArea() const
{
	return plot()->getArea();
}

CvArea* CvCity::area() const
{
	return plot()->area();
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/02/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
CvArea* CvCity::waterArea(bool bNoImpassable) const
{
	return plot()->waterArea(bNoImpassable);
}

// Expose plot function through city
CvArea* CvCity::secondWaterArea() const
{
	return plot()->secondWaterArea();
}

// Find the largest water area shared by this city and other city, if any
CvArea* CvCity::sharedWaterArea(CvCity* pOtherCity) const
{
	CvArea* pWaterArea = waterArea(true);
	if( pWaterArea != NULL )
	{
		CvArea* pOtherWaterArea = pOtherCity->waterArea(true);
		if( pOtherWaterArea != NULL )
		{
			if( pWaterArea == pOtherWaterArea )
			{
				return pWaterArea;
			}
			else
			{
				CvArea* pSecondWaterArea = secondWaterArea();
				CvArea* pOtherSecondWaterArea = pOtherCity->secondWaterArea();

				if( pSecondWaterArea != NULL && pSecondWaterArea == pOtherWaterArea )
				{
					return pSecondWaterArea;
				}
				else if( pOtherSecondWaterArea != NULL && pWaterArea == pOtherSecondWaterArea )
				{
					return pWaterArea;
				}
				else if( pSecondWaterArea != NULL && pOtherSecondWaterArea != NULL && pSecondWaterArea == pOtherSecondWaterArea )
				{
					return pSecondWaterArea;
				}
			}
		}
	}

	return NULL;
}

bool CvCity::isBlockaded() const
{
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if( pAdjacentPlot->getBlockadedCount(getTeam()) > 0 )
			{
				return true;
			}
		}
	}

	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

CvPlot* CvCity::getRallyPlot() const
{
	return GC.getMapINLINE().plotSorenINLINE(m_iRallyX, m_iRallyY);
}


void CvCity::setRallyPlot(CvPlot* pPlot)
{
	if (getRallyPlot() != pPlot)
	{
		if (pPlot != NULL)
		{
			m_iRallyX = pPlot->getX_INLINE();
			m_iRallyY = pPlot->getY_INLINE();
		}
		else
		{
			m_iRallyX = INVALID_PLOT_COORD;
			m_iRallyY = INVALID_PLOT_COORD;
		}

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
	}
}


int CvCity::getGameTurnFounded() const
{
	return m_iGameTurnFounded;
}


void CvCity::setGameTurnFounded(int iNewValue)
{
	if (getGameTurnFounded() != iNewValue)
	{
		m_iGameTurnFounded = iNewValue;
		FAssert(getGameTurnFounded() >= 0);

		GC.getMapINLINE().updateWorkingCity();
	}
}


int CvCity::getGameTurnAcquired() const
{
	return m_iGameTurnAcquired;
}


void CvCity::setGameTurnAcquired(int iNewValue)
{
	m_iGameTurnAcquired = iNewValue;
	FAssert(getGameTurnAcquired() >= 0);
}


int CvCity::getPopulation() const
{
	return m_iPopulation;
}


void CvCity::setPopulation(int iNewValue)
{
	int iOldPopulation = getPopulation();

	if (iOldPopulation != iNewValue)
	{
		m_iPopulation = iNewValue;

		FAssert(getPopulation() >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidatePopulationRankCache();

		if (getPopulation() > getHighestPopulation())
		{
			setHighestPopulation(getPopulation());
		}

		area()->changePopulationPerPlayer(getOwnerINLINE(), (getPopulation() - iOldPopulation));
		GET_PLAYER(getOwnerINLINE()).changeTotalPopulation(getPopulation() - iOldPopulation);
		GET_TEAM(getTeam()).changeTotalPopulation(getPopulation() - iOldPopulation);
		GC.getGameINLINE().changeTotalPopulation(getPopulation() - iOldPopulation);

		if (iOldPopulation > 0)
		{
			area()->changePower(getOwnerINLINE(), -(getPopulationPower(iOldPopulation)));
		}
		if (getPopulation() > 0)
		{
			area()->changePower(getOwnerINLINE(), getPopulationPower(getPopulation()));
		}
/************************************************************************************************/
/* Afforess	                  Start		 08/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		checkBuildings(false, false, false, false, true);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	
		if (plot()->getFeatureType() != NO_FEATURE)
		{
			int iPopDestroys = GC.getFeatureInfo(plot()->getFeatureType()).getPopDestroys();
			if (iPopDestroys != -1 && getPopulation() >= iPopDestroys)
			{
				plot()->setFeatureType(NO_FEATURE);
			}
		}

		plot()->updateYield();
		
		updateFeatureHealth();
		setMaintenanceDirty(true);

		if (((iOldPopulation == 1) && (getPopulation() > 1)) ||
			  ((getPopulation() == 1) && (iOldPopulation > 1))
			  || ((getPopulation() > iOldPopulation) && (GET_PLAYER(getOwnerINLINE()).getNumCities() <= 2)))
		{
			if (!isHuman())
			{
				AI_setChooseProductionDirty(true);
			}
		}

		GET_PLAYER(getOwnerINLINE()).AI_makeAssignWorkDirty();

		setInfoDirty(true);
		setLayoutDirty(true);

		plot()->plotAction(PUF_makeInfoBarDirty);

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		}

		//updateGenericBuildings();
	}
}


void CvCity::changePopulation(int iChange)
{
	setPopulation(getPopulation() + iChange);
}


long CvCity::getRealPopulation() const
{
	//return (((long)(pow((float)getPopulation(), 2.8f))) * 1000);

	//	Koshling - using table provided by Praetyre to give more realistic results
	static long	realPopulationTable[] =
	{
		0,
		50,
		100,
		250,
		500,
		750,
		1000,
		1500,
		2000,
		2500,
		3000,
		4000,
		5000,
		7500,
		10000,
		15000,
		20000,
		30000,
		40000,
		50000,
		60000,
		70000,
		80000,
		90000,
		100000,
		125000,
		150000,
		175000,
		200000,
		225000,
		250000,
		300000,
		350000,
		400000,
		450000,
		500000,
		550000,
		600000,
		650000,
		700000,
		750000,
		800000,
		850000,
		900000,
		950000,
		1000000,
		1100000,
		1200000,
		1300000,
		1400000,
		1500000,
		1600000,
		1700000,
		1800000,
		1900000,
		2000000,
		2250000,
		2500000,
		2750000,
		3000000,
		3500000,
		4000000,
		5000000,
		6000000,
		7000000,
		8000000,
		9000000,
		10000000,
		12000000,
		14000000,
		16000000,
		18000000,
		20000000,
		22250000,
		25000000,
		27500000,
		30000000,
		35000000,
		40000000,
		45000000,
		50000000,
		55000000,
		60000000,
		65000000,
		70000000,
		75000000,
		80000000,
		85000000,
		90000000,
		95000000,
		100000000,
		110000000,
		120000000,
		130000000,
		140000000,
		150000000,
		160000000,
		170000000,
		180000000,
		190000000,
		200000000
	};
#define NUM_POP_TABLE_ENTRIES (sizeof(realPopulationTable)/sizeof(long))

	if ( getPopulation() < NUM_POP_TABLE_ENTRIES )
	{
		return realPopulationTable[getPopulation()];
	}
	else
	{
		return realPopulationTable[NUM_POP_TABLE_ENTRIES-1] + ((realPopulationTable[NUM_POP_TABLE_ENTRIES-1]-realPopulationTable[NUM_POP_TABLE_ENTRIES-2])*(getPopulation() - NUM_POP_TABLE_ENTRIES));
	}
}

int CvCity::getHighestPopulation() const
{
	return m_iHighestPopulation;
}


void CvCity::setHighestPopulation(int iNewValue)
{
	m_iHighestPopulation = iNewValue;
	FAssert(getHighestPopulation() >= 0);
}


int CvCity::getWorkingPopulation() const
{
	return m_iWorkingPopulation;
}


void CvCity::changeWorkingPopulation(int iChange)														
{
	m_iWorkingPopulation = (m_iWorkingPopulation + iChange);
	FAssert(getWorkingPopulation() >= 0);
}


int CvCity::getSpecialistPopulation() const
{
	return m_iSpecialistPopulation;
}


void CvCity::changeSpecialistPopulation(int iChange)													
{
	if (iChange != 0)
	{
		m_iSpecialistPopulation = (m_iSpecialistPopulation + iChange);
		FAssert(getSpecialistPopulation() >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache();

		setCommerceDirty(NO_COMMERCE);
	}
}


int CvCity::getNumGreatPeople() const
{
	return m_iNumGreatPeople;
}


void CvCity::changeNumGreatPeople(int iChange)															
{
	if (iChange != 0)
	{
		m_iNumGreatPeople = (m_iNumGreatPeople + iChange);
		FAssert(getNumGreatPeople() >= 0);

		setCommerceDirty(NO_COMMERCE);
	}
}


int CvCity::getBaseGreatPeopleRate() const
{
	return m_iBaseGreatPeopleRate + GET_PLAYER(getOwnerINLINE()).getNationalGreatPeopleRate();
}


int CvCity::getGreatPeopleRate() const
{
	if (isDisorder())
	{
		return 0;
	}

	return ((getBaseGreatPeopleRate() * getTotalGreatPeopleRateModifier()) / 100);
}


int CvCity::getTotalGreatPeopleRateModifier() const
{
	int iModifier = getGreatPeopleRateModifier();
	iModifier += GET_PLAYER(getOwnerINLINE()).getGreatPeopleRateModifier();

	if (GET_PLAYER(getOwnerINLINE()).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(getOwnerINLINE()).getStateReligion()))
		{
			iModifier += GET_PLAYER(getOwnerINLINE()).getStateReligionGreatPeopleRateModifier();
		}
	}

	if (GET_PLAYER(getOwnerINLINE()).isGoldenAge())
	{
		iModifier += GC.getDefineINT("GOLDEN_AGE_GREAT_PEOPLE_MODIFIER");
	}

	return std::max(0, (iModifier + 100));
}


void CvCity::changeBaseGreatPeopleRate(int iChange)
{
	m_iBaseGreatPeopleRate = (m_iBaseGreatPeopleRate + iChange);
	FAssert(getBaseGreatPeopleRate() >= 0);
}


int CvCity::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


void CvCity::changeGreatPeopleRateModifier(int iChange)
{
	m_iGreatPeopleRateModifier = (m_iGreatPeopleRateModifier + iChange);
}


int CvCity::getGreatPeopleProgress() const
{
	return m_iGreatPeopleProgress;
}


void CvCity::changeGreatPeopleProgress(int iChange)
{
	m_iGreatPeopleProgress = (m_iGreatPeopleProgress + iChange);
	FAssert(getGreatPeopleProgress() >= 0);
}

CvProperties* CvCity::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvCity::getPropertiesConst() const
{
	return &m_Properties;
}


// BUG - Building Additional Great People - start
/*
 * Returns the total additional great people rate that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalGreatPeopleRateByBuilding(BuildingTypes eBuilding)
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	int iRate = getBaseGreatPeopleRate();
	int iModifier = getTotalGreatPeopleRateModifier();
	int iExtra = ((iRate + getAdditionalBaseGreatPeopleRateByBuilding(eBuilding)) * (iModifier + getAdditionalGreatPeopleRateModifierByBuilding(eBuilding)) / 100) - (iRate * iModifier / 100);
/************************************************************************************************/
/* Afforess	                  Start		 4/22/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			//Team Project (5)
			if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				if (GC.getBuildingInfo((BuildingTypes)iI).isReplaceBuildingClass((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()))
				{
					iExtra -= getAdditionalGreatPeopleRateByBuilding((BuildingTypes)iI);
				}
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	return iExtra;
}

/*
 * Returns the additional great people rate that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalBaseGreatPeopleRateByBuilding(BuildingTypes eBuilding)
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	bool bObsolete = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	int iExtraRate = 0;

	iExtraRate += kBuilding.getGreatPeopleRateChange();

	// Specialists
	if (!bObsolete)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
		{
			if (kBuilding.getFreeSpecialistCount((SpecialistTypes)iI) != 0)
			{
				iExtraRate += getAdditionalBaseGreatPeopleRateBySpecialist((SpecialistTypes)iI, kBuilding.getFreeSpecialistCount((SpecialistTypes)iI));
			}
		}
	}
/************************************************************************************************/
/* Afforess	                  Start		 08/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iSpecialistGreatPeopleRate = 0;

	for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
	{
		SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;
		CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eNewSpecialist);

		iSpecialistGreatPeopleRate += kSpecialist.getGreatPeopleRateChange();

	}
	iExtraRate += iSpecialistGreatPeopleRate;
	
	if (kBuilding.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
		iExtraRate += iGreatPeopleRate;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	


	return iExtraRate;
}

/*
 * Returns the additional great people rate modifier that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalGreatPeopleRateModifierByBuilding(BuildingTypes eBuilding)
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	bool bObsolete = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	int iExtraModifier = 0;

	if (!bObsolete)
	{
		iExtraModifier += kBuilding.getGreatPeopleRateModifier();
		iExtraModifier += kBuilding.getGlobalGreatPeopleRateModifier();
	}

	return iExtraModifier;
}
// BUG - Building Additional Great People - end


// BUG - Specialist Additional Great People - start
/*
 * Returns the total additional great people rate that changing the number of the given specialist will provide/remove.
 */
int CvCity::getAdditionalGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const
{
	int iRate = getBaseGreatPeopleRate();
	int iModifier = getTotalGreatPeopleRateModifier();
	int iExtraRate = getAdditionalBaseGreatPeopleRateBySpecialist(eSpecialist, iChange);

	int iExtra = ((iRate + iExtraRate) * iModifier / 100) - (iRate * iModifier / 100);

	return iExtra;
}

/*
 * Returns the additional great people rate that changing the number of the given specialist will provide/remove.
 */
int CvCity::getAdditionalBaseGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	return iChange * GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();
}
// BUG - Specialist Additional Great People - end


int CvCity::getNumWorldWonders() const
{
	return m_iNumWorldWonders;
}


void CvCity::changeNumWorldWonders(int iChange)
{
	m_iNumWorldWonders = (m_iNumWorldWonders + iChange);
	FAssert(getNumWorldWonders() >= 0);
}


int CvCity::getNumTeamWonders() const
{
	return m_iNumTeamWonders;
}


void CvCity::changeNumTeamWonders(int iChange)
{
	m_iNumTeamWonders = (m_iNumTeamWonders + iChange);
	FAssert(getNumTeamWonders() >= 0);
}


int CvCity::getNumNationalWonders() const
{
	return m_iNumNationalWonders;
}


void CvCity::changeNumNationalWonders(int iChange)
{
	m_iNumNationalWonders = (m_iNumNationalWonders + iChange);
	FAssert(getNumNationalWonders() >= 0);
}


int CvCity::getNumBuildings() const
{
	return m_iNumBuildings;
}


void CvCity::changeNumBuildings(int iChange)
{
	m_iNumBuildings = (m_iNumBuildings + iChange);
	FAssert(getNumBuildings() >= 0);
}


int CvCity::getGovernmentCenterCount() const																
{
	return m_iGovernmentCenterCount;
}


bool CvCity::isGovernmentCenter() const
{
	return (getGovernmentCenterCount() > 0);
}


void CvCity::changeGovernmentCenterCount(int iChange)
{
	if (iChange != 0)
	{
		m_iGovernmentCenterCount = (m_iGovernmentCenterCount + iChange);
		FAssert(getGovernmentCenterCount() >= 0);

		setMaintenanceDirty(true);
	}
}


// BUG - Building Saved Maintenance - start
/*
 * Returns the rounded total additional gold from saved maintenance that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getSavedMaintenanceByBuilding(BuildingTypes eBuilding) const
{
	return getSavedMaintenanceTimes100ByBuilding(eBuilding) / 100;
}

/*
 * Returns the total additional gold from saved maintenance times 100 that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getSavedMaintenanceTimes100ByBuilding(BuildingTypes eBuilding) const
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iModifier = kBuilding.getMaintenanceModifier();
	int iDirectMaintenance = 0;
	iModifier += kBuilding.getGlobalMaintenanceModifier();
	iModifier += kBuilding.getAreaMaintenanceModifier();
	if (isConnectedToCapital() && !(isCapital()))
	{
		iModifier += kBuilding.getConnectedCityMaintenanceModifier();
	}
	if ( kBuilding.getCommerceChange(COMMERCE_GOLD) < 0 && GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE"))
	{
		iDirectMaintenance = -kBuilding.getCommerceChange(COMMERCE_GOLD);
		iDirectMaintenance *= GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent();
	}
	if ((iModifier != 0 || iDirectMaintenance > 0) && !isDisorder() && !isWeLoveTheKingDay())
	{
		//	Koshling - the calculation below was incorrect because calculateBaseMaintenanceTimes100() itself includes
		//	num cities and distance maintainence effects
		int iOldBaseMaintenance = calculateBaseMaintenanceTimes100();
		int iNewBaseMaintenance = iOldBaseMaintenance + iDirectMaintenance;
		int iOldModifiedBaseMaintenance = std::max(0, iOldBaseMaintenance * (getEffectiveMaintenanceModifier() + 100) / 100);
		int iNewModifiedBaseMaintenance = std::max(0, iNewBaseMaintenance * (getEffectiveMaintenanceModifier() + iModifier + 100) / 100);
		return iOldModifiedBaseMaintenance - iNewModifiedBaseMaintenance;
		//int iNumCitiesMaintenanceTimes100 = calculateNumCitiesMaintenanceTimes100();
		//iNumCitiesMaintenanceTimes100 *= std::max(0, 100 + kBuilding.getNumCitiesMaintenanceModifier());
		//iNumCitiesMaintenanceTimes100 /= 100;
		//iNumCitiesMaintenanceTimes100 -= calculateNumCitiesMaintenanceTimes100();
		//iNewMaintenance += iNumCitiesMaintenanceTimes100;
		//iNewMaintenance += calculateDistanceMaintenanceTimes100(0, 0) - calculateDistanceMaintenanceTimes100(kBuilding.getDistanceMaintenanceModifier(), kBuilding.getCoastalDistanceMaintenanceModifier());
/************************************************************************************************/
/* Afforess	                  Start		 6/20/11                                                */
/*                                                                                              */
/*   Protect against negative maintenance                                                       */
/************************************************************************************************/
		//return getMaintenanceTimes100() - std::max(0, iNewMaintenance);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}

	return 0;
}
// BUG - Building Saved Maintenance - end

int CvCity::getDistanceMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FAssertMsg(eCivic >= 0, "eCivic expected to be >= 0");
	FAssertMsg(eCivic < GC.getNumCivicInfos(), "eBuilding expected to be < GC.getNumCivicInfos()");

	CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	int iModifier = kCivic.getDistanceMaintenanceModifier();

	if (iModifier != 0 && !isDisorder() && !isWeLoveTheKingDay())
	{
		int iOldBaseMaintenance = calculateDistanceMaintenanceTimes100();
		int iNewBaseMaintenance = calculateDistanceMaintenanceTimes100(iModifier);
		int iOldModifiedBaseMaintenance = std::max(0, iOldBaseMaintenance * (getEffectiveMaintenanceModifier() + 100) / 100);
		int iNewModifiedBaseMaintenance = std::max(0, iNewBaseMaintenance * (getEffectiveMaintenanceModifier() + 100) / 100);
		return iOldModifiedBaseMaintenance - iNewModifiedBaseMaintenance;
	}

	return 0;
}

int CvCity::getNumCitiesMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FAssertMsg(eCivic >= 0, "eCivic expected to be >= 0");
	FAssertMsg(eCivic < GC.getNumCivicInfos(), "eBuilding expected to be < GC.getNumCivicInfos()");

	CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	int iModifier = kCivic.getNumCitiesMaintenanceModifier();

	if (iModifier != 0 && !isDisorder() && !isWeLoveTheKingDay())
	{
		int iOldBaseMaintenance = calculateNumCitiesMaintenanceTimes100();
		int iNewBaseMaintenance = calculateNumCitiesMaintenanceTimes100(iModifier);
		int iOldModifiedBaseMaintenance = std::max(0, iOldBaseMaintenance * (getEffectiveMaintenanceModifier() + 100) / 100);
		int iNewModifiedBaseMaintenance = std::max(0, iNewBaseMaintenance * (getEffectiveMaintenanceModifier() + 100) / 100);
		return iOldModifiedBaseMaintenance - iNewModifiedBaseMaintenance;
	}

	return 0;
}

int CvCity::getHomeAreaMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FAssertMsg(eCivic >= 0, "eCivic expected to be >= 0");
	FAssertMsg(eCivic < GC.getNumCivicInfos(), "eBuilding expected to be < GC.getNumCivicInfos()");

	CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	int iModifier = kCivic.getHomeAreaMaintenanceModifier();

	if (iModifier != 0 && area()->isHomeArea(getOwnerINLINE()) &&  !isDisorder() && !isWeLoveTheKingDay())
	{
		int iOldBaseMaintenance = calculateBaseMaintenanceTimes100();
		int iOldModifiedBaseMaintenance = std::max(0, iOldBaseMaintenance * (getEffectiveMaintenanceModifier() + 100) / 100);
		int iNewModifiedBaseMaintenance = std::max(0, iOldBaseMaintenance * (getEffectiveMaintenanceModifier() + iModifier + 100) / 100);
		return iOldModifiedBaseMaintenance - iNewModifiedBaseMaintenance;
	}

	return 0;
}

int CvCity::getOtherAreaMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FAssertMsg(eCivic >= 0, "eCivic expected to be >= 0");
	FAssertMsg(eCivic < GC.getNumCivicInfos(), "eBuilding expected to be < GC.getNumCivicInfos()");

	CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	int iModifier = kCivic.getOtherAreaMaintenanceModifier();

	if (iModifier != 0 && !area()->isHomeArea(getOwnerINLINE()) && !isDisorder() && !isWeLoveTheKingDay())
	{
		int iOldBaseMaintenance = calculateBaseMaintenanceTimes100();
		int iOldModifiedBaseMaintenance = std::max(0, iOldBaseMaintenance * (getEffectiveMaintenanceModifier() + 100) / 100);
		int iNewModifiedBaseMaintenance = std::max(0, iOldBaseMaintenance * (getEffectiveMaintenanceModifier() + iModifier + 100) / 100);
		return iOldModifiedBaseMaintenance - iNewModifiedBaseMaintenance;
	}

	return 0;
}

int CvCity::getMaintenance() const
{
	if ( m_bMaintenanceDirty )
	{
		updateMaintenance();
	}

	return m_iMaintenance / 100;
}

int CvCity::getMaintenanceTimes100() const
{
	if ( m_bMaintenanceDirty )
	{
		updateMaintenance();
	}

	return m_iMaintenance;
}

int CvCity::getEffectiveMaintenanceModifier() const
{
	int iModifier = getMaintenanceModifier() + GET_PLAYER(getOwnerINLINE()).getMaintenanceModifier() + area()->getTotalAreaMaintenanceModifier(GET_PLAYER(getOwnerINLINE()).getID());

	if (isConnectedToCapital() && !(isCapital()))
	{
		iModifier += GET_PLAYER(getOwnerINLINE()).getConnectedCityMaintenanceModifier();
	}

	return iModifier;
}

void CvCity::setMaintenanceDirty(bool bDirty) const
{
	m_bMaintenanceDirty = bDirty;
	if (bDirty)
	{
		GET_PLAYER(getOwnerINLINE()).setMaintenanceDirty(true);
	}
}

void CvCity::updateMaintenance() const
{
	setMaintenanceDirty(false);

	int iOldMaintenance = getMaintenanceTimes100();

	int iNewMaintenance = 0;
/************************************************************************************************/
/* Afforess	                  Start		 09/12/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iEraMaintanceTimes100 = GC.getEraInfo(GET_PLAYER(getOwnerINLINE()).getCurrentEra()).getInitialCityMaintenancePercent();
//	iEraMaintanceTimes100 -= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getInitialCityMaintenancePercent();
	iNewMaintenance += std::max(0, iEraMaintanceTimes100);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (!isDisorder() && !isWeLoveTheKingDay() && (getPopulation() > 0))
	{
		//DPII < Maintenance Modifiers >
		int iModifier = getEffectiveMaintenanceModifier();
		iNewMaintenance += (calculateBaseMaintenanceTimes100() * std::max(0, (iModifier + 100))) / 100;
		//DPII < Maintenance Modifiers >
	}
/************************************************************************************************/
/* REVOLUTION_MOD                         01/31/08                                jdog5000      */
/*                                                                                              */
/* Rebels pay less maintenance                                                                  */
/************************************************************************************************/
	if( GET_PLAYER(getOwnerINLINE()).isRebel() )
	{
		iNewMaintenance /= 2;
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	if (iOldMaintenance != iNewMaintenance)
	{
		FAssert(iOldMaintenance >= 0);
		FAssert(iNewMaintenance >= 0);

		m_iMaintenance = iNewMaintenance;
		FAssert(getMaintenance() >= 0);
/************************************************************************************************/
/* Afforess	                  Start		 6/20/11                                                */
/*                                                                                              */
/*   Protect against negative maintenance                                                       */
/************************************************************************************************/
		m_iMaintenance = std::max(0, m_iMaintenance);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}
}

int CvCity::calculateDistanceMaintenance() const
{
	return (calculateDistanceMaintenanceTimes100() / 100);
}
/************************************************************************************************/
/* Afforess	                  Start		 09/07/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvCity::calculateDistanceMaintenanceTimes100(int iExtraDistanceModifier, int iExtraCoastalDistanceModifier) const
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
{
	int iWorstCityMaintenance = 0;
	int iBestCapitalMaintenance = MAX_INT;

	int iLoop = 0;
	for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		int iTempMaintenance = 100 * (GC.getDefineINT("MAX_DISTANCE_CITY_MAINTENANCE") * plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE()));

		iTempMaintenance *= (getPopulation() + 7);
		iTempMaintenance /= 10;
		
/************************************************************************************************/
/* Afforess	                  Start		 09/08/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		iTempMaintenance *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getDistanceMaintenanceModifier() + iExtraDistanceModifier + 100));
		iTempMaintenance /= 100;

		if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			iTempMaintenance *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getCoastalDistanceMaintenanceModifier() + iExtraCoastalDistanceModifier + 100));
			iTempMaintenance /= 100;
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		iTempMaintenance *= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getDistanceMaintenancePercent();
		iTempMaintenance /= 100;

		iTempMaintenance *= GC.getHandicapInfo(getHandicapType()).getDistanceMaintenancePercent();
		iTempMaintenance /= 100;

		iTempMaintenance /= GC.getMapINLINE().maxPlotDistance();

		//	To cope with the AI getting 2 starting cities on deity we greatly reduce
		//	distance maintenance for the AI until it builds its third city
		if ( !isHuman() )
		{
			if (GET_PLAYER(getOwnerINLINE()).getNumCities() < 3)
			{
				iTempMaintenance /= 3;
			}
		}

		iWorstCityMaintenance = std::max(iWorstCityMaintenance, iTempMaintenance);

		if (pLoopCity->isGovernmentCenter())
		{
			iBestCapitalMaintenance = std::min(iBestCapitalMaintenance, iTempMaintenance);
		}
	}

	int finalMaint = std::min(iWorstCityMaintenance, iBestCapitalMaintenance);
	FAssert(finalMaint >= 0);

	return finalMaint;
}

int CvCity::calculateNumCitiesMaintenance() const
{
	return (calculateNumCitiesMaintenanceTimes100() / 100);
}

int CvCity::calculateNumCitiesMaintenanceTimes100(int iExtraModifier) const
{
	int iNumCitiesPercent = 100;

	iNumCitiesPercent *= (getPopulation() + 17);
	iNumCitiesPercent /= 18;

	iNumCitiesPercent *= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getNumCitiesMaintenancePercent();
	iNumCitiesPercent /= 100;

	iNumCitiesPercent *= GC.getHandicapInfo(getHandicapType()).getNumCitiesMaintenancePercent();
	iNumCitiesPercent /= 100;

	int iNumVassalCities = 0;
	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; iPlayer++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.getTeam() != getTeam() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
		{
			iNumVassalCities += kLoopPlayer.getNumCities();
		}
	}
	iNumVassalCities /= std::max(1, GET_TEAM(getTeam()).getNumMembers());

	int iNumCitiesUsed = GET_PLAYER(getOwnerINLINE()).getNumCities();
	if ( !isHuman() && iNumCitiesUsed > 1 )
	{
		//	Koshling - count one less city for the AI - this is primarily to allow
		//	for the AI having 2 cities from the start on deity, but gives it a slight
		//	boost throughout
		iNumCitiesUsed--;
	}

	int iNumCitiesMaintenance = (iNumCitiesUsed + iNumVassalCities) * iNumCitiesPercent;

	if (GC.getHandicapInfo(getHandicapType()).getMaxNumCitiesMaintenance() > -1)
	{
		iNumCitiesMaintenance = std::min(iNumCitiesMaintenance, GC.getHandicapInfo(getHandicapType()).getMaxNumCitiesMaintenance() * 100);
	}

	iNumCitiesMaintenance *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getNumCitiesMaintenanceModifier() + iExtraModifier +  100));
	iNumCitiesMaintenance /= 100;

	FAssert(iNumCitiesMaintenance >= 0);

	return iNumCitiesMaintenance;
}


int CvCity::calculateColonyMaintenance() const
{
	return (calculateColonyMaintenanceTimes100() / 100);
}

int CvCity::calculateColonyMaintenanceTimes100() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_VASSAL_STATES))
	{
		return 0;
	}

	CvCity* pCapital = GET_PLAYER(getOwnerINLINE()).getCapitalCity();
	if (pCapital && pCapital->area() == area())
	{
		return 0;
	}

	int iNumCitiesPercent = 100;

	iNumCitiesPercent *= (getPopulation() + 17);
	iNumCitiesPercent /= 18;

	iNumCitiesPercent *= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getColonyMaintenancePercent();
	iNumCitiesPercent /= 100;

	iNumCitiesPercent *= GC.getHandicapInfo(getHandicapType()).getColonyMaintenancePercent();
	iNumCitiesPercent /= 100;

	int iNumCities = (area()->getCitiesPerPlayer(getOwnerINLINE()) - 1) * iNumCitiesPercent;
	
	int iMaintenance = (iNumCities * iNumCities) / 100;

	iMaintenance = std::min(iMaintenance, (GC.getHandicapInfo(getHandicapType()).getMaxColonyMaintenance() * calculateDistanceMaintenanceTimes100()) / 100);

	FAssert(iMaintenance >= 0);

	return iMaintenance;
}


int CvCity::calculateCorporationMaintenance() const
{
	return (calculateCorporationMaintenanceTimes100() / 100);
}

int CvCity::calculateCorporationMaintenanceTimes100() const
{
	int iMaintenance = 0;

	for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); ++iCorporation)
	{
		if (isActiveCorporation((CorporationTypes)iCorporation))
		{
			iMaintenance += calculateCorporationMaintenanceTimes100((CorporationTypes)iCorporation);
		}
	}

	return iMaintenance;
}

int CvCity::calculateCorporationMaintenanceTimes100(CorporationTypes eCorporation) const
{
	int iMaintenance = 0;

	for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
	{
		iMaintenance += 100 * GC.getCorporationInfo(eCorporation).getHeadquarterCommerce(iCommerce);
	}

	int iNumBonuses = 0;
	for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
	{
		BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(i);
		if (NO_BONUS != eBonus)
		{
			iNumBonuses += getNumBonuses(eBonus);
		}
	}

	int iBonusMaintenance = GC.getCorporationInfo(eCorporation).getMaintenance() * iNumBonuses;
	iBonusMaintenance *= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getCorporationMaintenancePercent();
	iBonusMaintenance /= 100;
	iMaintenance += iBonusMaintenance;

	iMaintenance *= (getPopulation() + 17);
	iMaintenance /= 18;

	iMaintenance *= GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent();
	iMaintenance /= 100;

	iMaintenance *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getCorporationMaintenanceModifier() + 100));
	iMaintenance /= 100;

	int iInflation = GET_PLAYER(getOwnerINLINE()).calculateInflationRate() + 100;
	if (iInflation > 0)
	{
		iMaintenance *= 100;
		iMaintenance /= iInflation;
	}
/************************************************************************************************/
/* Afforess	                  Start		 06/17/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iMaintenance *= (GET_TEAM(getTeam()).getCorporationMaintenanceModifier() + 100);
	iMaintenance /= 100;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 06/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_REALISTIC_CORPORATIONS))
	{
		if (GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent() > 80)
		{
			iMaintenance *= (GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent());
			iMaintenance /= 80;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	FAssert(iMaintenance >= 0);
	
	return iMaintenance;
}

int CvCity::calculateBuildingMaintenanceTimes100() const
{
	if ( GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE") )
	{
		int iResult = 0;

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			BuildingTypes eBuilding = (BuildingTypes)iI;
			if (getNumActiveBuilding(eBuilding) > 0)
			{
				//Team Project (5)
				if (!isReligiouslyDisabledBuilding(eBuilding))
				{
					int iBaseCommerceChange = GC.getBuildingInfo(eBuilding).getCommerceChange(COMMERCE_GOLD);

					if ( iBaseCommerceChange < 0 )
					{
						iResult -= iBaseCommerceChange*100;
					}
				}
			}
		}

		iResult = (GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent() * iResult) / 100;
		return iResult;
	}
	else
	{
		return 0;
	}
}

int CvCity::calculateBaseMaintenanceTimes100() const
{
	return (calculateBuildingMaintenanceTimes100() +calculateDistanceMaintenanceTimes100() + calculateNumCitiesMaintenanceTimes100() + calculateColonyMaintenanceTimes100() + calculateCorporationMaintenanceTimes100());
}


int CvCity::getMaintenanceModifier() const
{
	return m_iMaintenanceModifier;
}


void CvCity::changeMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iMaintenanceModifier = (m_iMaintenanceModifier + iChange);

		setMaintenanceDirty(true);
	}
}


int CvCity::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}


void CvCity::changeWarWearinessModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iWarWearinessModifier = (m_iWarWearinessModifier + iChange);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getHurryAngerModifier() const
{
	int iTotalHurryAngerModifier = m_iHurryAngerModifier;
	iTotalHurryAngerModifier += GET_PLAYER(getOwnerINLINE()).getNationalHurryAngerModifier();
	// AIAndy: This used to return m_iHurryAngerModifier instead of the calculated iTotalHurryAngerModifier which I don't assume is correct
	return iTotalHurryAngerModifier;
}


void CvCity::changeHurryAngerModifier(int iChange)
{
	if (0 != iChange)
	{
		int iRatio = 0;

		//	Foregiveness for crazily long hurry anger times because its was bugged in older
		//	versions!
		if (m_iHurryAngerTimer > 1000)
		{
			m_iHurryAngerTimer = 0;
		}

		if (m_iHurryAngerTimer > 0 && !GC.getGameINLINE().isRecalculatingModifiers())
		{
			iRatio = (100 * (m_iHurryAngerTimer - 1)) / std::max(1, 100 + getHurryAngerModifier());
		}

		m_iHurryAngerModifier += iChange;

		if (m_iHurryAngerTimer > 0 && !GC.getGameINLINE().isRecalculatingModifiers())
		{
			m_iHurryAngerTimer = (iRatio * std::max(1, 100 + getHurryAngerModifier())) / 100 + 1;
		}
	}
}


int CvCity::getHealRate() const
{
	return m_iHealRate;
}


void CvCity::changeHealRate(int iChange)
{
	m_iHealRate = (m_iHealRate + iChange);
	FAssert(getHealRate() >= 0);
}

int CvCity::getHealUnitCombatTypeTotal(UnitCombatTypes eUnitCombat) const
{
	FAssertMsg(eUnitCombat >= 0, "eUnitCombat expected to be >= 0");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat expected to be < GC.getNumUnitCombatInfos()");

	return m_paiHealUnitCombatTypeVolume[eUnitCombat];
}

void CvCity::changeHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombat, int iChange)
{
	FAssertMsg(eUnitCombat >= 0, "eUnitCombat expected to be >= 0");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat expected to be < GC.getNumUnitCombatInfos()");

	m_paiHealUnitCombatTypeVolume[eUnitCombat] += iChange;
}

void CvCity::setHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombat, int iChange)
{
	m_paiHealUnitCombatTypeVolume[eUnitCombat] = iChange;
}

int CvCity::getEspionageHealthCounter() const
{
/************************************************************************************************/
/* Afforess	                  Start		 06/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	return m_iEspionageHealthCounter;
*/
	return std::min(8, m_iEspionageHealthCounter);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


void CvCity::changeEspionageHealthCounter(int iChange)
{
	if (iChange != 0)
	{
		m_iEspionageHealthCounter += iChange;
	}
}

int CvCity::getEspionageHappinessCounter() const
{
/************************************************************************************************/
/* Afforess	                  Start		 06/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	return m_iEspionageHappinessCounter;
*/
	return std::min(8, m_iEspionageHappinessCounter);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


void CvCity::changeEspionageHappinessCounter(int iChange)
{
	if (iChange != 0)
	{
		m_iEspionageHappinessCounter += iChange;
	}
}


int CvCity::getFreshWaterGoodHealth() const
{
	return m_iFreshWaterGoodHealth;
}


int CvCity::getFreshWaterBadHealth() const
{
	return m_iFreshWaterBadHealth;
}

void CvCity::updateFreshWaterHealth()
{
	int iNewGoodHealth;
	int iNewBadHealth;

	iNewGoodHealth = 0;
	iNewBadHealth = 0;

	if (plot()->isFreshWater())
	{
		if (GC.getDefineINT("FRESH_WATER_HEALTH_CHANGE") > 0)
		{
			iNewGoodHealth += GC.getDefineINT("FRESH_WATER_HEALTH_CHANGE");
		}
		else
		{
			iNewBadHealth += GC.getDefineINT("FRESH_WATER_HEALTH_CHANGE");
		}
	}

	if ((getFreshWaterGoodHealth() != iNewGoodHealth) || (getFreshWaterBadHealth() != iNewBadHealth))
	{
		m_iFreshWaterGoodHealth = iNewGoodHealth;
		m_iFreshWaterBadHealth = iNewBadHealth;
		FAssert(getFreshWaterGoodHealth() >= 0);
		FAssert(getFreshWaterBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getFeatureGoodHealth() const
{
	return m_iFeatureGoodHealth;
}


int CvCity::getFeatureBadHealth() const
{
	return m_iFeatureBadHealth;
}


// BUG - Feature Health - start
/*
 * Recalculates the total percentage health effects from existing features
 * and updates the values if they have changed.
 *
 * Bad health is stored as a negative value.
 */
void CvCity::updateFeatureHealth()
{
	int iNewGoodHealth;
	int iNewBadHealth;

	iNewGoodHealth = 0;
	iNewBadHealth = 0;

	calculateFeatureHealthPercent(iNewGoodHealth, iNewBadHealth);
	iNewBadHealth = -iNewBadHealth;  // convert to "negative is bad"

	iNewGoodHealth /= 100;

	// AIAndy: Feature unhealthiness reduced for small cities
	iNewBadHealth *= std::max(std::min(getPopulation()-2, 5), 0);
	iNewBadHealth /= 500;

	if ((getFeatureGoodHealth() != iNewGoodHealth) || (getFeatureBadHealth() != iNewBadHealth))
	{
		m_iFeatureGoodHealth = iNewGoodHealth;
		m_iFeatureBadHealth = iNewBadHealth;
		FAssert(getFeatureGoodHealth() >= 0);
		FAssert(getFeatureBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

/*
 * Adds the total percentage health effects from existing features to iGood and iBad.
 *
 * Positive values for iBad mean an increase in unhealthiness.
 */
void CvCity::calculateFeatureHealthPercent(int& iGood, int& iBad) const
{
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			FeatureTypes eFeature = pLoopPlot->getFeatureType();

			if (eFeature != NO_FEATURE)
			{
				int iHealth = GC.getFeatureInfo(eFeature).getHealthPercent();

				if (iHealth > 0)
				{
					iGood += iHealth;
				}
				else
				{
					iBad -= iHealth;
				}
			}
		}
	}
}

/*
 * Subtracts the total percentage health effects of features currently being removed to iGood and iBad.
 * If pIgnorePlot is not NULL, it is not checked for feature removal.
 * Checks only plots visible to this city's owner.
 *
 * Positive values for iBad mean an increase in unhealthiness.
 */
void CvCity::calculateFeatureHealthPercentChange(int& iGood, int& iBad, CvPlot* pIgnorePlot) const
{
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL && pLoopPlot != pIgnorePlot && pLoopPlot->isVisible(getTeam(), true))
		{
			FeatureTypes eFeature = pLoopPlot->getFeatureType();

			if (eFeature != NO_FEATURE)
			{
				int iHealth = GC.getFeatureInfo(eFeature).getHealthPercent();

				if (iHealth != 0)
				{
					int iNumUnits = pLoopPlot->getNumUnits();

					if (iNumUnits > 0)
					{
						for (CvPlot::unit_iterator unitItr = pLoopPlot->beginUnits(); unitItr != pLoopPlot->endUnits(); ++unitItr)
						{
							BuildTypes eBuild = unitItr->getBuildType();

							if (eBuild != NO_BUILD)
							{
								CvBuildInfo& kBuild = GC.getBuildInfo(eBuild);

								if (kBuild.isFeatureRemove(eFeature))
								{
									if (iHealth > 0)
									{
										iGood += iHealth;
									}
									else
									{
										iBad -= iHealth;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

/*
 * Returns the total additional health that adding or removing iChange eFeatures will provide.
 */
int CvCity::getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange) const
{
	int iGood = 0, iBad = 0;
	return getAdditionalHealthByFeature(eFeature, iChange, iGood, iBad);
}

/*
 * Returns the total additional health that adding or removing iChange eFeatures will provide
 * and sets the good and bad levels individually.
 *
 * Doesn't reset iGood or iBad to zero.
 * Positive values for iBad mean an increase in unhealthiness.
 */
int CvCity::getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange, int& iGood, int& iBad) const
{
	FAssertMsg(eFeature >= 0, "eFeature expected to be >= 0");
	FAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature expected to be < GC.getNumFeatureInfos()");

	//CvFeatureInfo& kFeature = GC.getFeatureInfo(eFeature);
	int iHealth = GC.getFeatureInfo(eFeature).getHealthPercent();

	if (iHealth > 0)
	{
		return getAdditionalHealth(iChange * iHealth, 0, iGood, iBad);
	}
	else
	{
		return getAdditionalHealth(0, - iChange * iHealth, iGood, iBad);
	}
}

/*
 * Returns the total additional health that adding or removing a good or bad health percent will provide
 * and sets the good and bad levels individually.
 *
 * Doesn't reset iGood or iBad to zero.
 * Positive values for iBad and iBadPercent mean an increase in unhealthiness.
 */
int CvCity::getAdditionalHealth(int iGoodPercent, int iBadPercent, int& iGood, int& iBad) const
{
	int iStarting = iGood - iBad;

	// Add current
	calculateFeatureHealthPercent(iGoodPercent, iBadPercent);

	// Delta
	iGood += (iGoodPercent / 100) - getFeatureGoodHealth();
	iBad += (iBadPercent / 100) + getFeatureBadHealth();		// bad health is stored as negative

	return iGood - iBad - iStarting;
}
// BUG - Feature Health - end

// BUG - Actual Effects - start
/*
 * Returns the additional angry population caused by the given happiness changes.
 *
 * Positive values for iBad mean an increase in unhappiness.
 */
int CvCity::getAdditionalAngryPopuplation(int iGood, int iBad) const
{
	int iHappy = happyLevel();
	int iUnhappy = unhappyLevel();
	int iPop = getPopulation();

	return range((iUnhappy + iBad) - (iHappy + iGood), 0, iPop) - range(iUnhappy - iHappy, 0, iPop);
}

/*
 * Returns the additional spoiled food caused by the given health changes.
 *
 * Positive values for iBad mean an increase in unhealthiness.
 */
int CvCity::getAdditionalSpoiledFood(int iGood, int iBad, int iHealthAdjust) const
{
	int iHealthy = goodHealth();
	int iUnhealthy = badHealth();
	int iRate = iHealthy - iUnhealthy + iHealthAdjust;

	return std::min(0, iRate) - std::min(0, iRate + iGood - iBad);
}

/*
 * Returns the additional starvation caused by the given spoiled food.
 */
int CvCity::getAdditionalStarvation(int iSpoiledFood, int iFoodAdjust) const
{
	int iFood = getYieldRate(YIELD_FOOD) - foodConsumption() + iFoodAdjust;

	if (iSpoiledFood > 0)
	{
		if (iFood <= 0)
		{
			return iSpoiledFood;
		}
		else if (iSpoiledFood > iFood)
		{
			return iSpoiledFood - iFood;
		}
	}
	else if (iSpoiledFood < 0)
	{
		if (iFood < 0)
		{
			return std::max(iFood, iSpoiledFood);
		}
	}

	return 0;
}
// BUG - Actual Effects - start


int CvCity::getBuildingGoodHealth() const
{
	return m_iBuildingGoodHealth;
}


int CvCity::getBuildingBadHealth() const
{
	return m_iBuildingBadHealth;
}


int CvCity::getBuildingHealth(BuildingTypes eBuilding) const
{
	int iHealth = getBuildingGoodHealth(eBuilding);

	if (!isBuildingOnlyHealthy())
	{
		iHealth += getBuildingBadHealth(eBuilding);
	}

	return iHealth;
}

int CvCity::getBuildingGoodHealth(BuildingTypes eBuilding) const
{
	int iHealth = std::max(0, GC.getBuildingInfo(eBuilding).getHealth());
	iHealth += std::max(0, getBuildingHealthChange((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()));
	iHealth += std::max(0, GET_PLAYER(getOwnerINLINE()).getExtraBuildingHealth(eBuilding));
/************************************************************************************************/
/* Afforess	                  Start		 08/24/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iHealth += std::max(0, GET_TEAM(getTeam()).getTechExtraBuildingHealth(eBuilding));
	iHealth += std::max(0, (GC.getBuildingInfo(eBuilding).getHealthPercentPerPopulation() * getPopulation()) / 100);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iHealth;
}

int CvCity::getBuildingBadHealth(BuildingTypes eBuilding) const
{
	if (isBuildingOnlyHealthy())
	{
		return 0;
	}

	int iHealth = std::min(0, GC.getBuildingInfo(eBuilding).getHealth());
	iHealth += std::min(0, getBuildingHealthChange((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()));
	iHealth += std::min(0, GET_PLAYER(getOwnerINLINE()).getExtraBuildingHealth(eBuilding));
/************************************************************************************************/
/* Afforess	                  Start		 08/24/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iHealth += std::min(0, GET_TEAM(getTeam()).getTechExtraBuildingHealth(eBuilding));
	iHealth += std::min(0, (GC.getBuildingInfo(eBuilding).getHealthPercentPerPopulation() * getPopulation()) / 100);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iHealth;
}

void CvCity::changeBuildingGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHealth = (m_iBuildingGoodHealth + iChange);
		FAssert(getBuildingGoodHealth() >= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


void CvCity::changeBuildingBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHealth += iChange;
		FAssert(getBuildingBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getPowerGoodHealth() const
{
	return m_iPowerGoodHealth;
}


int CvCity::getPowerBadHealth() const
{
	return m_iPowerBadHealth;
}


void CvCity::updatePowerHealth()
{
	int iNewGoodHealth = 0;
	int iNewBadHealth = 0;

	if (isPower())
	{
		int iPowerHealth = GC.getDefineINT("POWER_HEALTH_CHANGE");
		if (iPowerHealth > 0)
		{
			iNewGoodHealth += iPowerHealth;
		}
		else
		{
			iNewBadHealth += iPowerHealth;
		}
	}

	if (isDirtyPower())
	{
		int iDirtyPowerHealth = GC.getDefineINT("DIRTY_POWER_HEALTH_CHANGE");
		if (iDirtyPowerHealth > 0)
		{
			iNewGoodHealth += iDirtyPowerHealth;
		}
		else
		{
			iNewBadHealth += iDirtyPowerHealth;
		}
	}

	if ((getPowerGoodHealth() != iNewGoodHealth) || (getPowerBadHealth() != iNewBadHealth))
	{
		m_iPowerGoodHealth = iNewGoodHealth;
		m_iPowerBadHealth = iNewBadHealth;
		FAssert(getPowerGoodHealth() >= 0);
		FAssert(getPowerBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getBonusGoodHealth() const
{
	return m_iBonusGoodHealth;
}


int CvCity::getBonusBadHealth() const
{
	return m_iBonusBadHealth;
}


void CvCity::changeBonusGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusGoodHealth += iChange;
		FAssert(getBonusGoodHealth() >= 0);

		FAssertMsg(getBonusGoodHealth() >= 0, "getBonusGoodHealth is expected to be >= 0");

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


void CvCity::changeBonusBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusBadHealth += iChange;
		FAssert(getBonusBadHealth() <= 0);

		FAssertMsg(getBonusBadHealth() <= 0, "getBonusBadHealth is expected to be <= 0");

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getMilitaryHappinessUnits() const
{
	return m_iMilitaryHappinessUnits;
}


int CvCity::getMilitaryHappiness() const
{
	return (getMilitaryHappinessUnits() * GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit());
}


void CvCity::changeMilitaryHappinessUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iMilitaryHappinessUnits = (m_iMilitaryHappinessUnits + iChange);
		FAssert(getMilitaryHappinessUnits() >= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getBuildingGoodHappiness() const
{
/************************************************************************************************/
/* Afforess	                  Start		 08/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	return m_iBuildingGoodHappiness
*/
	return m_iBuildingGoodHappiness + std::max(0, calculatePopulationHappiness());
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


int CvCity::getBuildingBadHappiness() const
{
/************************************************************************************************/
/* Afforess	                  Start		 08/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	return m_iBuildingBadHappiness
*/
	return m_iBuildingBadHappiness + std::min(0, calculatePopulationHappiness());
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


int CvCity::getBuildingHappiness(BuildingTypes eBuilding) const
{
	int iHappiness = GC.getBuildingInfo(eBuilding).getHappiness();

	if (GC.getBuildingInfo(eBuilding).getReligionType() != NO_RELIGION)
	{
		if (GC.getBuildingInfo(eBuilding).getReligionType() == GET_PLAYER(getOwnerINLINE()).getStateReligion())
		{
			iHappiness += GC.getBuildingInfo(eBuilding).getStateReligionHappiness();
		}
	}

	iHappiness += GET_PLAYER(getOwnerINLINE()).getExtraBuildingHappiness(eBuilding);

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iHappiness += ((GC.getBuildingInfo(eBuilding).getCommerceHappiness(iI) * GET_PLAYER(getOwnerINLINE()).getCommercePercent((CommerceTypes)iI)) / 100);
	}

	iHappiness += getBuildingHappyChange((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType());
/************************************************************************************************/
/* Afforess	                  Start		 08/24/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

	iHappiness += (GC.getBuildingInfo(eBuilding).getHappinessPercentPerPopulation() * getPopulation()) / 100;

	iHappiness += GET_TEAM(getTeam()).getTechExtraBuildingHappiness(eBuilding);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iHappiness;
}


void CvCity::changeBuildingGoodHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHappiness = (m_iBuildingGoodHappiness + iChange);
		FAssert(getBuildingGoodHappiness() >= 0);

		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeBuildingBadHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHappiness = (m_iBuildingBadHappiness + iChange);
		FAssert(getBuildingBadHappiness() <= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getExtraBuildingGoodHappiness() const
{
	return m_iExtraBuildingGoodHappiness;
}


int CvCity::getExtraBuildingBadHappiness() const
{
	return m_iExtraBuildingBadHappiness;
}


/********************************************************************************/
/* 	New Civic AI						19.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
void CvCity::updateExtraBuildingHappiness(bool bLimited)
{
	int iNewExtraBuildingGoodHappiness = 0;
	int iNewExtraBuildingBadHappiness = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			int iChange = getNumActiveBuilding((BuildingTypes)iI) * GET_PLAYER(getOwnerINLINE()).getExtraBuildingHappiness((BuildingTypes)iI);

			if (iChange > 0)
			{
				iNewExtraBuildingGoodHappiness += iChange;
			}
			else
			{
				iNewExtraBuildingBadHappiness += iChange;
			}
		}
	}

	if (getExtraBuildingGoodHappiness() != iNewExtraBuildingGoodHappiness)
	{
		m_iExtraBuildingGoodHappiness = iNewExtraBuildingGoodHappiness;
		FAssert(getExtraBuildingGoodHappiness() >= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getExtraBuildingBadHappiness() != iNewExtraBuildingBadHappiness)
	{
		m_iExtraBuildingBadHappiness = iNewExtraBuildingBadHappiness;
		FAssert(getExtraBuildingBadHappiness() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}
int CvCity::getAdditionalHappinessByCivic(CivicTypes eCivic, bool bDifferenceToCurrent, bool bCivicOptionVacuum, ReligionTypes eStateReligion, int iExtraPop, int iMilitaryHappinessUnits) const
{
	if (bDifferenceToCurrent)
	{
		return getAdditionalHappinessByCivic(eCivic, false, bCivicOptionVacuum, eStateReligion, iExtraPop, iMilitaryHappinessUnits) - getAdditionalHappinessByCivic( GET_PLAYER(getOwnerINLINE()).getCivics((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())) , false, bCivicOptionVacuum, eStateReligion, iExtraPop, iMilitaryHappinessUnits);
	}

	if (eCivic == NO_CIVIC)
	{
		return 0;
	}

	CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	if (eStateReligion == NO_RELIGION)
	{
		eStateReligion = kOwner.getStateReligion();
	}
	if (!bCivicOptionVacuum && !kCivic.isStateReligion() && kOwner.getStateReligionCount() == 1)
	{
		if ( GC.getCivicInfo( kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType())) ).isStateReligion() )
		{
			eStateReligion = NO_RELIGION;
		}
	}

	int iHappy = 0;

	//#1.a: Military Happiness
	if (kCivic.getHappyPerMilitaryUnit() != 0)
	{
		if (iMilitaryHappinessUnits < 0) //default -1
		{
			iMilitaryHappinessUnits = getMilitaryHappinessUnits();
		}
		iHappy += iMilitaryHappinessUnits * kCivic.getHappyPerMilitaryUnit();
	}


	//#1.b: CivicPercentAnger and WarWearinessModifier
	if ((kCivic.getCivicPercentAnger() != 0 && kOwner.getCivicPercentAnger(eCivic, true) != 0)
		|| (kCivic.getWarWearinessModifier() != 0 && kOwner.getWarWearinessPercentAnger() != 0))
	{
		//int CvCity::unhappyLevel(int iExtra) const
		int iAngerPercent = 0;

		iAngerPercent += getOvercrowdingPercentAnger(iExtraPop);
		iAngerPercent += getNoMilitaryPercentAnger();
		iAngerPercent += getCulturePercentAnger();
		iAngerPercent += getReligionPercentAnger();
		iAngerPercent += getHurryPercentAnger(iExtraPop);
		iAngerPercent += getConscriptPercentAnger(iExtraPop);
		iAngerPercent += getDefyResolutionPercentAnger(iExtraPop);
		int iOldWarWearinessAngerPercent;
		if ( !bCivicOptionVacuum && kOwner.getWarWearinessPercentAnger() != 0
			&& GC.getCivicInfo( kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType())) ).getWarWearinessModifier() != 0 )
		{
			//int CvCity::getWarWearinessPercentAnger() const
			iOldWarWearinessAngerPercent = kOwner.getWarWearinessPercentAnger();

			iOldWarWearinessAngerPercent *= std::max(0, (getWarWearinessModifier() + kOwner.getWarWearinessModifier() - GC.getCivicInfo( kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType())) ).getWarWearinessModifier() + 100));
			iOldWarWearinessAngerPercent /= 100;
		}
		else
		{
			iOldWarWearinessAngerPercent = getWarWearinessPercentAnger();
		}
		iAngerPercent += iOldWarWearinessAngerPercent;

		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			iAngerPercent += kOwner.getCivicPercentAnger((CivicTypes)iI, ( (CivicTypes)iI == eCivic || (!bCivicOptionVacuum && (CivicTypes)iI == kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType()))) ));
		}

		int iUnhappinessNow = ((iAngerPercent * (getPopulation() + iExtraPop)) / GC.getPERCENT_ANGER_DIVISOR());
		iAngerPercent -= kOwner.getCivicPercentAnger(eCivic, true);

		if (kOwner.getWarWearinessPercentAnger() != 0 && kCivic.getWarWearinessModifier() != 0)
		{
			//int CvCity::getWarWearinessPercentAnger() const
			int iNewWarWearinessAngerPercent = kOwner.getWarWearinessPercentAnger();

			iNewWarWearinessAngerPercent *= std::max(0, (getWarWearinessModifier() + kOwner.getWarWearinessModifier() + kCivic.getWarWearinessModifier() - ((bCivicOptionVacuum)? 0 : GC.getCivicInfo( kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType())) ).getWarWearinessModifier() ) + 100));
			iNewWarWearinessAngerPercent /= 100;
			iAngerPercent -= iOldWarWearinessAngerPercent;
			iAngerPercent += iNewWarWearinessAngerPercent;
		}
		int iUnhappinessThen = ((iAngerPercent * (getPopulation() + iExtraPop)) / GC.getPERCENT_ANGER_DIVISOR());
		iHappy += iUnhappinessNow - iUnhappinessThen;
	}

	//#1.c: LargestCityHappiness
	if (kCivic.getLargestCityHappiness() != 0)
	{
		//int CvCity::getLargestCityHappiness() const
		if (findPopulationRank() <= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities())
		{
			iHappy += kCivic.getLargestCityHappiness();
		}
	}

	if (kCivic.getCivicHappiness() != 0)
	{
		iHappy += kCivic.getCivicHappiness();
	}

	if (kCivic.getCityLimit(getOwnerINLINE()) > 0 && kCivic.getCityOverLimitUnhappy() > 0 && kOwner.getNumCities() > kCivic.getCityLimit(getOwnerINLINE()))
	{
		iHappy -= kCivic.getCityOverLimitUnhappy()*(kOwner.getNumCities() - kCivic.getCityLimit(getOwnerINLINE()));
	}
			
	if (kCivic.getForeignerUnhappyPercent() > 0)
	{
		iHappy -= ((100 / kCivic.getForeignerUnhappyPercent()) * (100 - plot()->calculateCulturePercent(getOwnerINLINE()))) / 100;
	}

	//#1.d: BuildingHappinessChanges
	if (kCivic.isAnyBuildingHappinessChange())
	{
		CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			int iTempHappy = kCivic.getBuildingHappinessChanges(iI);
			if (iTempHappy != 0)
			{
				BuildingTypes eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
				if (eLoopBuilding != NO_BUILDING)
				{
					iHappy += iTempHappy * getNumBuilding(eLoopBuilding);
				}
			}
		}
	}

	//#1.e: FeatureHappinessChanges
	if (kCivic.isAnyFeatureHappinessChange())
	{
		CvPlot* pLoopPlot;
		for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			int iTempHappy = kCivic.getFeatureHappinessChanges(iI);

			if (iTempHappy != 0)
			{
				int iCount = 0;
				for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iJ);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->getFeatureType() == (FeatureTypes)iI)
						{
							iCount++;
						}
					}
				}
				iHappy += iTempHappy * iCount;
			}
		}
	}

	//#1.f: Religious Happiness
	if (kCivic.getStateReligionHappiness() != 0 || kCivic.getNonStateReligionHappiness() != 0)
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (isHasReligion((ReligionTypes)iI))
			{
				if ((ReligionTypes)iI == eStateReligion /* redundant? -> */ && (ReligionTypes)iI != NO_RELIGION)
				{
					iHappy += kCivic.getStateReligionHappiness();
				}
				else
				{
					iHappy += kCivic.getNonStateReligionHappiness();
				}
			}
		}
	}

	if (isCapital() && kCivic.isNoCapitalUnhappiness())
	{
		iHappy = std::max(unhappyLevel(0),iHappy);
	}

	return iHappy;
}


int CvCity::getAdditionalHealthByCivic(CivicTypes eCivic, bool bDifferenceToCurrent) const
{
	int iGood = 0, iBad = 0, iBadBuilding = 0;
	return getAdditionalHealthByCivic(eCivic, iGood, iBad, iBadBuilding, bDifferenceToCurrent);
}

int CvCity::getAdditionalHealthByCivic(CivicTypes eCivic, int& iGood, int& iBad, int& iBadBuilding, bool bDifferenceToCurrent, int iExtraPop, bool bCivicOptionVacuum, int iIgnoreNoUnhealthyPopulationCount, int iIgnoreBuildingOnlyHealthyCount) const
{
	if (bDifferenceToCurrent)
	{
		int iHealthNew = getAdditionalHealthByCivic(eCivic, iGood, iBad, iBadBuilding, false, iExtraPop, bCivicOptionVacuum, iIgnoreNoUnhealthyPopulationCount, iIgnoreBuildingOnlyHealthyCount);

		int iTempGood = 0; int iTempBad = 0; int iTempBadBuilding = 0;
		int iHealthOld = getAdditionalHealthByCivic( GET_PLAYER(getOwnerINLINE()).getCivics((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())) , iTempGood, iTempBad, iTempBadBuilding, false, iExtraPop, bCivicOptionVacuum, iIgnoreNoUnhealthyPopulationCount, iIgnoreBuildingOnlyHealthyCount);
		iGood += iTempBad;
		iBad += iTempGood;
		iBadBuilding -= iTempBadBuilding; //can become negative

		return iHealthNew - iHealthOld;
	}


	if (eCivic == NO_CIVIC)
	{
		return 0;
	}

	CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	int iHealth = 0;

	if (!bCivicOptionVacuum)
	{
		if ( kOwner.isCivic(eCivic) || GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)kCivic.getCivicOptionType())).isNoUnhealthyPopulation() )
		{
			iIgnoreNoUnhealthyPopulationCount++;
		}
		if ( kOwner.isCivic(eCivic) || GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)kCivic.getCivicOptionType())).isBuildingOnlyHealthy() )
		{
			iIgnoreBuildingOnlyHealthyCount++;
		}
	}

	//#2.a: ExtraHealth
	addGoodOrBad(kCivic.getExtraHealth(), iGood, iBad);

	//#2.b: NoUnhealthyPopulation
	if (kCivic.isNoUnhealthyPopulation())
	{
		if ((iIgnoreNoUnhealthyPopulationCount != 0) )
		{
			if (kOwner.getNoUnhealthyPopulationCount() <= iIgnoreNoUnhealthyPopulationCount && getNoUnhealthyPopulationCount() <= 0)
			{
				//std::max(0, ((getPopulation() + iExtra - ((bNoAngry) ? angryPopulation(iExtra) : 0))))
				iGood += std::max(0, ((getPopulation() + iExtraPop)));
			}
		}
		else
		{
			iGood += unhealthyPopulation(false, iExtraPop);
		}
	}

	//#2.c: BuildingOnlyHealthy
	if (kCivic.isBuildingOnlyHealthy())
	{
		if (iIgnoreBuildingOnlyHealthyCount != 0 )
		{
			int iOwnerBuildingOnlyHealthyCount = kOwner.getBuildingOnlyHealthyCount();
			if (iOwnerBuildingOnlyHealthyCount <= iIgnoreBuildingOnlyHealthyCount && getBuildingOnlyHealthyCount() <= 0)
			{
				kOwner.changeBuildingOnlyHealthyCount(-iOwnerBuildingOnlyHealthyCount, true);
				iGood -= totalBadBuildingHealth();
				kOwner.changeBuildingOnlyHealthyCount( iOwnerBuildingOnlyHealthyCount, true);
			}
		}
		else
		{
			iGood -= totalBadBuildingHealth();
		}
	}

	//#2.d: BuildingHealthChanges
	if (kCivic.isAnyBuildingHealthChange())
	{
		CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			int iTempHealth = kCivic.getBuildingHealthChanges(iI);

			if (iTempHealth > 0)
			{
				BuildingTypes eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
				if (eLoopBuilding != NO_BUILDING)
				{
					iGood += iTempHealth * getNumBuilding(eLoopBuilding);
				}
			}
			else if (iTempHealth < 0 && (getBuildingOnlyHealthyCount() <= 0 || kOwner.getBuildingOnlyHealthyCount() <= iIgnoreBuildingOnlyHealthyCount))
			{
				BuildingTypes eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
				if (eLoopBuilding != NO_BUILDING)
				{
					iTempHealth *= getNumBuilding(eLoopBuilding);
					iBad -= iTempHealth;
					iBadBuilding -= iTempHealth;
					if (kCivic.isBuildingOnlyHealthy())
					{
						iGood += iTempHealth;
					}
				}
			}
		}
	}
	iHealth = iGood - iBad;

	return iHealth;
}

int CvCity::getAdditionalHealthByPlayerNoUnhealthyPopulation(int iExtraPop, int iIgnoreNoUnhealthyPopulationCount) const
{
	int iHealth = 0;
	if ((iIgnoreNoUnhealthyPopulationCount != 0) )
	{
		if (GET_PLAYER(getOwnerINLINE()).getNoUnhealthyPopulationCount() <= iIgnoreNoUnhealthyPopulationCount && getNoUnhealthyPopulationCount() <= 0)
		{
			//std::max(0, ((getPopulation() + iExtra - ((bNoAngry) ? angryPopulation(iExtra) : 0))))
			iHealth += std::max(0, ((getPopulation() + iExtraPop)));
		}
	}
	else
	{
		iHealth += unhealthyPopulation(false, iExtraPop);
	}
	return iHealth;
}

int CvCity::getAdditionalHealthByPlayerBuildingOnlyHealthy(int iIgnoreBuildingOnlyHealthyCount) const
{
	int iHealth = 0;
	if (iIgnoreBuildingOnlyHealthyCount != 0 )
	{
		CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
		int iOwnerBuildingOnlyHealthyCount = kOwner.getBuildingOnlyHealthyCount();
		if (iOwnerBuildingOnlyHealthyCount <= iIgnoreBuildingOnlyHealthyCount && getBuildingOnlyHealthyCount() <= 0)
		{
			kOwner.changeBuildingOnlyHealthyCount(-iOwnerBuildingOnlyHealthyCount, true);
			iHealth -= totalBadBuildingHealth();
			kOwner.changeBuildingOnlyHealthyCount( iOwnerBuildingOnlyHealthyCount, true);
		}
	}
	else
	{
		iHealth -= totalBadBuildingHealth();
	}
	return iHealth;
}

/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/

// BUG - Building Additional Happiness - start
/*
 * Returns the total additional happiness that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHappinessByBuilding(BuildingTypes eBuilding)
{
	int iGood = 0, iBad = 0, iAngryPop = 0;
	return getAdditionalHappinessByBuilding(eBuilding, iGood, iBad, iAngryPop);
}

/*
 * Returns the total additional happiness that adding one of the given buildings will provide
 * and sets the good and bad levels individually and any resulting additional angry population.
 *
 * Doesn't reset iGood or iBad to zero.
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHappinessByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iAngryPop)
{
	PROFILE_FUNC();

	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iStarting = iGood - iBad;
	int iStartingBad = iBad;

	// Basic
	addGoodOrBad(kBuilding.getHappiness(), iGood, iBad);

	// Building Class
	addGoodOrBad(getBuildingHappyChange((BuildingClassTypes)kBuilding.getBuildingClassType()), iGood, iBad);

	// Other Building Classes
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());
	if ( kBuilding.getBuildingHappinessChanges(NO_BUILDING) != 0 )
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			int iBuildingHappinessChanges = kBuilding.getBuildingHappinessChanges(iI);
			if (iBuildingHappinessChanges != 0)
			{
				BuildingTypes eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
				if (eLoopBuilding != NO_BUILDING)
				{
					addGoodOrBad(iBuildingHappinessChanges * (getNumBuilding(eLoopBuilding) + (eBuilding == eLoopBuilding ? 1 : 0)), iGood, iBad);
				}
			}
		}
	}

	// Player Building
	addGoodOrBad(GET_PLAYER(getOwnerINLINE()).getExtraBuildingHappiness(eBuilding), iGood, iBad);

	// Area
	addGoodOrBad(kBuilding.getAreaHappiness(), iGood, iBad);

	// Global
	addGoodOrBad(kBuilding.getGlobalHappiness(), iGood, iBad);

	// Religion
	if (kBuilding.getReligionType() != NO_RELIGION && kBuilding.getReligionType() == GET_PLAYER(getOwnerINLINE()).getStateReligion())
	{
		iGood += kBuilding.getStateReligionHappiness();
	}

	// Bonus
	if ( kBuilding.getBonusHappinessChanges(NO_BONUS) != 0 )
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			if ((hasBonus((BonusTypes)iI) || kBuilding.getFreeBonus() == iI || kBuilding.hasExtraFreeBonus((BonusTypes)iI)) && kBuilding.getNoBonus() != iI)
			{
				addGoodOrBad(kBuilding.getBonusHappinessChanges(iI), iGood, iBad);
			}
		}
	}

	// Commerce
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		int iCommerceHappiness = kBuilding.getCommerceHappiness(iI);

		if ( iCommerceHappiness != 0 )
		{
			addGoodOrBad( iCommerceHappiness * GET_PLAYER(getOwnerINLINE()).getCommercePercent((CommerceTypes)iI) / 100, iGood, iBad);
		}
	}

	// War Weariness Modifier
	int iWarWearinessModifier = kBuilding.getWarWearinessModifier() + kBuilding.getGlobalWarWearinessModifier();
	if (iWarWearinessModifier != 0)
	{
		int iBaseAngerPercent = 0;

		iBaseAngerPercent += getOvercrowdingPercentAnger();
		iBaseAngerPercent += getNoMilitaryPercentAnger();
		iBaseAngerPercent += getCulturePercentAnger();
		iBaseAngerPercent += getReligionPercentAnger();
		iBaseAngerPercent += getHurryPercentAnger();
		iBaseAngerPercent += getConscriptPercentAnger();
		iBaseAngerPercent += getDefyResolutionPercentAnger();
		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			iBaseAngerPercent += GET_PLAYER(getOwnerINLINE()).getCivicPercentAnger((CivicTypes)iI);
		}

		int iCurrentAngerPercent = iBaseAngerPercent + getWarWearinessPercentAnger();
		int iCurrentUnhappiness = iCurrentAngerPercent * getPopulation() / GC.getPERCENT_ANGER_DIVISOR();

		int iNewWarAngerPercent = GET_PLAYER(getOwnerINLINE()).getWarWearinessPercentAnger();
		iNewWarAngerPercent *= std::max(0, (iWarWearinessModifier + getWarWearinessModifier() + GET_PLAYER(getOwnerINLINE()).getWarWearinessModifier() + 100));
		iNewWarAngerPercent /= 100;
		int iNewAngerPercent = iBaseAngerPercent + iNewWarAngerPercent;
		int iNewUnhappiness = iNewAngerPercent * getPopulation() / GC.getPERCENT_ANGER_DIVISOR();

		iBad += iNewUnhappiness - iCurrentUnhappiness;
	}

	//	KOSHLING - port from K-mod - no unhappiness already => we don't care what this building does
	if ( isNoUnhappiness() )
	{
		iBad = iStartingBad;
	}
	// No Unhappiness
	else if (kBuilding.isNoUnhappiness())
	{
		// override extra unhappiness and completely negate all existing unhappiness
		iBad = iStartingBad - unhappyLevel();
	}
	// Effect on Angry Population
	int iHappy = happyLevel();
	int iUnhappy = unhappyLevel();
	int iPop = getPopulation();
	iAngryPop += range((iUnhappy + iBad) - (iHappy + iGood), 0, iPop) - range(iUnhappy - iHappy, 0, iPop);
	
/************************************************************************************************/
/* Afforess	                  Start		 12/20/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if ( kBuilding.getTechHappinessChanges(NO_TECH) != 0 )
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)iI))
			{
				addGoodOrBad(kBuilding.getTechHappinessChanges(iI), iGood, iBad);
			}
		}
	}
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			if (GC.getBuildingInfo((BuildingTypes)iI).isReplaceBuildingClass((BuildingClassTypes)kBuilding.getBuildingClassType()))
			{
				addGoodOrBad(-getBuildingHappiness((BuildingTypes)iI), iGood, iBad);
			}
		}
	}
	
	if (kBuilding.getHappinessPercentPerPopulation() > 0)
	{
		addGoodOrBad(kBuilding.getHappinessPercentPerPopulation() * getPopulation() / 100, iGood, iBad);
	}

	int iSpecialistExtraHappy = 0;

	for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
	{
		SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;
		CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eNewSpecialist);
		iSpecialistExtraHappy += kSpecialist.getHappinessPercent();
	}
	iSpecialistExtraHappy /= 100;
	addGoodOrBad(iSpecialistExtraHappy, iGood, iBad);
	
	if (kBuilding.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
		addGoodOrBad(iHappiness, iGood, iBad);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iGood - iBad - iStarting;
}


/*
 * Returns the total additional health that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHealthByBuilding(BuildingTypes eBuilding)
{
	int iGood = 0, iBad = 0, iSpoiledFood = 0, iStarvation = 0;
	return getAdditionalHealthByBuilding(eBuilding, iGood, iBad, iSpoiledFood, iStarvation);
}

/*
 * Returns the total additional health that adding one of the given buildings will provide
 * and sets the good and bad levels individually and any resulting additional spoiled food.
 *
 * Doesn't reset iGood, iBad, iSpoiledFood, iStarvation to zero.
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHealthByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iSpoiledFood, int& iStarvation) 
{
	PROFILE_FUNC();

	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iStarting = iGood - iBad;
	int iStartingBad = iBad;

	// Basic
	addGoodOrBad(kBuilding.getHealth(), iGood, iBad);

	// Building Class
	addGoodOrBad(getBuildingHealthChange((BuildingClassTypes)kBuilding.getBuildingClassType()), iGood, iBad);

	// Player Building
	addGoodOrBad(GET_PLAYER(getOwnerINLINE()).getExtraBuildingHealth(eBuilding), iGood, iBad);

	// Area
	addGoodOrBad(kBuilding.getAreaHealth(), iGood, iBad);

	// Global
	addGoodOrBad(kBuilding.getGlobalHealth(), iGood, iBad);

	// No Unhealthiness from Buildings
	if (isBuildingOnlyHealthy())
	{
		// undo bad from this building
		iBad = iStartingBad;
	}
	if (kBuilding.isBuildingOnlyHealthy())
	{
		// undo bad from this and all existing buildings
		iBad = iStartingBad + totalBadBuildingHealth();
	}

	// Bonus
	if ( kBuilding.getBonusHealthChanges(NO_BONUS) != 0)
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			if ((hasBonus((BonusTypes)iI) || kBuilding.getFreeBonus() == iI || kBuilding.hasExtraFreeBonus((BonusTypes)iI)) && kBuilding.getNoBonus() != iI)
			{
				addGoodOrBad(kBuilding.getBonusHealthChanges(iI), iGood, iBad);
			}
		}
	}

	// Power
	if (kBuilding.isPower() || kBuilding.isAreaCleanPower() || (kBuilding.getPowerBonus() != NO_BONUS && hasBonus((BonusTypes)kBuilding.getPowerBonus())))
	{
		// adding power
		if (!isPower())
		{
			addGoodOrBad(GC.getDefineINT("POWER_HEALTH_CHANGE"), iGood, iBad);

			// adding dirty power
			if (kBuilding.isDirtyPower())
			{
				addGoodOrBad(GC.getDefineINT("DIRTY_POWER_HEALTH_CHANGE"), iGood, iBad);
			}
		}
		else
		{
			// replacing dirty power with clean power
			if (isDirtyPower() && !kBuilding.isDirtyPower())
			{
				subtractGoodOrBad(GC.getDefineINT("DIRTY_POWER_HEALTH_CHANGE"), iGood, iBad);
			}
		}
	}

	// No Unhealthiness from Population
	if (kBuilding.isNoUnhealthyPopulation())
	{
		iBad -= getPopulation();
	}

	// Effect on Spoiled Food
	int iHealthy = goodHealth();
	int iUnhealthy = badHealth();
	int iFood = getYieldRate(YIELD_FOOD) - foodConsumption();
	iSpoiledFood -= std::min(0, (iHealthy + iGood) - (iUnhealthy + iBad)) - std::min(0, iHealthy - iUnhealthy);
	if (iSpoiledFood > 0)
	{
		if (iFood <= 0)
		{
			iStarvation += iSpoiledFood;
		}
		else if (iSpoiledFood > iFood)
		{
			iStarvation += iSpoiledFood - iFood;
		}
	}
	else if (iSpoiledFood < 0)
	{
		if (iFood < 0)
		{
			iStarvation += std::max(iFood, iSpoiledFood);
		}
	}

/************************************************************************************************/
/* Afforess	                  Start		 12/20/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if ( kBuilding.getTechHealthChanges(NO_TECH) != 0 )
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)iI))
			{
				addGoodOrBad(kBuilding.getTechHealthChanges(iI), iGood, iBad);
			}
		}
	}
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			//Team Project (5)
			if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				if (GC.getBuildingInfo((BuildingTypes)iI).isReplaceBuildingClass((BuildingClassTypes)kBuilding.getBuildingClassType()))
				{
					addGoodOrBad(-getBuildingHealth((BuildingTypes)iI), iGood, iBad);
				}
			}
		}
	}
	
	if (kBuilding.getHealthPercentPerPopulation() > 0)
	{
		addGoodOrBad(kBuilding.getHealthPercentPerPopulation() * getPopulation() / 100, iGood, iBad);
	}

	int iSpecialistExtraHealth = 0;

	for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
	{
		SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;
		CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eNewSpecialist);
		iSpecialistExtraHealth += kSpecialist.getHealthPercent();
	}
	iSpecialistExtraHealth /= 100;
	addGoodOrBad(iSpecialistExtraHealth, iGood, iBad);
	
	if (kBuilding.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
		addGoodOrBad(iHealthiness, iGood, iBad);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iGood - iBad - iStarting;
}

/*
 * Adds iValue to iGood if it is positive or its negative to iBad if it is negative.
 */
void addGoodOrBad(int iValue, int& iGood, int& iBad)
{
	if (iValue > 0)
	{
		iGood += iValue;
	}
	else if (iValue < 0)
	{
		iBad -= iValue;
	}
}

/*
 * Subtracts iValue from iGood if it is positive or its negative from iBad if it is negative.
 */
void subtractGoodOrBad(int iValue, int& iGood, int& iBad)
{
	if (iValue > 0)
	{
		iGood -= iValue;
	}
	else if (iValue < 0)
	{
		iBad += iValue;
	}
}
// BUG - Building Additional Happiness - end


int CvCity::getExtraBuildingGoodHealth() const
{
	return m_iExtraBuildingGoodHealth;
}


int CvCity::getExtraBuildingBadHealth() const
{
	return m_iExtraBuildingBadHealth;
}


/********************************************************************************/
/* 	New Civic AI						02.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
void CvCity::updateExtraBuildingHealth(bool bLimited)
{
	int iNewExtraBuildingGoodHealth = 0;
	int iNewExtraBuildingBadHealth = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			int iChange = getNumActiveBuilding((BuildingTypes)iI) * GET_PLAYER(getOwnerINLINE()).getExtraBuildingHealth((BuildingTypes)iI);

			if (iChange > 0)
			{
				iNewExtraBuildingGoodHealth += iChange;
			}
			else
			{
				iNewExtraBuildingBadHealth += iChange;
			}
		}
	}

	if (getExtraBuildingGoodHealth() != iNewExtraBuildingGoodHealth)
	{
		m_iExtraBuildingGoodHealth = iNewExtraBuildingGoodHealth;
		FAssert(getExtraBuildingGoodHealth() >= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getExtraBuildingBadHealth() != iNewExtraBuildingBadHealth)
	{
		m_iExtraBuildingBadHealth = iNewExtraBuildingBadHealth;
		FAssert(getExtraBuildingBadHealth() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}
/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/


int CvCity::getFeatureGoodHappiness() const
{
	return m_iFeatureGoodHappiness;
}


int CvCity::getFeatureBadHappiness() const
{
	return m_iFeatureBadHappiness;
}


/********************************************************************************/
/* 	New Civic AI						02.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
void CvCity::updateFeatureHappiness(bool bLimited)
{
	int iNewFeatureGoodHappiness = 0;
	int iNewFeatureBadHappiness = 0;

/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	for (int iI = 0; iI < getNumCityPlots(); iI++)
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			FeatureTypes eFeature = pLoopPlot->getFeatureType();

			if (eFeature != NO_FEATURE)
			{
				int iHappy = GET_PLAYER(getOwnerINLINE()).getFeatureHappiness(eFeature);
				if (iHappy > 0)
				{
					iNewFeatureGoodHappiness += iHappy;
				}
				else
				{
					iNewFeatureBadHappiness += iHappy;
				}
			}

			ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

			if (NO_IMPROVEMENT != eImprovement)
			{
				int iHappy = GC.getImprovementInfo(eImprovement).getHappiness();
/************************************************************************************************/
/* Afforess	                  Start		 07/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
				CvPlayer &kPlayer = GET_PLAYER(getOwnerINLINE());
				for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
				{
					if (kPlayer.getCivics((CivicOptionTypes)iJ) != NO_CIVIC)
					{
						iHappy += GC.getCivicInfo(kPlayer.getCivics((CivicOptionTypes)iJ)).getImprovementHappinessChanges(eImprovement);
					}
				}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

				if (iHappy > 0)
				{
					iNewFeatureGoodHappiness += iHappy;
				}
				else
				{
					iNewFeatureBadHappiness += iHappy;
				}
			}
		}
	}

	if (getFeatureGoodHappiness() != iNewFeatureGoodHappiness)
	{
		m_iFeatureGoodHappiness = iNewFeatureGoodHappiness;
		FAssert(getFeatureGoodHappiness() >= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getFeatureBadHappiness() != iNewFeatureBadHappiness)
	{
		m_iFeatureBadHappiness = iNewFeatureBadHappiness;
		FAssert(getFeatureBadHappiness() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}
/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/


int CvCity::getBonusGoodHappiness() const
{
	return m_iBonusGoodHappiness;
}


int CvCity::getBonusBadHappiness() const
{
	return m_iBonusBadHappiness;
}


void CvCity::changeBonusGoodHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusGoodHappiness = (m_iBonusGoodHappiness + iChange);
		FAssert(getBonusGoodHappiness() >= 0);

		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeBonusBadHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusBadHappiness = (m_iBonusBadHappiness + iChange);
		FAssert(getBonusBadHappiness() <= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getReligionGoodHappiness() const
{
	return m_iReligionGoodHappiness;
}


int CvCity::getReligionBadHappiness() const
{
	return m_iReligionBadHappiness;
}


int CvCity::getReligionHappiness(ReligionTypes eReligion) const
{
	int iHappiness;

	iHappiness = 0;

	if (isHasReligion(eReligion))
	{
		if (eReligion == GET_PLAYER(getOwnerINLINE()).getStateReligion())
		{
			iHappiness += GET_PLAYER(getOwnerINLINE()).getStateReligionHappiness();
		}
		else
		{
			iHappiness += GET_PLAYER(getOwnerINLINE()).getNonStateReligionHappiness();
		}
	}

	return iHappiness;
}


/********************************************************************************/
/* 	New Civic AI						02.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
void CvCity::updateReligionHappiness(bool bLimited)
{
	int iNewReligionGoodHappiness = 0;
	int iNewReligionBadHappiness = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		int iChange = getReligionHappiness((ReligionTypes)iI);

		if (iChange > 0)
		{
			iNewReligionGoodHappiness += iChange;
		}
		else
		{
			iNewReligionBadHappiness += iChange;
		}
	}

	if (getReligionGoodHappiness() != iNewReligionGoodHappiness)
	{
		m_iReligionGoodHappiness = iNewReligionGoodHappiness;
		FAssert(getReligionGoodHappiness() >= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getReligionBadHappiness() != iNewReligionBadHappiness)
	{
		m_iReligionBadHappiness = iNewReligionBadHappiness;
		FAssert(getReligionBadHappiness() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}
/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/


int CvCity::getExtraHappiness() const
{
	return m_iExtraHappiness;
}


void CvCity::changeExtraHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHappiness += iChange;

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getExtraHealth() const
{
	return m_iExtraHealth;
}


void CvCity::changeExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHealth += iChange;

		AI_setAssignWorkDirty(true);
	}
}



int CvCity::getHurryAngerTimer() const
{
	return m_iHurryAngerTimer;
}


void CvCity::changeHurryAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iHurryAngerTimer = (m_iHurryAngerTimer + iChange);
		FAssert(getHurryAngerTimer() >= 0);

		AI_setAssignWorkDirty(true);
	}
}

/************************************************************************************************/
/* REVOLUTION_MOD                         04/19/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvCity::getRevRequestAngerTimer() const
{
	return m_iRevRequestAngerTimer;
}


void CvCity::changeRevRequestAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iRevRequestAngerTimer = (m_iRevRequestAngerTimer + iChange);
		FAssert(getRevRequestAngerTimer() >= 0);

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getRevSuccessTimer() const
{
	return m_iRevSuccessTimer;
}

void CvCity::changeRevSuccessTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iRevSuccessTimer = (m_iRevSuccessTimer + iChange);
		FAssert(getRevSuccessTimer() >= 0);

		AI_setAssignWorkDirty(true);
	}
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

int CvCity::getConscriptAngerTimer() const
{
	return m_iConscriptAngerTimer;
}


void CvCity::changeConscriptAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iConscriptAngerTimer = (m_iConscriptAngerTimer + iChange);
		FAssert(getConscriptAngerTimer() >= 0);

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getDefyResolutionAngerTimer() const
{
	return m_iDefyResolutionAngerTimer;
}


void CvCity::changeDefyResolutionAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iDefyResolutionAngerTimer += iChange;
		FAssert(getDefyResolutionAngerTimer() >= 0);

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::flatDefyResolutionAngerLength() const
{
	int iAnger;

	iAnger = GC.getDefineINT("DEFY_RESOLUTION_ANGER_DIVISOR");

	iAnger *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getHurryConscriptAngerPercent();
	iAnger /= 100;

	return std::max(1, iAnger);
}


int CvCity::getHappinessTimer() const
{
	return m_iHappinessTimer;
}


void CvCity::changeHappinessTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iHappinessTimer += iChange;
		FAssert(getHappinessTimer() >= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getNoUnhappinessCount() const
{
	return m_iNoUnhappinessCount;
}


bool CvCity::isNoUnhappiness() const
{
	return (getNoUnhappinessCount() > 0);
}


void CvCity::changeNoUnhappinessCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoUnhappinessCount = (m_iNoUnhappinessCount + iChange);
		FAssert(getNoUnhappinessCount() >= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getNoUnhealthyPopulationCount()	const																	
{
	return m_iNoUnhealthyPopulationCount;
}


bool CvCity::isNoUnhealthyPopulation() const																		
{
	if (GET_PLAYER(getOwnerINLINE()).isNoUnhealthyPopulation())
	{
		return true;
	}

	return (getNoUnhealthyPopulationCount() > 0);
}


void CvCity::changeNoUnhealthyPopulationCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoUnhealthyPopulationCount = (m_iNoUnhealthyPopulationCount + iChange);
		FAssert(getNoUnhealthyPopulationCount() >= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getBuildingOnlyHealthyCount() const																	
{
	return m_iBuildingOnlyHealthyCount;
}


bool CvCity::isBuildingOnlyHealthy() const																		
 {
	if (GET_PLAYER(getOwnerINLINE()).isBuildingOnlyHealthy())
	{
		return true;
	}

	return (getBuildingOnlyHealthyCount() > 0);
}


void CvCity::changeBuildingOnlyHealthyCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingOnlyHealthyCount = (m_iBuildingOnlyHealthyCount + iChange);
		FAssert(getBuildingOnlyHealthyCount() >= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getFood() const
{
	return m_iFood;
}


void CvCity::setFood(int iNewValue)
{
	if (getFood() != iNewValue)
	{
		m_iFood = iNewValue;

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


void CvCity::changeFood(int iChange)
{
	setFood(getFood() + iChange);

#ifdef YIELD_VALUE_CACHING
	if ( iChange != 0 )
	{
		//	Yield calculation depends on city food stores so invalidate cache
		ClearYieldValueCache();
	}
#endif
}


int CvCity::getFoodKept() const
{
	return m_iFoodKept;
}


void CvCity::setFoodKept(int iNewValue)
{
	m_iFoodKept = iNewValue;
}


void CvCity::changeFoodKept(int iChange)
{
	setFoodKept(getFoodKept() + iChange);
}

void CvCity::recalculateMaxFoodKeptPercent()
{
	PROFILE_FUNC();

	m_fMaxFoodKeptMultiplierLog = 0;

	//	Game has been restored from an old save format so we have to calculate
	//	from first principles
	BuildingTypes eLoopBuilding;
	CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		eLoopBuilding = (BuildingTypes)kCivilizationInfo.getCivilizationBuildings(iI);
		if (eLoopBuilding != NO_BUILDING)
		{
			if (getNumBuilding(eLoopBuilding) > 0)
			{
				if ( GC.getBuildingInfo(eLoopBuilding).getFoodKept() != 0 )
				{
					changeMaxFoodKeptPercent(GC.getBuildingInfo(eLoopBuilding).getFoodKept(),true);
				}
			}
		}
	}
}

int CvCity::getMaxFoodKeptPercent() const
{
	if ( m_fMaxFoodKeptMultiplierLog == INVALID_STORED_FOOD_PERCENT_LOG )
	{
		(const_cast<CvCity*>(this))->recalculateMaxFoodKeptPercent();
	}

	float fMultiplier = exp(m_fMaxFoodKeptMultiplierLog);

	return (int)(100 - fMultiplier*100);
}


void CvCity::changeMaxFoodKeptPercent(int iChange, bool bAdd)
{
	FAssert(iChange >= 0);

	if ( m_fMaxFoodKeptMultiplierLog == INVALID_STORED_FOOD_PERCENT_LOG )
	{
		recalculateMaxFoodKeptPercent();
	}

	float logdiff = (bAdd ? 1 : -1)*log((100-(float)iChange)/100);
	
	m_fMaxFoodKeptMultiplierLog += logdiff;
}


int CvCity::getOverflowProduction() const
{
	return m_iOverflowProduction;
}


void CvCity::setOverflowProduction(int iNewValue)														
{
	m_iOverflowProduction = iNewValue;
	FAssert(getOverflowProduction() >= 0);
}


void CvCity::changeOverflowProduction(int iChange, int iProductionModifier)														
{
	int iOverflow = (100 * iChange) / std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, iProductionModifier));

	setOverflowProduction(getOverflowProduction() + iOverflow);
}


int CvCity::getFeatureProduction() const
{
	return m_iFeatureProduction;
}


void CvCity::setFeatureProduction(int iNewValue)														
{
	m_iFeatureProduction = iNewValue;
	FAssert(getFeatureProduction() >= 0);
}


void CvCity::changeFeatureProduction(int iChange)													
{
	setFeatureProduction(getFeatureProduction() + iChange);
}


int CvCity::getMilitaryProductionModifier()	const													
{
	return m_iMilitaryProductionModifier;
}


void CvCity::changeMilitaryProductionModifier(int iChange)												
{
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}


int CvCity::getSpaceProductionModifier() const															
{
	return m_iSpaceProductionModifier;
}


void CvCity::changeSpaceProductionModifier(int iChange)												
{
	m_iSpaceProductionModifier = (m_iSpaceProductionModifier + iChange);
}


int CvCity::getExtraTradeRoutes() const
{
	return m_iExtraTradeRoutes;
}


void CvCity::changeExtraTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraTradeRoutes = (m_iExtraTradeRoutes + iChange);
		//TB Note: There's a minimum on Trade routes, ultimately, of 0.  But it should be allowed to have a negative modifier.
		/*FAssert(getExtraTradeRoutes() >= 0);*/

		updateTradeRoutes();
	}
}


int CvCity::getTradeRouteModifier() const
{
	return m_iTradeRouteModifier;
}

void CvCity::changeTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRouteModifier = (m_iTradeRouteModifier + iChange);

		updateTradeRoutes();
	}
}

int CvCity::getForeignTradeRouteModifier() const
{
	return m_iForeignTradeRouteModifier;
}

void CvCity::changeForeignTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iForeignTradeRouteModifier = (m_iForeignTradeRouteModifier + iChange);

		updateTradeRoutes();
	}
}


int CvCity::getBuildingDefense() const
{
	return m_iBuildingDefense;
}


void CvCity::changeBuildingDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingDefense = (m_iBuildingDefense + iChange);

		setInfoDirty(true);

		plot()->plotAction(PUF_makeInfoBarDirty);
	}
}


int CvCity::getBuildingBombardDefense() const
{
	int iBombardDefenseTotal = m_iBuildingBombardDefense;
	iBombardDefenseTotal += GET_PLAYER(getOwnerINLINE()).getNationalBombardDefenseModifier();
	return std::min(GC.getGameINLINE().getModderGameOption(MODDERGAMEOPTION_MAX_BOMBARD_DEFENSE), iBombardDefenseTotal);
}


void CvCity::changeBuildingBombardDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBombardDefense += iChange;
		FAssert(getBuildingBombardDefense() >= 0);
	}
}

// BUG - Building Additional Bombard Defense - start
int CvCity::getAdditionalBombardDefenseByBuilding(BuildingTypes eBuilding) const
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iBaseDefense = getBuildingBombardDefense();

	// cap total bombard defense at 100
	return std::min(kBuilding.getBombardDefenseModifier() + iBaseDefense, 100) - iBaseDefense;
}
// BUG - Building Additional Bombard Defense - end


int CvCity::getFreeExperience() const
{
	return m_iFreeExperience;
}


void CvCity::changeFreeExperience(int iChange)
{
	m_iFreeExperience = (m_iFreeExperience + iChange);
	FAssert(getFreeExperience() >= 0);
}


int CvCity::getCurrAirlift() const
{
	return m_iCurrAirlift;
}


void CvCity::setCurrAirlift(int iNewValue)
{
	m_iCurrAirlift = iNewValue;
	FAssert(getCurrAirlift() >= 0);
}


void CvCity::changeCurrAirlift(int iChange)
{
	setCurrAirlift(getCurrAirlift() + iChange);
}


int CvCity::getMaxAirlift() const
{
	return m_iMaxAirlift;
}


void CvCity::changeMaxAirlift(int iChange)
{
	m_iMaxAirlift = (m_iMaxAirlift + iChange);
	FAssert(getMaxAirlift() >= 0);
}

int CvCity::getAirModifier() const
{
	return m_iAirModifier;
}

void CvCity::changeAirModifier(int iChange)
{
	m_iAirModifier += iChange;
}

int CvCity::getSMAirUnitCapacity(TeamTypes eTeam) const
{
	int iCapacity = getAirUnitCapacity(eTeam);
	iCapacity *= GC.getGameINLINE().getBaseAirUnitIncrementsbyCargoVolume();
	return iCapacity;
}

int CvCity::getAirUnitCapacity(TeamTypes eTeam) const
{
//Team Project (6)
	int iCapacity = (getTeam() == eTeam ? m_iAirUnitCapacity : GC.getDefineINT("CITY_AIR_UNIT_CAPACITY"));
	iCapacity += GET_PLAYER(getOwnerINLINE()).getNationalAirUnitCapacity();
	return iCapacity;
}

void CvCity::changeAirUnitCapacity(int iChange)
{
	m_iAirUnitCapacity += iChange;
	FAssert(getAirUnitCapacity(getTeam()) >= 0);
}

int CvCity::getNukeModifier() const
{
	return m_iNukeModifier;
}


void CvCity::changeNukeModifier(int iChange)
{
	m_iNukeModifier = (m_iNukeModifier + iChange);
}


int CvCity::getFreeSpecialist() const
{
	return std::max(0, m_iFreeSpecialist);
}


void CvCity::changeFreeSpecialist(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeSpecialist = (m_iFreeSpecialist + iChange);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getPowerCount() const
{
	return m_iPowerCount;
}


bool CvCity::isPower() const
{
/************************************************************************************************/
/* Afforess	                  Start		 06/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (getDisabledPowerTimer() > 0) return false;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return ((getPowerCount() > 0) || isAreaCleanPower());
}


bool CvCity::isAreaCleanPower() const
{
	if (area() == NULL)
	{
		return false;
	}

	return area()->isCleanPower(getTeam());
}


int CvCity::getDirtyPowerCount() const
{
	return m_iDirtyPowerCount;
}


bool CvCity::isDirtyPower() const
{
	return (isPower() && (getDirtyPowerCount() == getPowerCount()) && !isAreaCleanPower());
}


void CvCity::changePowerCount(int iChange, bool bDirty)
{
	if (iChange != 0)
	{
		bool wasPower = isPower();
		bool wasDirtyPower = isDirtyPower();

		m_iPowerCount = (m_iPowerCount + iChange);
		FAssert(getPowerCount() >= 0);
		if (bDirty)
		{
			m_iDirtyPowerCount += iChange;
			FAssert(getDirtyPowerCount() >= 0);
		}
		// cppcheck-suppress knownConditionTrueFalse
		if (wasPower != isPower())
		{
			GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache();

			setCommerceDirty();

			if (getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				setInfoDirty(true);
			}
		}
		// cppcheck-suppress knownConditionTrueFalse
		if (wasDirtyPower != isDirtyPower() || wasPower != isPower())
		{
			updatePowerHealth();
		}
	}
}


int CvCity::getDefenseDamage() const																
{
	return m_iDefenseDamage;
}


void CvCity::changeDefenseDamage(int iChange)
{
	if (iChange != 0)
	{
		m_iDefenseDamage = range((m_iDefenseDamage + iChange), 0, GC.getMAX_CITY_DEFENSE_DAMAGE());

		if (iChange > 0)
		{
			setBombarded(true);
		}

		setInfoDirty(true);

		plot()->plotAction(PUF_makeInfoBarDirty);
	}
}

void CvCity::changeDefenseModifier(int iChange)
{
	if (iChange != 0)
	{
		int iTotalDefense = getTotalDefense(false);

		if (iTotalDefense > 0)
		{
			changeDefenseDamage(-(GC.getMAX_CITY_DEFENSE_DAMAGE() * iChange + (iChange > 0 ? iTotalDefense : -iTotalDefense)/2) / iTotalDefense);
		}
	}
}


int CvCity::getLastDefenseDamage() const
{
	return m_iLastDefenseDamage;
}


void CvCity::setLastDefenseDamage(int iNewValue)
{
	m_iLastDefenseDamage = iNewValue;
}


bool CvCity::isBombardable(const CvUnit* pUnit) const
{
	if (NULL != pUnit && !pUnit->isEnemy(getTeam()))
	{
		return false;
	}

	return (getDefenseModifier(false) > getExtraMinDefense());
}


int CvCity::getNaturalDefense() const
{
	if (getCultureLevel() == NO_CULTURELEVEL)
	{
		return 0;
	}

	return GC.getCultureLevelInfo(getCultureLevel()).getCityDefenseModifier();
}


int CvCity::getTotalDefense(bool bIgnoreBuilding) const
{
	return (std::max(((bIgnoreBuilding) ? 0 : getBuildingDefense()), getNaturalDefense()) + GET_PLAYER(getOwnerINLINE()).getCityDefenseModifier() + calculateBonusDefense());
}


int CvCity::getDefenseModifier(bool bIgnoreBuilding) const
{
	if (isOccupation())
	{
		return 0;
	}

	return (std::max(getExtraMinDefense(),((getTotalDefense(bIgnoreBuilding) * (GC.getMAX_CITY_DEFENSE_DAMAGE() - getDefenseDamage())) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
}


int CvCity::getOccupationTimer() const
{
	return m_iOccupationTimer;
}


bool CvCity::isOccupation() const
{
	return (getOccupationTimer() > 0);
}


void CvCity::setOccupationTimer(int iNewValue)
{
	if (getOccupationTimer() != iNewValue)
	{
		bool wasOccupation = isOccupation();

		m_iOccupationTimer = iNewValue;
		FAssert(getOccupationTimer() >= 0);
		// cppcheck-suppress knownConditionTrueFalse
		if (wasOccupation != isOccupation())
		{
			updateCorporation();
			setMaintenanceDirty(true);
			updateTradeRoutes();

			updateCultureLevel(true);

			AI_setAssignWorkDirty(true);
		}

		setInfoDirty(true);
	}
}


void CvCity::changeOccupationTimer(int iChange)												
{
	setOccupationTimer(getOccupationTimer() + iChange);
}


int CvCity::getCultureUpdateTimer() const
{
	return m_iCultureUpdateTimer;
}


void CvCity::setCultureUpdateTimer(int iNewValue)
{
	m_iCultureUpdateTimer = iNewValue;
	FAssert(getOccupationTimer() >= 0);
}


void CvCity::changeCultureUpdateTimer(int iChange)												
{
	setCultureUpdateTimer(getCultureUpdateTimer() + iChange);
}


int CvCity::getCitySizeBoost() const
{
	return m_iCitySizeBoost;
}


void CvCity::setCitySizeBoost(int iBoost)
{
	if (getCitySizeBoost() != iBoost)
	{
		m_iCitySizeBoost = iBoost;

		setLayoutDirty(true);
	}
}

// < M.A.D. Nukes Start >
int CvCity::getMADIncoming()
{
	return m_iMADIncoming;
}

void CvCity::setMADIncoming(int iValue)
{
	m_iMADIncoming = iValue;
}

void CvCity::changeMADIncoming(int iValue)
{
	m_iMADIncoming += iValue;
}
// < M.A.D. Nukes End   >


bool CvCity::isNeverLost() const
{
	return m_bNeverLost;
}


void CvCity::setNeverLost(bool bNewValue)
{
	m_bNeverLost = bNewValue;
}


bool CvCity::isBombarded() const
{
	return m_bBombarded;
}


void CvCity::setBombarded(bool bNewValue)
{
	m_bBombarded = bNewValue;
}


bool CvCity::isDrafted() const
{
	return m_bDrafted;
}


void CvCity::setDrafted(bool bNewValue)
{
	m_bDrafted = bNewValue;
}


bool CvCity::isAirliftTargeted() const
{
	return m_bAirliftTargeted;
}


void CvCity::setAirliftTargeted(bool bNewValue)
{
	m_bAirliftTargeted = bNewValue;
}


bool CvCity::isPlundered() const
{
	return m_bPlundered;
}


void CvCity::setPlundered(bool bNewValue)
{
	if (bNewValue != isPlundered())
	{
		m_bPlundered = bNewValue;

		updateTradeRoutes();
	}
}


bool CvCity::isWeLoveTheKingDay() const
{
	return m_bWeLoveTheKingDay;
}


void CvCity::setWeLoveTheKingDay(bool bNewValue)
{
	if (isWeLoveTheKingDay() != bNewValue)
	{
		m_bWeLoveTheKingDay = bNewValue;

		setMaintenanceDirty(true);

		CivicTypes eCivic = NO_CIVIC;

		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (GET_PLAYER(getOwnerINLINE()).isCivic((CivicTypes)iI))
			{
				if (!CvWString(GC.getCivicInfo((CivicTypes)iI).getWeLoveTheKing()).empty())
				{
					eCivic = ((CivicTypes)iI);
					break;
				}
			}
		}

		if (eCivic != NO_CIVIC)
		{
			MEMORY_TRACK_EXEMPT();
			CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_CELEBRATE", getNameKey(), GC.getCivicInfo(eCivic).getWeLoveTheKing());
			AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WELOVEKING", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_HAPPY_PERSON")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
		}
	}
}


bool CvCity::isCitizensAutomated() const																
{
	return m_bCitizensAutomated;
}


void CvCity::setCitizensAutomated(bool bNewValue)
{
	if (isCitizensAutomated() != bNewValue)
	{
		m_bCitizensAutomated = bNewValue;

		if (isCitizensAutomated())
		{
			AI_assignWorkingPlots();
		}
		else
		{
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				setForceSpecialistCount(((SpecialistTypes)iI), 0);
			}
		}

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


bool CvCity::isProductionAutomated() const
{
	return m_bProductionAutomated;
}


void CvCity::setProductionAutomated(bool bNewValue, bool bClear)
{
	if (isProductionAutomated() != bNewValue)
	{
		m_bProductionAutomated = bNewValue;

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);

			// if automated and not network game and all 3 modifiers down, clear the queue and choose again
			if (bNewValue && bClear)
			{
				clearOrderQueue();
			}
		}
		
		if (!isProduction())
		{
			AI_chooseProduction();		    
		}
	}
}


bool CvCity::isWallOverride() const
{
	return m_bWallOverride;
}


void CvCity::setWallOverride(bool bOverride)
{
	if (isWallOverride() != bOverride)
	{
		m_bWallOverride = bOverride;

		setLayoutDirty(true);
	}
}


bool CvCity::isInfoDirty() const
{
	return m_bInfoDirty;
}


void CvCity::setInfoDirty(bool bNewValue)
{
	m_bInfoDirty = bNewValue;
}


bool CvCity::isLayoutDirty() const
{
	return m_bLayoutDirty;
}


void CvCity::setLayoutDirty(bool bNewValue)
{
	m_bLayoutDirty = bNewValue;
}


PlayerTypes CvCity::getOwner() const
{
	return getOwnerINLINE();
}


PlayerTypes CvCity::getPreviousOwner() const
{
	return m_ePreviousOwner;
}


void CvCity::setPreviousOwner(PlayerTypes eNewValue)
{
	m_ePreviousOwner = eNewValue;
}


PlayerTypes CvCity::getOriginalOwner() const
{
	return m_eOriginalOwner;
}


void CvCity::setOriginalOwner(PlayerTypes eNewValue)
{
	m_eOriginalOwner = eNewValue;
}


TeamTypes CvCity::getTeam() const
{
	return GET_PLAYER(getOwnerINLINE()).getTeam();
}


CultureLevelTypes CvCity::getCultureLevel() const
{
	return m_eCultureLevel;
}


int CvCity::getCultureThreshold() const
{
	return getCultureThreshold(getCultureLevel());
}

int CvCity::getCultureThreshold(CultureLevelTypes eLevel) const
{
	if (eLevel == NO_CULTURELEVEL)
	{
		return 1;
	}

/************************************************************************************************/
/* Afforess	                  Start		 07/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/* 
	return std::max(1, GC.getGameINLINE().getCultureThreshold((CultureLevelTypes)(std::min((eLevel + 1), (GC.getNumCultureLevelInfos() - 1)))));
*/
	CultureLevelTypes eCultureLevel;
	eCultureLevel = ((CultureLevelTypes)(GC.getNumCultureLevelInfos() - 1));
	for (int iI = 0; iI < GC.getNumCultureLevelInfos(); iI++)
	{
		if (GC.getCultureLevelInfo((CultureLevelTypes)iI).getPrereqGameOption() == NO_GAMEOPTION || GC.getGameINLINE().isOption((GameOptionTypes)GC.getCultureLevelInfo((CultureLevelTypes)iI).getPrereqGameOption()))
		{
			if (getCultureTimes100(getOwnerINLINE())/100 < GC.getGameINLINE().getCultureThreshold((CultureLevelTypes)iI))
			{
				eCultureLevel = ((CultureLevelTypes)iI);
				break;
			}
		}
	}
	return std::max(1, GC.getGameINLINE().getCultureThreshold(eCultureLevel));
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


void CvCity::setCultureLevel(CultureLevelTypes eNewValue, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CultureLevelTypes eOldValue = getCultureLevel();

	if (eOldValue != eNewValue)
	{
		m_eCultureLevel = eNewValue;

		//	Culture level chnage can change our radius so that requires
		//	recalculation of best builds
		AI_markBestBuildValuesStale();

		if (eOldValue != NO_CULTURELEVEL)
		{
			for (int iDX = -eOldValue; iDX <= eOldValue; iDX++)
			{
				for (int iDY = -eOldValue; iDY <= eOldValue; iDY++)
				{
					int iCultureRange = cultureDistance(iDX, iDY, true);

					if (iCultureRange > getCultureLevel())
					{
						if (iCultureRange <= eOldValue)
						{
							FAssert(iCultureRange <= GC.getNumCultureLevelInfos());

							CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								pLoopPlot->changeCultureRangeCities(getOwnerINLINE(), iCultureRange, -1, bUpdatePlotGroups);
							}
						}
					}
				}
			}
		}

/************************************************************************************************/
/* phunny_pharmer             Start		 04/21/10                                               */
/*   ensure that the scaling back of cultural distances occurs before caching                   */
/*     note that the first time the city is built, no caching is necessary since the above loop */
/*     will not be entered (the old value is equal to NO_CULTURELEVEL)                          */
/*       however, on all subsequent calls, the cultureDistance function refers to the culture   */
/*       level m_eCultureLevel, so that cannot be changed until after the function completes    */
/************************************************************************************************/
		m_eCultureLevel = eNewValue;
		clearCultureDistanceCache();
/************************************************************************************************/
/* phunny_pharmer             END                                                               */
/************************************************************************************************/

		if (getCultureLevel() != NO_CULTURELEVEL)
		{
			for (int iDX = -getCultureLevel(); iDX <= getCultureLevel(); iDX++)
			{
				for (int iDY = -getCultureLevel(); iDY <= getCultureLevel(); iDY++)
				{
					int iCultureRange = cultureDistance(iDX, iDY, true);

					if (iCultureRange > eOldValue)
					{
						if (iCultureRange <= getCultureLevel())
						{
							FAssert(iCultureRange <= GC.getNumCultureLevelInfos());

							CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								pLoopPlot->changeCultureRangeCities(getOwnerINLINE(), iCultureRange, 1, bUpdatePlotGroups);
							}
						}
					}
				}
			}
		}

		if (GC.getGameINLINE().isFinalInitialized())
		{
			if ((getCultureLevel() > eOldValue) && (getCultureLevel() > 1))
			{
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_BORDERS_EXPANDED", getNameKey());
					MEMORY_TRACK_EXEMPT();
					AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_CULTURE).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
				}
/************************************************************************************************/
/* Afforess	                  Start		 01/20/10                                               */
/*                                                                                              */
/*    Update Health and Happiness when culture expands                                          */
/************************************************************************************************/
				updateFeatureHappiness();
				updateFeatureHealth();
				updateImprovementHealth();
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				if (getCultureLevel() == (GC.getNumCultureLevelInfos() - 1))
				{
					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							MEMORY_TRACK_EXEMPT();

							if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL", getNameKey(), GC.getCultureLevelInfo(getCultureLevel()).getTextKeyWide());
								AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, GC.getCommerceInfo(COMMERCE_CULTURE).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
							}
							else
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL_UNKNOWN", GC.getCultureLevelInfo(getCultureLevel()).getTextKeyWide());
								AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, GC.getCommerceInfo(COMMERCE_CULTURE).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
							}
						}
					}
				}

				// ONEVENT - Culture growth
				CvEventReporter::getInstance().cultureExpansion(this, getOwnerINLINE());
				
				//Stop Build Culture
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       12/07/09                         denev & jdog5000     */
/*                                                                                              */
/* Bugfix, Odd behavior                                                                         */
/************************************************************************************************/
/* original BTS code
				if (isProductionProcess())
				{
					if (GC.getProcessInfo(getProductionProcess()).getProductionToCommerceModifier(COMMERCE_CULTURE) > 0)
					{
						popOrder(0, false, true);						
					}
				}
*/
				// For AI this is completely unnecessary.  Timing also appears to cause bug with overflow production, 
				// giving extra hammers inappropriately.
				if( isHuman() && !isProductionAutomated() )
				{
					if (isProductionProcess())
					{
						if (GC.getProcessInfo(getProductionProcess()).getProductionToCommerceModifier(COMMERCE_CULTURE) > 0)
						{
							//popOrder(0, false, true);
							m_bPopProductionProcess = true;
						}
					}
				}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
			}
		}
		//Bad code - this should go in checkBuildings with the rest of disabling effects so it's being transferred there.
		//Was causing buildings to be double processed when cities were captured.  And who knows when else as well.
		//int iChange = (int)eNewValue - (int)eOldValue;
		//if (iChange > 0)
		//{
		//	iChange = 1;
		//}
		//if (iChange < 0)
		//{
		//	iChange = -1;
		//}
		////ls612: Remove buildings which obsolete at a certain pop level
		////Re-enable them if the city shrinks?
		//for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		//{
		//	if (eNewValue != NO_CULTURELEVEL)
		//	{
		//		if (getNumActiveBuilding((BuildingTypes) iI) > 0 && 
		//			((iChange > 0 && GC.getBuildingInfo((BuildingTypes) iI).getMaxCultureLevelAllowed() == (int) eNewValue)
		//			|| (iChange < 0 && GC.getBuildingInfo((BuildingTypes) iI).getMaxCultureLevelAllowed() == ((int) eNewValue) - 1)))
		//		{
		//			processBuilding((BuildingTypes) iI, (-getNumActiveBuilding((BuildingTypes) iI) * iChange), true);
		//		}
		//	}
		//}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       08/08/10                              EmperorFool     */
/*                                                                                              */
/* Bugfix, already called by AI_doTurn()                                                        */
/************************************************************************************************/
		AI_updateBestBuild();
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
	}
}


void CvCity::updateCultureLevel(bool bUpdatePlotGroups)
{
	if (getCultureUpdateTimer() > 0)
	{
		return;
	}

	CultureLevelTypes eCultureLevel = ((CultureLevelTypes)0);

/************************************************************************************************/
/* REVOLUTION_MOD                         02/08/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// City borders don't collapse during a revolt, only during occupation right after capture
/* original BTS
	if (!isOccupation())
*/
	int iMaxOccupationTimer = GC.getDefineINT("BASE_OCCUPATION_TURNS") + ((getHighestPopulation() * GC.getDefineINT("OCCUPATION_TURNS_POPULATION_PERCENT")) / 100);
	if( !isOccupation() || ((!GC.getGameINLINE().isOption(GAMEOPTION_NO_REVOLUTION) && (GC.getGameINLINE().getGameTurn() - getGameTurnAcquired()) > iMaxOccupationTimer)) )
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	{
		for (int iI = (GC.getNumCultureLevelInfos() - 1); iI > 0; iI--)
		{
/************************************************************************************************/
/* Afforess	                  Start		 07/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			if (GC.getCultureLevelInfo((CultureLevelTypes)iI).getPrereqGameOption() == NO_GAMEOPTION || GC.getGameINLINE().isOption((GameOptionTypes)GC.getCultureLevelInfo((CultureLevelTypes)iI).getPrereqGameOption()))
			{
				if (getCultureTimes100(getOwnerINLINE())/100 >= GC.getGameINLINE().getCultureThreshold((CultureLevelTypes)iI))
				{
					eCultureLevel = ((CultureLevelTypes)iI);
					break;
				}
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		}
	}

	setCultureLevel(eCultureLevel, bUpdatePlotGroups);
}


int CvCity::getSeaPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	//TB Debug
	//Somehow we are getting under 0 values here and that could cause problems down the road
	//This is definately an xml value caused by the current settings on the Garbage Dock and as a result should actually be a fairly safe fix in this case.
	//This method enforces minimum of 0 without changing the actual value of m_aiSeaPlotYield[eIndex] as the integrity of that value should be maintained.
	int iValue = 0;
	if (m_aiSeaPlotYield[eIndex] < 0)
	{
		iValue = 0;
	}
	else
	{
		iValue = m_aiSeaPlotYield[eIndex];
	}
	return iValue;
}


void CvCity::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiSeaPlotYield[eIndex] = (m_aiSeaPlotYield[eIndex] + iChange);
		FAssert(getSeaPlotYield(eIndex) >= 0);

		updateYield();
	}
}


int CvCity::getRiverPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiRiverPlotYield[eIndex];
}


void CvCity::changeRiverPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiRiverPlotYield[eIndex] += iChange;

		updateYield();
	}
}


// BUG - Building Additional Yield - start
/*
 * Returns the total additional yield that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding, bool bFilter)
{
	//TB Traits begin
	int iRate = getModifiedBaseYieldRate(eIndex);
	//TB Traits end
	int iModifier = getBaseYieldRateModifier(eIndex);
	int iExtra = ((iRate + getAdditionalBaseYieldRateByBuilding(eIndex, eBuilding)) * (iModifier + getAdditionalYieldRateModifierByBuilding(eIndex, eBuilding, bFilter)) / 100) - (iRate * iModifier / 100);
/************************************************************************************************/
/* Afforess	                  Start		 4/22/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			//Team Project (5)
			if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				if (GC.getBuildingInfo((BuildingTypes)iI).isReplaceBuildingClass((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()))
				{
					iExtra -= getAdditionalYieldByBuilding(eIndex, (BuildingTypes)iI);
				}
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

	return iExtra;
}

/*
 * Returns the additional yield rate that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalBaseYieldRateByBuilding(YieldTypes eIndex, BuildingTypes eBuilding)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	bool bObsolete = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	int iExtraRate = 0;

	if (!bObsolete)
	{
		if (kBuilding.getSeaPlotYieldChange(eIndex) != 0)
		{
			int iChange = kBuilding.getSeaPlotYieldChange(eIndex);
			for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				if (isWorkingPlot(iI) && getCityIndexPlot(iI)->isWater())
				{
					iExtraRate += iChange;
				}
			}
		}
		if (kBuilding.getRiverPlotYieldChange(eIndex) != 0)
		{
			int iChange = kBuilding.getRiverPlotYieldChange(eIndex);
			for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				if (isWorkingPlot(iI) && getCityIndexPlot(iI)->isRiver())
				{
					iExtraRate += iChange;
				}
			}
		}
		iExtraRate += kBuilding.getYieldChange(eIndex);
		iExtraRate += ((kBuilding.getYieldPerPopChange(eIndex)*getPopulation())/100);
		iExtraRate += getBuildingYieldChange((BuildingClassTypes)kBuilding.getBuildingClassType(), eIndex);

		// Trade
		int iPlayerTradeYieldModifier = GET_PLAYER(getOwnerINLINE()).getTradeYieldModifier(eIndex);
		if (iPlayerTradeYieldModifier > 0 && (kBuilding.getTradeRouteModifier() != 0 || kBuilding.getForeignTradeRouteModifier() != 0))
		{
			int iTotalTradeYield = 0;
			int iNewTotalTradeYield = 0;
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
			int iTradeProfitDivisor = 100;
#else
			int iTradeProfitDivisor = 10000;
#endif
// BUG - Fractional Trade Routes - end

			for (int iI = 0; iI < getTradeRoutes(); ++iI)
			{
				CvCity* pCity = getTradeCity(iI);
				if (pCity)
				{
					int iTradeProfit = getBaseTradeProfit(pCity);
					int iTradeModifier = totalTradeModifier(pCity);
					int iTradeYield = iTradeProfit * iTradeModifier / iTradeProfitDivisor * iPlayerTradeYieldModifier / 100;
					iTotalTradeYield += iTradeYield;

					iTradeModifier += kBuilding.getTradeRouteModifier();
					if (pCity->getOwnerINLINE() != getOwnerINLINE())
					{
						iTradeModifier += kBuilding.getForeignTradeRouteModifier();
					}
					int iNewTradeYield = iTradeProfit * iTradeModifier / iTradeProfitDivisor * iPlayerTradeYieldModifier / 100;
					iNewTotalTradeYield += iNewTradeYield;
				}
			}

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
			iTotalTradeYield /= 100;
			iNewTotalTradeYield /= 100;
#endif
// BUG - Fractional Trade Routes - end
			iExtraRate += iNewTotalTradeYield - iTotalTradeYield;
		}

		// Specialists
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
		{
			if (kBuilding.getFreeSpecialistCount((SpecialistTypes)iI) != 0)
			{
				iExtraRate += getAdditionalBaseYieldRateBySpecialist(eIndex, (SpecialistTypes)iI, kBuilding.getFreeSpecialistCount((SpecialistTypes)iI));
			}
		}
/************************************************************************************************/
/* Afforess	                  Start		 12/20/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		{
			if (hasBonus((BonusTypes)iI))
			{
				iExtraRate += kBuilding.getBonusYieldChanges(iI, eIndex);
			}
			if (kBuilding.getVicinityBonusYieldChanges(iI, eIndex) != 0 && hasVicinityBonus((BonusTypes)iI))
			{
				iExtraRate += kBuilding.getVicinityBonusYieldChanges(iI, eIndex);
			}
		}
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)iI))
			{
				iExtraRate += kBuilding.getTechYieldChange(iI, eIndex);
			}
		}

		if ((kBuilding.getGlobalTradeRoutes() + kBuilding.getCoastalTradeRoutes() + kBuilding.getTradeRoutes()) != 0)
		{
			int* paiTradeYields;
			paiTradeYields = new int[NUM_YIELD_TYPES];
			calculateExtraTradeRouteProfit((kBuilding.getGlobalTradeRoutes() + kBuilding.getCoastalTradeRoutes() + kBuilding.getTradeRoutes()), paiTradeYields);
			iExtraRate += paiTradeYields[eIndex];
			SAFE_DELETE_ARRAY(paiTradeYields);
		}

		int iFreeSpecialistYield = 0;
		for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
		{
			SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
			if (eNewSpecialist == NO_SPECIALIST) break;			
			//Team Project (1)
			iFreeSpecialistYield += specialistYield(eNewSpecialist, eIndex);// old: GET_PLAYER(getOwnerINLINE()).specialistYield(eNewSpecialist, eIndex);
			//iFreeSpecialistYield += getLocalSpecialistExtraYield(eNewSpecialist, eIndex);
		}
		iExtraRate += iFreeSpecialistYield;
		
		if (kBuilding.getNumPopulationEmployed() > 0)
		{
			int* paiCommerce = new int[NUM_COMMERCE_TYPES];
			int* paiYield = new int[NUM_YIELD_TYPES];
			int iGreatPeopleRate;
			int iHappiness;
			int iHealthiness;
			removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
			iExtraRate += paiYield[eIndex];
			SAFE_DELETE_ARRAY(paiCommerce);
			SAFE_DELETE_ARRAY(paiYield);
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}

	return iExtraRate;
}

/*
 * Returns the additional yield rate modifier that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalYieldRateModifierByBuilding(YieldTypes eIndex, BuildingTypes eBuilding, bool bFilter) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	bool bObsolete = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	int iExtraModifier = 0;

	if (!bObsolete)
	{
		iExtraModifier += kBuilding.getYieldModifier(eIndex);
		if (isPower())
		{
			iExtraModifier += kBuilding.getPowerYieldModifier(eIndex);
		}
		else
		{
			if (kBuilding.isPower() || kBuilding.isAreaCleanPower() || (kBuilding.getPowerBonus() != NO_BONUS && hasBonus((BonusTypes)kBuilding.getPowerBonus())))
			{
				for (int i = 0; i < GC.getNumBuildingInfos(); i++)
				{
					iExtraModifier += getNumActiveBuilding((BuildingTypes)i) * GC.getBuildingInfo((BuildingTypes)i).getPowerYieldModifier(eIndex);
				}
			}
		}
		if (eIndex == YIELD_PRODUCTION && !bFilter)
		{
			iExtraModifier += kBuilding.getMilitaryProductionModifier() / 2; // AIAndy: It does not make sense to count the production increases for specific domains fully
			iExtraModifier += kBuilding.getSpaceProductionModifier() / 4;
			iExtraModifier += kBuilding.getGlobalSpaceProductionModifier() / 2;

			int iMaxModifier = 0;
			for (int i = 0; i < NUM_DOMAIN_TYPES; i++)
			{
				iMaxModifier = std::max(iMaxModifier, kBuilding.getDomainProductionModifier((DomainTypes)i));
			}
			iExtraModifier += iMaxModifier / 4;
		}
		for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		{
			if (hasBonus((BonusTypes)iI))
			{
				iExtraModifier += kBuilding.getBonusYieldModifier(iI, eIndex);
			}
		}
/************************************************************************************************/
/* Afforess	                  Start		 12/20/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)iI))
			{
				iExtraModifier += kBuilding.getTechYieldModifier(iI, eIndex);
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}

	return iExtraModifier;
}
// BUG - Building Additional Yield - end


// BUG - Specialist Additional Yield - start
/*
 * Returns the total additional yield that changing the number of given specialists will provide/remove.
 */
int CvCity::getAdditionalYieldBySpecialist(YieldTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	//TB Traits begin
	int iRate = getModifiedBaseYieldRate(eIndex);
	//TB Traits end
	int iModifier = getBaseYieldRateModifier(eIndex);
	int iExtra = ((iRate + getAdditionalBaseYieldRateBySpecialist(eIndex, eSpecialist, iChange)) * iModifier / 100) - (iRate * iModifier / 100);

	return iExtra;
}

/*
 * Returns the additional yield rate that changing the number of given specialists will provide/remove.
 */
int CvCity::getAdditionalBaseYieldRateBySpecialist(YieldTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	
	CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eSpecialist);
/************************************************************************************************/
/* Afforess	                  Start		 07/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	return iChange * (kSpecialist.getYieldChange(eIndex) + GET_PLAYER(getOwnerINLINE()).getSpecialistExtraYield(eSpecialist, eIndex));
*/
	//TB Traits begin
	return iChange * (kSpecialist.getYieldChange(eIndex) + GET_PLAYER(getOwnerINLINE()).getExtraSpecialistYield(eSpecialist, eIndex) + (GET_PLAYER(getOwnerINLINE()).getSpecialistYieldPercentChanges(eSpecialist, eIndex) / 100));
	//TB Traits end
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}
// BUG - Specialist Additional Yield - end


int CvCity::getBaseYieldRate(YieldTypes eIndex)	const													
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	int iResult = m_aiBaseYieldRate[eIndex] + popYield(eIndex);

	return iResult; 
}


int CvCity::getBaseYieldRateModifier(YieldTypes eIndex, int iExtra) const
{
	int iModifier;

	iModifier = getYieldRateModifier(eIndex);

	iModifier += getBonusYieldRateModifier(eIndex);

	if (isPower())
	{
		iModifier += getPowerYieldRateModifier(eIndex);
	}

	if (area() != NULL)
	{
		iModifier += area()->getYieldRateModifier(getOwnerINLINE(), eIndex);
	}

	iModifier += GET_PLAYER(getOwnerINLINE()).getYieldRateModifier(eIndex);

	if (isCapital())
	{
		iModifier += GET_PLAYER(getOwnerINLINE()).getCapitalYieldRateModifier(eIndex);
	}
	
/************************************************************************************************/
/* Afforess	                  Start		 01/25/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iModifier += calculateBuildingYieldModifier(eIndex);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	iModifier += iExtra;

	// note: player->invalidateYieldRankCache() must be called for anything that is checked here
	// so if any extra checked things are added here, the cache needs to be invalidated
	return std::max(0, (iModifier + 100));
}


int CvCity::getYieldRate(YieldTypes eIndex) const
{
	PROFILE_FUNC();

	return ((getModifiedBaseYieldRate(eIndex) * getBaseYieldRateModifier(eIndex)) / 100);
}


void CvCity::setBaseYieldRate(YieldTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (m_aiBaseYieldRate[eIndex] != iNewValue)
	{
		m_aiBaseYieldRate[eIndex] = iNewValue;
		FAssert(eIndex >= 0);
#ifdef YIELD_VALUE_CACHING
		ClearYieldValueCache();
#endif

		setCommerceDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);

			if (isCitySelected())
			{
				gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
				gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			}
		}
	}
}


void CvCity::changeBaseYieldRate(YieldTypes eIndex, int iChange)										
{
	setBaseYieldRate(eIndex, (m_aiBaseYieldRate[eIndex] + iChange));
}

int CvCity::popYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return ((m_aiBaseYieldPerPopRate[eIndex] * getPopulation())/100);
}

int CvCity::getBaseYieldPerPopRate(YieldTypes eIndex)	const													
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	return m_aiBaseYieldPerPopRate[eIndex]; 
}
void CvCity::setBaseYieldPerPopRate(YieldTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (m_aiBaseYieldPerPopRate[eIndex] != iNewValue)
	{
		m_aiBaseYieldPerPopRate[eIndex] = iNewValue;
		FAssert(eIndex >= 0);
#ifdef YIELD_VALUE_CACHING
		ClearYieldValueCache();
#endif

		setCommerceDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);

			if (isCitySelected())
			{
				gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
				gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			}
		}
	}
}


void CvCity::changeBaseYieldPerPopRate(YieldTypes eIndex, int iChange)										
{
	setBaseYieldPerPopRate(eIndex, (getBaseYieldPerPopRate(eIndex) + iChange));
}

int CvCity::getYieldRateModifier(YieldTypes eIndex)	const														
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiYieldRateModifier[eIndex];
}


void CvCity::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiYieldRateModifier[eIndex] = (m_aiYieldRateModifier[eIndex] + iChange);
		FAssert(eIndex >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getPowerYieldRateModifier(YieldTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiPowerYieldRateModifier[eIndex];
}


void CvCity::changePowerYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiPowerYieldRateModifier[eIndex] = (m_aiPowerYieldRateModifier[eIndex] + iChange);
		FAssert(getYieldRate(eIndex) >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getBonusYieldRateModifier(YieldTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiBonusYieldRateModifier[eIndex];
}


void CvCity::changeBonusYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiBonusYieldRateModifier[eIndex] = (m_aiBonusYieldRateModifier[eIndex] + iChange);
		FAssert(getYieldRate(eIndex) >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getTradeYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiTradeYield[eIndex];
}


int CvCity::totalTradeModifier(CvCity* pOtherCity) const
{
	int iModifier = 100;

	iModifier += getTradeRouteModifier();

	iModifier += getPopulationTradeModifier();
	
/************************************************************************************************/
/* Afforess	                  Start		 03/8/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iModifier += GET_TEAM(getTeam()).getTradeModifier();
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	if (isConnectedToCapital())
	{
		iModifier += GC.getDefineINT("CAPITAL_TRADE_MODIFIER");
	}

	if (NULL != pOtherCity)
	{
		if (area() != pOtherCity->area())
		{
			iModifier += GC.getDefineINT("OVERSEAS_TRADE_MODIFIER");
		}

		if (getTeam() != pOtherCity->getTeam())
		{
			iModifier += getForeignTradeRouteModifier();
/************************************************************************************************/
/* Afforess	                  Start		 3/8/09                                                 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			iModifier += GET_PLAYER(getOwnerINLINE()).getForeignTradeRouteModifier();
			iModifier += GET_TEAM(getTeam()).getForeignTradeModifier();
			if (GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_ECONOMY))
			{
				CvPlayer &kOtherPlayer = GET_PLAYER(pOtherCity->getOwnerINLINE());
				CvPlayer &kPlayer = GET_PLAYER(getOwnerINLINE());
				for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
				{
					if (kPlayer.getCivics((CivicOptionTypes)iI) == kOtherPlayer.getCivics((CivicOptionTypes)iI))
					{
						iModifier += GC.getCivicInfo(kPlayer.getCivics((CivicOptionTypes)iI)).getSharedCivicTradeRouteModifier();
					}
				}
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
			iModifier += getPeaceTradeModifier(pOtherCity->getTeam());
		}
	}

	return iModifier;
}

int CvCity::getPopulationTradeModifier() const
{
	return std::max(0, (getPopulation() + GC.getDefineINT("OUR_POPULATION_TRADE_MODIFIER_OFFSET")) * GC.getDefineINT("OUR_POPULATION_TRADE_MODIFIER"));
}

int CvCity::getPeaceTradeModifier(TeamTypes eTeam) const
{
	FAssert(NO_TEAM != eTeam);
	FAssert(eTeam != getTeam());

	if (atWar(eTeam, getTeam()))
	{
		return 0;
	}

	int iPeaceTurns = std::min(GC.getDefineINT("FOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS"), GET_TEAM(getTeam()).AI_getAtPeaceCounter(eTeam));

	if (GC.getGameINLINE().getElapsedGameTurns() <= iPeaceTurns)
	{
		return GC.getDefineINT("FOREIGN_TRADE_MODIFIER");
	}

	return ((GC.getDefineINT("FOREIGN_TRADE_MODIFIER") * iPeaceTurns) / std::max(1, GC.getDefineINT("FOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS")));
}

int CvCity::getBaseTradeProfit(CvCity* pCity) const
{
	int iProfit = std::min(pCity->getPopulation() * GC.getDefineINT("THEIR_POPULATION_TRADE_PERCENT"), plotDistance(getX_INLINE(), getY_INLINE(), pCity->getX_INLINE(), pCity->getY_INLINE()) * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTradeProfitPercent());

	iProfit *= GC.getDefineINT("TRADE_PROFIT_PERCENT");
	iProfit /= 100;

	iProfit = std::max(100, iProfit);

	return iProfit;
}

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE

// Note: getBaseTradeProfit() already returns a times-100 value.

/*
 * Returns the fractional (times 100) trade profit for the route to the given city.
 */
int CvCity::calculateTradeProfitTimes100(CvCity* pCity) const
{
	int iProfit = getBaseTradeProfit(pCity);

	iProfit *= totalTradeModifier(pCity);
	iProfit /= 100;

	return iProfit;
}

/*
 * Returns the truncated trade profit for the route to the given city.
 *
 * This function is kept only for old Python code.
 */
int CvCity::calculateTradeProfit(CvCity* pCity) const
{
	return calculateTradeProfitTimes100(pCity) / 100;
}

#else

// unchanged

int CvCity::calculateTradeProfit(CvCity* pCity) const
{
	int iProfit = getBaseTradeProfit(pCity);

	iProfit *= totalTradeModifier(pCity);
	iProfit /= 10000;

	return iProfit;
}

#endif
// BUG - Fractional Trade Routes - end

int CvCity::calculateTradeYield(YieldTypes eIndex, int iTradeProfit) const
{
	if ((iTradeProfit != 0) && (GET_PLAYER(getOwnerINLINE()).getTradeYieldModifier(eIndex) > 0))
	{
		return ((iTradeProfit * GET_PLAYER(getOwnerINLINE()).getTradeYieldModifier(eIndex)) / 100);
	}
	else
	{
		return 0;
	}
}

// BUG - Trade Totals - start
/*
 * Adds the yield and count for each trade route with eWithPlayer.
 *
 * The yield and counts are not reset to zero.
 * If Fractional Trade Routes is enabled and bRound is false, or if bBase if true, the yield values are left times 100.
 */
void CvCity::calculateTradeTotals(YieldTypes eIndex, int& iDomesticYield, int& iDomesticRoutes, int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer, bool bRound, bool bBase) const
{
	if (!isDisorder())
	{
		int iCityDomesticYield = 0;
		int iCityDomesticRoutes = 0;
		int iCityForeignYield = 0;
		int iCityForeignRoutes = 0;
		int iNumTradeRoutes = getTradeRoutes();
		PlayerTypes ePlayer = getOwnerINLINE();

		for (int iI = 0; iI < iNumTradeRoutes; ++iI)
		{
			CvCity* pTradeCity = getTradeCity(iI);
			if (pTradeCity && pTradeCity->getOwnerINLINE() >= 0 && (NO_PLAYER == eWithPlayer || pTradeCity->getOwnerINLINE() == eWithPlayer))
			{
				int iTradeYield;

				if (bBase)
				{
					iTradeYield = getBaseTradeProfit(pTradeCity);
				}
				else
				{
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
					int iTradeProfit = calculateTradeProfitTimes100(pTradeCity);
#else
					int iTradeProfit = calculateTradeProfit(pTradeCity);
#endif
// BUG - Fractional Trade Routes - end
					iTradeYield = calculateTradeYield(YIELD_COMMERCE, iTradeProfit);
				}

				if (pTradeCity->getOwnerINLINE() == ePlayer)
				{
					iCityDomesticYield += iTradeYield;
					iCityDomesticRoutes++;
				}
				else
				{
					iCityForeignYield += iTradeYield;
					iCityForeignRoutes++;
				}
			}
		}

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		if (bRound)
		{
			iDomesticYield += iCityDomesticYield / 100;
			iDomesticRoutes += iCityDomesticRoutes / 100;
			iForeignYield += iCityForeignYield / 100;
			iForeignRoutes += iCityForeignRoutes / 100;
		}
		else
#endif
// BUG - Fractional Trade Routes - end
		{
			iDomesticYield += iCityDomesticYield;
			iDomesticRoutes += iCityDomesticRoutes;
			iForeignYield += iCityForeignYield;
			iForeignRoutes += iCityForeignRoutes;
		}
	}
}

/*
 * Returns the total trade yield.
 *
 * If Fractional Trade Routes is enabled or bBase is true, the yield value is left times 100.
 * UNUSED
 */
int CvCity::calculateTotalTradeYield(YieldTypes eIndex, PlayerTypes eWithPlayer, bool bRound, bool bBase) const
{
	int iDomesticYield = 0;
	int iDomesticRoutes = 0;
	int iForeignYield = 0;
	int iForeignRoutes = 0;
	
	calculateTradeTotals(eIndex, iDomesticYield, iDomesticRoutes, iForeignYield, iForeignRoutes, eWithPlayer, bRound, bBase);
	return iDomesticYield + iForeignRoutes;
}
// BUG - Trade Totals - end


void CvCity::setTradeYield(YieldTypes eIndex, int iNewValue)
{
	int iOldValue;

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	iOldValue = getTradeYield(eIndex);

	if (iOldValue != iNewValue)
	{
		m_aiTradeYield[eIndex] = iNewValue;
		FAssert(getTradeYield(eIndex) >= 0);

		changeBaseYieldRate(eIndex, (iNewValue - iOldValue));
	}
}


int CvCity::getExtraSpecialistYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiExtraSpecialistYield[eIndex];
}


int CvCity::getExtraSpecialistYield(YieldTypes eIndex, SpecialistTypes eSpecialist) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos expected to be >= 0");
	//TB Traits begin
//Team Project (1)
	return (specialistCount(eSpecialist) * (getLocalSpecialistExtraYield(eSpecialist, eIndex) + GET_PLAYER(getOwnerINLINE()).getExtraSpecialistYield(eSpecialist, eIndex) + GET_PLAYER(getOwnerINLINE()).getSpecialistExtraYield(eIndex)));
// Original:
//	return ((getSpecialistCount(eSpecialist) + getFreeSpecialistCount(eSpecialist)) * (GET_PLAYER(getOwnerINLINE()).getExtraSpecialistYield(eSpecialist, eIndex) + GET_PLAYER(getOwnerINLINE()).getSpecialistExtraYield(eIndex)));
	//TB Traits end
}


void CvCity::updateExtraSpecialistYield(YieldTypes eYield)
{
	FAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");

	int iOldYield = getExtraSpecialistYield(eYield);
	int iNewYield = 0;

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iNewYield += getExtraSpecialistYield(eYield, ((SpecialistTypes)iI));
	}

	if (iOldYield != iNewYield)
	{
		m_aiExtraSpecialistYield[eYield] = iNewYield;

		changeBaseYieldRate(eYield, (iNewYield - iOldYield));
	}
}


void CvCity::updateExtraSpecialistYield()
{
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		updateExtraSpecialistYield((YieldTypes)iI);
	}
}

int CvCity::getExtraSpecialistCommerceTotal(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiExtraSpecialistCommerce[eIndex];
}

int CvCity::getExtraSpecialistCommerce(CommerceTypes eIndex, SpecialistTypes eSpecialist) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos expected to be >= 0");
	return (specialistCount(eSpecialist) * (getLocalSpecialistExtraCommerce(eSpecialist, eIndex) + GET_PLAYER(getOwnerINLINE()).getExtraSpecialistCommerce(eSpecialist, eIndex) + GET_PLAYER(getOwnerINLINE()).getSpecialistExtraCommerce(eIndex)));
}

void CvCity::updateExtraSpecialistCommerce(CommerceTypes eCommerce)
{
	FAssertMsg(eCommerce >= 0, "eCommerce expected to be >= 0");
	FAssertMsg(eCommerce < NUM_COMMERCE_TYPES, "eCommerce expected to be < NUM_COMMERCE_TYPES");

	int iOldCommerce = getExtraSpecialistCommerceTotal(eCommerce);

	int iNewCommerce = 0;

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iNewCommerce += getExtraSpecialistCommerce(eCommerce, ((SpecialistTypes)iI));
	}

	if (iOldCommerce != iNewCommerce)
	{
		m_aiExtraSpecialistCommerce[eCommerce] = iNewCommerce;
	}
}

void CvCity::updateExtraSpecialistCommerce()
{
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		updateExtraSpecialistCommerce((CommerceTypes)iI);
	}

	setCommerceDirty();

	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		setInfoDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
		}
	}
}


int CvCity::getCommerceRate(CommerceTypes eIndex) const
{
	return getCommerceRateTimes100(eIndex) / 100;
}

int CvCity::getCommerceRateTimes100(CommerceTypes eIndex) const
{
	PROFILE_FUNC();

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

	if ( m_abCommerceRateDirty[eIndex] )
	{
		updateCommerce(eIndex);
	}

	int iRate = m_aiCommerceRate[eIndex];
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		if (eIndex == COMMERCE_CULTURE)
		{
			iRate += m_aiCommerceRate[COMMERCE_ESPIONAGE];
		}
		else if (eIndex == COMMERCE_ESPIONAGE)
		{
			iRate = 0;
		}
	}

	return iRate;
}


int CvCity::getCommerceFromPercent(CommerceTypes eIndex, int iYieldRate) const
{
	int iCommerce = ((iYieldRate * GET_PLAYER(getOwnerINLINE()).getCommercePercent(eIndex)) / 100);

	if (eIndex == COMMERCE_GOLD)
	{
		iCommerce += (iYieldRate - iCommerce - getCommerceFromPercent(COMMERCE_RESEARCH, iYieldRate) - getCommerceFromPercent(COMMERCE_CULTURE, iYieldRate) - getCommerceFromPercent(COMMERCE_ESPIONAGE, iYieldRate));
	}

	return iCommerce;
}


int CvCity::getBaseCommerceRate(CommerceTypes eIndex) const
{
	return (getBaseCommerceRateTimes100(eIndex) / 100);
}

int CvCity::getBaseCommerceRateTimes100(CommerceTypes eIndex) const
{
	PROFILE_FUNC();

	if ( m_abCommerceRateDirty[eIndex] )
	{
		updateCommerce(eIndex);
	}

	//STEP 1 : Slider
	int iBaseCommerceRate = getCommerceFromPercent(eIndex, getYieldRate(YIELD_COMMERCE) * 100);

	//STEP 2 : Commerce Changes from specialists	
	int iSpecialistCommerce = getSpecialistCommerce(eIndex) + getExtraSpecialistCommerceTotal(eIndex);
	/*iSpecialistCommerce += ((getSpecialistPopulation() + getNumGreatPeople()) * GET_PLAYER(getOwnerINLINE()).getSpecialistExtraCommerce(eIndex));*/
	iBaseCommerceRate += 100 * iSpecialistCommerce;

	//STEP 3 : Religion Commerce
	iBaseCommerceRate += 100 * getReligionCommerce(eIndex);
	
	//STEP 4 : Corporation Commerce
	iBaseCommerceRate += 100 * getCorporationCommerce(eIndex);
	
	//STEP 5 : Building Commerce
	int iBuildingCommerce = getBuildingCommerce(eIndex);
	iBuildingCommerce *= 100;
	iBuildingCommerce += getBonusCommercePercentChanges(eIndex);

	iBaseCommerceRate += iBuildingCommerce;
	
	//STEP 6 : Free City Commerce (player tallied from civics/traits a change value to all cities commerce output)
	iBaseCommerceRate += 100 * GET_PLAYER(getOwnerINLINE()).getFreeCityCommerce(eIndex);

	//STEP 7 : Commerce Attacks
	iBaseCommerceRate += 100 * getCommerceAttacks(eIndex);

	//STEP 8 : Minted Commerce
	if (eIndex == COMMERCE_GOLD)
	{
		iBaseCommerceRate += getMintedCommerceTimes100();
	}

	//STEP 9 : Golden Age Base Commerce Changes (usually trait driven though it might be interesting to enable this on civics.)
	if (GET_PLAYER(getOwnerINLINE()).isGoldenAge())
	{
		iBaseCommerceRate += 100 * (GET_PLAYER(getOwnerINLINE()).getGoldenAgeCommerce(eIndex));
	}
	return iBaseCommerceRate;
}

/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*Was:
int CvCity::getTotalCommerceRateModifier(CommerceTypes eIndex) const
{
	return std::max(0, (getCommerceRateModifier(eIndex) + GET_PLAYER(getOwnerINLINE()).getCommerceRateModifier(eIndex) + ((isCapital()) ? GET_PLAYER(getOwnerINLINE()).getCapitalCommerceRateModifier(eIndex) : 0) + 100));
}
*/
int CvCity::getTotalCommerceRateModifier(CommerceTypes eIndex) const
{
	PROFILE_FUNC();
	
	if ( m_totalCommerceRateModifier[eIndex] == MIN_INT )
	{
		int iTotal = 100;
		//STEP 1 : Bonus Commerce Rate Modifier
		iTotal += calculateBonusCommerceRateModifier(eIndex);

		//STEP 2 : Buildings
			//Base
		iTotal += getCommerceRateModifier(eIndex);
			//From modifiers to existing buildings from buildings and techs and such
		iTotal += calculateBuildingCommerceModifier(eIndex);
			//Player level building effects
		iTotal += GET_PLAYER(getOwnerINLINE()).getCommerceRateModifierfromBuildings(eIndex);

		//STEP 3 : Events
		iTotal += GET_PLAYER(getOwnerINLINE()).getCommerceRateModifierfromEvents(eIndex);

		//STEP 4 : Player level 
		//(Noting subtractions so it's easier to follow in comparison to the Text Manager.)
		//Aside from those noted, Player level modifiers also include Civics, Traits, and Techs
		//Events and buildings were isolated out so they could be expressed in the above categories
		iTotal += (GET_PLAYER(getOwnerINLINE()).getCommerceRateModifier(eIndex)
			- GET_PLAYER(getOwnerINLINE()).getCommerceRateModifierfromEvents(eIndex)
			- GET_PLAYER(getOwnerINLINE()).getCommerceRateModifierfromBuildings(eIndex));

		//STEP 5 : Capital Modifier
		if (isCapital())
		{
			iTotal += GET_PLAYER(getOwnerINLINE()).getCapitalCommerceRateModifier(eIndex);
		}

		m_totalCommerceRateModifier[eIndex] = std::max(1, iTotal);
	}

	return m_totalCommerceRateModifier[eIndex];
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

void CvCity::setCommerceModifierDirty(CommerceTypes eCommerce)
{
	m_totalCommerceRateModifier[eCommerce] = MIN_INT;

	setCommerceDirty(eCommerce);
}

void CvCity::setCommerceDirty(CommerceTypes eCommerce)
{
	if ( eCommerce == NO_COMMERCE )
	{
		for(int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			setCommerceDirty((CommerceTypes)iI);
		}
	}
	else
	{
		m_abCommerceRateDirty[eCommerce] = true;
		if (getOwnerINLINE() != NO_PLAYER)
		{
			GET_PLAYER(getOwnerINLINE()).setCommerceDirty(eCommerce, true);
		}
	}
}

void CvCity::updateCommerce(CommerceTypes eIndex, bool bForce) const
{
	if ( !GC.getGameINLINE().isRecalculatingModifiers() )
	{
		if ( eIndex == NO_COMMERCE )
		{
			GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache();

			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				updateCommerce((CommerceTypes)iI, bForce);
			}
		}
		else
		{
			FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
			FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

			if (bForce || m_abCommerceRateDirty[eIndex])
			{
				m_abCommerceRateDirty[eIndex] = false;

				int iOldCommerce = m_aiCommerceRate[eIndex];
				int iNewCommerce = 0;

				if (!isDisorder())
				{
					iNewCommerce = getBaseCommerceRateTimes100(eIndex);
					
					//	Don't apply rate modifiers to negative commerce or you get counter-intuitive results
					//	like intelligence agencies makign your negative espionage worse!
					if ( iNewCommerce > 0 )
					{
						iNewCommerce = (iNewCommerce * getTotalCommerceRateModifier(eIndex)) / 100;
					}
					else
					{
						iNewCommerce = (iNewCommerce * 100) / getTotalCommerceRateModifier(eIndex);
					}
					iNewCommerce += getYieldRate(YIELD_PRODUCTION) * getProductionToCommerceModifier(eIndex);
				}

				//	Culture and science cannot be negative
				if ( iNewCommerce < 0 && (eIndex == COMMERCE_CULTURE || eIndex == COMMERCE_RESEARCH) )
				{
					iNewCommerce = 0;
				}

				if (iOldCommerce != iNewCommerce)
				{
					m_aiCommerceRate[eIndex] = iNewCommerce;

					GET_PLAYER(getOwnerINLINE()).invalidateCommerceRankCache(eIndex);

					GET_PLAYER(getOwnerINLINE()).changeCommerceRate(eIndex, (iNewCommerce - iOldCommerce));

					if (isCitySelected())
					{
						gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
						gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
					}
				}
			}
		}
	}
}

int CvCity::getProductionToCommerceModifier(CommerceTypes eIndex) const										 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiProductionToCommerceModifier[eIndex];
}


void CvCity::changeProductionToCommerceModifier(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

	if (iChange != 0)
	{
		m_aiProductionToCommerceModifier[eIndex] = (m_aiProductionToCommerceModifier[eIndex] + iChange);

		setCommerceDirty(eIndex);
	}
}


int CvCity::getBuildingCommerce(CommerceTypes eIndex) const												 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiBuildingCommerce[eIndex];
}


int CvCity::getBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_FUNC();

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "GC.getNumBuildingInfos expected to be >= 0");


	if (getNumBuilding(eBuilding) > 0)
	{
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);	
		if (!(kBuilding.isCommerceChangeOriginalOwner(eIndex)) || (getBuildingOriginalOwner(eBuilding) == getOwnerINLINE()))
		{
			int iCommerce = kBuilding.getObsoleteSafeCommerceChange(eIndex) * getNumBuilding(eBuilding);

			if (getNumActiveBuilding(eBuilding) > 0)
			{
				int iBaseCommerceChange = kBuilding.getCommerceChange(eIndex);

				if ( iBaseCommerceChange < 0 && eIndex == COMMERCE_GOLD && GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE") )
				{
					iBaseCommerceChange = 0;
				}

				iBaseCommerceChange += ((kBuilding.getCommercePerPopChange(eIndex) * getPopulation())/100);

				//Team Project (5)
				if (!isReligiouslyDisabledBuilding(eBuilding))
				{
					iCommerce += (iBaseCommerceChange + getBuildingCommerceChange((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType(), eIndex)) * getNumActiveBuilding(eBuilding);

					if (GC.getBuildingInfo(eBuilding).getReligionType() != NO_RELIGION)
					{
						if (GC.getBuildingInfo(eBuilding).getReligionType() == GET_PLAYER(getOwnerINLINE()).getStateReligion())
						{
								iCommerce += GET_PLAYER(getOwnerINLINE()).getStateReligionBuildingCommerce(eIndex) * getNumActiveBuilding(eBuilding);
						}
					}

					if (GC.getBuildingInfo(eBuilding).getGlobalReligionCommerce() != NO_RELIGION)
					{
						iCommerce += (GC.getReligionInfo((ReligionTypes)(GC.getBuildingInfo(eBuilding).getGlobalReligionCommerce())).getGlobalReligionCommerce(eIndex) * GC.getGameINLINE().countReligionLevels((ReligionTypes)(GC.getBuildingInfo(eBuilding).getGlobalReligionCommerce()))) * getNumActiveBuilding(eBuilding);
					}
				}

				if (GC.getBuildingInfo(eBuilding).getGlobalCorporationCommerce() != NO_CORPORATION)
				{
					iCommerce += (GC.getCorporationInfo((CorporationTypes)(GC.getBuildingInfo(eBuilding).getGlobalCorporationCommerce())).getHeadquarterCommerce(eIndex) * GC.getGameINLINE().countCorporationLevels((CorporationTypes)(GC.getBuildingInfo(eBuilding).getGlobalCorporationCommerce()))) * getNumActiveBuilding(eBuilding);
				}
			}

			if ((GC.getBuildingInfo(eBuilding).getCommerceChangeDoubleTime(eIndex) != 0) &&
				(getBuildingOriginalTime(eBuilding) != MIN_INT) &&
				((GC.getGameINLINE().getGameTurnYear() - getBuildingOriginalTime(eBuilding)) >= GC.getBuildingInfo(eBuilding).getCommerceChangeDoubleTime(eIndex)))
			{
				return (iCommerce * 2);
			}

			return iCommerce;
		}
	}

	return 0;
}

int CvCity::getOrbitalBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_FUNC();

	//ls612: Orbital Buildings have their commerce handled in a special manner
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "GC.getNumBuildingInfos expected to be >= 0");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	if (getNumActiveBuilding(eBuilding) > 0)
	{
		int iCommerce = 0;
		int iNumOrbital = GET_PLAYER(getOwnerINLINE()).countNumCitiesWithOrbitalInfrastructure();
		
		//CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);	
		int iBaseCommerceChange = kBuilding.getCommerceChange(eIndex);

		if ( iBaseCommerceChange < 0 && eIndex == COMMERCE_GOLD && GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE") )
		{
			iBaseCommerceChange = 0;
		}
		
		iBaseCommerceChange += ((kBuilding.getCommercePerPopChange(eIndex) * getPopulation())/100);

		if (iBaseCommerceChange != 0)
		{
			if (hasOrbitalInfrastructure())
			{
				iCommerce += std::min((iBaseCommerceChange * iNumOrbital), getPopulation());
			}

			if (!hasOrbitalInfrastructure())
			{
				iCommerce += std::min((iBaseCommerceChange * iNumOrbital), getPopulation());
				if (iCommerce != 0)
				{
					iCommerce /= 2;
				}
			}
		}

		return iCommerce;
	}

	return 0;
}
		
		
// BUG - Building Additional Commerce - start
/*
 * Returns the rounded total additional commerce that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 * Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE.
 */
int CvCity::getAdditionalCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding)
{
	return getAdditionalCommerceTimes100ByBuilding(eIndex, eBuilding) / 100;
}
/************************************************************************************************/
/* Afforess	                  Start		 09/07/10                                               */
/*                                                                                              */
/*   Originally By the BUG Mod - Heavily Modified to Include new XML Tags                       */
/************************************************************************************************/

/*
 * Returns the total additional commerce times 100 that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 * Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE.
 */
int CvCity::getAdditionalCommerceTimes100ByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding)
{
	int iExtraRateTimes100 = getAdditionalBaseCommerceRateByBuildingTimes100(eIndex, eBuilding);
	int iExtraModifier = getAdditionalCommerceRateModifierByBuilding(eIndex, eBuilding);
	if (iExtraRateTimes100 == 0 && iExtraModifier == 0)
	{
		return 0;
	}

	int iRateTimes100 = getBaseCommerceRateTimes100(eIndex);
	int iModifier = getTotalCommerceRateModifier(eIndex);

	int iCommerceWithoutBuilding;
	if ( iRateTimes100 > 0 )
	{
		iCommerceWithoutBuilding = (iRateTimes100 * iModifier) / 100;
	}
	else
	{
		iCommerceWithoutBuilding = (iRateTimes100 * 100) / iModifier;
	}

	int iCommerceWithBuilding;
	if ( iRateTimes100 + iExtraRateTimes100 > 0 )
	{
		iCommerceWithBuilding = ((iRateTimes100 + iExtraRateTimes100) * (iModifier + iExtraModifier)) / 100;
	}
	else
	{
		iCommerceWithBuilding = ((iRateTimes100 + iExtraRateTimes100) * 100) / (iModifier + iExtraModifier);
	}

	int iExtraTimes100 = iCommerceWithBuilding - iCommerceWithoutBuilding;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			//Team Project (5)
			if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				if (GC.getBuildingInfo((BuildingTypes)iI).isReplaceBuildingClass((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()))
				{
					iExtraTimes100 -= getAdditionalCommerceTimes100ByBuilding(eIndex, (BuildingTypes)iI);
				}
			}
		}
	}

	return iExtraTimes100;
}

/*
 * Returns the additional base commerce rate constructing the given building will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 * Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE.
 */
int CvCity::getAdditionalBaseCommerceRateByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding)
{
	bool bNoEspionage = GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE);

	if (bNoEspionage && eIndex == COMMERCE_ESPIONAGE)
	{
		return 0;
	}

	int iExtraRate = getAdditionalBaseCommerceRateByBuildingImpl(eIndex, eBuilding);
	if (bNoEspionage && eIndex == COMMERCE_CULTURE)
	{
		iExtraRate += getAdditionalBaseCommerceRateByBuildingImpl(COMMERCE_ESPIONAGE, eBuilding);
	}
	return iExtraRate;
}


int CvCity::getAdditionalBaseCommerceRateByBuildingTimes100(CommerceTypes eIndex, BuildingTypes eBuilding)
{
	int iExtraRateTimes100 = 100 * getAdditionalBaseCommerceRateByBuilding(eIndex, eBuilding);

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iExtraRateTimes100 += (kBuilding.getBonusCommercePercentChanges(iI, eIndex));
		}
	}
	
	return iExtraRateTimes100;
}


/*
 * Returns the additional base commerce rate constructing the given building will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalBaseCommerceRateByBuildingImpl(CommerceTypes eIndex, BuildingTypes eBuilding)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	bool bObsolete = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	int iExtraRate = 0;

	iExtraRate += kBuilding.getObsoleteSafeCommerceChange(eIndex);
	if (!bObsolete)
	{
		int iBaseCommerceChange = kBuilding.getCommerceChange(eIndex);
		if ( iBaseCommerceChange < 0 && eIndex == COMMERCE_GOLD && GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE") )
		{
			iBaseCommerceChange = 0;
		}
		iBaseCommerceChange += ((kBuilding.getCommercePerPopChange(eIndex) * getPopulation())/100);
		iExtraRate += iBaseCommerceChange;
		iExtraRate += getBuildingCommerceChange((BuildingClassTypes)kBuilding.getBuildingClassType(), eIndex);
		if (kBuilding.getReligionType() != NO_RELIGION)
		{
			if (kBuilding.getReligionType() == GET_PLAYER(getOwnerINLINE()).getStateReligion())
			{
				iExtraRate += GET_PLAYER(getOwnerINLINE()).getStateReligionBuildingCommerce(eIndex);
			}
		}
		if (kBuilding.getGlobalReligionCommerce() != NO_RELIGION)
		{
			iExtraRate += GC.getReligionInfo((ReligionTypes)(kBuilding.getGlobalReligionCommerce())).getGlobalReligionCommerce(eIndex) * GC.getGameINLINE().countReligionLevels((ReligionTypes)(kBuilding.getGlobalReligionCommerce()));
		}
		if (kBuilding.getGlobalCorporationCommerce() != NO_CORPORATION)
		{
			iExtraRate += GC.getCorporationInfo((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())).getHeadquarterCommerce(eIndex) * GC.getGameINLINE().countCorporationLevels((CorporationTypes)(kBuilding.getGlobalCorporationCommerce()));
		}
		// ignore double-time check since this assumes you are building it this turn

		// Specialists
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
		{
			if (kBuilding.getFreeSpecialistCount((SpecialistTypes)iI) != 0)
			{
				iExtraRate += getAdditionalBaseCommerceRateBySpecialistImpl(eIndex, (SpecialistTypes)iI, kBuilding.getFreeSpecialistCount((SpecialistTypes)iI));
			}
		}

		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isHasTech((TechTypes)iI))
			{
				iExtraRate += (kBuilding.getTechCommerceChange(iI, eIndex));
			}
		}

		if (kBuilding.isForceAllTradeRoutes())
		{
			int iCurrentTradeRevenue = GET_PLAYER(getOwnerINLINE()).calculateTotalExports(YIELD_COMMERCE);
			
			GET_PLAYER(getOwnerINLINE()).changeForceAllTradeRoutes(1);
			
			int iFutureTradeRevenue = GET_PLAYER(getOwnerINLINE()).calculateTotalExports(YIELD_COMMERCE);
				
			GET_PLAYER(getOwnerINLINE()).changeForceAllTradeRoutes(-1);
			
			iExtraRate += (iFutureTradeRevenue - iCurrentTradeRevenue) * GET_PLAYER(getOwnerINLINE()).getCommercePercent(eIndex) / 100;
		}
		
		int iFreeSpecialistCommerce = 0;

		for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
		{
			SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
			if (eNewSpecialist == NO_SPECIALIST) break;

			iFreeSpecialistCommerce += GET_PLAYER(getOwnerINLINE()).specialistCommerce(eNewSpecialist, eIndex);
		}
		iExtraRate += iFreeSpecialistCommerce;
		
		if (kBuilding.getNumPopulationEmployed() > 0)
		{
			int* paiCommerce = new int[NUM_COMMERCE_TYPES];
			int* paiYield = new int[NUM_YIELD_TYPES];
			int iGreatPeopleRate;
			int iHappiness;
			int iHealthiness;
			removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
			iExtraRate += paiCommerce[eIndex];
			SAFE_DELETE_ARRAY(paiCommerce);
			SAFE_DELETE_ARRAY(paiYield);
		}
	}
	
	return iExtraRate;
}

/*
 * Returns the additional commerce rate modifier constructing the given building will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 * Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE.
 */
int CvCity::getAdditionalCommerceRateModifierByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	bool bNoEspionage = GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE);

	if (bNoEspionage && eIndex == COMMERCE_ESPIONAGE)
	{
		return 0;
	}

	int iExtraModifier = getAdditionalCommerceRateModifierByBuildingImpl(eIndex, eBuilding);
	if (bNoEspionage && eIndex == COMMERCE_CULTURE)
	{
		iExtraModifier += getAdditionalCommerceRateModifierByBuildingImpl(COMMERCE_ESPIONAGE, eBuilding);
	}
	return iExtraModifier;
}

/*
 * Returns the additional commerce rate modifier constructing the given building will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalCommerceRateModifierByBuildingImpl(CommerceTypes eIndex, BuildingTypes eBuilding)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	bool bObsolete = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	int iExtraModifier = 0;

	if (!bObsolete && !isReligiouslyDisabledBuilding(eBuilding))
	{
		iExtraModifier += kBuilding.getCommerceModifier(eIndex);
		iExtraModifier += kBuilding.getGlobalCommerceModifier(eIndex);
		
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isHasTech((TechTypes)iI))
			{
				iExtraModifier += (kBuilding.getTechCommerceModifier(iI, eIndex));
			}
		}
		for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		{
			if (hasBonus((BonusTypes)iI))
			{
				iExtraModifier += (kBuilding.getBonusCommerceModifier(iI, eIndex));
			}
		}
	}
	
	return iExtraModifier;
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


// BUG - Building Additional Commerce - end


void CvCity::updateBuildingCommerce()
{
	PROFILE_FUNC();

	//	Disabled during modifier recalc (and called explicitly there after re-enabling)
	if ( !GC.getGameINLINE().isRecalculatingModifiers() )
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			int iNewBuildingCommerce = 0;

			for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				//ls612: Support for Orbital buildings
				if (!GC.getBuildingInfo((BuildingTypes)iJ).isOrbital())
				{
					iNewBuildingCommerce += getBuildingCommerceByBuilding(((CommerceTypes)iI), ((BuildingTypes)iJ));
				}
				else
				{
					iNewBuildingCommerce += getOrbitalBuildingCommerceByBuilding(((CommerceTypes)iI), ((BuildingTypes)iJ));
				}
			}

			if (getBuildingCommerce((CommerceTypes)iI) != iNewBuildingCommerce)
			{
				m_aiBuildingCommerce[iI] = iNewBuildingCommerce;

				setCommerceDirty((CommerceTypes)iI);
			}
		}
	}
}


int CvCity::getSpecialistCommerce(CommerceTypes eIndex)	const												 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiSpecialistCommerce[eIndex]/100;
}


void CvCity::changeSpecialistCommerceTimes100(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

	if (iChange != 0)
	{
		m_aiSpecialistCommerce[eIndex] = (m_aiSpecialistCommerce[eIndex] + iChange);
		//TBNOTE: This should be allowed to be less than one.  Some specialists go negative on commerce.
		/*FAssert(getSpecialistCommerce(eIndex) >= 0);*/

		setCommerceDirty(eIndex);
	}
}


// BUG - Specialist Additional Commerce - start
/*
 * Returns the total additional commerce that changing the number of given specialists will provide/remove.
 *
 * Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE.
 */
int CvCity::getAdditionalCommerceBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	return getAdditionalCommerceTimes100BySpecialist(eIndex, eSpecialist, iChange) / 100;
}

/*
 * Returns the total additional commerce times 100 that changing the number of given specialists will provide/remove.
 *
 * Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE.
 */
int CvCity::getAdditionalCommerceTimes100BySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	int iExtraRate = getAdditionalBaseCommerceRateBySpecialist(eIndex, eSpecialist, iChange);
	if (iExtraRate == 0)
	{
		return 0;
	}
/************************************************************************************************/
/* Afforess	                  Start		 07/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iCivicCommerce = GET_PLAYER(getOwnerINLINE()).getSpecialistCommercePercentChanges(eSpecialist, eIndex);
	iExtraRate += iCivicCommerce;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	// int iRateTimes100 = getBaseCommerceRateTimes100(eIndex);
	int iModifier = getTotalCommerceRateModifier(eIndex);
/************************************************************************************************/
/* Afforess	                  Start		 07/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	int iExtraTimes100 = (iModifier * (100 * iExtraRate + iRateTimes100) / 100) - (iModifier * iRateTimes100 / 100);
*/
	int iExtraTimes100 = iExtraRate * 100;
	//iExtraTimes100 = (iModifier * (100 * iExtraRate + iRateTimes100 + iCivicCommerce) / 100) - (iModifier * iRateTimes100 / 100);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	iExtraTimes100 *= iModifier;
	iExtraTimes100 /= 100;


	return iExtraTimes100;
}

/*
 * Returns the additional base commerce rate that changing the number of given specialists will provide/remove.
 *
 * Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE.
 */
int CvCity::getAdditionalBaseCommerceRateBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	bool bNoEspionage = GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE);

	if (bNoEspionage && eIndex == COMMERCE_ESPIONAGE)
	{
		return 0;
	}

	int iExtraRate = getAdditionalBaseCommerceRateBySpecialistImpl(eIndex, eSpecialist, iChange);
	if (bNoEspionage && eIndex == COMMERCE_CULTURE)
	{
		iExtraRate += getAdditionalBaseCommerceRateBySpecialistImpl(COMMERCE_ESPIONAGE, eSpecialist, iChange);
	}
	return iExtraRate;
}

/*
 * Returns the additional base commerce rate that changing the number of given specialists will provide/remove.
 */
int CvCity::getAdditionalBaseCommerceRateBySpecialistImpl(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eSpecialist);
	
	return iChange * (kSpecialist.getCommerceChange(eIndex) + GET_PLAYER(getOwnerINLINE()).getExtraSpecialistCommerce(eSpecialist,eIndex) + getLocalSpecialistExtraCommerce(eSpecialist, eIndex) + GET_PLAYER(getOwnerINLINE()).getSpecialistExtraCommerce(eIndex));
}
// BUG - Specialist Additional Commerce - end


int CvCity::getReligionCommerce(CommerceTypes eIndex) const												 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiReligionCommerce[eIndex];
}


int CvCity::getReligionCommerceByReligion(CommerceTypes eIndex, ReligionTypes eReligion) const
{
	int iCommerce;

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eReligion >= 0, "eReligion expected to be >= 0");
	FAssertMsg(eReligion < GC.getNumReligionInfos(), "GC.getNumReligionInfos expected to be >= 0");

	iCommerce = 0;

/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* RevTrait Effects                                                                             */
/************************************************************************************************/
	if ((GET_PLAYER(getOwnerINLINE()).getStateReligion() == eReligion) 
	|| (GET_PLAYER(getOwnerINLINE()).getStateReligion() == NO_RELIGION) 
	|| (GET_PLAYER(getOwnerINLINE()).isNonStateReligionCommerce())) //phungus enlightened
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
	{
		if (isHasReligion(eReligion))
		{
			iCommerce += GC.getReligionInfo(eReligion).getStateReligionCommerce(eIndex);

			if (isHolyCity(eReligion))
			{
				iCommerce += GC.getReligionInfo(eReligion).getHolyCityCommerce(eIndex);
			}
		}
	}

	return iCommerce;
}


// XXX can this be simplified???
void CvCity::updateReligionCommerce(CommerceTypes eIndex)
{
	int iNewReligionCommerce = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		iNewReligionCommerce += getReligionCommerceByReligion(eIndex, ((ReligionTypes)iI));
	}

	if (getReligionCommerce(eIndex) != iNewReligionCommerce)
	{
		m_aiReligionCommerce[eIndex] = iNewReligionCommerce;
		FAssert(getReligionCommerce(eIndex) >= 0);

		setCommerceDirty(eIndex);
	}
}


void CvCity::updateReligionCommerce()
{
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		updateReligionCommerce((CommerceTypes)iI);
	}
}


int CvCity::getCorporationYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiCorporationYield[eIndex];
}

void CvCity::setCorporationYield(YieldTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	int iOldValue = getCorporationYield(eIndex);

	if (iOldValue != iNewValue)
	{
		m_aiCorporationYield[eIndex] = iNewValue;
		FAssert(getCorporationYield(eIndex) >= 0);

		changeBaseYieldRate(eIndex, (iNewValue - iOldValue));
	}
}

int CvCity::getCorporationCommerce(CommerceTypes eIndex) const												 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiCorporationCommerce[eIndex];
}


int CvCity::getCorporationYieldByCorporation(YieldTypes eIndex, CorporationTypes eCorporation) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eCorporation >= 0, "eCorporation expected to be >= 0");
	FAssertMsg(eCorporation < GC.getNumCorporationInfos(), "GC.getNumCorporationInfos expected to be >= 0");

	int iYield = 0;

	if (isActiveCorporation(eCorporation) && !isDisorder())
	{
		for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
		{
			BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(i);
			if (NO_BONUS != eBonus && getNumBonuses(eBonus) > 0)
			{
				iYield += (GC.getCorporationInfo(eCorporation).getYieldProduced(eIndex) * getNumBonuses(eBonus) * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getCorporationMaintenancePercent()) / 100;
			}
		}
/************************************************************************************************/
/* Afforess	                  Start		 02/09/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		iYield += GC.getCorporationInfo(eCorporation).getYieldChange(eIndex) * 100;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}

	return (iYield + 99) / 100;
}

int CvCity::getCorporationCommerceByCorporation(CommerceTypes eIndex, CorporationTypes eCorporation) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eCorporation >= 0, "eCorporation expected to be >= 0");
	FAssertMsg(eCorporation < GC.getNumCorporationInfos(), "GC.getNumCorporationInfos expected to be >= 0");

	int iCommerce = 0;

	if (isActiveCorporation(eCorporation) && !isDisorder())
	{
		for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
		{
			BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(i);
			if (NO_BONUS != eBonus && getNumBonuses(eBonus) > 0)
			{
				iCommerce += (GC.getCorporationInfo(eCorporation).getCommerceProduced(eIndex) * getNumBonuses(eBonus) * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getCorporationMaintenancePercent()) / 100;
			}
		}
/************************************************************************************************/
/* Afforess	                  Start		 02/09/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		iCommerce += GC.getCorporationInfo(eCorporation).getCommerceChange(eIndex) * 100;
		
		iCommerce *= (GET_TEAM(getTeam()).getCorporationRevenueModifier() + 100);
		iCommerce /= 100;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	}

	return (iCommerce + 99) / 100;
}

void CvCity::updateCorporationCommerce(CommerceTypes eIndex)
{
	int iNewCommerce = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		iNewCommerce += getCorporationCommerceByCorporation(eIndex, ((CorporationTypes)iI));
	}

	if (getCorporationCommerce(eIndex) != iNewCommerce)
	{
		m_aiCorporationCommerce[eIndex] = iNewCommerce;
		FAssert(getCorporationCommerce(eIndex) >= 0);

		setCommerceDirty(eIndex);
	}
}

void CvCity::updateCorporationYield(YieldTypes eIndex)
{
	int iOldYield = getCorporationYield(eIndex);
	int iNewYield = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		iNewYield += getCorporationYieldByCorporation(eIndex, (CorporationTypes)iI);
	}

	if (iOldYield != iNewYield)
	{
		m_aiCorporationYield[eIndex] = iNewYield;
		FAssert(getCorporationYield(eIndex) >= 0);

		changeBaseYieldRate(eIndex, (iNewYield - iOldYield));
	}
}


void CvCity::updateCorporation()
{
	updateCorporationBonus();

	updateBuildingCommerce();

	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		updateCorporationYield((YieldTypes)iI);
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; ++iI)
	{
		updateCorporationCommerce((CommerceTypes)iI);
	}

	setMaintenanceDirty(true);
}


void CvCity::updateCorporationBonus()
{
	std::vector<int> aiExtraCorpProducedBonuses;
	std::vector<int> aiLastCorpProducedBonuses;
	std::vector<bool> abHadBonuses;

	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		abHadBonuses.push_back(hasBonus((BonusTypes)iI));
		m_paiNumCorpProducedBonuses[iI] = 0; 
		aiLastCorpProducedBonuses.push_back(getNumBonuses((BonusTypes)iI));
		aiExtraCorpProducedBonuses.push_back(0);
	}
	for (int iIter = 0; iIter < GC.getNumCorporationInfos(); ++iIter)
	{
		for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
		{
			int iBonusProduced = GC.getCorporationInfo((CorporationTypes)iCorp).getBonusProduced();

			if (NO_BONUS != iBonusProduced)
			{
				if (!GET_TEAM(getTeam()).isBonusObsolete((BonusTypes)iBonusProduced))
				{
					if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)iBonusProduced).getTechCityTrade())))
					{
						if (isHasCorporation((CorporationTypes)iCorp) && GET_PLAYER(getOwnerINLINE()).isActiveCorporation((CorporationTypes)iCorp))
						{
/************************************************************************************************/
/* Afforess	                  Start		 07/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
							bool bConsumes = false;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
							for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
							{
								int iBonusConsumed = GC.getCorporationInfo((CorporationTypes)iCorp).getPrereqBonus(i);
								if (NO_BONUS != iBonusConsumed)
								{
/************************************************************************************************/
/* Afforess	                  Start		 07/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
									bConsumes = true;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
									aiExtraCorpProducedBonuses[iBonusProduced] += aiLastCorpProducedBonuses[iBonusConsumed];
								}
							}
/************************************************************************************************/
/* Afforess	                  Start		 06/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
							if (!bConsumes && iBonusProduced != -1)
							{
								aiExtraCorpProducedBonuses[iBonusProduced] = 1;
							}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
						}
					}
				}
			}
		}

		bool bChanged = false;

		for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		{
			if (aiExtraCorpProducedBonuses[iI] != 0)
			{
				m_paiNumCorpProducedBonuses[iI] += aiExtraCorpProducedBonuses[iI];

				bChanged = true;
			}

			aiLastCorpProducedBonuses[iI] = aiExtraCorpProducedBonuses[iI];
			aiExtraCorpProducedBonuses[iI] = 0;
		}

		if (!bChanged)
		{
			break;
		}

		FAssertMsg(iIter < GC.getNumCorporationInfos() - 1, "Corporation cyclical resource dependency");
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (abHadBonuses[iI] != hasBonus((BonusTypes)iI))
		{
			if (hasBonus((BonusTypes)iI))
			{
				processBonus((BonusTypes)iI, 1);
			}
			else
			{
				processBonus((BonusTypes)iI, -1);
			}
		}
	}			
}

//TB NOTE: getCommerceRateModifier and changeCommerceRateModifier now only apply to events.  I'd rename them but it might get more confusing that way.
int CvCity::getCommerceRateModifier(CommerceTypes eIndex) const											 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiCommerceRateModifier[eIndex];
}


void CvCity::changeCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

	if (iChange != 0)
	{
		m_aiCommerceRateModifier[eIndex] = (m_aiCommerceRateModifier[eIndex] + iChange);

		setCommerceModifierDirty(eIndex);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getCommerceHappinessPer(CommerceTypes eIndex) const									 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiCommerceHappinessPer[eIndex];
}


int CvCity::getCommerceHappinessByType(CommerceTypes eCommerce) const
{
	return ((getCommerceHappinessPer(eCommerce) * GET_PLAYER(getOwnerINLINE()).getCommercePercent(eCommerce)) / 100);
}


int CvCity::getCommerceHappiness() const
{
	int iHappiness = 0;

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iHappiness += getCommerceHappinessByType((CommerceTypes)iI);
	}

	return iHappiness;
}


void CvCity::changeCommerceHappinessPer(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

	if (iChange != 0)
	{
		m_aiCommerceHappinessPer[eIndex] = (m_aiCommerceHappinessPer[eIndex] + iChange);
		FAssert(getCommerceHappinessPer(eIndex) >= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getDomainFreeExperience(DomainTypes eIndex) const													 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	int iTotalDomainFreeExperience = m_aiDomainFreeExperience[eIndex];
	iTotalDomainFreeExperience += GET_PLAYER(getOwnerINLINE()).getNationalDomainFreeExperience(eIndex);

	return std::max(0, iTotalDomainFreeExperience);
}


void CvCity::changeDomainFreeExperience(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_aiDomainFreeExperience[eIndex] = (m_aiDomainFreeExperience[eIndex] + iChange);
	FAssert(getDomainFreeExperience(eIndex) >= 0);
}


int CvCity::getDomainProductionModifier(DomainTypes eIndex) const										 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	int iTotalDomainProductionModifier = m_aiDomainProductionModifier[eIndex];
	iTotalDomainProductionModifier += GET_PLAYER(getOwnerINLINE()).getNationalDomainProductionModifier(eIndex);

	return std::max(0, iTotalDomainProductionModifier);
}


void CvCity::changeDomainProductionModifier(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_aiDomainProductionModifier[eIndex] = (m_aiDomainProductionModifier[eIndex] + iChange);
}


int CvCity::getCulture(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return (m_aiCulture[eIndex] < 0 ? MAX_INT/100 : m_aiCulture[eIndex] / 100);
}

int CvCity::getCultureTimes100(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return (m_aiCulture[eIndex] < 0 ? MAX_INT : m_aiCulture[eIndex]);
}


int CvCity::countTotalCultureTimes100() const
{
	int iTotalCulture = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iTotalCulture += getCultureTimes100((PlayerTypes)iI);
		}
	}

	return iTotalCulture;
}


PlayerTypes CvCity::findHighestCulture() const
{
	int iBestValue = 0;
	PlayerTypes eBestPlayer = NO_PLAYER;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			int iValue = getCultureTimes100((PlayerTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = ((PlayerTypes)iI);
			}
		}
	}

	return eBestPlayer;
}


int CvCity::calculateCulturePercent(PlayerTypes eIndex) const
{
	int iTotalCulture = countTotalCultureTimes100();

	if (iTotalCulture > 0)
	{
		return ((getCultureTimes100(eIndex) * 100) / iTotalCulture);
	}

	return 0;
}


int CvCity::calculateTeamCulturePercent(TeamTypes eIndex) const
{
	int iTeamCulturePercent = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex)
			{
				iTeamCulturePercent += calculateCulturePercent((PlayerTypes)iI);
			}
		}
	}

	return iTeamCulturePercent;
}


void CvCity::setCulture(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet)
{
	if ( iNewValue > MAX_INT/100 )
	{
		iNewValue = MAX_INT/100;
	}

	setCultureTimes100(eIndex, 100 * iNewValue, bPlots, bUpdatePlotGroups, bNationalSet);
}

void CvCity::setCultureTimes100(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	int iOldCulture = getCultureTimes100(eIndex);
	if (iOldCulture != iNewValue)
	{
		m_aiCulture[eIndex] = iNewValue;
		FAssert(getCultureTimes100(eIndex) >= 0);

		updateCultureLevel(bUpdatePlotGroups);

		if (bPlots)
		{
			doPlotCulture(true, eIndex, 0);
		}
	}
	if (!bNationalSet && (iOldCulture < iNewValue))
	{
		GET_PLAYER(getOwner()).changeCulture((iNewValue - iOldCulture) / 100);
	}
}


void CvCity::changeCulture(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups)
{
	int	iOld = getCultureTimes100(eIndex);

	GET_PLAYER(getOwner()).changeCulture(iChange);
	bool bNationalSet = true;

	int iNew;
	if ( iChange > 0 )
	{
		if ( MAX_INT - 100*iChange > iOld )
		{
			iNew = iOld + 100*iChange;
		}
		else
		{
			iNew = MAX_INT;
		}
	}
	else
	{
		iNew = iOld + 100 * iChange;
	}

	setCultureTimes100(eIndex, iNew, bPlots, bUpdatePlotGroups, bNationalSet);
}

void CvCity::changeCultureTimes100(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups)
{
	int iOld = getCultureTimes100(eIndex);

	GET_PLAYER(getOwner()).changeCulture( iChange / 100 );
	bool bNationalSet = true;

	int iNew;
	if ( iChange > 0 )
	{
		if ( MAX_INT - iChange > iOld )
		{
			iNew = iOld + iChange;
		}
		else
		{
			iNew = MAX_INT;
		}
	}
	else
	{
		iNew = iOld + iChange;
	}

	setCultureTimes100(eIndex, iNew, bPlots, bUpdatePlotGroups, bNationalSet);
}


int CvCity::getNumRevolts(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_aiNumRevolts[eIndex];
}


void CvCity::changeNumRevolts(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	m_aiNumRevolts[eIndex] = (m_aiNumRevolts[eIndex] + iChange);
	FAssert(getNumRevolts(eIndex) >= 0);
}

int CvCity::getRevoltTestProbability() const
{
	int iBestModifier = 0;

	//TB Note: Revolt Protection is now added into Culture Garrison mechanism.
	//The way this was handled was potentially eliminating all chance of revolt too easily/too difficult to balance as it's just not granular enough for a longer progression curve.
	//By placing the effect of Revolt Protection onto Culture Garrison, we have a much more expandable system of adding protection vs revolt.
	//CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	//while (pUnitNode)
	//{
	//	CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
	//	pUnitNode = plot()->nextUnitNode(pUnitNode);

	//	if (pLoopUnit->revoltProtectionTotal() > iBestModifier)
	//	{
	//		iBestModifier = pLoopUnit->revoltProtectionTotal();
	//	}
	//}
	//iBestModifier = range(iBestModifier, 0, 100);

	return ((GC.getDefineINT("REVOLT_TEST_PROB") * (100 - iBestModifier)) / 100);
}

bool CvCity::isEverOwned(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_abEverOwned[eIndex];
}


void CvCity::setEverOwned(PlayerTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	m_abEverOwned[eIndex] = bNewValue;
}


bool CvCity::isTradeRoute(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_abTradeRoute[eIndex];
}


void CvCity::setTradeRoute(PlayerTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	m_abTradeRoute[eIndex] = bNewValue;
}


bool CvCity::isRevealed(TeamTypes eIndex, bool bDebug) const
{
	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return true;
	}
	else
	{
		FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
		FAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

		return m_abRevealed[eIndex];
	}
}


void CvCity::setRevealed(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

	setupGraphical();

	if (isRevealed(eIndex, false) != bNewValue)
	{
		m_abRevealed[eIndex] = bNewValue;

		updateVisibility();

		if (eIndex == GC.getGameINLINE().getActiveTeam())
		{
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			for (int iI = 0; iI < getNumCityPlots(); iI++)
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updateSymbols();
				}
			}
		}
	}
}


bool CvCity::getEspionageVisibility(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

	return m_abEspionageVisibility[eIndex];
}


void CvCity::setEspionageVisibility(TeamTypes eIndex, bool bNewValue, bool bUpdatePlotGroups)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

	if (getEspionageVisibility(eIndex) != bNewValue)
	{
		plot()->updateSight(false, bUpdatePlotGroups);

		m_abEspionageVisibility[eIndex] = bNewValue;

		plot()->updateSight(true, bUpdatePlotGroups);
	}
}

void CvCity::updateEspionageVisibility(bool bUpdatePlotGroups)
{
	std::vector<EspionageMissionTypes> aMission;
	for (int iMission = 0; iMission < GC.getNumEspionageMissionInfos(); ++iMission)
	{
		if (GC.getEspionageMissionInfo((EspionageMissionTypes)iMission).isPassive() && GC.getEspionageMissionInfo((EspionageMissionTypes)iMission).getVisibilityLevel() > 0)
		{
			aMission.push_back((EspionageMissionTypes)iMission);
		}
	}

	for (int iTeam = 0; iTeam < MAX_PC_TEAMS; ++iTeam)
	{
		bool bVisibility = false;

		if (iTeam != getTeam())
		{
			if (isRevealed((TeamTypes)iTeam, false))
			{
				for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
					if (kPlayer.isAlive() && kPlayer.getTeam() == iTeam)
					{
						for (std::vector<EspionageMissionTypes>::iterator it = aMission.begin(); it != aMission.end(); ++it)
						{
							if (kPlayer.canDoEspionageMission(*it, getOwnerINLINE(), plot(), -1, NULL))
							{
								bVisibility = true;
								break;
							}
						}

						if (bVisibility)
						{
							break;
						}
					}
				}
			}
		}

		setEspionageVisibility((TeamTypes)iTeam, bVisibility, bUpdatePlotGroups);
	}
}

const wchar* CvCity::getNameKey() const
{
	return m_szName;
}


const CvWString CvCity::getName(uint uiForm) const
{
	return gDLL->getObjectText(m_szName, uiForm, true);
}


void CvCity::setName(const wchar* szNewValue, bool bFound)
{
	CvWString szName(szNewValue);
	gDLL->stripSpecialCharacters(szName);

	if (!szName.empty())
	{
		if (GET_PLAYER(getOwnerINLINE()).isCityNameValid(szName, false))
		{
			m_szName = szName;

			setInfoDirty(true);

			if (isCitySelected())
			{
				gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
		if (bFound)
		{
			doFoundMessage();
		}
	}
}


void CvCity::doFoundMessage()
{
	MEMORY_TRACK_EXEMPT();

	CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_FOUNDED", getNameKey());
	AddDLLMessage(getOwnerINLINE(), false, -1, szBuffer, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_MAJOR_EVENT, NULL, NO_COLOR, getX_INLINE(), getY_INLINE());

	szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_IS_FOUNDED", getNameKey());
	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_CITY_FOUNDED, getOwnerINLINE(), szBuffer, getX_INLINE(), getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_ALT_HIGHLIGHT_TEXT"));
}


std::string CvCity::getScriptData() const
{
	return m_szScriptData;
}

// cppcheck-suppress passedByValue
void CvCity::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}


int CvCity::getNoBonusCount(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	return m_paiNoBonus[eIndex];
}

bool CvCity::isNoBonus(BonusTypes eIndex) const
{
	return (getNoBonusCount(eIndex) > 0);
}

void CvCity::changeNoBonusCount(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");

	if (iChange != 0)
	{
		if (getNumBonuses(eIndex) > 0)
		{
			processBonus(eIndex, -1);
		}

		m_paiNoBonus[eIndex] += iChange;
		FAssert(getNoBonusCount(eIndex) >= 0);

		if (getNumBonuses(eIndex) > 0)
		{
			processBonus(eIndex, 1);
		}

		updateCorporation();

		AI_setAssignWorkDirty(true);

		setInfoDirty(true);
	}
}


int CvCity::getFreeBonus(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	return m_paiFreeBonus[eIndex];
}


void CvCity::changeFreeBonus(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");

	if (iChange != 0)
	{
		GET_PLAYER(getOwnerINLINE()).startDeferredPlotGroupBonusCalculation();

		plot()->updatePlotGroupBonus(false);
		m_paiFreeBonus[eIndex] += iChange;
		FAssert(getFreeBonus(eIndex) >= 0);
		plot()->updatePlotGroupBonus(true);

		GET_PLAYER(getOwnerINLINE()).endDeferredPlotGroupBonusCalculation();
	}
}

int CvCity::getNumBonusesFromBase(BonusTypes eIndex, int iBaseNum) const
{
	if (isNoBonus(eIndex))
	{
		return 0;
	}
	/************************************************************************************************/
	/* Afforess	                  Start		 5/30/11                                                */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	if (GET_PLAYER(getOwnerINLINE()).getBonusMintedPercent(eIndex) > 0)
	{
		return 0;
	}
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/
	return iBaseNum;
}

int CvCity::getNumBonuses(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");

	if (!GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBonusInfo(eIndex).getTechCityTrade()))
	{
		return 0;
	}
	return getNumBonusesFromBase(eIndex, m_paiNumBonuses[eIndex]) + m_paiNumCorpProducedBonuses[eIndex];
}


bool CvCity::hasBonus(BonusTypes eIndex) const
{
	return (getNumBonuses(eIndex) > 0);
}

void CvCity::startDeferredBonusProcessing()
{
	if (0 == m_deferringBonusProcessingCount++)
	{
		SAFE_DELETE_ARRAY(m_paiStartDeferredSectionNumBonuses);

		m_paiStartDeferredSectionNumBonuses = new int[GC.getNumBonusInfos()];

		for(int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiStartDeferredSectionNumBonuses[iI] = m_paiNumBonuses[iI];
		}
	}
}

void CvCity::endDeferredBonusProcessing()
{
	if (0 == --m_deferringBonusProcessingCount)
	{
		for(int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			processNumBonusChange( (BonusTypes)iI, m_paiStartDeferredSectionNumBonuses[iI], m_paiNumBonuses[iI] );
		}

		SAFE_DELETE_ARRAY(m_paiStartDeferredSectionNumBonuses);
	}
}

void CvCity::processNumBonusChange(BonusTypes eIndex, int iOldValue, int iNewValue)
{
	if ( iOldValue != iNewValue )
	{
		bool bOldHasBonus = (getNumBonusesFromBase(eIndex, iOldValue) !=0 );
		bool bNewHasBonus = (getNumBonusesFromBase(eIndex, iNewValue) !=0 );

		if (bOldHasBonus != bNewHasBonus)
		{
			if (bNewHasBonus)
			{
				processBonus(eIndex, 1);
			}
			else
			{
				processBonus(eIndex, -1);
			}
		}

		if (isCorporationBonus(eIndex))
		{
			updateCorporation();
		}

		//	Linking bonuses may change what is buildable
		FlushCanConstructCache();
	}
}

void CvCity::changeNumBonuses(BonusTypes eIndex, int iChange)
{
	PROFILE_FUNC();

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");

	if (iChange != 0)
	{
		//bool bOldHasBonus = hasBonus(eIndex);

		m_paiNumBonuses[eIndex] += iChange;

		if ( m_deferringBonusProcessingCount == 0 )
		{
			processNumBonusChange(eIndex, m_paiNumBonuses[eIndex] - iChange, m_paiNumBonuses[eIndex]);
		}
	}
}

int CvCity::getNumCorpProducedBonuses(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	return m_paiNumCorpProducedBonuses[eIndex];
}


bool CvCity::isCorporationBonus(BonusTypes eBonus) const
{
	FAssert(eBonus >= 0);
	FAssert(eBonus < GC.getNumBonusInfos());

	for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
	{
		if (GET_PLAYER(getOwnerINLINE()).isActiveCorporation((CorporationTypes)iCorp))
		{
			for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
			{
				if (NO_BONUS != GC.getCorporationInfo((CorporationTypes)iCorp).getPrereqBonus(i))
				{
					if (GC.getCorporationInfo((CorporationTypes)iCorp).getPrereqBonus(i) == eBonus && isHasCorporation((CorporationTypes)iCorp))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool CvCity::isActiveCorporation(CorporationTypes eCorporation) const
{
	FAssert(eCorporation >= 0 && eCorporation < GC.getNumCorporationInfos());

	if (!isHasCorporation(eCorporation))
	{
		return false;
	}

	if (!GET_PLAYER(getOwnerINLINE()).isActiveCorporation(eCorporation))
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	                  Start		 02/17/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getCorporationInfo(eCorporation).getObsoleteTech() != NO_TECH)
	{
		if (GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getCorporationInfo(eCorporation).getObsoleteTech()))
		{
			return false;
		}
	}
	bool bRequiresBonus = false;
	bool bHasRequiredBonus = false;

	for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
	{
		BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(i);

		if (NO_BONUS != eBonus)
		{
			bRequiresBonus = true;
			if (getNumBonuses(eBonus) > 0)
			{
				bHasRequiredBonus = true;
				break;
			}
		}
	}
	if (bRequiresBonus && bHasRequiredBonus)
	{
		return true;
	}
	if (!bRequiresBonus)
	{
		return true;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return false;
}

int CvCity::getBuildingProduction(BuildingTypes eIndex)	const															 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	int iAdaptedProduction = std::max(0,m_paiBuildingProduction[eIndex]);

	return iAdaptedProduction;
}


void CvCity::setBuildingProduction(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	if (getBuildingProduction(eIndex) != iNewValue)
	{
		m_paiBuildingProduction[eIndex] = iNewValue;
		FAssert(getBuildingProduction(eIndex) >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		}
	}
}


void CvCity::changeBuildingProduction(BuildingTypes eIndex, int iChange)							 
{
	int iAdaptedChange = std::max(0, (getBuildingProduction(eIndex) + iChange));
	setBuildingProduction(eIndex, iAdaptedChange);
}


int CvCity::getBuildingProductionTime(BuildingTypes eIndex)	const															 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiBuildingProductionTime[eIndex];
}


void CvCity::setBuildingProductionTime(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	m_paiBuildingProductionTime[eIndex] = iNewValue;
	FAssert(getBuildingProductionTime(eIndex) >= 0);
}


void CvCity::changeBuildingProductionTime(BuildingTypes eIndex, int iChange)							 
{
	setBuildingProductionTime(eIndex, (getBuildingProductionTime(eIndex) + iChange));
}


// BUG - Production Decay - start
/*
 * Returns true if the given building will decay this turn.
 */
bool CvCity::isBuildingProductionDecay(BuildingTypes eIndex) const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return isHuman() && getProductionBuilding() != eIndex && getBuildingProduction(eIndex) > 0 
			&& 100 * getBuildingProductionTime(eIndex) >= GC.getDefineINT("BUILDING_PRODUCTION_DECAY_TIME") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
}

/*
 * Returns the amount by which the given building will decay once it reaches the limit.
 * Ignores whether or not the building will actually decay this turn.
 */
int CvCity::getBuildingProductionDecay(BuildingTypes eIndex) const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	int iProduction = getBuildingProduction(eIndex);
	return iProduction - ((iProduction * GC.getDefineINT("BUILDING_PRODUCTION_DECAY_PERCENT")) / 100);
}

/*
 * Returns the number of turns left before the given building will decay.
 */
int CvCity::getBuildingProductionDecayTurns(BuildingTypes eIndex) const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return std::max(0, (GC.getDefineINT("BUILDING_PRODUCTION_DECAY_TIME") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent() + 99) / 100 - getBuildingProductionTime(eIndex)) + 1;
}
// BUG - Production Decay - end


int CvCity::getProjectProduction(ProjectTypes eIndex) const																 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex expected to be < GC.getNumProjectInfos()");
	return m_paiProjectProduction[eIndex];
}


void CvCity::setProjectProduction(ProjectTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex expected to be < GC.getNumProjectInfos()");

	if (getProjectProduction(eIndex) != iNewValue)
	{
		m_paiProjectProduction[eIndex] = iNewValue;
		FAssert(getProjectProduction(eIndex) >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		}
	}
}


void CvCity::changeProjectProduction(ProjectTypes eIndex, int iChange)							 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex expected to be < GC.getNumProjectInfos()");
	setProjectProduction(eIndex, (getProjectProduction(eIndex) + iChange));
}


int CvCity::getBuildingOriginalOwner(BuildingTypes eIndex) const															 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiBuildingOriginalOwner[eIndex];
}


int CvCity::getBuildingOriginalTime(BuildingTypes eIndex) const															  
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiBuildingOriginalTime[eIndex];
}


int CvCity::getUnitProduction(UnitTypes eIndex)	const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiUnitProduction[eIndex];
}


void CvCity::setUnitProduction(UnitTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");

	if (getUnitProduction(eIndex) != iNewValue)
	{
		m_paiUnitProduction[eIndex] = iNewValue;
		FAssert(getUnitProduction(eIndex) >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		}
	}
}


void CvCity::changeUnitProduction(UnitTypes eIndex, int iChange)											 
{
	setUnitProduction(eIndex, (getUnitProduction(eIndex) + iChange));
}


int CvCity::getUnitProductionTime(UnitTypes eIndex)	const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiUnitProductionTime[eIndex];
}


void CvCity::setUnitProductionTime(UnitTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_paiUnitProductionTime[eIndex] = iNewValue;
	FAssert(getUnitProductionTime(eIndex) >= 0);
}


void CvCity::changeUnitProductionTime(UnitTypes eIndex, int iChange)											 
{
	setUnitProductionTime(eIndex, (getUnitProductionTime(eIndex) + iChange));
}


// BUG - Production Decay - start
/*
 * Returns true if the given unit will decay this turn.
 */
bool CvCity::isUnitProductionDecay(UnitTypes eIndex) const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return isHuman() && getProductionUnit() != eIndex && getUnitProduction(eIndex) > 0 
			&& 100 * getUnitProductionTime(eIndex) >= GC.getDefineINT("UNIT_PRODUCTION_DECAY_TIME") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
}

/*
 * Returns the amount by which the given unit will decay once it reaches the limit.
 * Ignores whether or not the unit will actually decay this turn.
 */
int CvCity::getUnitProductionDecay(UnitTypes eIndex) const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	int iProduction = getUnitProduction(eIndex);
	return iProduction - ((iProduction * GC.getDefineINT("UNIT_PRODUCTION_DECAY_PERCENT")) / 100);
}

/*
 * Returns the number of turns left before the given unit will decay.
 */
int CvCity::getUnitProductionDecayTurns(UnitTypes eIndex) const																			 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return std::max(0, (GC.getDefineINT("UNIT_PRODUCTION_DECAY_TIME") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent() + 99) / 100 - getUnitProductionTime(eIndex)) + 1;
}
// BUG - Production Decay - end


int CvCity::getGreatPeopleUnitRate(UnitTypes eIndex) const																 
{
	FAssertMsg(eIndex >= -1, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	int iTotalGreatPeopleUnitRate = 0;
	iTotalGreatPeopleUnitRate += m_paiGreatPeopleUnitRate[eIndex];
	iTotalGreatPeopleUnitRate += GET_PLAYER(getOwnerINLINE()).getNationalGreatPeopleUnitRate(eIndex);
	return std::max(0, iTotalGreatPeopleUnitRate);
}


void CvCity::setGreatPeopleUnitRate(UnitTypes eIndex, int iNewValue)										 
{
	FAssertMsg(eIndex >= -1, "eIndex expected to be >= -1");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE) && GC.getUnitInfo(eIndex).getEspionagePoints() > 0)
	{
		return;
	}

	m_paiGreatPeopleUnitRate[eIndex] = iNewValue;
}


void CvCity::changeGreatPeopleUnitRate(UnitTypes eIndex, int iChange)									 
{
	setGreatPeopleUnitRate(eIndex, (m_paiGreatPeopleUnitRate[eIndex] + iChange));
}


int CvCity::getGreatPeopleUnitProgress(UnitTypes eIndex) const														 
{
	FAssertMsg(eIndex >= -1, "eIndex expected to be >= -1");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiGreatPeopleUnitProgress[eIndex];
}


void CvCity::setGreatPeopleUnitProgress(UnitTypes eIndex, int iNewValue)							 
{
	FAssertMsg(eIndex >= -1, "eIndex expected to be >= -1");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_paiGreatPeopleUnitProgress[eIndex] = iNewValue;
	FAssert(getGreatPeopleUnitProgress(eIndex) >= 0);
}


void CvCity::changeGreatPeopleUnitProgress(UnitTypes eIndex, int iChange)							 
{
	setGreatPeopleUnitProgress(eIndex, (getGreatPeopleUnitProgress(eIndex) + iChange));
}


int CvCity::getSpecialistCount(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	return m_paiSpecialistCount[eIndex];
}


void CvCity::setSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");

	int iOldValue = getSpecialistCount(eIndex);

	if (iOldValue != iNewValue)
	{
		m_paiSpecialistCount[eIndex] = iNewValue;
		FAssert(getSpecialistCount(eIndex) >= 0);

		changeSpecialistPopulation(iNewValue - iOldValue);
		processSpecialist(eIndex, (iNewValue - iOldValue));

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}

#ifdef YIELD_VALUE_CACHING
		AI_NoteSpecialistChange();
#endif
	}
}


void CvCity::changeSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	setSpecialistCount(eIndex, (getSpecialistCount(eIndex) + iChange));
}


void CvCity::alterSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		if (isCitizensAutomated())
		{
			if ((getForceSpecialistCount(eIndex) + iChange) < 0)
			{
				setCitizensAutomated(false);
			}
		}

		// cppcheck-suppress duplicateCondition
		if (isCitizensAutomated())
		{
			changeForceSpecialistCount(eIndex, iChange);
		}
		else
		{
			if (iChange > 0)
			{
				for (int iI = 0; iI < iChange; iI++)
				{
					if ((extraPopulation() > 0) || AI_removeWorstCitizen(eIndex))
					{
						if (isSpecialistValid(eIndex, 1))
						{
							changeSpecialistCount(eIndex, 1);
						}
					}
				}
			}
			else
			{
				for (int iI = 0; iI < -(iChange); iI++)
				{
					if (getSpecialistCount(eIndex) > 0)
					{
						changeSpecialistCount(eIndex, -1);

						if ((eIndex != GC.getDefineINT("DEFAULT_SPECIALIST")) && (GC.getDefineINT("DEFAULT_SPECIALIST") != NO_SPECIALIST))
						{
							changeSpecialistCount(((SpecialistTypes)GC.getDefineINT("DEFAULT_SPECIALIST")), 1);
						}
						else if (extraFreeSpecialists() > 0)
						{
							AI_addBestCitizen(false, true);
						}
						else
						{
							int iNumCanWorkPlots = 0;
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
							for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
							{
								if (iJ != CITY_HOME_PLOT)
								{
									if (!isWorkingPlot(iJ))
									{
										CvPlot* pLoopPlot = getCityIndexPlot(iJ);

										if (pLoopPlot != NULL)
										{
											if (canWork(pLoopPlot))
											{
												++iNumCanWorkPlots;
											}
										}
									}
								}
							}

							if (iNumCanWorkPlots > 0)
							{
								AI_addBestCitizen(true, false);
							}
							else
							{
								AI_addBestCitizen(false, true);
							}
						}
					}
				}
			}
		}
	}
}


int CvCity::getMaxSpecialistCount(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	return m_paiMaxSpecialistCount[eIndex];
}


bool CvCity::isSpecialistValid(SpecialistTypes eIndex, int iExtra) const
{
	return (((getSpecialistCount(eIndex) + iExtra) <= getMaxSpecialistCount(eIndex)) || GET_PLAYER(getOwnerINLINE()).isSpecialistValid(eIndex) || (eIndex == GC.getDefineINT("DEFAULT_SPECIALIST")));
}


void CvCity::changeMaxSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");

	if (iChange != 0)
	{
		m_paiMaxSpecialistCount[eIndex] = std::max(0, (m_paiMaxSpecialistCount[eIndex] + iChange));

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getForceSpecialistCount(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	return m_paiForceSpecialistCount[eIndex];
}


bool CvCity::isSpecialistForced() const
{
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		if (getForceSpecialistCount((SpecialistTypes)iI) > 0)
		{
			return true;
		}
	}

	return false;
}


void CvCity::setForceSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");

	if (getForceSpecialistCount(eIndex) != iNewValue)
	{
		m_paiForceSpecialistCount[eIndex] = std::max(0, iNewValue);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}

		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeForceSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	setForceSpecialistCount(eIndex, (getForceSpecialistCount(eIndex) + iChange));
}


int CvCity::getFreeSpecialistCount(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	return m_paiFreeSpecialistCount[eIndex];
}

int CvCity::getAddedFreeSpecialistCount(SpecialistTypes eIndex) const
{
	return m_paiFreeSpecialistCountUnattributed[eIndex];
}

void CvCity::setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");

	int iOldValue = getFreeSpecialistCount(eIndex);
/************************************************************************************************/
/* Afforess	                  Start		 03/27/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iNewValue += GET_PLAYER(getOwnerINLINE()).getFreeSpecialistCount(eIndex);
	iNewValue += GET_TEAM(getTeam()).getFreeSpecialistCount(eIndex);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	iNewValue = std::max(0, iNewValue);
	if (iOldValue != iNewValue)
	{
		m_paiFreeSpecialistCount[eIndex] = iNewValue;
		//Allows total of free specialist count to dip below zero from all modifier sources so negative modifiers are ok, but sets an effective minimum of 0
		//FAssert(getFreeSpecialistCount(eIndex) >= 0);

		changeNumGreatPeople(iNewValue - iOldValue);
		processSpecialist(eIndex, (iNewValue - iOldValue));

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}
	}
}

void CvCity::changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange, bool bUnattributed)
{
/************************************************************************************************/
/* Afforess	                  Start		 03/27/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	setFreeSpecialistCount(eIndex, (getFreeSpecialistCount(eIndex) + iChange - GET_PLAYER(getOwnerINLINE()).getFreeSpecialistCount(eIndex)) - GET_TEAM(getTeam()).getFreeSpecialistCount(eIndex));

	if ( bUnattributed )
	{
		m_paiFreeSpecialistCountUnattributed[eIndex] += iChange;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}

int CvCity::getImprovementFreeSpecialists(ImprovementTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex expected to be < GC.getNumImprovementInfos()");
	return m_paiImprovementFreeSpecialists[eIndex];
}

void CvCity::changeImprovementFreeSpecialists(ImprovementTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex expected to be < GC.getNumImprovementInfos()");

	if (iChange != 0)
	{
		m_paiImprovementFreeSpecialists[eIndex] = std::max(0, (m_paiImprovementFreeSpecialists[eIndex] + iChange));
	}
}

int CvCity::getReligionInfluence(ReligionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex expected to be < GC.getNumReligionInfos()");
	//TB Debug
	//Somehow we are getting under 0 values here and that could cause problems down the road
	//This method enforces minimum of 0 without changing the actual value of m_paiReligionInfluence[eIndex] as the integrity of that value should be maintained.
	int iValue = 0;
	if (m_paiReligionInfluence[eIndex] < 0)
	{
		iValue = 0;
	}
	else
	{
		iValue = m_paiReligionInfluence[eIndex];
	}
	return iValue;
}


void CvCity::changeReligionInfluence(ReligionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex expected to be < GC.getNumReligionInfos()");
	m_paiReligionInfluence[eIndex] = m_paiReligionInfluence[eIndex] + iChange;
	FAssert(getReligionInfluence(eIndex) >= 0);
}


int CvCity::getCurrentStateReligionHappiness() const
{
	if (GET_PLAYER(getOwnerINLINE()).getStateReligion() != NO_RELIGION)
	{
		return getStateReligionHappiness(GET_PLAYER(getOwnerINLINE()).getStateReligion());
	}

	return 0;
}


int CvCity::getStateReligionHappiness(ReligionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex expected to be < GC.getNumReligionInfos()");
	return m_paiStateReligionHappiness[eIndex];
}


void CvCity::changeStateReligionHappiness(ReligionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex expected to be < GC.getNumReligionInfos()");

	if (iChange != 0)
	{
		m_paiStateReligionHappiness[eIndex] = (m_paiStateReligionHappiness[eIndex] + iChange);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getUnitCombatFreeExperience(UnitCombatTypes eIndex) const
{
	FAssertMsg(eIndex >= -1, "eIndex expected to be >= -1");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	if (eIndex == -1) return 0;
	return m_paiUnitCombatFreeExperience[eIndex];
}


void CvCity::changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatFreeExperience[eIndex] = (m_paiUnitCombatFreeExperience[eIndex] + iChange);
}


int CvCity::getFreePromotionCount(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	//TB Debug
	//Somehow we are getting under 0 values here and that could cause problems down the road
	//This method enforces minimum of 0 without changing the actual value of m_paiFreePromotionCount[eIndex](particularly puzzling) as the integrity of that value should be maintained.
	int iValue = 0;
	if (m_paiFreePromotionCount[eIndex] < 0)
	{
		iValue = 0;
	}
	else
	{
		iValue = m_paiFreePromotionCount[eIndex];
	}
	return iValue;
}


bool CvCity::isFreePromotion(PromotionTypes eIndex) const
{
	return (getFreePromotionCount(eIndex) > 0);
}


void CvCity::changeFreePromotionCount(PromotionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	m_paiFreePromotionCount[eIndex] = (m_paiFreePromotionCount[eIndex] + iChange);
	FAssert(getFreePromotionCount(eIndex) >= 0);
}


int CvCity::getSpecialistFreeExperience() const
{
	return m_iSpecialistFreeExperience;
}


void CvCity::changeSpecialistFreeExperience(int iChange)
{
	m_iSpecialistFreeExperience += iChange;
	FAssert(m_iSpecialistFreeExperience >= 0);
}


int CvCity::getEspionageDefenseModifier() const
{
	int iNationalDefense = GET_PLAYER(getOwnerINLINE()).getNationalEspionageDefense();
	int iLocalDefense = m_iEspionageDefenseModifier;
	int iTotal = iNationalDefense + iLocalDefense;
	return iTotal;
}


void CvCity::changeEspionageDefenseModifier(int iChange)
{
	if (0 != iChange)
	{
		m_iEspionageDefenseModifier += iChange;
	}
}

bool CvCity::isWorkingPlot(int iIndex) const
{
	FAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	return (m_bPlotWorkingMasked ? 0 : m_pabWorkingPlot[iIndex]);
}


bool CvCity::isWorkingPlot(const CvPlot* pPlot) const
{
	int iIndex = getCityPlotIndex(pPlot);

	if (iIndex != -1)
	{
		return isWorkingPlot(iIndex);
	}

	return false;
}

void CvCity::processWorkingPlot(int iPlot, int iChange, bool yieldsOnly)
{
	CvPlot* pPlot = getCityIndexPlot(iPlot);

	if (pPlot != NULL)
	{
		FAssertMsg(pPlot->getWorkingCity() == this, "WorkingCity is expected to be this");

		if (!yieldsOnly)
		{
			if (iPlot != CITY_HOME_PLOT)
			{
				changeWorkingPopulation(iChange);
			}

			// update plot builder special case where a plot is being worked but is (a) unimproved  or (b) un-bonus'ed
			pPlot->updatePlotBuilder();

			if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
			{
				pPlot->updateSymbolDisplay();
			}			
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			changeBaseYieldRate(((YieldTypes)iI), iChange*pPlot->getYield((YieldTypes)iI));
		}
	}

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}

#ifdef YIELD_VALUE_CACHING
	AI_NoteWorkerChange();
#endif
}

void CvCity::setWorkingPlot(int iIndex, bool bNewValue)
{
	FAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	if (isWorkingPlot(iIndex) != bNewValue)
	{
		m_pabWorkingPlot[iIndex] = bNewValue;

		processWorkingPlot(iIndex, bNewValue ? 1 : -1);
		if (bNewValue)
		{
			CvPlot* pPlot = getCityIndexPlot(iIndex);
			pPlot->setPlotIgnoringImprovementUpgrade(false);
		}
	}
}

void CvCity::setWorkingPlot(CvPlot* pPlot, bool bNewValue)
{
	setWorkingPlot(getCityPlotIndex(pPlot), bNewValue);
}


void CvCity::alterWorkingPlot(int iIndex)
{
	FAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	if (iIndex == CITY_HOME_PLOT)
	{
		setCitizensAutomated(true);
	}
	else
	{
		CvPlot* pPlot = getCityIndexPlot(iIndex);

		if (pPlot != NULL)
		{
			if (canWork(pPlot))
			{
				setCitizensAutomated(false);

				if (isWorkingPlot(iIndex))
				{
					setWorkingPlot(iIndex, false);

					if (GC.getDefineINT("DEFAULT_SPECIALIST") != NO_SPECIALIST)
					{
						changeSpecialistCount(((SpecialistTypes)GC.getDefineINT("DEFAULT_SPECIALIST")), 1);
					}
					else
					{
						AI_addBestCitizen(false, true);
					}
				}
				else
				{
					if ((extraPopulation() > 0) || AI_removeWorstCitizen())
					{
						setWorkingPlot(iIndex, true);
					}
				}
			}
			else if (pPlot->getOwnerINLINE() == getOwnerINLINE())
			{
				pPlot->setWorkingCityOverride(this);
			}
		}
	}
}


int CvCity::getNumRealBuilding(BuildingTypes eIndex) const									 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return isDisabledBuilding(eIndex) ? 0 : m_paiNumRealBuilding[eIndex];
}


void CvCity::setNumRealBuilding(BuildingTypes eIndex, int iNewValue)
{
	setNumRealBuildingTimed(eIndex, iNewValue, true, getOwnerINLINE(), GC.getGameINLINE().getGameTurnYear());
}


void CvCity::setNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	int iChangeNumRealBuilding = iNewValue - getNumRealBuilding(eIndex);

	if (iChangeNumRealBuilding != 0)
	{
		int iOldNumBuilding = getNumBuilding(eIndex);

		if ( m_paiNumRealBuilding[eIndex] != iNewValue )
		{
			//	Changing the buildings in a city invaldiates lots of cached data so flush the caches
			if ( GC.getBuildingInfo(eIndex).EnablesOtherBuildings() )
			{
				AI_FlushBuildingValueCache(true);
				FlushCanConstructCache();
			}
#ifdef YIELD_VALUE_CACHING
			ClearYieldValueCache();		//	A new building can change yield rates
#endif
#ifdef CAN_TRAIN_CACHING
			//	Mark all unit canTrain values cached as dirty
			invalidateCachedCanTrainForUnit(NO_UNIT);
#endif
		}

		m_paiNumRealBuilding[eIndex] = iNewValue;

		if (getNumRealBuilding(eIndex) > 0)
		{
			m_paiBuildingOriginalOwner[eIndex] = eOriginalOwner;
			m_paiBuildingOriginalTime[eIndex] = iOriginalTime;
		}
		else
		{
			m_paiBuildingOriginalOwner[eIndex] = NO_PLAYER;
			m_paiBuildingOriginalTime[eIndex] = MIN_INT;
		}

		if (iOldNumBuilding != getNumBuilding(eIndex))
		{
			if (getNumRealBuilding(eIndex) > 0)
			{
				if (GC.getBuildingInfo(eIndex).isStateReligion())
				{
					for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
					{
						if (GC.getBuildingInfo(eIndex).getVoteSourceType() == (VoteSourceTypes)iI)
						{
							if (GC.getGameINLINE().getVoteSourceReligion((VoteSourceTypes)iI) == NO_RELIGION)
							{
								FAssert(GET_PLAYER(getOwnerINLINE()).getStateReligion() != NO_RELIGION);
								GC.getGameINLINE().setVoteSourceReligion((VoteSourceTypes)iI, GET_PLAYER(getOwnerINLINE()).getStateReligion(), true);
							}
						}
					}
				}
			}

			processBuilding(eIndex, getNumBuilding(eIndex) - iOldNumBuilding);
		}
		
/************************************************************************************************/
/* Afforess	                  Start		 06/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		//Remove any extensions of this buildingclass
		if (iOldNumBuilding > m_paiNumRealBuilding[eIndex])
		{
			for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
			{
				if (getNumRealBuilding((BuildingTypes)iI) > 0 && iI != eIndex)
				{
					if (GC.getBuildingInfo((BuildingTypes)iI).getExtendsBuildingClass() == GC.getBuildingInfo(eIndex).getBuildingClassType())
					{
						//avoid infinite recursion
						if (GC.getBuildingInfo(eIndex).getExtendsBuildingClass() != GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())
						{
							setNumRealBuilding((BuildingTypes)iI, 0);
						}
					}
				}
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

		if (!(GC.getBuildingClassInfo((BuildingClassTypes)(GC.getBuildingInfo(eIndex).getBuildingClassType())).isNoLimit()))
		{
			if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eIndex).getBuildingClassType())))
			{
				changeNumWorldWonders(iChangeNumRealBuilding);
			}
			else if (isTeamWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eIndex).getBuildingClassType())))
			{
				changeNumTeamWonders(iChangeNumRealBuilding);
			}
			else if (isNationalWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eIndex).getBuildingClassType())))
			{
				changeNumNationalWonders(iChangeNumRealBuilding);
			}
			else
			{
				changeNumBuildings(iChangeNumRealBuilding);
			}
		}

		if (iChangeNumRealBuilding > 0)
		{
			if (bFirst)
			{
				if (GC.getBuildingInfo(eIndex).isCapital())
				{
					GET_PLAYER(getOwnerINLINE()).setCapitalCity(this);
				}

				if (GC.getGameINLINE().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
				{
					if (GC.getBuildingInfo(eIndex).isGoldenAge())
					{
						GET_PLAYER(getOwnerINLINE()).changeGoldenAgeTurns(iChangeNumRealBuilding * (GET_PLAYER(getOwnerINLINE()).getGoldenAgeLength() + 1));
					}

					if (GC.getBuildingInfo(eIndex).getGlobalPopulationChange() != 0)
					{
						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
								{
									if (GC.getBuildingInfo(eIndex).isTeamShare() || (iI == getOwnerINLINE()))
									{
										int iLoop = 0;
										for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
										{
											pLoopCity->setPopulation(std::max(1, (pLoopCity->getPopulation() + iChangeNumRealBuilding * GC.getBuildingInfo(eIndex).getGlobalPopulationChange())));
											pLoopCity->AI_updateAssignWork();  // so subsequent cities don't starve with the extra citizen working nothing
										}
									}
								}
							}
						}
					}

					for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
					{
						if (GC.getBuildingInfo(eIndex).getReligionChange(iI) > 0)
						{
							setHasReligion(((ReligionTypes)iI), true, true, true);
						}
					}

					if (GC.getBuildingInfo(eIndex).getFreeTechs() > 0)
					{
						if (!isHuman())
						{
							for (int iI = 0; iI < GC.getBuildingInfo(eIndex).getFreeTechs(); iI++)
							{
								for (int iLoop = 0; iLoop < iChangeNumRealBuilding; iLoop++)
								{
									GET_PLAYER(getOwnerINLINE()).AI_chooseFreeTech();
								}
							}
						}
						else
						{
							GET_PLAYER(getOwnerINLINE()).chooseTech(GC.getBuildingInfo(eIndex).getFreeTechs() * iChangeNumRealBuilding, gDLL->getText("TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH", GC.getBuildingInfo(eIndex).getTextKeyWide()));
						}
					}

					if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eIndex).getBuildingClassType())))
					{
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_COMPLETES_WONDER", GET_PLAYER(getOwnerINLINE()).getNameKey(), GC.getBuildingInfo(eIndex).getTextKeyWide());
							GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwnerINLINE(), szBuffer, getX_INLINE(), getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_BUILDING_TEXT"));
						}

						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								MEMORY_TRACK_EXEMPT();

								if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
								{
									CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_WONDER_COMPLETED", GET_PLAYER(getOwnerINLINE()).getNameKey(), GC.getBuildingInfo(eIndex).getTextKeyWide());
									AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDER_BUILDING_BUILD", MESSAGE_TYPE_MAJOR_EVENT, GC.getBuildingInfo(eIndex).getArtInfo()->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_BUILDING_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
								}
								else
								{
									CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN", GC.getBuildingInfo(eIndex).getTextKeyWide());
									AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDER_BUILDING_BUILD", MESSAGE_TYPE_MAJOR_EVENT, GC.getBuildingInfo(eIndex).getArtInfo()->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_BUILDING_TEXT"));
								}
							}
						}
					}
				}

				GC.getGameINLINE().incrementBuildingClassCreatedCount((BuildingClassTypes)(GC.getBuildingInfo(eIndex).getBuildingClassType()));
			}
			
			if (GC.getBuildingInfo(eIndex).isAllowsNukes())
			{//TB Nukefix (changed to GET_PLAYER(getOwnerINLINE() rather than GC.getGameINLINE and moved down outside of if bfirst.)
				GET_PLAYER(getOwnerINLINE()).makeNukesValid(true);
			}
		}

		//great wall
		if (bFirst)
		{
			if (GC.getBuildingInfo(eIndex).isAreaBorderObstacle())
			{
				int iCountExisting = 0;
				for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					if (eIndex != iI && GC.getBuildingInfo((BuildingTypes)iI).isAreaBorderObstacle())
					{
						iCountExisting += getNumRealBuilding((BuildingTypes)iI);
					}
				}

				if (iCountExisting == 1 && iNewValue == 0)
				{
					processGreatWall(true, true);
				}
				else if (iCountExisting == 0 && iNewValue > 0)
				{
					processGreatWall(true, true);
				}
			}
		}
	}
}

bool CvCity::processGreatWall(bool bIn, bool bForce, bool bSeeded)
{
	return false;
	/*
	> TBNote: I've found both a crash scenario in PBEM and an infinite hang scenario in single player.
	> A player complained about exceedingly strange graphic artifice when they encircle the globe with a singular culture that possesses the GW and the hang looked to have a similar basis.
	> Unfortunately, I'm no front end specialist and I don't have a theory on how to isolate the cause except to say that it's the new way that the GW is being processed that's causing the crashes and hangs.
	> So for now, this is disabled.  I may eventually create a manual option to turn it off or on and that would give us the ability to turn it off only if it's creating problems.
	> Better would be to FIX it but I've got no clue on that.
	See https://github.com/caveman2cosmos/Caveman2Cosmos/issues/44
	*/
	#if 0

	if ( !bForce && !GC.getReprocessGreatWallDynamically() )
	{
		return true;
	}

	bool bHasGreatWall = false;
	if ( bIn || !bSeeded )
	{
		for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			if ( getNumRealBuilding((BuildingTypes)iI) > 0 )
			{
				if ( GC.getBuildingInfo((BuildingTypes)iI).isAreaBorderObstacle())
				{
					bHasGreatWall = true;
					break;
				}
			}
		}
	}
	else
	{
		bHasGreatWall = m_bIsGreatWallSeed;
	}

	if ( bHasGreatWall )
	{
		CvCity* pUseCity = NULL;

		if ( isInViewport() )
		{
			pUseCity = this;
		}
		else
		{
			//	Need to find a culturally connected city that IS in the current viewport
			int						iDummyVal;
			CvUnitSelectionCriteria	noGrowthCriteria;

			noGrowthCriteria.m_bIgnoreGrowth = true;

			UnitTypes				eDummyUnit = AI_bestUnitAI(UNITAI_ATTACK, iDummyVal, true, true, &noGrowthCriteria);

			if ( eDummyUnit == NO_UNIT )
			{
				eDummyUnit = AI_bestUnitAI(UNITAI_CITY_DEFENSE, iDummyVal, true, true, &noGrowthCriteria);

				FAssert(eDummyUnit != NO_UNIT);
			}
			CvUnit*				pTempUnit = GET_PLAYER(getOwnerINLINE()).getTempUnit(eDummyUnit, getX_INLINE(), getY_INLINE());
			CvReachablePlotSet	plotSet(pTempUnit->getGroup(), MOVE_OUR_TERRITORY, MAX_INT);

			for(CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
			{
				CvCity* pCity = itr.plot()->getPlotCity();

				if ( pCity != NULL && pCity->isInViewport() )
				{
					pUseCity = pCity;
					break;
				}
			}

			GET_PLAYER(getOwnerINLINE()).releaseTempUnit();
		}

		//	If no suitable city is within the viewport we'll have to move the viewport
		bool bViewportMoved = false;
		int	 iOldViewportXOffset = 0;
		int	 iOldViewportYOffset = 0;

		if ( pUseCity == NULL && !bSeeded )
		{
			pUseCity = this;
			bViewportMoved = true;

			GC.getCurrentViewport()->getMapOffset(iOldViewportXOffset, iOldViewportYOffset);
			GC.getCurrentViewport()->setOffsetToShow(getX_INLINE(), getY_INLINE());
		}
		//	remove or re-add
		if ( pUseCity != NULL )
		{
			if ( bIn )
			{
				pUseCity->m_bIsGreatWallSeed = true;
				gDLL->getEngineIFace()->AddGreatWall(pUseCity);
			}
			else
			{
				pUseCity->m_bIsGreatWallSeed = false;
				gDLL->getEngineIFace()->RemoveGreatWall(pUseCity);
			}
		}

		if ( bViewportMoved )
		{
			GC.getCurrentViewport()->setMapOffset(iOldViewportXOffset, iOldViewportYOffset);
		}

		return true;
	}

	return false;
#endif
}

int CvCity::getNumFreeBuilding(BuildingTypes eIndex) const													 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");	
	return m_paiNumFreeBuilding[eIndex];
}

int CvCity::getNumFreeAreaBuilding(BuildingTypes eIndex) const													 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiNumFreeAreaBuilding[eIndex];
}

void CvCity::setNumFreeBuilding(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	if (iNewValue != 0)
	{
		if (!isValidBuildingLocation(eIndex))
		{
			iNewValue = 0;
		}
	}

	if (getNumFreeBuilding(eIndex) != iNewValue)
	{
		int iOldNumBuilding = getNumBuilding(eIndex);

		m_paiNumFreeBuilding[eIndex] = iNewValue;
		// cppcheck-suppress knownConditionTrueFalse
		if (iOldNumBuilding != getNumBuilding(eIndex))
		{
			processBuilding(eIndex, iNewValue - iOldNumBuilding);
		}
	}
}

void CvCity::setNumFreeAreaBuilding(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	if (iNewValue != 0)
	{
		if (!isValidBuildingLocation(eIndex))
		{
			iNewValue = 0;
		}
	}

	if (getNumFreeAreaBuilding(eIndex) != iNewValue)
	{
		bool bHad = (getNumBuilding(eIndex) > 0);

		m_paiNumFreeAreaBuilding[eIndex] = iNewValue;

		bool bHas = (getNumBuilding(eIndex) > 0);
		// cppcheck-suppress knownConditionTrueFalse
		if (bHas != bHad)
		{
			processBuilding(eIndex, bHas ? 1 : -1);
		}
	}
}

bool CvCity::isHasReligion(ReligionTypes eIndex) const													 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex expected to be < GC.getNumReligionInfos()");
	return m_pabHasReligion[eIndex];
}

//Team Project (5)

void CvCity::checkReligiousDisablingAllBuildings()
{
	bool bChangeMade = false;

	if (GC.getGameINLINE().isOption(GAMEOPTION_RELIGIOUS_DISABLING))
	{
		for (int iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
		{
			ReligionTypes eReligion = ((ReligionTypes)iJ);
			if (isHasReligion(eReligion))
			{
				for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					BuildingTypes eBuilding = ((BuildingTypes)iI);
					if (GC.getBuildingInfo(eBuilding).getReligionType() != NO_RELIGION || GC.getBuildingInfo(eBuilding).getPrereqReligion() != NO_RELIGION) 
					{
						if (GC.getBuildingInfo(eBuilding).getReligionType() == eReligion || GC.getBuildingInfo(eBuilding).getPrereqReligion() == eReligion)
						{
							if (getNumBuilding(eBuilding) > 0)
							{
								if (isDisabledBuilding(eBuilding))
								{
									if (GET_PLAYER(getOwnerINLINE()).getStateReligion() == eReligion || !GET_PLAYER(getOwnerINLINE()).hasBannedNonStateReligions())
									{
										setDisabledBuilding(eBuilding, false);

										MEMORY_TRACK_EXEMPT();

										CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_RESTORED_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eBuilding).getDescription());
										AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
										bChangeMade = true;
									}
								}
								if (!isDisabledBuilding(eBuilding))
								{
									if (GET_PLAYER(getOwnerINLINE()).getStateReligion() != eReligion && GET_PLAYER(getOwnerINLINE()).hasBannedNonStateReligions() && eReligion != NO_RELIGION)
									{
										setDisabledBuilding((BuildingTypes)iI, true);

										MEMORY_TRACK_EXEMPT();

										CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_DISABLED_COMPLETELY_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eBuilding).getDescription());
										AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
										bChangeMade = true;
									}

									if (isReligiouslyDisabledBuilding(eBuilding))
									{
										if (GET_PLAYER(getOwnerINLINE()).hasAllReligionsActive() || eReligion == NO_RELIGION || GET_PLAYER(getOwnerINLINE()).getStateReligion() == eReligion)
										{
											setReligiouslyDisabledBuilding((BuildingTypes)iI, false);
											bChangeMade = true;
										}
									}
									if (!isReligiouslyDisabledBuilding(eBuilding))
									{
										if (!GET_PLAYER(getOwnerINLINE()).hasAllReligionsActive() && eReligion != NO_RELIGION && GET_PLAYER(getOwnerINLINE()).getStateReligion() != eReligion)
										{
											setReligiouslyDisabledBuilding(eBuilding, true);
											bChangeMade = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//Compatibility run.  Once this update was long forgotten we can disable this section but it should not cause trouble, only a very small delay
	//to run a loop through all buildings checking that all buildings are not religiously disabled.
	//else if (!GC.getGameINLINE().isOption(GAMEOPTION_RELIGIOUS_DISABLING))
	//{
	//	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	//	{
	//		BuildingTypes eBuilding = ((BuildingTypes)iI);
	//		if (isReligiouslyDisabledBuilding(eBuilding))
	//		{
	//			setReligiouslyDisabledBuilding(eBuilding, false);
	//			bChangeMade = true;
	//		}
	//	}
	//}

	//efficiency measure
	if (bChangeMade)
	{
		setMaintenanceDirty(true);
		updateReligionHappiness();
		updateReligionCommerce();
	}
}

void CvCity::checkReligiousDisabling(BuildingTypes eBuilding)
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_RELIGIOUS_DISABLING))
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			ReligionTypes eReligion = ((ReligionTypes)iI);
			if (isHasReligion(eReligion))
			{
				if (GC.getBuildingInfo(eBuilding).getReligionType() != NO_RELIGION || GC.getBuildingInfo(eBuilding).getPrereqReligion() != NO_RELIGION) 
				{
					if (GC.getBuildingInfo(eBuilding).getReligionType() == eReligion || GC.getBuildingInfo(eBuilding).getPrereqReligion() == eReligion)
					{
						if (getNumBuilding(eBuilding) > 0)
						{
							if (isDisabledBuilding(eBuilding))
							{
								if (GET_PLAYER(getOwnerINLINE()).getStateReligion() == eReligion || !GET_PLAYER(getOwnerINLINE()).hasBannedNonStateReligions())
								{
									setDisabledBuilding(eBuilding, false);

									MEMORY_TRACK_EXEMPT();

									CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_RESTORED_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eBuilding).getDescription());
									AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
								}
							}
							if (!isDisabledBuilding(eBuilding))
							{
								if (GET_PLAYER(getOwnerINLINE()).getStateReligion() != eReligion && GET_PLAYER(getOwnerINLINE()).hasBannedNonStateReligions() && eReligion != NO_RELIGION)
								{
									setDisabledBuilding((BuildingTypes)iI, true);

									MEMORY_TRACK_EXEMPT();

									CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_DISABLED_COMPLETELY_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eBuilding).getDescription());
									AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
								}

								if (isReligiouslyDisabledBuilding(eBuilding))
								{
									if (GET_PLAYER(getOwnerINLINE()).hasAllReligionsActive() || eReligion == NO_RELIGION || GET_PLAYER(getOwnerINLINE()).getStateReligion() == eReligion)
									{
										setReligiouslyDisabledBuilding((BuildingTypes)iI, false);
									}
								}
								if (!isReligiouslyDisabledBuilding(eBuilding))
								{
									if (!GET_PLAYER(getOwnerINLINE()).hasAllReligionsActive() && eReligion != NO_RELIGION && GET_PLAYER(getOwnerINLINE()).getStateReligion() != eReligion)
									{
										setReligiouslyDisabledBuilding(eBuilding, true);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (!GC.getGameINLINE().isOption(GAMEOPTION_RELIGIOUS_DISABLING) && isReligiouslyDisabledBuilding(eBuilding))
	{
		setReligiouslyDisabledBuilding(eBuilding, false);
	}
	setMaintenanceDirty(true);
	updateReligionHappiness();
	updateReligionCommerce();
}

void CvCity::applyReligionModifiers(ReligionTypes eIndex, bool bValue)
{
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).getPrereqReligion() == eIndex)
		{
			if (bValue && isDisabledBuilding((BuildingTypes)iI))
			{
				setDisabledBuilding((BuildingTypes)iI, false);
			}
			else if (!bValue && getNumBuilding((BuildingTypes)iI) > 0)
			{
				setDisabledBuilding((BuildingTypes)iI, true);
			}
		}
	}

	setMaintenanceDirty(true);
	updateReligionHappiness();
	updateReligionCommerce();
}

void CvCity::setHasReligion(ReligionTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex expected to be < GC.getNumReligionInfos()");

	if (isHasReligion(eIndex) != bNewValue)
	{
		for (int iVoteSource = 0; iVoteSource < GC.getNumVoteSourceInfos(); ++iVoteSource)
		{
			processVoteSourceBonus((VoteSourceTypes)iVoteSource, false);
		}

		m_pabHasReligion[eIndex] = bNewValue;

		for (int iVoteSource = 0; iVoteSource < GC.getNumVoteSourceInfos(); ++iVoteSource)
		{
			processVoteSourceBonus((VoteSourceTypes)iVoteSource, true);
		}

		GET_PLAYER(getOwnerINLINE()).changeHasReligionCount(eIndex, ((isHasReligion(eIndex)) ? 1 : -1));

		//	Religion changes may change what is buildable
		FlushCanConstructCache();

		AI_setAssignWorkDirty(true);

		setInfoDirty(true);

		if (isHasReligion(eIndex))
		{
			GC.getGameINLINE().makeReligionFounded(eIndex, getOwnerINLINE());

			if (bAnnounce)
			{
				if (GC.getGameINLINE().getHolyCity(eIndex) != this)
				{
					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
							{
								if ((getOwnerINLINE() == iI) || (GET_PLAYER((PlayerTypes)iI).getStateReligion() == eIndex) || GET_PLAYER((PlayerTypes)iI).hasHolyCity(eIndex))
								{
									MEMORY_TRACK_EXEMPT();

									CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_RELIGION_SPREAD", GC.getReligionInfo(eIndex).getTextKeyWide(), getNameKey());
									AddDLLMessage(((PlayerTypes)iI), false, GC.getDefineINT("EVENT_MESSAGE_TIME_LONG"), szBuffer, GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getReligionInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), bArrows, bArrows);
								}
							}
						}
					}
				}

				if (isHuman())
				{
					if (GET_PLAYER(getOwnerINLINE()).getHasReligionCount(eIndex) == 1)
					{
						if (GET_PLAYER(getOwnerINLINE()).canConvert(eIndex) && (GET_PLAYER(getOwnerINLINE()).getStateReligion() == NO_RELIGION))
						{
							CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGERELIGION);
							if (NULL != pInfo)
							{
								pInfo->setData1(eIndex);
								gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE());
							}
						}
					}
				}
			}
		}
/************************************************************************************************/
/* Afforess	                  Start		 06/24/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		else 
		{
			if (bAnnounce)
			{
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
						{
							if ((getOwnerINLINE() == iI) || (GET_PLAYER((PlayerTypes)iI).getStateReligion() == eIndex) || GET_PLAYER((PlayerTypes)iI).hasHolyCity(eIndex))
							{
								MEMORY_TRACK_EXEMPT();

								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_RELIGION_DECAY", getNameKey(), GC.getReligionInfo(eIndex).getTextKeyWide());
								AddDLLMessage(((PlayerTypes)iI), false, GC.getDefineINT("EVENT_MESSAGE_TIME_LONG"), szBuffer, GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getReligionInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), bArrows, bArrows);
							}
						}
					}
				}
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

		
		if (bNewValue)
		{
			// Python Event
			CvEventReporter::getInstance().religionSpread(eIndex, getOwnerINLINE(), this);
		}
		else
		{
			// Python Event
			CvEventReporter::getInstance().religionRemove(eIndex, getOwnerINLINE(), this);
		}

		applyReligionModifiers(eIndex, bNewValue);
		//Team Project (5)
		checkReligiousDisablingAllBuildings();
	}
/************************************************************************************************/
/* RevolutionDCM  Inquisitions                             29/05/10           phungus420        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(!(GET_PLAYER((PlayerTypes)iI)).isHuman())
		{
			if( getTeam() == GET_PLAYER((PlayerTypes)iI).getTeam() 
			|| GET_TEAM(getTeam()).isVassal(GET_PLAYER((PlayerTypes)iI).getTeam()) )
			{
				GET_PLAYER((PlayerTypes)iI).AI_setHasInquisitionTarget();
			}
		}
	}
/************************************************************************************************/
/* RevolutionDCM	                         END                                                */
/************************************************************************************************/
}


void CvCity::processVoteSourceBonus(VoteSourceTypes eVoteSource, bool bActive)
{
	if (!GET_PLAYER(getOwnerINLINE()).isLoyalMember(eVoteSource))
	{
		return;
	}

	if (GC.getGameINLINE().isDiploVote(eVoteSource))
	{
		ReligionTypes eReligion = GC.getGameINLINE().getVoteSourceReligion(eVoteSource);

		SpecialistTypes eSpecialist = (SpecialistTypes)GC.getVoteSourceInfo(eVoteSource).getFreeSpecialist();
		if (NO_SPECIALIST != eSpecialist)
		{
			if (NO_RELIGION == eReligion || isHasReligion(eReligion))
			{
				changeFreeSpecialistCount(eSpecialist, bActive ? 1 : -1);
			}
		}

		if (NO_RELIGION != eReligion && isHasReligion(eReligion))
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				int iChange = GC.getVoteSourceInfo(eVoteSource).getReligionYield(iYield);
				if (!bActive)
				{
					iChange = -iChange;
				}

				if (0 != iChange)
				{
					for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
					{
						if (GC.getBuildingInfo((BuildingTypes)iBuilding).getReligionType() == eReligion)
						{
							changeBuildingYieldChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iBuilding).getBuildingClassType(), (YieldTypes)iYield, iChange);
						}
					}
				}
			}

			for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
			{
				int iChange = GC.getVoteSourceInfo(eVoteSource).getReligionCommerce(iCommerce);
				if (!bActive)
				{
					iChange = -iChange;
				}

				if (0 != iChange)
				{
					for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
					{
						if (GC.getBuildingInfo((BuildingTypes)iBuilding).getReligionType() == eReligion)
						{
							changeBuildingCommerceChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iBuilding).getBuildingClassType(), (CommerceTypes)iCommerce, iChange);
						}
					}
				}
			}
		}
	}
}


bool CvCity::isHasCorporation(CorporationTypes eIndex) const													 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex expected to be < GC.getNumCorporationInfos()");
	return m_pabHasCorporation[eIndex];
}

void CvCity::applyCorporationModifiers(CorporationTypes eIndex, bool bValue)
{
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).getPrereqCorporation() == eIndex)
		{
			if (bValue && isDisabledBuilding((BuildingTypes)iI))
			{
				setDisabledBuilding((BuildingTypes)iI, false);
			}
			else if (!bValue && getNumBuilding((BuildingTypes)iI) > 0)
			{
				setDisabledBuilding((BuildingTypes)iI, true);
			}
		}
	}

	changeMilitaryProductionModifier(GC.getCorporationInfo(eIndex).getMilitaryProductionModifier() * (bValue ? 1 : -1));
	changeFreeExperience(GC.getCorporationInfo(eIndex).getFreeXP() * (bValue ? 1 : -1));

	CvCity* pHeadquarters = GC.getGameINLINE().getHeadquarters(eIndex);

	if (NULL != pHeadquarters)
	{
		pHeadquarters->updateCorporation();
	}

	updateCorporation();
}

void CvCity::setHasCorporation(CorporationTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex expected to be < GC.getNumCorporationInfos()");

	if (isHasCorporation(eIndex) != bNewValue)
	{
		if (bNewValue)
		{
			bool bReplacedHeadquarters = false;
			for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
			{
				if (iCorp != eIndex && isHasCorporation((CorporationTypes)iCorp))
				{
					if (GC.getGameINLINE().isCompetingCorporation((CorporationTypes)iCorp, eIndex))
					{
						if (GC.getGameINLINE().getHeadquarters((CorporationTypes)iCorp) == this)
						{
							GC.getGameINLINE().replaceCorporation((CorporationTypes)iCorp, eIndex);
							bReplacedHeadquarters = true;
						}
						else
						{
							setHasCorporation((CorporationTypes)iCorp, false, false);
						}
					}
				}
			}

			if (bReplacedHeadquarters)
			{
				return; // already set the corporation in this city
			}
		}

		m_pabHasCorporation[eIndex] = bNewValue;

		GET_PLAYER(getOwnerINLINE()).changeHasCorporationCount(eIndex, ((isHasCorporation(eIndex)) ? 1 : -1));

		applyCorporationModifiers(eIndex, bNewValue);

		AI_setAssignWorkDirty(true);

		setInfoDirty(true);

		if (isHasCorporation(eIndex))
		{
			GC.getGameINLINE().makeCorporationFounded(eIndex, getOwnerINLINE());
		}

		if (bAnnounce)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (getOwnerINLINE() == iI || GET_PLAYER((PlayerTypes)iI).hasHeadquarters(eIndex))
					{
/************************************************************************************************/
/* Afforess	                  Start		 06/17/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
						if (bNewValue)
						{
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

							{
								MEMORY_TRACK_EXEMPT();

								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_SPREAD", GC.getCorporationInfo(eIndex).getTextKeyWide(), getNameKey());
								AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getCorporationInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), bArrows, bArrows);
							}

							if (getOwnerINLINE() == iI)
							{
								CvWStringBuffer szBonusString;
								GAMETEXT.setCorporationHelpCity(szBonusString, eIndex, this);

								CvWString szBonusList;
								bool bFirst = true;
								for (int iJ = 0; iJ < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++iJ)
								{
									int iBonus = GC.getCorporationInfo(eIndex).getPrereqBonus(iJ);
									if (iBonus != NO_BONUS)
									{
										CvWString szTemp;
										szTemp.Format(L"%s", GC.getBonusInfo((BonusTypes)iBonus).getDescription());
										setListHelp(szBonusList, L"", szTemp, L", ", bFirst);
										bFirst = false;
									}
								}
								if (!bFirst)
								{
									MEMORY_TRACK_EXEMPT();

									CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_SPREAD_BONUS", GC.getCorporationInfo(eIndex).getTextKeyWide(), szBonusString.getCString(), getNameKey(), szBonusList.GetCString());
									AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MINOR_EVENT, GC.getCorporationInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), bArrows, bArrows);
								}

							}
/************************************************************************************************/
/* Afforess	                  Start		 06/17/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
						}
						else
						{
							MEMORY_TRACK_EXEMPT();

							CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_DECAY", GC.getCorporationInfo(eIndex).getTextKeyWide(), getNameKey());
							AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getCorporationInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), bArrows, bArrows);
						}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
					}
				}
			}
		}

		if (bNewValue)
		{
			// Python Event
			CvEventReporter::getInstance().corporationSpread(eIndex, getOwnerINLINE(), this);
		}
		else
		{
			// Python Event
			CvEventReporter::getInstance().corporationRemove(eIndex, getOwnerINLINE(), this);
		}
	}
}


CvCity* CvCity::getTradeCity(int iIndex) const
{
	//int iMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();
	FAssert(iIndex >= 0);
	FAssert(iIndex < GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment());
	return getCity(m_paTradeCities[iIndex]);
}


int CvCity::getTradeRoutes() const
{
	int iTradeRoutes = GC.getGameINLINE().getTradeRoutes();
	iTradeRoutes += GET_PLAYER(getOwnerINLINE()).getTradeRoutes();
/************************************************************************************************/
/* Afforess	                  Start		 01/03/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iTradeRoutes += GET_PLAYER(getOwnerINLINE()).getWorldTradeRoutes();
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		iTradeRoutes += GET_PLAYER(getOwnerINLINE()).getCoastalTradeRoutes();
	}
	iTradeRoutes += getExtraTradeRoutes();
	int iMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();
	int iFinalTradeRoutes = std::min(iTradeRoutes, iMaxTradeRoutes);
	return std::max(0, iFinalTradeRoutes );
}


void CvCity::clearTradeRoutes()
{
	int iMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();

	for (int iI = 0; iI < iMaxTradeRoutes; iI++)
	{
		CvCity* pLoopCity = getTradeCity(iI);

		if (pLoopCity != NULL)
		{
			pLoopCity->setTradeRoute(getOwnerINLINE(), false);
		}

		m_paTradeCities[iI].reset();
	}
}


// XXX eventually, this needs to be done when roads are built/destroyed...
void CvCity::updateTradeRoutes()
{
	PROFILE_FUNC();
	int iMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();

	std::vector<int> paiBestValue(iMaxTradeRoutes, 0);

	clearTradeRoutes();

	if (!isDisorder() && !isPlundered() && !isQuarantined())
	{
		int iTradeRoutes = getTradeRoutes();

		FAssert(iTradeRoutes <= iMaxTradeRoutes);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER(getOwnerINLINE()).canHaveTradeRoutesWith((PlayerTypes)iI))
			{
				int iLoop = 0;
				for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					if (pLoopCity != this)
					{
						if (!(pLoopCity->isTradeRoute(getOwnerINLINE())) || (getTeam() == GET_PLAYER((PlayerTypes)iI).getTeam()))
						{
							if (pLoopCity->plotGroup(getOwnerINLINE()) == plotGroup(getOwnerINLINE()) || GC.getDefineINT("IGNORE_PLOT_GROUP_FOR_TRADE_ROUTES"))
							{
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
								int iValue = calculateTradeProfitTimes100(pLoopCity);
#else
								int iValue = calculateTradeProfit(pLoopCity);
#endif
// BUG - Fractional Trade Routes - end

								for (int iJ = 0; iJ < iTradeRoutes; iJ++)
								{
									if (iValue > paiBestValue[iJ])
									{
										for (int iK = (iTradeRoutes - 1); iK > iJ; iK--)
										{
											paiBestValue[iK] = paiBestValue[(iK - 1)];
											m_paTradeCities[iK] = m_paTradeCities[(iK - 1)];
										}

										paiBestValue[iJ] = iValue;
										m_paTradeCities[iJ] = pLoopCity->getIDInfo();

										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	int iTradeProfit = 0;

	for (int iI = 0; iI < iMaxTradeRoutes; iI++)
	{
		CvCity* pLoopCity = getTradeCity(iI);

		if (pLoopCity != NULL)
		{
			pLoopCity->setTradeRoute(getOwnerINLINE(), true);

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
			iTradeProfit += calculateTradeProfitTimes100(pLoopCity);
#else
			iTradeProfit += calculateTradeProfit(pLoopCity);
#endif
// BUG - Fractional Trade Routes - end
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		setTradeYield(((YieldTypes)iI), calculateTradeYield(((YieldTypes)iI), iTradeProfit) / 100); // XXX could take this out if handled when CvPlotGroup changes...
#else
		setTradeYield(((YieldTypes)iI), calculateTradeYield(((YieldTypes)iI), iTradeProfit)); // XXX could take this out if handled when CvPlotGroup changes...
#endif
// BUG - Fractional Trade Routes - end
	}
}


void CvCity::clearOrderQueue()
{
	while (headOrderQueueNode() != NULL)
	{
		popOrder(0, false, false, false);
	}

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		setInfoDirty(true);
	}
}


bool CvCity::pushFirstValidBuildListOrder(int iListID)
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	int index = kPlayer.m_pBuildLists->getIndexByID(iListID);
	if (index < 0)
		return false;
	int iNum = kPlayer.m_pBuildLists->getListLength(index);
	for (int i=0; i<iNum; i++)
	{
		OrderData* pOrder = kPlayer.m_pBuildLists->getOrder(index,i);
		
		if (canContinueProduction(*pOrder))
		{
			pushOrder(pOrder->eOrderType, pOrder->iData1, pOrder->iData2, pOrder->bSave, false, false);
			return true;
		}
	}
	return false;
}

void CvCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce, CvPlot* deliveryDestination, UnitAITypes contractedAIType, byte contractFlags)
{
	//bool bBuildingUnit = false;
	//bool bBuildingBuilding = false;

	if (bPop)
	{
		popOrder(0, false, false, false);
	}

	bool bValid = false;

	switch (eOrder)
	{
	case ORDER_TRAIN:
		if (canTrain((UnitTypes)iData1) || bForce)
		{
			unsigned short iAIType = EXTERNAL_ORDER_IDATA(iData2);
			unsigned short iAux;

			if (iAIType == 0xFFFF)
			{
				iAIType = (short)GC.getUnitInfo((UnitTypes)iData1).getDefaultUnitAIType();
				iAux = ((contractFlags & 0xFF) << 8) + 0xFF;
			}
			else
			{
				FAssert(contractedAIType < 0xFF);
				iAux = ((contractFlags & 0xFF) << 8) + (short)(contractedAIType & 0xFF);
			}

			iData2 = PACK_INTERNAL_ORDER_IDATA(iAIType, iAux);

			if (deliveryDestination != NULL)
			{
				int	iPlotIndex = GC.getMapINLINE().plotNumINLINE(deliveryDestination->getX_INLINE(), deliveryDestination->getY_INLINE());

				FAssert(iPlotIndex <= 0xFFFF);

				iData1 = PACK_INTERNAL_ORDER_IDATA(iData1, iPlotIndex);
			}
			else
			{
				iData1 = PACK_INTERNAL_ORDER_IDATA(iData1, 0xFFFF);
			}

			GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes)EXTERNAL_ORDER_IDATA(iData1)).getUnitClassType())), 1);

			area()->changeNumTrainAIUnits(getOwnerINLINE(), ((UnitAITypes)EXTERNAL_ORDER_IDATA(iData2)), 1);
			GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(((UnitAITypes)EXTERNAL_ORDER_IDATA(iData2)), 1);

			bValid = true;
			//bBuildingUnit = true;
			CvEventReporter::getInstance().cityBuildingUnit(this, (UnitTypes)EXTERNAL_ORDER_IDATA(iData1));
			setUnitListInvalid();
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
			/*                                                                                              */
			/* AI logging                                                                                   */
			/************************************************************************************************/
			if (gCityLogLevel >= 1)
			{
				CvWString szString;
				getUnitAIString(szString, (UnitAITypes)EXTERNAL_ORDER_IDATA(iData2));
				logBBAI("    City %S pushes production of unit %S with UNITAI %S", getName().GetCString(), GC.getUnitInfo((UnitTypes)EXTERNAL_ORDER_IDATA(iData1)).getDescription(getCivilizationType()), szString.GetCString());
			}
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/
		}
		break;

	case ORDER_CONSTRUCT:
		if (canConstruct((BuildingTypes)iData1) || bForce)
		{
			NoteBuildingNoLongerConstructable((BuildingTypes)iData1);

			GET_PLAYER(getOwnerINLINE()).changeBuildingClassMaking(((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iData1).getBuildingClassType())), 1);

			SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes)GC.getBuildingInfo((BuildingTypes)iData1).getSpecialBuildingType();
			if (eSpecialBuilding != NO_SPECIALBUILDING)
			{
				GET_PLAYER(getOwnerINLINE()).changeBuildingGroupMaking(eSpecialBuilding, 1);
			}

			bValid = true;
			//bBuildingBuilding = true;
			CvEventReporter::getInstance().cityBuildingBuilding(this, (BuildingTypes)iData1);
			setBuildingListInvalid();
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
			/*                                                                                              */
			/* AI logging                                                                                   */
			/************************************************************************************************/
			if (gCityLogLevel >= 1)
			{
				logBBAI("    City %S pushes production of building %S", getName().GetCString(), GC.getBuildingInfo((BuildingTypes)iData1).getDescription());
			}
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/
		}
		break;

	case ORDER_CREATE:
		if (canCreate((ProjectTypes)iData1) || bForce)
		{
			GET_TEAM(getTeam()).changeProjectMaking(((ProjectTypes)iData1), 1);

			bValid = true;
			// BUG - Project Started Event - start
			CvEventReporter::getInstance().cityBuildingProject(this, (ProjectTypes)iData1);
			// BUG - Project Started Event - end
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
			/*                                                                                              */
			/* AI logging                                                                                   */
			/************************************************************************************************/
			if (gCityLogLevel >= 1)
			{
				logBBAI("    City %S pushes production of project %S", getName().GetCString(), GC.getProjectInfo((ProjectTypes)iData1).getDescription());
			}
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/
		}
		break;

	case ORDER_MAINTAIN:
		if (canMaintain((ProcessTypes)iData1) || bForce)
		{
			bValid = true;
			// BUG - Process Started Event - start
			CvEventReporter::getInstance().cityBuildingProcess(this, (ProcessTypes)iData1);
			// BUG - Process Started Event - end
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
			/*                                                                                              */
			/* AI logging                                                                                   */
			/************************************************************************************************/
			if (gCityLogLevel >= 1)
			{
				logBBAI("    City %S pushes production of process %S", getName().GetCString(), GC.getProcessInfo((ProcessTypes)iData1).getDescription());
			}
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/
		}
		break;

	case ORDER_LIST:
		bValid = true;
		break;

	default:
		FAssertMsg(false, "iOrder did not match a valid option");
		break;
	}

	if (!bValid)
	{
		return;
	}

	OrderData order;
	order.eOrderType = eOrder;
	order.iData1 = iData1;
	order.iData2 = iData2;
	order.bSave = bSave;

	if (bAppend)
	{
		m_orderQueue.insertAtEnd(order);
	}
	else
	{
		stopHeadOrder();
		m_orderQueue.insertAtBeginning(order);
	}

	if (!bAppend || (getOrderQueueLength() == 1))
	{
		// If the head order is a build list, resolve it
		if (eOrder == ORDER_LIST)
		{
			if (!pushFirstValidBuildListOrder(iData1))
			{
				// pop the list if there is nothing to construct on it any more
				popOrder(0);
			}
			else
			{
				if (!bSave)
				{
					popOrder(1);
				}
			}
		}
		else
		{
			startHeadOrder();
		}
	}

	// Why does this cause a crash???

/*	if (bBuildingUnit)
	{
		CvEventReporter::getInstance().cityBuildingUnit(this, (UnitTypes)iData1);
	}
	else if (bBuildingBuilding)
	{
		CvEventReporter::getInstance().cityBuildingBuilding(this, (BuildingTypes)iData1);
	}*/

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		setInfoDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			//gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			//gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}

void CvCity::popOrder(int iNum, bool bFinish, bool bChoose, bool bResolveList)
{
	PROFILE_FUNC();

	bool bWasFoodProduction = isFoodProduction();

	if (iNum == -1)
	{
		iNum = (getOrderQueueLength() - 1);
	}

	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (iCount == iNum)
		{
			break;
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	if (pOrderNode == NULL)
	{
		return;
	}

	if (bFinish && pOrderNode->m_data.bSave)
	{
		pushOrder(pOrderNode->m_data.eOrderType, EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1), EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2), true, false, true);
	}

	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;
	ProjectTypes eCreateProject = NO_PROJECT;

	switch (pOrderNode->m_data.eOrderType)
	{
	case ORDER_TRAIN:
	{
		int iTrainUnit = EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1);
		int iTrainAIUnit = EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2);

		if (iTrainUnit > -1 && iTrainUnit < GC.getNumUnitInfos() && iTrainAIUnit > -1 && iTrainAIUnit < NUM_UNITAI_TYPES)
		{
			eTrainUnit = ((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1));
			UnitAITypes eTrainAIUnit = ((UnitAITypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2));
			FAssertMsg(eTrainUnit != NO_UNIT, "eTrainUnit is expected to be assigned a valid unit type");
			FAssertMsg(eTrainAIUnit != NO_UNITAI, "eTrainAIUnit is expected to be assigned a valid unit AI type");

			GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo(eTrainUnit).getUnitClassType())), -1);

			area()->changeNumTrainAIUnits(getOwnerINLINE(), eTrainAIUnit, -1);
			GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(eTrainAIUnit, -1);

			setUnitListInvalid();

			if (bFinish)
			{
				AI_trained(eTrainUnit, eTrainAIUnit);

				if (gCityLogLevel >= 2)
				{
					logBBAI("      City %S builds unit %S",
						getName().GetCString(),
						GC.getUnitInfo(eTrainUnit).getDescription());
				}

				int iProductionNeeded = getProductionNeeded(eTrainUnit);

				// max overflow is the value of the item produced (to eliminate prebuild exploits)
				/************************************************************************************************/
				/* Afforess	Multiple Production Mod		 08/23/09                                            */
				/*                                                                                              */
				/*                                                                                              */
				/************************************************************************************************/
				//					iOverflow = getUnitProduction(eTrainUnit) - iProductionNeeded;
				int iUnlimitedOverflow = getUnitProduction(eTrainUnit) - iProductionNeeded;
				/************************************************************************************************/
				/* Afforess	Multiple Production Mod       END                                                */
				/************************************************************************************************/
				int iMaxOverflow = std::max(iProductionNeeded, getCurrentProductionDifference(false, false));
				/************************************************************************************************/
				/* Afforess	Multiple Production Mod		 08/23/09                                            */
				/*                                                                                              */
				/*                                                                                              */
				/************************************************************************************************/
				//					int iMaxOverflowForGold = std::max(iProductionNeeded, getProductionDifference(getProductionNeeded(), getProduction(), 0, isFoodProduction(), false));
				//					iOverflow = std::min(iMaxOverflow, iOverflow);
				int iLostProduction = std::max(0, iUnlimitedOverflow - iMaxOverflow);
				m_iLostProductionModified = iLostProduction;
				m_iLostProductionBase = (100 * iLostProduction) / std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier(eTrainUnit)));
				int iOverflow = std::min(iMaxOverflow, iUnlimitedOverflow);
				/************************************************************************************************/
				/* Afforess	Multiple Production Mod       END                                                */
				/************************************************************************************************/
				if (iOverflow > 0)
				{
					changeOverflowProduction(iOverflow, getProductionModifier(eTrainUnit));
				}
				setUnitProduction(eTrainUnit, 0);

				// Unofficial Patch Start
				// * Limited which production modifiers affect gold from production overflow. 1/3
				iLostProduction *= getBaseYieldRateModifier(YIELD_PRODUCTION);
				iLostProduction /= std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier(eTrainUnit)));
				// Unofficial Patch End
				/************************************************************************************************/
				/* Afforess	Multiple Production Mod		 08/23/09                                            */
				/*                                                                                              */
				/*                                                                                              */
				/************************************************************************************************/
				//					int iProductionGold = std::max(0, iOverflow - iMaxOverflowForGold) * GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT") / 100;
				int iProductionGold = ((iLostProduction * GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT")) / 100);
				m_iGoldFromLostProduction = iProductionGold;
				/************************************************************************************************/
				/* Afforess	Multiple Production Mod       END                                                */
				/************************************************************************************************/

				/************************************************************************************************/
				/* Afforess	Multiple Production Mod		 08/23/09                                            */
				/*                                                                                              */
				/*                                                                                              */
				/************************************************************************************************/
				//			if (iProductionGold > 0)
				//			{
				//				GET_PLAYER(getOwnerINLINE()).changeGold(iProductionGold);
				//			}
				/************************************************************************************************/
				/* Afforess	Multiple Production Mod       END                                                */
				/************************************************************************************************/

				CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eTrainUnit, getX_INLINE(), getY_INLINE(), eTrainAIUnit, NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));
				FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");
				/************************************************************************************************/
				/* Afforess	                  Start		 06/22/10                                               */
				/*                                                                                              */
				/*                                                                                              */
				/************************************************************************************************/
				if (GC.getGameINLINE().isModderGameOption(MODDERGAMEOPTION_MAX_UNITS_PER_TILES))
				{
					if (!pUnit->canMoveInto(plot(), false, false, false, false, true))
					{
						pUnit->jumpToNearestValidPlot(false);
					}
				}
				/************************************************************************************************/
				/* Afforess	                     END                                                            */
				/************************************************************************************************/
				pUnit->finishMoves();

				addProductionExperience(pUnit);

				short iPlotIndex = INTERNAL_AUXILIARY_ORDER_IDATA(pOrderNode->m_data.iData1);
				int iFlags;

				CvPlot* pRallyPlot = NULL;
				if (iPlotIndex != (short)0xFFFF)
				{
					iFlags = MOVE_NO_ENEMY_TERRITORY;
					pRallyPlot = GC.getMapINLINE().plotByIndexINLINE(iPlotIndex);
					if (pRallyPlot != NULL && gUnitLogLevel >= 3)
					{
						logBBAI("    New unit %S at (%d,%d) headed to contractual delivery plot (%d,%d)",
							pUnit->getUnitInfo().getDescription(),
							plot()->getX_INLINE(),
							plot()->getY_INLINE(),
							pRallyPlot->getX_INLINE(),
							pRallyPlot->getY_INLINE());
					}
				}
				else
				{
					iFlags = 0;
					pRallyPlot = getRallyPlot();
				}

				if (pRallyPlot != NULL)
				{
					bool bIsUnitMission = (((INTERNAL_AUXILIARY_ORDER_IDATA(pOrderNode->m_data.iData2) >> 8) & AUX_CONTRACT_FLAG_IS_UNIT_CONTRACT) != 0);

					if (pRallyPlot != plot())
					{
						pUnit->getGroup()->pushMission(MISSION_MOVE_TO,
							pRallyPlot->getX_INLINE(),
							pRallyPlot->getY_INLINE(),
							iFlags,
							false,
							false,
							bIsUnitMission ? MISSIONAI_CONTRACT_UNIT : MISSIONAI_CONTRACT,
							pRallyPlot);
					}
					else
					{
						pUnit->getGroup()->AI_setMissionAI(bIsUnitMission ? MISSIONAI_CONTRACT_UNIT : MISSIONAI_CONTRACT, plot(), NULL);
					}
				}
				/************************************************************************************************/
				/* Afforess	                  Start		 09/15/10                                               */
				/*                                                                                              */
				/*  There seems to be an issue with AI missionaries not working correctly - forcing automation  */
				/* is a kludgy way to fix this                                                                  */
				/************************************************************************************************/
				if (!isHuman())
				{
					pUnit->automate(AUTOMATE_RELIGION);
				}
				/************************************************************************************************/
				/* Afforess	                     END                                                            */
				/************************************************************************************************/
				if (isHuman())
				{
					if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_START_AUTOMATED))
					{
						pUnit->automate(AUTOMATE_BUILD);
					}

					if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MISSIONARIES_AUTOMATED))
					{
						pUnit->automate(AUTOMATE_RELIGION);
					}
					/************************************************************************************************/
					/* Afforess	                  Start		 01/14/10                                               */
					/*                                                                                              */
					/*                                                                                              */
					/************************************************************************************************/
					if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_2))
					{
						CvPlot* pPlot = plot();
						if (pPlot != NULL)
						{
							if (pUnit->canSleep(pPlot))
							{
								pUnit->getGroup()->setActivityType(ACTIVITY_SLEEP);
							}
							else if (pUnit->canFortify(pPlot))
							{
								pUnit->getGroup()->setActivityType(ACTIVITY_SLEEP);
							}
						}
					}
					/************************************************************************************************/
					/* Afforess	                     END                                                            */
					/************************************************************************************************/
				}

#ifdef CAN_TRAIN_CACHING
				//	Training a unit can mean we might not be abel to build any more of them
				//	so clear its entry in the canTrain cache to force recalculation
				invalidateCachedCanTrainForUnit(eTrainUnit);
#endif

				//	KOSHLING - must not hold onto the pointer after the Python call or
				//	a crash occurs if that Python decides to destroy the just-built unit
				int iUnitId = pUnit->getID();

				CvEventReporter::getInstance().unitBuilt(this, pUnit);

				//	Python may have destroyed the unit we just built so refind by id
				pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
				if (pUnit != NULL)
				{
					/************************************************************************************************/
					/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
					/*                                                                                              */
					/* AI logging                                                                                   */
					/************************************************************************************************/
					if (gCityLogLevel >= 1)
					{
						CvWString szString;
						getUnitAIString(szString, pUnit->AI_getUnitAIType());
						logBBAI("    City %S finishes production of unit %S with UNITAI %S", getName().GetCString(), pUnit->getName(0).GetCString(), szString.GetCString());
					}
					/************************************************************************************************/
					/* BETTER_BTS_AI_MOD                       END                                                  */
					/************************************************************************************************/

					if (GC.getUnitInfo(eTrainUnit).getDomainType() == DOMAIN_AIR)
					{
						if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
						{
							if (plot()->countNumAirUnitCargoVolume(getTeam()) > getSMAirUnitCapacity(getTeam()))
							{
								//probably need to look into this too.
								pUnit->jumpToNearestValidPlot();
							}
						}
						else if (plot()->countNumAirUnits(getTeam()) > getAirUnitCapacity(getTeam()))
						{
							pUnit->jumpToNearestValidPlot();  // can destroy unit
						}
					}

				}
			}
		}
	}
	break;

	case ORDER_CONSTRUCT:

	{

		eConstructBuilding = ((BuildingTypes)(pOrderNode->m_data.iData1));

		GET_PLAYER(getOwnerINLINE()).changeBuildingClassMaking(((BuildingClassTypes)(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType())), -1);

		SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes)GC.getBuildingInfo(eConstructBuilding).getSpecialBuildingType();
		if (eSpecialBuilding != NO_SPECIALBUILDING)
		{
			GET_PLAYER(getOwnerINLINE()).changeBuildingGroupMaking(eSpecialBuilding, -1);
		}

		if (bFinish)
		{
			/*************************************************************************************************/
			/* UNOFFICIAL_PATCH                       10/08/09                  davidlallen & jdog5000       */
			/*                                                                                               */
			/* Bugfix                                                                                        */
			/*************************************************************************************************/
			/* original bts code
						if (GET_PLAYER(getOwnerINLINE()).isBuildingClassMaxedOut(((BuildingClassTypes)(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType())), 1))
			*/
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo(eConstructBuilding).getBuildingClassType();
			if (GET_PLAYER(getOwnerINLINE()).isBuildingClassMaxedOut(eBuildingClass))
				/*************************************************************************************************/
				/* UNOFFICIAL_PATCH                         END                                                  */
				/*************************************************************************************************/
			{
				GET_PLAYER(getOwnerINLINE()).removeBuildingClass((BuildingClassTypes)(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType()));
			}

			setNumRealBuilding(eConstructBuilding, getNumRealBuilding(eConstructBuilding) + 1);

			AI_setPropertyControlBuildingQueued(false);

			if (gCityLogLevel >= 2)
			{
				logBBAI("      City %S builds building %S",
					getName().GetCString(),
					GC.getBuildingInfo(eConstructBuilding).getDescription());
			}

			int iProductionNeeded = getProductionNeeded(eConstructBuilding);
			// max overflow is the value of the item produced (to eliminate prebuild exploits)
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod		 08/23/09                                            */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
			//			int iOverflow = getBuildingProduction(eConstructBuilding) - iProductionNeeded;
			int iUnlimitedOverflow = getBuildingProduction(eConstructBuilding) - iProductionNeeded;
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod       END                                                */
			/************************************************************************************************/
			int iMaxOverflow = std::max(iProductionNeeded, getCurrentProductionDifference(false, false));
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod		 08/23/09                                            */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
			//			int iMaxOverflowForGold = std::max(iProductionNeeded, getProductionDifference(getProductionNeeded(), getProduction(), 0, isFoodProduction(), false));
			//			iOverflow = std::min(iMaxOverflow, iOverflow);
			int iLostProduction = std::max(0, iUnlimitedOverflow - iMaxOverflow);
			m_iLostProductionModified = iLostProduction;
			m_iLostProductionBase = (100 * iLostProduction) / std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier(eConstructBuilding)));
			int iOverflow = std::min(iMaxOverflow, iUnlimitedOverflow);
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod       END                                                */
			/************************************************************************************************/
			if (iOverflow > 0)
			{
				changeOverflowProduction(iOverflow, getProductionModifier(eConstructBuilding));
			}
			setBuildingProduction(eConstructBuilding, 0);
			/*************************************************************************************************/
			/* UNOFFICIAL_PATCH                       06/10/10                           EmperorFool         */
			/*                                                                                               */
			/* Bugfix                                                                                        */
			/*************************************************************************************************/
			setBuildingProductionTime(eConstructBuilding, 0);
			/*************************************************************************************************/
			/* UNOFFICIAL_PATCH                         END                                                  */
			/*************************************************************************************************/

						// Unofficial Patch Start
						// * Limited which production modifiers affect gold from production overflow. 2/3
			iLostProduction *= getBaseYieldRateModifier(YIELD_PRODUCTION);
			iLostProduction /= std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier(eConstructBuilding)));
			// Unofficial Patch End
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod		 08/23/09                                            */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
			//			int iProductionGold = std::max(0, iOverflow - iMaxOverflowForGold) * GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT") / 100;
			int iProductionGold = ((iLostProduction * GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT")) / 100);
			m_iGoldFromLostProduction = iProductionGold;

			//			if (iProductionGold > 0)
			//			{
			//				GET_PLAYER(getOwnerINLINE()).changeGold(iProductionGold);
			//			}
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod       END                                                */
			/************************************************************************************************/

			CvEventReporter::getInstance().buildingBuilt(this, eConstructBuilding);
		}
		/************************************************************************************************/
		/* Afforess	                  Start		 06/13/10                                               */
		/*                                                                                              */
		/*                                                                                              */
		/************************************************************************************************/
		else if (!canConstruct(eConstructBuilding))
		{
			if (GC.getBuildingInfo(eConstructBuilding).getProductionContinueBuildingClass() != NO_BUILDINGCLASS)
			{
				BuildingTypes eBuilding = (BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)GC.getBuildingInfo(eConstructBuilding).getProductionContinueBuildingClass()));
				if (eBuilding != NO_BUILDING)
				{
					if (canConstruct(eBuilding, true, false, false, false))
					{
						if (m_iLostProduction == 0)
						{
							m_iLostProduction = getBuildingProduction(eConstructBuilding);
							setBuildingProduction(eConstructBuilding, 0);
						}
						if (m_iLostProduction > 0)
						{
							MEMORY_TRACK_EXEMPT();

							setBuildingProduction(eBuilding, m_iLostProduction);
							CvWString szMessage;
							szMessage = gDLL->getText("TXT_KEY_MISC_PROD_CONVERTED", m_iLostProduction, GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), GC.getBuildingInfo(eBuilding).getTextKeyWide());
							AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_PRODUCTION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

							m_iLostProduction = 0;
						}
					}
				}
			}
		}

		/************************************************************************************************/
		/* Afforess	                     END                                                            */
		/************************************************************************************************/
		FlushCanConstructCache(eConstructBuilding);	//	Flush value for this building
		setBuildingListInvalid();
		break;
	}
	case ORDER_CREATE:
		eCreateProject = ((ProjectTypes)(pOrderNode->m_data.iData1));

		GET_TEAM(getTeam()).changeProjectMaking(eCreateProject, -1);

		if (bFinish)
		{
			OutputDebugString(CvString::format("Project %d (%S) built\n", eCreateProject, GC.getProjectInfo(eCreateProject).getDescription()).c_str());

			// Event reported to Python before the project is built, so that we can show the movie before awarding free techs, for example
			CvEventReporter::getInstance().projectBuilt(this, eCreateProject);

			GET_TEAM(getTeam()).changeProjectCount(eCreateProject, 1);

			if (GC.getProjectInfo(eCreateProject).isSpaceship())
			{
				bool needsArtType = true;
				VictoryTypes eVictory = (VictoryTypes)GC.getProjectInfo(eCreateProject).getVictoryPrereq();

				if (NO_VICTORY != eVictory && GET_TEAM(getTeam()).canLaunch(eVictory))
				{
					if (isHuman())
					{
						CvPopupInfo* pInfo = NULL;

						if (GC.getGameINLINE().isNetworkMultiPlayer())
						{
							pInfo = new CvPopupInfo(BUTTONPOPUP_LAUNCH, GC.getProjectInfo(eCreateProject).getVictoryPrereq());
						}
						else
						{
							pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN, eCreateProject);
							pInfo->setText(L"showSpaceShip");
							needsArtType = false;
						}

						gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE());
					}
					else
					{
						GET_PLAYER(getOwnerINLINE()).AI_launch(eVictory);
					}
				}
				else
				{
					//show the spaceship progress
					if (isHuman())
					{
						if (!GC.getGameINLINE().isNetworkMultiPlayer())
						{
							CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN, eCreateProject);
							pInfo->setText(L"showSpaceShip");
							gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE());
							needsArtType = false;
						}
					}
				}

				if (needsArtType)
				{
					int defaultArtType = GET_TEAM(getTeam()).getProjectDefaultArtType(eCreateProject);
					int projectCount = GET_TEAM(getTeam()).getProjectCount(eCreateProject);
					GET_TEAM(getTeam()).setProjectArtType(eCreateProject, projectCount - 1, defaultArtType);
				}
			}

			int iProductionNeeded = getProductionNeeded(eCreateProject);
			// max overflow is the value of the item produced (to eliminate pre-build exploits)
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod		 08/23/09                                            */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
			//			iOverflow = getProjectProduction(eCreateProject) - iProductionNeeded;
			int iUnlimitedOverflow = getProjectProduction(eCreateProject) - iProductionNeeded;
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod       END                                                */
			/************************************************************************************************/
			int iMaxOverflow = std::max(iProductionNeeded, getCurrentProductionDifference(false, false));
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod		 08/23/09                                            */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
			//			int iMaxOverflowForGold = std::max(iProductionNeeded, getProductionDifference(getProductionNeeded(), getProduction(), 0, isFoodProduction(), false));
			//			iOverflow = std::min(iMaxOverflow, iOverflow);
			int iLostProduction = std::max(0, iUnlimitedOverflow - iMaxOverflow);
			m_iLostProductionModified = iLostProduction;
			m_iLostProductionBase = (100 * iLostProduction) / std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier(eCreateProject)));
			int iOverflow = std::min(iMaxOverflow, iUnlimitedOverflow);
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod       END                                                */
			/************************************************************************************************/
			if (iOverflow > 0)
			{
				changeOverflowProduction(iOverflow, getProductionModifier(eCreateProject));
			}
			setProjectProduction(eCreateProject, 0);

			// Unofficial Patch Start
			// * Limited which production modifiers affect gold from production overflow. 3/3
			iLostProduction *= getBaseYieldRateModifier(YIELD_PRODUCTION);
			iLostProduction /= std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier(eCreateProject)));
			// Unofficial Patch End
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod		 08/23/09                                            */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
			//			int iProductionGold = std::max(0, iOverflow - iMaxOverflowForGold) * GC.getDefineINT("MAXED_PROJECT_GOLD_PERCENT") / 100;
			int iProductionGold = ((iLostProduction * GC.getDefineINT("MAXED_PROJECT_GOLD_PERCENT")) / 100);
			m_iGoldFromLostProduction = iProductionGold;


			//			if (iProductionGold > 0)
			//			{
			//				GET_PLAYER(getOwnerINLINE()).changeGold(iProductionGold);
			//			}
			/************************************************************************************************/
			/* Afforess	Multiple Production Mod       END                                                */
			/************************************************************************************************/
		}
		break;

	case ORDER_MAINTAIN:
	case ORDER_LIST:
		break;

	default:
		FAssertMsg(false, "pOrderNode->m_data.eOrderType is not a valid option");
		break;
	}

	bool bStart = pOrderNode == headOrderQueueNode();
	if (bStart)
	{
		stopHeadOrder();
	}

	m_orderQueue.deleteNode(pOrderNode);
	pOrderNode = NULL;

	if (bStart)
	{
		startHeadOrder();
	}

	//if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	//{
	//	setInfoDirty(true);

	//	if (isCitySelected())
	//	{
	//		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
	//		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	//		//gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
	//	}
	//}

	if (bResolveList)
	{
		// Check if head of queue is a build list and resolve it in that case
		pOrderNode = headOrderQueueNode();
		if (pOrderNode)
		{
			if (pOrderNode->m_data.eOrderType == ORDER_LIST)
			{
				bool bSave = pOrderNode->m_data.bSave;
				int iData1 = pOrderNode->m_data.iData1;
				
				if (!pushFirstValidBuildListOrder(iData1))
				{
					// pop the list if there is nothing to construct on it any more
					popOrder(0);
				}
				else
				{
					if (!bSave)
					{
						popOrder(1);
					}
				}
			}
		}
	}

	if (bChoose)
	{
		if (getOrderQueueLength() == 0)
		{
			if (!isHuman() || isProductionAutomated())
			{
				AI_chooseProduction();
			}
			else
			{
				if (bWasFoodProduction)
				{
					AI_assignWorkingPlots();
				}

				chooseProduction(eTrainUnit, eConstructBuilding, eCreateProject, bFinish);
			}
		}
	}


	if (bFinish)
	{
		LPCSTR szIcon = NULL;
		wchar szBuffer[1024];
		TCHAR szSound[1024];
		if (eTrainUnit != NO_UNIT)
		{
			swprintf(szBuffer, gDLL->getText(((isLimitedUnitClass((UnitClassTypes)(GC.getUnitInfo(eTrainUnit).getUnitClassType()))) ? "TXT_KEY_MISC_TRAINED_UNIT_IN_LIMITED" : "TXT_KEY_MISC_TRAINED_UNIT_IN"), GC.getUnitInfo(eTrainUnit).getTextKeyWide(), getNameKey()).GetCString());
			strcpy( szSound, GC.getUnitInfo(eTrainUnit).getArtInfo(0,GET_PLAYER(getOwnerINLINE()).getCurrentEra(), NO_UNIT_ARTSTYLE)->getTrainSound() );
			szIcon = GET_PLAYER(getOwnerINLINE()).getUnitButton(eTrainUnit);
		}
		else if (eConstructBuilding != NO_BUILDING)
		{
			swprintf(szBuffer, gDLL->getText(((isLimitedWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType()))) ? "TXT_KEY_MISC_CONSTRUCTED_BUILD_IN_LIMITED" : "TXT_KEY_MISC_CONSTRUCTED_BUILD_IN"), GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), getNameKey()).GetCString());
			strcpy(szSound, GC.getBuildingInfo(eConstructBuilding).getConstructSound());
			szIcon = GC.getBuildingInfo(eConstructBuilding).getButton();
		}
		else if (eCreateProject != NO_PROJECT)
		{
			swprintf(szBuffer, gDLL->getText(((isLimitedProject(eCreateProject)) ? "TXT_KEY_MISC_CREATED_PROJECT_IN_LIMITED" : "TXT_KEY_MISC_CREATED_PROJECT_IN"), GC.getProjectInfo(eCreateProject).getTextKeyWide(), getNameKey()).GetCString());
			strcpy(szSound, GC.getProjectInfo(eCreateProject).getCreateSound());
			szIcon = GC.getProjectInfo(eCreateProject).getButton();
		}
		if (isProduction())
		{
			wchar szTempBuffer[1024];
			swprintf(szTempBuffer, gDLL->getText(((isProductionLimited()) ? "TXT_KEY_MISC_WORK_HAS_BEGUN_LIMITED" : "TXT_KEY_MISC_WORK_HAS_BEGUN"), getProductionNameKey()).GetCString());
			wcscat(szBuffer, szTempBuffer);
		}
		{
			MEMORY_TRACK_EXEMPT();

			AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, szSound, MESSAGE_TYPE_MINOR_EVENT, szIcon, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
		}
	}

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		setInfoDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			//gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			//gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}


void CvCity::startHeadOrder()
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_MAINTAIN)
		{
			processProcess(((ProcessTypes)(pOrderNode->m_data.iData1)), 1);
		}

		AI_setAssignWorkDirty(true);
	}
}


void CvCity::stopHeadOrder()
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_MAINTAIN)
		{
			processProcess(((ProcessTypes)(pOrderNode->m_data.iData1)), -1);
		}
	}
}


int CvCity::getOrderQueueLength()
{
	return m_orderQueue.getLength();
}


OrderData* CvCity::getOrderFromQueue(int iIndex)
{
	CLLNode<OrderData>* pOrderNode;

	pOrderNode = m_orderQueue.nodeNum(iIndex);

	if (pOrderNode != NULL)
	{
		return &(pOrderNode->m_data);
	}
	else
	{
		return NULL;
	}
}


CLLNode<OrderData>* CvCity::nextOrderQueueNode(CLLNode<OrderData>* pNode) const
{
	return m_orderQueue.next(pNode);
}


CLLNode<OrderData>* CvCity::headOrderQueueNode() const
{
	return m_orderQueue.head();
}

int CvCity::getNumOrdersQueued() const
{
	return m_orderQueue.getLength();
}

OrderData CvCity::getOrderDataInternal(int iIndex, bool externalView) const
{
	OrderData kData;
	int iCount = 0;
	CLLNode<OrderData>* pNode = headOrderQueueNode();
	while (pNode != NULL)
	{
		if (iIndex == iCount)
		{
			kData.eOrderType = pNode->m_data.eOrderType;
			kData.iData1 = externalView ? EXTERNAL_ORDER_IDATA(pNode->m_data.iData1) : pNode->m_data.iData1;
			kData.iData2 = externalView ? EXTERNAL_ORDER_IDATA(pNode->m_data.iData2) : pNode->m_data.iData2;
			kData.bSave = pNode->m_data.bSave;

			return kData;
		}
		iCount++;
		pNode = nextOrderQueueNode(pNode);
	}
	kData.eOrderType = NO_ORDER;
	kData.iData1 = -1;
	kData.iData2 = -1;
	kData.bSave = false;
	return kData;
}

OrderData CvCity::getOrderData(int iIndex) const
{
	return getOrderDataInternal(iIndex);
}

void CvCity::setWallOverridePoints(const std::vector< std::pair<float, float> >& kPoints)
{
	m_kWallOverridePoints = kPoints;
	setLayoutDirty(true);
}


const std::vector< std::pair<float, float> >& CvCity::getWallOverridePoints() const
{
	return m_kWallOverridePoints;
}

// Protected Functions...

void CvCity::doGrowth()
{
/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_DO_GROWTH_CALLBACK())
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doGrowth", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	int iDiff = foodDifference();

	changeFood(iDiff);
	changeFoodKept(iDiff);

	setFoodKept(range(getFoodKept(), 0, ((growthThreshold() * getMaxFoodKeptPercent()) / 100)));

	if (getFood() >= growthThreshold())
	{
/************************************************************************************************/
/* Afforess	                  Start		 06/28/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
		if (AI_isEmphasizeAvoidGrowth())
*/
		if ((isHuman() && AI_avoidGrowth()) || AI_isEmphasizeAvoidGrowth())
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		{
			setFood(growthThreshold());
		}
		else
		{
			changeFood(-(std::max(0, (growthThreshold() - getFoodKept()))));
			changePopulation(1);

			// ONEVENT - City growth
			CvEventReporter::getInstance().cityGrowth(this, getOwnerINLINE());
		}
		//ls612: Remove buildings which obsolete at a certain pop level
		//Re-enable them if the city shrinks?
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			if (getNumActiveBuilding((BuildingTypes) iI) > 0 && GC.getBuildingInfo((BuildingTypes) iI).getMaxPopAllowed() == getPopulation())
			{
				processBuilding((BuildingTypes) iI, -getNumActiveBuilding((BuildingTypes) iI), true);
			}
		}
	}

	else if (getFood() < 0)
	{
		changeFood(-(getFood()));

		if (getPopulation() > 1)
		{
			changePopulation(-1);
		}
	}
}


void CvCity::doCulture()
{
	PROFILE_FUNC();
	
/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_DO_CULTURE_CALLBACK())
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doCulture", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	changeCultureTimes100(getOwnerINLINE(), getCommerceRateTimes100(COMMERCE_CULTURE), false, true);
}


void CvCity::doPlotCulture(bool bUpdate, PlayerTypes ePlayer, int iCultureRate)
{
	PROFILE_FUNC();

/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_DO_PLOT_CULTURE_CALLBACK())
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(bUpdate);
		argsList.add(ePlayer);
		argsList.add(iCultureRate);
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doPlotCulture", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	FAssert(NO_PLAYER != ePlayer);

	CultureLevelTypes eCultureLevel = (CultureLevelTypes)0;
	if (getOwnerINLINE() == ePlayer)
	{
		eCultureLevel = getCultureLevel();
	}
	else
	{
		for (int iI = (GC.getNumCultureLevelInfos() - 1); iI > 0; iI--)
		{
			if (getCultureTimes100(ePlayer) >= 100 * GC.getGameINLINE().getCultureThreshold((CultureLevelTypes)iI))
			{
				eCultureLevel = (CultureLevelTypes)iI;
				break;
			}
		}
	}

	int iFreeCultureRate = GC.getDefineINT("CITY_FREE_CULTURE_GROWTH_FACTOR");
	if (getCultureTimes100(ePlayer) > 0)
	{
		if (eCultureLevel != NO_CULTURELEVEL)
		{
			clearCultureDistanceCache();
			for (int iDX = -eCultureLevel; iDX <= eCultureLevel; iDX++)
			{
				for (int iDY = -eCultureLevel; iDY <= eCultureLevel; iDY++)
				{
					int iCultureRange = cultureDistance(iDX, iDY);

					if (iCultureRange <= eCultureLevel)
					{
						CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->isPotentialCityWorkForArea(area()))
							{
								pLoopPlot->changeCulture(ePlayer, (((eCultureLevel - iCultureRange) * iFreeCultureRate) + iCultureRate + 1), (bUpdate || !(pLoopPlot->isOwned())));
							}
						}
					}
				}
			}
		}
	}
}

bool CvCity::doCheckProduction()
{
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (getUnitProduction((UnitTypes)iI) > 0)
		{
			if (GET_PLAYER(getOwnerINLINE()).isProductionMaxedUnitClass((UnitClassTypes)(GC.getUnitInfo((UnitTypes)iI).getUnitClassType())))
			{
				int iProductionGold = ((getUnitProduction((UnitTypes)iI) * GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT")) / 100);

				if (iProductionGold > 0)
				{
					GET_PLAYER(getOwnerINLINE()).changeGold(iProductionGold);

					MEMORY_TRACK_EXEMPT();

					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED", getNameKey(), GC.getUnitInfo((UnitTypes)iI).getTextKeyWide(), iProductionGold);
					AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
				}

				setUnitProduction(((UnitTypes)iI), 0);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getBuildingProduction((BuildingTypes)iI) > 0)
		{
			if (GET_PLAYER(getOwnerINLINE()).isProductionMaxedBuildingClass((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())))
			{

				int iProductionGold = ((getBuildingProduction((BuildingTypes)iI) * GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT")) / 100);
/************************************************************************************************/
/* Afforess	                  Start		 06/13/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
				if (GC.getBuildingInfo((BuildingTypes)iI).getProductionContinueBuildingClass() != NO_BUILDINGCLASS)
				{
					BuildingTypes eBuilding = (BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI).getProductionContinueBuildingClass()));
					if (eBuilding != NO_BUILDING)
					{
						if (canConstruct(eBuilding))
						{
							//setBuildingProduction(eBuilding, getBuildingProduction((BuildingTypes)iI));
							iProductionGold = 0;
							m_iLostProduction = getBuildingProduction((BuildingTypes)iI);
							
							//szBuffer = DLL_SERIALIZE(gDLL->getText("TXT_KEY_MISC_PROD_CONVERTED", getBuildingProduction((BuildingTypes)iI),  GC.getBuildingInfo((BuildingTypes)iI).getTextKeyWide(), GC.getBuildingInfo(eBuilding).getTextKeyWide()));
							//AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_PRODUCTION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
						}
					}
				}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				if (iProductionGold > 0)
				{
					GET_PLAYER(getOwnerINLINE()).changeGold(iProductionGold);

					MEMORY_TRACK_EXEMPT();

					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED", getNameKey(), GC.getBuildingInfo((BuildingTypes)iI).getTextKeyWide(), iProductionGold);
					AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
				}

				setBuildingProduction(((BuildingTypes)iI), 0);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (getProjectProduction((ProjectTypes)iI) > 0)
		{
			if (GET_PLAYER(getOwnerINLINE()).isProductionMaxedProject((ProjectTypes)iI))
			{
				int iProductionGold = ((getProjectProduction((ProjectTypes)iI) * GC.getDefineINT("MAXED_PROJECT_GOLD_PERCENT")) / 100);

				if (iProductionGold > 0)
				{
					GET_PLAYER(getOwnerINLINE()).changeGold(iProductionGold);

					MEMORY_TRACK_EXEMPT();

					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED", getNameKey(), GC.getProjectInfo((ProjectTypes)iI).getTextKeyWide(), iProductionGold);
					AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
				}

				setProjectProduction(((ProjectTypes)iI), 0);
			}
		}
	}

	if (!isProduction() && !isDisorder() && isHuman() && !isProductionAutomated())
	{
		chooseProduction();
		return true;
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (getFirstUnitOrder((UnitTypes)iI) != -1)
		{
			UnitTypes eUpgradeUnit = allUpgradesAvailable((UnitTypes)iI);

			if (eUpgradeUnit != NO_UNIT)
			{
				FAssertMsg(eUpgradeUnit != iI, "eUpgradeUnit is expected to be different from iI");
				int iUpgradeProduction = getUnitProduction((UnitTypes)iI);
				setUnitProduction(((UnitTypes)iI), 0);
				setUnitProduction(eUpgradeUnit, iUpgradeProduction);

				CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

				while (pOrderNode != NULL)
				{
					if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
					{
						if (EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1) == iI)
						{
							GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)).getUnitClassType())), -1);
							pOrderNode->m_data.iData1 = PACK_INTERNAL_ORDER_IDATA(eUpgradeUnit, INTERNAL_AUXILIARY_ORDER_IDATA(pOrderNode->m_data.iData1));
							if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eUpgradeUnit, ((UnitAITypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2)), area()) == 0)
							{
								area()->changeNumTrainAIUnits(getOwnerINLINE(), ((UnitAITypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2)), -1);
								GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(((UnitAITypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2)), -1);
								pOrderNode->m_data.iData2 = GC.getUnitInfo(eUpgradeUnit).getDefaultUnitAIType();
								area()->changeNumTrainAIUnits(getOwnerINLINE(), ((UnitAITypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2)), 1);
								GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(((UnitAITypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData2)), 1);
							}
							GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes)EXTERNAL_ORDER_IDATA(pOrderNode->m_data.iData1)).getUnitClassType())), 1);
						}
					}

					pOrderNode = nextOrderQueueNode(pOrderNode);
				}
			}
		}
	}

	bool bOK = true;

	for (int iI = (getOrderQueueLength() - 1); iI >= 0; iI--)
	{
		OrderData* pOrder = getOrderFromQueue(iI);

		if (pOrder != NULL)
		{
			if (!canContinueProduction(*pOrder))
			{
				popOrder(iI, false, true);
				bOK = false;
			}
		}
	}

	return bOK;
}


void CvCity::doProduction(bool bAllowNoProduction)
{
	/************************************************************************************************/
	/* Afforess	                  Start		 12/21/09                                                */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	if (GC.getUSE_CAN_DO_PRODUCTION_CALLBACK())
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult = 0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doProduction", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return;
		}
	}
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/
	if (!isHuman() || isProductionAutomated())
	{
		//	Koshling - with the unit contracting system we only build units to contractual
		//	orders (apart from a few emergency cases) and we should not change from building
		//	them due to new techs etc
		if (!isProduction() || isProductionProcess() || (AI_isChooseProductionDirty() && !isProductionUnit()))
		{
			AI_chooseProduction();
		}
	}

	if (!bAllowNoProduction && !isProduction())
	{
		return;
	}

	if (isProductionProcess())
	{
		/************************************************************************************************/
		/* UNOFFICIAL_PATCH                       12/07/09                         denev & jdog5000     */
		/*                                                                                              */
		/* Bugfix, Odd behavior                                                                         */
		/************************************************************************************************/
		if (m_bPopProductionProcess)
		{
			popOrder(0, false, true);
			m_bPopProductionProcess = false;
		}
		/************************************************************************************************/
		/* UNOFFICIAL_PATCH                        END                                                  */
		/************************************************************************************************/
		return;
	}

	if (isDisorder())
	{
		return;
	}

	if (isProduction())
	{
		changeProduction(getCurrentProductionDifference(false, true));
		setOverflowProduction(0);
		setFeatureProduction(0);
		/************************************************************************************************/
		/* Afforess	Multiple Production Mod		 08/23/09                                            */
		/*                                                                                              */
		/*                                                                                              */
		/************************************************************************************************/
		setBuiltFoodProducedUnit(isFoodProduction());
		clearLostProduction();

		int iOverflowProductionModified = 0;
		while (isProduction() && productionLeft() <= iOverflowProductionModified)
		{
			changeProduction(iOverflowProductionModified);
			setOverflowProduction(0);

			popOrder(0, true, true);

			if (!isHuman() && !isProduction())
			{
				AI_chooseProduction();
			}

			//to eliminate pre-build exploits for all Wonders and all Projects
			if (isProductionWonder() || isProductionProject())
			{
				break;
			}

			//to eliminate pre-build exploits for Settlers and Workers
			if (isFoodProduction() && !isBuiltFoodProducedUnit())
			{
				break;
			}

			if (isProductionProcess())
			{
				break;
			}

			//fix production which floods from overflow capacity to next queue item if it exists
			if (isProduction() && m_iLostProductionBase > 0)
			{
				changeProduction(getExtraProductionDifference(m_iLostProductionBase));
				clearLostProduction();
			}

			iOverflowProductionModified = getOverflowProductionDifference();
		}

		if (m_iGoldFromLostProduction > 0)
		{
			MEMORY_TRACK_EXEMPT();

			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_PROD_CONVERTED", getNameKey(), m_iLostProductionModified, m_iGoldFromLostProduction);
			AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);

			GET_PLAYER(getOwnerINLINE()).changeGold(m_iGoldFromLostProduction);
			clearLostProduction();
		}
		/************************************************************************************************/
		/* Afforess	Multiple Production Mod       END                                                */
		/************************************************************************************************/
	}
	else
	{
		changeOverflowProduction(getCurrentProductionDifference(false, false), getProductionModifier());
	}
}


void CvCity::doDecay()
{
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		BuildingTypes eBuilding = (BuildingTypes) iI;
		if (getProductionBuilding() != eBuilding)
		{
			if (getBuildingProduction(eBuilding) > 0)
			{
				changeBuildingProductionTime(eBuilding, 1);

				if (isHuman())
				{
					int iGameSpeedPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
					if (100 * getBuildingProductionTime(eBuilding) > GC.getDefineINT("BUILDING_PRODUCTION_DECAY_TIME") * iGameSpeedPercent)
					{
						int iProduction = getBuildingProduction(eBuilding);
						int iAdaptedProduction = std::max(0, (iProduction - (iProduction * (100 - GC.getDefineINT("BUILDING_PRODUCTION_DECAY_PERCENT")) + iGameSpeedPercent - 1) / iGameSpeedPercent));
						setBuildingProduction(eBuilding, iAdaptedProduction);
					}
				}
			}
			else
			{
				setBuildingProductionTime(eBuilding, 0);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		UnitTypes eUnit = (UnitTypes) iI;
		if (getProductionUnit() != eUnit)
		{
			if (getUnitProduction(eUnit) > 0)
			{
				changeUnitProductionTime(eUnit, 1);

				if (isHuman())
				{
					int iGameSpeedPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
					if (100 * getUnitProductionTime(eUnit) > GC.getDefineINT("UNIT_PRODUCTION_DECAY_TIME") * iGameSpeedPercent)
					{
						int iProduction = getUnitProduction(eUnit);
						setUnitProduction(eUnit, iProduction - (iProduction * (100 - GC.getDefineINT("UNIT_PRODUCTION_DECAY_PERCENT")) + iGameSpeedPercent - 1) / iGameSpeedPercent);
					}
				}
			}
			else
			{
				setUnitProductionTime(eUnit, 0);
			}
		}
	}
}


void CvCity::doReligion()
{
/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_DO_RELIGION_CALLBACK())
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doReligion", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 06/09/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
	if (getReligionCount() == 0)
*/
	if (getReligionCount() == 0 || GC.getGameINLINE().isModderGameOption(MODDERGAMEOPTION_MULTIPLE_RELIGION_SPREAD))
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (!isHasReligion((ReligionTypes)iI))
			{
				if ((iI == GET_PLAYER(getOwnerINLINE()).getStateReligion()) || !(GET_PLAYER(getOwnerINLINE()).isNoNonStateReligionSpread()))
				{
					int iRandThreshold = 0;
	
					for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive())
						{
							int iLoop = 0;
							for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iJ).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iJ).nextCity(&iLoop))
							{
								if (pLoopCity->isConnectedTo(this))
								{
									int iSpread = pLoopCity->getReligionInfluence((ReligionTypes)iI);

									iSpread *= GC.getReligionInfo((ReligionTypes)iI).getSpreadFactor();
														
									if (iSpread > 0)
									{
										iSpread /= std::max(1, (((GC.getDefineINT("RELIGION_SPREAD_DISTANCE_DIVISOR") * plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE())) / GC.getMapINLINE().maxPlotDistance()) - 5));
/************************************************************************************************/
/* Afforess	                  Start		 06/09/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
										iSpread /= (getReligionCount() + 1);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
										iRandThreshold = std::max(iRandThreshold, iSpread);
									}
								}
							}
						}
					}
/************************************************************************************************/
/* Afforess	                  Start		 06/09/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
					int iSpreadMod = (GET_PLAYER(getOwnerINLINE()).getReligionSpreadRate()) + 100;
					if (iSpreadMod < -100)
					{
						iRandThreshold = 0;
					}
					iRandThreshold *= iSpreadMod;
					iRandThreshold /= 100;

					int iSpreadRand = GC.getDefineINT("RELIGION_SPREAD_RAND");
					iSpreadRand *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
					iSpreadRand /= 100;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
					if (GC.getGameINLINE().getSorenRandNum(iSpreadRand, "Religion Spread") < iRandThreshold)
					{
						setHasReligion(((ReligionTypes)iI), true, true, true);
						break;
					}
				}
			}
/************************************************************************************************/
/* Afforess	                  Start		 06/24/10                                               */
/*                                                                                              */
/* Religion Decay                                                                               */
/************************************************************************************************/
			else if (GC.getGameINLINE().isOption(GAMEOPTION_RELIGION_DECAY))
			{
				CvCity* pHolyCity = GC.getGameINLINE().getHolyCity((ReligionTypes)iI);
				if (iI != GET_PLAYER(getOwnerINLINE()).getStateReligion())
				{
					if (getReligionCount() > 2)
					{
						int iDecay = 0;
						if (pHolyCity != this)
						{
							iDecay = GC.getReligionInfo((ReligionTypes)iI).getSpreadFactor();
							//more decay for each religion above 3
							iDecay += 10 * (getReligionCount() - 3);

							int iLoop = 0;
							for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
							{
								if (pLoopCity->isConnectedTo(this))
								{
									if (pLoopCity->isHasReligion((ReligionTypes)iI))
									{
										iDecay *= 9;
										iDecay /= (10 + std::max(0, pLoopCity->getReligionInfluence((ReligionTypes)iI)));
									}
								}
							}

							iDecay /= std::max(1, 1 + getReligionInfluence((ReligionTypes)iI));
							if (pHolyCity != NULL)
							{
								if (pHolyCity->getOwnerINLINE() == getOwnerINLINE())
								{
									iDecay /= 2;
								}
								else if (GET_TEAM(getTeam()).isAtWar(pHolyCity->getTeam()))
								{
									iDecay *= 4;
									iDecay /= 3;
								}
							}
						}
						int iSpreadRand = GC.getDefineINT("RELIGION_SPREAD_RAND");
						iSpreadRand *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
						iSpreadRand /= 100;
						if (GC.getGameINLINE().getSorenRandNum(iSpreadRand, "Religion Decay") < iDecay)
						{
							setHasReligion(((ReligionTypes)iI), false, true, false);
							for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
							{
								if (getNumBuilding((BuildingTypes)iJ) > 0)
								{
									if (GC.getBuildingInfo((BuildingTypes)iJ).getPrereqReligion() == iI)
									{
										setNumRealBuilding((BuildingTypes)iJ, 0);
									}
								}
							}
							break;
						}
					}
				}
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		}
	}
}


void CvCity::doGreatPeople()
{
/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_DO_GREATPEOPLE_CALLBACK())
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doGreatPeople", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (isDisorder())
	{
		return;
	}

	changeGreatPeopleProgress(getGreatPeopleRate());

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		changeGreatPeopleUnitProgress(((UnitTypes)iI), getGreatPeopleUnitRate((UnitTypes)iI));
	}

	if (getGreatPeopleProgress() >= GET_PLAYER(getOwnerINLINE()).greatPeopleThreshold(false))
	{
		int iTotalGreatPeopleUnitProgress = 0;
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			iTotalGreatPeopleUnitProgress += getGreatPeopleUnitProgress((UnitTypes)iI);
		}

		int iGreatPeopleUnitRand = GC.getGameINLINE().getSorenRandNum(iTotalGreatPeopleUnitProgress, "Great Person");

		UnitTypes eGreatPeopleUnit = NO_UNIT;
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (iGreatPeopleUnitRand < getGreatPeopleUnitProgress((UnitTypes)iI))
			{
				eGreatPeopleUnit = ((UnitTypes)iI);
				break;
			}
			else
			{
				iGreatPeopleUnitRand -= getGreatPeopleUnitProgress((UnitTypes)iI);
			}
		}

		if (eGreatPeopleUnit != NO_UNIT)
		{
			changeGreatPeopleProgress(-(GET_PLAYER(getOwnerINLINE()).greatPeopleThreshold(false)));

			for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
			{
				setGreatPeopleUnitProgress(((UnitTypes)iI), 0);
			}

			createGreatPeople(eGreatPeopleUnit, true, false);
		}
	}
}


void CvCity::doMeltdown()
{
/************************************************************************************************/
/* Afforess	                  Start		 12/21/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_CAN_DO_MELTDOWN_CALLBACK())
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doMeltdown", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 
		if (lResult == 1)
		{
			return;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getNumBuilding((BuildingTypes)iI) > 0)
		{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                                                               jdog5000      */
/*                                                                                              */
/* Gamespeed scaling                                                                            */
/************************************************************************************************/
/* original bts code
			if (GC.getBuildingInfo((BuildingTypes)iI).getNukeExplosionRand() != 0)
			{
				if (GC.getGameINLINE().getSorenRandNum(GC.getBuildingInfo((BuildingTypes)iI).getNukeExplosionRand(), "Meltdown!!!") == 0)
				{
*/
			int iOdds = GC.getBuildingInfo((BuildingTypes)iI).getNukeExplosionRand();

			if( iOdds > 0 )
			{
				int iChance = 1000;

				if( GC.getGameINLINE().getSorenRandNum(iChance, "Meltdown!!!") < iOdds )
				{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
					if (getNumRealBuilding((BuildingTypes)iI) > 0)
					{
						setNumRealBuilding(((BuildingTypes)iI), 0);
					}

					plot()->nukeExplosion(1);

					{
						MEMORY_TRACK_EXEMPT();
						CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_MELTDOWN_CITY", getNameKey());
						AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MELTDOWN", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_UNHEALTHY_PERSON")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
					}
					break;
				}
			}
		}
	}
}

// Private Functions...

void CvCity::read(FDataStreamBase* pStream)
{
	MEMORY_TRACE_FUNCTION();

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init data before load
	reset();

	uint uiFlag=0;
	WRAPPER_READ(wrapper, "CvCity", &uiFlag);	// flags for expansion

	WRAPPER_READ(wrapper, "CvCity", &m_iID);
	WRAPPER_READ(wrapper, "CvCity", &m_iX);
	WRAPPER_READ(wrapper, "CvCity", &m_iY);
	WRAPPER_READ(wrapper, "CvCity", &m_iRallyX);
	WRAPPER_READ(wrapper, "CvCity", &m_iRallyY);
	WRAPPER_READ(wrapper, "CvCity", &m_iGameTurnFounded);
	WRAPPER_READ(wrapper, "CvCity", &m_iGameTurnAcquired);
	WRAPPER_READ(wrapper, "CvCity", &m_iPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iHighestPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iWorkingPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumGreatPeople);
	WRAPPER_READ(wrapper, "CvCity", &m_iBaseGreatPeopleRate);
	WRAPPER_READ(wrapper, "CvCity", &m_iGreatPeopleRateModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iGreatPeopleProgress);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumWorldWonders);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumTeamWonders);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumNationalWonders);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumBuildings);
	WRAPPER_READ(wrapper, "CvCity", &m_iGovernmentCenterCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iMaintenance);
	WRAPPER_READ(wrapper, "CvCity", &m_iMaintenanceModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iWarWearinessModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iHurryAngerModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iHealRate);
	WRAPPER_READ(wrapper, "CvCity", &m_iEspionageHealthCounter);
	WRAPPER_READ(wrapper, "CvCity", &m_iEspionageHappinessCounter);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreshWaterGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreshWaterBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iPowerGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iPowerBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iBonusGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iBonusBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iHurryAngerTimer);
/************************************************************************************************/
/* REVOLUTION_MOD                         04/28/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_READ(wrapper, "CvCity", &m_iRevRequestAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iRevSuccessTimer);
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	WRAPPER_READ(wrapper, "CvCity", &m_iConscriptAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iDefyResolutionAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iHappinessTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iMilitaryHappinessUnits);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iBonusGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iBonusBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iReligionGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iReligionBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iNoUnhappinessCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iNoUnhealthyPopulationCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingOnlyHealthyCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iFood);
	WRAPPER_READ(wrapper, "CvCity", &m_iFoodKept);

	//	Old format save before saved food chnages became multiplicative and the stored value
	//	became a logarithm
	m_fMaxFoodKeptMultiplierLog = INVALID_STORED_FOOD_PERCENT_LOG;

	WRAPPER_READ(wrapper, "CvCity", &m_fMaxFoodKeptMultiplierLog);

	WRAPPER_READ(wrapper, "CvCity", &m_iOverflowProduction);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureProduction);
	WRAPPER_READ(wrapper, "CvCity", &m_iMilitaryProductionModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpaceProductionModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTradeRoutes);
	WRAPPER_READ(wrapper, "CvCity", &m_iTradeRouteModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iForeignTradeRouteModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingBombardDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreeExperience);
	WRAPPER_READ(wrapper, "CvCity", &m_iCurrAirlift);
	WRAPPER_READ(wrapper, "CvCity", &m_iMaxAirlift);
	WRAPPER_READ(wrapper, "CvCity", &m_iAirModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iAirUnitCapacity);
	WRAPPER_READ(wrapper, "CvCity", &m_iNukeModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreeSpecialist);
	WRAPPER_READ(wrapper, "CvCity", &m_iPowerCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iDirtyPowerCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iDefenseDamage);
	WRAPPER_READ(wrapper, "CvCity", &m_iLastDefenseDamage);
	WRAPPER_READ(wrapper, "CvCity", &m_iOccupationTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iCultureUpdateTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iCitySizeBoost);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistFreeExperience);
	WRAPPER_READ(wrapper, "CvCity", &m_iEspionageDefenseModifier);
	// < M.A.D. Nukes Start >
	WRAPPER_READ(wrapper, "CvCity", &m_iMADIncoming);
	// < M.A.D. Nukes End   >


	WRAPPER_READ(wrapper, "CvCity", &m_bNeverLost);
	WRAPPER_READ(wrapper, "CvCity", &m_bBombarded);
	WRAPPER_READ(wrapper, "CvCity", &m_bDrafted);
	WRAPPER_READ(wrapper, "CvCity", &m_bAirliftTargeted);
	WRAPPER_READ(wrapper, "CvCity", &m_bWeLoveTheKingDay);
	WRAPPER_READ(wrapper, "CvCity", &m_bCitizensAutomated);
	WRAPPER_READ(wrapper, "CvCity", &m_bProductionAutomated);
	WRAPPER_READ(wrapper, "CvCity", &m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...
	WRAPPER_READ(wrapper, "CvCity", &m_bPlundered);

	WRAPPER_READ(wrapper, "CvCity", (int*)&m_eOwner);
	WRAPPER_READ(wrapper, "CvCity", (int*)&m_ePreviousOwner);
	WRAPPER_READ(wrapper, "CvCity", (int*)&m_eOriginalOwner);
	WRAPPER_READ(wrapper, "CvCity", (int*)&m_eCultureLevel);
	if ( m_eCultureLevel >= GC.getNumCultureLevelInfos() )
	{
		m_eCultureLevel = (CultureLevelTypes)(GC.getNumCultureLevelInfos() - 1);
	}

/************************************************************************************************/
/* REVOLUTION_MOD                         06/10/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_READ(wrapper, "CvCity", &m_iRevolutionIndex);
	WRAPPER_READ(wrapper, "CvCity", &m_iLocalRevIndex);
	WRAPPER_READ(wrapper, "CvCity", &m_iRevIndexAverage);
	WRAPPER_READ(wrapper, "CvCity", &m_iRevolutionCounter);
	WRAPPER_READ(wrapper, "CvCity", &m_iReinforcementCounter);
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiSeaPlotYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiRiverPlotYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRate);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiPowerYieldRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBonusYieldRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiTradeYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiCorporationYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiExtraSpecialistYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRate);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiProductionToCommerceModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBuildingCommerce);
	
	//	Old style was in units of gold - if we still have that load it and multiply it up...
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiSpecialistCommerce);
	for(int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiSpecialistCommerce[iI] *= 100;
	}
	//	...but new style is in 100ths to avoid rounding issues, so overwrite with that if present
	WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiSpecialistCommerce, "m_aiSpecialistCommerceTimes100");
	
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiReligionCommerce);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCorporationCommerce);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceHappinessPer);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiCulture);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiNumRevolts);

	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abEverOwned);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abTradeRoute);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abRevealed);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abEspionageVisibility);

	WRAPPER_READ_STRING(wrapper, "CvCity", m_szName);
	WRAPPER_READ_STRING(wrapper, "CvCity", m_szScriptData);

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNoBonus);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiFreeBonus);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumCorpProducedBonuses);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectProduction);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingProduction);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingProductionTime);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingOriginalOwner);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingOriginalTime);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitProduction);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitProductionTime);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitRate);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitProgress);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiMaxSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiForceSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCountUnattributed);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementFreeSpecialists);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiReligionInfluence);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiStateReligionHappiness);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatFreeExperience);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONS, GC.getNumPromotionInfos(), m_paiFreePromotionCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumRealBuilding);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumFreeBuilding);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumFreeAreaBuilding);

	// @SAVEBREAK DELETE 8/7/2018
	// Delete this section at the next save break.
	// Legacy for FreeTradeRegionBuilding.
	{
		// It has to be called m_paiNumFreeTradeRegionBuilding or the read code macro doesn't work!
		int* m_paiNumFreeTradeRegionBuilding = new int[GC.getNumBuildingInfos()];
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumFreeTradeRegionBuilding);
		SAFE_DELETE_ARRAY(m_paiNumFreeTradeRegionBuilding);
	}
	// SAVEBREAK@

	WRAPPER_READ(wrapper, "CvCity", &m_bHasCalculatedBuildingReplacement);

	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_CITY_PLOTS, m_pabWorkingPlot);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_pabHasReligion);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_pabHasCorporation);
	
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_READ(wrapper, "CvCity", &m_iImprovementGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iImprovementBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistUnhappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iEventAnger);

	//	Must recalculate from first principles if loading an old format save
	m_fPopulationgrowthratepercentageLog = INVALID_GROWTH_PERCENT_LOG;

	WRAPPER_READ(wrapper, "CvCity", &m_fPopulationgrowthratepercentageLog);
	
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommerceRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommercePercentChanges);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceAttacks);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiMaxCommerceAttacks);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNIT_CLASSES, GC.getNumUnitClassInfos(), m_paiUnitClassProductionModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, GC.getNumBuildingClassInfos(), m_paiBuildingClassProductionModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiBonusDefenseChanges);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadVicinityBonus);
	WRAPPER_READ_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CIVILIZATIONS, &m_iCiv);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraYieldTurns);
	WRAPPER_READ(wrapper, "CvCity", (int*)&m_eOccupationCultureLevel);
	WRAPPER_READ(wrapper, "CvCity", &m_iLineOfSight);
	WRAPPER_READ(wrapper, "CvCity", &m_iLandmarkAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iInvasionChance);
	WRAPPER_READ(wrapper, "CvCity", &m_iInvasionTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreshWater);

	//	Cope with older saves
	if ( wrapper.isUsingTaggedFormat() )
	{
		bool bFreshWater = false;
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &bFreshWater, "m_bFreshWater");

		if ( 0 == m_iFreshWater && bFreshWater )
		{
			m_iFreshWater = 1;
		}
	}

	WRAPPER_READ(wrapper, "CvCity", &m_iAdjacentDamagePercent);
	WRAPPER_READ(wrapper, "CvCity", &m_iWorkableRadiusOverride);
	WRAPPER_READ(wrapper, "CvCity", &m_iProtectedCultureCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumUnitFullHeal);
	WRAPPER_READ(wrapper, "CvCity", &m_iDisabledPowerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iWarWearinessTimer);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_pabDisabledBuilding);
	
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatExtraStrength);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_pabAutomatedCanBuild);

	WRAPPER_READ(wrapper, "CvCity", &m_iMinimumDefenseLevel);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumPopulationEmployed);
	WRAPPER_READ(wrapper, "CvCity", &m_iHappinessPercentPerPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iHealthPercentPerPopulation);
		
	unsigned int iNumElts = 0;
	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingCommerceModifier.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		BuildingCommerceModifier kChange;
		kChange.read(pStream);
		m_aBuildingCommerceModifier.push_back(kChange);
	}
	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingYieldModifier.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		BuildingYieldModifier kChange;
		kChange.read(pStream);
		m_aBuildingYieldModifier.push_back(kChange);
	}
/*	for (iI=0;iI<GC.getNumImprovementInfos();iI++)
	{
		WRAPPER_READ(wrapper, "CvCity", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

	int iMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();
	WRAPPER_READ(wrapper, "CvCity", &iMaxTradeRoutes);
	int iAllowedMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();// resetting again after first read resets

	int tradeRouteIdx = 0;
	for ( ; tradeRouteIdx < iMaxTradeRoutes; ++tradeRouteIdx)
	{
		if ( tradeRouteIdx < iAllowedMaxTradeRoutes )
		{
			WRAPPER_READ(wrapper, "CvCity", (int*)&m_paTradeCities[tradeRouteIdx].eOwner);
			WRAPPER_READ(wrapper, "CvCity", &m_paTradeCities[tradeRouteIdx].iID);
		}
		else
		{
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paTradeCities[tradeRouteIdx].eOwner, SAVE_VALUE_TYPE_INT);
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paTradeCities[tradeRouteIdx].iID, SAVE_VALUE_TYPE_INT);
		}
	}

	iMaxTradeRoutes = iAllowedMaxTradeRoutes;

	for ( ; tradeRouteIdx < iMaxTradeRoutes; ++tradeRouteIdx)
	{
		m_paTradeCities[tradeRouteIdx].eOwner = NO_PLAYER;
		m_paTradeCities[tradeRouteIdx].iID = -1;
	}

	bool bOrdersHaveContractInfo = false;
	WRAPPER_READ(wrapper, "CvCity", &bOrdersHaveContractInfo);

	m_orderQueue.Read(pStream);

	//	The order queue itself is not a streamable type so is serialized in raw
	//	binary image, which means we need to do some explicit tranlsation on load
	//	if we are using the tagged format
	if ( wrapper.isUsingTaggedFormat() )
	{
		CLLNode<OrderData>* pNode = headOrderQueueNode();
		while (pNode != NULL)
		{
			bool bDeleteNode = false;

			switch(pNode->m_data.eOrderType)
			{
			case ORDER_TRAIN:
				{
					int eUnit = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, EXTERNAL_ORDER_IDATA(pNode->m_data.iData1), true);

					bDeleteNode = (eUnit == -1);
					pNode->m_data.iData1 = PACK_INTERNAL_ORDER_IDATA(eUnit,
																	 bOrdersHaveContractInfo ? INTERNAL_AUXILIARY_ORDER_IDATA(pNode->m_data.iData1) : 0xFFFF);
				}
				break;
			case ORDER_CONSTRUCT:
				bDeleteNode = ((pNode->m_data.iData1 = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS,pNode->m_data.iData1, true)) == -1);
				break;
			case ORDER_CREATE:
				bDeleteNode = ((pNode->m_data.iData1 = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_PROJECTS,pNode->m_data.iData1, true)) == -1);
				break;
			case ORDER_MAINTAIN:
			case ORDER_LIST:
				break;
			default:
				break;
			}

			if ( bDeleteNode )
			{
				pNode = m_orderQueue.deleteNode(pNode);
			}
			else
			{
				pNode = nextOrderQueueNode(pNode);
			}
		}
	}

	WRAPPER_READ(wrapper, "CvCity", &m_iPopulationRank);
	WRAPPER_READ(wrapper, "CvCity", &m_bPopulationRankValid);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRank);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abBaseYieldRankValid);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRank);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abYieldRankValid);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRank);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_abCommerceRankValid);

	m_Properties.readWrapper(pStream);

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aEventsOccured.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		EventTypes eEvent = NO_EVENT;
		WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvCity", REMAPPED_CLASS_TYPE_EVENTS, (int*)&eEvent);

		if ( eEvent == NO_EVENT )
		{
			//	Old format so go for a raw read as the best we can do
			WRAPPER_READ(wrapper, "CvCity", (int*)&eEvent);
		}
		if ( eEvent != NO_EVENT )
		{
			m_aEventsOccured.push_back(eEvent);
		}
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingYieldChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		BuildingYieldChange kChange;
		kChange.read(pStream);
		m_aBuildingYieldChange.push_back(kChange);
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingCommerceChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		BuildingCommerceChange kChange;
		kChange.read(pStream);
		m_aBuildingCommerceChange.push_back(kChange);
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingHappyChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		int iBuildingClass;
		WRAPPER_READ_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, &iBuildingClass);
		int iChange;
		WRAPPER_READ(wrapper, "CvCity", &iChange);
		m_aBuildingHappyChange.push_back(std::make_pair((BuildingClassTypes)iBuildingClass, iChange));
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingHealthChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		int iBuildingClass;
		WRAPPER_READ_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, &iBuildingClass);
		int iChange;
		WRAPPER_READ(wrapper, "CvCity", &iChange);
		m_aBuildingHealthChange.push_back(std::make_pair((BuildingClassTypes)iBuildingClass, iChange));
	}

	//	Now the owner has been restored from the save set the info on the building list
	m_BuildingList.setPlayerToOwner();
	m_UnitList.setPlayerToOwner();

/************************************************************************************************/
/* phunny_pharmer             Start		 05/01/10                                               */
/*   clear the culture distance cache (note that it is not saved in the .sav file)              */
/************************************************************************************************/
	clearCultureDistanceCache();
/************************************************************************************************/
/* phunny_pharmer             END                                                               */
/************************************************************************************************/
	//TB Combat Mod (Buildings) begin
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_iAidRate, SAVE_VALUE_ANY);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paiExtraBonusAidModifier, SAVE_VALUE_ANY);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatProductionModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalFrontSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalShortRangeSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalMediumRangeSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalLongRangeSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalFlankSupportPercentModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatOngoingTrainingTimeCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatOngoingTrainingTimeIncrement);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paiNewDiseaseTypeCount, SAVE_VALUE_ANY);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paiNewExtraAfflictionOutbreakLevelModifier, SAVE_VALUE_ANY);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paiNewAfflictionToleranceModifier, SAVE_VALUE_ANY);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paiNewCurrentOvercomeModifier, SAVE_VALUE_ANY);
//Team Project (1)
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_TECHS); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TECHS, i, true);

		if ( iI != -1 )
		{
			WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppaaiTechSpecialistHappinessTypes[iI]);
		}
		else
		{
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiTechSpecialistHappinessTypes[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_TECHS); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TECHS, i, true);

		if ( iI != -1 )
		{
			WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppaaiTechSpecialistHealthTypes[iI]);
		}
		else
		{
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiTechSpecialistHealthTypes[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechHappiness);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechSpecialistHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTechSpecialistHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTechHappiness);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechHealth);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechSpecialistHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTechSpecialistHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTechHealth);
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_SPECIALISTS); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_SPECIALISTS, i, true);

		if ( iI != -1 )
		{
			WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_ppaaiLocalSpecialistExtraYield[iI]);
		}
		else
		{
			//	Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiLocalSpecialistExtraYield[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}
//Team Project (3)
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalCaptureProbabilityModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalCaptureResistanceModifier);
//Team Project (5)
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_pabReligiouslyDisabledBuilding);
	WRAPPER_READ(wrapper, "CvCity", &m_iPrioritySpecialist);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistBannedCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalDynamicDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraRiverDefensePenalty);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalRepel);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraMinDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingDefenseRecoverySpeedModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iModifiedBuildingDefenseRecoverySpeedCap);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraCityDefenseRecoverySpeedModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelAgainstModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatDefenseAgainstModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiDamageAttackingUnitCombatCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiHealUnitCombatTypeVolume);
	//TB Combat Mod (Buildings) end
	
	WRAPPER_READ(wrapper, "CvCity", &m_iZoCCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraInsidiousness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraInvestigation);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistInsidiousness);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistInvestigation);
	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aPropertySpawns.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		PropertySpawns kChange;
		kChange.read(pStream);
		m_aPropertySpawns.push_back(kChange);
	}
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiExtraSpecialistCommerce);
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_SPECIALISTS); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_SPECIALISTS, i, true);

		if ( iI != -1 )
		{
			WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_ppaaiLocalSpecialistExtraCommerce[iI]);
		}
		else
		{
			//	Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiLocalSpecialistExtraCommerce[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldPerPopRate);
	WRAPPER_READ(wrapper, "CvCity", &m_bVisibilitySetup);
	m_bVisibilitySetup = false;
	WRAPPER_READ(wrapper, "CvCity", &m_iLostProductionBase);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionTypeCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewExtraAfflictionOutbreakLevelChange);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionToleranceChange);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewCurrentOvercomeChange);
//Team Project (1)
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_BONUSES); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, i, true);

		if ( iI != -1 )
		{
			WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_ppaaiExtraBonusAidModifier[iI]);
		}
		else
		{
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiExtraBonusAidModifier[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_paiAidRate);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCostPopulationCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiPromotionLineAfflictionAttackCommunicability);
	WRAPPER_READ(wrapper, "CvCity", &m_iQuarantinedCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadRawVicinityBonus);
	WRAPPER_READ(wrapper, "CvCity", &m_bPropertyControlBuildingQueued);
	WRAPPER_READ_OBJECT_END(wrapper);
	//Example of how to skip an unneeded element
	//WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_iMaxFoodKeptPercent, SAVE_VALUE_ANY);	// was present in old formats
}

void CvCity::write(FDataStreamBase* pStream)
{
	uint uiFlag=0;

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvCity", uiFlag);		// flag for expansion

	WRAPPER_WRITE(wrapper, "CvCity", m_iID);
	WRAPPER_WRITE(wrapper, "CvCity", m_iX);
	WRAPPER_WRITE(wrapper, "CvCity", m_iY);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRallyX);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRallyY);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGameTurnFounded);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGameTurnAcquired);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHighestPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWorkingPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumGreatPeople);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBaseGreatPeopleRate);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGreatPeopleRateModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGreatPeopleProgress);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumWorldWonders);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumTeamWonders);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumNationalWonders);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumBuildings);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGovernmentCenterCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMaintenance);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMaintenanceModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWarWearinessModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHurryAngerModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHealRate);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEspionageHealthCounter);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEspionageHappinessCounter);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreshWaterGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreshWaterBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPowerGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPowerBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHurryAngerTimer);
/************************************************************************************************/
/* REVOLUTION_MOD                         04/28/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevRequestAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevSuccessTimer);
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvCity", m_iConscriptAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iDefyResolutionAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHappinessTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMilitaryHappinessUnits);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iReligionGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iReligionBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNoUnhappinessCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNoUnhealthyPopulationCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingOnlyHealthyCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFood);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFoodKept);
	WRAPPER_WRITE(wrapper, "CvCity", m_fMaxFoodKeptMultiplierLog);
	WRAPPER_WRITE(wrapper, "CvCity", m_iOverflowProduction);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureProduction);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMilitaryProductionModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpaceProductionModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTradeRoutes);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTradeRouteModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iForeignTradeRouteModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingBombardDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreeExperience);
	WRAPPER_WRITE(wrapper, "CvCity", m_iCurrAirlift);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMaxAirlift);
	WRAPPER_WRITE(wrapper, "CvCity", m_iAirModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iAirUnitCapacity);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNukeModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreeSpecialist);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPowerCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iDirtyPowerCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iDefenseDamage);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLastDefenseDamage);
	WRAPPER_WRITE(wrapper, "CvCity", m_iOccupationTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iCultureUpdateTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iCitySizeBoost);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistFreeExperience);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEspionageDefenseModifier);
	// < M.A.D. Nukes Start >
	WRAPPER_WRITE(wrapper, "CvCity", m_iMADIncoming);
	// < M.A.D. Nukes End   >


	WRAPPER_WRITE(wrapper, "CvCity", m_bNeverLost);
	WRAPPER_WRITE(wrapper, "CvCity", m_bBombarded);
	WRAPPER_WRITE(wrapper, "CvCity", m_bDrafted);
	WRAPPER_WRITE(wrapper, "CvCity", m_bAirliftTargeted);
	WRAPPER_WRITE(wrapper, "CvCity", m_bWeLoveTheKingDay);
	WRAPPER_WRITE(wrapper, "CvCity", m_bCitizensAutomated);
	WRAPPER_WRITE(wrapper, "CvCity", m_bProductionAutomated);
	WRAPPER_WRITE(wrapper, "CvCity", m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...
	WRAPPER_WRITE(wrapper, "CvCity", m_bPlundered);

	WRAPPER_WRITE(wrapper, "CvCity", m_eOwner);
	WRAPPER_WRITE(wrapper, "CvCity", m_ePreviousOwner);
	WRAPPER_WRITE(wrapper, "CvCity", m_eOriginalOwner);
	WRAPPER_WRITE(wrapper, "CvCity", m_eCultureLevel);

/************************************************************************************************/
/* REVOLUTION_MOD                         06/10/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevolutionIndex);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLocalRevIndex);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevIndexAverage);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevolutionCounter);
	WRAPPER_WRITE(wrapper, "CvCity", m_iReinforcementCounter);
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiSeaPlotYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiRiverPlotYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRate);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiPowerYieldRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBonusYieldRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiTradeYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiCorporationYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiExtraSpecialistYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRate);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiProductionToCommerceModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBuildingCommerce);
	WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiSpecialistCommerce, "m_aiSpecialistCommerceTimes100");
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiReligionCommerce);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCorporationCommerce);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceHappinessPer);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiCulture);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiNumRevolts);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abEverOwned);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abTradeRoute);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abRevealed);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abEspionageVisibility);

	WRAPPER_WRITE_STRING(wrapper, "CvCity", m_szName);
	WRAPPER_WRITE_STRING(wrapper, "CvCity", m_szScriptData);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNoBonus);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiFreeBonus);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumCorpProducedBonuses);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectProduction);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingProduction);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingProductionTime);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingOriginalOwner);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingOriginalTime);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitProduction);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitProductionTime);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitRate);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitProgress);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiMaxSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiForceSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCountUnattributed);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementFreeSpecialists);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiReligionInfluence);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiStateReligionHappiness);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatFreeExperience);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONS, GC.getNumPromotionInfos(), m_paiFreePromotionCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumRealBuilding);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumFreeBuilding);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumFreeAreaBuilding);

	// @SAVEBREAK DELETE 8/7/2018
	// Delete this section at the next save break.
	// Legacy for FreeTradeRegionBuilding.
	// This could be replaced with the following instead if it worked (and the write part could be deleted):
	// WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_paiNumFreeTradeRegionBuilding, SAVE_VALUE_TYPE_INT_ARRAY);
	{
		// It has to be called m_paiNumFreeTradeRegionBuilding or the read code macro doesn't work!
		int* m_paiNumFreeTradeRegionBuilding = new int[GC.getNumBuildingInfos()];
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiNumFreeTradeRegionBuilding);
		SAFE_DELETE_ARRAY(m_paiNumFreeTradeRegionBuilding);
	}
	// SAVEBREAK@

	WRAPPER_WRITE(wrapper, "CvCity", m_bHasCalculatedBuildingReplacement);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_CITY_PLOTS, m_pabWorkingPlot);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_pabHasReligion);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_pabHasCorporation);
	
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvCity", m_iImprovementGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iImprovementBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistUnhappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEventAnger);
	WRAPPER_WRITE(wrapper, "CvCity", m_fPopulationgrowthratepercentageLog);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommerceRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommercePercentChanges);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceAttacks);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiMaxCommerceAttacks);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNIT_CLASSES, GC.getNumUnitClassInfos(), m_paiUnitClassProductionModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, GC.getNumBuildingClassInfos(), m_paiBuildingClassProductionModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiBonusDefenseChanges);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadVicinityBonus);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CIVILIZATIONS, m_iCiv);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraYieldTurns);
	WRAPPER_WRITE(wrapper, "CvCity", m_eOccupationCultureLevel);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLineOfSight);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLandmarkAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iInvasionChance);
	WRAPPER_WRITE(wrapper, "CvCity", m_iInvasionTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreshWater);
	WRAPPER_WRITE(wrapper, "CvCity", m_iAdjacentDamagePercent);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWorkableRadiusOverride);
	WRAPPER_WRITE(wrapper, "CvCity", m_iProtectedCultureCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumUnitFullHeal);
	WRAPPER_WRITE(wrapper, "CvCity", m_iDisabledPowerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWarWearinessTimer);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_pabDisabledBuilding);
	
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatExtraStrength);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_pabAutomatedCanBuild);
	
	WRAPPER_WRITE(wrapper, "CvCity", m_iMinimumDefenseLevel);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumPopulationEmployed);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHappinessPercentPerPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHealthPercentPerPopulation);
	
	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingCommerceModifier.size(), "iNumElts");
	for (std::vector<BuildingCommerceModifier>::iterator it = m_aBuildingCommerceModifier.begin(); it != m_aBuildingCommerceModifier.end(); ++it)
	{
		(*it).write(pStream);
	}
	
	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingYieldModifier.size(), "iNumElts");
	for (std::vector<BuildingYieldModifier>::iterator it = m_aBuildingYieldModifier.begin(); it != m_aBuildingYieldModifier.end(); ++it)
	{
		(*it).write(pStream);
	}
/*	for (iI=0;iI<GC.getNumImprovementInfos();iI++)
	{
		WRAPPER_WRITE(wrapper, "CvCity", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	int iMaxTradeRoutes =  GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment() + GC.getDefineINT("MAX_TRADE_ROUTES");
	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", iMaxTradeRoutes, "iMaxTradeRoutes");
	for (int iI = 0; iI < iMaxTradeRoutes; iI++)
	{
		WRAPPER_WRITE(wrapper, "CvCity", m_paTradeCities[iI].eOwner);
		WRAPPER_WRITE(wrapper, "CvCity", m_paTradeCities[iI].iID);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", true, "bOrdersHaveContractInfo");

	m_orderQueue.Write(pStream);

	WRAPPER_WRITE(wrapper, "CvCity", m_iPopulationRank);
	WRAPPER_WRITE(wrapper, "CvCity", m_bPopulationRankValid);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abBaseYieldRankValid);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abYieldRankValid);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_abCommerceRankValid);

	m_Properties.writeWrapper(pStream);

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aEventsOccured.size(), "iNumElts");
	for (std::vector<EventTypes>::iterator it = m_aEventsOccured.begin(); it != m_aEventsOccured.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvCity", REMAPPED_CLASS_TYPE_EVENTS, *it, "eEvent");
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingYieldChange.size(), "iNumElts");
	for (std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		(*it).write(pStream);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingCommerceChange.size(), "iNumElts");
	for (std::vector<BuildingCommerceChange>::iterator it = m_aBuildingCommerceChange.begin(); it != m_aBuildingCommerceChange.end(); ++it)
	{
		(*it).write(pStream);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingHappyChange.size(), "iNumElts");
	for (BuildingChangeArray::iterator it = m_aBuildingHappyChange.begin(); it != m_aBuildingHappyChange.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, (*it).first, "iBuildingClass");
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "iChange");
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingHealthChange.size(), "iNumElts");
	for (BuildingChangeArray::iterator it = m_aBuildingHealthChange.begin(); it != m_aBuildingHealthChange.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDING_CLASSES, (*it).first, "iBuildingClass");
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "iChange");
	}
	//TB Combat Mod (Buildings) begin
	WRAPPER_WRITE(wrapper, "CvCity", m_iAidRate);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatProductionModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalFrontSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalShortRangeSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalMediumRangeSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalLongRangeSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalFlankSupportPercentModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatOngoingTrainingTimeCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatOngoingTrainingTimeIncrement);
//Team Project (1)
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppaaiTechSpecialistHappinessTypes[iI]);
	}
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppaaiTechSpecialistHealthTypes[iI]);
	}
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechHappiness);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechSpecialistHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTechSpecialistHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTechHappiness);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechHealth);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechSpecialistHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTechSpecialistHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTechHealth);
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_ppaaiLocalSpecialistExtraYield[iI]);
	}
//Team Project (3)
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalCaptureProbabilityModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalCaptureResistanceModifier);
//Team Project (5)
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_pabReligiouslyDisabledBuilding);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPrioritySpecialist);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistBannedCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalDynamicDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraRiverDefensePenalty);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalRepel);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraMinDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingDefenseRecoverySpeedModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iModifiedBuildingDefenseRecoverySpeedCap);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraCityDefenseRecoverySpeedModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelAgainstModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatDefenseAgainstModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiDamageAttackingUnitCombatCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiHealUnitCombatTypeVolume);
	//TB Combat Mod (Buildings) end

	WRAPPER_WRITE(wrapper, "CvCity", m_iZoCCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraInsidiousness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraInvestigation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistInsidiousness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistInvestigation);
	
	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aPropertySpawns.size(), "iNumElts");
	for (std::vector<PropertySpawns>::iterator it = m_aPropertySpawns.begin(); it != m_aPropertySpawns.end(); ++it)
	{
		(*it).write(pStream);
	}
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiExtraSpecialistCommerce);
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_ppaaiLocalSpecialistExtraCommerce[iI]);
	}
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldPerPopRate);
	WRAPPER_WRITE(wrapper, "CvCity", m_bVisibilitySetup);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLostProductionBase);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionTypeCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewExtraAfflictionOutbreakLevelChange);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionToleranceChange);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewCurrentOvercomeChange);

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_ppaaiExtraBonusAidModifier[iI]);
	}

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_paiAidRate);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCostPopulationCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiPromotionLineAfflictionAttackCommunicability);
	WRAPPER_WRITE(wrapper, "CvCity", m_iQuarantinedCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadRawVicinityBonus);
	WRAPPER_WRITE(wrapper, "CvCity", m_bPropertyControlBuildingQueued);
	WRAPPER_WRITE_OBJECT_END(wrapper);
}


//------------------------------------------------------------------------------------------------
class VisibleBuildingComparator
{
public:
	bool operator() (BuildingTypes e1, BuildingTypes e2)
	{
		if(GC.getBuildingInfo(e1).getVisibilityPriority() > GC.getBuildingInfo(e2).getVisibilityPriority())
			return true;
		else if(GC.getBuildingInfo(e1).getVisibilityPriority() == GC.getBuildingInfo(e2).getVisibilityPriority())
		{
			//break ties by building type higher building type
			if(e1 > e2)
				return true;
		}

		return false;
	}
};

//	Flags to determine which building types are displayed
#define	SHOW_BUILDINGS_WONDERS	1
#define	SHOW_BUILDINGS_DEFENSES	2
#define	SHOW_BUILDINGS_OTHER	128

void CvCity::getVisibleBuildings(std::list<BuildingTypes>& kChosenVisible, int& iChosenNumGenerics)
{

	if ( !plot()->shouldHaveFullGraphics() )
	{
		iChosenNumGenerics = 0;
		return;
	}

	int iShowFlags = GC.getDefineINT("SHOW_BUILDINGS_LEVEL");
	std::vector<BuildingTypes> kVisible;
	int iNumBuildings = GC.getNumBuildingInfos();
	for(int i = 0; i < iNumBuildings; i++)
	{
		BuildingTypes eCurType = (BuildingTypes) i;
		if(getNumBuilding(eCurType) > 0)
		{
			bool	bValid = false;
			CvBuildingInfo& kBuilding = GC.getBuildingInfo(eCurType);

			if (kBuilding.getNotShowInCity()) continue;

			bool	bIsWonder = isLimitedWonderClass((BuildingClassTypes)kBuilding.getBuildingClassType());
			bool	bIsDefense = (kBuilding.getDefenseModifier() > 0);

			if ( (iShowFlags & SHOW_BUILDINGS_WONDERS) != 0 )
			{
				//	Wonders
				bValid |= bIsWonder;
			}
			if ( (iShowFlags & SHOW_BUILDINGS_DEFENSES) != 0 )
			{
				//	Wonders
				bValid |= bIsDefense;
			}
			if ( (iShowFlags & SHOW_BUILDINGS_OTHER) != 0 )
			{
				//	Wonders
				bValid |= (!bIsWonder && !bIsDefense);
			}

			if ( bValid )
			{
				kVisible.push_back(eCurType);
			}
		}
	}

	// sort the visible ones by decreasing priority
	std::sort(kVisible.begin(), kVisible.end(), VisibleBuildingComparator());

	// how big is this city, in terms of buildings?
	// general rule: no more than fPercentUnique percent of a city can be uniques
	int iTotalVisibleBuildings = 0;
	if(stricmp(GC.getDefineSTRING("GAME_CITY_SIZE_METHOD"), "METHOD_EXPONENTIAL") == 0) 
	{
		int iCityScaleMod =  ((int)(pow((float)getPopulation(), GC.getDefineFLOAT("GAME_CITY_SIZE_EXP_MODIFIER")))) * 2;
		iTotalVisibleBuildings = (10 + iCityScaleMod);
	} 
	else 
	{
		float fLo = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_0");
		float fHi = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_50");
		float fCurSize = (float)getPopulation();
		iTotalVisibleBuildings = int(((fHi - fLo) / 50.0f) * fCurSize + fLo);
	}
	float fMaxUniquePercent = GC.getDefineFLOAT("GAME_CITY_SIZE_MAX_PERCENT_UNIQUE");
	int iMaxNumUniques = (int)(fMaxUniquePercent * iTotalVisibleBuildings);

	// compute how many buildings are generics vs. unique Civ buildings?
	int iNumGenerics;
	int iNumUniques;
	if((int)kVisible.size() > iMaxNumUniques)
	{
		iNumUniques = iMaxNumUniques;
	}
	else 
	{
		iNumUniques = kVisible.size();
	}
	iNumGenerics = iTotalVisibleBuildings - iNumUniques + getCitySizeBoost();
	
	// return
	iChosenNumGenerics = iNumGenerics;
	for(int i = 0; i < iNumUniques; i++)
	{
		kChosenVisible.push_back(kVisible[i]);
	}
}

static int natGetDeterministicRandom(int iMin, int iMax, int iSeedX, int iSeedY)
{
	srand(7297 * iSeedX + 2909  * iSeedY);
	return (rand() % (iMax - iMin)) + iMin;
}

void CvCity::getVisibleEffects(ZoomLevelTypes eCurZoom, std::vector<const TCHAR*>& kEffectNames) 
{
	if (isOccupation() && isVisible(getTeam(), false) == true)
	{
		if (eCurZoom  == ZOOM_DETAIL)
		{
			kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
			kEffectNames.push_back("EFFECT_CITY_FIRE");
		}
		else
		{
			kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
		}
		return;
	} 

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode()) 
	{
		
		if (angryPopulation() > 0)
		{
			kEffectNames.push_back("EFFECT_CITY_BURNING_SMOKE");
		}
		
		if (healthRate() < 0)
		{
			kEffectNames.push_back("EFFECT_CITY_DISEASED");
		}


		if (isWeLoveTheKingDay())
		{
			int iSeed = natGetDeterministicRandom(0, 32767, getX_INLINE(), getY_INLINE());
			CvRandom kRand;
			kRand.init(iSeed);

			// fireworks
			const TCHAR* szFireworkEffects[] =
			{"FIREWORKS_RED_LARGE_SLOW",
				"FIREWORKS_RED_SMALL_FAST",
				"FIREWORKS_GREEN_LARGE_SLOW",
				"FIREWORKS_GREEN_SMALL_FAST",
				"FIREWORKS_PURPLE_LARGE_SLOW",
				"FIREWORKS_PURPLE_SMALL_FAST",
				"FIREWORKS_YELLOW_LARGE_SLOW",
				"FIREWORKS_YELLOW_SMALL_FAST",
				"FIREWORKS_BLUE_LARGE_SLOW",
				"FIREWORKS_BLUE_SMALL_FAST"};

			int iNumEffects = sizeof(szFireworkEffects) / sizeof(TCHAR*);
			for(int i = 0; i < (iNumEffects < 3 ? iNumEffects : 3); i++)
			{
				kEffectNames.push_back(szFireworkEffects[kRand.get(iNumEffects)]);
			}
		}
	}
}

void CvCity::getCityBillboardSizeIconColors(NiColorA& kDotColor, NiColorA& kTextColor) const
{
	const NiColorA kPlayerColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(GET_PLAYER(getOwnerINLINE()).getPlayerColor()).getColorTypePrimary()).getColor();
	static const NiColorA kGrowing(0.73f, 1, 0.73f, 1);
	static const NiColorA kShrinking(1, 0.73f, 0.73f, 1);
	static const NiColorA kStagnant(0.83f, 0.83f, 0.83f, 1);
	static const NiColorA kUnknown(.5f, .5f, .5f, 1);
	static const NiColorA kWhite(1, 1, 1, 1);
	static const NiColorA kBlack(0, 0, 0, 1);

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()))
	{
		if (foodDifference() < 0)
		{
			if ((foodDifference() == -1) && (getFood() >= ((75 * growthThreshold()) / 100)))
			{
				kDotColor = kStagnant;
				kTextColor = kBlack;	
			}
			else
			{
				kDotColor = kShrinking;
				kTextColor = kBlack;
			}
		}
		else if (foodDifference() > 0)
		{
			kDotColor = kGrowing;
			kTextColor = kBlack;
		}
		else if (foodDifference() == 0)
		{
			kDotColor = kStagnant;
			kTextColor = kBlack;
		}
	}
	else
	{
		kDotColor = kPlayerColor;
		const NiColorA kPlayerSecondaryColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(GET_PLAYER(getOwnerINLINE()).getPlayerColor()).getColorTypeSecondary()).getColor();
		kTextColor = kPlayerSecondaryColor;
	}
}

const TCHAR* CvCity::getCityBillboardProductionIcon() const
{
	if (canBeSelected() && isProduction())
	{
		CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
		FAssert(pOrderNode != NULL);

		const TCHAR* szIcon = NULL;
		switch(pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			{
				UnitTypes eType = getProductionUnit();
				FAssert(eType != NO_UNIT);
				szIcon = GET_PLAYER(getOwnerINLINE()).getUnitButton(eType);
				break;
			}
		case ORDER_CONSTRUCT:
			{
				BuildingTypes eType = getProductionBuilding();
				FAssert(eType != NO_BUILDING);
				szIcon = GC.getBuildingInfo(eType).getButton();
				break;
			}
		case ORDER_CREATE:
			{
				ProjectTypes eType = getProductionProject();
				FAssert(eType != NO_PROJECT);
				szIcon = GC.getProjectInfo(eType).getButton();
				break;
			}
		case ORDER_MAINTAIN:
			{
				ProcessTypes eType = getProductionProcess();
				FAssert(eType != NO_PROCESS);
				szIcon = GC.getProcessInfo(eType).getButton();
				break;
			}
		case ORDER_LIST:
			{
				// Should not happen
				break;
			}
		default:
			{
				FAssert(false);
			}
		}
		return szIcon;
	}
	else
	{
		return ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_NOPRODUCTION")->getPath();
	}
}

bool CvCity::getFoodBarPercentages(std::vector<float>& afPercentages) const
{
	if (!canBeSelected())
	{
		return false;
	}

	afPercentages.resize(NUM_INFOBAR_TYPES, 0.0f);
	if (foodDifference() < 0)
	{
		afPercentages[INFOBAR_STORED] = std::max(0, (getFood() + foodDifference())) / (float) growthThreshold();
		afPercentages[INFOBAR_RATE_EXTRA] = std::min(-foodDifference(), getFood()) / (float) growthThreshold();
	}
	else
	{
		afPercentages[INFOBAR_STORED] = getFood() / (float) growthThreshold();
		afPercentages[INFOBAR_RATE] = foodDifference() / (float) growthThreshold();
	}

	return true;
}

bool CvCity::getProductionBarPercentages(std::vector<float>& afPercentages) const
{
	if (!canBeSelected())
	{
		return false;
	}

	if (!isProductionProcess())
	{
		afPercentages.resize(NUM_INFOBAR_TYPES, 0.0f);
		int iProductionDiffNoFood = getCurrentProductionDifference(true, true);
		int iProductionDiffJustFood = getCurrentProductionDifference(false, true) - iProductionDiffNoFood;
		afPercentages[INFOBAR_STORED] = getProduction() / (float) getProductionNeeded();
		afPercentages[INFOBAR_RATE] = iProductionDiffNoFood / (float) getProductionNeeded();
		afPercentages[INFOBAR_RATE_EXTRA] = iProductionDiffJustFood / (float) getProductionNeeded();
	}

	return true;
}

NiColorA CvCity::getBarBackgroundColor() const
{
	if (atWar(getTeam(), GC.getGameINLINE().getActiveTeam()))
	{
		return NiColorA(0.5f, 0, 0, 0.5f); // red
	}
	return NiColorA(0, 0, 0, 0.5f);
}

bool CvCity::isStarCity() const
{
	return isCapital();
}

bool CvCity::isValidBuildingLocation(BuildingTypes eBuilding) const
{
	PROFILE_FUNC();

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	// if both the river and water flags are set, we require one of the two conditions, not both
	if (kBuilding.isWater())
	{
		if (!kBuilding.isRiver() || !plot()->isRiver())
		{
			if (!isCoastal(kBuilding.getMinAreaSize()))
			{
				return false;
			}
		}
	}
	else
	{
		if (area()->getNumTiles() < kBuilding.getMinAreaSize())
		{
			return false;
		}

		if (kBuilding.isRiver())
		{
			if (!(plot()->isRiver()))
			{
				return false;
			}
		}
	}
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (!isValidTerrainForBuildings(eBuilding))
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/				
	if (kBuilding.isFreshWater())
	{
		if (!(plot()->isFreshWater()))
		{
			return false;
		}
	}

	int iCount = kBuilding.getNumMapCategoryTypes();
	bool bFound = (iCount < 1);
	for (int iI = 0; iI < iCount; iI++)
	{
		if (plot()->isMapCategoryType((MapCategoryTypes)kBuilding.getMapCategoryType(iI)))
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		return false;
	}


	return true;
}

bool CvCity::isEventTriggerPossible(EventTriggerTypes eTrigger) const
{
	FAssert(eTrigger >= 0);
	FAssert(eTrigger < GC.getNumEventTriggerInfos());

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (!CvString(kTrigger.getPythonCanDoCity()).empty())
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eTrigger);
		argsList.add(getOwnerINLINE());
		argsList.add(getID());

		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYRandomEventModule, kTrigger.getPythonCanDoCity(), argsList.makeFunctionArgs(), &lResult);

		if (0 == lResult)
		{
			return false;
		}
	}

	if (kTrigger.getNumBuildings() > 0 && kTrigger.getNumBuildingsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
		{
			if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
			{
				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kTrigger.getBuildingRequired(i));
				if (NO_BUILDING != eBuilding)
				{
					if (getNumRealBuilding(eBuilding) > 0)
					{
						bFoundValid = true;
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}


	if (getReligionCount() < kTrigger.getNumReligions())
	{
		return false;
	}

	if (kTrigger.getNumReligions() > 0 && kTrigger.getNumReligionsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
		{
			if (!kTrigger.isStateReligion() || kTrigger.getReligionRequired(i) == GET_PLAYER(getOwnerINLINE()).getStateReligion())
			{
				if (isHasReligion((ReligionTypes)kTrigger.getReligionRequired(i)))
				{
					if (!kTrigger.isHolyCity() || isHolyCity((ReligionTypes)kTrigger.getReligionRequired(i)))
					{
						bFoundValid = true;
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (getCorporationCount() < kTrigger.getNumCorporations())
	{
		return false;
	}

	if (kTrigger.getNumCorporations() > 0 && kTrigger.getNumCorporationsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
		{
			if (isHasCorporation((CorporationTypes)kTrigger.getCorporationRequired(i)))
			{
				if (!kTrigger.isHeadquarters() || isHeadquarters((CorporationTypes)kTrigger.getCorporationRequired(i)))
				{
					bFoundValid = true;
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getMinPopulation() > 0)
	{
		if (getPopulation() < kTrigger.getMinPopulation())
		{
			return false;
		}
	}


	if (kTrigger.getMaxPopulation() > 0)
	{
		if (getPopulation() > kTrigger.getMaxPopulation())
		{
			return false;
		}
	}

	if (kTrigger.getAngry() > 0)
	{
		if (unhappyLevel(0) - happyLevel() < kTrigger.getAngry())
		{
			return false;
		}
	}
	else if (kTrigger.getAngry() < 0)
	{
		if (happyLevel() - unhappyLevel(0) < -kTrigger.getAngry())
		{
			return false;
		}
	}

	if (kTrigger.getUnhealthy() > 0)
	{
		if (badHealth(false, 0) - goodHealth() < kTrigger.getUnhealthy())
		{
			return false;
		}
	}
	else if (kTrigger.getUnhealthy() < 0)
	{
		if (goodHealth() - badHealth(false, 0) < -kTrigger.getUnhealthy())
		{
			return false;
		}
	}

	if (kTrigger.isPrereqEventCity() && kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); ++iI)
		{
			if (!isEventOccured((EventTypes)kTrigger.getPrereqEvent(iI)))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (!((*getPropertiesConst()) >= *kTrigger.getPrereqMinProperties()))
		return false;

	if (!((*getPropertiesConst()) <= *kTrigger.getPrereqMaxProperties()))
		return false;

	if (0 == getFood() && kTrigger.getCityFoodWeight() > 0)
	{
		return false;
	}
	return true;
}

int CvCity::getTriggerValue(EventTriggerTypes eTrigger) const
{
	FAssert(eTrigger >= 0);
	FAssert(eTrigger < GC.getNumEventTriggerInfos());

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (!isEventTriggerPossible(eTrigger))
	{
		return MIN_INT;
	}

	int iValue = 0;

	iValue += getFood() * kTrigger.getCityFoodWeight();

	return iValue;
}

bool CvCity::canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		return true;
	}

	if (-1 == kTriggeredData.m_iCityId && kEvent.isCityEffect())
	{
		return false;
	}

	if (-1 == kTriggeredData.m_iOtherPlayerCityId && kEvent.isOtherPlayerCityEffect())
	{
		return false;
	}

	if (kEvent.getFood() + ((100 + kEvent.getFoodPercent()) * getFood()) / 100 < 0)
	{
		return false;
	}

	if (kEvent.getPopulationChange() + getPopulation() <= 0)
	{
		return false;
	}

	if (100 * kEvent.getCulture() + getCultureTimes100(getOwnerINLINE()) < 0)
	{
		return false;
	}

	if (kEvent.getBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kEvent.getBuildingClass());
		if (eBuilding == NO_BUILDING)
		{
			return false;
		}

		if (kEvent.getBuildingChange() > 0)
		{
			if (getNumBuilding(eBuilding) >= GC.getCITY_MAX_NUM_BUILDINGS())
			{
				return false;
			}
		}
		else if (kEvent.getBuildingChange() < 0)
		{
			if (getNumRealBuilding(eBuilding) + kEvent.getBuildingChange() < 0)
			{
				return false;
			}
		}
	}

	if (-1 != kEvent.getMaxNumReligions() && getReligionCount() > kEvent.getMaxNumReligions())
	{
		return false;
	}

	if (kEvent.getMinPillage() > 0)
	{
		int iNumImprovements = 0;
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		for (int i = 0; i < getNumCityPlots(); ++i)
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
		{
			if (CITY_HOME_PLOT != i)
			{
				CvPlot* pPlot = getCityIndexPlot(i);
				if (NULL != pPlot && pPlot->getOwnerINLINE() == getOwnerINLINE())
				{
					if (NO_IMPROVEMENT != pPlot->getImprovementType() && !GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
					{
						++iNumImprovements;
					}
				}
			}
		}

		if (iNumImprovements < kEvent.getMinPillage())
		{
			return false;
		}
	}

	return true;
}

void CvCity::applyEvent(EventTypes eEvent, const EventTriggeredData* pTriggeredData)
{
	//	NULL pTriggeredData implies a replay after a reset of modifiers and only modifier effects
	//	should be applied
	bool	adjustModifiersOnly = (pTriggeredData == NULL);
	const EventTriggeredData& kTriggeredData = *pTriggeredData;

	if ( !adjustModifiersOnly )
	{
		if (!canApplyEvent(eEvent, kTriggeredData))
		{
			return;
		}

		setEventOccured(eEvent, true);
	}

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getHappy() != 0)
		{
			changeExtraHappiness(kEvent.getHappy());
		}

		if (kEvent.getHealth() != 0)
		{
			changeExtraHealth(kEvent.getHealth());
		}

		if (kEvent.getHurryAnger() != 0 && !adjustModifiersOnly)
		{
			changeHurryAngerTimer(kEvent.getHurryAnger() * flatHurryAngerLength());
		}

		if (kEvent.getHappyTurns() != 0 && !adjustModifiersOnly)
		{
			changeHappinessTimer(kEvent.getHappyTurns());
		}

		if (kEvent.getFood() != 0 || kEvent.getFoodPercent() != 0)
		{
			changeFood(kEvent.getFood() + (kEvent.getFoodPercent() * getFood()) / 100);
		}

		if (kEvent.getPopulationChange() != 0 && !adjustModifiersOnly)
		{
			changePopulation(kEvent.getPopulationChange());
		}

		if (kEvent.getRevoltTurns() > 0 && !adjustModifiersOnly)
		{
			changeCultureUpdateTimer(kEvent.getRevoltTurns());
			changeOccupationTimer(kEvent.getRevoltTurns());
		}

		if (0 != kEvent.getSpaceProductionModifier())
		{
			changeSpaceProductionModifier(kEvent.getSpaceProductionModifier());
		}

		if (kEvent.getMaxPillage() > 0 && !adjustModifiersOnly)
		{
			FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
			int iNumPillage = kEvent.getMinPillage() + GC.getGameINLINE().getSorenRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage(), "Pick number of event pillaged plots");

			int iNumPillaged = 0;
			for (int i = 0; i < iNumPillage; ++i)
			{
				int iRandOffset = GC.getGameINLINE().getSorenRandNum(NUM_CITY_PLOTS, "Pick event pillage plot");
				for (int j = 0; j < NUM_CITY_PLOTS; ++j)
				{
					int iPlot = (j + iRandOffset) % NUM_CITY_PLOTS;
					if (CITY_HOME_PLOT != iPlot)
					{
						CvPlot* pPlot = getCityIndexPlot(iPlot);
						if (NULL != pPlot && pPlot->getOwnerINLINE() == getOwnerINLINE())
						{
							if (NO_IMPROVEMENT != pPlot->getImprovementType() && !GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
							{
								MEMORY_TRACK_EXEMPT();

								CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
								AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, GC.getImprovementInfo(pPlot->getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
								pPlot->setImprovementType(NO_IMPROVEMENT);
								++iNumPillaged;
								break;
							}
						}
					}
				}
			}

			PlayerTypes eOtherPlayer = kTriggeredData.m_eOtherPlayer;
			if (!kEvent.isCityEffect() && kEvent.isOtherPlayerCityEffect())
			{
				eOtherPlayer = kTriggeredData.m_ePlayer;
			}

			if (NO_PLAYER != eOtherPlayer)
			{
				MEMORY_TRACK_EXEMPT();

				CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED", iNumPillaged, GET_PLAYER(getOwnerINLINE()).getCivilizationAdjectiveKey());
				AddDLLMessage(eOtherPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO);
			}
		}

		for (int i = 0; i < NUM_COMMERCE_TYPES; ++i)
		{
			if (kEvent.getCommerceModifier(i) != 0)
			{
				changeCommerceRateModifier((CommerceTypes)i, kEvent.getCommerceModifier(i));
			}
		}

		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if (kEvent.getYieldModifier(i) != 0)
			{
				changeYieldRateModifier((YieldTypes)i, kEvent.getYieldModifier(i));
			}
		}

		for (int i = 0; i < GC.getNumSpecialistInfos(); ++i)
		{
			if (kEvent.getFreeSpecialistCount(i) > 0)
			{
				changeFreeSpecialistCount((SpecialistTypes)i, kEvent.getFreeSpecialistCount(i));
			}
		}

		if (kEvent.getCulture() != 0 && !adjustModifiersOnly)
		{
			changeCulture(getOwnerINLINE(), kEvent.getCulture(), true, true);
		}
		
/************************************************************************************************/
/* Afforess	                  Start		 01/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		if (kEvent.getRevolutionIndexChange() > 0 && !adjustModifiersOnly)
		{
			changeLocalRevIndex(kEvent.getRevolutionIndexChange());
		}
		else if (kEvent.getRevolutionIndexChange() < 0 && !adjustModifiersOnly)
		{
			changeLocalRevIndex(std::max(-getLocalRevIndex(), kEvent.getRevolutionIndexChange()));
		}		
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	}


	if (kEvent.getUnitClass() != NO_UNITCLASS && !adjustModifiersOnly)
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kEvent.getUnitClass());
		if (eUnit != NO_UNIT)
		{
			for (int i = 0; i < kEvent.getNumUnits(); ++i)
			{
				GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));
			}
		}
	}

	if (kEvent.getBuildingClass() != NO_BUILDINGCLASS && !adjustModifiersOnly)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kEvent.getBuildingClass());
		if (eBuilding != NO_BUILDING)
		{
			if (0 != kEvent.getBuildingChange())
			{
				setNumRealBuilding(eBuilding, getNumRealBuilding(eBuilding) + kEvent.getBuildingChange());
			}
		}
	}

	if (kEvent.getNumBuildingYieldChanges() > 0)
	{
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				setBuildingYieldChange((BuildingClassTypes)iBuildingClass, (YieldTypes)iYield, getBuildingYieldChange((BuildingClassTypes)iBuildingClass, (YieldTypes)iYield) + kEvent.getBuildingYieldChange(iBuildingClass, iYield));
			}
		}
	}

	if (kEvent.getNumBuildingCommerceChanges() > 0)
	{
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
			{
				setBuildingCommerceChange((BuildingClassTypes)iBuildingClass, (CommerceTypes)iCommerce, getBuildingCommerceChange((BuildingClassTypes)iBuildingClass, (CommerceTypes)iCommerce) + kEvent.getBuildingCommerceChange(iBuildingClass, iCommerce));
			}
		}
	}

	if (kEvent.getNumBuildingHappyChanges() > 0)
	{
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			setBuildingHappyChange((BuildingClassTypes)iBuildingClass, kEvent.getBuildingHappyChange(iBuildingClass));
		}
	}

	if (kEvent.getNumBuildingHealthChanges() > 0)
	{
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			setBuildingHealthChange((BuildingClassTypes)iBuildingClass, kEvent.getBuildingHealthChange(iBuildingClass));
		}
	}

	getProperties()->addProperties(kEvent.getProperties());
	//GET_PLAYER(getOwnerINLINE()).getProperties()->addProperties(kEvent.getProperties());
}

bool CvCity::isEventOccured(EventTypes eEvent) const
{
	for (std::vector<EventTypes>::const_iterator it = m_aEventsOccured.begin(); it != m_aEventsOccured.end(); ++it)
	{
		if (*it == eEvent)
		{
			return true;
		}
	}

	return false;
}

void CvCity::setEventOccured(EventTypes eEvent, bool bOccured)
{
	for (std::vector<EventTypes>::iterator it = m_aEventsOccured.begin(); it != m_aEventsOccured.end(); ++it)
	{
		if (*it == eEvent)
		{
			if (!bOccured)
			{
				m_aEventsOccured.erase(it);
			}
			return;
		}
	}

	if (bOccured)
	{
		m_aEventsOccured.push_back(eEvent);
	}
}

// CACHE: cache frequently used values
///////////////////////////////////////
bool CvCity::hasShrine(ReligionTypes eReligion)
{
	bool bHasShrine = false;
	
	// note, for normal XML, this count will be one, there is only one shrine of each religion
	int	shrineBuildingCount = GC.getGameINLINE().getShrineBuildingCount(eReligion);
	for (int iI = 0; iI < shrineBuildingCount; iI++)
	{
		BuildingTypes eBuilding = GC.getGameINLINE().getShrineBuilding(iI, eReligion);

		if (getNumBuilding(eBuilding) > 0)
		{
			bHasShrine = true;
			break;
		}
	}

	return bHasShrine;
}

bool CvCity::hasOrbitalInfrastructure() const
{
	//ls612: To check if a city gets full benefits from Orbital Buildings
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).isOrbitalInfrastructure() && getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			return true;
		}
	}

	return false;
}

void CvCity::invalidatePopulationRankCache()
{
	m_bPopulationRankValid = false;
}

void CvCity::invalidateYieldRankCache(YieldTypes eYield)
{
	FAssertMsg(eYield >= NO_YIELD && eYield < NUM_YIELD_TYPES, "invalidateYieldRankCache passed bogus yield index");

	if (eYield == NO_YIELD)
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			m_abBaseYieldRankValid[iI] = false;
			m_abYieldRankValid[iI] = false;
		}
	}
	else
	{
		m_abBaseYieldRankValid[eYield] = false;
		m_abYieldRankValid[eYield] = false;
	}
}

void CvCity::invalidateCommerceRankCache(CommerceTypes eCommerce)
{
	FAssertMsg(eCommerce >= NO_COMMERCE && eCommerce < NUM_COMMERCE_TYPES, "invalidateCommerceRankCache passed bogus commerce index");

	if (eCommerce == NO_COMMERCE)
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			m_abCommerceRankValid[iI] = false;
		}
	}
	else
	{
		m_abCommerceRankValid[eCommerce] = false;
	}
}

int CvCity::getBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const
{
	for (std::vector<BuildingYieldChange>::const_iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		if ((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

void CvCity::setBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	for (std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		BuildingYieldChange& yieldChange = *it;
		if (yieldChange.eBuildingClass == eBuildingClass && yieldChange.eYield == eYield)
		{
			int iOldChange = yieldChange.iChange;
			if (iOldChange != iChange)
			{

				if (iChange == 0)
				{
					// Don't worry, we are exiting the function at this point, not continuing the loop
					m_aBuildingYieldChange.erase(it);
				}
				else
				{
					yieldChange.iChange = iChange;
				}

				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
				if (NO_BUILDING != eBuilding)
				{
					if (getNumActiveBuilding(eBuilding) > 0)
					{
						//Team Project (5)
						if (!isReligiouslyDisabledBuilding(eBuilding))
						{
							changeBaseYieldRate(eYield, (iChange - iOldChange) * getNumActiveBuilding(eBuilding));
						}
					}
				}
			}

			return;
		}
	}

	if (0 != iChange)
	{
		BuildingYieldChange kChange;
		kChange.eBuildingClass = eBuildingClass;
		kChange.eYield = eYield;
		kChange.iChange = iChange;
		m_aBuildingYieldChange.push_back(kChange);

		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
		if (NO_BUILDING != eBuilding)
		{
			if (getNumActiveBuilding(eBuilding) > 0)
			{
				//Team Project (5)
				if (!isReligiouslyDisabledBuilding(eBuilding))
				{
					changeBaseYieldRate(eYield, iChange * getNumActiveBuilding(eBuilding));
				}
			}
		}
	}
}

void CvCity::changeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	setBuildingYieldChange(eBuildingClass, eYield, getBuildingYieldChange(eBuildingClass, eYield) + iChange);
}

int CvCity::getBuildingCommerceChange(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce) const
{
	for (std::vector<BuildingCommerceChange>::const_iterator it = m_aBuildingCommerceChange.begin(); it != m_aBuildingCommerceChange.end(); ++it)
	{
		if ((*it).eBuildingClass == eBuildingClass && (*it).eCommerce == eCommerce)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

void CvCity::setBuildingCommerceChange(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce, int iChange)
{
	for (std::vector<BuildingCommerceChange>::iterator it = m_aBuildingCommerceChange.begin(); it != m_aBuildingCommerceChange.end(); ++it)
	{
		BuildingCommerceChange& commerceChange = *it;
		if (commerceChange.eBuildingClass == eBuildingClass && commerceChange.eCommerce == eCommerce)
		{
			if (commerceChange.iChange != iChange)
			{
				if (iChange == 0)
				{
					// Don't worry, we are exiting the function at this point, not continuing the loop
					m_aBuildingCommerceChange.erase(it);
				}
				else
				{
					commerceChange.iChange = iChange;
				}

				updateBuildingCommerce();
			}

			return;
		}
	}

	if (0 != iChange)
	{
		BuildingCommerceChange kChange;
		kChange.eBuildingClass = eBuildingClass;
		kChange.eCommerce = eCommerce;
		kChange.iChange = iChange;
		m_aBuildingCommerceChange.push_back(kChange);

		updateBuildingCommerce();
	}
}

void CvCity::changeBuildingCommerceChange(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce, int iChange)
{
	setBuildingCommerceChange(eBuildingClass, eCommerce, getBuildingCommerceChange(eBuildingClass, eCommerce) + iChange);
}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       10/22/09                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* orginal bts code
void CvCity::setBuildingHappyChange(BuildingClassTypes eBuildingClass, int iChange)
{
	for (BuildingChangeArray::iterator it = m_aBuildingHappyChange.begin(); it != m_aBuildingHappyChange.end(); ++it)
	{
		if ((*it).first == eBuildingClass)
		{
			if ((*it).second != iChange)
			{
				if ((*it).second > 0)
				{
					changeBuildingGoodHappiness(-(*it).second);
				}
				else if ((*it).second < 0)
				{
					changeBuildingBadHappiness((*it).second);
				}

				if (iChange == 0)
				{
					m_aBuildingHappyChange.erase(it);
				}
				else
				{
					(*it).second = iChange;
				}

				if (iChange > 0)
				{
					changeBuildingGoodHappiness(iChange);
				}
				else if (iChange < 0)
				{
					changeBuildingGoodHappiness(-iChange);
				}
			}

			return;
		}
	}

	if (0 != iChange)
	{
		m_aBuildingHappyChange.push_back(std::make_pair(eBuildingClass, iChange));

		if (iChange > 0)
		{
			changeBuildingGoodHappiness(iChange);
		}
		else if (iChange < 0)
		{
			changeBuildingGoodHappiness(-iChange);
		}
	}
}
*/
void CvCity::setBuildingHappyChange(BuildingClassTypes eBuildingClass, int iChange)
{
	for (BuildingChangeArray::iterator it = m_aBuildingHappyChange.begin(); it != m_aBuildingHappyChange.end(); ++it)
	{
		if ((*it).first == eBuildingClass)
		{
			if ((*it).second != iChange)
			{
				int iOldChange = (*it).second;

				m_aBuildingHappyChange.erase(it);

				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
				if (NO_BUILDING != eBuilding)
				{
					if (getNumActiveBuilding(eBuilding) > 0)
					{
						//Team Project (5)
						if (!isReligiouslyDisabledBuilding(eBuilding))
						{
							if (iOldChange > 0)
							{
								changeBuildingGoodHappiness(-iOldChange);
							}
							else if (iOldChange < 0)
							{
								changeBuildingBadHappiness(-iOldChange);
							}

							if( iChange != 0 )
							{
								m_aBuildingHappyChange.push_back(std::make_pair(eBuildingClass, iChange));
								if (iChange > 0)
								{
									changeBuildingGoodHappiness(iChange);
								}
								else if (iChange < 0)
								{
									changeBuildingBadHappiness(iChange);
								}
							}
						}
					}
				}
			}

			return;
		}
	}

	if (0 != iChange)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
		if (NO_BUILDING != eBuilding)
		{
			if (getNumActiveBuilding(eBuilding) > 0)
			{
				//Team Project (5)
				if (!isReligiouslyDisabledBuilding(eBuilding))
				{
					m_aBuildingHappyChange.push_back(std::make_pair(eBuildingClass, iChange));

					if (iChange > 0)
					{
						changeBuildingGoodHappiness(iChange);
					}
					else if (iChange < 0)
					{
						changeBuildingBadHappiness(iChange);
					}
				}
			}
		}
	}
}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/

int CvCity::getBuildingHappyChange(BuildingClassTypes eBuildingClass) const
{
	for (BuildingChangeArray::const_iterator it = m_aBuildingHappyChange.begin(); it != m_aBuildingHappyChange.end(); ++it)
	{
		if ((*it).first == eBuildingClass)
		{
			return (*it).second;
		}
	}

	return 0;
}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       10/22/09                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* orginal bts code
void CvCity::setBuildingHealthChange(BuildingClassTypes eBuildingClass, int iChange)
{
	for (BuildingChangeArray::iterator it = m_aBuildingHealthChange.begin(); it != m_aBuildingHealthChange.end(); ++it)
	{
		if ((*it).first == eBuildingClass)
		{
			if ((*it).second != iChange)
			{
				if ((*it).second > 0)
				{
					changeBuildingGoodHealth(-(*it).second);
				}
				else if ((*it).second < 0)
				{
					changeBuildingBadHealth((*it).second);
				}

				if (iChange == 0)
				{
					m_aBuildingHealthChange.erase(it);
				}
				else
				{
					(*it).second = iChange;
				}

				if (iChange > 0)
				{
					changeBuildingGoodHealth(iChange);
				}
				else if (iChange < 0)
				{
					changeBuildingBadHealth(-iChange);
				}
			}

			return;
		}
	}

	if (0 != iChange)
	{
		m_aBuildingHealthChange.push_back(std::make_pair(eBuildingClass, iChange));

		if (iChange > 0)
		{
			changeBuildingGoodHappiness(iChange);
		}
		else if (iChange < 0)
		{
			changeBuildingGoodHappiness(-iChange);
		}
	}
}
*/
void CvCity::setBuildingHealthChange(BuildingClassTypes eBuildingClass, int iChange)
{
	for (BuildingChangeArray::iterator it = m_aBuildingHealthChange.begin(); it != m_aBuildingHealthChange.end(); ++it)
	{
		if ((*it).first == eBuildingClass)
		{
			if ((*it).second != iChange)
			{
				int iOldChange = (*it).second;

				m_aBuildingHealthChange.erase(it);

				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
				if (NO_BUILDING != eBuilding)
				{
					if (getNumActiveBuilding(eBuilding) > 0)
					{
						//Team Project (5)
						if (!isReligiouslyDisabledBuilding(eBuilding))
						{
							if (iOldChange > 0)
							{
								changeBuildingGoodHealth(-iOldChange);
							}
							else if (iOldChange < 0)
							{
								changeBuildingBadHealth(-iOldChange);
							}

							if( iChange != 0 )
							{
								m_aBuildingHealthChange.push_back(std::make_pair(eBuildingClass, iChange));
								if (iChange > 0)
								{
									changeBuildingGoodHealth(iChange);
								}
								else if (iChange < 0)
								{
									changeBuildingBadHealth(iChange);
								}
							}
						}
					}
				}
			}

			return;
		}
	}

	if (0 != iChange)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
		if (NO_BUILDING != eBuilding)
		{
			if (getNumActiveBuilding(eBuilding) > 0)
			{	
				//Team Project (5)
				if (!isReligiouslyDisabledBuilding(eBuilding))
				{
					m_aBuildingHealthChange.push_back(std::make_pair(eBuildingClass, iChange));

					if (iChange > 0)
					{
						changeBuildingGoodHealth(iChange);
					}
					else if (iChange < 0)
					{
						changeBuildingBadHealth(iChange);
					}
				}
			}
		}
	}
}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/

int CvCity::getBuildingHealthChange(BuildingClassTypes eBuildingClass) const
{
	for (BuildingChangeArray::const_iterator it = m_aBuildingHealthChange.begin(); it != m_aBuildingHealthChange.end(); ++it)
	{
		if ((*it).first == eBuildingClass)
		{
			return (*it).second;
		}
	}

	return 0;
}

void CvCity::liberate(bool bConquest)
{
	CvPlot* pPlot = plot();
	PlayerTypes ePlayer = getLiberationPlayer(bConquest);
	PlayerTypes eOwner = getOwnerINLINE();

	if (NO_PLAYER != ePlayer)
	{
		int iOldOwnerCulture = getCultureTimes100(eOwner);
		int iOldMasterLand = 0;
		int iOldVassalLand = 0;
		if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isVassal(GET_PLAYER(eOwner).getTeam()))
		{
			iOldMasterLand = GET_TEAM(GET_PLAYER(eOwner).getTeam()).getTotalLand();
			iOldVassalLand = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getTotalLand(false);
		}

		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_LIBERATED", getNameKey(), GET_PLAYER(eOwner).getNameKey(), GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					MEMORY_TRACK_EXEMPT();

					AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
				}
			}
		}
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eOwner, szBuffer, getX_INLINE(), getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

		GET_PLAYER(ePlayer).acquireCity(this, false, true, true);
		GET_PLAYER(ePlayer).AI_changeMemoryCount(eOwner, MEMORY_LIBERATED_CITIES, 1);

		if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isVassal(GET_PLAYER(eOwner).getTeam()))
		{
			int iNewMasterLand = GET_TEAM(GET_PLAYER(eOwner).getTeam()).getTotalLand();
			int iNewVassalLand = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getTotalLand(false);

			GET_TEAM(GET_PLAYER(ePlayer).getTeam()).setMasterPower(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getMasterPower() + iNewMasterLand - iOldMasterLand);
			GET_TEAM(GET_PLAYER(ePlayer).getTeam()).setVassalPower(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getVassalPower() + iNewVassalLand - iOldVassalLand);
		}

		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();
			if (NULL != pCity)
			{
				pCity->setCultureTimes100(ePlayer, pCity->getCultureTimes100(ePlayer) + iOldOwnerCulture / 2, true, true);
			}

			if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAVassal())
			{
				for (int i = 0; i < GC.getDefineINT("COLONY_NUM_FREE_DEFENDERS"); ++i)
				{
					pCity->initConscriptedUnit();
				}
			}
		}
	}
}

PlayerTypes CvCity::getLiberationPlayer(bool bConquest) const
{
	if (isCapital())
	{
		return NO_PLAYER;
	}

	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getOwnerINLINE())
		{
			CvCity* pLoopCapital = kLoopPlayer.getCapitalCity();
			if (NULL != pLoopCapital)
			{
				if (pLoopCapital->area() == area())
				{
					return (PlayerTypes)iPlayer;
				}
			}
		}
	}

	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	if (kOwner.canSplitEmpire() && kOwner.canSplitArea(area()->getID()))
	{
		PlayerTypes ePlayer = GET_PLAYER(getOwnerINLINE()).getSplitEmpirePlayer(area()->getID());

		if (NO_PLAYER != ePlayer)
		{
			if (GET_PLAYER(ePlayer).isAlive())
			{
				return ePlayer;
			}
		}
	}

	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;

	int iTotalCultureTimes100 = countTotalCultureTimes100();

	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.canReceiveTradeCity())
			{
				CvCity* pCapital = kLoopPlayer.getCapitalCity();
				if (NULL != pCapital)
				{
					int iCapitalDistance = ::plotDistance(getX_INLINE(), getY_INLINE(), pCapital->getX_INLINE(), pCapital->getY_INLINE());
					if (area() != pCapital->area())
					{
						iCapitalDistance *= 2;
					}

					int iCultureTimes100 = getCultureTimes100((PlayerTypes)iPlayer);

					if (bConquest)
					{
						if (iPlayer == getOriginalOwner())
						{
							iCultureTimes100 *= 3;
							iCultureTimes100 /= 2;
						}
					}

					if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getTeam() 
						|| GET_TEAM(GET_PLAYER((PlayerTypes)iPlayer).getTeam()).isVassal(getTeam()) 
						|| GET_TEAM(getTeam()).isVassal(GET_PLAYER((PlayerTypes)iPlayer).getTeam()))
					{
						iCultureTimes100 *= 2;
						iCultureTimes100 = (iCultureTimes100 + iTotalCultureTimes100) / 2;
					}

					int iValue = std::max(100, iCultureTimes100) / std::max(1, iCapitalDistance);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestPlayer = (PlayerTypes)iPlayer;
					}
				}
			}
		}
	}

	if (NO_PLAYER != eBestPlayer)
	{
		if (getOwnerINLINE() == eBestPlayer)
		{
			return NO_PLAYER;
		}

/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		for (int iPlot = 0; iPlot < getNumCityPlots(); ++iPlot)
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
		{
			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), iPlot);

			if (NULL != pLoopPlot)
			{
				if (pLoopPlot->isVisibleEnemyUnit(eBestPlayer))
				{
					return NO_PLAYER;
				}
			}
		}
	}

	return eBestPlayer;
}

int CvCity::getBestYieldAvailable(YieldTypes eYield) const
{
	int iBestYieldAvailable = 0;

	for (int iJ = 0; iJ < NUM_CITY_PLOTS; ++iJ)
	{
		if (iJ != CITY_HOME_PLOT)
		{
			if (!isWorkingPlot(iJ))
			{
				CvPlot* pPlot = getCityIndexPlot(iJ);

				if (NULL != pPlot && canWork(pPlot))
				{
					if (pPlot->getYield(eYield) > iBestYieldAvailable)
					{
						iBestYieldAvailable = pPlot->getYield(eYield);
					}
				}
			}
		}
	}

	for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); ++iJ)
	{
		if (isSpecialistValid((SpecialistTypes)iJ, 1))
		{
			int iYield = GC.getSpecialistInfo((SpecialistTypes)iJ).getYieldChange(eYield);
			if (iYield > iBestYieldAvailable)
			{
				iBestYieldAvailable = iYield;
			}
		}
	}

	return iBestYieldAvailable;
}

bool CvCity::isAutoRaze() const
{
	if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_CITY_RAZING))
	{
		if (getHighestPopulation() == 1)
		{
			return true;
		}

		if (GC.getGameINLINE().getMaxCityElimination() > 0)
		{
			return true;
		}
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return true;
	}
/************************************************************************************************/
/* Afforess	                  Start		 06/26/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_BARBARIANS_ALWAYS_RAZE) && isHominid())
	{
		return true;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	//Insectoids always raze
	if (GET_PLAYER(getOwner()).getID() == NPC6_PLAYER)
	{
		return true;
	}

	return false;
}

int CvCity::getMusicScriptId() const
{
	bool bIsHappy = true;
	if (GC.getGameINLINE().getActiveTeam() == getTeam())
	{
		if (angryPopulation() > 0)
		{
			bIsHappy = false;
		}
	}
	else
	{			
		if (GET_TEAM(GC.getGameINLINE().getActiveTeam()).isAtWar(getTeam()))
		{
			bIsHappy = false;
		}
	}

	CvLeaderHeadInfo& kLeaderInfo = GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getLeaderType());
	EraTypes eCurEra = GET_PLAYER(getOwnerINLINE()).getCurrentEra();
	if (bIsHappy)
	{	
		return (kLeaderInfo.getDiploPeaceMusicScriptIds(eCurEra));
	}
	else
	{
		return (kLeaderInfo.getDiploWarMusicScriptIds(eCurEra));
	}
}

int CvCity::getSoundscapeScriptId() const
{
	return GC.getEraInfo(GET_PLAYER(getOwnerINLINE()).getCurrentEra()).getCitySoundscapeSciptId(getCitySizeType());
}

void CvCity::cheat(bool bCtrl, bool bAlt, bool bShift)
{
	if (gDLL->getChtLvl() > 0)
	{
		if (bCtrl)
		{
			changeCulture(getOwnerINLINE(), 10, true, true);
		}
		else if (bShift)
		{
			changePopulation(1);
		}
		else
		{
			popOrder(0, true);
		}
	}
}

void CvCity::getBuildQueue(std::vector<std::string>& astrQueue) const
{
	CLLNode<OrderData>* pNode = headOrderQueueNode();
	while (pNode != NULL)
	{
		switch (pNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			astrQueue.push_back(GC.getUnitInfo((UnitTypes)EXTERNAL_ORDER_IDATA(pNode->m_data.iData1)).getType());
			break;

		case ORDER_CONSTRUCT:
			astrQueue.push_back(GC.getBuildingInfo((BuildingTypes)(pNode->m_data.iData1)).getType());
			break;

		case ORDER_CREATE:
			astrQueue.push_back(GC.getProjectInfo((ProjectTypes)(pNode->m_data.iData1)).getType());
			break;

		case ORDER_MAINTAIN:
			astrQueue.push_back(GC.getProcessInfo((ProcessTypes)(pNode->m_data.iData1)).getType());
			break;

		case ORDER_LIST:
			astrQueue.push_back("List");

		default:
			FAssert(false);
			break;
		}

		pNode = nextOrderQueueNode(pNode);
	}
}

/************************************************************************************************/
/* INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/*                                                                                              */
/* Original Author Moctezuma              Start                                                 */
/************************************************************************************************/
// ------ BEGIN InfluenceDrivenWar -------------------------------
void CvCity::emergencyConscript()
{
	CvUnit* pUnit;
	UnitAITypes eCityAI;
	UnitTypes eConscriptUnit;

	int iEmergencyDraftMinPopulation = GC.getIDW_EMERGENCY_DRAFT_MIN_POPULATION();

	if (getPopulation() < iEmergencyDraftMinPopulation)
	{
		return;
	}

	if (getConscriptUnit() == NO_UNIT)
	{
		return;
	}

	changePopulation(-1);

	float fEmergencyDraftAngerMultiplier = GC.getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER();
		
	changeConscriptAngerTimer(int(flatConscriptAngerLength() * fEmergencyDraftAngerMultiplier));

	eConscriptUnit = getConscriptUnit();

	if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eConscriptUnit, UNITAI_CITY_DEFENSE, area()) > 0)
	{
		eCityAI = UNITAI_CITY_DEFENSE;
	}
	else if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eConscriptUnit, UNITAI_CITY_COUNTER, area()) > 0)
	{
		eCityAI = UNITAI_CITY_COUNTER;
	}
	else if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eConscriptUnit, UNITAI_CITY_SPECIAL, area()) > 0)
	{
		eCityAI = UNITAI_CITY_SPECIAL;
	}
	else
	{
		eCityAI = NO_UNITAI;
	}

	pUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eConscriptUnit, getX_INLINE(), getY_INLINE(), eCityAI, NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));
	FAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

	addProductionExperience(pUnit, true);

	pUnit->setMoves(0);

	float fEmergencyDraftStrength = GC.getIDW_EMERGENCY_DRAFT_STRENGTH();

	pUnit->setDamage(int((1 - fEmergencyDraftStrength) * pUnit->maxHitPoints()), getOwnerINLINE());
}
// ------ END InfluenceDrivenWar ---------------------------------
/************************************************************************************************/
/* INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/*                                                                                              */
/* Original Author Moctezuma              End                                                   */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*        New Functions                                                                         */
/************************************************************************************************/

int CvCity::getRevTrend()
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_REVOLUTION))
		return 0;
		
	//This is the value from python
	int iRevInsigatorThreshold = 1000;
	int iRevIndex = std::min(getRevolutionIndex(), iRevInsigatorThreshold);
	int iDeltaTrend = iRevIndex - getRevIndexAverage();
	if (iDeltaTrend != 0)
		iDeltaTrend *= std::max(abs(iDeltaTrend), (iRevInsigatorThreshold / 100) + 1);
	return iDeltaTrend;
}

bool CvCity::isInquisitionConditions()
{
	ReligionTypes eStateReligion = GET_PLAYER(getOwnerINLINE()).getStateReligion();
	
	if (eStateReligion == NO_RELIGION)
		return false;
	
	if (isHasReligion(eStateReligion))
	{
		bool bValid = false;
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if ( isHasReligion(ReligionTypes(iI))
			&& (ReligionTypes(iI) != eStateReligion) )
			{
				if ( !(isHolyCity(ReligionTypes(iI)))
				|| (isHolyCity(ReligionTypes(iI)) && GC.isOC_RESPAWN_HOLY_CITIES()) )
				{
					bValid = true;
					break;
				}
			}
		}
		if (bValid)
		{
			return true;
		}
	}
	
	return false;
}

/* 
Checks the cities culture level and if it meets the criteria specified in the CultureLevelsInfo.xml, the city radius expands.
NUM_CITY_PLOTS is the largest city size, a radius of 3, NUM_CITY_PLOTS_2 is the standard BTS city size ( a radius of 2), and NUM_CITY_PLOTS_3
is a city size of 1.
*/
int CvCity::getNumCityPlots() const
{
	if (getWorkableRadiusOverride() == 0)
	{
		if (!GC.getGameINLINE().isOption(GAMEOPTION_LARGER_CITIES))
		{
			return NUM_CITY_PLOTS_2;
		}
	}
	
	int iRadius;
	int var_city_plots;
	if (getCultureLevel() == -1)
	{
		return NUM_CITY_PLOTS_1;
	}

	iRadius = GC.getCultureLevelInfo(getCultureLevel()).getCityRadius();

	if (getWorkableRadiusOverride() > 0)
	{
		iRadius = getWorkableRadiusOverride();
	}
	
	switch (iRadius)
	{
	case 3:
		var_city_plots = NUM_CITY_PLOTS;
		break;
	case 2:
		var_city_plots = NUM_CITY_PLOTS_2;
		break;
	case 1:
		var_city_plots = NUM_CITY_PLOTS_1;
		break;
	default:
		var_city_plots = NUM_CITY_PLOTS_2;
		break;
	}
	return (var_city_plots);
}

/*
 updateYieldRate(...) checks to see if the building given is in the city, and if it is, updates the yield by the iChange amount.
*/
void CvCity::updateYieldRate(BuildingTypes eBuilding, YieldTypes eYield, int iChange)
{
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType();
	setBuildingYieldChange(eBuildingClass, eYield, iChange);
}

/*
Given a building type, specialist type and the amount of specialists to change, updateMaxSpecialistCount changes the specialist count by the iChange value.
*/
void CvCity::updateMaxSpecialistCount(BuildingTypes eBuilding, SpecialistTypes eSpecialist, int iChange)
{
	if (getNumActiveBuilding(eBuilding) > 0) 
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding(eBuilding))
		{
			changeMaxSpecialistCount(eSpecialist, iChange);
		}
	}
}

int CvCity::getBuildingCommerceModifier(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce) const
{
	for (std::vector<BuildingCommerceModifier>::const_iterator it = m_aBuildingCommerceModifier.begin(); it != m_aBuildingCommerceModifier.end(); ++it)
	{
		if ((*it).eBuildingClass == eBuildingClass && (*it).eCommerce == eCommerce)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

void CvCity::updateCommerceModifierByBuilding(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange)
{	
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType();
	for (std::vector<BuildingCommerceModifier>::iterator it = m_aBuildingCommerceModifier.begin(); it != m_aBuildingCommerceModifier.end(); ++it)
	{
		BuildingCommerceModifier& commerceModifier = *it;

		if (commerceModifier.eBuildingClass == eBuildingClass && commerceModifier.eCommerce == eCommerce)
		{
			if (commerceModifier.iChange != iChange)
			{
				if (iChange == 0)
				{
					// Don't worry, we are exiting the function at this point, not continuing the loop
					m_aBuildingCommerceModifier.erase(it);
				}
				else
				{
					commerceModifier.iChange = iChange;
				}

				setCommerceModifierDirty(eCommerce);
				//updateBuildingCommerce();
			}

			return;
		}
	}

	if (0 != iChange)
	{
		if (NO_BUILDING != eBuilding)
		{
			BuildingCommerceModifier kChange;
			kChange.eBuildingClass = eBuildingClass;
			kChange.eCommerce = eCommerce;
			kChange.iChange = iChange;
			m_aBuildingCommerceModifier.push_back(kChange);

			setCommerceModifierDirty(eCommerce);
			//updateBuildingCommerce();
			GET_PLAYER(getOwnerINLINE()).invalidateCommerceRankCache();
		}
	}
}

/*

*/
void CvCity::updateCommerceRateByBuilding(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange)
{	
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType();
	for (std::vector<BuildingCommerceChange>::iterator it = m_aBuildingCommerceChange.begin(); it != m_aBuildingCommerceChange.end(); ++it)
	{
		BuildingCommerceChange& commerceChange = *it;

		if (commerceChange.eBuildingClass == eBuildingClass && commerceChange.eCommerce == eCommerce)
		{
			if (commerceChange.iChange != iChange)
			{
				if (iChange == 0)
				{
					// Don't worry, we are exiting the function at this point, not continuing the loop
					m_aBuildingCommerceChange.erase(it);
				}
				else
				{
					commerceChange.iChange = iChange;
				}

				updateBuildingCommerce();
			}

			return;
		}
	}

	if (0 != iChange)
	{
		if (NO_BUILDING != eBuilding)
		{
			BuildingCommerceChange kChange;
			kChange.eBuildingClass = eBuildingClass;
			kChange.eCommerce = eCommerce;
			kChange.iChange = iChange;
			m_aBuildingCommerceChange.push_back(kChange);

			updateBuildingCommerce();
		}
	}
}

int CvCity::calculateBuildingCommerceModifier(CommerceTypes eCommerce) const
{
	int iTotalModifier = 0;
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(iI)));
		if (eLoopBuilding != NO_BUILDING)
		{
			if (getNumActiveBuilding(eLoopBuilding))
			{
				//Team Project (5)
				if (!isReligiouslyDisabledBuilding(eLoopBuilding))
				{
					iTotalModifier += getBuildingCommerceModifier((BuildingClassTypes)iI, eCommerce);
				}
			}
		}
	}
	return iTotalModifier;
}

int CvCity::calculateBuildingYieldModifier(YieldTypes eYield) const
{
	PROFILE_FUNC();

	if ( m_cachedBuildingYieldModifers[eYield] == -1 )
	{
		int iTotalModifier = 0;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(iI)));
			if (eLoopBuilding != NO_BUILDING)
			{
				if (getNumActiveBuilding(eLoopBuilding) > 0)
				{
					//Team Project (5)
					if (!isReligiouslyDisabledBuilding(eLoopBuilding))
					{
						iTotalModifier += getBuildingYieldModifier((BuildingClassTypes)iI, eYield);
					}
				}
			}
		}
		m_cachedBuildingYieldModifers[eYield] = iTotalModifier;
	}

	return m_cachedBuildingYieldModifers[eYield];
}


int CvCity::getBuildingYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYield) const
{
	PROFILE_FUNC();

	for (std::vector<BuildingYieldModifier>::const_iterator it = m_aBuildingYieldModifier.begin(); it != m_aBuildingYieldModifier.end(); ++it)
	{
		if ((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

void CvCity::updateYieldModifierByBuilding(BuildingTypes eBuilding, YieldTypes eYield, int iChange)
{
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType();
	for (std::vector<BuildingYieldModifier>::iterator it = m_aBuildingYieldModifier.begin(); it != m_aBuildingYieldModifier.end(); ++it)
	{
		BuildingYieldModifier& yieldModifier = *it;

		if (yieldModifier.eBuildingClass == eBuildingClass && yieldModifier.eYield == eYield)
		{
			int iOldChange = yieldModifier.iChange;
			if (iOldChange != iChange)
			{
				//	Clear cached yield modifier
				m_cachedBuildingYieldModifers[eYield] = -1;

				if (iChange == 0)
				{
					// Don't worry, we are exiting the function at this point, not continuing the loop
					m_aBuildingYieldModifier.erase(it);
				}
				else
				{
					(*it).iChange = iChange;
				}
			}
			GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache();
			return;
		}
	}

	if (0 != iChange)
	{
		if (NO_BUILDING != eBuilding)
		{
			BuildingYieldModifier kChange;
			kChange.eBuildingClass = eBuildingClass;
			kChange.eYield = eYield;
			kChange.iChange = iChange;
			m_aBuildingYieldModifier.push_back(kChange);
		}
		GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache();
	}
}


/*
	Checks to see if the city is producing a wonder
*/
bool CvCity::isProductionWonder() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT)
		{
			BuildingTypes eBuilding = (BuildingTypes)(pOrderNode->m_data.iData1);
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType());
			return (isWorldWonderClass(eBuildingClass) || isTeamWonderClass(eBuildingClass) || isNationalWonderClass(eBuildingClass));
		}
	}

	return false;
}

void CvCity::clearLostProduction()
{
	m_iLostProductionBase = 0;
	m_iLostProductionModified = 0;
	m_iGoldFromLostProduction = 0;
}

int CvCity::getSpecialistGoodHealth() const
{
	return m_iSpecialistGoodHealth;
}

int CvCity::getSpecialistBadHealth() const
{
	return m_iSpecialistBadHealth;
}

int CvCity::getSpecialistHappiness() const
{
	return m_iSpecialistHappiness;
}

int CvCity::getSpecialistUnhappiness() const
{
	return m_iSpecialistUnhappiness;
}

void CvCity::changeSpecialistGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistGoodHealth += iChange;
		FAssert(getSpecialistGoodHealth() >= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeSpecialistBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistBadHealth += iChange;
		FAssert(getSpecialistBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeSpecialistHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistHappiness += iChange;
		FAssert(getSpecialistHappiness() >= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


void CvCity::changeSpecialistUnhappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistUnhappiness += iChange;

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getImprovementGoodHealth() const
{
	return m_iImprovementGoodHealth;
}


int CvCity::getImprovementBadHealth() const
{
	return m_iImprovementBadHealth;
}

void CvCity::updateImprovementHealth()
{
	CvPlayer &kPlayer = GET_PLAYER(getOwnerINLINE());

	int iNewGoodHealthPercent = 0;
	int iNewBadHealthPercent = 0;
	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getOwner() != NO_PLAYER)
			{
				if (pLoopPlot->getOwner() == getOwnerINLINE())
				{
					ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

					if (eImprovement != NO_IMPROVEMENT)
					{
						if (GC.getImprovementInfo(eImprovement).getHealthPercent() > 0)
						{
							iNewGoodHealthPercent += GC.getImprovementInfo(eImprovement).getHealthPercent();
							for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
							{
								if (kPlayer.getCivics((CivicOptionTypes)iJ) != NO_CIVIC)
								{
									iNewGoodHealthPercent += std::max(0, GC.getCivicInfo(kPlayer.getCivics((CivicOptionTypes)iJ)).getImprovementHealthPercentChanges(eImprovement));
								}
							}
							
						}
						else
						{
							iNewBadHealthPercent += GC.getImprovementInfo(eImprovement).getHealthPercent();
							for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
							{
								if (kPlayer.getCivics((CivicOptionTypes)iJ) != NO_CIVIC)
								{
									iNewBadHealthPercent += std::min(0, GC.getCivicInfo(kPlayer.getCivics((CivicOptionTypes)iJ)).getImprovementHealthPercentChanges(eImprovement));
								}
							}
						}
					}
				}
			}
		}
	}

	if ((getImprovementGoodHealth() != iNewGoodHealthPercent) || (getImprovementBadHealth() != iNewBadHealthPercent))
	{
		m_iImprovementGoodHealth = iNewGoodHealthPercent;
		m_iImprovementBadHealth = iNewBadHealthPercent;
		FAssert(getImprovementGoodHealth() >= 0);
		FAssert(getImprovementBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}



int CvCity::getInvasionChance() const
{
	return m_iInvasionChance;
}

void CvCity::changeInvasionChance(int iChange)
{
	m_iInvasionChance += iChange;
}

int CvCity::getInvasionTimer() const
{
	return m_iInvasionTimer;
}

void CvCity::changeInvasionTimer(int iChange)
{
	m_iInvasionTimer += iChange;
}

bool CvCity::isInvaded() const
{
	return getInvasionTimer() > 0;
}

int CvCity::getLandmarkAngerTimer() const
{
	return m_iLandmarkAngerTimer;
}

void CvCity::changeLandmarkAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iLandmarkAngerTimer += iChange;
		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getLandmarkAnger() const
{
	int iAnger = 0;
	if (getLandmarkAngerTimer() > 0)
	{
		iAnger++;
	}
	int iDivisor = std::max(1, GC.getDefineINT("LANDMARK_ANGER_DIVISOR"));
	iDivisor *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getHurryConscriptAngerPercent();
	iDivisor /= 100;
	
	iAnger += getLandmarkAngerTimer() / std::max(1, iDivisor);

	return iAnger;
}

int CvCity::getCivicHappiness() const
{
	return GET_PLAYER(getOwnerINLINE()).getCivicHappiness();
}

bool CvCity::isBuiltFoodProducedUnit() const
{
	return m_bBuiltFoodProducedUnit;
}

void CvCity::setBuiltFoodProducedUnit(bool bNewValue)
{
	m_bBuiltFoodProducedUnit = bNewValue;
}

int CvCity::getBonusCommerceRateModifier(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiBonusCommerceRateModifier[eIndex];
}

void CvCity::changeBonusCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiBonusCommerceRateModifier[eIndex] = (m_aiBonusCommerceRateModifier[eIndex] + iChange);
		FAssert(getCommerceRate(eIndex) >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateCommerceRankCache(eIndex);

		setCommerceModifierDirty(eIndex);
		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getBonusCommerceRateModifier(CommerceTypes eIndex, BonusTypes eBonus) const
{
	int iModifier = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			iModifier += getNumActiveBuilding((BuildingTypes)iI) * GC.getBuildingInfo((BuildingTypes) iI).getBonusCommerceModifier(eBonus, eIndex);
		}
	}

	return iModifier;
}

int CvCity::getBonusCommercePercentChanges(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiBonusCommercePercentChanges[eIndex];
}

void CvCity::changeBonusCommercePercentChanges(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiBonusCommercePercentChanges[eIndex] = (m_aiBonusCommercePercentChanges[eIndex] + iChange);
		FAssert(getCommerceRate(eIndex) >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateCommerceRankCache(eIndex);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getBonusCommercePercentChanges(CommerceTypes eIndex, BonusTypes eBonus) const
{
	int iPercentCommerce = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		//Team Project (5)
		if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
		{
			iPercentCommerce += getNumActiveBuilding((BuildingTypes)iI) * GC.getBuildingInfo((BuildingTypes) iI).getBonusCommercePercentChanges(eBonus, eIndex);
		}
	}

	return iPercentCommerce;
}

int CvCity::getBonusCommercePercentChanges(CommerceTypes eIndex, BuildingTypes eBuilding) const
{
	int iPercentCommerce = 0;

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			//Team Project (5)
			if (!isReligiouslyDisabledBuilding(eBuilding))
			{
				iPercentCommerce += getNumActiveBuilding(eBuilding) * GC.getBuildingInfo(eBuilding).getBonusCommercePercentChanges((BonusTypes)iI, eIndex);
			}
		}
	}

	return iPercentCommerce;
}



int CvCity::getCommerceAttacks(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return std::min(getMaxCommerceAttacks(eIndex), m_aiCommerceAttacks[eIndex]);
}

void CvCity::changeCommerceAttacks(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiCommerceAttacks[eIndex] = (m_aiCommerceAttacks[eIndex] + iChange);
		FAssert(getCommerceRate(eIndex) >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateCommerceRankCache(eIndex);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getMaxCommerceAttacks(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiMaxCommerceAttacks[eIndex];
}

void CvCity::changeMaxCommerceAttacks(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiMaxCommerceAttacks[eIndex] = (m_aiMaxCommerceAttacks[eIndex] + iChange);
	}
}


int CvCity::getUnitClassProductionModifier(UnitClassTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex expected to be < GC.getNumUnitClassInfos()");
	return m_paiUnitClassProductionModifier[eIndex];
}

void CvCity::changeUnitClassProductionModifier(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex expected to be < GC.getNumUnitClassInfos()");
	m_paiUnitClassProductionModifier[eIndex] = (m_paiUnitClassProductionModifier[eIndex] + iChange);
}

int CvCity::getBuildingClassProductionModifier(BuildingClassTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex expected to be < GC.getNumBuildingClassInfos()");
	return m_paiBuildingClassProductionModifier[eIndex];
}

void CvCity::changeBuildingClassProductionModifier(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex expected to be < GC.getNumBuildingClassInfos()");
	m_paiBuildingClassProductionModifier[eIndex] = (m_paiBuildingClassProductionModifier[eIndex] + iChange);
}

int CvCity::getBonusDefenseChanges(BonusTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	return m_paiBonusDefenseChanges[eIndex];
}

void CvCity::changeBonusDefenseChanges(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	m_paiBonusDefenseChanges[eIndex] = (m_paiBonusDefenseChanges[eIndex] + iChange);
}

bool CvCity::hadVicinityBonus(BonusTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	return m_pabHadVicinityBonus[eIndex];
}

bool CvCity::hadRawVicinityBonus(BonusTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	return m_pabHadRawVicinityBonus[eIndex];
}


void CvCity::recalculatePopulationgrowthratepercentage()
{
	PROFILE_FUNC();

	m_fPopulationgrowthratepercentageLog = 0;

	//	Game has been restored from an old save format so we have to calculate
	//	from first principles
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI);
		if (eLoopBuilding != NO_BUILDING)
		{
			if (getNumBuilding(eLoopBuilding) > 0)
			{
				if ( GC.getBuildingInfo(eLoopBuilding).getPopulationgrowthratepercentage() != 0 )
				{
					changePopulationgrowthratepercentage(GC.getBuildingInfo(eLoopBuilding).getPopulationgrowthratepercentage(),true);
				}
			}
		}
	}
}

int CvCity::getPopulationgrowthratepercentage() const
{
	if ( m_fPopulationgrowthratepercentageLog == INVALID_GROWTH_PERCENT_LOG )
	{
		(const_cast<CvCity*>(this))->recalculatePopulationgrowthratepercentage();
	}

	float fMultiplier = exp(m_fPopulationgrowthratepercentageLog);

	return (int)(fMultiplier*100 - 100);
}

void CvCity::changePopulationgrowthratepercentage(int iChange, bool bAdd)
{
	if ( m_fPopulationgrowthratepercentageLog == INVALID_GROWTH_PERCENT_LOG )
	{
		recalculatePopulationgrowthratepercentage();
	}

	float logdiff = (bAdd ? 1 : -1)*log((100+(float)iChange)/100);
	
	m_fPopulationgrowthratepercentageLog += logdiff;
}

void CvCity::doPromotion()
{
	PROFILE_FUNC();

	if (isDisorder())
	{
		return;
	}
	bool hasFreePromofromList = false;

	const int numNumBuildingInfos = GC.getNumBuildingInfos();
	for (int iI = 0; iI < numNumBuildingInfos; iI++)
	{
		const BuildingTypes eBuilding = ((BuildingTypes)iI);
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

		if (kBuilding.isApplyFreePromotionOnMove())
		{
			if (getNumActiveBuilding(eBuilding) > 0)
			{
				if (kBuilding.getNumFreePromoTypes() > 0)
				{
					hasFreePromofromList = true;
				}

				const PromotionTypes ePromotion1 = (PromotionTypes)kBuilding.getFreePromotion();
				const PromotionTypes ePromotion2 = (PromotionTypes)kBuilding.getFreePromotion_2();
				const PromotionTypes ePromotion3 = (PromotionTypes)kBuilding.getFreePromotion_3();

				if (ePromotion1 != NO_PROMOTION || ePromotion2 != NO_PROMOTION || ePromotion3 != NO_PROMOTION || hasFreePromofromList)
				{
					CvPlot* pPlot = plot();
					for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
					{
						CvUnit* pLoopUnit = unitItr.ptr();
						if (GET_TEAM(pLoopUnit->getTeam()).getID() == GET_TEAM(GET_PLAYER(getOwner()).getTeam()).getID())
						{
							assignPromotionChecked(ePromotion1, pLoopUnit);
							assignPromotionChecked(ePromotion2, pLoopUnit);
							assignPromotionChecked(ePromotion3, pLoopUnit);

							if (hasFreePromofromList)
							{
								assignPromotionsFromBuildingChecked(kBuilding, pLoopUnit);
							}
						} //TB SubCombat Mod End
					}
				}
			}
		}
	}
}
/*

*/
bool CvCity::isValidTerrainForBuildings(BuildingTypes eBuilding) const
{
	//This had to be hardcoded, since there is a terrain peak, but it is really a plot type, not a terrain.
	bool bValidTerrain = false;
	//int RequiresAndTerrain = 0;
	bool bRequiresTerrain = false;
	//bool bValidAndTerrain = false;
	int iTerrainPeak = CvTerrainInfo::getTerrainPeak();
	int iTerrainHill = CvTerrainInfo::getTerrainHill();
	bool bPeak = false, bPeak2 = false;
	bool bHill = false, bHill2 = false;
	bool bRequiresOrImprovement = false;
	bool bHasValidImprovement = false;
	bool bRequiresOrFeature = false;
	bool bHasValidFeature = false;

	CvBuildingInfo&	kBuilding = GC.getBuildingInfo(eBuilding);

	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		if (kBuilding.isPrereqOrTerrain(iI))
		{
			bRequiresTerrain = true;
			if (iI == iTerrainPeak)
				bPeak = true;
			else if (iI == iTerrainHill)
				bHill = true;
			for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iJ);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->getTerrainType() == ((TerrainTypes)iI))
					{
						bValidTerrain = true;
						break;
					}
					if (pLoopPlot->isPeak2(true) && bPeak)
					{
						bValidTerrain = true;
						break;
					}
					if (pLoopPlot->isHills() && bHill)
					{
						bValidTerrain = true;
						break;
					}
				}
			}
		}
		
		if (kBuilding.isPrereqAndTerrain(iI))
		{
			bool bHasAndTerrain = false;

			if (iI == iTerrainPeak)
				bPeak2 = true;
			else if (iI == iTerrainHill)
				bHill2 = true;

			//Checks the city plots for a valid terrain
			for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iJ);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->getTerrainType() == ((TerrainTypes)iI))
					{
						bHasAndTerrain = true;
						break;
					}
					if (pLoopPlot->isPeak2(true) && bPeak2)
					{
						bHasAndTerrain = true;
						break;
					}
					if (pLoopPlot->isHills() && bHill2)
					{
						bHasAndTerrain = true;
						break;
					}
				}
			}
			if (!bHasAndTerrain)
			{
				return false;
			}
		}
	}
	
	if (!bValidTerrain && bRequiresTerrain)
	{
		return false;
	}
	
	for (int iI = 0; iI < GC.getNumImprovementInfos() && !bHasValidImprovement; iI++)
	{
		if (kBuilding.isPrereqOrImprovement(iI))
		{
			bRequiresOrImprovement = true;
			for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iJ);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
					{
						if (pLoopPlot->getImprovementType() == (ImprovementTypes)iI)
						{
							bHasValidImprovement = true;
							break;
						}
					}
				}
			}		
		}
	}
	
	if (!bHasValidImprovement && bRequiresOrImprovement)
	{
		return false;
	}
	
	for (int iI = 0; iI < GC.getNumFeatureInfos() && !bHasValidFeature; iI++)
	{
		if (kBuilding.isPrereqOrFeature(iI))
		{
			bRequiresOrFeature = true;
			for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iJ);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->getFeatureType() == iI)
					{
						bHasValidFeature = true;
						break;
					}
				}
			}		
		}
	}
	
	if (!bHasValidFeature && bRequiresOrFeature)
	{
		return false;
	}
	
	return true;
}

void CvCity::changeFreshWater(int iChange)
{
	if (iChange != 0)
	{
		m_iFreshWater += iChange;
		for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
		{
			CvPlot*  pLoopPlot = getCityIndexPlot(iJ);
			if (pLoopPlot != NULL)
			{
				pLoopPlot->updateIrrigated();
			}
		}
		updateFreshWaterHealth();
	}
}

bool CvCity::hasFreshWater() const
{
	return (m_iFreshWater > 0);
}
/*

*/
bool CvCity::canUpgradeUnit(UnitTypes eUnit) const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		if (GC.getUnitInfo(eUnit).getUpgradeUnitClass((UnitClassTypes)iI))
		{
			UnitClassTypes eUpgradeUnitClass = ((UnitClassTypes)iI);
			if ((GC.getGameINLINE().isUnitClassMaxedOut(eUpgradeUnitClass)) || (GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isUnitClassMaxedOut(eUpgradeUnitClass)) || GET_PLAYER(getOwnerINLINE()).isUnitClassMaxedOut(eUpgradeUnitClass))
			{//if the upgrade unitclass is maxed out, I assume you can construct them, and already have construct the max
				return true;
			}
		}
	}
	return false;
}
/*

*/
int CvCity::calculateBonusDefense() const
{
	int iBonusDefense = 0;
	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iBonusDefense += getBonusDefenseChanges((BonusTypes)iI);
		}
	}
	return iBonusDefense;
}

void CvCity::setCivilizationType(int iCiv)
{
	m_iCiv = iCiv;
}

int CvCity::getAdditionalDefenseByBuilding(BuildingTypes eBuilding) const
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	bool bObsolete = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	int iExtraRate = 0;
	int iExtraBuildingRate = 0;

	if (!bObsolete)
	{
		if (kBuilding.getDefenseModifier() != 0)
		{
			//int iCultureDefense = getNaturalDefense() - getBuildingDefense();
			//iExtraRate += std::max(0, kBuilding.getDefenseModifier() - std::max(0, iCultureDefense));
			iExtraBuildingRate += kBuilding.getDefenseModifier();
		}
		for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
		{
			if (hasBonus((BonusTypes)iJ))
			{
				iExtraRate += kBuilding.getBonusDefenseChanges(iJ);
			}
		}
		if (kBuilding.getAllCityDefenseModifier() != 0)
		{
			iExtraRate += kBuilding.getAllCityDefenseModifier();
		}
		
		//if this new building replaces an old one, subtract the old defense rate from the new one.
		BuildingTypes eLoopBuilding;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			eLoopBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex()));
			if (eLoopBuilding != NO_BUILDING)
			{
				if (GC.getBuildingInfo(eLoopBuilding).isReplaceBuildingClass(kBuilding.getBuildingClassType()))
				{
					if (getNumBuilding(eLoopBuilding) > 0)
					{
						iExtraBuildingRate -= GC.getBuildingInfo(eLoopBuilding).getDefenseModifier();
						for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
						{
							if (hasBonus((BonusTypes)iJ))
							{
								iExtraRate -= GC.getBuildingInfo(eLoopBuilding).getBonusDefenseChanges(iJ);
							}
						}
						iExtraRate -= GC.getBuildingInfo(eLoopBuilding).getAllCityDefenseModifier();
					}
				}
			}
		}
	}

	int iOldEffectiveBuildingRate = std::max(getBuildingDefense(), getNaturalDefense());
	int iNewEffectiveBuildingRate = std::max(getBuildingDefense() + iExtraBuildingRate, getNaturalDefense());

	return iExtraRate + (iNewEffectiveBuildingRate - iOldEffectiveBuildingRate);
}

void CvCity::checkBuildings(bool bBonus, bool bCivics, bool bWar, bool bPower, bool bPopulation, bool bAlertOwner)
{
	PROFILE_FUNC();
	
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

		if (eLoopBuilding != NO_BUILDING &&
			!GET_TEAM(getTeam()).isObsoleteBuilding(eLoopBuilding) &&
			(getNumBuilding(eLoopBuilding) > 0 || isDisabledBuilding(eLoopBuilding)))
		{
			bool bRestoreBuildings = false;
			bool bObsoleteBuildings = false;
		
			bool bMissingBonus = false;
			bool bMissingFreshWater = false;
			bool bRequiresCivics = false;
			bool bRequiresWar = false;
			bool bRequiresPower = false;
			bool bRequiresPopulation = false;
			bool bCultureTooHigh = false;

			CvBuildingInfo& kBuilding = GC.getBuildingInfo(eLoopBuilding);
				
			/* Check for Appropriate Resources */
			if (bBonus)
			{
				bool bNeedsBonus = false;
				bool bHasBonus = false;
				bool bHasRawVicinityBonus = false;
				bool bNeedsRawVicinityBonus = false;
				bool bNeedsVicinityBonus = false;
				bool bHasVicinityBonus = false;
				if (kBuilding.getPrereqVicinityBonus() != NO_BONUS)
				{
					bNeedsVicinityBonus = true;
					bHasVicinityBonus = hasVicinityBonus((BonusTypes)kBuilding.getPrereqVicinityBonus());
				}
				if (kBuilding.getPrereqRawVicinityBonus() != NO_BONUS)
				{
					bNeedsRawVicinityBonus = true;
					bHasRawVicinityBonus = hasRawVicinityBonus((BonusTypes)kBuilding.getPrereqRawVicinityBonus());
				}

				if ( !bNeedsVicinityBonus || bHasVicinityBonus )
				{
					bool bHasORVicinityBonus = false;
					bool bNeedsORVicinityBonus = false;

					for (int iJ = 0; iJ < GC.getNUM_BUILDING_PREREQ_OR_BONUSES(); iJ++)
					{
						if (kBuilding.getPrereqOrVicinityBonuses(iJ) != NO_BONUS)
						{
							bNeedsORVicinityBonus = true;
							if (hasVicinityBonus((BonusTypes)kBuilding.getPrereqOrVicinityBonuses(iJ)))
							{
								bHasORVicinityBonus = true;
								break;
							}
						}
					}

					bNeedsVicinityBonus |= bNeedsORVicinityBonus;
					if ( bNeedsORVicinityBonus )
					{
						bHasVicinityBonus = bHasORVicinityBonus;
					}
				}

				if ( !bNeedsRawVicinityBonus || bHasRawVicinityBonus )
				{
					bool bHasORRawVicinityBonus = false;
					bool bNeedsORRawVicinityBonus = false;

					for (int iJ = 0; iJ < GC.getNUM_BUILDING_PREREQ_OR_BONUSES(); iJ++)
					{
						if (kBuilding.getPrereqOrRawVicinityBonuses(iJ) != NO_BONUS)
						{
							bNeedsORRawVicinityBonus = true;
							if (hasRawVicinityBonus((BonusTypes)kBuilding.getPrereqOrRawVicinityBonuses(iJ)))
							{
								bHasORRawVicinityBonus = true;
								break;
							}
						}
					}

					bNeedsRawVicinityBonus |= bNeedsORRawVicinityBonus;
					if ( bNeedsORRawVicinityBonus )
					{
						bHasRawVicinityBonus = bHasORRawVicinityBonus;
					}
				}
						
				//We have the resource, turn on any buildings that are disabled
				if (bNeedsRawVicinityBonus && bHasRawVicinityBonus)
				{
					bRestoreBuildings = true;
				}
				//we lack the nessecary resource, turn off the building
				else if (bNeedsRawVicinityBonus && !bHasRawVicinityBonus)
				{
					bObsoleteBuildings = true;
					bMissingBonus = true;
				}
				if (bNeedsVicinityBonus && bHasVicinityBonus)
				{
					bRestoreBuildings = true;
				}
				//we lack the nessecary resource, turn off the building
				else if (bNeedsVicinityBonus && !bHasVicinityBonus)
				{
					bObsoleteBuildings = true;
					bMissingBonus = true;
				}

				//	Check trade-available resource requirements
				if (kBuilding.getPrereqAndBonus() != NO_BONUS)
				{
					bNeedsBonus = true;
					bHasBonus = hasBonus((BonusTypes)kBuilding.getPrereqAndBonus());
				}
				if ( !bNeedsBonus || bHasBonus )
				{
					bool bHasORBonus = false;
					bool bNeedsORBonus = false;

					for (int iJ = 0; iJ < kBuilding.getNumPrereqOrBonuses(); iJ++)
					{
						if (kBuilding.getPrereqOrBonuses(iJ) != NO_BONUS)
						{
							bNeedsORBonus = true;
							if (hasBonus((BonusTypes)kBuilding.getPrereqOrBonuses(iJ)))
							{
								bHasORBonus = true;
								break;
							}
						}
					}

					bNeedsBonus |= bNeedsORBonus;
					if ( bNeedsORBonus )
					{
						bHasBonus = bHasORBonus;
					}
				}
						
				//We have the resource, turn on any buildings that are disabled
				if (bNeedsBonus && bHasBonus)
				{
					bRestoreBuildings = true;
				}
				//we lack the nessecary resource, turn off the building
				else if (bNeedsBonus && !bHasBonus)
				{
					bObsoleteBuildings = true;
					bMissingBonus = true;
				}
			}
					
			/* Check fresh water */
			if (kBuilding.isFreshWater())
			{
				if ( plot()->isFreshWater() )
				{
					bRestoreBuildings = true;
				}
				else
				{
					bObsoleteBuildings = true;
					bMissingFreshWater = true;
				}
			}
					
			//Check Max Culture
			int iMaxCulture = kBuilding.getMaxCultureLevelAllowed();
			if (iMaxCulture != -1)
			{
				if (iMaxCulture < getCultureLevel())
				{
					bObsoleteBuildings = true;
					bCultureTooHigh = true;
				}
				else
				{
					bRestoreBuildings = true;
				}
			}

			/* Check War Conditions */
			if (bWar)
			{
				//Not at war
				if (kBuilding.isPrereqWar())
				{
					if (GET_TEAM(getTeam()).getAtWarCount(true) > 0)
					{
						bRestoreBuildings = true;
					}
					else
					{
						bObsoleteBuildings = true;
						bRequiresWar = true;
					}
				}
			}
					
			/* Check Civic Requirements */
			if (bCivics)
			{
				if (kBuilding.isRequiresActiveCivics())
				{
					if (GET_PLAYER(getOwnerINLINE()).hasValidCivics(eLoopBuilding))
					{
						bRestoreBuildings = true;
					}
					else
					{
						bObsoleteBuildings = true;
						bRequiresCivics = true;
					}
				}
			}
					
			/*Check Elecricity Requirements */
			if (bPower)
			{
				if (kBuilding.isPrereqPower())
				{
					if (isPower())
					{
						bRestoreBuildings = true;
					}
					else
					{
						bObsoleteBuildings = true;
						bRequiresPower = true;
					}
				}
			}
					
			/* Check The Employed Population */
			//Team Project (5)
			if (!isReligiouslyDisabledBuilding(eLoopBuilding))
			{
				if (bPopulation)
				{
					if (visiblePopulation() < 0)
					{
						if (kBuilding.getNumPopulationEmployed() > 0)
						{
							//Try and re-assign work before turning off the building
							AI_assignWorkingPlots();

							if (visiblePopulation() < 0)
							{
								bObsoleteBuildings = true;
								bRequiresPopulation = true;
							}
						}
					}
					else if (visiblePopulation() - kBuilding.getNumPopulationEmployed() >= 0)
					{
						if (kBuilding.getNumPopulationEmployed() > 0)
						{
							bRestoreBuildings = true;
						}
					}
				}
			}
					
			/* Alert the Player */
			CvWString szBuffer;
			if (bRestoreBuildings && !bObsoleteBuildings && isDisabledBuilding(eLoopBuilding))
			{
				setDisabledBuilding(eLoopBuilding, false);

				if ( bAlertOwner )
				{
					if (!GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS))
					{
						MEMORY_TRACK_EXEMPT();

						szBuffer = gDLL->getText("TXT_KEY_CITY_RESTORED_BUILDINGS", getNameKey(), kBuilding.getDescription(), kBuilding.getDescription());
						AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, kBuilding.getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
					}
				}
			}
			else if (bObsoleteBuildings && !isDisabledBuilding(eLoopBuilding))
			{
				setDisabledBuilding(eLoopBuilding, true);

				if ( bAlertOwner )
				{
					if (bMissingBonus)
						szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_RESOURCES", kBuilding.getDescription(), getNameKey(), kBuilding.getDescription());
					else if (bRequiresWar)
						szBuffer = gDLL->getText("TXT_KEY_REMOVED_BUILDINGS_WARTIME", kBuilding.getDescription(), getNameKey());
					else if (bRequiresCivics)
						szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_CIVICS", kBuilding.getDescription(), getNameKey(), kBuilding.getDescription());
					else if (bRequiresPower)
						szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_POWER", kBuilding.getDescription(), getNameKey());
					else if (bRequiresPopulation)
						szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_POPULATION", kBuilding.getDescription(), getNameKey());
					else if (bMissingFreshWater)
						szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_FRESH_WATER", kBuilding.getDescription(), getNameKey());
					else if (bCultureTooHigh)
						szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_MAX_CULTURE", kBuilding.getDescription(), getNameKey());
					else
						FAssert(false);
						szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_RESOURCES", kBuilding.getDescription(), getNameKey(), kBuilding.getDescription());
					if (!GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS))
					{
						MEMORY_TRACK_EXEMPT();

						AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, kBuilding.getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
					}
				}
			}
		}
	}
}



int CvCity::calculateCorporationHealth() const
{
	int iHealth = 0;
	
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isActiveCorporation((CorporationTypes)iI))
		{
			iHealth += GC.getCorporationInfo((CorporationTypes)iI).getHealth();
		}
	}
	return iHealth;
}

int CvCity::calculateCorporationHappiness() const
{
	int iHappiness = 0;
	
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isActiveCorporation((CorporationTypes)iI))
		{
			iHappiness += GC.getCorporationInfo((CorporationTypes)iI).getHappiness();
		}
	}
	return iHappiness;
}

int CvCity::getExtraYieldTurns() const
{
	return m_iExtraYieldTurns;
}

void CvCity::changeExtraYieldTurns(int iChange)
{
	int iOldVal = m_iExtraYieldTurns;
	CvPlot* pPlot = GC.getMapINLINE().plotINLINE(getX(), getY());
	
	m_iExtraYieldTurns += iChange;
	
	if (iOldVal == -1 && iChange == 1)
	{
		changeBaseYieldRate(YIELD_FOOD, -1);
		pPlot->updateSymbols();
	}
	else if (iOldVal == 1 && iChange == -1)
	{
		changeBaseYieldRate(YIELD_PRODUCTION, -1);
		pPlot->updateSymbols();
	}
	
}

int CvCity::getLineOfSight() const
{
	return m_iLineOfSight;
}

void CvCity::changeLineOfSight(int iChange)
{
	m_iLineOfSight += iChange;
}

void CvCity::setExtraYieldTurns(int iNewVal)
{
	m_iExtraYieldTurns = iNewVal;
	CvPlot* pPlot = GC.getMapINLINE().plotINLINE(getX(), getY());
	
	if (iNewVal > 0)
	{
		changeBaseYieldRate(YIELD_PRODUCTION, 1);
		pPlot->updateSymbols();
	}
	else if (iNewVal < 0)
	{
		changeBaseYieldRate(YIELD_FOOD, 1);
		pPlot->updateSymbols();
	}
}

BuildTypes CvCity::findChopBuild(FeatureTypes eFeature)
{
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		CvBuildInfo& kBuild = GC.getBuildInfo((BuildTypes)iI);
		if (kBuild.getImprovement() == NO_IMPROVEMENT)
		{
			if (kBuild.isFeatureRemove(eFeature) &&
				kBuild.getFeatureProduction(eFeature) != 0 &&
				GET_TEAM(getTeam()).isHasTech((TechTypes)kBuild.getTechPrereq()) &&
				(kBuild.getObsoleteTech() == NO_TECH || !GET_TEAM(getTeam()).isHasTech((TechTypes)kBuild.getObsoleteTech())))
			{
				return (BuildTypes)iI;
			}
		}
	}
	
	return NO_BUILD;
}

CultureLevelTypes CvCity::getOccupationCultureLevel() const
{
	return m_eOccupationCultureLevel;
}


void CvCity::setOccupationCultureLevel(CultureLevelTypes eNewValue)
{
	PROFILE_FUNC();

	int eOldValue = getOccupationCultureLevel();

	if (eOldValue != eNewValue)
	{
		m_eOccupationCultureLevel = eNewValue;

		if (eOldValue != NO_CULTURELEVEL)
		{
			clearCultureDistanceCache();
			for (int iDX = -eOldValue; iDX <= eOldValue; iDX++)
			{
				for (int iDY = -eOldValue; iDY <= eOldValue; iDY++)
				{
					int iCultureRange = cultureDistance(iDX, iDY);

					if (iCultureRange > getOccupationCultureLevel())
					{
						if (iCultureRange <= eOldValue)
						{
							FAssert(iCultureRange <= GC.getNumCultureLevelInfos());

							CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								pLoopPlot->changeOccupationCultureRangeCities(getOwnerINLINE(), -1);
							}
						}
					}
				}
			}
		}

		if (getOccupationCultureLevel() != NO_CULTURELEVEL)
		{
			for (int iDX = -getOccupationCultureLevel(); iDX <= getOccupationCultureLevel(); iDX++)
			{
				for (int iDY = -getOccupationCultureLevel(); iDY <= getOccupationCultureLevel(); iDY++)
				{
/************************************************************************************************/
/* phunny_pharmer             Start		 05/01/10                                               */
/*   occupation culture doesn't play well with caching, so just use the plot distance           */
/************************************************************************************************/
					int iCultureRange = cultureDistance(iDX, iDY, true);
//					iCultureRange = plotDistance(0, 0, iDX, iDY);
/************************************************************************************************/
/* phunny_pharmer             END                                                               */
/************************************************************************************************/

					if (iCultureRange > eOldValue)
					{
						if (iCultureRange <= getOccupationCultureLevel())
						{
							FAssert(iCultureRange <= GC.getNumCultureLevelInfos());

							CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								if (pLoopPlot->isPotentialCityWorkForArea(area()))
								{
									pLoopPlot->changeCulture(getOwnerINLINE(), 1, false);
								}
								pLoopPlot->changeOccupationCultureRangeCities(getOwnerINLINE(), 1);
							}
						}
					}
				}
			}
		}
	}
}

CultureLevelTypes CvCity::getMaxCultureLevelAmongPlayers() const
{
	int iMaxCulture = getCultureTimes100((PlayerTypes)0);

	for (int iI = 1; iI < MAX_PLAYERS; iI++)
	{
		if (getCultureTimes100((PlayerTypes)iI) > iMaxCulture) 
		{
			iMaxCulture = getCultureTimes100((PlayerTypes)iI);
		}
	}


	return getCultureLevelForCulture(iMaxCulture);
}


CultureLevelTypes CvCity::getCultureLevel(PlayerTypes eIndex) const
{
	return getCultureLevelForCulture(getCultureTimes100(eIndex));
}


CultureLevelTypes CvCity::getCultureLevelForCulture(int iCulture) const
{
	CultureLevelTypes eCultureLevel = ((CultureLevelTypes)0);

	for (int iI = (GC.getNumCultureLevelInfos() - 1); iI > 0; iI--)
	{
		if (iCulture >= 100 * GC.getCultureLevelInfo((CultureLevelTypes)iI).getSpeedThreshold(GC.getGameINLINE().getGameSpeedType()))
		{
			eCultureLevel = ((CultureLevelTypes)iI);
			break;
		}
	}

	return eCultureLevel;
}

int CvCity::calculateBonusCommerceRateModifier(CommerceTypes eIndex) const
{
	int iMultiplier = 0;
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMultiplier += GET_PLAYER(getOwnerINLINE()).getBonusCommerceModifier((BonusTypes)iI, eIndex);//from Player level
		}
	}
	iMultiplier += getBonusCommerceRateModifier(eIndex);//local from buildings
	return iMultiplier;
}

static bool bonusAvailableFromBuildings(BonusTypes eBonus)
{
	static bool* bBonusAvailability = NULL;

	FAssert( 0 <= eBonus );
	FAssert( GC.getNumBonusInfos() > eBonus );

	if ( bBonusAvailability == NULL )
	{
		bBonusAvailability = new bool[GC.getNumBonusInfos()];

		for(int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			bBonusAvailability[iI] = false;
		}

		for(int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			for(int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				if ( GC.getBuildingInfo((BuildingTypes)iJ).getFreeBonus() == iI || GC.getBuildingInfo((BuildingTypes)iJ).hasExtraFreeBonus((BonusTypes)iI) )
				{
					bBonusAvailability[iI] = true;
					break;
				}
			}
		}
	}

	return bBonusAvailability[eBonus];
}

void CvCity::clearVicinityBonusCache(BonusTypes eBonus)
{
	if ( m_pabHasVicinityBonusCached != NULL )
	{
		m_pabHasVicinityBonusCached[eBonus] = false;
	}
}

bool CvCity::hasVicinityBonus(BonusTypes eBonus) const
{
	PROFILE_FUNC();
	if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		if ( m_pabHasVicinityBonusCached == NULL)
		{
			m_pabHasVicinityBonusCached = new bool[GC.getNumBonusInfos()];
			SAFE_DELETE_ARRAY(m_pabHasVicinityBonus);
			m_pabHasVicinityBonus = new bool[GC.getNumBonusInfos()];

			for(int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				m_pabHasVicinityBonusCached[iI] = false;
			}
		}

		if ( m_pabHasVicinityBonusCached[eBonus] )
		{
			return m_pabHasVicinityBonus[eBonus];
		}
	}

	bool bResult = false;

	//No sense in checking...
	if (hasBonus(eBonus))
	{
		if (plot()->getBonusType() == eBonus)
		{
			bResult = true;
		}
		
		if ( !bResult )
		{
			for (int iI = 0; iI < getNumCityPlots(); iI++)
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->getBonusType() == eBonus)
					{
						if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
						{
							if (pLoopPlot->isHasValidBonus() && pLoopPlot->isConnectedTo(this))
							{
								bResult = true;
								break;
							}
						}
					}
				}
			}
		}

		if (!bResult)
		{
			if ( bonusAvailableFromBuildings(eBonus) )
			{
				for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iI);

					if ( kBuilding.getFreeBonus() == eBonus || kBuilding.hasExtraFreeBonus(eBonus) )
					{
						if ( getNumActiveBuilding((BuildingTypes)iI) > 0 )
						{
							bResult = true;
							break;
						}
					}
				}
			}
		}
	}
	
	if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		m_pabHasVicinityBonus[eBonus] = bResult;
		m_pabHasVicinityBonusCached[eBonus] = true;
	}

	return bResult;
}

void CvCity::clearRawVicinityBonusCache(BonusTypes eBonus)
{
	if ( m_pabHasRawVicinityBonusCached != NULL )
	{
		m_pabHasRawVicinityBonusCached[eBonus] = false;
	}
}

bool CvCity::hasRawVicinityBonus(BonusTypes eBonus) const
{
	PROFILE_FUNC();
	if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		if ( m_pabHasRawVicinityBonusCached == NULL)
		{
			m_pabHasRawVicinityBonusCached = new bool[GC.getNumBonusInfos()];
			SAFE_DELETE_ARRAY(m_pabHasRawVicinityBonus);
			m_pabHasRawVicinityBonus = new bool[GC.getNumBonusInfos()];

			for(int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				m_pabHasRawVicinityBonusCached[iI] = false;
			}
		}

		if ( m_pabHasRawVicinityBonusCached[eBonus] )
		{
			return m_pabHasRawVicinityBonus[eBonus];
		}
	}

	bool bResult = false;

	//No sense in checking...
	if (plot()->getBonusType() == eBonus)
	{
		bResult = true;
	}
	
	if ( !bResult )
	{
		for (int iI = 0; iI < getNumCityPlots(); iI++)
		{
			CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->getBonusType() == eBonus)
				{
					if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
					{
						if (pLoopPlot->getBonusType() == eBonus)
						{
							bResult = true;
							break;
						}
					}
				}
			}
		}
	}

	if (!bResult)
	{
		if ( bonusAvailableFromBuildings(eBonus) )
		{
			for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
			{
				CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iI);

				if ( kBuilding.getFreeBonus() == eBonus || kBuilding.hasExtraFreeBonus(eBonus) )
				{
					if ( getNumActiveBuilding((BuildingTypes)iI) > 0 )
					{
						bResult = true;
						break;
					}
				}
			}
		}
	}
	
	if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		m_pabHasRawVicinityBonus[eBonus] = bResult;
		m_pabHasRawVicinityBonusCached[eBonus] = true;
	}

	return bResult;
}

bool CvCity::isDevelopingCity() const
{
	int iNumCities = GET_PLAYER(getOwnerINLINE()).getNumCities();
	int iPopulation = 0;
	bool bDevelopingCity = false;
	if (findPopulationRank() >= ((iNumCities * 2) / 3))
	{
		iPopulation = getPopulation();
		int iEmpirePop = GET_PLAYER(getOwnerINLINE()).getTotalPopulation();
		int iAvgPop = iEmpirePop / iNumCities;
		if (iPopulation < ((iAvgPop * 2) / 3))
		{
			bDevelopingCity = true;
		}
	}
	if (iNumCities < 3)
	{
		if (iPopulation <= 2)
		{
			if (!isCapital())
			{
				bDevelopingCity = true;
			}
		}
	}
	return bDevelopingCity;
}

void CvCity::doVicinityBonus()
{
	PROFILE_FUNC();
	
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		//	Clear the cache each turn before performing this calculation
		clearVicinityBonusCache((BonusTypes)iI);
		clearRawVicinityBonusCache((BonusTypes)iI);

		int iChange = 0;
		bool bHadVicinityBonus = hadVicinityBonus((BonusTypes)iI);
		bool bHasVicinityBonus = hasVicinityBonus((BonusTypes)iI);
		if (bHadVicinityBonus && !bHasVicinityBonus)
		{
			iChange = -1;
		}
		else if (bHasVicinityBonus && !bHadVicinityBonus)
		{
			iChange = 1;
		}
		if (iChange != 0)
		{
			for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				const CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iJ);

				if (kBuilding.getVicinityBonusYieldChanges(NO_BONUS, NO_YIELD) != 0 && getNumRealBuilding((BuildingTypes)iJ) > 0)
				{
					for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
					{
						int iYieldChange = kBuilding.getVicinityBonusYieldChanges((BonusTypes)iI, iK);

						if ( iYieldChange != 0 )
						{
							updateYieldRate((BuildingTypes)iJ, (YieldTypes)iK, ((getBuildingYieldChange((BuildingClassTypes)(kBuilding.getBuildingClassType()), (YieldTypes)iK)) + (iYieldChange * iChange)));
						}
					}
				}
			}
		}
	}
}
bool CvCity::isDisabledBuilding(BuildingTypes eIndex) const													 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_pabDisabledBuilding[eIndex];
}

void CvCity::setDisabledBuilding(BuildingTypes eIndex, bool bNewValue)
{
	//bool bOldValue = isDisabledBuilding(eIndex);

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	if (isDisabledBuilding(eIndex) != bNewValue)
	{
		m_pabDisabledBuilding[eIndex] = bNewValue;
		
		processBuilding(eIndex, bNewValue ? -1 : 1);
	}
}

//Team Project (5)
bool CvCity::isReligiouslyDisabledBuilding(BuildingTypes eIndex) const													 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	if (!GC.getGame().isOption(GAMEOPTION_RELIGIOUS_DISABLING))
	{
		return false;
	}
	return m_pabReligiouslyDisabledBuilding[eIndex];
}

void CvCity::setReligiouslyDisabledBuilding(BuildingTypes eIndex, bool bNewValue)
{
	bool bOldValue = isReligiouslyDisabledBuilding(eIndex);
	ReligionTypes eReligion = (ReligionTypes)GC.getBuildingInfo(eIndex).getReligionType();
	if ( eReligion == NO_RELIGION )
	{
		eReligion = (ReligionTypes)GC.getBuildingInfo(eIndex).getPrereqReligion();
	}

	FAssert(eReligion != NO_RELIGION);
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	if (bOldValue != bNewValue)
	{
		m_pabReligiouslyDisabledBuilding[eIndex] = bNewValue;
		
		processBuilding(eIndex, bNewValue ? -1 : 1, false, false, true);
		
		if (!bNewValue)
		{
			if (/*!GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS) &&*/ GET_PLAYER(getOwnerINLINE()).isHuman())
			{
				MEMORY_TRACK_EXEMPT();

				CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_RESTORED_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eIndex).getDescription());
				AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
			}
		}
		else
		{
			if (/*!GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS) && */GET_PLAYER(getOwnerINLINE()).isHuman())
			{
				MEMORY_TRACK_EXEMPT();

				CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_DISABLED_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eIndex).getDescription());
				AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
			}
		}
	}
}

int CvCity::getUnitCombatExtraStrength(UnitCombatTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatExtraStrength[eIndex];
}


void CvCity::changeUnitCombatExtraStrength(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatExtraStrength[eIndex] = (m_paiUnitCombatExtraStrength[eIndex] + iChange);
}

void CvCity::doInvasion()
{
	PROFILE_FUNC();
	
	PlayerTypes ePlayer = NO_PLAYER;
	if (!isInvaded())
	{
		bool bTestInvasion = false;
		if (getInvasionChance() > 0)
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)iI);
				if (pAdjacentPlot != NULL && !bTestInvasion)
				{
					for (CvPlot::unit_iterator unitItr = pAdjacentPlot->beginUnits(); unitItr != pAdjacentPlot->endUnits(); ++unitItr)
					{
						if (GET_TEAM(unitItr->getTeam()).isAtWar(getTeam()) && !unitItr->isAnimal() && !unitItr->isOnlyDefensive() && !unitItr->canAttackOnlyCities())
						{
							bTestInvasion = true;
							ePlayer = unitItr->getOwnerINLINE();
							break;
						}
					}
				}
			}
		}
		if (bTestInvasion)
		{
			if (GC.getGameINLINE().getSorenRandNum(100, "Enemy Invades City Chance") < getInvasionChance())
			{
				int iTurns = getInvasionChance() / 2;
				changeInvasionTimer(iTurns);
				//Alert the Player
				if (GET_PLAYER(getOwnerINLINE()).isHuman())
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_INVASION);
					pInfo->setData1(getID());
					gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE());
				}
				//Alert the invader
				if (GET_PLAYER(ePlayer).isHuman())
				{
					MEMORY_TRACK_EXEMPT();

					CvWString szBuffer = gDLL->getText("TXT_KEY_INVASION_SUCCESSFUL", getNameKey());
					AddDLLMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BARRACKS", MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
				}
			}
		}
	}
	else
	{
		changeInvasionTimer(-1);
		if (!isInvaded())
		{
			MEMORY_TRACK_EXEMPT();

			CvWString szBuffer = gDLL->getText("TXT_KEY_INVASION_ENDED", getNameKey());
			AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BARRACKS", MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
		}
	}
}

bool CvCity::isZoneOfControl() const
{
	return (m_iZoCCount > 0);
}

void CvCity::changeZoCCount(short iChange)
{	
	if(iChange != 0)
	{
		m_iZoCCount = std::max(0, (m_iZoCCount + iChange));
	}
}

int CvCity::getAdjacentDamagePercent() const
{
	return m_iAdjacentDamagePercent;
}

void CvCity::changeAdjacentDamagePercent(int iChange)
{
	m_iAdjacentDamagePercent += iChange;
}

int CvCity::getWorkableRadiusOverride() const
{
	return m_iWorkableRadiusOverride;
}

void CvCity::setWorkableRadiusOverride(int iNewVal)
{
	m_iWorkableRadiusOverride = iNewVal;
}

bool CvCity::isProtectedCulture() const
{
	return getProtectedCultureCount() > 0;
}

int CvCity::getProtectedCultureCount() const
{
	return m_iProtectedCultureCount;
}

void CvCity::changeProtectedCultureCount(int iChange)
{
	m_iProtectedCultureCount += iChange;
}

int CvCity::getNumUnitFullHeal() const
{
	return m_iNumUnitFullHeal;
}

void CvCity::changeNumUnitFullHeal(int iChange)
{
	m_iNumUnitFullHeal += iChange;
}


void CvCity::doAttack()
{
	PROFILE_FUNC();
	
	if (getAdjacentDamagePercent() > 0)
	{
		if (GET_TEAM(getTeam()).getAtWarCount(false) > 0)
		{
			bool abInformPlayer[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				abInformPlayer[iI] = false;
			}
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
				if (pAdjacentPlot != NULL)
				{
					for (CvPlot::unit_iterator unitItr = pAdjacentPlot->beginUnits(); unitItr != pAdjacentPlot->endUnits(); ++unitItr)
					{
						CvUnit* pLoopUnit = unitItr.ptr();
						
						if (pLoopUnit->getTeam() != getTeam())
						{
							if (GET_TEAM(getTeam()).isAtWar(pLoopUnit->getTeam()))
							{
								//	Koshling - changed city defenses to have a 1-in-4 chance of damaging each unit each turn
								if (pLoopUnit->baseCombatStr() && GC.getGameINLINE().getSorenRandNum(4,"City adjacent damage") == 0)
								{
									int iDamage = pLoopUnit->currHitPoints();
									iDamage *= getAdjacentDamagePercent();
									iDamage /= 100;
									
									pLoopUnit->changeDamage(iDamage, getOwnerINLINE());
									if (!abInformPlayer[pLoopUnit->getOwnerINLINE()])
									{
										MEMORY_TRACK_EXEMPT();

										abInformPlayer[pLoopUnit->getOwnerINLINE()] = true;
										CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_UNITS_DAMAGED", getNameKey());
										AddDLLMessage(pLoopUnit->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_INFO, pLoopUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopUnit->getX_INLINE(), pLoopUnit->getY_INLINE(), true, true);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CvCity::doHeal()
{
	PROFILE_FUNC();
	
	if (getNumUnitFullHeal() > 0)
	{
		int iNumHeals = getNumUnitFullHeal();
		int iNumDamagedUnits = 0;

		CvPlot* pPlot = plot();
		for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
		{
			if (unitItr->getTeam() == getTeam())
			{
				if (unitItr->getDamage() > 0)
				{
					iNumDamagedUnits++;
				}
			}
		}
		
		iNumHeals = std::min(iNumHeals, iNumDamagedUnits);
		while (iNumHeals > 0)
		{
			for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
			{
				if (unitItr->getTeam() == getTeam())
				{
					if (unitItr->getDamage() > 0)
					{
						if (GC.getGameINLINE().getSorenRandNum(100, "Unit Full Heals") > 50)
						{
							unitItr->setDamage(0, getOwnerINLINE(), false);
							iNumHeals--;
							//Recheck condition after each heal
							break;
						}
					}
				}
			}
		}
	}
}

void CvCity::decayCommerce()
{
	if (GC.getGameINLINE().getElapsedGameTurns() % std::max(1, GC.getDefineINT("COMMERCE_ATTACKS_FADE_RATE")))
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			if (getCommerceAttacks((CommerceTypes)iI) > 0)
			{
				changeCommerceAttacks((CommerceTypes)iI, -1);
			}
			else if (getCommerceAttacks((CommerceTypes)iI) < 0)
			{
				changeCommerceAttacks((CommerceTypes)iI, 1);
			}
		}
	}
}

void CvCity::doCorporation()
{
	PROFILE_FUNC();
	
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	if (!GC.getGameINLINE().isOption(GAMEOPTION_REALISTIC_CORPORATIONS))
	{
		return;
	}

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (GC.getGameINLINE().getHeadquarters((CorporationTypes)iI) != NULL)
		{
			if (!isHasCorporation((CorporationTypes)iI) && GC.getGameINLINE().canEverSpread((CorporationTypes)iI))
			{
				if (!kOwner.isNoCorporations() && (!kOwner.isNoForeignCorporations() || GC.getGameINLINE().getHeadquarters((CorporationTypes)iI)->getOwner() == getOwner()))
				{
					int iRandThreshold = 0;
					for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iJ);
						if (kPlayer.isAlive())
						{
							int iLoop = 0;
							for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
							{
								if (pLoopCity->isConnectedTo(this))
								{
									int iSpread = pLoopCity->getCorporationInfluence((CorporationTypes)iI);
									
									iSpread *= GC.getCorporationInfo((CorporationTypes)iI).getSpread();
									
									iSpread /= 100;

									if (kPlayer.getID() != kOwner.getID())
									{
										if (GET_TEAM(kPlayer.getTeam()).isFreeTradeAgreement(kOwner.getTeam()))
										{
											iSpread *= (100 + GC.getDefineINT("FREE_TRADE_CORPORATION_SPREAD_MOD"));
											iSpread /= 100;
										}
									}
									
									if (iSpread > 0)
									{
										iSpread /= std::max(1, (((GC.getDefineINT("CORPORATION_SPREAD_DISTANCE_DIVISOR") * plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE())) / GC.getMapINLINE().maxPlotDistance()) - 5));
										
										iRandThreshold = std::max(iRandThreshold, iSpread);
									}
								}
							}
						}
					}
					
					iRandThreshold *= kOwner.getCorporationSpreadModifier() + 100;
					iRandThreshold /= 100;
					iRandThreshold *= kOwner.getCorporationInfluence((CorporationTypes)iI);
					iRandThreshold /= 100;
					
					iRandThreshold /= (1 + (getCorporationCount() / 2));
					
					logBBAI("  City (%S) Has Rand Threshold of %d for Corporation %S", getName().GetCString(), iRandThreshold, GC.getCorporationInfo((CorporationTypes)iI).getDescription());
					int iRand = GC.getDefineINT("CORPORATION_SPREAD_RAND");
					iRand *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
					iRand /= 100;
					iRand = GC.getGameINLINE().getSorenRandNum(iRand, "Corporation Spread");
					if (iRand < iRandThreshold)
					{
						//Remove Hostile Corporations
						for (int iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
						{
							if (iI != iJ)
							{
								if (GC.getGameINLINE().isCompetingCorporation((CorporationTypes)iJ, (CorporationTypes)iI))
								{
									if (isActiveCorporation((CorporationTypes)iJ))
									{
										MEMORY_TRACK_EXEMPT();

										setHasCorporation((CorporationTypes)iJ, false, false, false);
										CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_HOSTILE_TAKEOVER", GC.getCorporationInfo((CorporationTypes)iJ).getTextKeyWide(), GC.getCorporationInfo((CorporationTypes)iI).getTextKeyWide(), getNameKey());
										AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getCorporationInfo((CorporationTypes)iJ).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getCorporationInfo((CorporationTypes)iJ).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), false, false);
									}
								}
							}
						}
						setHasCorporation((CorporationTypes)iI, true, true, false);
						break;
					}
				}
			}
			//Decay
			else
			{
				//TODO: Should HQ ever relocate?
				if (this != GC.getGameINLINE().getHeadquarters((CorporationTypes)iI))
				{
					int iSpread = GC.getCorporationInfo((CorporationTypes)iI).getSpread();
					iSpread *= getCorporationInfluence((CorporationTypes)iI);
					iSpread /= 100;
					iSpread *= kOwner.getCorporationInfluence((CorporationTypes)iI);
					iSpread /= 100;
					
					int iAverageSpread = GC.getGameINLINE().getAverageCorporationInfluence(this, (CorporationTypes)iI);
					int iDiff = iAverageSpread - iSpread;
					//Our influence is lower than average
					if (iDiff > 0)
					{
						int iRand = GC.getDefineINT("CORPORATION_SPREAD_RAND");
						iRand *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
						iRand /= 100;
						if (GC.getGameINLINE().getSorenRandNum(iRand, "Corporation Decay") < iDiff)
						{
							setHasCorporation((CorporationTypes)iI, false, true, false);
							break;
						}
					}
				}
			}
		}
	}
}

int CvCity::getCorporationInfluence(CorporationTypes eCorporation) const
{
	int iInfluence = 100;
	
	int iBonusesConsumed = 0;
	int iNumAvailBonuses = 0;
	//Influence scales based on the number of resources a corporation consumes
	for (int iI = 0; iI < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++iI)
	{
		BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(iI);
		if (eBonus != NO_BONUS)
		{
			iBonusesConsumed++;
			iNumAvailBonuses += getNumBonuses(eBonus);
		}
	}
	
	if (iNumAvailBonuses > 0)
	{
		iInfluence += iNumAvailBonuses;
	}
	else
	{
		if (iNumAvailBonuses == 0 && iBonusesConsumed > 0)
		{
			return 0;
		}
	}
	
	if (iBonusesConsumed > 0)
	{
		for (int iI = 0; iI < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++iI)
		{
			BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(iI);
			if (eBonus != NO_BONUS)
			{
				if (hasBonus(eBonus))
				{
					iInfluence += (GC.getDefineINT("CORPORATION_RESOURCE_BASE_INFLUENCE") / iBonusesConsumed);
				}
			}
		}
	}
	
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (iI != eCorporation)
		{
			if (GC.getGameINLINE().isCompetingCorporation(eCorporation, (CorporationTypes)iI))
			{
				if (isActiveCorporation((CorporationTypes)iI))
				{
					iInfluence /= 10;
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (iI != eCorporation)
		{
			if (GC.getGameINLINE().isCompetingCorporation(eCorporation, (CorporationTypes)iI))
			{
				if (isActiveCorporation((CorporationTypes)iI))
				{
					if (GC.getGameINLINE().getHeadquarters((CorporationTypes)iI) == this)
					{
						return 0;
					}
				}
			}
		}
	}
	int iAveragePopulation = GC.getGame().getTotalPopulation();
	iAveragePopulation /= std::max(1, GC.getGame().getNumCivCities());
	if (iAveragePopulation > 0)
	{
		iInfluence *= getPopulation();
		iInfluence /= iAveragePopulation;
	}
	return iInfluence;
}

int CvCity::calculateCorporateTaxes() const
{
	PROFILE_FUNC();
	
	int iTaxes = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isActiveCorporation((CorporationTypes)iI) && GET_PLAYER(getOwnerINLINE()).isActiveCorporation((CorporationTypes)iI))
		{
			CorporationTypes eCorporation = (CorporationTypes)iI;


			for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
			{
				iTaxes += 100 * GC.getCorporationInfo(eCorporation).getHeadquarterCommerce(iCommerce);
			}

			int iNumBonuses = 0;
			for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
			{
				BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(i);
				if (NO_BONUS != eBonus)
				{
					iNumBonuses += getNumBonuses(eBonus);
				}
			}

			int iBonusTaxes = GC.getCorporationInfo(eCorporation).getMaintenance() * iNumBonuses;
			iBonusTaxes *= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getCorporationMaintenancePercent();
			iBonusTaxes /= 200;
			iTaxes += iBonusTaxes;

			int iAveragePopulation = GC.getGame().getTotalPopulation();
			iAveragePopulation /= std::max(1, GC.getGame().getNumCivCities());
			if (iAveragePopulation > 0)
			{
				iTaxes *= getPopulation();
				iTaxes /= iAveragePopulation;
			}

			iTaxes *= std::min(0, (GET_PLAYER(getOwnerINLINE()).getCorporationMaintenanceModifier() + 100));
			iTaxes /= 100;
			iTaxes = abs(iTaxes);

			int iInflation = GET_PLAYER(getOwnerINLINE()).calculateInflationRate() + 100;
			if (iInflation > 0)
			{
				iTaxes *= 100;
				iTaxes /= iInflation;
			}
			
			iTaxes /= (1 + GET_PLAYER(getOwnerINLINE()).getHandicapType() / 2);
		}
	}
			
	return iTaxes / 100;
}

int CvCity::getDisabledPowerTimer() const
{
	return m_iDisabledPowerTimer;
}

void CvCity::changeDisabledPowerTimer(int iChange)
{
	m_iDisabledPowerTimer += iChange;
}

int CvCity::getWarWearinessTimer() const
{
	return m_iWarWearinessTimer;
}

void CvCity::changeWarWearinessTimer(int iChange)
{
	m_iWarWearinessTimer += iChange;
}

void CvCity::doDisabledPower()
{
	if (getDisabledPowerTimer() > 0)
	{
		changeDisabledPowerTimer(-1);
		if (getDisabledPowerTimer() == 0)
		{
			MEMORY_TRACK_EXEMPT();

			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_POWER_RESTORED", getNameKey());
			AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), false, false);
		}
	}
}

void CvCity::doWarWeariness()
{
	if (getWarWearinessTimer() > 0)
	{
		changeWarWearinessTimer(-20);
	}
	if (getEventAnger() > 0)
	{
		int iTurnCheck = 10;
		iTurnCheck *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent();
		iTurnCheck /= 100;
		if (GC.getGameINLINE().getElapsedGameTurns() % iTurnCheck == 0)
		{
			changeEventAnger(-1);
		}
	}
}

int CvCity::getEventAnger() const
{
	return m_iEventAnger;
}

void CvCity::changeEventAnger(int iChange)
{
	if (iChange != 0)
	{
		m_iEventAnger += iChange;
		FAssert(getEventAnger() >= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getNonHolyReligionCount() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isHasReligion((ReligionTypes)iI))
		{
			if (!isHolyCity((ReligionTypes)iI))
			{
				iCount++;
			}
		}
	}

	return iCount;
}

void CvCity::calculateExtraTradeRouteProfit(int iExtra, int* &aiTradeYields) const
{
	PROFILE_FUNC();
	
	int iMaxTradeRoutes = GC.getDefineINT("MAX_TRADE_ROUTES") + GET_PLAYER(getOwnerINLINE()).getMaxTradeRoutesAdjustment();

	std::vector<int> paiBestValue(iMaxTradeRoutes, 0);
	std::vector<IDInfo> paTradeCities(iMaxTradeRoutes, IDInfo());

	if (!isDisorder() && !isPlundered())
	{
		int iTradeRoutes = std::min(getTradeRoutes() + std::max(0, iExtra), iMaxTradeRoutes);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER(getOwnerINLINE()).canHaveTradeRoutesWith((PlayerTypes)iI))
			{
				int iLoop = 0;
				for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					if (pLoopCity != this)
					{
						if (!(pLoopCity->isTradeRoute(getOwnerINLINE())) || (getTeam() == GET_PLAYER((PlayerTypes)iI).getTeam()))
						{
							if (pLoopCity->plotGroup(getOwnerINLINE()) == plotGroup(getOwnerINLINE()) || GC.getDefineINT("IGNORE_PLOT_GROUP_FOR_TRADE_ROUTES"))
							{
								int iValue = calculateTradeProfitTimes100(pLoopCity);

								for (int iJ = 0; iJ < iTradeRoutes; iJ++)
								{
									if (iValue > paiBestValue[iJ])
									{
										for (int iK = (iTradeRoutes - 1); iK > iJ; iK--)
										{
											paiBestValue[iK] = paiBestValue[(iK - 1)];
											paTradeCities[iK] = paTradeCities[(iK - 1)];
										}

										paiBestValue[iJ] = iValue;
										paTradeCities[iJ] = pLoopCity->getIDInfo();

										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	int iTradeProfit = 0;

	for (int iI = 0; iI < std::min(getTradeRoutes(), iMaxTradeRoutes); iI++)
	{
		CvCity* pLoopCity = getCity(paTradeCities[iI]);

		if (pLoopCity != NULL)
		{
			iTradeProfit -= calculateTradeProfitTimes100(pLoopCity);
		}
	}

	for (int iI = 0; iI < getTradeRoutes() + iExtra; iI++)
	{
		CvCity* pLoopCity = getCity(paTradeCities[iI]);

		if (pLoopCity != NULL)
		{
			iTradeProfit += calculateTradeProfitTimes100(pLoopCity);
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiTradeYields[iI] = calculateTradeYield(((YieldTypes)iI), iTradeProfit) / 100;
	}
}

int CvCity::getMinimumDefenseLevel() const
{
	if (!GC.getGameINLINE().isOption(GAMEOPTION_REALISTIC_SIEGE))
	{
		return 0;
	}

	if (m_iMinimumDefenseLevel == 0)
	{
		return m_iMinimumDefenseLevel;
	}

	if (getExtraMinDefense() > m_iMinimumDefenseLevel)
	{
		return getExtraMinDefense();
	}

	return m_iMinimumDefenseLevel;
}

void CvCity::setMinimumDefenseLevel(int iNewValue)
{
	m_iMinimumDefenseLevel = iNewValue;
}

int CvCity::getNumPopulationEmployed() const
{
	return m_iNumPopulationEmployed;
}

void CvCity::setNumPopulationEmployed(int iNewValue)
{
	m_iNumPopulationEmployed = iNewValue;
}

void CvCity::changeNumPopulationEmployed(int iChange)
{
	setNumPopulationEmployed(iChange + getNumPopulationEmployed());
	FAssert(getNumPopulationEmployed() >= 0);
}

void CvCity::removeWorstCitizenActualEffects(int iNumCitizens, int& iGreatPeopleRate, int& iHappiness, int& iHealthiness, int*& aiYields, int*& aiCommerces)
{
	PROFILE_FUNC();

	std::vector<SpecialistTypes> paeRemovedSpecailists(iNumCitizens, NO_SPECIALIST);
	std::vector<bool> abRemovedPlots(NUM_CITY_PLOTS, false);

	iGreatPeopleRate = 0;
	iHappiness = 0;
	iHealthiness = 0;
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYields[iI] = 0;
	}
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerces[iI] = 0;
	}
	int iGenericSpecialist = GC.getDefineINT("DEFAULT_SPECIALIST");

	bool bAvoidGrowth = false;
	bool bIgnoreGrowth = false;
	int iNumRemoved = 0;
	int iNumSpecialistsRemoved = 0;

	// if we are using more specialists than the free ones we get
	while (getAssignedSpecialistCount() < iNumRemoved && iNumRemoved < iNumCitizens)
	{
		// does generic 'citizen' specialist exist?
		if (iGenericSpecialist != NO_SPECIALIST)
		{
			// do we have at least one more generic citizen than we are forcing?
			if (getSpecialistCount((SpecialistTypes)iGenericSpecialist) > getForceSpecialistCount((SpecialistTypes)iGenericSpecialist))
			{
				paeRemovedSpecailists[iNumRemoved] = (SpecialistTypes)(iGenericSpecialist);
				iNumRemoved++;
				iNumSpecialistsRemoved++;
			}
		}
	}
	
	while (iNumRemoved < iNumCitizens)
	{
		int iWorstValue = MAX_INT;
		SpecialistTypes eWorstSpecialist = NO_SPECIALIST;
		int iWorstPlot = -1;

		// if we are using more specialists than the free ones we get
		if (getAssignedSpecialistCount() < iNumSpecialistsRemoved)
		{
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				{
					if (getSpecialistCount((SpecialistTypes)iI) > getForceSpecialistCount((SpecialistTypes)iI))
					{
						int iValue = AI_specialistValue(((SpecialistTypes)iI), bAvoidGrowth, /*bRemove*/ true);

						if (iValue < iWorstValue)
						{
							iWorstValue = iValue;
							eWorstSpecialist = ((SpecialistTypes)iI);
							iWorstPlot = -1;
						}
					}
				}
			}
		}
		
		// check all the plots we working
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				if (isWorkingPlot(iI) && !abRemovedPlots[iI])
				{
					CvPlot* pLoopPlot = getCityIndexPlot(iI);

					if (pLoopPlot != NULL)
					{
						int iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ true, /*bIgnoreFood*/ false, bIgnoreGrowth);

						if (iValue < iWorstValue)
						{
							iWorstValue = iValue;
							eWorstSpecialist = NO_SPECIALIST;
							iWorstPlot = iI;
						}
					}
				}
			}
		}
		
		if (eWorstSpecialist != NO_SPECIALIST)
		{
			paeRemovedSpecailists[iNumRemoved] = eWorstSpecialist;
			iNumRemoved++;
			iNumSpecialistsRemoved++;
		}
		else if (iWorstPlot != -1)
		{
			abRemovedPlots[iWorstPlot] = true;
			iNumRemoved++;
		}
		else
		{
			break;
		}
	}
	
	for (int iI = 0; iI < iNumCitizens; iI++)
	{
		if (paeRemovedSpecailists[iI] != NO_SPECIALIST)
		{
			CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(paeRemovedSpecailists[iI]);
			iHappiness -= kSpecialist.getHappinessPercent();
			iHealthiness -= kSpecialist.getHealthPercent();
			iGreatPeopleRate -= kSpecialist.getGreatPeopleRateChange();
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
			//Team Project (1)
				aiYields[iJ] -= specialistYield(paeRemovedSpecailists[iI], (YieldTypes)iJ);
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				aiCommerces[iJ] -= GET_PLAYER(getOwnerINLINE()).specialistCommerce(paeRemovedSpecailists[iI], (CommerceTypes)iJ);
			}
		}
	}
	iHealthiness /= 100;
	iHappiness /= 100;
	for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
	{
		if (abRemovedPlots[iJ])
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iJ);
			for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
			{
				aiYields[iK] -= pLoopPlot->getYield((YieldTypes)iK);
			}
		}
	}
}

int CvCity::calculatePopulationHappiness() const
{
	return m_iHappinessPercentPerPopulation * getPopulation() / 100;
}

void CvCity::changeHappinessPercentPerPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iHappinessPercentPerPopulation += iChange;
		AI_setAssignWorkDirty(true);
	}
}

int CvCity::calculatePopulationHealth() const
{
	return m_iHealthPercentPerPopulation * getPopulation() / 100;
}

void CvCity::changeHealthPercentPerPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iHealthPercentPerPopulation += iChange;
		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getAssignedSpecialistCount() const
{
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iCount += getSpecialistCount((SpecialistTypes)iI);
	}
	return iCount;
}

bool CvCity::isAutomatedCanBuild(BuildTypes eBuild) const
{
	return m_pabAutomatedCanBuild[eBuild];
}

void CvCity::setAutomatedCanBuild(BuildTypes eBuild, bool bNewValue)
{
	m_pabAutomatedCanBuild[eBuild] = bNewValue;
}

int CvCity::getMintedCommerceTimes100() const
{
	int iCommerceTimes100 = 0;
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		int iBonusCount = getNumBonuses((BonusTypes)iI);
		if (iBonusCount != 0)
		{
			iCommerceTimes100 += iBonusCount * GET_PLAYER(getOwnerINLINE()).getBonusMintedPercent((BonusTypes)iI);
		}
	}
	return iCommerceTimes100;
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

void CvCity::clearModifierTotals()
{
	//	If the city is running a process turn it off until the recalc completes
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_MAINTAIN:
			processProcess((ProcessTypes)pOrderNode->m_data.iData1, -1);
			break;
		default:
			break;
		}
	}

	//	Remove this city from its plot group (without bothering to do any recalculation iteratively)
	CvPlotGroup* ownerPlotGroup = plotGroup(getOwnerINLINE());
	if ( ownerPlotGroup != NULL )
	{
		ownerPlotGroup->removePlot(plot(), false);
	}

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if ( isHasCorporation((CorporationTypes)iI) )
		{
			applyCorporationModifiers((CorporationTypes)iI, false);
		}
	}

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if ( isHasReligion((ReligionTypes)iI) )
		{
			applyReligionModifiers((ReligionTypes)iI, false);
		}
	}

	m_iGreatPeopleRateModifier = 0;
	m_iGovernmentCenterCount = 0;
	m_iMaintenanceModifier = 0;
	m_iWarWearinessModifier = 0;
	m_iHurryAngerModifier = 0;
	m_iHealRate = 0;
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iPowerGoodHealth = 0;
	m_iPowerBadHealth = 0;
	m_iBonusGoodHealth = 0;
	m_iBonusBadHealth = 0;
	m_iBuildingGoodHappiness = 0;
	m_iBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHappiness = 0;
	m_iExtraBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHealth = 0;
	m_iExtraBuildingBadHealth = 0;
	m_iBonusGoodHappiness = 0;
	m_iBonusBadHappiness = 0;
	m_iReligionGoodHappiness = 0;
	m_iReligionBadHappiness = 0;
	m_iExtraHappiness = 0;
	m_iExtraHealth = 0;
	m_iNoUnhappinessCount = 0;
	m_iNoUnhealthyPopulationCount = 0;
	m_iBuildingOnlyHealthyCount = 0;
	m_fMaxFoodKeptMultiplierLog = 0.0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iExtraTradeRoutes = 0;
	m_iTradeRouteModifier = 0;
	m_iForeignTradeRouteModifier = 0;
	m_iBuildingDefense = 0;
	m_iBuildingBombardDefense = 0;
	m_iFreeExperience = 0;
	m_iMaxAirlift = 0;
	m_iAirModifier = 0;
	m_iAirUnitCapacity = 0;
	m_iNukeModifier = 0;
	m_iFreeSpecialist = 0;
	m_iPowerCount = 0;
	m_iDirtyPowerCount = 0;
	m_iSpecialistFreeExperience = 0;
	m_iEspionageDefenseModifier = 0;
	m_fPopulationgrowthratepercentageLog = 0.0;
	m_iLostProductionBase = 0;
	m_iImprovementGoodHealth = 0;
	m_iImprovementBadHealth = 0;
	m_iSpecialistGoodHealth = 0;
	m_iSpecialistBadHealth = 0;
	m_iSpecialistHappiness = 0;
	m_iSpecialistUnhappiness = 0;
	m_iLineOfSight = 0;
	m_iInvasionChance = 0;
	m_iInvasionTimer = 0;
	m_iAdjacentDamagePercent = 0;
	m_iWorkableRadiusOverride = 0;
	m_iProtectedCultureCount = 0;
	m_iNumUnitFullHeal = 0;
	m_iMinimumDefenseLevel = 0;
	m_iHealthPercentPerPopulation = 0;
	m_iHappinessPercentPerPopulation = 0;
	m_iBaseGreatPeopleRate = 0;
	m_iNumPopulationEmployed = 0;

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumPropertyInfos(); iJ++)
		{
			m_ppaaiExtraBonusAidModifier[iI][iJ]=0;
		}
		m_paiNoBonus[iI] = 0;
		m_paiFreeBonus[iI] = 0;
		m_paiNumBonuses[iI] = 0;
		m_paiNumCorpProducedBonuses[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_paiNumFreeBuilding[iI] = 0;
		m_paiNumFreeAreaBuilding[iI] = 0;
		m_pabDisabledBuilding[iI] = false;
		if ( m_paiBuildingReplaced != NULL )
		{
			m_paiBuildingReplaced[iI] = 0;
		}
//Team Project (5)
		m_pabReligiouslyDisabledBuilding[iI] = false;
	}

	for (int iI = 0;iI < GC.getNumUnitInfos();iI++)
	{
		m_paiGreatPeopleUnitRate[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_paiMaxSpecialistCount[iI] = 0;
		m_paiForceSpecialistCount[iI] = 0;
		m_paiFreeSpecialistCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		m_paiImprovementFreeSpecialists[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		m_paiStateReligionHappiness[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		m_paiUnitCombatFreeExperience[iI] = 0;
		m_paiUnitCombatExtraStrength[iI] = 0;
		//TB Combat Mods (Buildings) begin
		m_paiUnitCombatProductionModifier[iI] = 0;
		m_paiUnitCombatRepelModifier[iI] = 0;
		m_paiUnitCombatRepelAgainstModifier[iI] = 0;
		m_paiUnitCombatDefenseAgainstModifier[iI] = 0;
		m_paiUnitCombatOngoingTrainingTimeCount[iI] = 0;
		m_paiUnitCombatOngoingTrainingTimeIncrement[iI] = 0;
		m_paiDamageAttackingUnitCombatCount[iI] = 0;
		m_paiHealUnitCombatTypeVolume[iI] = 0;
		//TB Combat Mods (Buildings) end
	}

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		m_paiFreePromotionCount[iI] = 0;
	}

	//TB Combat Mods (Buildings) Begin
	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		m_paiNewAfflictionTypeCount[iI] = 0;
		m_paiNewExtraAfflictionOutbreakLevelChange[iI] = 0;
		m_paiNewAfflictionToleranceChange[iI] = 0;
		m_paiNewCurrentOvercomeChange[iI] = 0;
	}
	//TB Combat Mods (Buildings) end

	for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
	{
		m_paiAidRate[iI] = 0;
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSeaPlotYield[iI] = 0;
		m_aiRiverPlotYield[iI] = 0;
		m_aiBaseYieldRate[iI] = 0;
		m_aiBaseYieldPerPopRate[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiPowerYieldRateModifier[iI] = 0;
		m_aiBonusYieldRateModifier[iI] = 0;
		m_aiTradeYield[iI] = 0;
		m_aiCorporationYield[iI] = 0;
		m_aiExtraSpecialistYield[iI] = 0;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiCommerceRate[iI] = 0;
		m_abCommerceRateDirty[iI] = false;
		m_aiProductionToCommerceModifier[iI] = 0;
		m_aiBuildingCommerce[iI] = 0;
		m_aiSpecialistCommerce[iI] = 0;
		m_aiReligionCommerce[iI] = 0;
		m_aiCorporationCommerce[iI] = 0;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCommerceHappinessPer[iI] = 0;
		m_aiBonusCommerceRateModifier[iI] = 0;
		m_aiBonusCommercePercentChanges[iI] = 0;
		m_aiCommerceAttacks[iI] = 0;
		m_aiMaxCommerceAttacks[iI] = 0;
		m_aiExtraSpecialistCommerce[iI] = 0;
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiDomainFreeExperience[iI] = 0;
		m_aiDomainProductionModifier[iI] = 0;
	}

	m_aBuildingYieldChange.clear();
	m_aBuildingCommerceChange.clear();
	m_aBuildingHappyChange.clear();
	m_aBuildingHealthChange.clear();

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		m_paiUnitClassProductionModifier[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		m_paiBuildingClassProductionModifier[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		m_paiBonusDefenseChanges[iI] = 0;
	}
	
	m_aBuildingCommerceModifier.clear();
	m_aBuildingYieldModifier.clear();

	//m_Properties.clear();
	m_aPropertySpawns.clear();

	//	Until this city gets to process its buildings
	m_recalcBuilding = -1;

	//	Force isWorkingPlot() to return false for now because we don't
	//	want chnages to other thuings like traits, adjusting trhe not-yte-set
	//	city yields based on plots being worked until we explicitly add them back in
	m_bPlotWorkingMasked = true;
	//TB Combat Mods (Buildings) begin	
	m_iAidRate = 0;
	m_iTotalFrontSupportPercentModifier = 0;
	m_iTotalShortRangeSupportPercentModifier = 0;
	m_iTotalMediumRangeSupportPercentModifier = 0;
	m_iTotalLongRangeSupportPercentModifier = 0;
	m_iTotalFlankSupportPercentModifier = 0;
//Team Project (3)
	m_iExtraLocalCaptureProbabilityModifier = 0;
	m_iExtraLocalCaptureResistanceModifier = 0;
	m_iExtraLocalDynamicDefense = 0;
	m_iExtraRiverDefensePenalty = 0;
	m_iExtraLocalRepel = 0;
	m_iExtraMinDefense = 0;
	m_iExtraBuildingDefenseRecoverySpeedModifier = 0;
	m_iModifiedBuildingDefenseRecoverySpeedCap = 0;
	m_iExtraCityDefenseRecoverySpeedModifier = 0;
//Team Project (1)
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			m_ppaaiTechSpecialistHappinessTypes[iI][iJ] = 0;
			m_ppaaiTechSpecialistHealthTypes[iI][iJ] = 0;
		}
		m_paiTechHappiness[iI] = 0;
		m_paiTechSpecialistHappiness[iI] = 0;
		m_paiTechHealth[iI] = 0;
		m_paiTechSpecialistHealth[iI] = 0;
	}
	m_iExtraTechSpecialistHappiness = 0;
	m_iExtraTechHappiness = 0;
	m_iExtraTechSpecialistHealth = 0;
	m_iExtraTechHealth = 0;
	m_iPrioritySpecialist = NO_SPECIALIST;
	m_iExtraInsidiousness = 0;
	m_iExtraInvestigation = 0;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppaaiLocalSpecialistExtraYield[iI][iJ] = 0;
		}
		m_paiSpecialistBannedCount[iI] = 0;
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppaaiLocalSpecialistExtraCommerce[iI][iJ] = 0;
		}
	}
	//TB Combat Mods (Buildings) end
}

void CvCity::recalculateModifiers()
{
	area()->changePower(getOwnerINLINE(), getPopulationPower(getPopulation()));

	m_bPlotWorkingMasked = false;

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if ( isWorkingPlot(iI) )
		{
			processWorkingPlot(iI, 1, true);
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		int	iSpecialistCount = getSpecialistCount((SpecialistTypes)iI);

		if ( iSpecialistCount > 0 )
		{
			processSpecialist((SpecialistTypes)iI, iSpecialistCount);
		}

		//	Add back the unattributed specialists (those direct from Python or
		//	from GPs that joined the city)
		if ( m_paiFreeSpecialistCountUnattributed[iI] != 0 )
		{
			m_paiFreeSpecialistCount[iI] += m_paiFreeSpecialistCountUnattributed[iI];
			processSpecialist((SpecialistTypes)iI, m_paiFreeSpecialistCountUnattributed[iI]);
		}
	}

	for(m_recalcBuilding = 0; m_recalcBuilding < GC.getNumBuildingInfos(); m_recalcBuilding++)
	{
		if ( getNumRealBuilding((BuildingTypes)m_recalcBuilding) > 0 )
		{
			//	Process back the buildings we physically have. This will generate free
			//	buildings as it goes.  Tech reprocessing will be called later which will
			//	re-obsolete those that need it
			processBuilding((BuildingTypes)m_recalcBuilding, 1);
		}
	}

	//	After processing all buildings set the indicator that reprocessing is not in progress any more
	m_recalcBuilding = MAX_INT;

	//	Put corporations back
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if ( isHasCorporation((CorporationTypes)iI) )
		{
			applyCorporationModifiers((CorporationTypes)iI, true);
		}
	}

	updateCorporation();

	//	Put religions back
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if ( isHasReligion((ReligionTypes)iI) )
		{
			applyReligionModifiers((ReligionTypes)iI, true);
		}
	}
	//Team Project (5)
	checkReligiousDisablingAllBuildings();

	//	Replay events in so far as they effect modifiers
	for (int iI = 0; iI < (int)m_aEventsOccured.size(); iI++)
	{
		if ( m_aEventsOccured[iI] != NO_EVENT )
		{
			applyEvent(m_aEventsOccured[iI], NULL);
		}
	}

	//	If the city is running a process acout for it again
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_MAINTAIN:
			processProcess((ProcessTypes)pOrderNode->m_data.iData1, 1);
			break;
		default:
			break;
		}
	}

	updateFreshWaterHealth();
	updateFeatureHealth();
	updateImprovementHealth();
	updateFeatureHappiness();
	updatePowerHealth();

	//ls612: Make Sure to keep the Air Unit capacity
	changeAirUnitCapacity(GC.getDefineINT("CITY_AIR_UNIT_CAPACITY"));
}

void CvCity::setBuildingListInvalid()
{
	m_BuildingList.setInvalid();
}

bool CvCity::getBuildingListFilterActive(BuildingFilterTypes eFilter)
{
	return m_BuildingList.getFilterActive(eFilter);
}

void CvCity::setBuildingListFilterActive(BuildingFilterTypes eFilter, bool bActive)
{
	m_BuildingList.setFilterActive(eFilter, bActive);
}

BuildingGroupingTypes CvCity::getBuildingListGrouping()
{
	return m_BuildingList.getGroupingActive();
}

void CvCity::setBuildingListGrouping(BuildingGroupingTypes eGrouping)
{
	m_BuildingList.setGroupingActive(eGrouping);
}

BuildingSortTypes CvCity::getBuildingListSorting()
{
	return m_BuildingList.getSortingActive();
}

void CvCity::setBuildingListSorting(BuildingSortTypes eSorting)
{
	m_BuildingList.setSortingActive(eSorting);
}

int CvCity::getBuildingListGroupNum()
{
	return m_BuildingList.getGroupNum();
}

int CvCity::getBuildingListNumInGroup(int iGroup)
{
	return m_BuildingList.getNumInGroup(iGroup);
}

BuildingTypes CvCity::getBuildingListType(int iGroup, int iPos)
{
	return m_BuildingList.getBuildingType(iGroup, iPos);
}

int CvCity::getBuildingListSelectedBuildingRow()
{
	return m_BuildingList.getBuildingSelectionRow();
}

int CvCity::getBuildingListSelectedWonderRow()
{
	return m_BuildingList.getWonderSelectionRow();
}

BuildingTypes CvCity::getBuildingListSelectedBuilding()
{
	return m_BuildingList.getSelectedBuilding();
}

BuildingTypes CvCity::getBuildingListSelectedWonder()
{
	return m_BuildingList.getSelectedWonder();
}

void CvCity::setBuildingListSelectedBuilding(BuildingTypes eBuilding)
{
	m_BuildingList.setSelectedBuilding(eBuilding);
}

void CvCity::setBuildingListSelectedWonder(BuildingTypes eWonder)
{
	m_BuildingList.setSelectedWonder(eWonder);
}

void CvCity::setUnitListInvalid()
{
	m_UnitList.setInvalid();
}

bool CvCity::getUnitListFilterActive(UnitFilterTypes eFilter)
{
	return m_UnitList.getFilterActive(eFilter);
}

void CvCity::setUnitListFilterActive(UnitFilterTypes eFilter, bool bActive)
{
	m_UnitList.setFilterActive(eFilter, bActive);
}

UnitGroupingTypes CvCity::getUnitListGrouping()
{
	return m_UnitList.getGroupingActive();
}

void CvCity::setUnitListGrouping(UnitGroupingTypes eGrouping)
{
	m_UnitList.setGroupingActive(eGrouping);
}

UnitSortTypes CvCity::getUnitListSorting()
{
	return m_UnitList.getSortingActive();
}

void CvCity::setUnitListSorting(UnitSortTypes eSorting)
{
	m_UnitList.setSortingActive(eSorting);
}

int CvCity::getUnitListGroupNum()
{
	return m_UnitList.getGroupNum();
}

int CvCity::getUnitListNumInGroup(int iGroup)
{
	return m_UnitList.getNumInGroup(iGroup);
}

UnitTypes CvCity::getUnitListType(int iGroup, int iPos)
{
	return m_UnitList.getUnitType(iGroup, iPos);
}

int CvCity::getUnitListSelectedRow()
{
	return m_UnitList.getSelectionRow();
}

UnitTypes CvCity::getUnitListSelected()
{
	return m_UnitList.getSelectedUnit();
}

void CvCity::setUnitListSelected(UnitTypes eUnit)
{
	m_UnitList.setSelectedUnit(eUnit);
}

int CvCity::getTotalBuildingSourcedProperty(PropertyTypes eProperty) const
{
	std::map<int,int>::const_iterator itr = m_buildingSourcedPropertyCache.find(eProperty);

	if ( itr != m_buildingSourcedPropertyCache.end() )
	{
		return itr->second;
	}
	else
	{
		MEMORY_TRACK_EXEMPT();

		int	iValue = 0;

		for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			if ( getNumActiveBuilding((BuildingTypes)iI) > 0 )
			{
				//Team Project (5)
				if (!isReligiouslyDisabledBuilding((BuildingTypes)iI))
				{
					CvPropertyManipulators* pBuildingPropertyManipulators = GC.getBuildingInfo((BuildingTypes)iI).getPropertyManipulators();

					int num = pBuildingPropertyManipulators->getNumSources();
					for (int iJ = 0; iJ < num; iJ++)
					{
						CvPropertySource* pSource = pBuildingPropertyManipulators->getSource(iJ);

						//	For now we're only interested in constant sources
						//	TODO - expand this as buildings add other types
						if ( pSource->getType() == PROPERTYSOURCE_CONSTANT && pSource->getProperty() == eProperty)
						{
							iValue += static_cast<CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObjectConst());
						}
					}
				}
			}
		}

		m_buildingSourcedPropertyCache[(int)eProperty] = iValue;

		return iValue;
	}
}

void unitSources(CvGameObject* pObject, CvPropertyManipulators* pMani, PropertyTypes eProperty, const CvCity* pCity, int* iValue)
{
	int iNum = pMani->getNumSources();

	for (int i=0; i<iNum; i++)
	{
		CvPropertySource* pSource = pMani->getSource(i);

		//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
		//	to the city for most properties anyway
		if (pSource->getProperty() == eProperty &&
			(pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT) &&
			pSource->getType() == PROPERTYSOURCE_CONSTANT)
		{
			*iValue += static_cast<CvPropertySourceConstant*>(pSource)->getAmountPerTurn(pCity->getGameObjectConst());
		}
	}
}

int CvCity::getTotalUnitSourcedProperty(PropertyTypes eProperty) const
{
	std::map<int,int>::const_iterator itr = m_unitSourcedPropertyCache.find(eProperty);

	if ( itr != m_unitSourcedPropertyCache.end() )
	{
		return itr->second;
	}
	else
	{
		MEMORY_TRACK_EXEMPT();
		int	iValue = 0;

		CvPlot* pPlot = plot();
		for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
		{
			unitItr->getGameObject()->foreachManipulator(boost::bind(unitSources, _1, _2, eProperty, this, &iValue));
		}

		m_unitSourcedPropertyCache[(int)eProperty] = iValue;

		return iValue;
	}
}

void unitHasSources(CvGameObject* pObject, CvPropertyManipulators* pMani, bool* bHasSources)
{
	int iNum = pMani->getNumSources();

	for (int i=0; i<iNum; i++)
	{
		CvPropertySource* pSource = pMani->getSource(i);

		//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
		//	to the city for most properties anyway
		if ((pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT) &&
			pSource->getType() == PROPERTYSOURCE_CONSTANT)
		{
			*bHasSources = true;
			break;
		}
	}
}

//	Helper function to determine of a unit has any city/plot property sources
static bool unitHasCityOrPlotPropertySources(CvUnit* pUnit)
{
	bool bHasSources;

	pUnit->getGameObject()->foreachManipulator(boost::bind(unitHasSources, _1, _2, &bHasSources));

	return bHasSources;
}

void CvCity::noteUnitMoved(CvUnit* pUnit) const
{
	if ( unitHasCityOrPlotPropertySources( pUnit ) )
	{
		m_unitSourcedPropertyCache.clear();
	}
}

void sumCitySources(CvGameObject* pObject, CvPropertyManipulators* pMani, const CvCity* pCity, int* iSum, PropertyTypes eProperty)
{
	int iNum = pMani->getNumSources();
	for (int i=0; i<iNum; i++)
	{
		CvPropertySource* pSource = pMani->getSource(i);
		if (pSource->getProperty() == eProperty)
		{
			if (pSource->isActive(const_cast<CvGameObjectCity*>(pCity->getGameObjectConst())))
			{
				*iSum += pSource->getSourcePredict(pCity->getGameObjectConst(), pCity->getPropertiesConst()->getValueByProperty((int)eProperty));
			}
		}
	}
}

int CvCity::getGlobalSourcedProperty(PropertyTypes eProperty) const
{
	int iSum = 0;
	CvPropertyManipulators* pMani = GC.getPropertyInfo(eProperty).getPropertyManipulators();
	int iNum = pMani->getNumSources();
	for (int i=0; i<iNum; i++)
	{
		CvPropertySource* pSource = pMani->getSource(i);
		if (pSource->isActive(const_cast<CvGameObjectCity*>(getGameObjectConst())))
		{
			iSum += pSource->getSourcePredict(getGameObjectConst(), getPropertiesConst()->getValueByProperty((int)eProperty));
		}
	}
	// Add sources from the player object that have an effect on cities
	GET_PLAYER(getOwner()).getGameObject()->foreachManipulator(boost::bind(sumCitySources, _1, _2, this, &iSum, eProperty));

	return iSum;
}
//TB Combat Mods (Buildings) begin
int CvCity::getUnitAidPresent(PropertyTypes eProperty) const
{
	PROFILE_FUNC();
	
	int iBestAid = 0;

	CvPlot* pPlot = plot();
	for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
	{
		if (unitItr->getTeam() == getTeam())
		{
			int  iAid = unitItr->aidTotal(eProperty);

			if (iAid > iBestAid)
			{
				iBestAid = iAid;
			}
		}
	}
	return iBestAid;
}

int CvCity::getCityAidTotal(PromotionLineTypes ePromotionLineType) const
{
	if (ePromotionLineType != NO_PROMOTIONLINE)
	{
		PropertyTypes ePropertyType = GC.getPromotionLineInfo(ePromotionLineType).getPropertyType();
		if (ePropertyType != NO_PROPERTY)
		{
			int iBonusTotal = 0;
			for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				BonusTypes eBonus = ((BonusTypes)iI);
				iBonusTotal += getExtraBonusAidModifier(eBonus, ePropertyType);
			}
			return (getAidRate(ePropertyType) + getUnitAidPresent(ePropertyType) + iBonusTotal);
		}
	}
	return 0;
}

int CvCity::getAidRate(PropertyTypes ePropertyType) const
{
	FAssertMsg(ePropertyType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePropertyType < GC.getNumPropertyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return std::max(0, m_paiAidRate[ePropertyType]);
}

void CvCity::changeAidRate(PropertyTypes ePropertyType, int iChange)
{
	FAssertMsg(ePropertyType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePropertyType < GC.getNumPropertyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiAidRate[ePropertyType] = (m_paiAidRate[ePropertyType] + iChange);
}

void CvCity::setAidRate(PropertyTypes ePropertyType, int iChange)//may be unnecessary
{
	FAssertMsg(ePropertyType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePropertyType < GC.getNumPropertyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiAidRate[ePropertyType] = iChange;
}

bool CvCity::hasAfflictionType(PromotionLineTypes ePromotionLineType) const
{
	FAssertMsg(ePromotionLineType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLineType < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return (m_paiNewAfflictionTypeCount[ePromotionLineType] > 0);
}

void CvCity::changeAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange)
{
	FAssertMsg(ePromotionLineType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLineType < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewAfflictionTypeCount[ePromotionLineType] = (m_paiNewAfflictionTypeCount[ePromotionLineType] + iChange);
}

void CvCity::setAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange)
{
	FAssertMsg(ePromotionLineType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLineType < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewAfflictionTypeCount[ePromotionLineType] = iChange;
}

int CvCity::getExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType) const
{
	FAssertMsg(eBonusType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eBonusType < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiExtraBonusAidModifier[eBonusType][ePropertyType];
}

void CvCity::changeExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType, int iChange)
{
	FAssertMsg(eBonusType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eBonusType < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ppaaiExtraBonusAidModifier[eBonusType][ePropertyType] = (m_ppaaiExtraBonusAidModifier[eBonusType][ePropertyType] + iChange);
}

void CvCity::setExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType, int iChange)
{
	FAssertMsg(eBonusType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eBonusType < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ppaaiExtraBonusAidModifier[eBonusType][ePropertyType] = iChange;
}

int CvCity::getExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine) const
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNewExtraAfflictionOutbreakLevelChange[ePromotionLine];
}

void CvCity::changeExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewExtraAfflictionOutbreakLevelChange[ePromotionLine] = (m_paiNewExtraAfflictionOutbreakLevelChange[ePromotionLine] + iChange);
}

void CvCity::setExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewExtraAfflictionOutbreakLevelChange[ePromotionLine] = iChange;
}

int CvCity::getAfflictionToleranceChange(PromotionLineTypes ePromotionLine) const
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNewAfflictionToleranceChange[ePromotionLine];
}

void CvCity::changeAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewAfflictionToleranceChange[ePromotionLine] = std::max(0,(m_paiNewAfflictionToleranceChange[ePromotionLine] + iChange));
}

void CvCity::setAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewAfflictionToleranceChange[ePromotionLine] = iChange;
}

int CvCity::getCurrentOvercomeChange(PromotionLineTypes ePromotionLine) const
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNewCurrentOvercomeChange[ePromotionLine];
}

void CvCity::changeCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewCurrentOvercomeChange[ePromotionLine] = (m_paiNewCurrentOvercomeChange[ePromotionLine] + iChange);
}

void CvCity::setCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FAssertMsg(ePromotionLine >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotionLine < GC.getNumPromotionLineInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNewCurrentOvercomeChange[ePromotionLine] = iChange;
}

int CvCity::getTotalTechOutbreakLevelChange(BuildingTypes eBuilding) const
{
	TechTypes eTech;
	int iTotalTechOutbreakLevelChange = 0;
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		eTech = ((TechTypes)iI);
		if (GET_TEAM(getTeam()).isHasTech(eTech))
		{
			iTotalTechOutbreakLevelChange += GC.getBuildingInfo(eBuilding).getTechOutbreakLevelChange(iI);
		}
	}
	return iTotalTechOutbreakLevelChange;
}

int CvCity::getPropertyValue(PropertyTypes eProperty)
{
	return getProperties()->getValueByProperty(eProperty);
}

int CvCity::getOutbreakChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const
{
	int iTotal = 0;
	//iTotal -= getTradeCommunicabilityTotal(eAfflictionBuilding, eAfflictionLine);
	//iTotal -= getUnitCommunicability(eAfflictionLine);
	//iTotal += getCityAidTotal();
	//TB Note: Communicability is tallied in the Outbreak Check function earlier than this function is called, then subtracted there
	iTotal += getExtraAfflictionOutbreakLevelChange(eAfflictionLine);
	iTotal += getTotalTechOutbreakLevelChange(eAfflictionBuilding);
	iTotal += getAfflictionToleranceChange(eAfflictionLine);

	return iTotal;
}

int CvCity::getOvercomeChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const
{
	int iTotal = 0;
	iTotal -= getTotalCommunicableExposure(eAfflictionLine);
	iTotal += getCityAidTotal(eAfflictionLine);
	iTotal += getAfflictionToleranceChange(eAfflictionLine);
	iTotal -= getCurrentOvercomeChange(eAfflictionLine);
	/*iTotal += getExtraAfflictionOutbreakLevelChange(eAfflictionLine);*/
	/*iTotal += getTotalTechOutbreakLevelChange(eAfflictionBuilding);*/
	/*iTotal -= getUnitCommunicability(eAfflictionLine);*///Now included in Total Communicable Exposure

	return iTotal;
}

int CvCity::getUnitCommunicability(PromotionLineTypes eAfflictionLine) const
{
	PROFILE_FUNC();

		
	int iCommunicability = 0;

	if (!GC.getPromotionLineInfo(eAfflictionLine).isNoSpreadUnittoCity())
	{
		bool bCheck = false;	
		int iWorstContractModifier = 0;
		CvPlot* pPlot = plot();
		for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
		{
			if (unitItr->hasAfflictionLine(eAfflictionLine))
			{
				bCheck = true;
				iWorstContractModifier = std::max<int>(unitItr->worsenedProbabilitytoAfflict(eAfflictionLine), iWorstContractModifier);
			}
		}
		if (bCheck)
		{
			iCommunicability = GC.getPromotionLineInfo(eAfflictionLine).getCommunicability();
			iCommunicability += iWorstContractModifier;
		}
	}
	return iCommunicability;
}

int CvCity::getTradeCommunicabilityTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const
{
	int iTradeCommunicabilityTotal = 0;

	for (int iI = 0; iI < getTradeRoutes(); ++iI)
	{
		CvCity* pCity = getTradeCity(iI);
		if (pCity->hasAfflictionType(eAfflictionLine))
		{
			if (pCity->getNumRealBuilding(eAfflictionBuilding) > 0)
			{
				iTradeCommunicabilityTotal += GC.getBuildingInfo(eAfflictionBuilding).getTradeCommunicability();
			}
		}
	}
	return iTradeCommunicabilityTotal;
}

bool CvCity::canAcquireAffliction(BuildingTypes eDisease, PromotionLineTypes eAfflictionLine) const
{
	if (eDisease == NO_BUILDING)
	{
		return false;
	}

	if (getNumRealBuilding(eDisease) > 0)
	{
		//Already Has
		return false;
	}
	return true;
}

int CvCity::getTotalCommunicableExposure(PromotionLineTypes eAfflictionLine) const
{
	CvPromotionLineInfo& kAffliction = GC.getPromotionLineInfo(eAfflictionLine);

	int iTradeCommunicability = 0;
	for (int iI = 0; iI < kAffliction.getNumBuildings(); iI++)
	{
		iTradeCommunicability += getTradeCommunicabilityTotal((BuildingTypes)kAffliction.getBuilding(iI), eAfflictionLine);
	}
	int iUnitCommunicability = getUnitCommunicability(eAfflictionLine);
	int iWorkedTileCommunicability = 0;
	int iVicinityTileCommunicability = 0;

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), iI);
		if (isWorkingPlot(iI))
		{
			iWorkedTileCommunicability += pLoopPlot->getCommunicability(eAfflictionLine, true, false, false);
		}
		iVicinityTileCommunicability += pLoopPlot->getCommunicability(eAfflictionLine, false, true, false);
	}

	int iAccessVolumeBonusCommunicability = 0;

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		BonusTypes eBonus = (BonusTypes)iI;
		iAccessVolumeBonusCommunicability += (getNumBonuses(eBonus) * GC.getBonusInfo(eBonus).getAfflictionCommunicabilityType(eAfflictionLine, false, false, true).iModifier);
	}

	int iTotalCommunicableExposure = iTradeCommunicability + iUnitCommunicability + iWorkedTileCommunicability + iVicinityTileCommunicability + iAccessVolumeBonusCommunicability;
	return iTotalCommunicableExposure;
}

void CvCity::doOutbreakCheck(PromotionLineTypes eAfflictionLine)
{
	PropertyTypes ePropertyType = GC.getPromotionLineInfo(eAfflictionLine).getPropertyType();
	CvPropertyInfo& kInfo = GC.getPropertyInfo(ePropertyType);
	CvPromotionLineInfo& kAffliction = GC.getPromotionLineInfo(eAfflictionLine);
	BuildingTypes eBuilding;
	CvWString szBuffer;
	int iLowestLinePriority = MAX_INT;

	
	if (GC.getPromotionLineInfo(eAfflictionLine).getObsoleteTech() != NO_TECH)
	{
		if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getPromotionLineInfo(eAfflictionLine).getObsoleteTech())))
		{
			return;
		}
	}

	if (GC.getPromotionLineInfo(eAfflictionLine).getPrereqTech() != NO_TECH)
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getPromotionLineInfo(eAfflictionLine).getPrereqTech())))
		{
			return;
		}
	}

	for (int iI = 0; iI < kAffliction.getNumBuildings(); iI++)
	{
		if (canAcquireAffliction((BuildingTypes)kAffliction.getBuilding(iI), eAfflictionLine))
		{
			int iLinePriority = GC.getBuildingInfo((BuildingTypes)kAffliction.getBuilding(iI)).getLinePriority();
			if (iLinePriority < iLowestLinePriority)
			{
				iLowestLinePriority = iLinePriority;
				eBuilding = (BuildingTypes)kAffliction.getBuilding(iI);
			}
		}
	}

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if (iLowestLinePriority == MAX_INT)
	{
		//can't acquire
		return;
	}
	//set bonuses with promotionlines like buildings with promotionlines on promotionlineinfos so we can loop here
	int iTotalCommunicableExposure = getTotalCommunicableExposure(eAfflictionLine);

	bool bExposed = (iTotalCommunicableExposure > 0);
	bool bDoCheck = false;

	if (iLowestLinePriority <= 1)
	{
		//we do not have an outbreak at all and must check to see if we're exposed.  
		//If not exposed, we check canconstruct with a fair amount of prerequisite details.  The base likelihood is used for the check.
		//Otherwise, many canconstruct factors won't matter.  And the degree to which we are exposed will make a difference in the likelihood.
		bDoCheck = canConstruct(eBuilding, false, false, true/*bIgnoreCost - must be true for affliction checks*/, false, false, NO_TECH, 0, true, bExposed);
		//<-Add bools for bAffliction and bExposed - bAffliction would be true and bExposed would be bExposed			
	}
	else
	{
		//We already are in an outbreak.
		//Exposed has nothing to do with it.  The likelihood for getting worse is entirely a matter of the disease threshhold for this lowest building.
		bDoCheck = true;
	}

	if (!bDoCheck)
	{
		return;
	}

	//Design the check portion.
	int iOutbreakBase = kBuilding.getOutbreakBase();
	int iOutbreakTotal = iOutbreakBase - iTotalCommunicableExposure + getOutbreakChangesTotal(eBuilding, eAfflictionLine);

	int iChancetoOutbreak = getPropertyValue(ePropertyType) - iOutbreakTotal;
	int iChanceModifier = kAffliction.getOutbreakModifier();
	iChancetoOutbreak *= iChanceModifier;//spreads the chance of an outbreak across a wider range of property totals if low and condenses it into a smaller range if high.
	iChancetoOutbreak /= 100; //in short, this is a % modifier to the value of 1 of the property value.  Thus, if you would normally have a 100% chance to have an outbreak at 100 property value (0 base threshold with no modifiers) and a 50% chance at 50 property value, a 50% modifier here (half) would make it so you have a 100% chance at 200, a 50% chance at 100 and a 25% chance at 50.  This halving increased the RANGE by twice as much and diluted the value of a given point of the modifier.

	int iOutbreakRollResult = GC.getGameINLINE().getSorenRandNum(100, "Outbreak Check");

	if (iOutbreakRollResult < iChancetoOutbreak)
	{
		MEMORY_TRACK_EXEMPT();

		szBuffer = gDLL->getText("TXT_KEY_MISC_OUTBREAK", getNameKey(), GC.getBuildingInfo(eBuilding).getDescription());
		AddDLLMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), plot()->getX_INLINE(), plot()->getY_INLINE());
		setNumRealBuilding(eBuilding, 1);
	}

	return;
}

void CvCity::doOvercomeCheck(PromotionLineTypes eAfflictionLine)
{
	PropertyTypes ePropertyType = GC.getPromotionLineInfo(eAfflictionLine).getPropertyType();
	CvPropertyInfo& kInfo = GC.getPropertyInfo(ePropertyType);
	CvPromotionLineInfo& kAffliction = GC.getPromotionLineInfo(eAfflictionLine);
	int iHighestLinePriority = 0;
	BuildingTypes eBuilding;
	CvWString szBuffer;

	for (int iI = 0; iI < kAffliction.getNumBuildings(); iI++)
	{
		if (getNumRealBuilding((BuildingTypes)kAffliction.getBuilding(iI)) > 0)
		{
			int iLinePriority = GC.getBuildingInfo((BuildingTypes)kAffliction.getBuilding(iI)).getLinePriority();
			if (iLinePriority > iHighestLinePriority)
			{
				iHighestLinePriority = iLinePriority;
				eBuilding = (BuildingTypes)kAffliction.getBuilding(iI);
			}
		}
	}

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iOvercomeBase = kBuilding.getOvercomeBase();
	int iOvercomeTotal = iOvercomeBase + getOvercomeChangesTotal(eBuilding, eAfflictionLine);
	int iChancetoOvercome = iOvercomeTotal - getPropertyValue(ePropertyType);
	int iChanceModifier = kAffliction.getOvercomeModifier();
	iChancetoOvercome *= iChanceModifier;
	iChancetoOvercome /= 100;

	int iOvercomeRollResult = GC.getGameINLINE().getSorenRandNum(100, "Overcome Check");

	if (iOvercomeRollResult < iChancetoOvercome)
	{
		MEMORY_TRACK_EXEMPT();

		szBuffer = gDLL->getText("TXT_KEY_MISC_OVERCOME_CITY", getNameKey(), GC.getBuildingInfo(eBuilding).getDescription());
		AddDLLMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), plot()->getX_INLINE(), plot()->getY_INLINE());
		setNumRealBuilding(eBuilding, 0);
	}
	//PropertyTypes eDiseaseType = (PropertyTypes)GC.getInfoTypeForString("PROPERTY_DISEASE");
	//PropertyTypes eProperty;
	//PromotionLineTypes eCityDisease;
	//CvPropertyInfo& kInfo = GC.getPropertyInfo(eDiseaseType);
	//CvWString szBuffer;
	//int iNum = kInfo.getNumPropertyBuildings();
	//int iI;
	//int iJ;
	//int iOutbreakBase;
	//int iOvercomeTotal;
	//int iChancetoOvercome;
	//int iOvercomeRollResult;

	//for (iI=0; iI<iNum; iI++)
	//{
	//	eProperty = ((PropertyTypes)iI);
	//	if (eProperty == eDiseaseType)
	//	{
	//		PropertyBuilding& kBuilding = kInfo.getPropertyBuilding(iI);

	//		for (int iJ = 0; iJ < GC.getBuildingInfo(kBuilding.eBuilding).getNumDiseaseTypes(); iJ++)
	//		{
	//			eCityDisease = (PromotionLineTypes)GC.getBuildingInfo(kBuilding.eBuilding).getDiseaseType(iJ);
	//			if (eCityDisease == eAfflictionLine)
	//			{
	//				iOutbreakBase = kBuilding.iMinValue;
	//				iOvercomeTotal = iOutbreakBase + getOvercomeChangesTotal(kBuilding.eBuilding, eAfflictionLine);
	//				iChancetoOvercome = iOvercomeTotal - getDiseasePropertyValue();

	//				if (getDiseasePropertyValue() < iOvercomeTotal)
	//				{
	//					if (getNumRealBuilding(kBuilding.eBuilding) > 0)
	//					{
	//						iOvercomeRollResult = GC.getGameINLINE().getSorenRandNum(100, "Overcome");
	//						if (iOvercomeRollResult < iChancetoOvercome)
	//						{
	//							MEMORY_TRACK_EXEMPT();

	//							szBuffer = DLL_SERIALIZE(gDLL->getText("TXT_KEY_MISC_OVERCOME_CITY", getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getDescription()));
	//							AddDLLMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), plot()->getX_INLINE(), plot()->getY_INLINE());
	//							setNumRealBuilding(kBuilding.eBuilding, 0);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

int CvCity::getPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction) const						 
{
	FAssertMsg(eAffliction >= 0, "eAffliction expected to be >= 0");
	FAssertMsg(eAffliction < GC.getNumPromotionLineInfos(), "eAffliction expected to be < GC.getNumPromotionLineInfos()");
	return m_paiPromotionLineAfflictionAttackCommunicability[eAffliction];
}

void CvCity::changePromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iChange)
{
	FAssertMsg(eAffliction >= 0, "eAffliction expected to be >= 0");
	FAssertMsg(eAffliction < GC.getNumPromotionLineInfos(), "eAffliction expected to be < GC.getNumPromotionLineInfos()");
	m_paiPromotionLineAfflictionAttackCommunicability[eAffliction] = (m_paiPromotionLineAfflictionAttackCommunicability[eAffliction] + iChange);
}

void CvCity::setPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iValue)
{
	FAssertMsg(eAffliction >= 0, "eAffliction expected to be >= 0");
	FAssertMsg(eAffliction < GC.getNumPromotionLineInfos(), "eAffliction expected to be < GC.getNumPromotionLineInfos()");
	m_paiPromotionLineAfflictionAttackCommunicability[eAffliction] = iValue;
}

int CvCity::getUnitCombatProductionModifier(UnitCombatTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatProductionModifier[eIndex];
}

void CvCity::changeUnitCombatProductionModifier(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatProductionModifier[eIndex] = (m_paiUnitCombatProductionModifier[eIndex] + iChange);
}

int CvCity::getUnitCombatRepelModifierTotal(UnitCombatTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatRepelModifier[eIndex];
}

void CvCity::changeUnitCombatRepelModifierTotal(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatRepelModifier[eIndex] = (m_paiUnitCombatRepelModifier[eIndex] + iChange);
}

int CvCity::getUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatRepelAgainstModifier[eIndex];
}

void CvCity::changeUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatRepelAgainstModifier[eIndex] = (m_paiUnitCombatRepelAgainstModifier[eIndex] + iChange);
}

int CvCity::getUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatDefenseAgainstModifier[eIndex];
}

void CvCity::changeUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatDefenseAgainstModifier[eIndex] = (m_paiUnitCombatDefenseAgainstModifier[eIndex] + iChange);
}

int CvCity::getTotalFrontSupportPercentModifier() const
{
	return m_iTotalFrontSupportPercentModifier;
}

void CvCity::changeTotalFrontSupportPercentModifier(int iChange)
{
	m_iTotalFrontSupportPercentModifier = (m_iTotalFrontSupportPercentModifier + iChange);
	FAssert(getTotalFrontSupportPercentModifier() >= 0);
}

int CvCity::getTotalShortRangeSupportPercentModifier() const
{
	return m_iTotalShortRangeSupportPercentModifier;
}

void CvCity::changeTotalShortRangeSupportPercentModifier(int iChange)
{
	m_iTotalShortRangeSupportPercentModifier = (m_iTotalShortRangeSupportPercentModifier + iChange);
	FAssert(getTotalShortRangeSupportPercentModifier() >= 0);
}

int CvCity::getTotalMediumRangeSupportPercentModifier() const
{
	return m_iTotalMediumRangeSupportPercentModifier;
}

void CvCity::changeTotalMediumRangeSupportPercentModifier(int iChange)
{
	m_iTotalMediumRangeSupportPercentModifier = (m_iTotalMediumRangeSupportPercentModifier + iChange);
	FAssert(getTotalMediumRangeSupportPercentModifier() >= 0);
}

int CvCity::getTotalLongRangeSupportPercentModifier() const
{
	return m_iTotalLongRangeSupportPercentModifier;
}

void CvCity::changeTotalLongRangeSupportPercentModifier(int iChange)
{
	m_iTotalLongRangeSupportPercentModifier = (m_iTotalLongRangeSupportPercentModifier + iChange);
	FAssert(getTotalLongRangeSupportPercentModifier() >= 0);
}

int CvCity::getTotalFlankSupportPercentModifier() const
{
	return m_iTotalFlankSupportPercentModifier;
}

void CvCity::changeTotalFlankSupportPercentModifier(int iChange)
{
	m_iTotalFlankSupportPercentModifier = (m_iTotalFlankSupportPercentModifier + iChange);
	FAssert(getTotalFlankSupportPercentModifier() >= 0);
}

int CvCity::getUnitCombatOngoingTrainingTimeCount(UnitCombatTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatOngoingTrainingTimeCount[eIndex];
}

void CvCity::changeUnitCombatOngoingTrainingTimeCount(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatOngoingTrainingTimeCount[eIndex] = (m_paiUnitCombatOngoingTrainingTimeCount[eIndex] + iChange);
}

int CvCity::getUnitCombatOngoingTrainingTimeIncrement(UnitCombatTypes eIndex) const						 
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatOngoingTrainingTimeIncrement[eIndex];
}

void CvCity::setUnitCombatOngoingTrainingTimeIncrement(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatOngoingTrainingTimeIncrement[eIndex] = iChange;
}

void CvCity::updateOngoingTraining(UnitCombatTypes eCombat)
{
	CvPlot* pPlot = plot();
	changeUnitCombatOngoingTrainingTimeCount(eCombat, 1);
	if (getUnitCombatOngoingTrainingTimeCount(eCombat) == getUnitCombatOngoingTrainingTimeIncrement(eCombat))
	{
		int iChange = -(getUnitCombatOngoingTrainingTimeCount(eCombat));
		changeUnitCombatOngoingTrainingTimeCount(eCombat, iChange);
		assignOngoingTraining(eCombat, pPlot);
	}
}

void CvCity::assignOngoingTraining(UnitCombatTypes eCombat, CvPlot* pPlot)
{
	PROFILE_FUNC();


	int iLowestValidity = MAX_INT;

	CvUnit* pBestUnit = NULL;
	for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
	{
		CvUnit* pLoopUnit = unitItr.ptr();
		if (pLoopUnit->getTeam() == getTeam())
		{
			if (pLoopUnit->isHasUnitCombat(eCombat))
			{
				int iCurrentValidity = pLoopUnit->getExperience() + pLoopUnit->getOngoingTrainingCount(eCombat);
				if (iCurrentValidity < iLowestValidity)
				{
					pBestUnit = pLoopUnit;
					iLowestValidity = iCurrentValidity;
				}
			} 
		}
	}
	if (pBestUnit != NULL)
	{
		pBestUnit->changeExperience(1);
		pBestUnit->changeOngoingTrainingCount(eCombat, 1);
	}
}

bool CvCity::assignPromotionChecked(PromotionTypes promotion, CvUnit* unit) const
{
	if (promotion != NO_PROMOTION &&
		((GC.getPromotionInfo(promotion).isEquipment() && canEquip(unit, promotion)) ||
		unit->canAcquirePromotion(promotion, false, false, false, true, false, false, true)))
	{
		unit->setHasPromotion(promotion, true);
		return true;
	}
	return false;
}

void CvCity::assignPromotionsFromBuildingChecked(const CvBuildingInfo &building, CvUnit* unit) const
{
	for (int promoTypeIdx = 0; promoTypeIdx < building.getNumFreePromoTypes(); ++promoTypeIdx)
	{
		const FreePromoTypes& freePromoType = building.getFreePromoType(promoTypeIdx);
		if (freePromoType.ePromotion != NO_PROMOTION &&
			((GC.getPromotionInfo(freePromoType.ePromotion).isEquipment() && canEquip(unit, freePromoType.ePromotion)) ||
			unit->canAcquirePromotion(freePromoType.ePromotion, false, false, false, true, false, false, true)))
		{
			if (!freePromoType.m_pExprFreePromotionCondition ||
				freePromoType.m_pExprFreePromotionCondition->evaluate(const_cast<CvGameObjectUnit*>(unit->getGameObjectConst())))
			{
				unit->setHasPromotion(freePromoType.ePromotion, true);
			}
		}
	}
}

bool CvCity::canEquip(CvUnit* pUnit, PromotionTypes eEquipment) const
{
	//Some of this could be a bit misleading if its not understood that the result should be true if its NOT an equipment at all.
	if (GC.getPromotionInfo(eEquipment).isEquipment())
	{
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if (pUnit->isHasPromotion((PromotionTypes)iI) && GC.getPromotionInfo((PromotionTypes)iI).isEquipment())
			{
				PromotionTypes eMayDeny = ((PromotionTypes)iI);
				if (GC.getPromotionInfo(eEquipment).getPromotionLine() != NO_PROMOTIONLINE && GC.getPromotionInfo(eMayDeny).getPromotionLine() != NO_PROMOTIONLINE)
				{
					if (GC.getPromotionInfo(eEquipment).getPromotionLine() == GC.getPromotionInfo(eMayDeny).getPromotionLine())
					{
						if (GC.getPromotionInfo(eEquipment).getLinePriority() <= GC.getPromotionInfo(eMayDeny).getLinePriority())
						{
							return false;
						}
					}
				}
			}
		}

		for (int iJ = 0; iJ < GC.getPromotionInfo(eEquipment).getNumNoAutoEquiptoCombatClassTypes(); iJ++)
		{
			UnitCombatTypes eNoAuto = (UnitCombatTypes)GC.getPromotionInfo(eEquipment).getNoAutoEquiptoCombatClassType(iJ);
			if (pUnit->isHasUnitCombat(eNoAuto))
			{
				return false;
			}
		}
	}
	return true;
}

//TB Combat Mods (Buildings) end
//TB Traits begin
int CvCity::getModifiedBaseYieldRate(YieldTypes eIndex) const
{
	int iModifiedYield = getBaseYieldRate(eIndex);
	iModifiedYield += GET_PLAYER(getOwnerINLINE()).getFreeCityYield(eIndex);

//Team Project (7)
	if (GET_PLAYER(getOwnerINLINE()).isGoldenAge())
	{
		if (GET_PLAYER(getOwnerINLINE()).getGoldenAgeYield(eIndex) > 0)
		{
			iModifiedYield += GET_PLAYER(getOwnerINLINE()).getGoldenAgeYield(eIndex);
		}
	}

	return iModifiedYield;
}
//TB Traits end
//TB Building Tags
//Team Project (3)
int CvCity::getExtraLocalCaptureProbabilityModifier() const
{
	return m_iExtraLocalCaptureProbabilityModifier;
}
void CvCity::setExtraLocalCaptureProbabilityModifier(int iValue)
{
	m_iExtraLocalCaptureProbabilityModifier = iValue;
}
void CvCity::changeExtraLocalCaptureProbabilityModifier(int iChange)
{
	m_iExtraLocalCaptureProbabilityModifier += iChange;
}

int CvCity::getExtraLocalCaptureResistanceModifier() const
{
	return m_iExtraLocalCaptureResistanceModifier;
}
void CvCity::setExtraLocalCaptureResistanceModifier(int iValue)
{
	m_iExtraLocalCaptureResistanceModifier = iValue;
}
void CvCity::changeExtraLocalCaptureResistanceModifier(int iChange)
{
	m_iExtraLocalCaptureResistanceModifier += iChange;
}

int CvCity::localCitizenCaptureResistance() const
{
	int iTotal = 0;
	iTotal += getExtraLocalCaptureResistanceModifier();
	iTotal += GET_PLAYER(getOwnerINLINE()).getExtraNationalCaptureResistanceModifier();
	return iTotal;
}

int CvCity::getExtraLocalDynamicDefense() const
{
	return m_iExtraLocalDynamicDefense;
}
void CvCity::setExtraLocalDynamicDefense(int iValue)
{
	m_iExtraLocalDynamicDefense = iValue;
}
void CvCity::changeExtraLocalDynamicDefense(int iChange)
{
	m_iExtraLocalDynamicDefense += iChange;
}

int CvCity::getExtraRiverDefensePenalty() const
{
	return m_iExtraRiverDefensePenalty;
}
void CvCity::setExtraRiverDefensePenalty(int iValue)
{
	m_iExtraRiverDefensePenalty = iValue;
}
void CvCity::changeExtraRiverDefensePenalty(int iChange)
{
	m_iExtraRiverDefensePenalty += iChange;
}

int CvCity::getExtraLocalRepel() const
{
	return m_iExtraLocalRepel;
}
void CvCity::setExtraLocalRepel(int iValue)
{
	m_iExtraLocalRepel = iValue;
}
void CvCity::changeExtraLocalRepel(int iChange)
{
	m_iExtraLocalRepel += iChange;
}

int CvCity::getExtraMinDefense() const
{
	return m_iExtraMinDefense;
}
void CvCity::setExtraMinDefense(int iValue)
{
	m_iExtraMinDefense = iValue;
}
void CvCity::changeExtraMinDefense(int iChange)
{
	m_iExtraMinDefense += iChange;
}

int CvCity::getExtraBuildingDefenseRecoverySpeedModifier() const
{
	return m_iExtraBuildingDefenseRecoverySpeedModifier;
}
void CvCity::setExtraBuildingDefenseRecoverySpeedModifier(int iValue)
{
	m_iExtraBuildingDefenseRecoverySpeedModifier = iValue;
}
void CvCity::changeExtraBuildingDefenseRecoverySpeedModifier(int iChange)
{
	m_iExtraBuildingDefenseRecoverySpeedModifier += iChange;
}

int CvCity::getModifiedBuildingDefenseRecoverySpeedCap() const
{
	return m_iModifiedBuildingDefenseRecoverySpeedCap;
}
void CvCity::setModifiedBuildingDefenseRecoverySpeedCap(int iValue)
{
	m_iModifiedBuildingDefenseRecoverySpeedCap = iValue;
}
void CvCity::changeModifiedBuildingDefenseRecoverySpeedCap(int iChange)
{
	m_iModifiedBuildingDefenseRecoverySpeedCap += iChange;
}

int CvCity::getExtraCityDefenseRecoverySpeedModifier() const
{
	return m_iExtraCityDefenseRecoverySpeedModifier;
}
void CvCity::setExtraCityDefenseRecoverySpeedModifier(int iValue)
{
	m_iExtraCityDefenseRecoverySpeedModifier = iValue;
}
void CvCity::changeExtraCityDefenseRecoverySpeedModifier(int iChange)
{
	m_iExtraCityDefenseRecoverySpeedModifier += iChange;
}

int CvCity::cityDefenseRecoveryRate() const
{
	int iValue = GC.getDefineINT("CITY_DEFENSE_DAMAGE_HEAL_RATE");

	int iRecoveryModifier = getExtraCityDefenseRecoverySpeedModifier();

	if (getDefenseModifier(false) < getModifiedBuildingDefenseRecoverySpeedCap())
	{
		iRecoveryModifier += getExtraBuildingDefenseRecoverySpeedModifier();
	}

	iValue *= (100 + iRecoveryModifier);
	iValue /= 100;

	return iValue;
}
	
bool CvCity::canDamageAttackingUnitCombat(UnitCombatTypes eUnitCombat) const
{
	FAssertMsg(eUnitCombat >= 0, "eUnitCombat expected to be >= 0");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat expected to be < GC.getNumUnitCombatInfos()");

	return (m_paiDamageAttackingUnitCombatCount[eUnitCombat] > 0);
}

int CvCity::getDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat) const
{
	FAssertMsg(eUnitCombat >= 0, "eUnitCombat expected to be >= 0");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat expected to be < GC.getNumUnitCombatInfos()");

	return m_paiDamageAttackingUnitCombatCount[eUnitCombat];
}

void CvCity::setDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat, int iValue)
{
	m_paiDamageAttackingUnitCombatCount[eUnitCombat] = iValue;
}

void CvCity::changeDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat, int iChange)
{
	FAssertMsg(eUnitCombat >= 0, "eUnitCombat expected to be >= 0");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat expected to be < GC.getNumUnitCombatInfos()");

	m_paiDamageAttackingUnitCombatCount[eUnitCombat] += iChange;
}

bool CvCity::canBuildingCostPopulation(BuildingTypes eBuilding) const
{
	return (m_paiBuildingCostPopulationCount[eBuilding] < 1);
}

int CvCity::getBuildingCostPopulation(BuildingTypes eBuilding) const
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	return m_paiBuildingCostPopulationCount[eBuilding];
}

void CvCity::setBuildingCostPopulation(BuildingTypes eBuilding, int iValue)
{
	m_paiBuildingCostPopulationCount[eBuilding] = iValue;
}

void CvCity::changeBuildingCostPopulationCount(BuildingTypes eBuilding, int iChange)
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	m_paiBuildingCostPopulationCount[eBuilding] += iChange;
}

//Team Project (1)
int CvCity::getTechSpecialistHappinessTypes(TechTypes eTech, SpecialistTypes eSpecialist) const
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	return m_ppaaiTechSpecialistHappinessTypes[eTech][eSpecialist];
}


void CvCity::changeTechSpecialistHappinessTypes(TechTypes eTech, SpecialistTypes eSpecialist, int iChange)
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	if (iChange != 0)
	{
		m_ppaaiTechSpecialistHappinessTypes[eTech][eSpecialist] = (m_ppaaiTechSpecialistHappinessTypes[eTech][eSpecialist] + iChange);

		updateExtraTechSpecialistHappiness();

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getTechSpecialistHappiness(TechTypes eTech) const
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	return m_paiTechSpecialistHappiness[eTech];
}


void CvCity::changeTechSpecialistHappiness(TechTypes eTech, int iChange)
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");

	if (iChange != 0)
	{
		m_paiTechSpecialistHappiness[eTech] = (m_paiTechSpecialistHappiness[eTech] + iChange);

		updateExtraTechSpecialistHappiness();

		AI_setAssignWorkDirty(true);
	}
}

void CvCity::updateExtraTechSpecialistHappiness()
{
	int iBaseSpecialistCount = 0;
	int iRunningTotal = 0;

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		SpecialistTypes eSpecialist = ((SpecialistTypes)iI);
		iBaseSpecialistCount += specialistCount(eSpecialist);
		int iSpecificSpecialistCount = specialistCount(eSpecialist);
		for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
		{
			TechTypes eTech = ((TechTypes)iJ);
			if (GET_TEAM(getTeam()).isHasTech(eTech))
			{
				iRunningTotal += iSpecificSpecialistCount * getTechSpecialistHappinessTypes(eTech, eSpecialist);
				for (int iK = 0; iK < GC.getSpecialistInfo(eSpecialist).getNumTechHappinessTypes(); iK++)
				{
					if (GC.getSpecialistInfo(eSpecialist).getTechHappinessType(iK).eTech == eTech)
					{
						iRunningTotal += iSpecificSpecialistCount * GC.getSpecialistInfo(eSpecialist).getTechHappinessType(iK).iModifier;
					}
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		TechTypes eTech = ((TechTypes)iI);
		if (GET_TEAM(getTeam()).isHasTech(eTech))
		{
			iRunningTotal += iBaseSpecialistCount * getTechSpecialistHappiness(eTech);
		}
	}

	m_iExtraTechSpecialistHappiness = iRunningTotal;
}

int CvCity::getExtraTechSpecialistHappiness() const
{
	return m_iExtraTechSpecialistHappiness;
}

int CvCity::getTechHappiness(TechTypes eTech) const
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	return m_paiTechHappiness[eTech];
}


void CvCity::changeTechHappiness(TechTypes eTech, int iChange)
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");

	if (iChange != 0)
	{
		m_paiTechHappiness[eTech] = (m_paiTechHappiness[eTech] + iChange);

		updateExtraTechHappiness();
	}
}

void CvCity::updateExtraTechHappiness()
{
	int iRunningTotal = 0;

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		TechTypes eTech = ((TechTypes)iI);
		if (GET_TEAM(getTeam()).isHasTech(eTech))
		{
			iRunningTotal += getTechHappiness(eTech);
		}
	}
	m_iExtraTechHappiness = iRunningTotal;
}

int CvCity::getExtraTechHappiness() const
{
	return m_iExtraTechHappiness;
}

int CvCity::getExtraTechHappinessTotal() const
{
	int iTotal = 0;
	iTotal += getExtraTechSpecialistHappiness();
	iTotal += getExtraTechHappiness();
	int iGrandTotal = std::max(0, iTotal);
	return (iGrandTotal);
}

int CvCity::getExtraTechUnHappinessTotal() const
{
	int iTotal = 0;
	iTotal += getExtraTechSpecialistHappiness();
	iTotal += getExtraTechHappiness();
	int iGrandTotal = std::max(0, -iTotal);
	return (iGrandTotal);
}

void CvCity::updateTechHappinessandHealth()
{
	updateExtraTechHappiness();
	updateExtraTechSpecialistHappiness();
	updateExtraTechHealth();
	updateExtraTechSpecialistHealth();

	AI_setAssignWorkDirty(true);

	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		setInfoDirty(true);
	}
}

int CvCity::getTechSpecialistHealthTypes(TechTypes eTech, SpecialistTypes eSpecialist) const
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	return m_ppaaiTechSpecialistHealthTypes[eTech][eSpecialist];
}


void CvCity::changeTechSpecialistHealthTypes(TechTypes eTech, SpecialistTypes eSpecialist, int iChange)
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	if (iChange != 0)
	{
		m_ppaaiTechSpecialistHealthTypes[eTech][eSpecialist] = (m_ppaaiTechSpecialistHealthTypes[eTech][eSpecialist] + iChange);

		updateExtraTechSpecialistHealth();

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getTechSpecialistHealth(TechTypes eTech) const
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	return m_paiTechSpecialistHealth[eTech];
}


void CvCity::changeTechSpecialistHealth(TechTypes eTech, int iChange)
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");

	if (iChange != 0)
	{
		m_paiTechSpecialistHealth[eTech] = (m_paiTechSpecialistHealth[eTech] + iChange);

		updateExtraTechSpecialistHealth();

		AI_setAssignWorkDirty(true);
	}
}

void CvCity::updateExtraTechSpecialistHealth()
{
	int iBaseSpecialistCount = 0;
	int iRunningTotal = 0;

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		SpecialistTypes eSpecialist = ((SpecialistTypes)iI);
		iBaseSpecialistCount += specialistCount(eSpecialist);
		int iSpecificSpecialistCount = specialistCount(eSpecialist);
		for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
		{
			TechTypes eTech = ((TechTypes)iJ);
			if (GET_TEAM(getTeam()).isHasTech(eTech))
			{
				iRunningTotal += iSpecificSpecialistCount * getTechSpecialistHealthTypes(eTech, eSpecialist);
				for (int iK = 0; iK < GC.getSpecialistInfo(eSpecialist).getNumTechHealthTypes(); iK++)
				{
					TechTypes kTech = GC.getSpecialistInfo(eSpecialist).getTechHealthType(iK).eTech;
					if (kTech == eTech)
					{
						iRunningTotal += iSpecificSpecialistCount * GC.getSpecialistInfo(eSpecialist).getTechHealthType(iK).iModifier;
					}
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		TechTypes eTech = ((TechTypes)iI);
		if (GET_TEAM(getTeam()).isHasTech(eTech))
		{
			iRunningTotal += iBaseSpecialistCount * getTechSpecialistHealth(eTech);
		}
	}

	m_iExtraTechSpecialistHealth = iRunningTotal;
}

int CvCity::getExtraTechSpecialistHealth() const
{
	return m_iExtraTechSpecialistHealth;
}

int CvCity::getTechHealth(TechTypes eTech) const
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");
	return m_paiTechHealth[eTech];
}


void CvCity::changeTechHealth(TechTypes eTech, int iChange)
{
	FAssertMsg(eTech >= 0, "eTech expected to be >= 0");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech expected to be < GC.getNumTechInfos()");

	if (iChange != 0)
	{
		m_paiTechHealth[eTech] = (m_paiTechHealth[eTech] + iChange);

		updateExtraTechHealth();
	}
}

void CvCity::updateExtraTechHealth()
{
	int iRunningTotal = 0;

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		TechTypes eTech = ((TechTypes)iI);
		if (GET_TEAM(getTeam()).isHasTech(eTech))
		{
			iRunningTotal += getTechHealth(eTech);
		}
	}
	m_iExtraTechHealth = iRunningTotal;
}

int CvCity::getExtraTechHealth() const
{
	return m_iExtraTechHealth;
}

int CvCity::getExtraTechHealthTotal() const
{
	int iTotal = 0;
	iTotal += getExtraTechSpecialistHealth();
	iTotal += getExtraTechHealth();
	int iGrandTotal = std::max(0, iTotal);
	return (iGrandTotal);
}

int CvCity::getExtraTechUnHealthTotal() const
{
	int iTotal = 0;
	iTotal += getExtraTechSpecialistHealth();
	iTotal += getExtraTechHealth();
	int iGrandTotal = std::max(0, -iTotal);
	return (iGrandTotal);
}

int CvCity::getLocalSpecialistExtraYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");

	return m_ppaaiLocalSpecialistExtraYield[eSpecialist][eYield];
}

void CvCity::changeLocalSpecialistExtraYield(SpecialistTypes eSpecialist, YieldTypes eYield, int iChange)
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	if (iChange != 0)
	{
		m_ppaaiLocalSpecialistExtraYield[eSpecialist][eYield] = (m_ppaaiLocalSpecialistExtraYield[eSpecialist][eYield] + iChange);
	}

	updateExtraSpecialistYield();

	AI_setAssignWorkDirty(true);
}

int CvCity::getLocalSpecialistExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eCommerce >= 0, "eCommerce expected to be >= 0");
	FAssertMsg(eCommerce < NUM_COMMERCE_TYPES, "eCommerce expected to be < NUM_COMMERCE_TYPES");

	return m_ppaaiLocalSpecialistExtraCommerce[eSpecialist][eCommerce];
}

void CvCity::changeLocalSpecialistExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce, int iChange)
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eCommerce >= 0, "eCommerce expected to be >= 0");
	FAssertMsg(eCommerce < NUM_COMMERCE_TYPES, "eYield expected to be < NUM_COMMERCE_TYPES");
	if (iChange != 0)
	{
		m_ppaaiLocalSpecialistExtraCommerce[eSpecialist][eCommerce] = (m_ppaaiLocalSpecialistExtraCommerce[eSpecialist][eCommerce] + iChange);
	}

	updateExtraSpecialistCommerce();

	AI_setAssignWorkDirty(true);
}

int CvCity::specialistCount(SpecialistTypes eSpecialist) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	int iTotal = getSpecialistCount(eSpecialist);
	iTotal += getFreeSpecialistCount(eSpecialist);

	return iTotal;
}

int CvCity::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");

	int iTotal = GET_PLAYER(getOwnerINLINE()).specialistYield(eSpecialist, eYield);
	iTotal += getLocalSpecialistExtraYield(eSpecialist, eYield);

	return iTotal;
}

int CvCity::specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eCommerce >= 0, "eCommerce expected to be >= 0");
	FAssertMsg(eCommerce < NUM_COMMERCE_TYPES, "eCommerce expected to be < NUM_COMMERCE_TYPES");

	int iTotal = GET_PLAYER(getOwnerINLINE()).specialistCommerce(eSpecialist, eCommerce);
	iTotal += getLocalSpecialistExtraCommerce(eSpecialist, eCommerce);

	return iTotal;
}

int CvCity::specialistYieldTotal(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");

	return (specialistCount(eSpecialist) * specialistYield(eSpecialist, eYield));
}

int CvCity::getPrioritorizedSpecialist() const
{
	return m_iPrioritySpecialist;
}

void CvCity::setPrioritorizedSpecialist(SpecialistTypes eSpecialist)
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	m_iPrioritySpecialist = (int)eSpecialist;
	if (isSpecialistBanned(eSpecialist))
	{
		removeSpecialistBan(eSpecialist);
	}
}

bool CvCity::isSpecialistBanned(SpecialistTypes eSpecialist)
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	return (m_paiSpecialistBannedCount[eSpecialist] > 0);
}

void CvCity::banSpecialist(SpecialistTypes eSpecialist)
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	m_paiSpecialistBannedCount[eSpecialist] = 1;
	if (m_iPrioritySpecialist == (int)eSpecialist)
	{
		m_iPrioritySpecialist = NO_SPECIALIST;
	}
}

void CvCity::removeSpecialistBan(SpecialistTypes eSpecialist)
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	m_paiSpecialistBannedCount[eSpecialist] = 0;
}

bool CvCity::isDirectAttackable() const
{
	int iMinimumDefenseLevel = getMinimumDefenseLevel();
	if (iMinimumDefenseLevel == 0)
	{
		iMinimumDefenseLevel = MAX_INT;
	}

	//ls612 Quickfix: Cities can be attacked if they have no defenders, regardless of if there is a minimun defense level
	if (getDefenseModifier(false) > iMinimumDefenseLevel && plot()->getNumDefenders(getOwnerINLINE()) > 0)
	{
		return false;
	}

	return true;
}

int CvCity::getInvestigationTotal(bool bActual) const
{
	int iTotal = 0;
	int iBestUnitInvestigation = 0;
	int iAssistance = 0;
	int iFivePercentAssistance = 0;

	if (plot() != NULL)
	{
		CvUnit* pBestUnit = NULL;
		CvPlot* pPlot = plot();
		for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
		{
			CvUnit* pLoopUnit = unitItr.ptr();

			int iUnitInvestigation = 0;
			if (pLoopUnit->getOwner() == getOwner())
			{
				iUnitInvestigation = pLoopUnit->getInvestigationTotal();
			}
			if (iUnitInvestigation > iBestUnitInvestigation)
			{
				iBestUnitInvestigation = iUnitInvestigation;
				pBestUnit = pLoopUnit;
			}
			if (iUnitInvestigation > 0)
			{
				iAssistance++;
				iFivePercentAssistance += iUnitInvestigation;
			}
		}
		if (bActual && pBestUnit != NULL)
		{
			iAssistance--;//To remove the bonus one would give itself.
			iFivePercentAssistance -= iBestUnitInvestigation;
			pBestUnit->changeExperience100(5);
		}
	}
	iFivePercentAssistance /= 20;
	iTotal += iAssistance;
	iTotal += iFivePercentAssistance;
	iTotal += iBestUnitInvestigation;
	iTotal += getExtraInvestigation();
	iTotal += getSpecialistInvestigation();

	return iTotal;
}

int CvCity::getExtraInsidiousness() const
{
	return m_iExtraInsidiousness;
}
void CvCity::setExtraInsidiousness(int iValue)
{
	m_iExtraInsidiousness = iValue;
}
void CvCity::changeExtraInsidiousness(int iChange)
{
	m_iExtraInsidiousness += iChange;
}

int CvCity::getExtraInvestigation() const
{
	return m_iExtraInvestigation;
}
void CvCity::setExtraInvestigation(int iValue)
{
	m_iExtraInvestigation = iValue;
}
void CvCity::changeExtraInvestigation(int iChange)
{
	m_iExtraInvestigation += iChange;
}

int CvCity::getSpecialistInsidiousness() const
{
	return m_iSpecialistInsidiousness;
}
void CvCity::changeSpecialistInsidiousness(int iChange)
{
	m_iSpecialistInsidiousness += iChange;
	FAssert(m_iSpecialistInsidiousness >= 0);
}

int CvCity::getSpecialistInvestigation() const
{
	return m_iSpecialistInvestigation;
}
void CvCity::changeSpecialistInvestigation(int iChange)
{
	m_iSpecialistInvestigation += iChange;
	FAssert(m_iSpecialistInvestigation >= 0);
}

int CvCity::getPropertyNeed(PropertyTypes eProperty)
{
	if (m_icachedPropertyNeedsTurn != GC.getGameINLINE().getGameTurn() || NULL == m_cachedPropertyNeeds)
	{
		EraTypes eEra = GET_PLAYER(getOwner()).getCurrentEra();

		if (NULL == m_cachedPropertyNeeds)
		{
			m_cachedPropertyNeeds = new int[GC.getNumPropertyInfos()];
			for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
			{
				m_cachedPropertyNeeds[iI] = 0;
			}
		}
		for( int iI= 0; iI < GC.getNumPropertyInfos(); iI++)
		{
			PropertyTypes pProperty = (PropertyTypes)iI;
			if ( GC.getPropertyInfo(pProperty).getAIWeight() != 0 )
			{
				int iCurrentValue = getPropertiesConst()->getValueByProperty(pProperty);
				//TB attempt to allow some modification to need based on existing drift value
				int iCurrentSourceSize = getTotalBuildingSourcedProperty(eProperty) + getTotalUnitSourcedProperty(eProperty);
				iCurrentSourceSize *= 4;
				iCurrentValue += iCurrentSourceSize;
				//
				int iTarget = 0;
				if (GC.getPropertyInfo(pProperty).isTargetLevelbyEraType((int)eEra))
				{
					iTarget = GC.getPropertyInfo(pProperty).getTargetLevelbyEraType((int)eEra);
				}
				else
				{
					iTarget = GC.getPropertyInfo(pProperty).getTargetLevel();
				}
				int iNeed = iTarget - iCurrentValue;
				//value always positive if need is real
				m_cachedPropertyNeeds[iI] = (iNeed * (GC.getPropertyInfo(pProperty).getAIWeight()/50));
			}
		}
		m_icachedPropertyNeedsTurn = GC.getGameINLINE().getGameTurn();
	}
	int iIndex = (int)eProperty;

	return (m_cachedPropertyNeeds[iIndex]);
}

int CvCity::getNumPropertySpawns() const
{
	return (int)m_aPropertySpawns.size();
}

PropertySpawns& CvCity::getPropertySpawn(int iIndex)
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < getNumPropertySpawns());
	return m_aPropertySpawns[iIndex];
}

void CvCity::changePropertySpawn(int iChange, PropertyTypes eProperty, UnitClassTypes eUnitClass)
{
	bool bAdding = (iChange > 0);
	if (bAdding)
	{
		if (NO_PROPERTY != eProperty && NO_UNITCLASS != eUnitClass)
		{
			PropertySpawns kChange;
			kChange.eProperty = eProperty;
			kChange.eUnitClass = eUnitClass;
			m_aPropertySpawns.push_back(kChange);
		}
	}
	else
	{
		std::vector<PropertySpawns> m_aTempPropertySpawns;
		for (std::vector<PropertySpawns>::iterator it = m_aPropertySpawns.begin(); it != m_aPropertySpawns.end(); ++it)
		{
			if ((*it).eProperty != eProperty || (*it).eUnitClass != eUnitClass)
			{
				PropertySpawns kChange;
				kChange.eProperty = (*it).eProperty ;
				kChange.eUnitClass = (*it).eUnitClass;
				m_aTempPropertySpawns.push_back(kChange);
			}
		}
		m_aPropertySpawns.clear();
		for (std::vector<PropertySpawns>::iterator it = m_aTempPropertySpawns.begin(); it != m_aTempPropertySpawns.end(); ++it)
		{
			PropertySpawns kChange;
			kChange.eProperty = (*it).eProperty ;
			kChange.eUnitClass = (*it).eUnitClass;
			m_aPropertySpawns.push_back(kChange);
		}
		m_aTempPropertySpawns.clear();
	}
}

void CvCity::doPropertyUnitSpawn()
{
	int	iNumCriminals = plot()->getNumCriminals();
	int iMaximum = getPopulation()/2;
	if (iNumCriminals < iMaximum)
	{
		for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
		{
			PropertyTypes eProperty = (PropertyTypes)iI;
			bool bPositiveProperty = (GC.getPropertyInfo(eProperty).getAIWeight() >= 0);
			int iCurrentValue = std::max(0, getPropertiesConst()->getValueByProperty(eProperty));
			if (eProperty == 0) //SHOULD be crime but this is subject to flaw if the first property type ever changes.  There's a faster way than getvisual but it takes some setup.  if this becomes necessary to move off of hard coding, use the examples for peaks and hills.
			{
				iCurrentValue -= ((iCurrentValue/10) * iNumCriminals);
			}
			iCurrentValue = std::max(0,iCurrentValue);
			iCurrentValue /= 2;
			PlayerTypes eSpawnOwner = getOwner();
			if (!bPositiveProperty)
			{
				eSpawnOwner = (PlayerTypes)BARBARIAN_PLAYER;
			}
			for (std::vector<PropertySpawns>::iterator it = m_aPropertySpawns.begin(); it != m_aPropertySpawns.end(); ++it)
			{
				if ((*it).eProperty == eProperty)
				{
					UnitClassTypes eUnitClass = (*it).eUnitClass;
					if(GC.getGameINLINE().getSorenRandNum(10000, "Property Unit Spawn Check") < iCurrentValue)
					{
						UnitTypes eUnit = ((UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass));

						if ( !GET_PLAYER(getOwnerINLINE()).canTrain(eUnit, false, false, true, true) )
						{
							continue;
						}
						//TBNote:Saving this brilliant example...
						//std::vector<int> aiUnitAIIndex;
						//aiUnitAIIndex.clear();
						//for (int iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
						//{
						//	if (GC.getUnitInfo(eUnit).getUnitAIType(iJ))
						//	{
						//		aiUnitAIIndex.push_back(iJ);
						//	}
						//}
						//int iAIRoll = GC.getGameINLINE().getSorenRandNum(aiUnitAIIndex.size(), "Property Unit Spawn AI Check");
						//UnitAITypes eUnitAI = (UnitAITypes)aiUnitAIIndex[iAIRoll];

						FAssertMsg(GC.getUnitInfo(eUnit).isBlendIntoCity(), CvString::format("Trying to spawn %s from property spawn, but it doesn't have bBlendIntoCity enabled, which is a requirement", GC.getUnitInfo(eUnit).getType()).c_str());

						CvUnit* pUnit = GET_PLAYER(eSpawnOwner).initUnit(eUnit, getX_INLINE(), getY_INLINE(), UNITAI_BARB_CRIMINAL, NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));

						if (pUnit != NULL)
						{

							FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");
							if (pUnit->isExcile())
							{
								pUnit->jumpToNearestValidPlot(false);
							}
							pUnit->finishMoves();

							addProductionExperience(pUnit);

							if (!bPositiveProperty)
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_PROPERTY_SPAWN_BARB", GC.getUnitInfo(eUnit).getDescription(), getNameKey());

								if (!GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS))
								{
									MEMORY_TRACK_EXEMPT();
			
									AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
								}
							}
							else
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_PROPERTY_SPAWN_FRIENDLY", GC.getUnitInfo(eUnit).getDescription(), getNameKey());

								if (!GET_PLAYER(getOwnerINLINE()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS))
								{
									MEMORY_TRACK_EXEMPT();
			
									AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
								}
							}
						}
					}
				}
			}
		}
	}
}

bool CvCity::isQuarantined() const
{
	return (getQuarantinedCount() > 0);
}

int CvCity::getQuarantinedCount() const
{
	return m_iQuarantinedCount;
}

void CvCity::changeQuarantinedCount(int iChange)
{
	m_iQuarantinedCount += iChange;
}

void CvCity::resetQuarantinedCount()
{
	m_iQuarantinedCount = 0;
}

void CvCity::flushBestSeeInvisibleUnitCache()
{
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		m_aiCachedBestSeeInvisibleUnit[iI] = -1;
		m_abCachedBestSeeInvisibleUnit[iI] = false;
		m_aiCachedBestSeeInvisibleUnitSea[iI] = -1;
		m_abCachedBestSeeInvisibleUnitSea[iI] = false;
	}
}

void CvCity::setBestSeeInvisibleUnitCache(InvisibleTypes eInvisible, UnitTypes eUnitType, bool bSea)
{
	int iI = (int)eInvisible;
	if (bSea)
	{
		m_aiCachedBestSeeInvisibleUnitSea[iI] = (int)eUnitType;
		m_abCachedBestSeeInvisibleUnitSea[iI] = true;
		return;
	}
	m_aiCachedBestSeeInvisibleUnit[iI] = (int)eUnitType;
	m_abCachedBestSeeInvisibleUnit[iI] = true;
}

UnitTypes CvCity::getCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea) const
{
	int iI = (int)eInvisible;
	if (bSea)
	{
		return (UnitTypes)m_aiCachedBestSeeInvisibleUnitSea[iI];
	}
	return (UnitTypes)m_aiCachedBestSeeInvisibleUnit[iI];
}

bool CvCity::isCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea) const
{
	int iI = (int)eInvisible;
	if (bSea)
	{
		return m_abCachedBestSeeInvisibleUnitSea[iI];
	}
	return m_abCachedBestSeeInvisibleUnit[iI];
}

void CvCity::AI_setPropertyControlBuildingQueued(bool bSet)
{
	m_bPropertyControlBuildingQueued = bSet;
}

bool CvCity::AI_isPropertyControlBuildingQueued() const
{
	return m_bPropertyControlBuildingQueued;
}