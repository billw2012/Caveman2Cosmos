// cityAI.cpp

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

//	KOSHLING MOD - calculate all possible building focuses at once
//	to avoid multiple looping - need to know how many options there
//	are.  Note that I am also ASSUMING only one flag is passed at a
//	time (and runtime checking that)
#define BUILDINGFOUCSINDEX_ANY					0
#define BUILDINGFOCUSINDEX_FOOD					1
#define BUILDINGFOCUSINDEX_PRODUCTION			2
#define BUILDINGFOCUSINDEX_GOLD					3
#define BUILDINGFOCUSINDEX_RESEARCH				4
#define BUILDINGFOCUSINDEX_CULTURE				5
#define BUILDINGFOCUSINDEX_DEFENSE				6
#define BUILDINGFOCUSINDEX_HAPPY				7
#define BUILDINGFOCUSINDEX_HEALTHY				8
#define BUILDINGFOCUSINDEX_EXPERIENCE			9
#define BUILDINGFOCUSINDEX_MAINTENANCE			10
#define BUILDINGFOCUSINDEX_SPECIALIST			11
#define BUILDINGFOCUSINDEX_ESPIONAGE			12
#define BUILDINGFOCUSINDEX_BIGCULTURE			13
#define BUILDINGFOCUSINDEX_WORLDWONDER			14
#define BUILDINGFOCUSINDEX_DOMAINSEA			15
#define BUILDINGFOCUSINDEX_WONDEROK				16
#define BUILDINGFOCUSINDEX_CAPITAL				17

#define NUM_REAL_BUILDINGFOCUS_FLAGS			18
//	Pseudo flags which represent combinations used in the calculation
//	and are not independent so have to be accumulated separately
#define BUILDINGFOCUSINDEX_GOLDANDRESEARCH		18
#define BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE	19

#define NUM_ALL_BUILDINGFOCUS_FLAGS				20

//	Helper function to detrmine if a given bonus is provided by a building
static bool isFreeBonusOfBuilding(CvBuildingInfo& kBuilding, BonusTypes eBonus)
{
	return kBuilding.isFreeBonusOfBuilding(eBonus);
}

// Public Functions...

CvCityAI::CvCityAI()
{
	m_aiEmphasizeYieldCount = new int[NUM_YIELD_TYPES];
	m_aiEmphasizeCommerceCount = new int[NUM_COMMERCE_TYPES];
	m_bForceEmphasizeCulture = false;
	m_aiSpecialYieldMultiplier = new int[NUM_YIELD_TYPES];
	m_aiPlayerCloseness = new int[MAX_PLAYERS];

	m_pbEmphasize = NULL;
	m_pbEmphasizeSpecialist = NULL;
	cachedBuildingValues = NULL;

	AI_reset();
}


CvCityAI::~CvCityAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiEmphasizeYieldCount);
	SAFE_DELETE_ARRAY(m_aiEmphasizeCommerceCount);
	SAFE_DELETE_ARRAY(m_aiSpecialYieldMultiplier);
	SAFE_DELETE_ARRAY(m_aiPlayerCloseness);
}


void CvCityAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
	AI_assignWorkingPlots();

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/14/09                                jdog5000      */
/*                                                                                              */
/* City AI, Worker AI                                                                           */
/************************************************************************************************/
/* original bts code
	AI_updateWorkersNeededHere();
	
	AI_updateBestBuild();
*/
	AI_updateBestBuild();

	AI_updateWorkersNeededHere();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
}


void CvCityAI::AI_uninit()
{
	AI_FlushBuildingValueCache();

	SAFE_DELETE_ARRAY(m_pbEmphasize);
	SAFE_DELETE_ARRAY(m_pbEmphasizeSpecialist);
}


// FUNCTION: AI_reset()
// Initializes data members that are serialized.
void CvCityAI::AI_reset()
{
	AI_uninit();

	m_iEmphasizeAvoidGrowthCount = 0;
	m_iEmphasizeGreatPeopleCount = 0;
	m_iUnitSummonedLastEstablishedTurn = 0;
	m_bForceEmphasizeCulture = false;
/************************************************************************************************/
/* Afforess	                  Start		 02/10/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_iEmphasizeAvoidAngryCitizensCount = 0;
	m_iEmphasizeAvoidUnhealthyCitizensCount = 0;
	m_bMilitaryProductionCity = false;
	m_bNavalMilitaryProductionCity = false;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	m_bAssignWorkDirty = false;
	m_bChooseProductionDirty = false;

	m_routeToCity.reset();

	m_bestBuildValuesStale = true;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiEmphasizeYieldCount[iI] = 0;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiEmphasizeCommerceCount[iI] = 0;
	}

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aiBestBuildValue[iI] = NO_BUILD;
	}

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aeBestBuild[iI] = NO_BUILD;
	}
	
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSpecialYieldMultiplier[iI] = 0;
	}
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiPlayerCloseness[iI] = 0;
	}
	m_iCachePlayerClosenessTurn = -1;
	m_iCachePlayerClosenessDistance = -1;
	
	m_iNeededFloatingDefenders = -1;
	m_iNeededFloatingDefendersCacheTurn = -1;
	m_neededDefenders = -1;

	m_iWorkersNeeded = 0;
	m_iWorkersHave = 0;

	m_iBuildPriority = CITY_BUILD_PRIORITY_CEILING;
	m_bRequestedUnit = false;
	m_bRequestedBuilding = false;

	FAssertMsg(m_pbEmphasize == NULL, "m_pbEmphasize not NULL!!!");
	FAssertMsg(GC.getNumEmphasizeInfos() > 0,  "GC.getNumEmphasizeInfos() is not greater than zero but an array is being allocated in CvCityAI::AI_reset");
	m_pbEmphasize = new bool[GC.getNumEmphasizeInfos()];
	for (int iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		m_pbEmphasize[iI] = false;
	}

	FAssertMsg(m_pbEmphasizeSpecialist == NULL, "m_pbEmphasizeSpecialist not NULL!!!");
	FAssertMsg(GC.getNumSpecialistInfos() > 0,  "GC.getNumSpecialistInfos() is not greater than zero but an array is being allocated in CvCityAI::AI_reset");
	m_pbEmphasizeSpecialist = new bool[GC.getNumSpecialistInfos()];
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_pbEmphasizeSpecialist[iI] = false;
	}

#ifdef YIELD_VALUE_CACHING
	ClearYieldValueCacheImpl();
#endif

	m_iCityThreat = -1;
}

//	KOSHLING - initialisation called on every city prior to performing unit mission allocation logic
//	This allows caches that will remain valid for the processing of the current turn's units to be cleared
void CvCityAI::AI_preUnitTurn()
{
	PROFILE_FUNC();

	m_neededDefenders = -1;
	m_requestedEscorts = 0;

	m_iCityThreat = -1;

	m_bestUnits.clear();
}

void CvCityAI::AI_noteUnitEscortNeeded()
{
	m_requestedEscorts++;
}

void CvCityAI::AI_trained(UnitTypes eUnitType, UnitAITypes eUnitAIType)
{
}

void CvCityAI::AI_doTurn()
{
	PROFILE_FUNC();

	m_buildValueCache.clear();

	if (!isHuman())
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			setForceSpecialistCount(((SpecialistTypes)iI), 0);
		}

		AI_stealPlots();
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/14/09                                jdog5000      */
/*                                                                                              */
/* City AI, Worker AI                                                                           */
/************************************************************************************************/
/* original bts code
	AI_updateWorkersNeededHere();
	
	AI_updateBestBuild();
*/
	AI_updateBestBuild();

	AI_updateWorkersNeededHere();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	//AI_updateRouteToCity();
	m_routeToCityUpdated = false;	//	Update on demand

	if (isHuman())
	{
		if (isProductionAutomated())
		{
			AI_doHurry();	        
		}
		return;
	}
	
	AI_doPanic();

	AI_doDraft();

	AI_doHurry();

	AI_doEmphasize();

	AI_doContractFloatingDefenders();
}


void CvCityAI::AI_doContractFloatingDefenders()
{
	PROFILE_FUNC();

	if (gCityLogLevel >= 2)
	{
		int iOurThreat = AI_cityThreat();
		int iLargestThreat = 0;
		int iTotalThreat = std::max(1, GET_PLAYER(getOwnerINLINE()).AI_getTotalAreaCityThreat(area(), &iLargestThreat));
		logBBAI("      City %S has threat level %d (highest %d, total %d)", getName().GetCString(), iOurThreat, iLargestThreat, iTotalThreat);
	}

	//	If below optimal garrison size request extra troops
	//	to move to us with a priority based on our threat level
	//	The highest priority we assign to this is FLOATING_DEFENDER_REQUEST_PRIORITY_MAX
	//	which is below the lowest building priority, and will thus soak up existing
	//	troops without local work without causing new production
	if ( !AI_isDefended(0, false) )
	{
		//	Assess our priority
		int iGarrisonStrength = getGarrisonStrength(false);
		int iMinStrength = AI_minDefenseStrength();
		int iOurPriority;
		int iRequiredStrength;

		//	If we don't even have minimum strength pull in a few defenders at higher priority
		// TB note: !isCapital() added so as to give the AI a way to ignore the need for a while as it develops its first city more efficiently at first.
		if ( iGarrisonStrength < iMinStrength && !isCapital())
		{
			iOurPriority = CITY_BUILD_PRIORITY_CEILING;
			iRequiredStrength = iMinStrength - iGarrisonStrength;				
		}
		else if ( iGarrisonStrength < iMinStrength && isCapital())
		{
			iOurPriority = MINIMUM_CAPITAL_DEFENDER_PRIORITY;
			iRequiredStrength = iMinStrength - iGarrisonStrength;
		}
		else
		{
			int iOurThreat = AI_cityThreat();
			int iLargestThreat = 0;
			GET_PLAYER(getOwnerINLINE()).AI_getTotalAreaCityThreat(area(), &iLargestThreat);

			iOurPriority = (MINIMUM_CITY_DEFENDER_PRIORITY * iOurThreat)/std::max(1,iLargestThreat);
			iRequiredStrength = AI_neededDefenseStrength() - iGarrisonStrength;
		}

		FAssert(iRequiredStrength > 0);

		if( gCityLogLevel >= 2 )
		{
			logBBAI("      City %S requests %d floating defender strength at priority %d", getName().GetCString(), iRequiredStrength, iOurPriority);
		}
		
		UnitAITypes eFloatingDefenderUnitAI = NO_UNITAI;
		if (iGarrisonStrength < iMinStrength)
		{
			eFloatingDefenderUnitAI = UNITAI_CITY_DEFENSE;
		}
		GET_PLAYER(getOwnerINLINE()).getContractBroker().advertiseWork(iOurPriority,
																	   DEFENSIVE_UNITCAPABILITIES,
																	   getX_INLINE(),
																	   getY_INLINE(),
																	   NULL,
																	   eFloatingDefenderUnitAI,
																	   iRequiredStrength);
	}
	else
	{
		if( gCityLogLevel >= 2 )
		{
			logBBAI("      City %S adequately defended", getName().GetCString());
		}
	}
}


void CvCityAI::AI_assignWorkingPlots()
{
	PROFILE_FUNC();

	if (0 != GC.getDefineINT("AI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT"))
	{
		return;
	}

	//	Koshling - I can't see the point in scrapping the current worker setup
	//	each time (it's very costly in performance terms), and the eventual
	//	AI_juggleCitizens() in what follows will adjust the allocation if priorities
	//	have changed.  Hence changing it to only stop working plots that are
	//	now invalid as worked plots
	//	Empirically it does make a small difference to the eventual assignments
	//	but it seems to be a neither-here-nor-there difference between two approximations
	//	to optimality - better to choose the low-overhead one!
#ifdef OLD_BTS_EVALUATION
	// remove all assigned plots if we automated
	if (!isHuman() || isCitizensAutomated())
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			setWorkingPlot(iI, false);
		}
	}
#endif
	
	//update the special yield multiplier to be current
	AI_updateSpecialYieldMultiplier();
	
	//	If we're in disorder no need to go through all of this, but we must call updateCommerce()
	//	directly to have the disorder take effect on the commerce of this city
	if ( isDisorder() )
	{
		setCommerceDirty();
	}
	else
	{
		// remove any plots we can no longer work for any reason
		verifyWorkingPlots();
		
		// if forcing specialists, try to make all future specialists of the same type
		bool bIsSpecialistForced = false;
		int iTotalForcedSpecialists = 0;

		// make sure at least the forced amount of specialists are assigned
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			int iForcedSpecialistCount = getForceSpecialistCount((SpecialistTypes)iI);
			if (iForcedSpecialistCount > 0)
			{
				bIsSpecialistForced = true;
				iTotalForcedSpecialists += iForcedSpecialistCount;
			}
			
			if (!isHuman() || isCitizensAutomated() || (getSpecialistCount((SpecialistTypes)iI) < iForcedSpecialistCount))
			{
				setSpecialistCount(((SpecialistTypes)iI), iForcedSpecialistCount);
			}
		}
		
		// if we have more specialists of any type than this city can have, reduce to the max
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (!isSpecialistValid((SpecialistTypes)iI))
			{
				if (getSpecialistCount((SpecialistTypes)iI) > getMaxSpecialistCount((SpecialistTypes)iI))
				{
					setSpecialistCount(((SpecialistTypes)iI), getMaxSpecialistCount((SpecialistTypes)iI));
				}
			}
		}
		
		// always work the home plot (center)
		CvPlot* pHomePlot = getCityIndexPlot(CITY_HOME_PLOT);
		if (pHomePlot != NULL)
		{
			setWorkingPlot(CITY_HOME_PLOT, ((getPopulation() > 0) && canWork(pHomePlot)));
		}
		
		// keep removing the worst citizen until we are not over the limit
		while (extraPopulation() < 0)
		{
			if (!AI_removeWorstCitizen())
			{
				FAssert(false);
				break;
			}
		}
		
		// extraSpecialists() is less than extraPopulation()
		FAssertMsg(extraSpecialists() >= 0, "extraSpecialists() is expected to be non-negative (invalid Index)");
		
		// do we have population unassigned
		while (extraPopulation() > 0)
		{
			// (AI_addBestCitizen now handles forced specialist logic)
			if (!AI_addBestCitizen(/*bWorkers*/ true, /*bSpecialists*/ true))
			{
				break;
			}
		}

		// if forcing specialists, assign any other specialists that we must place based on forced specialists
		int iInitialExtraSpecialists = extraSpecialists();
		int iExtraSpecialists = iInitialExtraSpecialists;
		if (bIsSpecialistForced && iExtraSpecialists > 0)
		{
			FAssertMsg(iTotalForcedSpecialists > 0, "zero or negative total forced specialists");
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				if (isSpecialistValid((SpecialistTypes)iI, 1))
				{
					int iForcedSpecialistCount = getForceSpecialistCount((SpecialistTypes)iI);
					if (iForcedSpecialistCount > 0)
					{
						int iSpecialistCount = getSpecialistCount((SpecialistTypes)iI);
						int iMaxSpecialistCount = getMaxSpecialistCount((SpecialistTypes)iI);

						int iSpecialistsToAdd = ((iInitialExtraSpecialists * iForcedSpecialistCount) + (iTotalForcedSpecialists/2)) / iTotalForcedSpecialists;
						if (iExtraSpecialists < iSpecialistsToAdd)
						{
							iSpecialistsToAdd = iExtraSpecialists;
						}
						
						iSpecialistCount += iSpecialistsToAdd;
						iExtraSpecialists -= iSpecialistsToAdd;

						// if we cannot fit that many, then add as many as we can
						if (iSpecialistCount > iMaxSpecialistCount && !GET_PLAYER(getOwnerINLINE()).isSpecialistValid((SpecialistTypes)iI))
						{
							iExtraSpecialists += iSpecialistCount - iMaxSpecialistCount;
							iSpecialistCount = iMaxSpecialistCount;
						}

						setSpecialistCount((SpecialistTypes)iI, iSpecialistCount);
					}
				}
			}
		}
		FAssertMsg(iExtraSpecialists >= 0, "added too many specialists");

		// if we still have population to assign, assign specialists
		while (extraSpecialists() > 0)
		{
			if (!AI_addBestCitizen(/*bWorkers*/ false, /*bSpecialists*/ true))
			{
				break;
			}
		}
		
		// if automated, look for better choices than the current ones
		if (!isHuman() || isCitizensAutomated())
		{
			AI_juggleCitizens();
		}
		
		// at this point, we should not be over the limit
		FAssert((getWorkingPopulation() + getSpecialistPopulation()) <= (totalFreeSpecialists() + getPopulation()));
	}

	AI_setAssignWorkDirty(false);

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	}
}


void CvCityAI::AI_updateAssignWork()
{
	if (AI_isAssignWorkDirty())
	{
		//	Don't mess with plot assignments while the user is in the city screen as
		//	current assignments might be transitory
		if ( !GC.getGameINLINE().isNetworkMultiPlayer() && isHuman() && gDLL->getInterfaceIFace()->isCityScreenUp() )
		{
			//	Also assume when they leave the city screen they will leave a state
			//	they are happy with
			AI_setAssignWorkDirty(false);
			return;
		}

		AI_assignWorkingPlots();
	}
}


bool CvCityAI::AI_avoidGrowth()
{
	PROFILE_FUNC();
	
	if (isFoodProduction())
	{
		return true;
	}

	if ( isHuman() && !isProductionAutomated() )
	{
		return false;
	}

	if (AI_isEmphasizeAvoidGrowth())
	{
		return true;
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/14/10                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
/* original code
	if (!AI_isEmphasizeYield(YIELD_FOOD) && !AI_isEmphasizeGreatPeople())
*/
	// AI should avoid growth when it has angry citizens, even if emphasizing great people
	if( !(isHuman()) || (!AI_isEmphasizeYield(YIELD_FOOD) && !AI_isEmphasizeGreatPeople()) )
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	{
		int iExtra = (isHuman()) ? 0 : 1;
		int iHappinessLevel = happyLevel() - unhappyLevel(iExtra);
		
		// ignore military unhappy, since we assume it will be fixed quickly, grow anyway
		if (getMilitaryHappinessUnits() == 0)
		{
			iHappinessLevel += ((GC.getDefineINT("NO_MILITARY_PERCENT_ANGER") * (getPopulation() + 1)) / GC.getPERCENT_ANGER_DIVISOR());
		}
		
/************************************************************************************************/
/* REVOLUTION_MOD                         09/05/09                                jdog5000      */
/*                                                                                              */
/* Revolution AI                                                                                */
/************************************************************************************************/
		if(!GC.getGameINLINE().isOption(GAMEOPTION_NO_REVOLUTION))
		{
			iHappinessLevel -= std::min(getRevolutionIndex()/600, 2);
		}
		else
		{
			// if we can pop rush, we want to grow one over the cap
			if (GET_PLAYER(getOwnerINLINE()).canPopRush())
			{
				iHappinessLevel++;
			}
		}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
		
		// if we have angry citizens
		if (iHappinessLevel < 0)
		{
			return true;
		}
	}
/************************************************************************************************/
/* Afforess	                  Start		 02/10/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (AI_isEmphasizeAvoidAngryCitizens())
	{
		if (unhappyLevel() > happyLevel())
		{
			return true;
		}

		if (getFoodTurnsLeft() == 1)
		{
			int iHappyFacesLeft = happyLevel() - unhappyLevel(getHappinessTimer() > 0 ? GC.getDefineINT("TEMP_HAPPY") : 0);

			if (iHappyFacesLeft < 1)
			{
				return true;
			}
		}
	}

	if (AI_isEmphasizeAvoidUnhealthyCitizens())
	{
		if (badHealth() > goodHealth())
		{
			return true;
		}

		if (getFoodTurnsLeft() == 1)
		{
			int iHealthyFacesLeft = goodHealth() - badHealth();

			if (iHealthyFacesLeft < 1)
			{
				return true;
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return false;
}


bool CvCityAI::AI_ignoreGrowth()
{
	PROFILE_FUNC();

	if (!AI_isEmphasizeYield(YIELD_FOOD) && !AI_isEmphasizeGreatPeople())
	{
		if (!AI_foodAvailable((isHuman()) ? 0 : 1))
		{
			return true;
		}
	}

	return false;
}


int CvCityAI::AI_specialistValue(SpecialistTypes eSpecialist, bool bAvoidGrowth, bool bRemove)

{
	PROFILE_FUNC();

	short aiYields[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYields[iI] = GET_PLAYER(getOwnerINLINE()).specialistYield(eSpecialist, ((YieldTypes)iI));
	}

	short int aiCommerceYields[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerceYields[iI] = GET_PLAYER(getOwnerINLINE()).specialistCommerce(eSpecialist, ((CommerceTypes)iI));
	}

	int iValue = AI_yieldValue(aiYields, aiCommerceYields, bAvoidGrowth, bRemove);

	int iGreatPeopleRate = GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();
	if (iGreatPeopleRate != 0)
	{
		int iEmphasisCount = 0;
		int iGPPValue = 4;
		if (AI_isEmphasizeGreatPeople())
		{
			iGPPValue = isHuman() ? 30 : 20;
		}
		else
		{
			if (AI_isEmphasizeYield(YIELD_COMMERCE))
			{
				iGPPValue = 2;
				iEmphasisCount++;
			}
			if (AI_isEmphasizeYield(YIELD_PRODUCTION))
			{
				iGPPValue = 1;
				iEmphasisCount++;
			}
			if (AI_isEmphasizeYield(YIELD_FOOD))
			{
				iGPPValue = 1;
				iEmphasisCount++;
			}
		}

		//iGreatPeopleRate = ((iGreatPeopleRate * getTotalGreatPeopleRateModifier()) / 100);
		// UnitTypes iGreatPeopleType = (UnitTypes)GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitClass();

		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                      12/06/09                                jdog5000      */
		/*                                                                                              */
		/* City AI                                                                                      */
		/************************************************************************************************/
		// Scale up value for civs/civics with bonuses
		iGreatPeopleRate *= (100 + GET_PLAYER(getOwnerINLINE()).getGreatPeopleRateModifier());
		iGreatPeopleRate /= 100;
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                       END                                                  */
		/************************************************************************************************/

		int iTempValue = (iGreatPeopleRate * iGPPValue);

		//		if (isHuman() && (getGreatPeopleUnitRate(iGreatPeopleType) == 0)
		//			&& (getForceSpecialistCount(eSpecialist) == 0) && !AI_isEmphasizeGreatPeople())
		//		{
		//			iTempValue -= (iGreatPeopleRate * 4);			
		//		}

		if (!isHuman() || AI_isEmphasizeGreatPeople())
		{
			int iProgress = getGreatPeopleProgress();
			if (iProgress > 0)
			{
				int iThreshold = GET_PLAYER(getOwnerINLINE()).greatPeopleThreshold();
				iTempValue += ((iGreatPeopleRate * (isHuman() ? 1 : 4) * iGPPValue * iProgress * iProgress) / (iThreshold * iThreshold));
			}
		}

		int iCurrentEra = GET_PLAYER(getOwnerINLINE()).getCurrentEra();
		int iTotalEras = GC.getNumEraInfos();

		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                      03/08/10                                jdog5000      */
		/*                                                                                              */
		/* Victory Strategy AI                                                                          */
		/************************************************************************************************/
		if (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
		{
			int iUnitClass = GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitClass();
			//FAssert(iUnitClass != NO_UNITCLASS);
			/************************************************************************************************/
			/* Afforess	                  Start		 01/18/10                                               */
			/************************************************************************************************/
			if (iUnitClass != NO_UNITCLASS)
			{
				/************************************************************************************************/
				/* Afforess	                     END                                                            */
				/************************************************************************************************/

				UnitTypes eGreatPeopleUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iUnitClass);
				if (eGreatPeopleUnit != NO_UNIT)
				{
					CvUnitInfo& kUnitInfo = GC.getUnitInfo(eGreatPeopleUnit);
					if (kUnitInfo.getGreatWorkCulture() > 0)
					{
						iTempValue += kUnitInfo.getGreatWorkCulture() / ((GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3)) ? 200 : 350);
					}
				}
				/************************************************************************************************/
				/* Afforess	                  Start		 01/18/10                                               */
				/************************************************************************************************/
			}
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/
		}
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                       END                                                  */
		/************************************************************************************************/

		if (!isHuman() && (iCurrentEra <= ((iTotalEras * 2) / 3)))
		{
			// try to spawn a prophet for any shrines we have yet to build
			bool bNeedProphet = false;
			int iBestSpreadValue = 0;


			for (int iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
			{
				ReligionTypes eReligion = (ReligionTypes)iJ;

				if (isHolyCity(eReligion) && !hasShrine(eReligion)
					&& ((iCurrentEra < iTotalEras / 2) || GC.getGameINLINE().countReligionLevels(eReligion) >= 10))
				{
					CvCivilizationInfo* pCivilizationInfo = &GC.getCivilizationInfo(getCivilizationType());

					int iUnitClass = GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitClass();
					//FAssert(iUnitClass != NO_UNITCLASS);
					/************************************************************************************************/
					/* Afforess	                  Start		 01/18/10                                               */
					/************************************************************************************************/
					if (iUnitClass != NO_UNITCLASS)
					{
						/************************************************************************************************/
						/* Afforess	                     END                                                            */
						/************************************************************************************************/
						UnitTypes eGreatPeopleUnit = (UnitTypes)pCivilizationInfo->getCivilizationUnits(iUnitClass);
						if (eGreatPeopleUnit != NO_UNIT)
						{
							// note, for normal XML, this count will be one (there is only 1 shrine building for each religion)
							int	shrineBuildingCount = GC.getGameINLINE().getShrineBuildingCount(eReligion);
							for (int iI = 0; iI < shrineBuildingCount; iI++)
							{
								int eBuilding = (int)GC.getGameINLINE().getShrineBuilding(iI, eReligion);

								// if this unit builds or forceBuilds this building
								if (GC.getUnitInfo(eGreatPeopleUnit).getHasBuilding(eBuilding))
								{
									bNeedProphet = true;
									iBestSpreadValue = std::max(iBestSpreadValue, GC.getGameINLINE().countReligionLevels(eReligion));
								}
							}
						}
						/************************************************************************************************/
						/* Afforess	                  Start		 01/18/10                                               */
						/************************************************************************************************/
					}
					/************************************************************************************************/
					/* Afforess	                     END                                                            */
					/************************************************************************************************/
				}
			}

			if (bNeedProphet)
			{
				iTempValue += ((iGreatPeopleRate * iBestSpreadValue));
			}
		}

		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                      12/06/09                                jdog5000      */
		/*                                                                                              */
		/* Bugfix, City AI                                                                              */
		/************************************************************************************************/
		/* original BTS code
				iTempValue *= 100;
		*/
		// Scale up value for civs/civics with bonuses
		iTempValue *= getTotalGreatPeopleRateModifier();
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                       END                                                  */
		/************************************************************************************************/
		iTempValue /= GET_PLAYER(getOwnerINLINE()).AI_averageGreatPeopleMultiplier();

		iTempValue /= (1 + iEmphasisCount);
		iValue += iTempValue;
	}
	else
	{
		SpecialistTypes eGenericCitizen = (SpecialistTypes)GC.getDefineINT("DEFAULT_SPECIALIST");

		// are we the generic specialist?
		if (eSpecialist == eGenericCitizen)
		{
			iValue *= 60;
			iValue /= 100;
		}
	}

	int iExperience = (GC.getSpecialistInfo(eSpecialist).getExperience() * 2);
	for (int iI = 0; iI < GC.getSpecialistInfo(eSpecialist).getNumUnitCombatExperienceTypes(); iI++)
	{
		if (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI, false).eUnitCombat != NO_UNITCOMBAT)
		{
			iExperience += (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI, false).iModifier);
			if (isProductionUnit() && isProductionUnitCombat((int)GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI, false).eUnitCombat))
			{
				iExperience += (2 * (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI, false).iModifier));
			}
		}
	}
	if (0 != iExperience)
	{
		int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);
		int iHasMetCount = GET_TEAM(getTeam()).getHasMetCivCount(true);

		iValue += (iExperience * ((iHasMetCount > 0) ? 4 : 2));
		if (iProductionRank <= GET_PLAYER(getOwnerINLINE()).getNumCities() / 2 + 1)
		{
			iValue += iExperience * 4;
		}
		iValue += ((getMilitaryProductionModifier() * iExperience * 10) / 100);
	}

	/************************************************************************************************/
	/* Specialists Enhancements, by Supercheese 10/12/09                                            */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	int iSpecialistHealth = GC.getSpecialistInfo(eSpecialist).getHealthPercent() / 100;
	int iSpecialistHappiness = GC.getSpecialistInfo(eSpecialist).getHappinessPercent() / 100;
	int iHappinessLevel = happyLevel() - unhappyLevel(1) + getEspionageHappinessCounter();
	int iHealthLevel = goodHealth() - badHealth(/*bNoAngry*/ false, std::max(0, (iHappinessLevel + 1) / 2)) + getEspionageHealthCounter();

	int iPopulationExponent = (getPopulation() - 1); // Each pop past the first increases consumption per population by .1, rounded down.  Each point of population means more actual people the higher the amount goes.
	int iConsumptionPerPopulationBase = iPopulationExponent + (GC.getFOOD_CONSUMPTION_PER_POPULATION() * 10);
	int iConsumptionbyPopulation = (getPopulation() * iConsumptionPerPopulationBase) / 10;

	int iBaseFoodDifference = getYieldRate(YIELD_FOOD) - iConsumptionbyPopulation - std::max(0, -iHealthLevel);

	if (iSpecialistHealth != 0)
	{
		iValue += (healthValue(iSpecialistHealth, iHappinessLevel - (getEspionageHappinessCounter() / 2) + std::max(0, iSpecialistHappiness), iHealthLevel, iBaseFoodDifference) * 2);
	}

	if (iSpecialistHappiness != 0)
	{
		iValue += (happynessValue(iSpecialistHappiness, iHappinessLevel, iHealthLevel) * 2);
	}
	/************************************************************************************************/
	/* Specialists Enhancements                          END                                        */
	/************************************************************************************************/

		//	Koshling - evaluate properties
	int iPropertyValue = 0;

	CvPropertyManipulators* pMani = GC.getSpecialistInfo(eSpecialist).getPropertyManipulators();

	for (int i = 0; i < pMani->getNumSources(); i++)
	{
		CvPropertySource* pSource = pMani->getSource(i);

		//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
		//	to the city for most properties anyway
		if (pSource->getType() == PROPERTYSOURCE_CONSTANT &&
			(pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT))
		{
			PropertyTypes eProperty = pSource->getProperty();
			int iCurrentSourceSize = getTotalBuildingSourcedProperty(eProperty) + getTotalUnitSourcedProperty(eProperty) + getPropertyNonBuildingSource(eProperty);
			int iNewSourceSize = iCurrentSourceSize + static_cast<CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObjectConst());
			int iDecayPercent = getPropertyDecay(eProperty);

			//	Steady state occurs at a level where the decay removes as much per turn as the sources add
			//	Decay can be 0 if the current level is below the threshold at which decay cuts in, so for the
			//	purposes of calculation just treat this as very slow decay
			int	iCurrentSteadyStateLevel = (100 * iCurrentSourceSize) / std::max(1, iDecayPercent);
			int	iNewSteadyStateLevel = (100 * iNewSourceSize) / std::max(1, iDecayPercent);

			iPropertyValue += (getPropertySourceValue(eProperty, iNewSteadyStateLevel - iCurrentSteadyStateLevel) / 100);
		}
	}

	iValue += iPropertyValue;

	//TB Insidiousness/Investigation
	int iNumCriminals = plot()->getNumCriminals();
	int iMultiplier = (iNumCriminals * iNumCriminals);
	int iInvestigate = GC.getSpecialistInfo(eSpecialist).getInvestigation();
	int iInsidious = GC.getSpecialistInfo(eSpecialist).getInsidiousness();
	iValue += iInvestigate + (iMultiplier * iInvestigate);
	iValue -= (iInsidious + (iMultiplier * iInsidious));
	//TB DEBUG test
	//Lets see how city assignments go if we half this value - thinking that it may be comparing to the workable plots
	iValue /= 2;

	return AI_isEmphasizeSpecialist(eSpecialist) ? (iValue * 175) : (iValue * 100);
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/22/09                                jdog5000      */
/*                                                                                              */
/* City AI, War Strategy AI                                                                     */
/************************************************************************************************/
void CvCityAI::AI_chooseProduction()
{
	PROFILE_FUNC();

	m_bRequestedBuilding = false;
	m_bRequestedUnit = false;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	if (isProduction())
	{
		if (getProduction() > 0)
		{
			bool bDanger = AI_isDanger();
			
			if ((getProductionUnitAI() != UNITAI_SETTLE) || !kPlayer.AI_isFinancialTrouble())
			{
				//if we are killing our growth to train this, then finish it.
				if (!bDanger && isFoodProduction())
				{
					if ((area()->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE))
					{
						return;
					}
				}
			}

			// if nearly done, keep building current item
			int iTurns = 5 * std::max(100, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent());
			iTurns /= 100;
			if (getProductionTurnsLeft() <= iTurns)
			{
				return;
			}
			
			// if building a combat unit, and we have no defenders, keep building it
			UnitTypes eProductionUnit = getProductionUnit();
			if (eProductionUnit != NO_UNIT)
			{
				if (plot()->getNumDefenders(getOwnerINLINE()) == 0)
				{
/************************************************************************************************/
/* Afforess	                  Start		 6/11/11                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
					if ((GC.getUnitInfo(eProductionUnit).getCombat() + 
					GET_TEAM(getTeam()).getUnitClassStrengthChange((UnitClassTypes)GC.getUnitInfo(eProductionUnit).getUnitClassType())) > 0)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
					{
						return;
					}
				}
			}
			
			// if we are building a wonder, do not cancel, keep building it (if no danger)
			BuildingTypes eProductionBuilding = getProductionBuilding();
			if (!bDanger && eProductionBuilding != NO_BUILDING && 
				isLimitedWonderClass((BuildingClassTypes) GC.getBuildingInfo(eProductionBuilding).getBuildingClassType()))
			{
				return;
			}
		}

		clearOrderQueue();
}


	if (kPlayer.isAnarchy())
	{
		return;
	}

	bool bDanger = AI_isDanger();
	
	m_iBuildPriority = CITY_BUILD_PRIORITY_CEILING;
	m_iTempBuildPriority = CITY_BUILD_PRIORITY_CEILING;

	// only clear the dirty bit if we actually do a check, multiple items might be queued
	AI_setChooseProductionDirty(false);
/************************************************************************************************/
/* Afforess	                  Start		 04/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if(GC.getUSE_AI_CHOOSE_PRODUCTION_CALLBACK())
	{
		// allow python to handle it
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "AI_chooseProduction", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return;
		}
	}

	bool bInhibitUnits = false;
	if (isHuman() && isProductionAutomated())
	{
		if (!GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_3))
		{
			AI_buildGovernorChooseProduction();
			return;
		}
		else
		{
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				if (AI_isEmphasizeCommerce((CommerceTypes)iI))
				{
					bInhibitUnits = true;
				}
			}
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				if (AI_isEmphasizeYield((YieldTypes)iI))
				{
					bInhibitUnits = true;
				}
			}
		}
	}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	
	CvArea* pArea = area();
	CvArea* pWaterArea = waterArea(true);
	bool bMaybeWaterArea = false;
	bool bWaterDanger = false;
	
	if (pWaterArea != NULL)
	{
		bMaybeWaterArea = true;
		if (!GET_TEAM(getTeam()).AI_isWaterAreaRelevant(pWaterArea))
		{
			pWaterArea = NULL;
		}

		bWaterDanger = kPlayer.AI_getWaterDanger(plot(), 4) > 0;
	}

	bool bWasFoodProduction = isFoodProduction();
	//AIAndy: Not used here at the moment
	//bool bHasMetHuman = GET_TEAM(getTeam()).hasMetHuman();
	bool bMassing = (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT_MASSING);
	bool bLandWar = ((pArea->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) || bMassing);
	bool bDefenseWar = (pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
	bool bAssaultAssist = (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT_ASSIST);
	bool bAssault = bAssaultAssist || (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT) || bMassing;
	bool bPrimaryArea = kPlayer.AI_isPrimaryArea(pArea);
	bool bFinancialTrouble = kPlayer.AI_isFinancialTrouble();
	int iCulturePressure = AI_calculateCulturePressure();
	int iNumCitiesInArea = pArea->getCitiesPerPlayer(getOwnerINLINE());
	bool bImportantCity = false; //be very careful about setting this.
	int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
	int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();

	int iWarSuccessRatio = GET_TEAM(getTeam()).AI_getWarSuccessCapitulationRatio();
	int iEnemyPowerPerc = GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true);
	int iWarTroubleThreshold = 0;

	//	Are our best worker units one-shot usage (mostly)?
	//bool bBestWorkerIsOneShot = false;
	//TB OOS Debug: Undefined variable causing an OOS
	int iUnitValue = 0;
	//UnitTypes eBestWorker = AI_bestUnitAI(UNITAI_WORKER, iUnitValue, false, true);
	//if ( eBestWorker != NO_UNIT )
	//{
	//	int	iNoKillBuildsCount = 0;
	//	int	iKillBuildsCount = 0;

	//	for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
	//	{
	//		if (GC.getUnitInfo(eBestWorker).getBuilds(iI))
	//		{
	//			if ( GC.getBuildInfo((BuildTypes)iI).isKill() )
	//			{
	//				iKillBuildsCount++;
	//			}
	//			else
	//			{
	//				iNoKillBuildsCount++;
	//			}
	//		}
	//	}

	//	//	Empirical ratio needed to distinguish C2C gatherer, but should hopefully
	//	//	be a reasonable heuristic should any similar units be added in future
	//	bBestWorkerIsOneShot = (3*iKillBuildsCount >= iNoKillBuildsCount);
	//}  TB Note: disabling the inhibition for one shot workers to see if we can't get more AI improvement earlier.

	if( bLandWar && iWarSuccessRatio < 30 )
	{
		iWarTroubleThreshold = std::max(3,(-iWarSuccessRatio/8));
	}

	if( !bLandWar && !bAssault && GET_TEAM(getTeam()).isAVassal() )
	{
		bLandWar = GET_TEAM(getTeam()).isMasterPlanningLandWar(area());

		if( !bLandWar )
		{
			bAssault = GET_TEAM(getTeam()).isMasterPlanningSeaWar(area());
		}
	}

	bool bGetBetterUnits = kPlayer.AI_isDoStrategy(AI_STRATEGY_GET_BETTER_UNITS);
	bool bAggressiveAI = GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI);

	int iUnitCostPercentage = (kPlayer.calculateUnitCost() * 100) / std::max(1, kPlayer.calculatePreInflatedCosts());
	int iWaterPercent = AI_calculateWaterWorldPercent();
	
	int iBuildUnitProb = AI_buildUnitProb();
	
	int iExistingWorkers = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_WORKER);
	int iNeededWorkers = kPlayer.AI_neededWorkers(pArea);
	// Sea worker need independent of whether water area is militarily relevant
	int iNeededSeaWorkers = (bMaybeWaterArea) ? AI_neededSeaWorkers() : 0;
	int iExistingSeaWorkers = (waterArea(true) != NULL) ? kPlayer.AI_totalWaterAreaUnitAIs(waterArea(true), UNITAI_WORKER_SEA) : 0;

	int iTargetCulturePerTurn = AI_calculateTargetCulturePerTurn();
	
	int iAreaBestFoundValue;
	int iNumAreaCitySites = kPlayer.AI_getNumAreaCitySites(getArea(), iAreaBestFoundValue);
	
	int iWaterAreaBestFoundValue = 0;
	CvArea* pWaterSettlerArea = pWaterArea;
	if( pWaterSettlerArea == NULL )
	{
		pWaterSettlerArea = GC.getMap().findBiggestArea(true);

		if( pWaterSettlerArea != NULL && GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterSettlerArea, UNITAI_SETTLER_SEA) == 0 )
		{
			pWaterSettlerArea = NULL;
		}
	}
	int iNumWaterAreaCitySites = (pWaterSettlerArea == NULL) ? 0 : kPlayer.AI_getNumAdjacentAreaCitySites(pWaterSettlerArea->getID(), getArea(), iWaterAreaBestFoundValue);
	int iNumSettlers = kPlayer.AI_totalUnitAIs(UNITAI_SETTLE) + GET_PLAYER(getOwnerINLINE()).getContractBroker().numRequestsOutstanding(UNITAI_SETTLE);
	
	bool bIsCapitalArea = false;
	int iNumCapitalAreaCities = 0;
	if (kPlayer.getCapitalCity() != NULL)
	{
		iNumCapitalAreaCities = kPlayer.getCapitalCity()->area()->getCitiesPerPlayer(getOwnerINLINE());
		if (getArea() == kPlayer.getCapitalCity()->getArea())
		{
			bIsCapitalArea = true;
		}
	}
		
	int iMaxSettlers = 0;
	if (!bFinancialTrouble && GET_PLAYER(getOwnerINLINE()).bestBuildableUnitForAIType(NO_DOMAIN, UNITAI_SETTLE) != NO_UNIT)
	{
		iMaxSettlers= std::min((kPlayer.getNumCities() + 1) / 2, iNumAreaCitySites + iNumWaterAreaCitySites);
		if (bLandWar || bAssault)
		{
			iMaxSettlers = (iMaxSettlers + 2) / 3;
		}

		if (kPlayer.getCityLimit() > 0)
		{
			if ( kPlayer.getCityOverLimitUnhappy() > 0 )
			{
				//	Soft limit.  If we already have unhappy cities don't create
				//	settlers that will increase overall unhappiness as they found
				//	new cities
				if ( kPlayer.AI_getOverallHappyness() < 0 )
				{
					iMaxSettlers = std::min(iMaxSettlers, std::max(0,kPlayer.getCityLimit() - kPlayer.getNumCities()));
				}
			}
			else
			{
				//	Hard limit - don't stockpile more than 2 settlers
				iMaxSettlers = std::min(iMaxSettlers,kPlayer.getCityLimit() - kPlayer.getNumCities() + 2);
			}
		}
	}
/************************************************************************************************/
/* Afforess	                  Start		 06/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/	
	// AIAndy: Not used here at the moment
	//bool bDevelopingCity = isDevelopingCity();
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	
	bool bChooseWorker = false;
	
	int iEconomyFlags = 0;
	int iEconomyFlagBits = 0;


	if ( !isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION) )
	{
		iEconomyFlags |= BUILDINGFOCUS_PRODUCTION;
		iEconomyFlagBits++;
	}
	if ( !isHuman() || AI_isEmphasizeYield(YIELD_COMMERCE) )
	{
		iEconomyFlags |= BUILDINGFOCUS_GOLD;
		iEconomyFlagBits++;
	}
	if ( !isHuman() || AI_isEmphasizeCommerce(COMMERCE_RESEARCH) )
	{
		iEconomyFlags |= BUILDINGFOCUS_RESEARCH;
		iEconomyFlagBits++;
	}
	if ( !bInhibitUnits )	//	This is actually a proxy for the human owner having set no explicit preferences
	{
		iEconomyFlags |= BUILDINGFOCUS_MAINTENANCE;
		iEconomyFlags |= BUILDINGFOCUS_HAPPY;
		iEconomyFlags |= BUILDINGFOCUS_HEALTHY;

		iEconomyFlagBits += 3;
	}
	if (AI_isEmphasizeGreatPeople())
	{
		iEconomyFlags |= BUILDINGFOCUS_SPECIALIST;
		iEconomyFlagBits++;
	}
	if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		iEconomyFlags |= BUILDINGFOCUS_ESPIONAGE;
		iEconomyFlagBits++;
	}

	//	Normalize threholds using this against the number of bits we are including
	int iEcononmyFlagsThreasholdWeighting = (100*iEconomyFlagBits)/8;

	if (iNumCitiesInArea > 2)
	{
		if (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
		{
			if (iCultureRateRank <= iCulturalVictoryNumCultureCities + 1)
			{
				bool bBigCultureCity = true;

				// if we do not have enough cities, then the highest culture city will not get special attention
				if (iCultureRateRank > 1 || (kPlayer.getNumCities() > (iCulturalVictoryNumCultureCities + 1)))
				{
					if ((((iNumAreaCitySites + iNumWaterAreaCitySites) > 0) && (kPlayer.getNumCities() < 6)) && (GC.getGameINLINE().getSorenRandNum(2, "AI Less Culture More Expand") == 0))
					{
						bImportantCity = false;
					}
					else
					{
						bImportantCity = true;
					}
				}
			}
		}
	}

	// Free experience for various unit domains
	int iFreeLandExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_LAND);
	// AIAndy: Not used here at the moment
	//int iFreeSeaExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_SEA);
	int iFreeAirExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_AIR);

	clearOrderQueue();

	if (bWasFoodProduction)
	{
		AI_assignWorkingPlots();
	}

	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

	// K-Mod, military exemption for commerce cities and underdeveloped cities
	//bool bUnitExempt = false;
	//if (kPlayer.AI_isDoStrategy(AI_STRATEGY_ECONOMY_FOCUS))
	//{
	//	bUnitExempt = true;
	//}
	//else if (iProductionRank > kPlayer.getNumCities() / 2)
	//{
	//	bool bBelowMedian = true;
	//	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	//	{
	//		// I'd use the total commerce rank, but there currently isn't a cached value of that.
	//		int iRank = findCommerceRateRank((CommerceTypes)iI);
	//		if (iRank < iProductionRank)
	//		{
	//			bUnitExempt = true;
	//			break;
	//		}
	//		if (iRank < kPlayer.getNumCities() / 2)
	//		{
	//			bBelowMedian = false;
	//		}
	//	}

	//	if (bBelowMedian)
	//	{
	//		bUnitExempt = true;
	//	}
	//}
	////Exemptions (Afforess)
	//if (isCapital() || kPlayer.getNumCities() < 3)
	//{
	//	bUnitExempt = false;
	//}
	// K-Mod end

	if( gCityLogLevel >= 3 ) logBBAI("      City %S pop %d considering new production: iProdRank %d, iBuildUnitProb %d", getName().GetCString(), getPopulation(), iProductionRank, iBuildUnitProb);

	// -------------------- BBAI Notes -------------------------
	// Start special circumstances

	// -------------------- BBAI Notes -------------------------
	// Barbarian city build priorities
	if (isHominid())
	{
		if( gCityLogLevel >= 3 )
		{
			logBBAI("      Barb city %S - area workers %d (needed %d), local %d (needed %d)", getName().GetCString(), iExistingWorkers, iNeededWorkers, AI_getWorkersHave(), AI_getWorkersNeeded());
		}
		if (!AI_isDefended(plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_isUnitAIType, UNITAI_ATTACK, -1, getOwnerINLINE()))) // XXX check for other team's units?
		{
			if (AI_chooseDefender("barbarian defenders"))
			{
				return;
			}

			if (AI_chooseUnit("barbarian lack of defense", UNITAI_ATTACK))
			{
				return;
			}
		}
		
		if (!bDanger && (2*iExistingWorkers < iNeededWorkers) && (AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
		{
			if( getPopulation() > 1 || (GC.getGameINLINE().getGameTurn() - getGameTurnAcquired() > (15 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent())/100) )
			{
				if (AI_chooseUnit("barbarian worker for established city", UNITAI_WORKER))
				{
					return;
				}
			}			
		}

		if (!bDanger && !bWaterDanger && (iNeededSeaWorkers > 0))
		{
			if (AI_chooseUnit("barbarian navy", UNITAI_WORKER_SEA))
			{
				return;
			}
		}
		
		iBuildUnitProb += (3 * iFreeLandExperience);
		
		bool bRepelColonists = false;
		if( area()->getNumCities() > area()->getCitiesPerPlayer(BARBARIAN_PLAYER) + 2 )
		{
			if( area()->getCitiesPerPlayer(BARBARIAN_PLAYER) > area()->getNumCities()/3 )
			{
				// New world scenario with invading colonists ... fight back!
				bRepelColonists = true;
				iBuildUnitProb += 8*(area()->getNumCities() - area()->getCitiesPerPlayer(BARBARIAN_PLAYER));
			}
		}

		if (!bDanger && GC.getGameINLINE().getSorenRandNum(100, "AI Build Unit Production") > iBuildUnitProb)
		{
			
			int iBarbarianFlags = 0;
			if( getPopulation() < 4 ) iBarbarianFlags |= BUILDINGFOCUS_FOOD;
			iBarbarianFlags |= BUILDINGFOCUS_PRODUCTION;
			iBarbarianFlags |= BUILDINGFOCUS_EXPERIENCE;
			if( getPopulation() > 3 ) iBarbarianFlags |= BUILDINGFOCUS_DEFENSE;
			
			if (AI_chooseBuilding(iBarbarianFlags, 15))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb AI_chooseBuilding with flags and iBuildUnitProb = %d", getName().GetCString(), iBuildUnitProb);
				return;
			}

			if( GC.getGameINLINE().getSorenRandNum(100, "AI Build Unit Production") > iBuildUnitProb)
			{
				if (AI_chooseBuilding())
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb AI_chooseBuilding without flags and iBuildUnitProb = %d", getName().GetCString(), iBuildUnitProb);
					return;
				}
			}
		}
		
		if (plot()->plotCount(PUF_isUnitAIType, UNITAI_ASSAULT_SEA, -1, NULL, getOwnerINLINE()) > 0)
		{
			if (AI_chooseUnit("barbarian choose attack city for transports", UNITAI_ATTACK_CITY))
			{
				return;
			}
		}
		
		if (!bDanger && (pWaterArea != NULL) && (iWaterPercent > 30))
		{
			if (GC.getGameINLINE().getSorenRandNum(3, "AI Coast Raiders!") == 0)
			{
				if (kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) <= (1 + kPlayer.getNumCities() / 2))
				{
					if (AI_chooseUnit("barbarian transports", UNITAI_ASSAULT_SEA))
					{
						return;
					}
				}
			}
			if (GC.getGameINLINE().getSorenRandNum(110, "AI arrrr!") < (iWaterPercent + 10))
			{
				if (kPlayer.AI_totalUnitAIs(UNITAI_PIRATE_SEA) <= kPlayer.getNumCities())
				{
					if (AI_chooseUnit("barbarian pirates", UNITAI_PIRATE_SEA))
					{
						return;
					}
				}
				
				if (kPlayer.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ATTACK_SEA) < iNumCitiesInArea)
				{
					if (AI_chooseUnit("barbarian sea attack", UNITAI_ATTACK_SEA))
					{
						return;
					}
				}
			}
		}

		if (GC.getGameINLINE().getSorenRandNum(2, "Barb worker") == 0)
		{
			if (!bDanger && (iExistingWorkers < iNeededWorkers) && (AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
			{
				if( getPopulation() > 1 )
				{
					if (AI_chooseUnit("barbarian worker", UNITAI_WORKER))
					{
						return;
					}
				}			
			}
		}

		UnitTypeWeightArray barbarianTypes;
		barbarianTypes.push_back(std::make_pair(UNITAI_ATTACK, 125));
		barbarianTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, (bRepelColonists ? 200 : 100)));
		barbarianTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		barbarianTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 50));

		if (AI_chooseLeastRepresentedUnit("barbarian units", barbarianTypes))
		{
			return;
		}
		
		if (AI_chooseUnit("barbarian last resort"))
		{
			return;
		}
		
		return;
	}
	
//TB Build Mod (is considered a poor strategy now)	// if we need to pop borders, then do that immediately if we have drama and can do it
#ifndef C2C_BUILD
	// if we need to pop borders, then do that immediately if we have drama and can do it
	// cppcheck-suppress knownConditionTrueFalse
	if ((iTargetCulturePerTurn > 0) && (getCultureLevel() <= (CultureLevelTypes) 1))
	{
		if (AI_chooseProcess(COMMERCE_CULTURE))
		{
			return;
		}
	}
#endif

/************************************************************************************************/
/* REVOLUTION_MOD                         06/11/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if( kPlayer.isRebel() )
	{		
		UnitTypeWeightArray rebelDefenseTypes;
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 100));
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));

		UnitTypeWeightArray rebelOffenseTypes;
		rebelOffenseTypes.push_back(std::make_pair(UNITAI_ATTACK, 125));
		rebelOffenseTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		rebelOffenseTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 75));

		if( isOccupation() )
		{
			// Probably just captured ... may be getting defensive units, 
			// but need to build offense to keep rolling
			if( bLandWar || bDanger )
			{
				if (AI_chooseLeastRepresentedUnit("rebel offense", rebelOffenseTypes))
				{
					return;
				}
			}
		}
		else
		{
			// City Defense
			if (plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, NULL, getOwnerINLINE()) < (AI_minDefenders()))
			{
				if (AI_chooseUnit("rebel city defense", UNITAI_CITY_DEFENSE))
				{
					return;
				}

				if (AI_chooseDefender("rebel city defense (non-specific)"))
				{
					return;
				}
			}

			// Area defense
			int iNeededFloatingDefenders = kPlayer.AI_getTotalFloatingDefendersNeeded(pArea);
			int iTotalFloatingDefenders = kPlayer.AI_getTotalFloatingDefenders(pArea);
			
			if (iTotalFloatingDefenders < ((iNeededFloatingDefenders + 1) / (2)))
			{
				if (AI_chooseLeastRepresentedUnit("rebel defense", rebelDefenseTypes))
				{
					return;
				}
			}

			// Offensive rebel units
			if( bDanger || (GC.getGameINLINE().getSorenRandNum(100, "AI Build Unit Production") < AI_buildUnitProb()) )
			{
				if( (getYieldRate(YIELD_PRODUCTION) > 5) )
				{
					// Air units
					int iBestDefenseValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_DEFENSE_AIR, this);

					if( iBestDefenseValue > 0 )
					{
						UnitTypes eBestAttackAircraft = NO_UNIT;
						int iBestAirValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_ATTACK_AIR, this, &eBestAttackAircraft);

						int iAircraftHave = kPlayer.AI_getNumAIUnits(UNITAI_ATTACK_AIR) + kPlayer.AI_getNumAIUnits(UNITAI_DEFENSE_AIR) + kPlayer.AI_getNumAIUnits(UNITAI_MISSILE_AIR);
						int iAircraftNeed = (2 + kPlayer.getNumCities() * (3 * GC.getUnitInfo(eBestAttackAircraft).getAirCombat())) / (2 * std::max(1, GC.getGame().getBestLandUnitCombat()));

						UnitTypeWeightArray airUnitTypes;
						airUnitTypes.push_back(std::make_pair(UNITAI_ATTACK_AIR, 60));
						airUnitTypes.push_back(std::make_pair(UNITAI_DEFENSE_AIR, 100));
						
						if ((iAircraftHave * 2 < iAircraftNeed) && (GC.getGameINLINE().getSorenRandNum(2, "AI train escort sea") == 0))
						{
							if (AI_chooseLeastRepresentedUnit("rebel air units", airUnitTypes))
							{
								return;
							}
						}
					}
				}

				if( bLandWar || bDanger )
				{
					if (AI_chooseLeastRepresentedUnit("rebel offense units", rebelOffenseTypes))
					{
						return;
					}
				}

				if( bAssault )
				{
					if( pWaterArea != NULL )
					{
						UnitTypes eBestAssaultUnit = NO_UNIT;  
						kPlayer.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, this, &eBestAssaultUnit);
						int iBestSeaAssaultCapacity = 0;
						if (eBestAssaultUnit != NO_UNIT)
						{
							iBestSeaAssaultCapacity = GC.getUnitInfo(eBestAssaultUnit).getCargoSpace();
						}

						if( iBestSeaAssaultCapacity > 0 )
						{
							int iUnitsToTransport = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
							iUnitsToTransport += kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
							iUnitsToTransport += kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_COUNTER);
							
							int iLocalTransports = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ASSAULT_SEA);
							int iTransportsAtSea = kPlayer.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ASSAULT_SEA);
							int iTransports = iLocalTransports + iTransportsAtSea;

							int iEscorts = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ESCORT_SEA);
							iEscorts += kPlayer.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ESCORT_SEA);

							// Escorts
							if ((iEscorts < ((1 + 2 * iTransports) / 3)) && (GC.getGameINLINE().getSorenRandNum(2, "AI train escort sea") == 0))
							{
								if ( AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA) )
								{
									return;
								}

								if (AI_chooseUnit("rebel sea escorts", UNITAI_ESCORT_SEA))
								{
									return;
								}
							}

							// Transports
							if (iUnitsToTransport > (iTransports * iBestSeaAssaultCapacity))
							{
								if ( AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA) )
								{
									return;
								}

								if (AI_chooseUnit("rebel sea assault", UNITAI_ASSAULT_SEA))
								{
									return;
								}
							}

							// Attack troops
							if( iUnitsToTransport < ((iLocalTransports + (bPrimaryArea ? iTransportsAtSea/2 : 0))*iBestSeaAssaultCapacity) )
							{
								if (AI_chooseLeastRepresentedUnit("rebel offensive units", rebelOffenseTypes))
								{
									return;
								}
							}
						}
					}
				}

				if (AI_chooseUnit("rebel last resort"))
				{
					return;
				}
			}			
		}

		// Buildings important for rebels
		if ((getPopulation() > 3) && (getCommerceRate(COMMERCE_CULTURE) == 0))
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
			{
				return;
			}
		}

		if( getPopulation() < 8 )
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_FOOD))
			{
				return;
			}
		}

		// Happiness?  Health?  

		int iRebelFlags = 0;
		iRebelFlags |= BUILDINGFOCUS_CULTURE;
		iRebelFlags |= BUILDINGFOCUS_PRODUCTION;
		iRebelFlags |= BUILDINGFOCUS_EXPERIENCE;
		iRebelFlags |= BUILDINGFOCUS_DEFENSE;

		if (AI_chooseBuilding(iRebelFlags))
		{
			return;
		}

		//essential economic builds
		if (AI_chooseBuilding(iEconomyFlags, 10, (25*iEcononmyFlagsThreasholdWeighting)/100))
		{
			return;
		}

		// If nothing special to build, continue to regular logic
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	if (isOccupation())
	{
		// pick granary or lighthouse, any duration
		if (AI_chooseBuilding(BUILDINGFOCUS_FOOD))
		{
			return;
		}

		// try picking forge, etc, any duration
		if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION))
		{
			return;
		}
		
		// just pick any building, any duration
		if (AI_chooseBuilding())
		{
			return;
		}
	}

	if (!bInhibitUnits && plot()->getNumDefenders(getOwnerINLINE()) == 0) // XXX check for other team's units?
	{
		static CvUnitSelectionCriteria DefaultCriteria;
		if (AI_chooseUnit("defenseless city", UNITAI_CITY_DEFENSE, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}
/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bIgnoreNotUnitAIs
		if (kPlayer.getNumCities() <= 3)
		{
			CvUnitSelectionCriteria criteria;

			criteria.m_bIgnoreNotUnitAIs = true;
			if (AI_chooseUnit("defenseless city any unit", UNITAI_CITY_DEFENSE, -1, -1, CITY_BUILD_PRIORITY_CEILING, &criteria))
			{
				return;
			}
		}
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/

		if (AI_chooseUnit("defenseless city counter units", UNITAI_CITY_COUNTER, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (AI_chooseUnit("defenseless city special units", UNITAI_CITY_SPECIAL, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
		if (AI_chooseUnit("defenseless city reserve units", UNITAI_RESERVE, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

//Fuyu bIgnoreNotUnitAIs
		if (kPlayer.getNumCities() > 3 && iNumCitiesInArea <= 3)
		{
			CvUnitSelectionCriteria criteria;

			criteria.m_bIgnoreNotUnitAIs = true;
			if (AI_chooseUnit("defenseless city any unit (low city count area)", UNITAI_CITY_DEFENSE, -1, -1, HIGHEST_PRIORITY_ESCORT_PRIORITY, &criteria))
			{
				return;
			}
		}
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/

		if (AI_chooseUnit("defenseless city (accept attack units)", UNITAI_ATTACK, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}
	}

	if( kPlayer.isStrike() )
	{
		// pick granary or lighthouse, any duration
		int iStrikeFlags = 0;
		iStrikeFlags |= BUILDINGFOCUS_GOLD;
		iStrikeFlags |= BUILDINGFOCUS_MAINTENANCE;

		if(AI_chooseBuilding(iStrikeFlags))
		{
			return;
		}

		// try picking forge, etc, any duration
		if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION))
		{
			return;
		}
		
		// just pick any building, any duration
		if (AI_chooseBuilding())
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	// So what's the right detection of defense which works in early game too?
	// Fuyu: The right way is to 1) queue the warriors with UNITAI_CITY_DEFENCE or UNITAI_RESERVE,
	// then 2) to detect defenders with plot()->plotCount(PUF_canDefendGroupHead, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType) - compare AI_isDefended()
	int iPlotSettlerCount = (iNumSettlers == 0) ? 0 : plot()->plotCount(PUF_isUnitAIType, UNITAI_SETTLE, -1, NULL, getOwnerINLINE());
	int iPlotCityDefenderCount = plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, NULL, getOwnerINLINE(), NO_TEAM, NULL, -1, -1, 2);
	int iPlotOtherCityAICount = plot()->plotCount(PUF_canDefend, -1, -1, NULL, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType, -1, -1, 2);
/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
	int iPlotCityDefenderStrength = getGarrisonStrength();
	int iPlotOtherCityAIStrength = plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType, -1, -1, 2) - iPlotCityDefenderStrength;
	if( kPlayer.getCurrentEra() == 0 )
	{
		// Warriors are blocked from UNITAI_CITY_DEFENSE, in early game this confuses AI city building
		if( kPlayer.AI_totalUnitAIs(UNITAI_CITY_DEFENSE) <= kPlayer.getNumCities() + iNumSettlers )
		{
			if( kPlayer.AI_bestCityUnitAIValue(UNITAI_CITY_DEFENSE, this) == 0 )
			{
				iPlotCityDefenderStrength += iPlotOtherCityAIStrength;
				iPlotOtherCityAIStrength = 0;
				if (iPlotCityDefenderStrength == 0)
				{
					iPlotCityDefenderStrength = plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isDomainType, DOMAIN_LAND, -1, 2);
				}
			}
		}
	}
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/

	m_iTempBuildPriority--;

	//	Emergency happyness
	int iHappyness = happyLevel() - unhappyLevel(0);

	if ( iHappyness < -1 || iHappyness <= -getPopulation()/2 )
	{
		//	More than 2 unhappy citizens (or more than half the population)
		if (AI_chooseBuilding(BUILDINGFOCUS_HAPPY, 30, 0, -1, true))
		{
			return;
		}

		//	Can we build military happyness units?  Only do that up to 3 over what we might
		//	normally consider
		if ( GET_PLAYER(getOwner()).getHappyPerMilitaryUnit() > 0 &&
			 plot()->plotCount(PUF_canDefend) < (AI_minDefenders() + iPlotSettlerCount + 3) )
		{
			if (AI_chooseUnit("emergency military happyness", UNITAI_CITY_DEFENSE))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

#if 0
	//minimal defense.
	if (!bInhibitUnits && (iPlotCityDefenderCount <= iPlotSettlerCount || m_requestedEscorts > 0))
	{
		UnitAITypes bestAIType = (m_requestedEscorts > 0 ? UNITAI_CITY_COUNTER : UNITAI_CITY_DEFENSE);

		if( gCityLogLevel >= 2 ) logBBAI("      City %S needs escort for existing settler", getName().GetCString());
		if (AI_chooseUnit("settler escort", bestAIType))
		{
			// BBAI TODO: Does this work right after settler is built???
			return;
		}

/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bIgnoreNotUnitAIs
		if (iNumCitiesInArea <= 3)
		{
			if (AI_chooseUnit("settler escort low area count (accept any)", bestAIType, -1, true))
			{
				// Yes it probably works but it should never happen in the first place
				return;
			}
		}

		if (iPlotCityDefenderCount + iPlotOtherCityAICount <= iPlotSettlerCount)
		{
			if (AI_chooseUnit("settler escort (city defense)", UNITAI_CITY_DEFENSE))
			{
				return;
			}

			if (AI_chooseUnit("settler escort (attack)", UNITAI_ATTACK))
			{
				return;
			}

			if (AI_chooseUnit("settler escort (city counter)", UNITAI_CITY_COUNTER))
			{
				return;
			}

			if (AI_chooseUnit("settler escort (special)", UNITAI_CITY_SPECIAL))
			{
				return;
			}

			if (AI_chooseUnit("settler escort (reserve)", UNITAI_RESERVE))
			{
				return;
			}

		}
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/
	}
#endif

	//	emergency property control (bad property > 20% through op range - choose unit to deal if possible)
	if ( AI_choosePropertyControlUnit( 20 ) )
	{
		return;
	}

	m_iTempBuildPriority--;

	//	Non-emergency, but still urgent happyness
	if ( iHappyness < 0 )
	{
		//	More than 2 unhappy citizens (or more than half the population)
		if (AI_chooseBuilding(BUILDINGFOCUS_HAPPY, 15, 0, -1, true))
		{
			return;
		}

		//	Can we build military happyness units?  Only do that up to 1 over what we might
		//	normally consider
		if ( GET_PLAYER(getOwner()).getHappyPerMilitaryUnit() > 0 &&
			 plot()->plotCount(PUF_canDefend) < (AI_minDefenders() + iPlotSettlerCount + 1) )
		{
			if (AI_chooseUnit("military happyness", UNITAI_CITY_DEFENSE))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//	Really easy production trumps everything
	if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, 1))
	{
		return;
	}

	//TB Note: Moved escorts and settlers to almost the top priority IF it's possible and needed.  This should solve the complaints about settlers not being trained fast enough.
	m_iTempBuildPriority--;

	//minimal defense.
/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
	int iPlotSettlerEscortCityDefenseCount =  plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1,NULL, getOwnerINLINE()) - (AI_minDefenders()+1);
	int iPlotSettlerEscortCounterCount = plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, getOwnerINLINE());
	int iPlotSettlerEscortCount = iPlotSettlerEscortCityDefenseCount + iPlotSettlerEscortCounterCount;
	if (iMaxSettlers > 0 && !bInhibitUnits && iPlotSettlerCount > 0 && iPlotSettlerEscortCount < (iPlotSettlerCount * 4))
		//if (!bInhibitUnits && iPlotCityDefenderCount + iPlotOtherCityAICount < (AI_minDefenders() + iPlotSettlerCount))
		//if (!bUnitExempt && !bInhibitUnits && iPlotCityDefenderStrength + iPlotOtherCityAIStrength < (AI_minDefenseStrength() + iPlotSettlerCount*GET_PLAYER(getOwnerINLINE()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE))) //k-mod
	{
		if (AI_chooseUnit("min defender", UNITAI_CITY_DEFENSE))
		{
			return;
		}
	
		if (AI_chooseUnit("min defender", UNITAI_CITY_COUNTER))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_CITY_DEFENSE))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_CITY_COUNTER))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_CITY_SPECIAL))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_RESERVE))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_ATTACK))
		{
			return;
		}

//Fuyu bIgnoreNotUnitAIs
		if (iNumCitiesInArea <= 3)
		{
			if (AI_chooseUnit("min defender (any unit)", UNITAI_CITY_DEFENSE, -1, 1))
			{
				return;
			}
		}
	}
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/

	m_iTempBuildPriority--;

	// Afforess - don't wait until we are in trouble, preventative medicine is best
	int iFundedPercent = kPlayer.AI_costAsPercentIncome();
	int iSafePercent = kPlayer.AI_safeCostAsPercentIncome();
	// if ( bFinancialTrouble )
	if (iFundedPercent < iSafePercent - 10)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_GOLD, 10))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses financial difficulty resolution", getName().GetCString());
			return;
		}
	}

	m_iTempBuildPriority--;

	int iMinFoundValue = kPlayer.AI_getMinFoundValue();
	if (bDanger)
	{
		iMinFoundValue *= 3;
		iMinFoundValue /= 2;
	}

	if(iMaxSettlers > 0 && !bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -50) )
	{
		if ((iAreaBestFoundValue > iMinFoundValue) || (iWaterAreaBestFoundValue > iMinFoundValue))
		{
			// BBAI TODO: Needs logic to check for early settler builds, settler builds in small cities, whether settler sea exists for water area sites?
			if (pWaterArea != NULL)
			{
				int iTotalCities = kPlayer.getNumCities();
				int iSettlerSeaNeeded = std::min(iNumWaterAreaCitySites, ((iTotalCities + 4) / 8) + 1);
				if (kPlayer.getCapitalCity() != NULL)
				{
					int iOverSeasColonies = iTotalCities - kPlayer.getCapitalCity()->area()->getCitiesPerPlayer(getOwnerINLINE());
					int iLoop = 2;
					int iExtras = 0;
					while (iOverSeasColonies >= iLoop)
					{
						iExtras++;
						iLoop += iLoop + 2;
					}
					iSettlerSeaNeeded += std::min(kPlayer.AI_totalUnitAIs(UNITAI_WORKER) / 4, iExtras);
				}
				if (bAssault)
				{
					iSettlerSeaNeeded = std::min(1, iSettlerSeaNeeded);
				}
				
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) < iSettlerSeaNeeded)
				{
/********************************************************************************/
/* 	Build more workers #1.1										Fuyu		    */
/********************************************************************************/
					/* financial trouble: 2/3; */
					if (!bDanger && bFinancialTrouble && (iExistingWorkers < ((2*iNeededWorkers) + 2)/3))
					{
						if ((AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
						{
							if( getPopulation() > 1 || (GC.getGameINLINE().getGameTurn() - getGameTurnAcquired() > (15 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent())/100) )
							{
								if (!bChooseWorker && AI_chooseUnit("worker needed", UNITAI_WORKER))
								{
									return;
								}
								bChooseWorker = true;
							}
						}
					}
/********************************************************************************/
/* 	Build more workers #1.1										END			    */
/********************************************************************************/
					if (AI_chooseUnit("sea settler needed", UNITAI_SETTLER_SEA))
					{
						return;
					}
				}
			}
			
			if (iPlotSettlerCount == 0)
			{
				if ((iNumSettlers < iMaxSettlers))
				{
					if (GC.getGameINLINE().getSorenRandNum(2, "settler training decision") < (bLandWar ? 1 : 2))
					{
/********************************************************************************/
/* 	Build more workers #1.2										Fuyu		    */
/********************************************************************************/
						/* financial trouble: 2/3; */
						if (!bDanger && bFinancialTrouble && (iExistingWorkers < ((2*iNeededWorkers) + 2)/3))
						{
							if ((AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
							{
								if( getPopulation() > 1 || (GC.getGameINLINE().getGameTurn() - getGameTurnAcquired() > (15 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent())/100) )
								{
									if (!bChooseWorker && AI_chooseUnit("worker needed 2", UNITAI_WORKER))
									{
										return;
									}
									bChooseWorker = true;
								}
							}
						}
/********************************************************************************/
/* 	Build more workers #1.2										END			    */
/********************************************************************************/
/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
						if ((kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() + iNumSettlers + 1))
							//Fuyu: in the beginning, don't count on other cities to build the escorts
							|| (kPlayer.getNumCities() <= 7 && iNumCitiesInArea <= 3 && (plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType) <= 1)))
						{
							if (AI_chooseUnit("extra quick defender", UNITAI_CITY_DEFENSE))
							{
								return;
							}
//Fuyu bIgnoreNotUnitAIs
							if (iNumCitiesInArea <= 3)
							{
								CvUnitSelectionCriteria criteria;

								criteria.m_bIgnoreNotUnitAIs = true;
								if (AI_chooseUnit("extra quick defender (any unit)", UNITAI_CITY_DEFENSE, -1, -1, -1, &criteria))
								{
									return;
								}
							}

							if (AI_chooseUnit("extra quick defender", UNITAI_CITY_COUNTER))
							{
								return;
							}
							if (AI_chooseUnit("extra quick defender", UNITAI_CITY_SPECIAL))
							{
								return;
							}

							if (AI_chooseUnit("extra quick defender", UNITAI_RESERVE))
							{
								return;
							}
						}
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/
						if (AI_chooseUnit("settler needed", UNITAI_SETTLE) )
						{
							return;
						}
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	if (AI_establishInvestigatorCoverage())
	{
		return;
	}

	m_iTempBuildPriority--;

	if (!m_bRequestedUnit)
	{
		if (!bInhibitUnits)
		{
			if (AI_establishSeeInvisibleCoverage())
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//TB Note: min 1 hunter goes under the priority level of settling initiation because this is exploitable with ambushers (or just plain bad luck for the hunters which is not unlikely).  Destroy all hunters and you cripple growth.
	//Koshling - made having at least 1 hunter a much higher priority
	int iNeededExplorers = kPlayer.AI_neededExplorers(pArea);
	int iNeededHunters = kPlayer.AI_neededHunters(pArea);
	int iExplorerDeficitPercent = (iNeededExplorers == 0) ? 0 : ((iNeededExplorers - kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_EXPLORE))*100)/iNeededExplorers;
	int iHunterDeficitPercent = (iNeededHunters == 0) ? 0 : ((iNeededHunters - kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_HUNTER))*100)/iNeededHunters;

	if ( !bInhibitUnits && iNeededHunters > 0 && iHunterDeficitPercent == 100 )
	{
		if ( isCapital() )
		{
			UnitTypes eBestUnit = AI_bestUnitAI(UNITAI_HUNTER, iUnitValue);
			if ( eBestUnit != NO_UNIT )
			{
				int iCost = (GC.getUnitInfo(eBestUnit).getProductionCost() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent()) / 100;
				int perTurnLossesCost = (iCost*area()->getRecentCombatDeathRate(getOwnerINLINE(), UNITAI_HUNTER))/100;

				if( gCityLogLevel >= 2 )
				{
					logBBAI("      City %S evaluating recent hunter deaths - %d per/turn cost vs %d production",
							getName().GetCString(),
							perTurnLossesCost,
							GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION));
				}
				if ( perTurnLossesCost > GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION)/4 )
				{
					if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 30))
					{
						return;
					}
				}
			}
		}

		if (AI_chooseUnit("no hunters at all", UNITAI_HUNTER))
		{
			return;
		}
	}

/********************************************************************************/
/*	RevDCM uncommented Better BUG AI changes	28.10.2010				Fuyu	*/
/********************************************************************************/
	if( !bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -50) && !bDanger )
	{
		if ((iExistingWorkers == 0))
		{
			int iLandBonuses = AI_countNumImprovableBonuses(true, kPlayer.getCurrentResearch());
			if ((iLandBonuses > 1) || (getPopulation() > 3 && iNeededWorkers > 0))
			{
				if (!bChooseWorker && AI_chooseUnit("no workers", UNITAI_WORKER, -1, -1, CITY_NO_WORKERS_WORKER_PRIORITY))
				{
					return;
				}
				bChooseWorker = true;
			}

			if (!bWaterDanger && (iNeededSeaWorkers > iExistingSeaWorkers) && (getPopulation() < 3))
			{
				if (AI_chooseUnit("no sea workers", UNITAI_WORKER_SEA, -1, -1, CITY_NO_WORKERS_WORKER_PRIORITY))
				{
					return;
				}
			}

			if (iLandBonuses >= 1  && getPopulation() > 1)
			{
				if (!bChooseWorker && AI_chooseUnit("secondary worker", UNITAI_WORKER))
				{
					return;
				}
				bChooseWorker = true;
			}
		}
	}
/********************************************************************************/
/*	RevDCM uncommented Better BUG AI changes	28.10.2010				END		*/
/********************************************************************************/
   int iHealth = goodHealth() - badHealth(true, 0);
   int iFoodDiffBase = foodDifference(false, false, true);

   //	If there is negative health address it if we can do so quickly (relative to the amount of ill health)
   //	Koshling - don't waste time with this if we have a food excess that is being trimmed by less than 20%
   //	by the ill health (rounded - so at less than 5 net food we always consider this)
   if ( iHealth < 0 && -iHealth > iFoodDiffBase/5 )
   {
		//	Koshling - reduce the time we're prepared to spend on this by the current food excess (if any)
		if (AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, std::max(1,-iHealth - std::max(0,iFoodDiffBase)), 0, -1, true))
		{
			return;
		}
   }
	
	m_iTempBuildPriority--;

	//Koshling - increase priority of hunetrs up to half what we would ideally want
	if ( iNeededHunters > 0 && iHunterDeficitPercent > 50 )
	{
		if ( isCapital() )
		{
			UnitTypes eBestUnit = AI_bestUnitAI(UNITAI_HUNTER, iUnitValue);
			if ( eBestUnit != NO_UNIT )
			{
				int iCost = (GC.getUnitInfo(eBestUnit).getProductionCost() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent()) / 100;
				int perTurnLossesCost = (iCost*area()->getRecentCombatDeathRate(getOwnerINLINE(), UNITAI_HUNTER))/100;

				if( gCityLogLevel >= 2 )
				{
					logBBAI("      City %S evaluating recent hunter deaths - %d per/turn cost vs %d production",
							getName().GetCString(),
							perTurnLossesCost,
							GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION));
				}
				if ( perTurnLossesCost > GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION)/4 )
				{
					if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 30))
					{
						return;
					}
				}
			}
		}

		if (AI_chooseUnit("less than half the  required hunters", UNITAI_HUNTER))
		{
			return;
		}
	}
	
	m_iTempBuildPriority--;

#ifdef C2C_BUILD
//TB Build Priority Mod Begin    
	{
		PROFILE("AI_chooseProduction.TB_Mod");

		int iPriorityCheckFlags = 0;

		if (!isHuman())
		{
			iPriorityCheckFlags |= BUILDINGFOCUS_RESEARCH;
		}

		if (!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			iPriorityCheckFlags |= BUILDINGFOCUS_PRODUCTION;
		}

		if (!isHuman() || AI_isEmphasizeYield(YIELD_FOOD))
		{
			iPriorityCheckFlags |= BUILDINGFOCUS_FOOD;
		}

		if ( iPriorityCheckFlags != 0 )
		{
			if (AI_chooseBuilding(iPriorityCheckFlags, 5))
			{
				return;
			}
		}
	}
//TB Build Priority Mod End
#endif
	
	m_iTempBuildPriority--;
	// cppcheck-suppress knownConditionTrueFalse
	if (((iTargetCulturePerTurn > 0) || (getPopulation() > 5)) && (getCommerceRate(COMMERCE_CULTURE) == 0))
	{
		if( !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
		{
			if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	if ( m_iTempBuildPriority >= HIGH_PRIORITY_ESCORT_PRIORITY )
	{
		m_iTempBuildPriority = HIGH_PRIORITY_ESCORT_PRIORITY - 1;
	}

	// Early game worker logic
	if( !bInhibitUnits && isCapital() && (GC.getGame().getElapsedGameTurns() < ((30 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent()) / 100)))
	{
		if( !bDanger && !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
		{	
			if (!bWaterDanger && (getPopulation() <= 2) && (iNeededSeaWorkers > 0))
			{
				if (iExistingSeaWorkers == 0)
				{
					// Build workboat first since it doesn't stop growth
					if (AI_chooseUnit("capital with no sea workers", UNITAI_WORKER_SEA))
					{
						return;
					}
				}
			}

			if( iExistingWorkers == 0 && AI_totalBestBuildValue(area()) > 0 /*Fuyu: anything bigger than 0 is ok*/ )
			{
				if (!bChooseWorker && AI_chooseUnit("capital with no workers", UNITAI_WORKER))
				{
					return;
				}
				bChooseWorker = true;
			}
/********************************************************************************/
/* 	Build more early sea workers								Fuyu		    */
/********************************************************************************/
			else
			{
				if (!bWaterDanger && (getPopulation() <= 4) && (iNeededSeaWorkers > 0) && (happyLevel() - unhappyLevel(1)) > 0)
				{
					if (iExistingSeaWorkers == 0)
					{
						if (AI_chooseUnit("capital worker", UNITAI_WORKER_SEA))
						{
							return;
						}
					}
				}
			}
/********************************************************************************/
/* 	Build more early sea workers								END			    */
/********************************************************************************/
		}
	}

	m_iTempBuildPriority--;

	if( !bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -50) && !bDanger )
	{
		if ((iExistingWorkers == 0))
		{
			int iLandBonuses = AI_countNumImprovableBonuses(true, kPlayer.getCurrentResearch());
			if ((iLandBonuses > 1) || (getPopulation() > 3 && iNeededWorkers > 0))
			{
				if (!bChooseWorker && AI_chooseUnit("non capital primary worker", UNITAI_WORKER))
				{
					return;
				}
				bChooseWorker = true;
			}

			if (!bWaterDanger && (iNeededSeaWorkers > iExistingSeaWorkers) && (getPopulation() < 3))
			{
				if (AI_chooseUnit("secondary sea worker", UNITAI_WORKER_SEA))
				{
					return;
				}
			}

			if (iLandBonuses >= 1  && getPopulation() > 1)
			{
				if (!bChooseWorker && AI_chooseUnit("secondary worker", UNITAI_WORKER))
				{
					return;
				}
				bChooseWorker = true;
			}
		}
	}

	m_iTempBuildPriority--;

	if ( kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3) )
	{
		if (iHealth < 1)
		{
			if ( AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, 20, 0, (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4) ? 50 : 20)) )
			{
				return;
			}
		}
	}

	if( GET_TEAM(getTeam()).isAVassal() && GET_TEAM(getTeam()).isCapitulated() )
	{
		if( !bLandWar )
		{
			if (iHealth < 1)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, 30, 0, 3*getPopulation()))
				{
					return;
				}
			}

			if ((getPopulation() > 3) && (getCommerceRate(COMMERCE_CULTURE) < 5))
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30, 0 + 3*iWarTroubleThreshold, 3*getPopulation()))
				{
					return;
				}
			}
		}
	}
 
	
	m_iTempBuildPriority--;

	// -------------------- BBAI Notes -------------------------
	// Minimal attack force, both land and sea
	if (bDanger && !bInhibitUnits ) 
	{
		int iAttackNeeded = 4;
/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
		iAttackNeeded += std::max(0, AI_neededDefenders() - (iPlotCityDefenderCount + iPlotOtherCityAICount));
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/

		if( kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK) <  iAttackNeeded)
		{
			if (AI_chooseUnit("minimal attack (danger)", UNITAI_ATTACK))
			{
				return;
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (bMaybeWaterArea && !bInhibitUnits)
	{
		if( !(bLandWar && iWarSuccessRatio < -30) && !bDanger && !bFinancialTrouble )
		{
			if (kPlayer.AI_getNumTrainAIUnits(UNITAI_ATTACK_SEA) + kPlayer.AI_getNumTrainAIUnits(UNITAI_PIRATE_SEA) + kPlayer.AI_getNumTrainAIUnits(UNITAI_RESERVE_SEA) < std::min(3,kPlayer.getNumCities()))
			{
				if ((bMaybeWaterArea && bWaterDanger)
					|| (pWaterArea != NULL &&
						bPrimaryArea &&
						kPlayer.AI_countNumAreaHostileUnits(pWaterArea, true, false, false, false, plot(), 15) > 0) )
				{
					int iLocalNavy = kPlayer.AI_countNumLocalNavy(plot(),4);
					
					if ( iLocalNavy < 2 )	//	If there are a few local appropriate AI units already assume they will deal
					{
						if (AI_chooseUnit("minimal navy", UNITAI_ATTACK_SEA))
						{
							return;
						}
						if (AI_chooseUnit("minimal navy", UNITAI_PIRATE_SEA))
						{
							return;
						}
						if (AI_chooseUnit("minimal navy", UNITAI_RESERVE_SEA))
						{
							return;
						}
					}
				}
			}
		
			if (NULL != pWaterArea)
			{
				int iOdds = -1;
				if (iAreaBestFoundValue == 0 || iWaterAreaBestFoundValue > iAreaBestFoundValue)
				{
					iOdds = 100;
				}
				else if (iWaterPercent > 60)
				{
					iOdds = 13;
				}

				if( iOdds >= 0 )
				{
					if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) == 0)
					{
						if (AI_chooseUnit("sea explorer", UNITAI_EXPLORE_SEA, iOdds))
						{
							return;
						}
					}

					// BBAI TODO: Really only want to do this if no good area city sites ... 13% chance on water heavy maps
					// of slow start, little benefit
					if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) == 0)
					{
						if (AI_chooseUnit("settler sea", UNITAI_SETTLER_SEA, iOdds))
						{
							return;
						}
					}
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	// -------------------- BBAI Notes -------------------------
	// Top normal priorities
	
	if (!bPrimaryArea && !bLandWar)
	{
		if (!isHuman() || AI_isEmphasizeYield(YIELD_FOOD))
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_FOOD, 60, 10 + 2*iWarTroubleThreshold, isHuman() ? -1 : 50))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_FOOD 1", getName().GetCString());
				return;
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (!bDanger && ((kPlayer.getCurrentEra() > (GC.getGame().getStartEra() + iProductionRank / 2))) || (kPlayer.getCurrentEra() > (GC.getNumEraInfos() / 2)))
	{
		if (!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, 20 - iWarTroubleThreshold, 15, ((!isHuman() && (bLandWar || bAssault)) ? 25 : -1)))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_PRODUCTION 1", getName().GetCString());
				return;	
			}
		}

		if( !bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -30) )
		{
			if ((iExistingWorkers < ((iNeededWorkers + 1) / 2)))
			{
				if( getPopulation() > 3 || (iProductionRank < (kPlayer.getNumCities() + 1) / 2) )
				{
					if (!bChooseWorker && AI_chooseUnit("no danger workers", UNITAI_WORKER))
					{
						return;
					}
					bChooseWorker = true;
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	bool bCrushStrategy = kPlayer.AI_isDoStrategy(AI_STRATEGY_CRUSH);
	int iNeededFloatingDefenders = (isNPC() || bCrushStrategy) ?  0 : kPlayer.AI_getTotalFloatingDefendersNeeded(pArea);
	int iTotalFloatingDefenders = (isNPC() ? 0 : kPlayer.AI_getTotalFloatingDefenders(pArea));
	
	UnitTypeWeightArray floatingDefenderTypes;
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 125));
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_COUNTER, 100));
	//floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_SPECIAL, 0));
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_RESERVE, 100));
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));
	
	if (!bInhibitUnits && iTotalFloatingDefenders < ((iNeededFloatingDefenders + 1) / (bGetBetterUnits ? 3 : 2)))
	{
		if (/*!bUnitExempt && */AI_chooseLeastRepresentedUnit("floating defender", floatingDefenderTypes))// K-Mod
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	// If losing badly in war, need to build up defenses and counter attack force
	if( !bInhibitUnits && bLandWar && (iWarSuccessRatio < -30 || iEnemyPowerPerc > 150) )
	{
		UnitTypeWeightArray defensiveTypes;
		defensiveTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		defensiveTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));
		defensiveTypes.push_back(std::make_pair(UNITAI_RESERVE, 60));
		defensiveTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 60));
		if ( bDanger || (iTotalFloatingDefenders < (5*iNeededFloatingDefenders)/(bGetBetterUnits ? 6 : 4)))
		{
			defensiveTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 200));
			defensiveTypes.push_back(std::make_pair(UNITAI_CITY_COUNTER, 50));
		}

		int iOdds = iBuildUnitProb;
		if( iWarSuccessRatio < -50 )
		{
			iOdds += abs(iWarSuccessRatio/3);
		}
		if( bDanger )
		{
			iOdds += 10;
		}

		if (AI_chooseLeastRepresentedUnit("extra defense", defensiveTypes, iOdds))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	if( !bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -50) )
	{
		if (!(iExistingWorkers == 0))
		{
			if (!bDanger && (iExistingWorkers < ((iNeededWorkers + 1) / 2)))
			{
				if( getPopulation() > 3 || (iProductionRank < (kPlayer.getNumCities() + 1) / 2) )
				{
					if (!bChooseWorker && AI_chooseUnit("no danger large city extra worker", UNITAI_WORKER))
					{
						return;
					}
					bChooseWorker = true;
				}
			}
		}
	}
	
#if 0
	//do a check for one tile island type thing?
	//this can be overridden by "wait and grow more"
	if (!bDanger && (iExistingWorkers == 0) && (isCapital() || (iNeededWorkers > 0) || (iNeededSeaWorkers > iExistingSeaWorkers)))
	{
		if( !(bDefenseWar && iWarSuccessRatio < -30) && !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
		{
			if ((AI_countNumBonuses(NO_BONUS, /*bIncludeOurs*/ true, /*bIncludeNeutral*/ true, -1, /*bLand*/ true, /*bWater*/ false) > 0) || 
				(isCapital() && (getPopulation() > 3) && iNumCitiesInArea > 1))
			{
				if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 5", getName().GetCString());
					return;
				}
				bChooseWorker = true;
			}

			if (iNeededSeaWorkers > iExistingSeaWorkers)
			{
				if (AI_chooseUnit(UNITAI_WORKER_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker sea 2", getName().GetCString());
					return;
				}
			}
		}
	}
#endif

	m_iTempBuildPriority--;

	if ( m_iTempBuildPriority >= LOW_PRIORITY_ESCORT_PRIORITY )
	{
		m_iTempBuildPriority = LOW_PRIORITY_ESCORT_PRIORITY - 1;
	}

	if (!m_bRequestedUnit)
	{
		if( !bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -30) )
		{
			if (!bWaterDanger && iNeededSeaWorkers > iExistingSeaWorkers)
			{
				if (AI_chooseUnit("no danger extra sea worker", UNITAI_WORKER_SEA))
				{
					return;
				}
			}
		}
	}

//TB Build Mod (Moves below to lower priority)
#ifndef C2C_BUILD
	if	(!bLandWar && !bAssault && (iTargetCulturePerTurn > getCommerceRate(COMMERCE_CULTURE)))
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, bAggressiveAI ? 10 : 20, 0, bAggressiveAI ? 33 : 50))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses minimal culture rate", getName().GetCString());
			return;
		}
	}
#endif
//TB Build Mod end	

	m_iTempBuildPriority--;

	bool bRivalCitiesExist = (area()->getNumCities() > iNumCapitalAreaCities);

	if (!m_bRequestedUnit)
	{
		if (!bInhibitUnits && bRivalCitiesExist && bIsCapitalArea)
		{
			//harass first
			int iInfiltratorCount = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_INFILTRATOR);
			if (iInfiltratorCount < 1)//eventually find a way to vary the amount of desired infiltrators by leader personality
			{
				//this is enough of a trigger.  Once introduced they will expand themselves if needed and possible.
				if (AI_chooseUnit("Infiltrator needed", UNITAI_INFILTRATOR) )
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	// BBAI TODO: Check that this works to produce early rushes on tight maps
	if (!bInhibitUnits && !bGetBetterUnits && (bIsCapitalArea) && (iAreaBestFoundValue < (iMinFoundValue * 2)))
	{
		//Building city hunting stack.

		if ((getDomainFreeExperience(DOMAIN_LAND) == 0) && (getYieldRate(YIELD_PRODUCTION) > 4))
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, (kPlayer.getCurrentEra() > 1) ? 0 : 7, 33))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses special BUILDINGFOCUS_EXPERIENCE 1a", getName().GetCString());
				return;
			}
		}

		int iStartAttackStackRand = 0;
		if (pArea->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0 || pArea->getCitiesPerPlayer(NPC7_PLAYER) > 0)
		{
			iStartAttackStackRand += 15;
		}
		if ((pArea->getNumCities() - iNumCitiesInArea) > 0)
		{
			iStartAttackStackRand += iBuildUnitProb / 2;
		}

		if( iStartAttackStackRand > 0 )///CHECK THIS OUT
		{
			int iAttackCityCount = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
			int iAttackCount = iAttackCityCount + kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);

			if( (iAttackCount) == 0 )
			{
				if( !bFinancialTrouble )
				{
					if (AI_chooseUnit("build attack force", UNITAI_ATTACK, iStartAttackStackRand))
					{
						return;
					}
				}
			}
			else
			{
				if( (iAttackCount > 1) && (iAttackCityCount == 0) )
				{
					if (AI_chooseUnit("start city attack stack", UNITAI_ATTACK_CITY))
					{
						return;
					}
				}
				else if (iAttackCityCount < (3 + iBuildUnitProb / 10))
				{
					if (AI_chooseUnit("add to city attack stack", UNITAI_ATTACK_CITY))
					{
						return;
					}
				}
				else if (iAttackCount-iAttackCityCount < (3 + iBuildUnitProb / 10))
				{
					if (AI_chooseUnit("add to attack stack", UNITAI_ATTACK))
					{
						return;
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//opportunistic wonder build (1)
	if (!bDanger && (!hasActiveWorldWonder()) && (kPlayer.getNumCities() <= 3))
	{
		// For small civ at war, don't build wonders unless winning
		if( !bLandWar || (iWarSuccessRatio > 30) )
		{
/************************************************************************************************/
/* Afforess	                  Start		 06/30/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
			int iWonderTime = GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand");
*/
			int iWonderTime = GC.getGameINLINE().getSorenRandNum(GET_PLAYER(getOwnerINLINE()).getWonderConstructRand(), "Wonder Construction Rand");
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

			iWonderTime /= 5;
			iWonderTime += 7;
			if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, iWonderTime))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses oppurtunistic wonder build 1", getName().GetCString());
				return;
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (!bDanger && !bIsCapitalArea && area()->getCitiesPerPlayer(getOwnerINLINE()) > iNumCapitalAreaCities)
	{
		// BBAI TODO:  This check should be done by player, not by city and optimize placement
		// If losing badly in war, don't build big things
		if( !bLandWar || (iWarSuccessRatio > -30) )
		{
			if( kPlayer.getCapitalCity() == NULL || area()->getPopulationPerPlayer(getOwnerINLINE()) > kPlayer.getCapitalCity()->area()->getPopulationPerPlayer(getOwnerINLINE()) )
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CAPITAL, 15))
				{
					return;
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (!isHuman() || AI_isEmphasizeYield(YIELD_FOOD))
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_FOOD, isCapital() ? 5 : 30, 30))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_FOOD 2", getName().GetCString());
			return;
		}
	}

	int iSpreadUnitThreshold = 1000;

	if( bLandWar )
	{
		iSpreadUnitThreshold += 800 - 10*iWarSuccessRatio;
	}
	iSpreadUnitThreshold += 300*plot()->plotCount(PUF_isUnitAIType, UNITAI_MISSIONARY, -1, NULL, getOwnerINLINE());

	UnitTypes eBestSpreadUnit = NO_UNIT;
	int iBestSpreadUnitValue = -1;
	
	if( !bInhibitUnits && !bDanger && !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
	{
		int iSpreadUnitRoll = (100 - iBuildUnitProb) / 3;
		iSpreadUnitRoll += bLandWar ? 0 : 10;
		
		if (AI_bestSpreadUnit(true, true, iSpreadUnitRoll, &eBestSpreadUnit, &iBestSpreadUnitValue))
		{
			if (iBestSpreadUnitValue > iSpreadUnitThreshold)
			{
				if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

/************************************************************************************************/
/* RevolutionDCM  Inquisitions                             01/29/10             Afforess        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (!bInhibitUnits && AI_trainInquisitor())
	{
		return;
	}
/************************************************************************************************/
/* RevolutionDCM	                         END                                                */
/************************************************************************************************/
	
	if( !isHuman() && !(bLandWar && iWarSuccessRatio < 30) )
	{
		if (!bDanger && (iProductionRank <= ((kPlayer.getNumCities() / 5) + 1)))
		{
			// BBAI TODO: Temporary for testing
			//if( getOwnerINLINE()%2 == 1 )
			//{
				if (AI_chooseProject())
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose project 1", getName().GetCString());
					return;
				}
			//}
		}
	}

	m_iTempBuildPriority--;

/********************************************************************************/
/* 	Build more workers #2										Fuyu		    */
/********************************************************************************/
	if( !bInhibitUnits && !(bLandWar && iWarSuccessRatio < 0) && !bDanger/* && !bBestWorkerIsOneShot*/)	//	KOSHLING - don't build frivolous one-shot workers like this; TB-Workers aren't doing enough early on
	{
		/* financial trouble: ---; will grow above happy cap: 2/3; both: 3/4; else 4/7 */
		if ( (iExistingWorkers < ((4*iNeededWorkers) + 6)/7)
			/* || (bFinancialTrouble && (iExistingWorkers < (((2*iNeededWorkers) + 1)/3))) */
			|| (((iExistingWorkers < ((2*iNeededWorkers) + 2)/3) || (bFinancialTrouble && (iExistingWorkers < (((3*iNeededWorkers) + 3)/4))))
				&& (((happyLevel() - unhappyLevel()) <= 0) && (foodDifference(false) > 0 || (foodDifference(false) == 0 && happyLevel() - unhappyLevel() < 0)))) )
		{
			if ((AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
			{
				if( getPopulation() > 2 || (GC.getGameINLINE().getGameTurn() - getGameTurnAcquired() > (15 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent())/100) )
				{
					if (!bChooseWorker && AI_chooseUnit("worker for established city", UNITAI_WORKER))
					{
						return;

						//	Koshling - the following is not operatiional with the unit tendering system, and I can't quite fuigure out what it was for anyway, so
						//	deactivated for now

						//if (!isFoodProduction() && ((happyLevel() - unhappyLevel()) <= 0)
						//	&& getProductionUnit() != NO_UNIT && getUnitProduction(getProductionUnit()) == 0)
						//{
						//	popOrder(0);
						//	bChooseWorker = true;

							//Already set by chooseUnit but I'm not taking any chances
						//	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
						//	{
						//		setInfoDirty(true);

						//		if (isCitySelected())
						//		{
						//			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
						//			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
						//			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
						//			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
						//		}
						//	}
						//}
						//else
						//{
						//	if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 6b", getName().GetCString());
						//	return;
						//}
					}
					bChooseWorker = !bChooseWorker;
				}
			}
		}
	}
/********************************************************************************/
/* 	Build more workers #2										END			   */
/********************************************************************************/


	m_iTempBuildPriority--;

	//this is needed to build the cathedrals quickly
	//also very good for giving cultural cities first dibs on wonders
	if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
	{
		if (bImportantCity && (iCultureRateRank <= iCulturalVictoryNumCultureCities))
		{
			if (iCultureRateRank == iCulturalVictoryNumCultureCities)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_BIGCULTURE | BUILDINGFOCUS_CULTURE | BUILDINGFOCUS_WONDEROK, 40))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses cultural victory 1", getName().GetCString());
					return;
				}
			}
			else if (GC.getGameINLINE().getSorenRandNum(((iCultureRateRank == 1) ? 4 : 1) + iCulturalVictoryNumCultureCities * 2 + (bLandWar ? 5 : 0), "AI Build up Culture") < iCultureRateRank)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_BIGCULTURE | BUILDINGFOCUS_CULTURE | BUILDINGFOCUS_WONDEROK, (bLandWar ? 20 : 40)))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses cultural victory 2", getName().GetCString());
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	// don't build frivolous things if this is an important city unless we at war
	if (!bInhibitUnits && (!bImportantCity || bLandWar || bAssault))
	{
		//	Koshling in early game moved optional non-wartime attack unit builds below economy
		if (bPrimaryArea && kPlayer.getCurrentEra() != 0)
		{
			if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK) == 0)
			{
				if (AI_chooseUnit("optional attack", UNITAI_ATTACK))
				{
					return;
				}
			}
		}

		if ((bMassing || !bLandWar) && !bDanger && !bFinancialTrouble)
		{
			//	Normalize them by the ideally needed numbers (which are not limited by our number of cities and
			//	reflect the geography only)
			iExplorerDeficitPercent *= kPlayer.AI_neededExplorers(pArea, true);
			iHunterDeficitPercent *= kPlayer.AI_neededHunters(pArea, true);

			if ( iExplorerDeficitPercent >= iHunterDeficitPercent && iExplorerDeficitPercent > 0 )
			{
				//	If we are just pumping out explorer units and having them die fast
				//	go for EXP giving buildings first
				UnitTypes eBestUnit = AI_bestUnitAI(UNITAI_EXPLORE, iUnitValue);
				if ( eBestUnit != NO_UNIT )
				{
					if ( isCapital() )
					{
						//	What is the cost of one of these units?
						int iCost = (GC.getUnitInfo(eBestUnit).getProductionCost() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent()) / 100;
						int perTurnLossesCost = (iCost*area()->getRecentCombatDeathRate(getOwnerINLINE(), UNITAI_EXPLORE))/100;

						if( gCityLogLevel >= 2 )
						{
							logBBAI("      City %S evaluating recent explorer deaths - %d per/turn cost vs %d production",
									getName().GetCString(),
									perTurnLossesCost,
									GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION));
						}

						if ( perTurnLossesCost > GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION)/6 )
						{
							if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 30))
							{
								return;
							}
						}
					}

					if (AI_chooseUnit("need explorers", UNITAI_EXPLORE))
					{
						return;
					}
				}
			}

			if (iHunterDeficitPercent > 0)
			{
				//	If we are just pumping out hunting units and having them die fast
				//	go for EXP giving buildings first
				UnitTypes eBestUnit = AI_bestUnitAI(UNITAI_HUNTER, iUnitValue, false);
				if ( eBestUnit != NO_UNIT )
				{
					//	What is the cost of one of these units?
					int iCost = (GC.getUnitInfo(eBestUnit).getProductionCost() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent()) / 100;
					int perTurnLossesCost = (iCost*area()->getRecentCombatDeathRate(getOwnerINLINE(), UNITAI_HUNTER))/100;

					if( gCityLogLevel >= 2 )
					{
						logBBAI("      City %S evaluating recent hunter deaths - %d per/turn cost vs %d production",
							getName().GetCString(),
							perTurnLossesCost,
							GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION));
					}

					if ( perTurnLossesCost > GET_PLAYER(getOwnerINLINE()).calculateTotalYield(YIELD_PRODUCTION)/6 )
					{
						if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 30))
						{
							return;
						}
					}

					if (AI_chooseUnit("need hunters", UNITAI_HUNTER))
					{
						return;
					}
				}
			}
		}

		if( bDefenseWar || (bLandWar && (iWarSuccessRatio < -30)) )
		{
			UnitTypeWeightArray panicDefenderTypes;
			panicDefenderTypes.push_back(std::make_pair(UNITAI_RESERVE, 100));
			panicDefenderTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
			panicDefenderTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));
			panicDefenderTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));

			if (AI_chooseLeastRepresentedUnit("panic defender", panicDefenderTypes, (bGetBetterUnits ? 40 : 60) - iWarSuccessRatio/3))
			{
				return;
			}
		}
	}
		
	m_iTempBuildPriority--;

/************************************************************************************************/
/* Afforess	                  Start		 1/22/12                                                */
/*                                                                                              */
/* Encourage Aggressive AI to stock up on a few nukes                                           */
/************************************************************************************************/
	if (!bInhibitUnits && (GC.getGameINLINE().isOption(GAMEOPTION_RUTHLESS_AI) || kPlayer.isEnabledMAD() || (GC.getGameINLINE().getSorenRandNum(10, "AI consider Nuke") == 0)))
	{
		if(!bFinancialTrouble)
		{
			int iTotalNukes = kPlayer.AI_totalUnitAIs(UNITAI_ICBM);
			//keep between 3 to 8 nukes, at least for early considerations
			int iNukesNeeded = std::max(3, std::min((GC.getGame().getNumCities() - kPlayer.getNumCities()) / 5, 8));
			if (iTotalNukes < iNukesNeeded)
			{
				//Reordered, because nukes are more valuable than carriers
				if (AI_chooseUnit("ICBM", UNITAI_ICBM))
				{
					return;
				}
			}
		}
	}  
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/ 

	m_iTempBuildPriority--;

	if (!isHuman() || AI_isEmphasizeYield(YIELD_FOOD))
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_FOOD, 60, 10, (bLandWar ? 30 : -1)))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_FOOD 3", getName().GetCString());
			return;
		}
	}
	
	m_iTempBuildPriority--;

	//opportunistic wonder build
	if (!isHuman() && !bDanger && (!hasActiveWorldWonder() || (kPlayer.getNumCities() > 3)))
	{
		// For civ at war, don't build wonders if losing
		if( !bLandWar || (iWarSuccessRatio > -30) )
		{
/************************************************************************************************/
/* Afforess	                  Start		 06/30/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
			int iWonderTime = GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand");
*/
			int iWonderTime = GC.getGameINLINE().getSorenRandNum(GET_PLAYER(getOwnerINLINE()).getWonderConstructRand(), "Wonder Construction Rand");
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

			iWonderTime /= 5;
			iWonderTime += 8;
			if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, iWonderTime))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses oppurtunistic wonder build 2", getName().GetCString());
				return;
			}
		}
	}
	
	m_iTempBuildPriority--;

	if( !bInhibitUnits && !(bLandWar && iWarSuccessRatio < -30) && !bDanger )
	{
		if (iExistingWorkers < iNeededWorkers )
		{
			if ((AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
			{
				if( getPopulation() > 1 || (GC.getGameINLINE().getGameTurn() - getGameTurnAcquired() > (15 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent())/100) )
				{
					if (!bChooseWorker && AI_chooseUnit("established city needs more workers", UNITAI_WORKER))
					{
						return;
					}
					bChooseWorker = true;
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	//	semi-urgent property control (bad property > 15% through op range - choose unit to deal if possible)
	if ( AI_choosePropertyControlUnit( 15 ) )
	{
		return;
	}
	
	m_iTempBuildPriority--;

	//essential economic builds
	if (AI_chooseBuilding(iEconomyFlags, 10, (iEcononmyFlagsThreasholdWeighting*(25 + iWarTroubleThreshold))/100, (bLandWar && !isHuman() ? 40 : -1)))
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose iEconomyFlags 1", getName().GetCString());
		return;
	}

	m_iTempBuildPriority--;

/************************************************************************************************/
/* Afforess	                  Start		 07/28/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if ((isCapital() || getYieldRate(YIELD_PRODUCTION) > std::min(70, std::max(40, iNumCitiesInArea * 6))) && !bDanger)
	{
		if (bFinancialTrouble)
		{
			if (AI_chooseProcess(COMMERCE_GOLD))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S chooses to build wealth due to financial trouble", getName().GetCString());
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//	normal running property control (bad property > 10% through op range - choose unit to deal if possible)
	if ( AI_choosePropertyControlUnit( 10 ) )
	{
		return;
	}

	m_iTempBuildPriority--;

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if( !bInhibitUnits && !bDanger )
	{
		if (iBestSpreadUnitValue > ((iSpreadUnitThreshold * (bLandWar ? 80 : 60)) / 100))
		{
			if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	if ((getDomainFreeExperience(DOMAIN_LAND) == 0) && (getYieldRate(YIELD_PRODUCTION) > 4))
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, (kPlayer.getCurrentEra() > 1) ? 0 : 7, 33))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses special BUILDINGFOCUS_EXPERIENCE 1", getName().GetCString());
			return;
		}
	}

	m_iTempBuildPriority--;

	//	Koshling - increased priority for economic builds, especially in early game and allowed
	//	wonders in if they appear best
	if (AI_chooseBuilding(iEconomyFlags | BUILDINGFOCUS_WONDEROK, 20, 0, kPlayer.getCurrentEra() == 0 ? 100 : 50))
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose iEconomyFlags (Koshling)", getName().GetCString());
		return;
	}

	m_iTempBuildPriority--;

	// Koshling in early game moved optional non-wartime attack unit builds below economy
	// don't build frivolous things if this is an important city unless we at war
	if (!bInhibitUnits && (!bImportantCity || bLandWar || bAssault) && kPlayer.getCurrentEra() == 0)
	{
		if (bPrimaryArea)
		{
			if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK) == 0)
			{
				if (AI_chooseUnit("primary area attack", UNITAI_ATTACK))
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//	Koshling - next section moved from quite a bit earlier to avoid not-needed-yet worker builds
	//	before we have checked basic economy builds
	//do a check for one tile island type thing?
	//this can be overridden by "wait and grow more"
	if (!bInhibitUnits && !bDanger && (iExistingWorkers == 0) && (isCapital() || (iNeededWorkers > 0) || (iNeededSeaWorkers > iExistingSeaWorkers)))
	{
		if( !(bDefenseWar && iWarSuccessRatio < -30) && !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
		{
			if ((AI_countNumBonuses(NO_BONUS, /*bIncludeOurs*/ true, /*bIncludeNeutral*/ true, -1, /*bLand*/ true, /*bWater*/ false) > 0) || 
				(isCapital() && (getPopulation() > 3) && iNumCitiesInArea > 1))
			{
				if (!bChooseWorker && AI_chooseUnit("optional worker", UNITAI_WORKER))
				{
					return;
				}
				bChooseWorker = true;
			}

			if (iNeededSeaWorkers > iExistingSeaWorkers)
			{
				if (AI_chooseUnit("optional sea worker", UNITAI_WORKER_SEA))
				{
					return;
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	int iMaxUnitSpending = (bAggressiveAI ? 6 : 3) + iBuildUnitProb / 3;

	if( kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4) )
	{
		iMaxUnitSpending += 7;
	}
	else if( kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST3) || kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3) )
	{
		iMaxUnitSpending += 3;
	}
	else if( kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST1) )
	{
		iMaxUnitSpending += 1;
	}

	if (kPlayer.AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
	{
		iMaxUnitSpending = 5 + iMaxUnitSpending + (100 - iMaxUnitSpending) / 2;
	}
	else
	{
		iMaxUnitSpending += bDefenseWar ? 4 : 0;
		switch (pArea->getAreaAIType(getTeam()))
		{
			case AREAAI_OFFENSIVE:
				iMaxUnitSpending += 5;
				break;
			
			case AREAAI_DEFENSIVE:
				iMaxUnitSpending += 10;
				break;
			
			case AREAAI_MASSING:
				iMaxUnitSpending += 25;
				break;
			
			case AREAAI_ASSAULT:
				iMaxUnitSpending += 8;
				break;
				
			case AREAAI_ASSAULT_MASSING:
				iMaxUnitSpending += 16;
				break;
			
			case AREAAI_ASSAULT_ASSIST:
				iMaxUnitSpending += 6;
				break;
			
			case AREAAI_NEUTRAL:
				break;
			default:
				FAssert(false);
		}
	}

	int iCarriers = kPlayer.AI_totalUnitAIs(UNITAI_CARRIER_SEA);
	
	//Afforess moved AI Aircraft logic up, Aircraft >> Sea transports
	UnitTypeWeightArray airUnitTypes;

	int iAircraftNeed = 0;
	int iAircraftHave = 0;
	UnitTypes eBestAttackAircraft = NO_UNIT;
	UnitTypes eBestMissile = NO_UNIT;
	
	if (!bInhibitUnits && iUnitCostPercentage < (iMaxUnitSpending + 4) && (!bImportantCity || bDefenseWar) )
	{
		if( bLandWar || bAssault || (iFreeAirExperience > 0) || (GC.getGameINLINE().getSorenRandNum(3, "AI train air") == 0) )
		{
			int iBestAirValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_ATTACK_AIR, this, &eBestAttackAircraft);
			int iBestMissileValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_MISSILE_AIR, this, &eBestMissile);
			if ((iBestAirValue + iBestMissileValue) > 0)
			{
				iAircraftHave = kPlayer.AI_totalUnitAIs(UNITAI_ATTACK_AIR) + kPlayer.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) + kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_AIR);
				if (NO_UNIT != eBestAttackAircraft)
				{
					iAircraftNeed = (2 + kPlayer.getNumCities() * (3 * GC.getUnitInfo(eBestAttackAircraft).getAirCombat())) / (2 * std::max(1, GC.getGame().getBestLandUnitCombat()));
					int iBestDefenseValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_DEFENSE_AIR, this);
					if ((iBestDefenseValue > 0) && (iBestAirValue > iBestDefenseValue))
					{
						iAircraftNeed *= 3;
						iAircraftNeed /= 2;
					}
				}
				if (iBestMissileValue > 0)
				{
					iAircraftNeed = std::max(iAircraftNeed, 1 + kPlayer.getNumCities() / 2);
				}
				
				bool bAirBlitz = kPlayer.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ);
				bool bLandBlitz = kPlayer.AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ);
				if (bAirBlitz)
				{
					iAircraftNeed *= 3;
					iAircraftNeed /= 2;
				}
				else if (bLandBlitz)
				{
					iAircraftNeed /= 2;
					iAircraftNeed += 1;
				}
				
				airUnitTypes.push_back(std::make_pair(UNITAI_ATTACK_AIR, bAirBlitz ? 125 : 80));
				airUnitTypes.push_back(std::make_pair(UNITAI_DEFENSE_AIR, /*bLandBlitz ? 100 :*/ 100));
				if (iBestMissileValue > 0)
				{
					airUnitTypes.push_back(std::make_pair(UNITAI_MISSILE_AIR, bAssault ? 60 : 40));
				}
				
				airUnitTypes.push_back(std::make_pair(UNITAI_ICBM, 20));
				
				if (iAircraftHave * 2 < iAircraftNeed)
				{
					if (AI_chooseLeastRepresentedUnit("need airforce", airUnitTypes))
					{
						return;
					}
				}
				// Additional check for air defenses
				int iFightersHave = kPlayer.AI_totalUnitAIs(UNITAI_DEFENSE_AIR);

				if( 3*iFightersHave < iAircraftNeed )
				{
					if (AI_chooseUnit("need air defense", UNITAI_DEFENSE_AIR))
					{
						return;
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	// Revamped logic for production for invasions
	if (!bInhibitUnits && iUnitCostPercentage < (iMaxUnitSpending + 10))
	{
		bool bBuildAssault = bAssault;
		CvArea* pAssaultWaterArea = NULL;
		if (NULL != pWaterArea)
		{
			// Coastal city extra logic

			pAssaultWaterArea = pWaterArea;

			// If on offensive and can't reach enemy cities from here, act like using AREAAI_ASSAULT
			if( (pAssaultWaterArea != NULL) && !bBuildAssault )
			{
				if( (GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0) )
				{
					if( (pArea->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE) )
					{
						// BBAI TODO: faster to switch to checking path for some selection group?
						if( !(plot()->isHasPathToEnemyCity(getTeam())) )
						{
							bBuildAssault = true;
						}
					}
				}
			}
		}

		if( bBuildAssault )
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build assault", getName().GetCString());

			UnitTypes eBestAssaultUnit = NO_UNIT; 
			if (NULL != pAssaultWaterArea)
			{
				kPlayer.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, this, &eBestAssaultUnit);
			}
			else
			{
				kPlayer.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, NULL, &eBestAssaultUnit);
			}
			
			int iBestSeaAssaultCapacity = 0;
			if (eBestAssaultUnit != NO_UNIT)
			{
				iBestSeaAssaultCapacity = GC.getUnitInfo(eBestAssaultUnit).getCargoSpace();
			}

			int iAreaAttackCityUnits = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
			
			int iUnitsToTransport = iAreaAttackCityUnits;
			iUnitsToTransport += kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
			iUnitsToTransport += kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_COUNTER)/2;

			int iLocalTransports = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ASSAULT_SEA);
			int iTransportsAtSea = 0;
			if (NULL != pAssaultWaterArea)
			{
				iTransportsAtSea = kPlayer.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ASSAULT_SEA);
			}
			else
			{
				iTransportsAtSea = kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA)/2;
			}

			//The way of calculating numbers is a bit fuzzy since the ships
			//can make return trips. When massing for a war it'll train enough
			//ships to move it's entire army. Once the war is underway it'll stop
			//training so many ships on the assumption that those out at sea
			//will return...

			int iTransports = iLocalTransports + (bPrimaryArea ? iTransportsAtSea : iTransportsAtSea/4);
			int iTransportCapacity = iBestSeaAssaultCapacity*(iTransports);

			if (NULL != pAssaultWaterArea)
			{
				int iEscorts = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ESCORT_SEA);
				iEscorts += kPlayer.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ESCORT_SEA);

				int iTransportViability = kPlayer.AI_calculateUnitAIViability(UNITAI_ASSAULT_SEA, DOMAIN_SEA);

				int iDesiredEscorts = ((1 + 2 * iTransports) / 3);
				if( iTransportViability > 95 )
				{
					// Transports are stronger than escorts (usually Galleons and Caravels)
					iDesiredEscorts /= 3;
				}
				
				if ((iEscorts < iDesiredEscorts))
				{
					if ( AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12) )
					{
						return;
					}

					if (AI_chooseUnit("sea escort", UNITAI_ESCORT_SEA, (iEscorts < iDesiredEscorts/3) ? -1 : 50))
					{
						return;
					}
				}
			
				UnitTypes eBestAttackSeaUnit = NO_UNIT;  
				kPlayer.AI_bestCityUnitAIValue(UNITAI_ATTACK_SEA, this, &eBestAttackSeaUnit);
				if (eBestAttackSeaUnit != NO_UNIT)
				{
					int iDivisor = 2;
					if (GC.getUnitInfo(eBestAttackSeaUnit).getBombardRate() == 0)
					{
						iDivisor = 5;
					}

					int iAttackSea = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_SEA);
					iAttackSea += kPlayer.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ATTACK_SEA);
						
					if ((iAttackSea < ((1 + 2 * iTransports) / iDivisor)))
					{
						if ( AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12) )
						{
							return;
						}

						if (AI_chooseUnit("sea attack", UNITAI_ATTACK_SEA, (iUnitCostPercentage < iMaxUnitSpending) ? 50 : 20))
						{
							return;
						}
					}
				}
				
				if (iUnitsToTransport > iTransportCapacity)
				{
					if ((iUnitCostPercentage < iMaxUnitSpending) || (iUnitsToTransport > 2*iTransportCapacity))
					{
						if ( AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12) )
						{
							return;
						}

						if (AI_chooseUnit("sea assault", UNITAI_ASSAULT_SEA))
						{
							return;
						}
					}
				}
			}

			if (iUnitCostPercentage < iMaxUnitSpending)
			{
				if (NULL != pAssaultWaterArea)
				{
					if (!bFinancialTrouble && iCarriers < (kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) / 4))
					{
						// Reduce chances of starting if city has low production
						if ( iProductionRank > (kPlayer.getNumCities() / 3) && GC.getGameINLINE().getSorenRandNum(100, "AI train carrier") < 30 )
						{
							if ( AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12) )
							{
								return;
							}

							if (AI_chooseUnit("need carrier", UNITAI_CARRIER_SEA) )
							{
								return;
							}
						}
					}
				}
			}

			// Consider building more land units to invade with
			int iTrainInvaderChance = iBuildUnitProb + 10;

			iTrainInvaderChance += (bAggressiveAI ? 15 : 0);
			iTrainInvaderChance /= (bAssaultAssist ? 2 : 1);
			iTrainInvaderChance /= (bImportantCity ? 2 : 1);
			iTrainInvaderChance /= (bGetBetterUnits ? 2 : 1);

			iUnitsToTransport *= 9;
			iUnitsToTransport /= 10;

			if( (iUnitsToTransport > iTransportCapacity) && (iUnitsToTransport > (bAssaultAssist ? 2 : 4)*iBestSeaAssaultCapacity) )
			{
				// Already have enough
				iTrainInvaderChance /= 2;
			}
			else if( iUnitsToTransport < (iLocalTransports*iBestSeaAssaultCapacity) )
			{
				iTrainInvaderChance += 15;
			}

			if( getPopulation() < 4 )
			{
				// Let small cities build themselves up first
				iTrainInvaderChance /= (5 - getPopulation());
			}

			UnitTypeWeightArray invaderTypes;
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 100));
			invaderTypes.push_back(std::make_pair(UNITAI_COUNTER, 50));
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK, 40));
			if( kPlayer.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ) )
			{
				invaderTypes.push_back(std::make_pair(UNITAI_PARADROP, 20));
			}

			if( !bImportantCity && (iUnitsToTransport >= (iLocalTransports*iBestSeaAssaultCapacity)) )
			{
				// Have time to build barracks first
				if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 20))
				{
					return;
				}
			}

			if (AI_chooseLeastRepresentedUnit("invader units", invaderTypes, iTrainInvaderChance))
			{
				return;
			}

			if (iUnitCostPercentage < (iMaxUnitSpending))
			{
				int iMissileCarriers = kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_CARRIER_SEA);
			
				if (!bFinancialTrouble && iMissileCarriers > 0 && !bImportantCity)
				{
					if( (iProductionRank <= ((kPlayer.getNumCities() / 2) + 1)) )
					{
						UnitTypes eBestMissileCarrierUnit = NO_UNIT;  
						kPlayer.AI_bestCityUnitAIValue(UNITAI_MISSILE_CARRIER_SEA, NULL, &eBestMissileCarrierUnit);
						if (eBestMissileCarrierUnit != NO_UNIT)
						{
							FAssert(GC.getUnitInfo(eBestMissileCarrierUnit).getDomainCargo() == DOMAIN_AIR);
							
							int iMissileCarrierAirNeeded = iMissileCarriers * GC.getUnitInfo(eBestMissileCarrierUnit).getCargoSpace();
							
							if ((kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_AIR) < iMissileCarrierAirNeeded) || 
								(bPrimaryArea && (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_MISSILE_CARRIER_SEA) * GC.getUnitInfo(eBestMissileCarrierUnit).getCargoSpace() < kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_MISSILE_AIR))))
							{
								// Don't always build missiles, more likely if really low
								if (AI_chooseUnit("need missiles", UNITAI_MISSILE_AIR, (kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_AIR) < iMissileCarrierAirNeeded/2) ? 50 : 20))
								{
									return;
								}
							}
						}
					}
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	// Check for whether to produce planes to fill carriers
	if ( !bInhibitUnits && (bLandWar || bAssault) && iUnitCostPercentage < (iMaxUnitSpending))
	{			
		if (iCarriers > 0 && !bImportantCity)
		{
			UnitTypes eBestCarrierUnit = NO_UNIT;  
			kPlayer.AI_bestCityUnitAIValue(UNITAI_CARRIER_SEA, NULL, &eBestCarrierUnit);
			if (eBestCarrierUnit != NO_UNIT)
			{
				FAssert(GC.getUnitInfo(eBestCarrierUnit).getDomainCargo() == DOMAIN_AIR);
				
				int iCarrierAirNeeded = iCarriers * GC.getUnitInfo(eBestCarrierUnit).getCargoSpace();

				// Reduce chances if city gives no air experience
				if (kPlayer.AI_totalUnitAIs(UNITAI_CARRIER_AIR) < iCarrierAirNeeded)
				{
					if (AI_chooseUnit("need planes for carriers", UNITAI_CARRIER_AIR, (iFreeAirExperience > 0) ? -1 : 35))
					{
						return;
					}
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	//Afforess reduced 12 -> 6, since AI rarely reaches this logic, Added exemption for MAD players
	int chance = !GC.getGameINLINE().isOption(GAMEOPTION_RUTHLESS_AI) ? 6 : 3;
	if (!bInhibitUnits && (kPlayer.AI_isDoStrategy(AI_STRATEGY_OWABWNW) || kPlayer.isEnabledMAD() || (GC.getGameINLINE().getSorenRandNum(chance, "AI consider Nuke") == 0)))
	{
		if( !bFinancialTrouble )
		{
			int iTotalNukes = kPlayer.AI_totalUnitAIs(UNITAI_ICBM);
			int iNukesWanted = 1 + 2 * std::min(kPlayer.getNumCities(), GC.getGame().getNumCities() - kPlayer.getNumCities());
			//Afforess rolling randoms twice makes getting nukes much harder
			if ((iTotalNukes < iNukesWanted)/* && (GC.getGameINLINE().getSorenRandNum(100, "AI train nuke MWAHAHAH") < (90 - (80 * iTotalNukes) / iNukesWanted))*/)
			{
				//Reordered, because nukes are more valuable than carriers
				if (AI_chooseUnit("ICBM (2)", UNITAI_ICBM))
				{
					return;
				}

				if ((pWaterArea != NULL))
				{
					if (AI_chooseUnit("need missile carrier", UNITAI_MISSILE_CARRIER_SEA, 50))
					{
						return;	
					}
				}
			}
		}
	}   

	m_iTempBuildPriority--;

	// Assault case now completely handled above
	if (/*!bUnitExempt && */!bInhibitUnits && !bAssault && (!bImportantCity || bDefenseWar) && (iUnitCostPercentage < iMaxUnitSpending))
	{
		if (!bFinancialTrouble && (bLandWar || (kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER) && !bGetBetterUnits)))
		{
			int iTrainInvaderChance = iBuildUnitProb + 10;

			if (bAggressiveAI)
			{
				iTrainInvaderChance += 15;
			}

			if( bGetBetterUnits )
			{
				iTrainInvaderChance /= 2;
			}
			else if ((pArea->getAreaAIType(getTeam()) == AREAAI_MASSING) || (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT_MASSING))
			{
				iTrainInvaderChance = (100 - ((100 - iTrainInvaderChance) / (bCrushStrategy ? 6 : 3)));
			}        	

			if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 20, 0, bDefenseWar ? 10 : 30))
			{
				return;
			}

			UnitTypeWeightArray invaderTypes;
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 100));
			invaderTypes.push_back(std::make_pair(UNITAI_COUNTER, 50));
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK, 40));
			invaderTypes.push_back(std::make_pair(UNITAI_PARADROP, (kPlayer.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ) ? 30 : 20) / (bAssault ? 2 : 1)));

			if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_PILLAGE) <= ((iNumCitiesInArea + 1) / 2))
			{
				invaderTypes.push_back(std::make_pair(UNITAI_PILLAGE, 30));
			}

			if (AI_chooseLeastRepresentedUnit("invader units (2)", invaderTypes, iTrainInvaderChance))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	if (!bInhibitUnits && (pWaterArea != NULL) && !bDefenseWar && !bAssault)
	{
		if( !bFinancialTrouble )
		{
			// Force civs with foreign colonies to build a few assault transports to defend the colonies
			if( kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) < (kPlayer.getNumCities() - iNumCapitalAreaCities)/3 )
			{
				if (AI_chooseUnit("colony defense assault ships", UNITAI_ASSAULT_SEA))
				{
					return;
				}
			}

			if (kPlayer.AI_calculateUnitAIViability(UNITAI_SETTLER_SEA, DOMAIN_SEA) < 61)
			{
				// Force civs to build escorts for settler_sea units
				if( kPlayer.AI_totalUnitAIs(UNITAI_SETTLER_SEA) > kPlayer.AI_getNumAIUnits(UNITAI_RESERVE_SEA) )
				{
					if (AI_chooseUnit("sea settler escorts", UNITAI_RESERVE_SEA))
					{
						return;
					}
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	//Arr.  Don't build pirates in financial trouble, as they'll be disbanded with high probability
	if (!bInhibitUnits && (pWaterArea != NULL) && !bLandWar && !bAssault && !bFinancialTrouble/* && !bUnitExempt*/) //k-mod
	{
		int iPirateCount = kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_PIRATE_SEA);
		int iNeededPirates = (1 + (pWaterArea->getNumTiles() / std::max(1, 200 - iBuildUnitProb)));
		iNeededPirates *= (20 + iWaterPercent);
		iNeededPirates /= 100;
		
		if (kPlayer.isNoForeignTrade())
		{
			iNeededPirates *= 3;
			iNeededPirates /= 2;
		}
		if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_PIRATE_SEA) < iNeededPirates)
		{
			if (kPlayer.AI_calculateUnitAIViability(UNITAI_PIRATE_SEA, DOMAIN_SEA) > 49)
			{
				if (AI_chooseUnit("pirates", UNITAI_PIRATE_SEA, iWaterPercent / (1 + iPirateCount)))
				{
					return;
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (!bInhibitUnits && !bLandWar && !bFinancialTrouble)
	{
		if ((pWaterArea != NULL) && (iWaterPercent > 40))
		{
			if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_SPY) > 0)
			{
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SPY_SEA) == 0)
				{
					if (AI_chooseUnit("sea spy", UNITAI_SPY_SEA))
					{
						return;
					}
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (!bInhibitUnits && iBestSpreadUnitValue > ((iSpreadUnitThreshold * 40) / 100))
	{
		if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
		{
			return;
		}
	}
		
	m_iTempBuildPriority--;

	if (!bInhibitUnits && (iTotalFloatingDefenders < iNeededFloatingDefenders && (!bFinancialTrouble || bLandWar)))
	{
		if (AI_chooseLeastRepresentedUnit("floating defenders", floatingDefenderTypes, 50))
		{
			return;
		}
	}
	
	m_iTempBuildPriority--;

	//	Koshling - protect against broken AI counts, which is known to happen occasionally
	int iNumSpies = std::max(0,kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_SPY));
	int iNeededSpies = iNumCitiesInArea / 3;
	iNeededSpies += isCapital() ? 1 : 0;
	// K-Mod
	if (kPlayer.AI_isDoStrategy(AI_STRATEGY_BIG_ESPIONAGE))
	{
		iNeededSpies *= 2;
	}

	if (!bInhibitUnits && iNumSpies < iNeededSpies)
	{
		//if (AI_chooseUnit("spy", UNITAI_SPY, 5 + 50 / (1 + iNumSpies)))
		if (AI_chooseUnit("spy", UNITAI_SPY, 30 * iNeededSpies / (3 * iNumSpies + iNeededSpies)))
		{
			return;
		}
	}
	
	if (!bInhibitUnits && bLandWar && !bDanger)
	{
		if (iNumSettlers < iMaxSettlers)
		{
			if (!bFinancialTrouble)
			{
				if (iAreaBestFoundValue > iMinFoundValue)
				{
					if (AI_chooseUnit("optional settlers", UNITAI_SETTLE))
					{
						return;
					}
				}
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (!isHuman() &&
		(iProductionRank <= ((kPlayer.getNumCities() > 8) ? 3 : 2))
		&& (getPopulation() > 3))
	{
/************************************************************************************************/
/* Afforess	                  Start		 06/30/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
		int iWonderRand = 8 + getCitySorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand");
*/
		int iWonderRand = 8 + GC.getGameINLINE().getSorenRandNum(GET_PLAYER(getOwnerINLINE()).getWonderConstructRand(), "Wonder Construction Rand");
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		
		// increase chance of going for an early wonder
		if (GC.getGameINLINE().getElapsedGameTurns() < (100 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent() / 100) && iNumCitiesInArea > 1)
		{
			iWonderRand *= 35;
			iWonderRand /= 100;
		}
		else if (iNumCitiesInArea >= 3)
		{
			iWonderRand *= 30;
			iWonderRand /= 100;
		}
		else
		{
			iWonderRand *= 25;
			iWonderRand /= 100;
		}
		
		if (bAggressiveAI)
		{
			iWonderRand *= 2;
			iWonderRand /= 3;
		}
		
		int iWonderRoll = GC.getGameINLINE().getSorenRandNum(100, "Wonder Build Rand");
		
		if (iProductionRank == 1)
		{
			iWonderRoll /= 2;
		}
		
		if (iWonderRoll < iWonderRand)
		{
			int iWonderMaxTurns = 20 + ((iWonderRand - iWonderRoll) * 2);
			if (bLandWar)
			{
				iWonderMaxTurns /= 2;
			}
			
			if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, iWonderMaxTurns))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses oppurtunistic wonder build 3", getName().GetCString());
				return;
			}
		}
	}
	
	m_iTempBuildPriority--;

	if (!bInhibitUnits && iUnitCostPercentage < iMaxUnitSpending + 4 && !bFinancialTrouble)
	{
		if ((iAircraftHave * 2 >= iAircraftNeed) && (iAircraftHave < iAircraftNeed))
		{
			int iOdds = 33;

			if( iFreeAirExperience > 0 || (iProductionRank <= (1 + kPlayer.getNumCities()/2)) )
			{
				iOdds = -1;
			}

			if (AI_chooseLeastRepresentedUnit("air units", airUnitTypes, iOdds))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	if (!bLandWar)
	{		
		if ((iCulturePressure > 90) || kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
		{
			if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 20))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses cultural pressure/cultural victory 3", getName().GetCString());
					return;
				}
			}
		}

		if (!bInhibitUnits && pWaterArea != NULL && bFinancialTrouble)
		{
			if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_MISSIONARY) > 0)
			{
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_MISSIONARY_SEA) == 0)
				{
					if (AI_chooseUnit("sea missionary", UNITAI_MISSIONARY_SEA))
					{
						return;
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
	{
		if (getCommerceRateTimes100(COMMERCE_CULTURE) == 0)
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
			{
				return;
			}
		}
	}


	//20 means 5g or ~2 happiness...
	if (AI_chooseBuilding(iEconomyFlags, 15, (20*iEcononmyFlagsThreasholdWeighting)/100))
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose iEconomyFlags 2", getName().GetCString());
		return;
	}

	m_iTempBuildPriority--;

	if (!bLandWar)
	{
		if (AI_chooseBuilding(iEconomyFlags, 40, (8*iEcononmyFlagsThreasholdWeighting)/100))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose iEconomyFlags 3", getName().GetCString());
			return;
		}

		if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
		{
			if (iCulturePressure > 50)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 60))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose cultural pressure 2", getName().GetCString());
					return;
				}
			}
		}

		if (!bInhibitUnits && pWaterArea != NULL)
		{
			if (bPrimaryArea)
			{
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) < std::min(1, kPlayer.AI_neededExplorers(pWaterArea)))
				{
					if (AI_chooseUnit("explore sea", UNITAI_EXPLORE_SEA))
					{
						return;
					}
				}
			}
		}

		if (!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			if (getBaseYieldRate(YIELD_PRODUCTION) >= 8)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, 80))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_PRODUCTION 3", getName().GetCString());
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//	If there are no city counter units in the vicinity best create 1
	if (!bInhibitUnits && plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, getOwnerINLINE(), NO_TEAM, NULL, -1, -1, 2) == 0)
	{
		if (AI_chooseUnit("city counter units needed locally", UNITAI_CITY_COUNTER))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//	If there are no pillage counter units in the vicinity best create 1
	if (!bInhibitUnits && plot()->plotCount(PUF_isUnitAIType, UNITAI_PILLAGE_COUNTER, -1, NULL, getOwnerINLINE(), NO_TEAM, NULL, -1, -1, 2) == 0)
	{
		if (AI_chooseUnit("pillage counter units needed locally", UNITAI_PILLAGE_COUNTER))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	// we do a similar check lower, in the landwar case
	if (!bLandWar && bFinancialTrouble)
	{
		if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_GOLD))
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_GOLD))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose financial trouble gold", getName().GetCString());
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//	low priority property control (bad property > 5% through op range - choose unit to deal if possible)
	if ( AI_choosePropertyControlUnit( 5 ) )
	{
		return;
	}
	
	m_iTempBuildPriority--;

	if (!bInhibitUnits)
	{
		if ( AI_chooseHealerUnit() )
		{
			return;
		}
	}
	
	m_iTempBuildPriority--;

	if (!bInhibitUnits)
	{
		//harass first
		int iInfiltratorCount = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_INFILTRATOR);
		if (iInfiltratorCount < 1)
		{
			//this is enough of a trigger.  Once introduced they will expand themselves if needed and possible.
			if (AI_chooseUnit("Infiltrator needed anywhere", UNITAI_INFILTRATOR) )
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	bool bChooseUnit = false;
	if (!bInhibitUnits && iUnitCostPercentage < iMaxUnitSpending + 5)
	{
		if ((bLandWar) ||
			  ((kPlayer.getNumCities() <= 3) && (GC.getGameINLINE().getElapsedGameTurns() < 60)) ||
			  (GC.getGameINLINE().getSorenRandNum(100, "AI Build Unit Production") < AI_buildUnitProb()) ||
				(isHuman() && (getGameTurnFounded() == GC.getGameINLINE().getGameTurn())))
		{
			if (AI_chooseUnit("optional units"))
			{
				return;
			}

			bChooseUnit = true;
		}
	}

	m_iTempBuildPriority--;

	// BBAI TODO: Temporary for testing
	//if( getOwnerINLINE()%2 == 1 )
	//{
		// Only cities with reasonable production
		if (!isHuman() && (iProductionRank <= ((kPlayer.getNumCities() > 8) ? 3 : 2))
		&& (getPopulation() > 3))
		{
			if (AI_chooseProject())
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose project 2", getName().GetCString());
				return;
			}
		}
	//}

	m_iTempBuildPriority--;

	if (AI_chooseBuilding())
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose building by probability", getName().GetCString());
		return;
	}
	
	m_iTempBuildPriority--;

	if (!bChooseUnit && !bFinancialTrouble && kPlayer.AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
	{
		if (AI_chooseUnit("final war units"))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//	Koshling.  AI shouldn't choose gold as often as it does.  If we have plenty of
	//	gold prefer research most of the time

	//	Set up weights 0-100 for each commerce type to weight the choice (gold weigth can actuially go higher than
	//	100, but only if we already have less gold than our target in which case we should already have unconditonally
	//	gone for gold earlier in this choice)
	int commerceWeights[NUM_COMMERCE_TYPES];
	commerceWeights[COMMERCE_GOLD] = std::max(0,100-(GET_PLAYER(getOwnerINLINE()).getEffectiveGold() - GET_PLAYER(getOwnerINLINE()).AI_goldTarget())*100/std::max(1,GET_PLAYER(getOwnerINLINE()).AI_goldTarget()));
	if (isHuman() && !AI_isEmphasizeCommerce(COMMERCE_GOLD))
	{
		commerceWeights[COMMERCE_GOLD] = 0;
	}

	commerceWeights[COMMERCE_RESEARCH] = 50;	//	Can't think of a good measure for research pressure so just set to a middling value
	if (isHuman() && !AI_isEmphasizeCommerce(COMMERCE_RESEARCH))
	{
		commerceWeights[COMMERCE_RESEARCH] = 0;
	}
	commerceWeights[COMMERCE_CULTURE] = iCulturePressure;
	if (isHuman() && !AI_isEmphasizeCommerce(COMMERCE_CULTURE))
	{
		commerceWeights[COMMERCE_CULTURE] = 0;
	}
	commerceWeights[COMMERCE_ESPIONAGE] = 10;	//	Is this ever really worthwhile?

	if (!AI_chooseProcess(NO_COMMERCE, commerceWeights))
	{
		if (!AI_finalProcessSelection())
		{
			FAssert(false);
		}
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

UnitTypes CvCityAI::AI_bestUnit(int& iBestUnitValue, int iNumSelectableTypes, UnitAITypes* pSelectableTypes, bool bAsync, UnitAITypes* peBestUnitAI, bool bNoRand, bool bNoWeighting, CvUnitSelectionCriteria* criteria)
{
	iBestUnitValue = 0;

	if (peBestUnitAI != NULL)
	{
		*peBestUnitAI = NO_UNITAI;
	}

	int aiUnitAIVal[NUM_UNITAI_TYPES];
	int iMilitaryWeight = 0;
	if ( !bNoWeighting )
	{
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      11/30/08                                jdog5000      */
	/*                                                                                              */
	/* City AI                                                                                      */
	/************************************************************************************************/
	/* original bts code
		pWaterArea = waterArea();
	*/
		CvArea* pWaterArea = waterArea(true);
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/

		bool bWarPlan = (GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0);
		bool bDefense = (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
		bool bLandWar = (bDefense || (area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
		bool bAssault = (area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT);
		bool bPrimaryArea = GET_PLAYER(getOwnerINLINE()).AI_isPrimaryArea(area());
		bool bAreaAlone = GET_PLAYER(getOwnerINLINE()).AI_isAreaAlone(area());
		bool bFinancialTrouble = GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble();
		bool bWarPossible = GET_TEAM(getTeam()).AI_isWarPossible();
		bool bDanger = AI_isDanger();

		int iHasMetCount = GET_TEAM(getTeam()).getHasMetCivCount(true);
		iMilitaryWeight = GET_PLAYER(getOwnerINLINE()).AI_militaryWeight(area());
		int iNumCitiesInArea = area()->getCitiesPerPlayer(getOwnerINLINE());

		int iCoastalCities = 0;
		if (pWaterArea != NULL)
		{
			iCoastalCities = GET_PLAYER(getOwnerINLINE()).countNumCoastalCitiesByArea(pWaterArea);
		}

		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			aiUnitAIVal[iI] = 0;
		}

		if (!bFinancialTrouble && ((bPrimaryArea) ? (GET_PLAYER(getOwnerINLINE()).findBestFoundValue() > 0) : (area()->getBestFoundValue(getOwnerINLINE()) > 0)))
		{
			aiUnitAIVal[UNITAI_SETTLE]++;
		}

		aiUnitAIVal[UNITAI_WORKER] += GET_PLAYER(getOwnerINLINE()).AI_neededWorkers(area());

		aiUnitAIVal[UNITAI_ATTACK] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 7 : 12)) + ((bPrimaryArea && bWarPossible) ? 2 : 0) + 1);

		aiUnitAIVal[UNITAI_CITY_DEFENSE] += (iNumCitiesInArea + 1);
		aiUnitAIVal[UNITAI_CITY_COUNTER] += ((5 * (iNumCitiesInArea + 1)) / 8);
		aiUnitAIVal[UNITAI_CITY_SPECIAL] += ((iNumCitiesInArea + 1) / 2);

		if (bWarPossible)
		{
			aiUnitAIVal[UNITAI_ATTACK_CITY] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 10 : 17)) + ((bPrimaryArea) ? 1 : 0));
			aiUnitAIVal[UNITAI_COUNTER] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 13 : 22)) + ((bPrimaryArea) ? 1 : 0));
			aiUnitAIVal[UNITAI_PARADROP] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 5 : 8)) + ((bPrimaryArea) ? 1 : 0));

			aiUnitAIVal[UNITAI_DEFENSE_AIR] += (GET_PLAYER(getOwnerINLINE()).getNumCities() + 1);
			aiUnitAIVal[UNITAI_CARRIER_AIR] += GET_PLAYER(getOwnerINLINE()).AI_countCargoSpace(UNITAI_CARRIER_SEA);
			aiUnitAIVal[UNITAI_MISSILE_AIR] += GET_PLAYER(getOwnerINLINE()).AI_countCargoSpace(UNITAI_MISSILE_CARRIER_SEA);

			if (bPrimaryArea)
			{
				aiUnitAIVal[UNITAI_ICBM] += std::max((GET_PLAYER(getOwnerINLINE()).getTotalPopulation() / 25), ((GC.getGameINLINE().countCivPlayersAlive() + GC.getGameINLINE().countTotalNukeUnits()) / (GC.getGameINLINE().countCivPlayersAlive() + 1)));
			}
		}

		if (isHominid())
		{
			aiUnitAIVal[UNITAI_ATTACK] *= 2;
		}
		else
		{
			if (!bLandWar)
			{
				aiUnitAIVal[UNITAI_EXPLORE] += GET_PLAYER(getOwnerINLINE()).AI_neededExplorers(area());
			}

			if (pWaterArea != NULL)
			{
				aiUnitAIVal[UNITAI_WORKER_SEA] += AI_neededSeaWorkers();

				if ((GET_PLAYER(getOwnerINLINE()).getNumCities() > 3) || (area()->getNumUnownedTiles() < 10))
				{
					if (bPrimaryArea)
					{
						aiUnitAIVal[UNITAI_EXPLORE_SEA] += GET_PLAYER(getOwnerINLINE()).AI_neededExplorers(pWaterArea);
					}

					if (bPrimaryArea && (GET_PLAYER(getOwnerINLINE()).findBestFoundValue() > 0) && (pWaterArea->getNumTiles() > 300))
					{
						aiUnitAIVal[UNITAI_SETTLER_SEA]++;
					}

					if (bPrimaryArea && (GET_PLAYER(getOwnerINLINE()).AI_totalAreaUnitAIs(area(), UNITAI_MISSIONARY) > 0) && (pWaterArea->getNumTiles() > 400))
					{
						aiUnitAIVal[UNITAI_MISSIONARY_SEA]++;
					}

					if (bPrimaryArea && (GET_PLAYER(getOwnerINLINE()).AI_totalAreaUnitAIs(area(), UNITAI_SPY) > 0) && (pWaterArea->getNumTiles() > 500))
					{
						aiUnitAIVal[UNITAI_SPY_SEA]++;
					}

					aiUnitAIVal[UNITAI_PIRATE_SEA] += pWaterArea->getNumTiles() / 600;

					if (bWarPossible)
					{
						aiUnitAIVal[UNITAI_ATTACK_SEA] += std::min((pWaterArea->getNumTiles() / 150), ((((iCoastalCities * 2) + (iMilitaryWeight / 9)) / ((bAssault) ? 4 : 6)) + ((bPrimaryArea) ? 1 : 0)));
						aiUnitAIVal[UNITAI_RESERVE_SEA] += std::min((pWaterArea->getNumTiles() / 200), ((((iCoastalCities * 2) + (iMilitaryWeight / 7)) / 5) + ((bPrimaryArea) ? 1 : 0)));
						aiUnitAIVal[UNITAI_ESCORT_SEA] += (GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_ASSAULT_SEA) + (GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_CARRIER_SEA) * 2));
						aiUnitAIVal[UNITAI_ASSAULT_SEA] += std::min((pWaterArea->getNumTiles() / 250), ((((iCoastalCities * 2) + (iMilitaryWeight / 6)) / ((bAssault) ? 5 : 8)) + ((bPrimaryArea) ? 1 : 0)));
						aiUnitAIVal[UNITAI_CARRIER_SEA] += std::min((pWaterArea->getNumTiles() / 350), ((((iCoastalCities * 2) + (iMilitaryWeight / 8)) / 7) + ((bPrimaryArea) ? 1 : 0)));
						aiUnitAIVal[UNITAI_MISSILE_CARRIER_SEA] += std::min((pWaterArea->getNumTiles() / 350), ((((iCoastalCities * 2) + (iMilitaryWeight / 8)) / 7) + ((bPrimaryArea) ? 1 : 0)));
					}
				}
			}

			if ((iHasMetCount > 0) && bWarPossible)
			{
				if (bLandWar || bAssault || !bFinancialTrouble || (GET_PLAYER(getOwnerINLINE()).calculateUnitCost() == 0))
				{
					aiUnitAIVal[UNITAI_ATTACK] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 9 : 16)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
					aiUnitAIVal[UNITAI_ATTACK_CITY] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 7 : 15)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
					aiUnitAIVal[UNITAI_COLLATERAL] += ((iMilitaryWeight / ((bDefense) ? 8 : 14)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
					aiUnitAIVal[UNITAI_PILLAGE] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 10 : 19)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
					aiUnitAIVal[UNITAI_RESERVE] += ((iMilitaryWeight / ((bLandWar) ? 12 : 17)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
					aiUnitAIVal[UNITAI_COUNTER] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 9 : 16)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
					aiUnitAIVal[UNITAI_PARADROP] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 4 : 8)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));

					aiUnitAIVal[UNITAI_ATTACK_AIR] += (GET_PLAYER(getOwnerINLINE()).getNumCities() + 1);

					if (pWaterArea != NULL)
					{
						if ((GET_PLAYER(getOwnerINLINE()).getNumCities() > 3) || (area()->getNumUnownedTiles() < 10))
						{
							aiUnitAIVal[UNITAI_ATTACK_SEA] += std::min((pWaterArea->getNumTiles() / 100), ((((iCoastalCities * 2) + (iMilitaryWeight / 10)) / ((bAssault) ? 5 : 7)) + ((bPrimaryArea) ? 1 : 0)));
							aiUnitAIVal[UNITAI_RESERVE_SEA] += std::min((pWaterArea->getNumTiles() / 150), ((((iCoastalCities * 2) + (iMilitaryWeight / 11)) / 8) + ((bPrimaryArea) ? 1 : 0)));
						}
					}
				}
			}
		}

		// XXX this should account for air and heli units too...
		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			if (GET_PLAYER(getOwnerINLINE()).AI_unitAIDomainType((UnitAITypes)iI) == DOMAIN_SEA)
			{
				if (pWaterArea != NULL)
				{
					aiUnitAIVal[iI] -= GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterArea, ((UnitAITypes)iI));
				}
			}
			else if ((GET_PLAYER(getOwnerINLINE()).AI_unitAIDomainType((UnitAITypes)iI) == DOMAIN_AIR) || (iI == UNITAI_ICBM))
			{
				aiUnitAIVal[iI] -= GET_PLAYER(getOwnerINLINE()).AI_totalUnitAIs((UnitAITypes)iI);
			}
			else
			{
				aiUnitAIVal[iI] -= GET_PLAYER(getOwnerINLINE()).AI_totalAreaUnitAIs(area(), ((UnitAITypes)iI));
			}
		}

		aiUnitAIVal[UNITAI_SETTLE] *= ((bDanger) ? 8 : 20);
		aiUnitAIVal[UNITAI_WORKER] *= ((bDanger) ? 2 : 7);
		aiUnitAIVal[UNITAI_ATTACK] *= 3;
		aiUnitAIVal[UNITAI_ATTACK_CITY] *= 4;
		aiUnitAIVal[UNITAI_COLLATERAL] *= 5;
		aiUnitAIVal[UNITAI_PILLAGE] *= 3;
		aiUnitAIVal[UNITAI_RESERVE] *= 3;
		aiUnitAIVal[UNITAI_COUNTER] *= 3;
		aiUnitAIVal[UNITAI_CITY_DEFENSE] *= 2;
		aiUnitAIVal[UNITAI_CITY_COUNTER] *= 2;
		aiUnitAIVal[UNITAI_CITY_SPECIAL] *= 2;
		aiUnitAIVal[UNITAI_EXPLORE] *= ((bDanger) ? 6 : 15);
		aiUnitAIVal[UNITAI_ICBM] *= 18;
		aiUnitAIVal[UNITAI_WORKER_SEA] *= ((bDanger) ? 3 : 10);
		aiUnitAIVal[UNITAI_ATTACK_SEA] *= 5;
		aiUnitAIVal[UNITAI_RESERVE_SEA] *= 4;
		aiUnitAIVal[UNITAI_ESCORT_SEA] *= 20;
		aiUnitAIVal[UNITAI_EXPLORE_SEA] *= 18;
		aiUnitAIVal[UNITAI_ASSAULT_SEA] *= 14;
		aiUnitAIVal[UNITAI_SETTLER_SEA] *= 16;
		aiUnitAIVal[UNITAI_MISSIONARY_SEA] *= 12;
		aiUnitAIVal[UNITAI_SPY_SEA] *= 10;
		aiUnitAIVal[UNITAI_CARRIER_SEA] *= 8;
		aiUnitAIVal[UNITAI_MISSILE_CARRIER_SEA] *= 8;
		aiUnitAIVal[UNITAI_PIRATE_SEA] *= 5;
		aiUnitAIVal[UNITAI_ATTACK_AIR] *= 6;
		aiUnitAIVal[UNITAI_DEFENSE_AIR] *= 3;
		aiUnitAIVal[UNITAI_CARRIER_AIR] *= 15;
		aiUnitAIVal[UNITAI_MISSILE_AIR] *= 15;
		aiUnitAIVal[UNITAI_PROPERTY_CONTROL] *= 4;
		aiUnitAIVal[UNITAI_PROPERTY_CONTROL_SEA] *= 3;
		aiUnitAIVal[UNITAI_HEALER] *= 4;
		aiUnitAIVal[UNITAI_HEALER_SEA] *= 4;

		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			aiUnitAIVal[iI] *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getUnitAIWeightModifier(iI) + 100));
			aiUnitAIVal[iI] /= 100;
		}
	}

	int iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;

	for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		bool bValid = false;

		if ( iNumSelectableTypes != -1 )
		{
			for (int iJ = 0; iJ < iNumSelectableTypes; iJ++)
			{
				if ( pSelectableTypes[iJ] == iI )
				{
					bValid = true;
					break;
				}
			}
		}
		else
		{
			bValid = true;
		}

		if (bValid && (bNoWeighting || aiUnitAIVal[iI] > 0))
		{
			if ( !bNoWeighting )
			{
				if ( !bNoRand )
				{
					if (bAsync)
					{
						aiUnitAIVal[iI] += GC.getASyncRand().get(iMilitaryWeight, "AI Best UnitAI ASYNC");
					}
					else
					{
						aiUnitAIVal[iI] += GC.getGameINLINE().getSorenRandNum(iMilitaryWeight, "AI Best UnitAI");
					}
				}

				if (aiUnitAIVal[iI] < iBestValue)
				{
					bValid = false;
				}
			}

			if ( bValid )
			{
				int iUnitValue;

				UnitTypes eUnit = AI_bestUnitAI(((UnitAITypes)iI), iUnitValue, bAsync, bNoRand, criteria);

				if (eUnit != NO_UNIT)
				{
					if (AI_meetsUnitSelectionCriteria(eUnit, criteria))
					{
						if (iUnitValue > iBestUnitValue)
						{
							iBestValue = aiUnitAIVal[iI];
							eBestUnit = eUnit;
							if (peBestUnitAI != NULL)
							{
								*peBestUnitAI = ((UnitAITypes)iI);
							}
							iBestUnitValue = iUnitValue;
						}
					}
				}
			}
		}
	}

	return eBestUnit;
}

/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bIgnoreNotUnitAIs
UnitTypes CvCityAI::AI_bestUnitAI(UnitAITypes eUnitAI, int& iBestValue, bool bAsync, bool bNoRand, CvUnitSelectionCriteria* criteria)
/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/
{
	PROFILE_FUNC();

	UnitTypes eLoopUnit;
	UnitTypes eBestUnit = NO_UNIT;
	int iValue;
	/*int iOriginalValue;*/
	/*int iBestOriginalValue;*/
	int iI, iJ, iK;
	CvUnitSelectionCriteria tempCriteria;
	
	FAssert (eUnitAI != NO_UNITAI);

	iBestValue = 0;
	//iBestOriginalValue = 0;
	
	if ( criteria != NULL )
	{
		tempCriteria = *criteria;
	}

	if ( tempCriteria.m_eUnitAI == NO_UNITAI )
	{
		tempCriteria.m_eUnitAI = eUnitAI;
	}
	else
	{
		FAssert(tempCriteria.m_eUnitAI == eUnitAI);
	}

	bool bGrowMore = false;
	if (!tempCriteria.m_bIgnoreGrowth && foodDifference(true, true) > 0)
	{
		// BBAI NOTE: This is where small city worker and settler production is blocked
		// TB Interpret Note: bGrowMore must be FALSE for cities to queue settlers!
		// This was basically saying that if the city has more population than improved tiles then it won't build settlers!
		if (GET_PLAYER(getOwnerINLINE()).getNumCities() <= 2 || isHominid())
		{
			/* original bts code
			bGrowMore = ((getPopulation() < 3) && (AI_countGoodTiles(true, false, 100) >= getPopulation())); */
			// K-Mod. We need to allow the starting city to build a worker at size 1.
			bGrowMore = (eUnitAI != UNITAI_WORKER || GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_WORKER) > 0)
				&& getPopulation() < 3 && AI_countGoodTiles(true, false, 100) >= getPopulation();
			// K-Mod end
		}
		else
		{
			bGrowMore = ((getPopulation() < 3) || (AI_countGoodTiles(true, false, 100) >= getPopulation()));
		}

		if (!bGrowMore && (getPopulation() < 6) && (AI_countGoodTiles(true, false, 80) >= getPopulation()))
		{
			if ((getFood() - (getFoodKept() / 2)) >= (growthThreshold() / 2))
			{
				if ((angryPopulation(1) == 0) && (healthRate(false, 1) == 0))
				{
					bGrowMore = true;
				}
			}
		}
		//K-Mod
		else if (bGrowMore)
		{
			if (angryPopulation(1) > 0)
				bGrowMore = false;
		}
		// K-Mod end
	}
	
	//	Check the cache
	bool bHasCachedValue = false;
	int cacheKey = tempCriteria.getHash();
	std::map<int,UnitValueInfo>::const_iterator itr = m_bestUnits.find(cacheKey);

	if ( itr != m_bestUnits.end() )
	{
		//	Have to recheck canTrain as we may hit a unit limit mid-turn
		if ( (itr->second.eUnit == NO_UNIT || !bGrowMore || !isFoodProduction(itr->second.eUnit)) && AI_meetsUnitSelectionCriteria(itr->second.eUnit, &tempCriteria) && canTrain(itr->second.eUnit))
		{
			iBestValue = itr->second.iValue;
			eBestUnit = itr->second.eUnit;
			bHasCachedValue = true;
		}
	}

	if ( bHasCachedValue )
	{
		if ( UNITAI_CITY_DEFENSE == eUnitAI && eBestUnit == NO_UNIT && (criteria == NULL || criteria->m_eProperty == NO_PROPERTY) )
		{
			OutputDebugString("No buildable defender!!\n");
		}
		return eBestUnit;
	}

	CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eLoopUnit = ((UnitTypes)(kCivilizationInfo.getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			if (tempCriteria.m_eIgnoreAdvisor == NO_ADVISOR || (GC.getUnitInfo(eLoopUnit).getAdvisorType() != tempCriteria.m_eIgnoreAdvisor))
			{
				//	Koshling - this causes crashes with new code that asumes you will have some unit capable
				//	of UNITAI_CITY_DEFENDER becasuse early on you have nothing with this as default.  Since I cannot
				//	see why the human player wouldn't want an alternate that CAN do the requested AI, any less than
				//	the AI does I have decided to just remove this check for now
				//if (!isHuman() || (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == eUnitAI))
				{
				    
					if (!(bGrowMore && isFoodProduction(eLoopUnit)))
					{
						if (AI_meetsUnitSelectionCriteria(eLoopUnit, &tempCriteria) && canTrain(eLoopUnit))
						{
							iValue = GET_PLAYER(getOwnerINLINE()).AI_unitValue(eLoopUnit, eUnitAI, area(), &tempCriteria);

							FAssert((MAX_INT / 100) > iValue);
							if (iValue > (MAX_INT/100))
							{
								iValue = (MAX_INT/100);
							}

							//	Allow order fo magnitude
							iValue *= 100;	//	Need it multiplying up so that truncation errors don't render
											//	modifiers irrelevant
							
							int iPromotionValue = 0;
							if (iValue > 0)
							{
								iValue += getProductionExperience(eLoopUnit);

								//	KOSHLING - this need rework to take actual promotion values.  May need some caching to do so at
								//	appropriate performance levels.  TODO ****


								//free promotions. slow?
								//only 1 promotion per source is counted (ie protective isn't counted twice)
								//buildings
								for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
								{
									if (isFreePromotion((PromotionTypes)iJ) && !GC.getUnitInfo(eLoopUnit).getFreePromotions((PromotionTypes)iJ))
									{
										if ((GC.getUnitInfo(eLoopUnit).getUnitCombatType() != NO_UNITCOMBAT) && GC.getPromotionInfo((PromotionTypes)iJ).getUnitCombat(GC.getUnitInfo(eLoopUnit).getUnitCombatType()))
										{
											iPromotionValue += 15;

											break;
										}
									}
								}
								//TB SubCombat Mod Begin
								UnitCombatTypes eSubCombatType;
								
								for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
								{
									if (isFreePromotion((PromotionTypes)iJ) && !GC.getUnitInfo(eLoopUnit).getFreePromotions((PromotionTypes)iJ))
									{
										for (iK = 0; iK < GC.getUnitInfo(eLoopUnit).getNumSubCombatTypes(); iK++)
										{
											eSubCombatType = ((UnitCombatTypes)GC.getUnitInfo(eLoopUnit).getSubCombatType(iK));
											if (GC.getPromotionInfo((PromotionTypes)iJ).getUnitCombat(eSubCombatType))
											{											
												iPromotionValue += 15;
												break;
											}
										}
									}
								}							
								//TB SubCombat Mod End
								//special to the unit
								for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
								{
									if (GC.getUnitInfo(eLoopUnit).getFreePromotions(iJ))
									{
										iPromotionValue += 15;
										break;
									}
								}

								//traits

								if (GC.getUnitInfo(eLoopUnit).getUnitCombatType() != NO_UNITCOMBAT)
								{
									for (iJ = 0; iJ < GC.getNumTraitInfos(); iJ++)
									{
										if (hasTrait((TraitTypes)iJ))
										{
											for (iK = 0; iK < GC.getNumPromotionInfos(); iK++)
											{
												if (GC.getTraitInfo((TraitTypes) iJ).isFreePromotionUnitCombats(iK, GC.getUnitInfo(eLoopUnit).getUnitCombatType()))
												{
													iPromotionValue += 15;
												}
											}
										}
									}
								}
							}

                            iValue *= (iPromotionValue + 100);
                            iValue /= 100;

							if ( !bNoRand )
							{
								if (bAsync)
								{
									iValue *= (GC.getASyncRand().get(50, "AI Best Unit ASYNC") + 100);
									iValue /= 100;
								}
								else
								{
									iValue *= (GC.getGameINLINE().getSorenRandNum(50, "AI Best Unit") + 100);
									iValue /= 100;
								}
							}

							//int iBestHappy = 0;
							//for (int iHurry = 0; iHurry < GC.getNumHurryInfos(); ++iHurry)
							//{
							//	if (canHurryUnit((HurryTypes)iHurry, eLoopUnit, true))
							//	{
							//		int iHappy = AI_getHappyFromHurry((HurryTypes)iHurry, eLoopUnit, true);
							//		if (iHappy > iBestHappy)
							//		{
							//			iBestHappy = iHappy;
							//		}
							//	}
							//}

							//if (0 == iBestHappy)
							//{
							//	iValue += getUnitProduction(eLoopUnit);
							//}

							//iValue *= (GET_PLAYER(getOwnerINLINE()).getNumCities() * 2);
							//iValue /= std::max(1,GET_PLAYER(getOwnerINLINE()).getUnitClassCountPlusMaking((UnitClassTypes)iI) + GET_PLAYER(getOwnerINLINE()).getNumCities()+1);
			
							bool bIsSuicide = GC.getUnitInfo(eLoopUnit).isSuicide();
							
							if (bIsSuicide)
							{
								//much of this is compensated
								iValue /= 3;
							}

							//if (0 == iBestHappy)
							//{
							//	iValue /= std::max(1, (getProductionTurnsLeft(eLoopUnit, 0) + (GC.getUnitInfo(eLoopUnit).isSuicide() ? 1 : 4)));
							//}
							//else
							//{
							//	iValue *= (2 + 3 * iBestHappy);
							//	iValue /= 100;
							//}

							iValue = std::max(0, iValue);

							if (GC.getUnitInfo(eLoopUnit).getNotUnitAIType(eUnitAI))
							{ 
								iValue = 0;
							}

							if (iValue > 0 && iValue > iBestValue)
							{
								iBestValue = iValue;
								eBestUnit = eLoopUnit;
							}
						}
					}
				}
			}
		}
	}

	if ( !bGrowMore && !bAsync )
	{
		UnitValueInfo unitValueInfo;

		unitValueInfo.eUnit = eBestUnit;
		unitValueInfo.iValue = iBestValue;

		m_bestUnits.insert(std::make_pair(cacheKey, unitValueInfo));
	}
	if ( UNITAI_CITY_DEFENSE == eUnitAI && eBestUnit == NO_UNIT && (criteria == NULL || criteria->m_eProperty == NO_PROPERTY) )
	{
		OutputDebugString("No buildable defender!!\n");
	}
	if (eBestUnit == NO_UNIT)
	{
		OutputDebugString("No Buildable Unit for selected AI!!\n");
	}

	return eBestUnit;
}


BuildingTypes CvCityAI::AI_bestBuilding(int iFocusFlags, int iMaxTurns, bool bAsync, AdvisorTypes eIgnoreAdvisor, bool bMaximizeFlaggedValue)
{
	return AI_bestBuildingThreshold(iFocusFlags, iMaxTurns, /*iMinThreshold*/ 0, bAsync, eIgnoreAdvisor, bMaximizeFlaggedValue);
}

BuildingTypes CvCityAI::AI_bestBuildingThreshold(int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor, bool bMaximizeFlaggedValue, PropertyTypes eProperty)
{
	std::vector<BuildingTypes> possibles;

	CvCivilizationInfo& civ = GC.getCivilizationInfo(getCivilizationType());
	for (int idx = 0; idx < GC.getNumBuildingClassInfos(); ++idx)
	{
		if (!(GET_PLAYER(getOwnerINLINE()).isBuildingClassMaxedOut(((BuildingClassTypes)idx), GC.getBuildingClassInfo((BuildingClassTypes)idx).getExtraPlayerInstances())))
		{
			BuildingTypes building = ((BuildingTypes)(civ.getCivilizationBuildings(idx)));
			if (NO_BUILDING != building && canConstruct(building))
			{
				possibles.push_back(building);
			}
		}
	}

	std::vector<ScoredBuilding> scoredBuildings;
	if (AI_scoreBuildingsFromListThreshold(scoredBuildings, possibles, iFocusFlags, iMaxTurns, iMinThreshold, bAsync, eIgnoreAdvisor, bMaximizeFlaggedValue, eProperty))
	{
		return scoredBuildings[0].building;
	}

	return NO_BUILDING;
}

bool AI_buildingInfluencesProperty(CvCity* city, CvBuildingInfo& buildingInfo, PropertyTypes eProperty)
{
	bool bFoundValidation = false;
	CvPropertyManipulators* pMani = buildingInfo.getPropertyManipulators();
	int numSources = pMani->getNumSources();
	for (int i = 0; i < numSources; i++)
	{
		CvPropertySource* pSource = pMani->getSource(i);
		//	For now we're only interested in constant sources
		//	TODO - expand this as buildings add other types
		if (pSource->getProperty() == eProperty &&
			pSource->getType() == PROPERTYSOURCE_CONSTANT &&
			GC.getPropertyInfo(eProperty).getAIWeight() * static_cast<CvPropertySourceConstant*>(pSource)->getAmountPerTurn(city->getGameObjectConst()) > 0)
		{
			bFoundValidation = true;
			break;
		}
	}
	return bFoundValidation;
}

bool CvCityAI::AI_scoreBuildingsFromListThreshold(std::vector<ScoredBuilding>& scoredBuildings, const std::vector<BuildingTypes>& possibleBuildings, int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor, bool bMaximizeFlaggedValue, PropertyTypes eProperty)
{
	PROFILE_FUNC();

	// If we are focusing on cheap things then we just score by turns to complete only
	if (iFocusFlags & BUILDINGFOCUS_CAPITAL)
	{
		for (std::vector<BuildingTypes>::const_iterator itr = possibleBuildings.begin(); itr != possibleBuildings.end(); ++itr)
		{
			BuildingTypes building = *itr;
			FAssertMsg(building != NO_BUILDING, "AI_scoreBuildingsFromListThreshold isn't given a valid possibleBuildings list");

			if (GC.getBuildingInfo(building).isCapital())
			{
				scoredBuildings.push_back(ScoredBuilding(building, getProductionTurnsLeft(building, 0)));
			}
		}

		// Sort from least turns to most
		std::sort(scoredBuildings.begin(), scoredBuildings.end());
		return !scoredBuildings.empty();
	}

	// A couple of shortcuts to make the code more legible (probably the optimier inlines them all but better safe than sorry)
	CvCivilizationInfo& civInfo = GC.getCivilizationInfo(getCivilizationType());
	CvPlayerAI& player = GET_PLAYER(getOwnerINLINE());

	bool bAreaAlone = player.AI_isAreaAlone(area());
	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);
	
	for (std::vector<BuildingTypes>::const_iterator itr = possibleBuildings.begin(); itr != possibleBuildings.end(); ++itr)
	{
		BuildingTypes building = *itr;
		FAssertMsg(building != NO_BUILDING, "AI_scoreBuildingsFromListThreshold isn't given a valid possibleBuildings list");

		CvBuildingInfo& buildingInfo = GC.getBuildingInfo(building);
		BuildingClassTypes buildingClassType = (BuildingClassTypes)buildingInfo.getBuildingClassType();

		if (
			// We are not exceeding max buildings
			getNumBuilding(building) < GC.getCITY_MAX_NUM_BUILDINGS()
			&& 
			(
				// Building is not a wonder
				!isLimitedWonderClass(buildingClassType)
				||
				// Or we are allowed to build a wonder
				// i.e. Production isn't automated or we aren't considering a wonder (we don't want automated production producing wonders)
				(!isProductionAutomated() && (iFocusFlags == 0 || (iFocusFlags & BUILDINGFOCUS_WONDEROK) || (iFocusFlags & BUILDINGFOCUS_WORLDWONDER)))
			)
			&&
			// adviser is not one we are ignoring
			(eIgnoreAdvisor == NO_ADVISOR || buildingInfo.getAdvisorType() != eIgnoreAdvisor)
			&&
			// We can actually build the building
			canConstruct(building)
			&&
			(
				// Automated production doesn't look at buildings with prerequisites?
				!isProductionAutomated() || buildingInfo.getPrereqNumOfBuildingClass(NO_BUILDINGCLASS) <= 0
			)
		)
		{
			int iValue = 0;

			if (
				!(iFocusFlags & BUILDINGFOCUS_PROPERTY)
				||
				// If we want to build a building that influences a property (crime/pollution/tourism etc.) then 
				// invalidate the building if it doesn't influence the property we are interested in
				AI_buildingInfluencesProperty(this, buildingInfo, eProperty)
				)
			{
				iValue = AI_buildingValueThreshold(building, iFocusFlags, iMinThreshold, bMaximizeFlaggedValue);

				// If the building also gives a free building then factor that in as well
				if (buildingInfo.getFreeBuildingClass() != NO_BUILDINGCLASS)
				{
					BuildingTypes eFreeBuilding = (BuildingTypes)civInfo.getCivilizationBuildings(buildingInfo.getFreeBuildingClass());
					if (eFreeBuilding != NO_BUILDING)
					{
						// Add value of the free building taking into account our focus, and scale it by the number of cities that don't
						// yet have the building.
						iValue += (AI_buildingValue(eFreeBuilding, iFocusFlags) * (player.getNumCities() - player.getBuildingClassCountPlusMaking((BuildingClassTypes)buildingInfo.getFreeBuildingClass())));
					}
				}

				if (gCityLogLevel > 3)
				{
					logBBAI("City %S base value for %S (flags %08lx)=%d", getName().GetCString(), buildingInfo.getDescription(), iFocusFlags, iValue);
				}

				// If this new building replaces an old one, subtract the old value.
				if (iValue > 0)
				{
					PROFILE("CvCityAI::AI_bestBuildingThreshold.Replacement");

					// Look for building it replaces
					// TODO OPT: change replace building to vector instead of mask
					for (int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
					{
						BuildingTypes eJLoopBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes)iJ).getDefaultBuildingIndex()));
						if (eJLoopBuilding != NO_BUILDING 
							&&
							GC.getBuildingInfo(eJLoopBuilding).isReplaceBuildingClass(buildingClassType)
							&&
							// Only care if we actually have the building
							getNumBuilding(eJLoopBuilding) > 0
							)
						{
							PROFILE("AI_bestBuildingThreshold.Replace");

							iValue -= AI_buildingValueThreshold(eJLoopBuilding, iFocusFlags, iMinThreshold, bMaximizeFlaggedValue, true);

							if (gCityLogLevel > 3)
							{
								logBBAI("    replaces %S - reduce value to %d", GC.getBuildingInfo(eJLoopBuilding).getDescription(), iValue);
							}
						}
					}
				}
			}

			// If this building enables the construction of other buildings then we want to rate it higher!
			if (buildingInfo.EnablesOtherBuildings())
			{
				PROFILE("CvCityAI::AI_bestBuildingThreshold.EnablesOthers");

				// TODO OPT: convert the masks to vectors so this look is faster
				for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					if ((GC.getBuildingInfo((BuildingTypes)iJ).isBuildingClassNeededInCity(buildingClassType) || GC.getBuildingInfo((BuildingTypes)iJ).isPrereqOrBuildingClass(buildingClassType)) && 
						getNumBuilding((BuildingTypes)iJ) == 0 && 
						canConstructInternal((BuildingTypes)iJ, false, false, false, true, buildingClassType))
					{
						PROFILE("AI_bestBuildingThreshold.Enablement");

						// We only value the unlocked building at 1/2 rate
						iValue += AI_buildingValueThreshold((BuildingTypes)iJ, iFocusFlags, 0, false, true) / 2;

						if (gCityLogLevel > 3)
						{
							logBBAI("    enables %S - increase value to %d", GC.getBuildingInfo((BuildingTypes)iJ).getDescription(), iValue);
						}
					}
				}
			}

			// If the buildings provides a free area building as well then adjust the score up 
			if (buildingInfo.getFreeAreaBuildingClass() != NO_BUILDINGCLASS)
			{
				BuildingTypes eFreeAreaBuilding = (BuildingTypes)civInfo.getCivilizationBuildings(buildingInfo.getFreeAreaBuildingClass());
				if (eFreeAreaBuilding != NO_BUILDING)
				{
					// Score is weighted by the number of cities that don't have the free building
					int weighting = player.getNumCities() - player.getBuildingClassCountPlusMaking((BuildingClassTypes)buildingInfo.getFreeAreaBuildingClass());
					// Scaled by the ratio of cities in the area / total, giving a guess as to how important this area is in general
					weighting = (100 * weighting * area()->getCitiesPerPlayer(getOwnerINLINE())) / player.getNumCities();
					// If we have none of the free buildings at all then also increase the score weighting
					if (getNumBuilding(eFreeAreaBuilding) == 0)
					{
						weighting++;
					}
					// Finally scale by the actual value of the free building based on our focus
					iValue += (AI_buildingValue(eFreeAreaBuilding, iFocusFlags) * weighting) / 100;
				}
			}

			if (gCityLogLevel > 3)
			{
				logBBAI("    final value %d", iValue);
			}

			// If we got here, and the building value is above zero, then it certainly
			// full-filled some of our criteria and is worth evaluating further
			if (iValue > 0)
			{
				// If its a wonder and this city is rated high for production relative to our other cities, then we will bump up the score
				if (isWorldWonderClass(buildingClassType) &&
					iProductionRank <= std::min(3, ((player.getNumCities() + 2) / 3)))
				{
					int wonderScore = bAsync ?
						GC.getASyncRand().get(player.getWonderConstructRand(), "Wonder Construction Rand ASYNC")
						:
						GC.getGameINLINE().getSorenRandNum(player.getWonderConstructRand(), "Wonder Construction Rand");

					// We particularly want to build the wonder if we are alone in an area.
					// Probably because aren't as worried about producing combat units?
					if (bAreaAlone)
					{
						wonderScore *= 2;
					}
					iValue += wonderScore;
				}

				// Add a general random factor between 100% to 125%
				iValue *= bAsync ? 
					(GC.getASyncRand().get(25, "AI Best Building ASYNC") + 100) 
					:
					(GC.getGameINLINE().getSorenRandNum(25, "AI Best Building") + 100);
				iValue /= 100;

				// Add on how much this building is already constructed (could be partially constructed already)
				iValue += getBuildingProduction(building);

				// Factor in how many turns are left to complete this building
				int iTurnsLeft = getProductionTurnsLeft(building, 0);

				// Apply final checks based on how many turns to build
				if (
					(iMaxTurns <= 0)
					||
					(iTurnsLeft <= GC.getGameINLINE().AI_turnsPercent(iMaxTurns, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent()))
					||
					AI_canRushBuildingConstruction(building)
					)
				{
					FAssert((MAX_INT / 1000) > iValue);
					iValue *= 1000;
					// Adjust the score based on the turns to complete the building, more turns means lower score
					iValue /= std::max(1, iTurnsLeft + 3);
					// As we got this far we definitely consider this building a candidate so we should give it a score of at least 1
					iValue = std::max(1, iValue);
					// Add to our list of potential buildings to return later
					scoredBuildings.push_back(ScoredBuilding(building, iValue));
				}
			}
		}
	}

	// Sort from highest score to lowest. Need to reverse the order of the sort because default sort is ascending.
	std::sort(scoredBuildings.rbegin(), scoredBuildings.rend());

	return !scoredBuildings.empty();
}

bool CvCityAI::AI_canRushBuildingConstruction(BuildingTypes building)
{
	for (int iHurry = 0; iHurry < GC.getNumHurryInfos(); ++iHurry)
	{
		if (canHurryBuilding((HurryTypes)iHurry, building, true)
			&&
			AI_getHappyFromHurry((HurryTypes)iHurry, building, true) > 0)
		{
			return true;
		}
	}					
	return false;
}

int CvCityAI::AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags, bool bForTech)
{
	if ( bForTech )
	{
		PROFILE("AI_buildingValue.ForTech");

		return AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, 0, false, false, bForTech);
	}
	else
	{
		return AI_buildingValueThreshold(eBuilding, iFocusFlags, 0);
	}
}

int CvCityAI::AI_buildingValueThreshold(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bMaximizeFlaggedValue, bool bIgnoreCanConstruct)
{
	PROFILE_FUNC();

	int result = GetBuildingValue(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue, bIgnoreCanConstruct);

	return result;
}

// XXX should some of these count cities, buildings, etc. based on teams (because wonders are shared...)
// XXX in general, this function needs to be more sensitive to what makes this city unique (more likely to build airports if there already is a harbor...)
//
// KOSHLING - this routine is no longer used for most building evaluations during city production
// determination.  For efficiency reasons this now pre-calculates most of the needed values and then
// assembles the final result from cached partial results.  For this reason if you are making changes to this routine
// you must also make equivalent changes to CalculateAllBuildingValues() also in this file (it mirriors the calculations
// in a fairly obvious and direct way)
int CvCityAI::AI_buildingValueThresholdOriginal(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bMaximizeFlaggedValue, bool bIgnoreCanBuildReplacement, bool bForTech)
{
	PROFILE_FUNC();

	//	We keep a small cache of results for cases not involved in city production (that therefore still
	//	use this routine)
	CvChecksum	cacheKey;

	cacheKey.add((int)eBuilding);
	cacheKey.add(iFocusFlags);
	cacheKey.add(iThreshold);
	cacheKey.add(bMaximizeFlaggedValue);
	cacheKey.add(bIgnoreCanBuildReplacement);
	cacheKey.add(bForTech);

	std::map<int,int>::const_iterator itr = m_buildValueCache.find(cacheKey.get());
	if ( itr != m_buildValueCache.end() )
	{
		return itr->second;
	}
	else
	{
		int iResult = AI_buildingValueThresholdOriginalUncached(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue, bIgnoreCanBuildReplacement, bForTech);

		m_buildValueCache.insert(std::make_pair(cacheKey.get(), iResult));

		return iResult;
	}
}

int CvCityAI::AI_buildingValueThresholdOriginalUncached(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bMaximizeFlaggedValue, bool bIgnoreCanBuildReplacement, bool bForTech)
{
	PROFILE_FUNC();

	MEMORY_TRACK();

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	BuildingClassTypes eBuildingClass = (BuildingClassTypes) kBuilding.getBuildingClassType();
	int iLimitedWonderLimit = limitedWonderClassLimit(eBuildingClass);
	bool bIsLimitedWonder = (iLimitedWonderLimit >= 0);

	ReligionTypes eStateReligion = kOwner.getStateReligion();

	bool bAreaAlone = kOwner.AI_isAreaAlone(area());
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/24/10                        jdog5000 & Fuyu       */
/*                                                                                              */
/* City AI, Bugfix                                                                              */
/************************************************************************************************/
	int iHasMetCount = GET_TEAM(getTeam()).getHasMetCivCount(true);

	int iFoodDifference = foodDifference(false);

	// Reduce reaction to espionage induced happy/health problems
	int iHappinessLevel = happyLevel() - unhappyLevel(1) + getEspionageHappinessCounter()/2;
	int iAngryPopulation = range(-iHappinessLevel, 0, (getPopulation() + 1));
	int iHealthLevel = goodHealth() - badHealth(/*bNoAngry*/ false, std::max(0, (iHappinessLevel + 1) / 2)) + getEspionageHealthCounter()/2;
	int iBadHealth = std::max(0, -iHealthLevel);

	int iHappyModifier = (iHappinessLevel <= iHealthLevel && iHappinessLevel <= 6) ? 6 : 3;
	int iHealthModifier = (iHealthLevel < iHappinessLevel && iHealthLevel <= 4) ? 4 : 2;
	if (iHappinessLevel >= 10)
	{
		iHappyModifier = 1;
	}
	if (iHealthLevel >= 8)
	{
		iHealthModifier = 0;
	}

	int iGoldValueAssessmentModifier = GET_PLAYER(getOwnerINLINE()).AI_goldValueAssessmentModifier();

	bool bProvidesPower = (kBuilding.isPower() || ((kBuilding.getPowerBonus() != NO_BONUS) && hasBonus((BonusTypes)(kBuilding.getPowerBonus()))) || kBuilding.isAreaCleanPower());
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/********************************************************************************/
/* 	Alternative Building Evaluation				20.02.2010		Fuyu		    */
/* 	 - requires Actual Building Effects (BULL)								    */
/* 																			    */
/* 	TODO:																		*/
/* 		value riverPlotYieldChanges higher when we are already working on river	*/
/* 		add getBuildingYieldChange() ?										    */
/* 		more consistency (values from yield changes should not differ so much	*/
/* 			depending on where they come from)									*/
/********************************************************************************/
	//Don't consider a building if it causes the city to immediately start shrinking from unhealthiness
//For that purpose ignore bad health and unhappiness from Espionage.
	int iBuildingActualHappiness = getAdditionalHappinessByBuilding(eBuilding);
	int iBuildingActualHealth = getAdditionalHealthByBuilding(eBuilding);
	int iBaseHappinessLevel = happyLevel() - unhappyLevel() + getEspionageHappinessCounter();
	int iBaseHealthLevel = goodHealth() - badHealth() + getEspionageHealthCounter();

	int iPopulationExponent = getPopulation() - 1; // Each pop past the first increases consumption per population by .1, rounded down.  Each point of population means more actual people the higher the amount goes.
	int iConsumptionPerPopulationBase = iPopulationExponent + (GC.getFOOD_CONSUMPTION_PER_POPULATION() * 10);
	int iConsumptionbyPopulation = (getPopulation() * iConsumptionPerPopulationBase) / 10;

	int iBaseFoodDifference = getYieldRate(YIELD_FOOD) - iConsumptionbyPopulation - std::max(0, -iHealthLevel);

	//Temporary unhappiness that will mostly not be ignored
	int iTemporaryUnhappiness = 0;
	int iAngerPercent = 0;
	iAngerPercent += getHurryPercentAnger();
	iAngerPercent += getConscriptPercentAnger();
	iAngerPercent += getDefyResolutionPercentAnger();
	iAngerPercent += getWarWearinessPercentAnger();
	iTemporaryUnhappiness += ((iAngerPercent * (getPopulation())) / GC.getPERCENT_ANGER_DIVISOR());

	int iBadHealthFromBuilding = std::max(0,(-iBuildingActualHealth));
	int iUnhealthyPopulationFromBuilding = std::min(0,(-iBaseHealthLevel)) + iBadHealthFromBuilding;
//	int iTotalHealth = iBaseHealthLevel + iBuildingActualHealth;
	bool bShrinksWithPower = false;
//Allow a bit of shrinking: Population is expendable if angry, working a bad tile, or running a not-so-good specialist
	int iAllowedShrinkRate = (iConsumptionPerPopulationBase * (0
		+ std::max(0,-iBaseHappinessLevel-iTemporaryUnhappiness)
		+ std::min(1, std::max(0,(getWorkingPopulation() - AI_countGoodTiles(true, false, 50))))
		+ std::max(0,(visiblePopulation() - AI_countGoodSpecialists(false)))))/10;
	if (iUnhealthyPopulationFromBuilding > 0 && (iBaseFoodDifference + iAllowedShrinkRate < iUnhealthyPopulationFromBuilding ))
	{
		return 0;
	}
	else if (!(bProvidesPower || isPower()))  //if the city is still without power after building this
	{
		int iUnhealthyPopulationFromBuildingPlusPower = std::min(0,(badHealth() - goodHealth() - getEspionageHealthCounter())) + iBadHealthFromBuilding - GC.getDefineINT("DIRTY_POWER_HEALTH_CHANGE");
		if (iUnhealthyPopulationFromBuildingPlusPower > 0 && (iBaseFoodDifference + iAllowedShrinkRate < iUnhealthyPopulationFromBuildingPlusPower ))
		{
			bShrinksWithPower = true;
		}
	}
/********************************************************************************/
/* 	ABE END																	    */
/********************************************************************************/

	int iTotalPopulation = kOwner.getTotalPopulation();
	int iNumCities = kOwner.getNumCities();
	int iNumCitiesInArea = area()->getCitiesPerPlayer(getOwnerINLINE());
	
	int aiYieldRank[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYieldRank[iI] = MAX_INT;
	}

	int aiCommerceRank[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerceRank[iI] = MAX_INT;
	}

	int aiBaseCommerceRate[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiBaseCommerceRate[iI] = MAX_INT;
	}

	int aiPlayerCommerceRate[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiPlayerCommerceRate[iI] = kOwner.getTotalCityBaseCommerceRate((CommerceTypes)iI);
	}

	aiYieldRank[YIELD_PRODUCTION] = findBaseYieldRateRank(YIELD_PRODUCTION);
	bool bIsHighProductionCity = (aiYieldRank[YIELD_PRODUCTION] <= std::max(3, (iNumCities / 2)));
	
	int iCultureRank = findCommerceRateRank(COMMERCE_CULTURE);
	int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();

	bool bFinancialTrouble = GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble();

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/08/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI                                                                          */
/************************************************************************************************/
	bool bCulturalVictory1 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1);
	bool bCulturalVictory2 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	bool bCulturalVictory3 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);

	bool bSpaceVictory1 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE1);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/		
/************************************************************************************************/
/* Afforess	                  Start		 07/28/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iValue = 0;
	
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());
	
	{
		PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.MainBody");
		
		int* paiFreeSpecialistCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiFreeSpecialistYield = new int[NUM_YIELD_TYPES];
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			paiFreeSpecialistYield[iI] = 0;
		}
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			paiFreeSpecialistCommerce[iI] = 0;
		}

		int iSpecialistGreatPeopleRate = 0;
		int iSpecialistExtraHealth = 0;
		int iSpecialistExtraHappy = 0;

		if (kBuilding.getNumPopulationEmployed() > 0)
		{
			removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iSpecialistGreatPeopleRate, iSpecialistExtraHappy, iSpecialistExtraHealth, paiFreeSpecialistYield, paiFreeSpecialistCommerce);
		}
		
		int aiFreeSpecialistYield[NUM_YIELD_TYPES];
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			aiFreeSpecialistYield[iI] = paiFreeSpecialistYield[iI];
		}

		int aiFreeSpecialistCommerce[NUM_COMMERCE_TYPES];
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			aiFreeSpecialistCommerce[iI] = paiFreeSpecialistCommerce[iI];
		}
		
		SAFE_DELETE_ARRAY(paiFreeSpecialistCommerce);
		SAFE_DELETE_ARRAY(paiFreeSpecialistYield);
		
		for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
		{
			SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
			if (eNewSpecialist == NO_SPECIALIST) break;
			CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eNewSpecialist);
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				aiFreeSpecialistYield[iJ] += GET_PLAYER(getOwnerINLINE()).specialistYield(eNewSpecialist, (YieldTypes)iJ);
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				aiFreeSpecialistCommerce[iJ] += GET_PLAYER(getOwnerINLINE()).specialistCommerce(eNewSpecialist, (CommerceTypes)iJ);
			}
			iSpecialistGreatPeopleRate += kSpecialist.getGreatPeopleRateChange();
			iSpecialistExtraHealth += kSpecialist.getHealthPercent();
			iSpecialistExtraHappy += kSpecialist.getHappinessPercent();
		}
		iSpecialistExtraHappy /= 100;
		iSpecialistExtraHealth /= 100;
		iBuildingActualHappiness += iSpecialistExtraHappy;
		iBuildingActualHealth += iSpecialistExtraHealth;
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/
		bool bCanPopRush = GET_PLAYER(getOwnerINLINE()).canPopRush();

		bool bForeignTrade = false;
		int iNumTradeRoutes = getTradeRoutes();
		for (int iI = 0; iI < iNumTradeRoutes; ++iI)
		{
			PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.TradePartners");

			CvCity* pTradeCity = getTradeCity(iI);
			if (NULL != pTradeCity)
			{
				if (GET_PLAYER(pTradeCity->getOwnerINLINE()).getTeam() != getTeam() || pTradeCity->area() != area())
				{
					bForeignTrade = true;
					break;
				}
			}
		}

		if (kBuilding.isCapital())
		{
			return 0;
		}
		
	/************************************************************************************************/
	/* Afforess	                  Start		 01/24/10                                               */
	/*                                                                                              */
	/* If the AI can build the replacement building...                                              */
	/************************************************************************************************/
		if ( !bIgnoreCanBuildReplacement )
		{
			PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Replacements");

			for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				BuildingTypes eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
				if (kBuilding.isReplaceBuildingClass((BuildingClassTypes)iI))
				{
					if (eLoopBuilding != NO_BUILDING)
					{
						if(canConstruct(eLoopBuilding))
						{
							return 0;
						}
					}
				}
			}
		}
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/

		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (kBuilding.getReligionChange(iI) > 0)
			{
				if (!(GET_TEAM(getTeam()).hasHolyCity((ReligionTypes)iI)))
				{
					return 0;
				}
			}
		}

		int iPass1Value;

		for (int iPass = 0; iPass < 2; iPass++)
		{
			if ( iPass == 1 )
			{
				iPass1Value = iValue;
			}

			if ((iFocusFlags == 0) || (iValue > 0) || (iPass == 0))
			{
				iValue -= kBuilding.getInsidiousness();
				iValue += kBuilding.getInvestigation();

				if ((iFocusFlags & BUILDINGFOCUS_WORLDWONDER) || (iPass > 0))
				{
					if (isWorldWonderClass(eBuildingClass))
					{
						if (aiYieldRank[YIELD_PRODUCTION] <= 3)
						{
							iValue++;
						}
					}
				}
				
				if ((iFocusFlags & BUILDINGFOCUS_DEFENSE) || (iPass > 0))
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Defense");

					if (!bAreaAlone)
					{
						if ((GC.getGameINLINE().getBestLandUnit() == NO_UNIT) || !(GC.getUnitInfo(GC.getGameINLINE().getBestLandUnit()).isIgnoreBuildingDefense()))
						{
							iValue += (std::max(0, std::min(((kBuilding.getDefenseModifier() + getBuildingDefense()) - getNaturalDefense() - 10), kBuilding.getDefenseModifier())) / 4);

	/************************************************************************************************/
	/* Afforess	                  Start		 06/15/10                                               */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
							for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
							{
								if (hasBonus((BonusTypes)iI))
								{
									iValue += (kBuilding.getBonusDefenseChanges(iI) / 4);
								}
							}
						}
						
					
						if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ZOC))
						{
							iValue += kBuilding.isZoneOfControl() ? 50 : 0;
						}
							
						iValue += kBuilding.getLineOfSight() * 15;

						for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
						{
							if (kBuilding.getUnitCombatExtraStrength(iI) != 0)
							{
								int iValidUnitCount = 0;
								
								CvPlot* pPlot = plot();
								for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
								{
									if (unitItr->getTeam() == getTeam() && unitItr->getUnitCombatType() == iI)
									{
										iValidUnitCount++;
									}
								}
								iValue += iValidUnitCount * kBuilding.getUnitCombatExtraStrength(iI) / 6;
							}
						}
						
						bool bDefense = (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
						bool bLandWar = (bDefense || (area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
						bool bDanger = AI_isDanger();

						if (bDanger || bLandWar)
						{
							iValue += kBuilding.getAdjacentDamagePercent() * 10;
						}
						
						iValue -= kBuilding.getInvasionChance() * 5;
						
						iValue += kBuilding.isProtectedCulture() ? 50 : 0;
						iValue += kBuilding.getOccupationTimeModifier() / 20;
						
						if (kBuilding.getNoEntryDefenseLevel() > 0 && kBuilding.getNoEntryDefenseLevel() < getTotalDefense(false))
						{
							iValue += getTotalDefense(false) - kBuilding.getNoEntryDefenseLevel() / 2;
						}
						if (kBuilding.getNumUnitFullHeal() > 0)
						{
							iValue += kBuilding.getNumUnitFullHeal() * 50;
						}
								
						iValue += kBuilding.getBombardDefenseModifier() / 4;

						if (GC.getGameINLINE().isOption(GAMEOPTION_SAD))
						{
							iValue += kBuilding.getLocalDynamicDefense() / 2;
						}
						if (GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
						{
							iValue += kBuilding.getFrontSupportPercentModifier() / 4;
							iValue += kBuilding.getShortRangeSupportPercentModifier() / 4;
							iValue += kBuilding.getMediumRangeSupportPercentModifier() / 4;
							iValue += kBuilding.getLongRangeSupportPercentModifier() / 4;
							iValue += kBuilding.getFlankSupportPercentModifier() / 4;
						}

						iValue += kBuilding.getLocalCaptureProbabilityModifier() / 6;
						iValue += kBuilding.getLocalCaptureResistanceModifier() / 3;
						iValue -= kBuilding.getRiverDefensePenalty() / 2;
						iValue += kBuilding.getLocalRepel();
						iValue += kBuilding.getMinDefense();
						iValue += kBuilding.getBuildingDefenseRecoverySpeedModifier() / 20;
						iValue += kBuilding.getCityDefenseRecoverySpeedModifier() / 5;
					}

					iValue += kBuilding.getNationalCaptureResistanceModifier();

					//TB Note: Once we have improved methods of evaluating the value of a given combat class and an ability to evaluate the strength
					//of a given combat class as it exists in volume among perceived or real enemies, we need to update the following to something
					//more intricate and accurate.  These are just semi-sufficient patches 'for now'.
					for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
					{
						iValue += kBuilding.getUnitCombatRepelModifier(iI) / 2;
						iValue += kBuilding.getUnitCombatRepelAgainstModifier(iI) / 2;
						iValue += kBuilding.getUnitCombatDefenseAgainstModifier(iI) / 2;
						if (kBuilding.isMayDamageAttackingUnitCombatType(iI))
						{
							iValue += (kBuilding.getDamageAttackerChance()*kBuilding.getDamageToAttacker())/4;
						}
					}	
					if (kBuilding.isDamageAllAttackers())
					{
						iValue += (kBuilding.getDamageAttackerChance()*kBuilding.getDamageToAttacker());
					}		

					
					
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/		
					iValue += -kBuilding.getAirModifier() / 4;
					iValue += -kBuilding.getNukeModifier() / 4;
					
					iValue += ((kBuilding.getAllCityDefenseModifier() * iNumCities) / 5);
					
					iValue += kBuilding.getAirlift() * 25;
	/************************************************************************************************/
	/* Afforess	                  Start		 03/31/10                                               */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
					if (kBuilding.isAreaBorderObstacle())
					{
						//The great wall is much more valuable with more barbarian activity.
						int iTempValue = (iNumCitiesInArea);
						if(GC.getGameINLINE().isOption(GAMEOPTION_RAGING_BARBARIANS))
							iTempValue *=2;
						if (GC.getGameINLINE().isOption(GAMEOPTION_BARBARIAN_WORLD))
							iTempValue *=2;
						if (GC.getGameINLINE().isOption(GAMEOPTION_BARBARIAN_GENERALS))
							iTempValue *=2;
						if (GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS))
							iTempValue = 0;
						iValue += iTempValue;
					}
				}
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/
				if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) || (iPass > 0))
				{
					iValue += kBuilding.getEspionageDefenseModifier() / 8;
				}

				//	If we're evaluating a building we already have (e.g. - for civic enabling/disabling)
				//	and it gives no unhealthy and that's the reason we have it, count it!
				bool bCountHappy = !isNoUnhappiness();

				if ( getNumBuilding(eBuilding) > 0 && kBuilding.isNoUnhappiness() )
				{
					bCountHappy = true;
				}

				if (((iFocusFlags & BUILDINGFOCUS_HAPPY) || (iPass > 0)) && bCountHappy)
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Happy");

					int iBestHappy = 0;
					for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
					{
						if (canHurryBuilding((HurryTypes)iI, eBuilding, true))
						{
							int iHappyFromHurry = AI_getHappyFromHurry((HurryTypes)iI, eBuilding, true);
							if (iHappyFromHurry > iBestHappy)
							{
								iBestHappy = iHappyFromHurry;
							}
						}
					}
					iValue += iBestHappy * 10;

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      02/24/10                              jdog5000        */
	/*                                                                                              */
	/* City AI                                                                                      */
	/************************************************************************************************/

	/********************************************************************************/
	/* 	Alternative Building Evaluation				09.03.2010		Fuyu		    */
	/********************************************************************************/
	/* original Better AI code
					if( iBuildingActualHappiness < 0 )
					{
						// Building causes net decrease in city happiness
						iValue -= (-iBuildingActualHappiness + iAngryPopulation) * 6
							+ (-iBuildingActualHappiness) * iHappyModifier;

						// BBAI TODO: Check for potential shrink in population
						
					}
					else if( iBuildingActualHappiness > 0 )
					{
						// Building causes net increase in city happiness
						iValue += (std::min(iBuildingActualHappiness, iAngryPopulation) * 10) 
							+ (std::max(0, iBuildingActualHappiness - iAngryPopulation) * iHappyModifier);
					}
	*/

					//Fuyu ToDo: How to handle Globe Theater national wonder?
					//For now just give massive boost if city is high food yet not one of the main production or commerce cities
					if (kBuilding.isNoUnhappiness() && bIsLimitedWonder)
					{
						iValue += (iAngryPopulation * 10) + getPopulation();
						aiYieldRank[YIELD_FOOD] = findBaseYieldRateRank(YIELD_FOOD);
						aiYieldRank[YIELD_COMMERCE] = findBaseYieldRateRank(YIELD_COMMERCE);
						if (aiYieldRank[YIELD_FOOD] <= (2 + iLimitedWonderLimit))
						{
							if (!(isNationalWonderClass(eBuildingClass)) ||
								((aiYieldRank[YIELD_PRODUCTION] > (2 + iLimitedWonderLimit)) 
								&& (aiYieldRank[YIELD_COMMERCE] > (2 + iLimitedWonderLimit))))
							{
								iValue += getPopulation() * 5;
							}
						}
					}
					else
					{
						iValue += happynessValue(iBuildingActualHappiness, iBaseHappinessLevel, iBaseHealthLevel);
					}
	/********************************************************************************/
	/* 	ABE END																	    */
	/********************************************************************************/

					iValue += (-kBuilding.getHurryAngerModifier() * getHurryPercentAnger()) / 100;

					for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
					{
						iValue += (kBuilding.getCommerceHappiness(iI) * iHappyModifier) / 4;
					}

					int iWarWearinessModifer = kBuilding.getWarWearinessModifier();
					if (iWarWearinessModifer != 0)
					{
						if ( GET_TEAM(getTeam()).getAtWarCount(true) == 0 )
						{
							iWarWearinessModifer /= 2;
						}

						iValue += (-iWarWearinessModifer * iHappyModifier) / 16;
					}

					iValue += (kBuilding.getAreaHappiness() * (iNumCitiesInArea - 1) * 8);
					iValue += (kBuilding.getGlobalHappiness() * (iNumCities - 1) * 8);

					int iWarWearinessPercentAnger = kOwner.getWarWearinessPercentAnger();
					int iGlobalWarWearinessModifer = kBuilding.getGlobalWarWearinessModifier();
					if (iGlobalWarWearinessModifer != 0)
					{
						iValue += (-(((iGlobalWarWearinessModifer * iWarWearinessPercentAnger / 100) / GC.getPERCENT_ANGER_DIVISOR())) * (iNumCities - 1));
						iValue += (-iGlobalWarWearinessModifer * iHappyModifier) / 16;
					}


					for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
					{
						int iBuildingHappinessChanges = kBuilding.getBuildingHappinessChanges(iI);
						if (iBuildingHappinessChanges != 0)
						{
							BuildingTypes eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
							if (eLoopBuilding != NO_BUILDING)
							{
								iValue += (iBuildingHappinessChanges * (kOwner.getBuildingClassCount((BuildingClassTypes)iI) - getNumBuilding(eLoopBuilding)) * 8);
							}
							else
							{
								iValue += (iBuildingHappinessChanges * kOwner.getBuildingClassCount((BuildingClassTypes)iI) * 8);
							}
						}
					}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/

	/************************************************************************************************/
	/* Afforess	                  Start		 07/12/10                                               */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
					if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_REVOLUTION))
					{
						if (kBuilding.getRevIdxLocal() != 0)
						{
							int localRevIdx = getLocalRevIndex();

							//	Use the more serious of the before and after values if this building were to be built
							if ( kBuilding.getRevIdxLocal() > 0 )
							{
								localRevIdx += kBuilding.getRevIdxLocal();
							}

							//	Treat instability seriously as it goes up - not just linear
							int localRevScaling = (localRevIdx < 0 ? 0 : std::min(localRevIdx*localRevIdx/50 + localRevIdx/2, 100));

							iValue -= (kBuilding.getRevIdxLocal() * localRevScaling)/4;
						}
						if (kBuilding.getRevIdxNational() != 0)
						{
							iValue -= (8 * GET_PLAYER(getOwnerINLINE()).getNumCities()) * kBuilding.getRevIdxNational();
						}
						if (kBuilding.getRevIdxDistanceModifier() != 0)
						{
							if (!isCapital())
							{
								CvCity* pCapital = GET_PLAYER(getOwnerINLINE()).getCapitalCity();
								if (pCapital != NULL)
								{
									int iCapitalDistance = ::plotDistance(getX_INLINE(), getY_INLINE(), pCapital->getX_INLINE(), pCapital->getY_INLINE());
									int iOldCapitalDistance = iCapitalDistance;
									iCapitalDistance *= 100 + kBuilding.getRevIdxDistanceModifier();
									iCapitalDistance /= 100;
									
									iValue += ((iOldCapitalDistance - iCapitalDistance) * (10+std::max(0,getLocalRevIndex())));
								}
							}
						}
					}
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/


				}

				//	If we're evaluating a building we already have (e.g. - for civic enabling/disabling)
				//	and it gives no unhealthy and that's the reason we have it, count it!
				bool bCountHealth = !isNoUnhealthyPopulation();

				if ( getNumBuilding(eBuilding) > 0 && kBuilding.isNoUnhealthyPopulation() )
				{
					bCountHealth = true;
				}

				if (((iFocusFlags & BUILDINGFOCUS_HEALTHY) || (iPass > 0)) && bCountHealth)
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Healthy");

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      02/24/10                          jdog5000 & Fuyu     */
	/*                                                                                              */
	/* City AI                                                                                      */
	/************************************************************************************************/

	/********************************************************************************/
	/* 	Alternative Building Evaluation				09.03.2010		Fuyu		    */
	/********************************************************************************/
	/* original Better AI code
					if( iBuildingActualHealth < 0 )
					{
						// Building causes net decrease in city health
						iValue -= (-iBuildingActualHealth + iBadHealth) * 6
							+ (-iBuildingActualHealth) * iHealthModifier;

						// BBAI TODO: Check for potential shrink in population
						
					}
					else if( iBuildingActualHealth > 0 )
					{
						// Building causes net increase in city health
						iValue += (std::min(iBuildingActualHealth, iBadHealth) * 10)
							+ (std::max(0, iBuildingActualHealth - iBadHealth) * iHealthModifier);
					}
	*/

					iValue += healthValue(iBuildingActualHealth, iBaseHappinessLevel - (getEspionageHappinessCounter() / 2) + std::max(0, iBuildingActualHappiness), iBaseHealthLevel, iBaseFoodDifference);
	/********************************************************************************/
	/* 	ABE END																	    */
	/********************************************************************************/

					iValue += (kBuilding.getAreaHealth() * (iNumCitiesInArea - 1 ) * 4);
					iValue += (kBuilding.getGlobalHealth() * (iNumCities - 1) * 4);
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/
				}

				if ((iFocusFlags & BUILDINGFOCUS_EXPERIENCE) || (iPass > 0))
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Experience");

					iValue += (kBuilding.getFreeExperience() * ((iHasMetCount > 0) ? 12 : 6));

					for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
					{
						if (kBuilding.getUnitCombatFreeExperience((UnitCombatTypes)iI) != 0 && canTrain((UnitCombatTypes)iI))
						{
							iValue += (kBuilding.getUnitCombatFreeExperience((UnitCombatTypes)iI) * ((iHasMetCount > 0) ? 6 : 3));
						}

						if (kBuilding.isUnitCombatRetrainType((UnitCombatTypes)iI))
						{
							iValue += 20;
						}

						if (kBuilding.getNumUnitCombatOngoingTrainingDurations() > 0)
						{
							if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iI) > 0)
							{
								if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iI) < getUnitCombatOngoingTrainingTimeIncrement((UnitCombatTypes)iI))
								{
									iValue = (20 - kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iI)) * 10;
								}
							}
						}
					}

					for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
					{
						int iDomainExpValue = 0;
						if (iI == DOMAIN_SEA)
						{
							iDomainExpValue = 7;
						}
						else if (iI == DOMAIN_LAND)
						{
							iDomainExpValue = 12;
						}
						else
						{
							iDomainExpValue = 6;
						}
						iValue += (kBuilding.getDomainFreeExperience(iI) * ((iHasMetCount > 0) ? iDomainExpValue : iDomainExpValue / 2));
					}
					/************************************************************************************************/
					/* Afforess	                  Start		 06/04/10                                               */
					/*                                                                                              */
					/*                                                                                              */
					/************************************************************************************************/
					int iTempValue = 0;
					bool bWarPlan = (GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0);
					if (kBuilding.getFreePromotion() != NO_PROMOTION)
					{
						iTempValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromotion());
					}
					if (kBuilding.getFreePromotion_2() != NO_PROMOTION)
					{
						iTempValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromotion_2());
					}
					if (kBuilding.getFreePromotion_3() != NO_PROMOTION)
					{
						iTempValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromotion_3());
					}
					for (int iI = 0; iI < kBuilding.getNumFreePromoTypes(); iI++)
					{
						if (kBuilding.getFreePromoType(iI).ePromotion)
						{
							if (kBuilding.getFreePromoType(iI).m_pExprFreePromotionCondition)
							{
								iTempValue += (AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromoType(iI).ePromotion)/2);
							}
							else
							{
								iTempValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromoType(iI).ePromotion);
							}
						}
					}
					for (int iI = 0; iI < kBuilding.getNumFreeTraitTypes(); iI++)
					{
						TraitTypes eTrait = kBuilding.getFreeTraitType(iI).eTrait;
						if (GC.getTraitInfo(eTrait).isCivilizationTrait())
						{
							if (!GC.getTraitInfo(eTrait).isNegativeTrait())
							{
								for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
								{
									iTempValue += (GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ));
								}
							}
							else
							{
								for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
								{
									iTempValue -= (GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ));
								}
							}
						}
					}
					if (kBuilding.isApplyFreePromotionOnMove())
					{
						iTempValue *= 2;
					}

					iValue += kBuilding.getNationalCaptureProbabilityModifier() * 2;

					iValue += iTempValue;
					bool bDevelopingCity = isDevelopingCity();
					if ((!bDevelopingCity || isCapital()) && kBuilding.EnablesUnits())
					{
						for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
						{
							CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);
							bool bUnitIsEnabler = (kUnit.getPrereqBuilding() == eBuilding);
							bool bUnitIsOtherwiseEnabled = (bUnitIsEnabler ||
															(BuildingTypes)kUnit.getPrereqBuilding() == NO_BUILDING ||
															getNumBuilding((BuildingTypes)kUnit.getPrereqBuilding()) > 0) &&
														   GET_TEAM(getTeam()).isHasTech((TechTypes)kUnit.getPrereqAndTech());

							if ( bUnitIsOtherwiseEnabled )
							{
								bool bUnitIsBonusEnabled = true;
								if ( kUnit.getPrereqAndBonus() != NO_BONUS )
								{
									if ( isFreeBonusOfBuilding(kBuilding, (BonusTypes)kUnit.getPrereqAndBonus()) )
									{
										if ( !hasBonus((BonusTypes)kUnit.getPrereqAndBonus()) )
										{
											bUnitIsEnabler = true;
										}
									}
									else
									{
										if ( !hasBonus((BonusTypes)kUnit.getPrereqAndBonus()) )
										{
											bUnitIsBonusEnabled = false;
										}
									}
								}

								bool	bHasORBonusAlready = false;
								bool	bFreeBonusIsORBonus = false;
								int		iFreeExtraBonusCount = 0;

								for (int iK = 0; iK < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); iK++)
								{
									BonusTypes eXtraFreeBonus = (BonusTypes)kUnit.getPrereqOrBonuses(iK);

									if ( eXtraFreeBonus != NO_BONUS )
									{
										iFreeExtraBonusCount++;

										if ( hasBonus(eXtraFreeBonus) )
										{
											bHasORBonusAlready = true;
										}
										else if ( isFreeBonusOfBuilding(kBuilding, eXtraFreeBonus) )
										{
											bFreeBonusIsORBonus = true;
										}
									}
								}

								if ( iFreeExtraBonusCount > 0 && !bHasORBonusAlready )
								{
									if ( bFreeBonusIsORBonus )
									{
										bUnitIsEnabler = true;
									}
									else
									{
										bUnitIsBonusEnabled = false;
									}
								}

								if ( bUnitIsEnabler )
								{
									PROFILE("CalculateAllBuildingValues.IsUnitPrereq");

									if (kUnit.getDefaultUnitAIType() != NO_UNITAI && kOwner.AI_totalAreaUnitAIs(area(), ((UnitAITypes)(kUnit.getDefaultUnitAIType()))) == 0)
									{
										iValue += iNumCitiesInArea;
									}
									//This forces the AI to build necessary buildings for units. 
									
									UnitAITypes eUnitAI = (UnitAITypes)(kUnit.getDefaultUnitAIType());
									
									int iAllowedUnitsValue = 0;
									
									switch (eUnitAI)
									{
										case UNITAI_UNKNOWN:
										break;
										case UNITAI_ANIMAL:
											iAllowedUnitsValue += kUnit.getCombat() / 5;
											break;
										case UNITAI_SETTLE:
										case UNITAI_SETTLER_SEA:
											iAllowedUnitsValue += 25;
											break;
										case UNITAI_WORKER:
										case UNITAI_WORKER_SEA:
											iAllowedUnitsValue += std::max(0, AI_getWorkersNeeded()) * 10;
											break;
										case UNITAI_ATTACK:
										case UNITAI_ATTACK_CITY:
										case UNITAI_COLLATERAL:
										case UNITAI_PILLAGE:
										case UNITAI_RESERVE:
										case UNITAI_COUNTER:
										case UNITAI_CITY_DEFENSE:
										case UNITAI_CITY_COUNTER:
										case UNITAI_CITY_SPECIAL:
										case UNITAI_ATTACK_SEA:
										case UNITAI_RESERVE_SEA:
										case UNITAI_ESCORT_SEA:
										case UNITAI_ASSAULT_SEA:
										case UNITAI_CARRIER_SEA:
										case UNITAI_MISSILE_CARRIER_SEA:
										case UNITAI_PIRATE_SEA:
										case UNITAI_ATTACK_AIR:
										case UNITAI_DEFENSE_AIR:
										case UNITAI_CARRIER_AIR:
										case UNITAI_MISSILE_AIR:
										case UNITAI_PARADROP:
										case UNITAI_ATTACK_CITY_LEMMING:
											iAllowedUnitsValue += std::max(6, (kUnit.getCombat() * kUnit.getCombat())) / 5 + (kUnit.getAirCombat() * kUnit.getAirCombat()) / 5;
											break;
										case UNITAI_INVESTIGATOR:
										case UNITAI_PROPERTY_CONTROL:
										case UNITAI_HEALER:
										case UNITAI_PROPERTY_CONTROL_SEA:
										case UNITAI_HEALER_SEA:
										case UNITAI_EXPLORE:
										case UNITAI_MISSIONARY:
										case UNITAI_PROPHET:
										case UNITAI_ARTIST:
										case UNITAI_SCIENTIST:
										case UNITAI_GENERAL:
										case UNITAI_GREAT_HUNTER:
										case UNITAI_GREAT_ADMIRAL:
										case UNITAI_MERCHANT:
										case UNITAI_ENGINEER:
										case UNITAI_SPY:
										case UNITAI_SPY_SEA:
										case UNITAI_MISSIONARY_SEA:
										case UNITAI_EXPLORE_SEA:
											iAllowedUnitsValue += 10;
											break;
										case UNITAI_ICBM:
											iAllowedUnitsValue += (kUnit.getNukeRange() != -1 ? kUnit.getNukeRange() * 50: 0);
											break;
									}
									if (bWarPlan)
										iAllowedUnitsValue *= 2;

									if (iAllowedUnitsValue > 0)
									{
										//	Just because we can build new units here doesn't add much if we can already
										//	build them a ton of other places
										iAllowedUnitsValue = (iAllowedUnitsValue*3)/(3+std::min(10,kOwner.countNumBuildings(eBuilding)));
									}

									iValue += bUnitIsBonusEnabled ? iAllowedUnitsValue : iAllowedUnitsValue/5;
								}
							}
						}
					}
				}
				/************************************************************************************************/
				/* Afforess	                     END                                                            */
				/************************************************************************************************/				
				// since this duplicates BUILDINGFOCUS_EXPERIENCE checks, do not repeat on pass 1
				if ((iFocusFlags & BUILDINGFOCUS_DOMAINSEA))
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Sea");

					iValue += (kBuilding.getFreeExperience() * ((iHasMetCount > 0) ? 16 : 8));

					for (int iUnitIndex = 0; iUnitIndex < GC.getNumUnitClassInfos(); iUnitIndex++)
					{
						UnitTypes eUnit = (UnitTypes)kCivilization.getCivilizationUnits(iUnitIndex);

						if (NO_UNIT != eUnit)
						{
							CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);
							int iCombatType = kUnitInfo.getUnitCombatType();
							if (kUnitInfo.getDomainType() == DOMAIN_SEA && canTrain(eUnit) && iCombatType != NO_UNITCOMBAT)
							{
								iValue += (kBuilding.getUnitCombatFreeExperience(iCombatType) * ((iHasMetCount > 0) ? 6 : 3));

								if (kBuilding.isUnitCombatRetrainType((UnitCombatTypes)iCombatType))
								{
									iValue += 20;
								}

								if (kBuilding.getNumUnitCombatOngoingTrainingDurations() > 0)
								{
									if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iUnitIndex) > 0)
									{
										if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iUnitIndex) < getUnitCombatOngoingTrainingTimeIncrement((UnitCombatTypes)iUnitIndex))
										{
											iValue = (20 - kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iUnitIndex)) * 10;
										}
									}
								}
							}
						}
					}

					iValue += (kBuilding.getDomainFreeExperience(DOMAIN_SEA) * ((iHasMetCount > 0) ? 16 : 8));

					iValue += (kBuilding.getDomainProductionModifier(DOMAIN_SEA) / 4);
				}

				if ((iFocusFlags & BUILDINGFOCUS_MAINTENANCE) || (iFocusFlags & BUILDINGFOCUS_GOLD) || (iPass > 0))
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Maintenance");

					int iExtraMaintenance = 0;
					if ( kBuilding.getCommerceChange(COMMERCE_GOLD) < 0 && GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE"))
					{
						iExtraMaintenance = -kBuilding.getCommerceChange(COMMERCE_GOLD);
						iExtraMaintenance *= GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent();
					}

					int iBaseMaintenance = getMaintenanceTimes100();
					int iExistingUpkeep = (iBaseMaintenance * std::max(0, 100 + getEffectiveMaintenanceModifier())) / 100;
					int iNewUpkeep = ((iBaseMaintenance + iExtraMaintenance)* std::max(0, 100 + getEffectiveMaintenanceModifier() + kBuilding.getMaintenanceModifier())) / 100;
					
					int iTempValue = (iExistingUpkeep - iNewUpkeep) / 16;
					
					if (bFinancialTrouble)
					{
						iTempValue *= 2;
					}
					else
					{
						iTempValue = (iGoldValueAssessmentModifier*iTempValue)/100;
					}

					iValue += iTempValue;
				}

				if ((iFocusFlags & BUILDINGFOCUS_SPECIALIST) || (iPass > 0))
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Specialist");

					int iSpecialistsValue = 0;
					int iCurrentSpecialistsRunnable = 0;
					for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
					{
						if (iI != GC.getDefineINT("DEFAULT_SPECIALIST"))
						{
							bool bUnlimited = (GET_PLAYER(getOwnerINLINE()).isSpecialistValid((SpecialistTypes)iI));
							int iRunnable = (getMaxSpecialistCount((SpecialistTypes)iI) > 0);
							
							if (bUnlimited || (iRunnable > 0))
							{
								if (bUnlimited)
								{
									iCurrentSpecialistsRunnable += 5;
								}
								else
								{
									iCurrentSpecialistsRunnable += iRunnable;
								}
							}
							
							
	/************************************************************************************************/
	/* Afforess	                  Start		 12/7/09                                                */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/		
							if ((kBuilding.getSpecialistCount(iI) + GET_TEAM(getTeam()).getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI)) > 0)	
							/* Was: 
							if (kBuilding.getSpecialistCount(iI) > 0)
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/	
							{
								if ((!bUnlimited) && (iRunnable < 5))
								{
									int iTempValue = AI_specialistValue(((SpecialistTypes)iI), false, false);

	/************************************************************************************************/
	/* Afforess	                  Start		 12/7/09                                                */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
									iTempValue *= (20 + (40 * (kBuilding.getSpecialistCount(iI) + GET_TEAM(getTeam()).getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI))));
									/* Was:
									iTempValue *= (20 + (40 * kBuilding.getSpecialistCount(iI)));
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/	
			
									iTempValue /= 100;

	/************************************************************************************************/
	/* UNOFFICIAL_PATCH                       01/09/10                                jdog5000      */
	/*                                                                                              */
	/* Bugfix                                                                                       */
	/************************************************************************************************/
	/* original bts code
									if (iFoodDifference < 2)
									{
										iValue /= 4;
									}
									if (iRunnable > 0)
									{
										iValue /= 1 + iRunnable;
									}
	*/
									if (iFoodDifference < 2)
									{
										iTempValue /= 4;
									}
									if (iRunnable > 0)
									{
										iTempValue /= 1 + iRunnable;
									}
	/************************************************************************************************/
	/* UNOFFICIAL_PATCH                        END                                                  */
	/************************************************************************************************/

									iSpecialistsValue += std::max(12, (iTempValue / 100));
								}
							}
						}
					}
					
					if (iSpecialistsValue > 0)
					{
						iValue += iSpecialistsValue / std::max(2, iCurrentSpecialistsRunnable);
					}
				}
				
				int iCommerceYieldValue = 0;
				if ((iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH | BUILDINGFOCUS_CULTURE | BUILDINGFOCUS_ESPIONAGE)) || iPass > 0)
				{
					iCommerceYieldValue = AI_buildingYieldValue(YIELD_COMMERCE, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[YIELD_COMMERCE]);
				}

				if ((iFocusFlags & BUILDINGFOCUS_GOLD) && iCommerceYieldValue > 0)
				{
					// trade routes
					iValue += (kOwner.getCommercePercent(COMMERCE_GOLD)*iCommerceYieldValue*(bFinancialTrouble ? 200 : iGoldValueAssessmentModifier))/10000;
				}
				
				if ((iFocusFlags & BUILDINGFOCUS_RESEARCH) && iCommerceYieldValue > 0)
				{
					// trade routes
					iValue += (kOwner.getCommercePercent(COMMERCE_RESEARCH)*iCommerceYieldValue)/100;
				}
				
				if ((iFocusFlags & BUILDINGFOCUS_CULTURE) && iCommerceYieldValue > 0)
				{
					// trade routes
					iValue += (kOwner.getCommercePercent(COMMERCE_CULTURE)*iCommerceYieldValue)/100;
				}
				
				if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) && iCommerceYieldValue > 0)
				{
					// trade routes
					iValue += (kOwner.getCommercePercent(COMMERCE_ESPIONAGE)*iCommerceYieldValue)/100;
				}

				if (iPass > 0)
				{
					for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
					{
						if (kBuilding.isHurry(iI))
						{
							iValue += 50;
						}
					}
					iValue -= kBuilding.getRiverDefensePenalty() / 4;
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      02/24/10                       jdog5000 & Afforess    */
	/*                                                                                              */
	/* City AI                                                                                      */
	/************************************************************************************************/
					if (kBuilding.isAreaCleanPower() && !(area()->isCleanPower(getTeam())))
					{
						int iLoop;
						for( CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop) )
						{
							if( pLoopCity->area() == area() )
							{
								if( pLoopCity->isDirtyPower() )
								{
									//Fuyu: less value for cities that don't already have power
									iValue += 8;
								}
								else if( !(pLoopCity->isPower()) )
								{
									iValue += 12;
								}
							}
						}
					}

					if (kBuilding.getDomesticGreatGeneralRateModifier() != 0)
					{
						iValue += (kBuilding.getDomesticGreatGeneralRateModifier() / 10);
					}

					if (kBuilding.isAreaBorderObstacle() && !(area()->isBorderObstacle(getTeam())))
					{
						if( !GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS) )
						{
							iValue += (iNumCitiesInArea);

							if(GC.getGameINLINE().isOption(GAMEOPTION_RAGING_BARBARIANS))
							{
								iValue += (iNumCitiesInArea);
							}
						}
					}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/

					if (kBuilding.isGovernmentCenter())
					{
						FAssert(!(kBuilding.isCapital()));
						iValue += ((calculateDistanceMaintenance() - 3) * iNumCitiesInArea);
					}

					if (kBuilding.isMapCentering())
					{
						iValue++;
					}

					if (kBuilding.getFreeBonus() != NO_BONUS)
					{
						//	If we have spares adding another doesn't do anything that scales by city count - only
						//	the first one does that.  Furthermore as spares rack up even their trade value decreases
						iValue += (kOwner.AI_bonusVal((BonusTypes)(kBuilding.getFreeBonus()), 1) *
									 ((kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getFreeBonus())) == 0) ? iNumCities : 1) *
									 kBuilding.getNumFreeBonuses() *
									 100)/(100*std::max(1,kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getFreeBonus()))));
					}

					for(int iI = 0; iI < kBuilding.getNumExtraFreeBonuses(); iI++)
					{
						//	If we have spares adding another doesn't do anything that scales by city count - only
						//	the first one does that.  Furthermore as spares rack up even their trade value decreases
						iValue += (kOwner.AI_bonusVal((BonusTypes)(kBuilding.getExtraFreeBonus(iI)), 1) *
									 ((kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getExtraFreeBonus(iI))) == 0) ? iNumCities : 1) *
									 kBuilding.getExtraFreeBonusNum(iI) *
									 100)/(100*std::max(1,kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getExtraFreeBonus(iI)))));
					}

					if (kBuilding.getNoBonus() != NO_BONUS)
					{
						iValue -= kOwner.AI_bonusVal((BonusTypes)kBuilding.getNoBonus());
					}

					if (kBuilding.getCivicOption() != NO_CIVICOPTION)
					{
						for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
						{
							if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == kBuilding.getCivicOption())
							{
								if (!(kOwner.canDoCivics((CivicTypes)iI)))
								{
									iValue += (kOwner.AI_civicValue((CivicTypes)iI) / 10);
								}
							}
						}
					}
					
					int iGreatPeopleRateModifier = kBuilding.getGreatPeopleRateModifier();
					if (iGreatPeopleRateModifier > 0)
					{
						int iGreatPeopleRate = getBaseGreatPeopleRate();
						const int kTargetGPRate = 10;

						// either not a wonder, or a wonder and our GP rate is at least the target rate
						if (!bIsLimitedWonder || iGreatPeopleRate >= kTargetGPRate)
						{
							iValue += ((iGreatPeopleRateModifier * iGreatPeopleRate) / 16);
						}
						// otherwise, this is a limited wonder (aka National Epic), we _really_ do not want to build this here
						// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
						else
						{
							iValue -= ((iGreatPeopleRateModifier * (kTargetGPRate - iGreatPeopleRate)) / 12);
						}
					}

					iValue += ((kBuilding.getGlobalGreatPeopleRateModifier() * iNumCities) / 8);

					iValue += (-(kBuilding.getAnarchyModifier()) / 4);

					iValue += (-(kBuilding.getGlobalHurryModifier()) * 2);

					iValue += (kBuilding.getGlobalFreeExperience() * iNumCities * ((iHasMetCount > 0) ? 6 : 3));

					if (bCanPopRush)
					{
						iValue += kBuilding.getFoodKept() / 2;
					}

					iValue += kBuilding.getAirlift() * (getPopulation() * 3 + 10);
					
					int iAirDefense = -kBuilding.getAirModifier();
					if (iAirDefense > 0)
					{
						if (((kOwner.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) > 0) && (kOwner.AI_totalUnitAIs(UNITAI_ATTACK_AIR) > 0)) || (kOwner.AI_totalUnitAIs(UNITAI_MISSILE_AIR) > 0))
						{
							iValue += iAirDefense / ((iHasMetCount > 0) ? 2 : 4);
						}
					}

					iValue += kBuilding.getAirUnitCapacity() * (getPopulation() * 2 + 10);

					iValue += (-(kBuilding.getNukeModifier()) / ((iHasMetCount > 0) ? 10 : 20));

					iValue += (kBuilding.getFreeSpecialist() * 16);
					iValue += (kBuilding.getAreaFreeSpecialist() * iNumCitiesInArea * 12);
					iValue += (kBuilding.getGlobalFreeSpecialist() * iNumCities * 12);

					iValue += ((kBuilding.getWorkerSpeedModifier() * kOwner.AI_getNumAIUnits(UNITAI_WORKER)) / 10);

					int iMilitaryProductionModifier = kBuilding.getMilitaryProductionModifier();

					for (int iJ = 0; iJ < kBuilding.getNumUnitCombatProdModifiers(); iJ++)
					{
						iMilitaryProductionModifier += kBuilding.getUnitCombatProdModifier(iJ) / 4;
					}
					if (iHasMetCount > 0 && iMilitaryProductionModifier > 0)
					{
						// either not a wonder, or a wonder and we are a high production city
						if (!bIsLimitedWonder || bIsHighProductionCity)
						{
							iValue += (iMilitaryProductionModifier / 4);

							// if a wonder, then pick one of the best cities
							if (bIsLimitedWonder)
							{
								// if one of the top 3 production cities, give a big boost
								if (aiYieldRank[YIELD_PRODUCTION] <= (2 + iLimitedWonderLimit))
								{
									iValue += (2 * iMilitaryProductionModifier) / (2 + aiYieldRank[YIELD_PRODUCTION]);
								}
							}
							// otherwise, any of the top half of cities will do
							else if (bIsHighProductionCity)
							{
								iValue += iMilitaryProductionModifier / 4;
							}
							iValue += ((iMilitaryProductionModifier * (getFreeExperience() + getSpecialistFreeExperience())) / 10);
						}
						// otherwise, this is a limited wonder (aka Heroic Epic), we _really_ do not want to build this here
						// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
						else
						{
							iValue -= (iMilitaryProductionModifier * aiYieldRank[YIELD_PRODUCTION]) / 5;
						}
					}
					iValue += (kBuilding.getSpaceProductionModifier() / 5);
					iValue += ((kBuilding.getGlobalSpaceProductionModifier() * iNumCities) / 20);
					

					if (kBuilding.getGreatPeopleUnitClass() != NO_UNITCLASS)
					{
						iValue++; // XXX improve this for diversity...
					}
					
					// prefer to build great people buildings in places that already have some GP points
	/************************************************************************************************/
	/* Afforess	                  Start		 07/28/10                                               */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	/*
					iValue += (kBuilding.getGreatPeopleRateChange() * 10) * (1 + (getBaseGreatPeopleRate() / 2));
	*/
					iValue += ((kBuilding.getGreatPeopleRateChange() + iSpecialistGreatPeopleRate ) * 10) * (1 + (getBaseGreatPeopleRate() / 2));
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/


					if (!bAreaAlone)
					{
						for (int iI = 0; iI < kBuilding.getNumHealUnitCombatTypes(); iI++)
						{
							iValue += (kBuilding.getHealUnitCombatType(iI).iHeal /4);
						}
						iValue += (kBuilding.getHealRateChange() / 2);
						for (int iI = 0; iI < kBuilding.getNumAidRateChanges(); iI++)
						{
							iValue += (kBuilding.getAidRateChange(iI).iChange / 3);//Update
						}
						for (int iI = 0; iI < kBuilding.getNumBonusAidModifiers(); iI++)
						{
							if (hasBonus(kBuilding.getBonusAidModifier(iI).eBonusType))
							{
								iValue += kBuilding.getBonusAidModifier(iI).iModifier / 3;
							}
						}
					}

					if ( kBuilding.getGlobalPopulationChange() != 0 )
					{
						int iFoodPerTurn = foodDifference(false);
						int iFoodToGrow = growthThreshold();
						int	iGrowthTurns = iFoodToGrow/std::max(1,iFoodPerTurn);
						int	iValueMultiplier = (400 * iNumCities * iGrowthTurns)/(20*GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent());

						iValue += (kBuilding.getGlobalPopulationChange() * iValueMultiplier);
					}

					iValue += (kBuilding.getFreeTechs() * 80);

					iValue += kBuilding.getEnemyWarWearinessModifier() / 2;

					for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
					{
						if (kBuilding.getFreeSpecialistCount(iI) > 0)
						{
							iValue += ((AI_specialistValue(((SpecialistTypes)iI), false, false) * kBuilding.getFreeSpecialistCount(iI)) / 50);
						}
					}

					for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
					{
						if (kBuilding.getImprovementFreeSpecialist(iI) > 0)
						{
							iValue += kBuilding.getImprovementFreeSpecialist(iI) * countNumImprovedPlots((ImprovementTypes)iI, true) * 50;
						}
					}

					for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
					{
						iValue += (kBuilding.getDomainProductionModifier(iI) / 5);
						
						if (bIsHighProductionCity)
						{
							iValue += (kBuilding.getDomainProductionModifier(iI) / 5);
						}
					}
	/************************************************************************************************/
	/* Afforess	                  Start		 12/7/09                                                */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
					int forcedTradeRoutesValue = 0;
					if (kBuilding.isForceAllTradeRoutes())
					{
						for (int iI = 0; iI < MAX_TEAMS; iI++)
						{
							if (GET_TEAM((TeamTypes)iI).isAlive())
							{
								if (!GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isVassal((TeamTypes)iI))
								{
									if (!GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isOpenBorders((TeamTypes)iI))
									{
										forcedTradeRoutesValue += GET_TEAM((TeamTypes)iI).getNumCities() * 2;
									}
								}
							}
						}
					}
					iValue += forcedTradeRoutesValue;
					if (kBuilding.getUnitClassProductionModifier(NO_UNITCLASS) != 0)
					{
						int unitProductionModifierValue = 0;
						for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
						{
							PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.UnitClass");

							int iModifier = kBuilding.getUnitClassProductionModifier(iI);
							if (iModifier != 0)
							{
								UnitTypes eLoopUnit = (UnitTypes)kCivilization.getCivilizationUnits(iI);
							
								if (eLoopUnit != NO_UNIT && canTrain(eLoopUnit))
								{
									UnitAITypes eUnitAI = (UnitAITypes) GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType();
									//unitProductionModifierValue += GET_PLAYER(getOwnerINLINE()).AI_unitValue(eLoopUnit, eUnitAI, area()) * iModifier / 100;

									UnitTypes eBestUnit = kOwner.bestBuildableUnitForAIType((DomainTypes)GC.getUnitInfo(eLoopUnit).getDomainType(), eUnitAI);

									int iBuildCost = 0;
									if ( eBestUnit == NO_UNIT )
									{
										iBuildCost = GC.getUnitInfo(eLoopUnit).getProductionCost();
									}
									else
									{
										int	iBestUnitAIValue = kOwner.AI_unitValue(eBestUnit, eUnitAI, area());
										int	iThisUnitAIValue = kOwner.AI_unitValue(eLoopUnit, eUnitAI, area());

										//	Value as cost of production of the unit we can build scaled by their relative AI value (non-linear - we're squaring the ratio)
										int	iComparisonToBestFactor = (10*iThisUnitAIValue)/std::max(1,iBestUnitAIValue);

										iBuildCost = (iComparisonToBestFactor * iComparisonToBestFactor * GC.getUnitInfo(eBestUnit).getProductionCost())/100;
									}

									unitProductionModifierValue += (iBuildCost*2*iModifier)/100;
								}
							}
						}
						if (bIsHighProductionCity)
						{
							unitProductionModifierValue *= 3; // adding 10% and 20% is same as adding 30% or 3 * 10%
						}	

						iValue += unitProductionModifierValue;
					}
					if (kBuilding.getPopulationgrowthratepercentage() != 0)
					{
						int iFoodPerTurn = foodDifference();

						if ( iFoodPerTurn > 0 )
						{
							int iCityHappy = happyLevel() - unhappyLevel();
							if ( iCityHappy >= 0 )
							{
								int iCurrentFoodToGrow = growthThreshold();
								int growthValue = (std::min(3,iCityHappy+1)*iCurrentFoodToGrow)/iFoodPerTurn;
								iValue -= (kBuilding.getPopulationgrowthratepercentage()*growthValue)/100;
							}
						}

					}
					if (kBuilding.getGlobalPopulationgrowthratepercentage() != 0)
					{
						int iLoop;
						int iCityCount = 0;
						int globalGrowthValue = 0;
						for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
						{
							int iCityHappy = pLoopCity->happyLevel() - pLoopCity->unhappyLevel();
							int iCurrentFoodToGrow = pLoopCity->growthThreshold();
							int iFoodPerTurn = pLoopCity->foodDifference();

							iCityCount++;

							if ( iFoodPerTurn > 0 )
							{
								if ( iCityHappy >= 0 )
								{
									globalGrowthValue -= (std::min(3,iCityHappy+1)*iCurrentFoodToGrow)/iFoodPerTurn;
								}
							}
						}

						if ( iCityCount > 0 )
						{
							iValue += (kBuilding.getGlobalPopulationgrowthratepercentage()*globalGrowthValue)/(iCityCount*100);
						}
					}

					if (kBuilding.getBuildingClassProductionModifier(NO_BUILDINGCLASS) != 0)
					{
						int buildingProductionModifierValue = 0;
						for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
						{
							PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.BuildingClass1");

							if (kBuilding.getBuildingClassProductionModifier(iI) != 0)
							{
								BuildingTypes eLoopBuilding = ((BuildingTypes)(kCivilization.getCivilizationBuildings(iI)));
								if (eLoopBuilding != NO_BUILDING)
								{
									if (canConstruct(eLoopBuilding))
									{
										int iModifier = kBuilding.getBuildingClassProductionModifier(iI);
										int iOriginalCost = getHurryCost(true, eLoopBuilding, false);
										if (iModifier > -100)
										{
											int	iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
											buildingProductionModifierValue +=  (iOriginalCost - iNewCost) / 10;
										}
										else
										{//If the modifier is less than -100, avoid it like the plague
											buildingProductionModifierValue -= 100;
										}
									}
								}
							}
						}
						iValue += buildingProductionModifierValue;
					}

					if (kBuilding.getGlobalBuildingClassProductionModifier(NO_BUILDINGCLASS) != 0)
					{
						int globalBuildingProductionModifierValue = 0;
						for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
						{
							PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.BuildingClass2");

							if (kBuilding.getGlobalBuildingClassProductionModifier(iI) != 0)
							{
								BuildingTypes eLoopBuilding = ((BuildingTypes)(kCivilization.getCivilizationBuildings(iI)));
								if (eLoopBuilding != NO_BUILDING)
								{

									if (canConstruct(eLoopBuilding))
									{
										int iModifier = kBuilding.getGlobalBuildingClassProductionModifier(iI);
										int iOriginalCost = getHurryCost(true, eLoopBuilding, false);

										if (iModifier > -100)
										{
											int iLoop;
											int iCount = 0;

											for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
											{
												if ( pLoopCity->getNumBuilding(eLoopBuilding) == 0 )
												{
													iCount++;
												}
											}

											int iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
											globalBuildingProductionModifierValue += ((iOriginalCost - iNewCost)*iCount) / 10;
										}
										else
										{//If the modifier is less than -100, avoid it like the plague
											globalBuildingProductionModifierValue -= 100;
										}
									}
								}
							}
						}
						iValue += globalBuildingProductionModifierValue;
					}
					if (kBuilding.isProvidesFreshWater() && !plot()->isFreshWater())
					{
						CvPlot* pLoopPlot;
						int freshWaterValue = 0;
						for (int iI = 0; iI < NUM_CITY_PLOTS_1; iI++)
						{
							if (iI != CITY_HOME_PLOT)
							{
								pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
								if (pLoopPlot != NULL &&
									!pLoopPlot->isWater() &&
									!pLoopPlot->isFreshWater() &&
									!pLoopPlot->isHills() &&
									!pLoopPlot->isImpassable() )
								{
									if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
									{//if there are no improvements, farms become much more likely
										freshWaterValue += 20;
									}
									else
									{//we are not likely to tear down existing improvements for a farm
										freshWaterValue += 5;
									}
								}
							}
						}
						iValue += freshWaterValue;
					}
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/	

					int religiousBuildingValue = 0;
					for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
					{
						PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Units");

						if (GC.getUnitInfo((UnitTypes)iI).getPrereqBuilding() == eBuilding)
						{
							// BBAI TODO: Smarter monastary construction, better support for mods

							if (kOwner.AI_totalAreaUnitAIs(area(), ((UnitAITypes)(GC.getUnitInfo((UnitTypes)iI).getDefaultUnitAIType()))) == 0)
							{
								religiousBuildingValue += iNumCitiesInArea;
							}

							religiousBuildingValue++;

							ReligionTypes eReligion = (ReligionTypes)(GC.getUnitInfo((UnitTypes)iI).getPrereqReligion());
							if (eReligion != NO_RELIGION)
							{
								//encouragement to get some minimal ability to train special units
								if (bCulturalVictory1 || isHolyCity(eReligion) || isCapital())
								{
									religiousBuildingValue += (2 + iNumCitiesInArea);
								}
								
								if (bCulturalVictory2 && GC.getUnitInfo((UnitTypes)iI).getReligionSpreads(eReligion))
								{
									//this gives a very large extra value if the religion is (nearly) unique
									//to no extra value for a fully spread religion.
									//I'm torn between huge boost and enough to bias towards the best monastery type.
									int iReligionCount = GET_PLAYER(getOwnerINLINE()).getHasReligionCount(eReligion);
									religiousBuildingValue += (100 * (iNumCities - iReligionCount)) / (iNumCities * (iReligionCount + 1));
								}
							}
						}
					}

					if (religiousBuildingValue > 0)
					{
						//	Just because we can build new units here doesn't add much if we can already
						//	build them a ton of other places
						religiousBuildingValue = (religiousBuildingValue*3)/(3+std::min(10,kOwner.countNumBuildings(eBuilding)));
					}

					iValue += religiousBuildingValue;
					
					// is this building needed to build other buildings?
					for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
					{
						PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Buildings");

						int iPrereqBuildings = kOwner.getBuildingClassPrereqBuilding(((BuildingTypes) iI), eBuildingClass);

						// if we need some of us to build iI building, and we dont need more than we have cities
						if (iPrereqBuildings > 0 && iPrereqBuildings <= iNumCities)
						{
							// do we need more than what we are currently building?
							if (iPrereqBuildings > kOwner.getBuildingClassCountPlusMaking(eBuildingClass))
							{
								iValue += (iNumCities * 3);

								if (bCulturalVictory1)
								{
									BuildingTypes eLoopBuilding = (BuildingTypes) iI;
									CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
									int iLoopBuildingCultureModifier = kLoopBuilding.getCommerceModifier(COMMERCE_CULTURE);
									if (iLoopBuildingCultureModifier > 0)
									{
										int iLoopBuildingsBuilt = kOwner.getBuildingClassCount((BuildingClassTypes) kLoopBuilding.getBuildingClassType());
										
										// if we have less than the number needed in culture cities
										//		OR we are one of the top cities and we do not have the building
										if (iLoopBuildingsBuilt < iCulturalVictoryNumCultureCities || 
											(iCultureRank <= iCulturalVictoryNumCultureCities && 0 == getNumBuilding(eLoopBuilding)))
										{
											iValue += iLoopBuildingCultureModifier;

											if (bCulturalVictory3)
											{
												iValue += iLoopBuildingCultureModifier * 2;
											}
										}
									}
								}
							}
						}
					}
					
					for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
					{
						PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Votes");

						if (kBuilding.getVoteSourceType() == iI)
						{
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      05/24/10                              jdog5000        */
	/*                                                                                              */
	/* City AI, Victory Strategy AI                                                                 */
	/************************************************************************************************/					
							int votingSourceValue = 0;
							if (kBuilding.isStateReligion())
							{
								int iShareReligionCount = 0;
								int iPlayerCount = 0;
								for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
								{
									CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
									if ((iPlayer != getOwner()) && kLoopPlayer.isAlive())
									{
										iPlayerCount++;
										if (GET_PLAYER(getOwnerINLINE()).getStateReligion() == kLoopPlayer.getStateReligion())
										{
											iShareReligionCount++;										
										}
									}
								}
								votingSourceValue += (200 * (1 + iShareReligionCount)) / (1 + iPlayerCount);
							}
							else
							{
								votingSourceValue += 100;
							}

							iValue += (votingSourceValue * (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_DIPLOMACY1) ? 5 : 1));
						}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/
					}

					for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
					{
						PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Yields");

						int yieldModifierValue = 0;

						//Trade route yields are now accounted in pass1
						//iValue += ((kBuilding.getTradeRouteModifier() * getTradeYield((YieldTypes)iI)) / 12);
						//if (bForeignTrade)
						//{
						//	iValue += ((kBuilding.getForeignTradeRouteModifier() * getTradeYield((YieldTypes)iI)) / 12);
						//}

						if (iFoodDifference > 0)
						{
							iValue += kBuilding.getFoodKept() / 2;
						}

						iValue += AI_buildingYieldValue((YieldTypes)iI, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[iI]);

						int iGlobalModifier = kBuilding.getGlobalYieldModifier(iI);
						int iAreaModifier = kBuilding.getAreaYieldModifier(iI);
						int globalYieldModifierValue = 0;

						if ( iGlobalModifier > 0 || iAreaModifier > 0 )
						{
							int iLoop = 0;
							for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
							{
								int iCityValue = pLoopCity->getBaseYieldRate((YieldTypes)iI);
								globalYieldModifierValue += iCityValue*(iGlobalModifier + (pLoopCity->area() == area() ? iAreaModifier : 0));
							}
						}
						yieldModifierValue += globalYieldModifierValue/12;
						//yieldModifierValue += ((kBuilding.getAreaYieldModifier(iI) * iNumCitiesInArea) / 3);
						//yieldModifierValue += ((kBuilding.getGlobalYieldModifier(iI) * iNumCities) / 3);

						for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
						{
							yieldModifierValue += ((kBuilding.getSpecialistYieldChange(iJ, iI) * kOwner.getTotalPopulation()) / 5);
						}
						
						if (yieldModifierValue != 0)
						{
							if (bFinancialTrouble && iI == YIELD_COMMERCE)
							{
								yieldModifierValue *= 2;
							}

							yieldModifierValue *= kOwner.AI_yieldWeight((YieldTypes)iI);
							yieldModifierValue /= 100;
							
							if (aiYieldRank[iI] == MAX_INT)
							{
								aiYieldRank[iI] = findBaseYieldRateRank((YieldTypes) iI);
							}

							// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
							// we do _not_ want to build this here (unless the value was small anyway)
							// Exempt unit build from this test however
							if (bIsLimitedWonder && canConstruct(eBuilding) && (aiYieldRank[iI] > (3 + iLimitedWonderLimit)))
							{
								yieldModifierValue *= -1;
							}

							iValue += yieldModifierValue;
						}
					}
				}
				else
				{
					if (iFocusFlags & BUILDINGFOCUS_FOOD)
					{
						iValue += AI_buildingYieldValue(YIELD_FOOD, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[YIELD_FOOD]);

						iValue += kBuilding.getFoodKept();
					}

					if (iFocusFlags & BUILDINGFOCUS_PRODUCTION)
					{
						PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Production");

						int directYieldValue = AI_buildingYieldValue(YIELD_PRODUCTION, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[YIELD_PRODUCTION]);
						
						// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
						// we do _not_ want to build this here (unless the value was small anyway)
						if (bIsLimitedWonder && (aiYieldRank[YIELD_PRODUCTION] > (3 + iLimitedWonderLimit)))
						{
							directYieldValue *= -1;
						}

						iValue += directYieldValue;

						// Koshling - modify direct production value if the base yield rate for this city (for production) is low
						// in relation to the amount this would add
						// i.e. - boost the value of extra production especially when we donit already have much
						if ( directYieldValue > getBaseYieldRate(YIELD_PRODUCTION)/3 )
						{
							//	directYieldValue is roughly 8*<direct production gain>, so if that implies a net gain
							//	of 4% (ish) or more boost the value
							int iBoost = (directYieldValue*12 - getBaseYieldRate(YIELD_PRODUCTION)*4);

							//	Examples -
							//	1) City production is 1, building adds 1 (directYieldValue 8), so iBoost is 92 - value almost trebbled
							//	2) Same building when production is already 10, iBoost is 56 - value roughly doubled
							//	3) Same building when production >= 24 - no boost
							directYieldValue += (directYieldValue*iBoost)/50;
						}

						iValue += directYieldValue;
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/	
					}
				}
				//	Deal with properties
				int iPropValue = buildingPropertiesValue(kBuilding);
				if (iFocusFlags & BUILDINGFOCUS_PROPERTY)
				{
					iValue += iPropValue * 10;
				}
				else
				{
					iValue += iPropValue * 5;
				}

				int iInvestigationValue = kBuilding.getInvestigation();
				if (iFocusFlags & BUILDINGFOCUS_INVESTIGATION)
				{
					iValue += iInvestigationValue * 10;
				}
				else
				{
					iValue += iInvestigationValue * 5;
				}

				if (iPass > 0)
				{

					for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
					{
						PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Commerces");

						int directCommerceValue = getBuildingCommerceValue(eBuilding, iI, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);

						for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
						{
							directCommerceValue += ((kBuilding.getSpecialistCommerceChange(iJ, iI) * kOwner.getTotalPopulation()) / 6);
						}
						if (directCommerceValue != 0)
						{
							//	Make sure we don't reduce 1 to 0!
							if ( directCommerceValue >= 2 )
							{
								directCommerceValue /= 2;
							}

							if (bFinancialTrouble && iI == COMMERCE_GOLD)
							{
								directCommerceValue *= 2;
							}

							directCommerceValue *= kOwner.AI_commerceWeight(((CommerceTypes)iI), this);
							directCommerceValue = (directCommerceValue + 99) / 100;

							// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
							// we do _not_ want to build this here (unless the value was small anyway)
							if (MAX_INT == aiCommerceRank[iI])
							{
								aiCommerceRank[iI] = findCommerceRateRank((CommerceTypes) iI);
							}
							if (bIsLimitedWonder && ((aiCommerceRank[iI] > (3 + iLimitedWonderLimit)))
								|| (bCulturalVictory1 && (iI == COMMERCE_CULTURE) && (aiCommerceRank[iI] == 1)))
							{
								directCommerceValue *= -1;

								// for culture, just set it to zero, not negative, just about every wonder gives culture
								if (iI == COMMERCE_CULTURE)
								{
									directCommerceValue = 0;
								}
							}
							iValue += directCommerceValue;
						}
					}

					for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
					{
						if (kBuilding.getReligionChange(iI) > 0)
						{
							if (GET_TEAM(getTeam()).hasHolyCity((ReligionTypes)iI))
							{
								iValue += (kBuilding.getReligionChange(iI) * ((eStateReligion == iI) ? 10 : 1));
							}
						}
					}

					if (NO_VOTESOURCE != kBuilding.getVoteSourceType())
					{
						iValue += 100;
					}
				}
				else
				{
					if (iFocusFlags & BUILDINGFOCUS_GOLD)
					{
						iValue += (getBuildingCommerceValue(eBuilding, COMMERCE_GOLD, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate)*iGoldValueAssessmentModifier)/100;
					}

					if (iFocusFlags & BUILDINGFOCUS_RESEARCH)
					{
						iValue += getBuildingCommerceValue(eBuilding, COMMERCE_RESEARCH, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);
					}

					if (iFocusFlags & BUILDINGFOCUS_CULTURE)
					{
						iValue += getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);
					}
					
					if (iFocusFlags & BUILDINGFOCUS_BIGCULTURE)
					{
						iValue += getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate)/5;
					}
					
					if (iFocusFlags & BUILDINGFOCUS_ESPIONAGE || (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE) && (iFocusFlags & BUILDINGFOCUS_CULTURE)))
					{
						iValue += getBuildingCommerceValue(eBuilding, COMMERCE_ESPIONAGE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);
					}
				}
				
				if ((iThreshold > 0) && (iPass == 0))
				{
					if (iValue < iThreshold)
					{
						iValue = 0;
					}
				}

				if (iPass > 0 && !isHuman() && !bForTech)
				{
					iValue += kBuilding.getAIWeight();
					if (iValue > 0)
					{
						for (int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
						{
							iValue += (kOwner.AI_getFlavorValue((FlavorTypes)iI) * kBuilding.getFlavorValue(iI));
						}
					}
				}
			}
		}
	//	
	//	// obsolete checks
	//	bool bCanResearchObsoleteTech = false;
	//	int iErasUntilObsolete = MAX_INT;
	//	if (kBuilding.getObsoleteTech() != NO_TECH)
	//	{
	//		TechTypes eObsoleteTech = (TechTypes) kBuilding.getObsoleteTech();
	//		FAssertMsg(eObsoleteTech == NO_TECH || !(GET_TEAM(getTeam()).isHasTech(eObsoleteTech)), "Team expected to not have the tech that obsoletes eBuilding");
	//		iErasUntilObsolete = GC.getTechInfo(eObsoleteTech).getEra() - kOwner.getCurrentEra();
	//		bCanResearchObsoleteTech = kOwner.canResearch(eObsoleteTech);
	//	}
	//
	//	if (kBuilding.getSpecialBuildingType() != NO_SPECIALBUILDING)
	//	{
	//		TechTypes eSpecialBldgObsoleteTech = (TechTypes) GC.getSpecialBuildingInfo((SpecialBuildingTypes)(kBuilding.getSpecialBuildingType())).getObsoleteTech();
	//		FAssertMsg(eSpecialBldgObsoleteTech == NO_TECH || !(GET_TEAM(getTeam()).isHasTech(eSpecialBldgObsoleteTech)), "Team expected to not have the tech that obsoletes eBuilding");
	//		if (eSpecialBldgObsoleteTech != NO_TECH)
	//		{
	//			int iSpecialBldgErasUntilObsolete = GC.getTechInfo(eSpecialBldgObsoleteTech).getEra() - kOwner.getCurrentEra();
	//
	//			if (iSpecialBldgErasUntilObsolete < iErasUntilObsolete)
	//			{
	//				iErasUntilObsolete = iSpecialBldgErasUntilObsolete;
	//			}
	//
	//			if (!bCanResearchObsoleteTech)
	//			{
	//				bCanResearchObsoleteTech = kOwner.canResearch(eSpecialBldgObsoleteTech);
	//			}
	//		}
	//	}
	//	
	//	// tech path method commented out, it would be more accurate if we want to be so.
	//	//int iObsoleteTechPathLength = 0;
	//	//if (kBuilding.getObsoleteTech() != NO_TECH)
	//	//{
	//	//	iObsoleteTechPathLength = findPathLength(kBuilding.getObsoleteTech(), false);
	//	//}
	//
	//	//if (kBuilding.getSpecialBuildingType() != NO_SPECIALBUILDING)
	//	//{
	//	//	TechTypes eSpecialBldgObsoleteTech = GC.getSpecialBuildingInfo((SpecialBuildingTypes)(kBuilding.getSpecialBuildingType())).getObsoleteTech();
	//	//	int iSpecialBldgObsoleteTechPathLength = findPathLength(eSpecialBldgObsoleteTech, false);
	//
	//	//	if (iSpecialBldgObsoleteTechPathLength < iObsoleteTechPathLength)
	//	//	{
	//	//		iObsoleteTechPathLength = iSpecialBldgObsoleteTechPathLength;
	//	//	}
	//	//}
	//	
	//	// if we can research obsolete tech, then this almost no value
	//	if (bCanResearchObsoleteTech)
	//	{
	//		iValue = std::min(16, (iValue + 31) / 32);
	//	}
	//	// if we are going obsolete in the next era, the current era, or even previous eras...
	//	else if (iErasUntilObsolete < 2)
	//	{
	//		// do not care about obsolete if we are going cultural and there is a culture benefit post obsolesence
	//		if (!bCulturalVictory1 || (kBuilding.getObsoleteSafeCommerceChange(COMMERCE_CULTURE) <= 0))
	//		{
	//			iValue++;
	//			iValue /= 2;
	//		}
	//	}

	//	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	//	{
	//		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBuildingInfo((BuildingTypes)iI).getObsoleteTech())))
	//		{
	//			if (GC.getBuildingInfo((BuildingTypes)iI).isBuildingClassNeededInCity(kBuilding.getBuildingClassType()))
	//			{
	//				iValue += AI_buildingValue((BuildingTypes)iI, iFocusFlags);
	//			}
	//
	//			int iNumPrereqs = kOwner.getBuildingClassPrereqBuilding((BuildingTypes)iI, (BuildingClassTypes)kBuilding.getBuildingClassType()) - kOwner.getBuildingClassCount((BuildingClassTypes)kBuilding.getBuildingClassType());
	//			if (iNumPrereqs > 0)
	//			{
	//				iValue += AI_buildingValue((BuildingTypes)iI, iFocusFlags) / iNumPrereqs;
	//			}
	//		}
	//	}

		//	If we are really seeking an answer specifically on the values from the specific flags
		//	then the rest is just a tie-breaker so dramatically boost the value of the flag-specific
		//	stuff from pass1
		if ( bMaximizeFlaggedValue )
		{
			iValue += iPass1Value*19;
		}
		else
		{
			//	Even when not maximizing with all the extra effects on buildings pass1 was getting a
			//	bit dominated by pass2 so provide a little extra pass1 weight anyway
			iValue += iPass1Value;
		}

		if (!bForTech && !canConstruct(eBuilding))
		{
			//This building is being constructed in some special way, 
			//reduce the value for small cities if it's a wonder.
			if (getPopulation() < 6 && bIsLimitedWonder)
			{
				iValue /= (8 - getPopulation());
			}
		}
	}

	return std::max(0, iValue);
}

int CvCityAI::AI_buildingYieldValue(YieldTypes eYield, BuildingTypes eBuilding, CvBuildingInfo& kBuilding, bool bForeignTrade, int iFoodDifference, int iFreeSpecialistYield)
{
	int iValue;
	int iTempValue;

	// trade routes
	iValue = tradeRouteValue(kBuilding, eYield, bForeignTrade);

	if (kBuilding.getSeaPlotYieldChange(eYield) > 0)
	{
		iTempValue = kBuilding.getSeaPlotYieldChange(eYield) * AI_buildingSpecialYieldChangeValue(eBuilding, eYield);

		if ( eYield == YIELD_FOOD )
		{
			if ((iTempValue < 8) && (getPopulation() > 3))
			{
				// don't bother
			}
			else
			{
				iValue += ((iTempValue * 4) / std::max(2, iFoodDifference));
			}
		}
		else
		{
			iValue += iTempValue;
		}
	}

/************************************************************************************************/
/* Afforess	                  Start		 3/26/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iTempValue = 0;
	iTempValue += ((GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, eYield) * getBaseYieldRate(eYield)) / 8);
	iTempValue += ((kBuilding.getYieldChange(eYield) + ((kBuilding.getYieldPerPopChange(eYield)*getPopulation())/100) + GET_TEAM(getTeam()).getBuildingYieldChange(eBuilding, eYield) + iFreeSpecialistYield) * 8);
	iTempValue += ((kBuilding.getYieldModifier(eYield) + GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, eYield)) * getBaseYieldRate(eYield) / 12);
	for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
	{
		if (hasBonus((BonusTypes)iJ))
		{
			iTempValue += (kBuilding.getBonusYieldModifier(iJ, eYield) * getBaseYieldRate(eYield) / 12);
			if (kBuilding.getVicinityBonusYieldChanges(iJ, eYield) != 0 && (hasVicinityBonus((BonusTypes)iJ) || hasRawVicinityBonus((BonusTypes)iJ)))
			{
				iTempValue += (kBuilding.getVicinityBonusYieldChanges(iJ, eYield) * 8);
			}

		}
	}
	iValue += iTempValue;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	if (kBuilding.getRiverPlotYieldChange(eYield) > 0)
	{
		iValue += (kBuilding.getRiverPlotYieldChange(eYield) * countNumRiverPlots() * 4);
	}

	if (!isPower() && (kBuilding.isPower() || ((kBuilding.getPowerBonus() != NO_BONUS) && hasBonus((BonusTypes)(kBuilding.getPowerBonus()))) || kBuilding.isAreaCleanPower()))
	{
/********************************************************************************/
/* 	Alternative Building Evaluation				18/01/10		Fuyu		    */
/********************************************************************************/
/* original code
		iTempValue += ((getPowerYieldRateModifier(YIELD_PRODUCTION) * getBaseYieldRate(YIELD_PRODUCTION)) / 12);

*/
		//Fuyu "/ 12" is too much. -> "/ 20"
		iValue += ((getPowerYieldRateModifier(eYield) * getBaseYieldRate(eYield)) / 20);
/********************************************************************************/
/* ABE END																	    */
/********************************************************************************/
	}


	return iValue;
}

ProjectTypes CvCityAI::AI_bestProject()
{
	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

	int iBestValue = 0;
	ProjectTypes eBestProject = NO_PROJECT;

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (canCreate((ProjectTypes)iI))
		{
			int iValue = AI_projectValue((ProjectTypes)iI);

			if ((GC.getProjectInfo((ProjectTypes)iI).getEveryoneSpecialUnit() != NO_SPECIALUNIT) ||
				  (GC.getProjectInfo((ProjectTypes)iI).getEveryoneSpecialBuilding() != NO_SPECIALBUILDING) ||
				  GC.getProjectInfo((ProjectTypes)iI).isAllowsNukes())
			{
				if (GC.getGameINLINE().getSorenRandNum(100, "Project Everyone") == 0)
				{
					iValue++;
				}
			}

			if (iValue > 0)
			{
				iValue += getProjectProduction((ProjectTypes)iI);

				int iTurnsLeft = getProductionTurnsLeft(((ProjectTypes)iI), 0);

				if ((iTurnsLeft <= GC.getGameINLINE().AI_turnsPercent(10, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getCreatePercent())) || !(GET_TEAM(getTeam()).isHuman()))
				{
					if ((iTurnsLeft <= GC.getGameINLINE().AI_turnsPercent(20, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getCreatePercent())) || (iProductionRank <= std::max(3, (GET_PLAYER(getOwnerINLINE()).getNumCities() / 2))))
					{
						if (iProductionRank == 1)
						{
							iValue += iTurnsLeft;
						}
						else
						{
							FAssert((MAX_INT / 1000) > iValue);
							iValue *= 1000;
							iValue /= std::max(1, (iTurnsLeft + 10));
						}

						iValue = std::max(1, iValue);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestProject = ((ProjectTypes)iI);
						}
					}
				}
			}
		}
	}

	return eBestProject;
}


int CvCityAI::AI_projectValue(ProjectTypes eProject)
{
	int iValue = 0;

	if (GC.getProjectInfo(eProject).getNukeInterception() > 0)
	{
		if (GC.getGameINLINE().canTrainNukes())
		{
			iValue += (GC.getProjectInfo(eProject).getNukeInterception() / 10);
		}
	}

	if (GC.getProjectInfo(eProject).getTechShare() > 0)
	{
		if (GC.getProjectInfo(eProject).getTechShare() < GET_TEAM(getTeam()).getHasMetCivCount(true))
		{
			iValue += (20 / GC.getProjectInfo(eProject).getTechShare());
		}
	}

	for (int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		if (GC.getGameINLINE().isVictoryValid((VictoryTypes)iI))
		{
			iValue += (std::max(0, (GC.getProjectInfo(eProject).getVictoryThreshold(iI) - GET_TEAM(getTeam()).getProjectCount(eProject))) * 20);
		}
	}

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		iValue += (std::max(0, (GC.getProjectInfo((ProjectTypes)iI).getProjectsNeeded(eProject) - GET_TEAM(getTeam()).getProjectCount(eProject))) * 10);
	}
	
/************************************************************************************************/
/* Afforess	                  Start		 01/02/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	CvProjectInfo& kProject = GC.getProjectInfo(eProject);	//New Project Valuations Begin
	int iOurNumCities = GET_PLAYER(getOwnerINLINE()).getNumCities();
	int iOurHappy = kProject.getGlobalHappiness();
	int iOurHealth = kProject.getGlobalHealth();
	int iWorldHappy = kProject.getWorldHappiness();
	int iWorldHealth = kProject.getWorldHealth();
	int iWorldTradeRoutes = kProject.getWorldTradeRoutes();
	
	if (iOurHappy != 0)
		iValue += 4 * iOurNumCities * iOurHappy;
	if (iOurHealth != 0)
		iValue += 4 * iOurNumCities * iOurHealth;
		
	iValue += ((iWorldTradeRoutes * 8 * (getPopulation() / 5 + 1)));
	
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iValue += ((kProject.getCommerceModifier(iI) * iOurNumCities) / 4);
	}
	
	int iTempValue = 0;
	if ((iWorldHappy != 0) && (iWorldHealth != 0))
	{
		iTempValue += 4 * iOurNumCities * iWorldHappy;
		iTempValue += 4 * iOurNumCities * iWorldHealth;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && !GET_PLAYER((PlayerTypes)iI).isMinorCiv() && GET_PLAYER((PlayerTypes)iI).getID() != GET_PLAYER(getOwnerINLINE()).getID())
			{
				int iNumCities = GET_PLAYER((PlayerTypes)iI).getNumCities();
				int iAttitude = GET_PLAYER(getOwnerINLINE()).AI_getAttitude((PlayerTypes)iI);
				switch(iAttitude)
				{
					case ATTITUDE_FURIOUS:
						iTempValue -= 2 * iNumCities * iWorldHealth;
						iTempValue -= 2 * iNumCities * iWorldHappy;
						iTempValue -= iWorldTradeRoutes * 8;
						break;

					case ATTITUDE_ANNOYED:
						iTempValue -= iNumCities * iWorldHealth;
						iTempValue -= iNumCities * iWorldHappy;
						iTempValue -= iWorldTradeRoutes * 4;
						break;

					case ATTITUDE_CAUTIOUS:
						iTempValue -= (iNumCities  * iWorldHealth / 3);
						iTempValue -= (iNumCities  * iWorldHappy/ 3);
						iTempValue -= iWorldTradeRoutes;
						break;

					case ATTITUDE_PLEASED:
						iTempValue += iNumCities * iWorldHealth;
						iTempValue += iNumCities * iWorldHappy;
						iTempValue += iWorldTradeRoutes * 4;
						break;

					case ATTITUDE_FRIENDLY:
						iTempValue += 2 * iNumCities * iWorldHealth;
						iTempValue += 2 * iNumCities * iWorldHappy;
						iTempValue += iWorldTradeRoutes * 8;
						break;

					default:
						FAssert(false);
						break;
				}
			}
		}
	}
	iValue += iTempValue;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


	return iValue;
}

ProcessTypes CvCityAI::AI_bestProcess(CommerceTypes eCommerceType, int* commerceWeights)
{
	int iBestValue = 0;
	ProcessTypes eBestProcess = NO_PROCESS;

	for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		if (canMaintain((ProcessTypes)iI))
		{
			int iValue = AI_processValue((ProcessTypes)iI, eCommerceType, commerceWeights);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestProcess = ((ProcessTypes)iI);
			}
		}
	}

	return eBestProcess;
}

int CvCityAI::AI_processValue(ProcessTypes eProcess, CommerceTypes eCommerceType, int* commerceWeights)
{
	
	int iValue = 0;

	if (GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
	{
		iValue += GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_GOLD);
	}

	// if we own less than 50%, or we need to pop borders
	if ((plot()->calculateCulturePercent(getOwnerINLINE()) < 50) || (getCultureLevel() <= (CultureLevelTypes) 1))
	{
		iValue += GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_CULTURE);
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/30/09                                jdog5000      */
/*                                                                                              */
/* Cultural Victory AI                                                                          */
/************************************************************************************************/
	if ( GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3) )
	{
		// Final city for cultural victory will build culture to speed up victory
		if( findCommerceRateRank(COMMERCE_CULTURE) == GC.getGameINLINE().culturalVictoryNumCultureCities() )
		{
			iValue += 2*GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_CULTURE);
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool bValid = (eCommerceType == NO_COMMERCE);
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		int iTempValue = GC.getProcessInfo(eProcess).getProductionToCommerceModifier((CommerceTypes)iI);
		if (!bValid && ((CommerceTypes)iI == eCommerceType) && (iTempValue > 0))
		{
			bValid = true;
			iTempValue *= 2;
		}

		iTempValue *= GET_PLAYER(getOwnerINLINE()).AI_commerceWeight(((CommerceTypes)iI), this);
		
		iTempValue /= 100;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      07/08/09                                jdog5000      */
/*                                                                                              */
/* Gold AI                                                                                      */
/************************************************************************************************/
		iTempValue *= GET_PLAYER(getOwnerINLINE()).AI_averageCommerceExchange((CommerceTypes)iI);

		iTempValue /= 60;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

		//	Koshling - take into account external factors determining which commerce types
		//	we WANT most right now
		if ( commerceWeights != NULL )
		{
			iTempValue *= commerceWeights[iI];
		}

		iValue += iTempValue;
	}

	return (bValid ? iValue : 0);
}

bool CvCityAI::AI_finalProcessSelection()
{
#ifdef USE_UNIT_TENDERING
	if ( m_bRequestedBuilding /*|| m_bRequestedUnit*/)//A city isn't necessarily going to train the unit it requested but allow any qualified building to immediately override this 'last check for a process'.
	{
		return false;
	}

	m_iBuildPriority = m_iTempBuildPriority;
#endif

	int iBestValue = 0;
	ProcessTypes eBestProcess = NO_PROCESS;

	for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		if (canMaintain((ProcessTypes)iI))
		{
			ProcessTypes eProcess = ((ProcessTypes)iI);
			int iValue = 1;
			if (GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
			{
				iValue += GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_GOLD);
			}

			// if we own less than 50%, or we need to pop borders
			if ((plot()->calculateCulturePercent(getOwnerINLINE()) < 50) || (getCultureLevel() <= (CultureLevelTypes) 1))
			{
				iValue += GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_CULTURE);
			}

			if ( GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3) )
			{
				// Final city for cultural victory will build culture to speed up victory
				if( findCommerceRateRank(COMMERCE_CULTURE) == GC.getGameINLINE().culturalVictoryNumCultureCities() )
				{
					iValue += 2*GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_CULTURE);
				}
			}

			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				CommerceTypes eCommerce = (CommerceTypes)iJ;
				int iTemp = GC.getProcessInfo(eProcess).getProductionToCommerceModifier(eCommerce);
				if (eCommerce == COMMERCE_RESEARCH && iTemp > 0)
				{
					iTemp *= 120;
					iTemp /= 100;
				}
				iValue += iTemp;
			}
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestProcess = eProcess;
			}
		}
	}
	if (eBestProcess != NO_PROCESS)
	{
		pushOrder(ORDER_MAINTAIN, eBestProcess, -1, false, false, false);
		return true;
	}

	return false;
}


int CvCityAI::AI_neededSeaWorkers()
{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/01/09                                jdog5000      */
/*                                                                                              */
/* Worker AI                                                                                    */
/************************************************************************************************/
	CvArea* pWaterArea = waterArea(true);
	
	if (pWaterArea == NULL)
	{
		return 0;
	}

	int iNeededSeaWorkers = GET_PLAYER(getOwnerINLINE()).countUnimprovedBonuses(pWaterArea);

	// Check if second water area city can reach was any unimproved bonuses
	pWaterArea = secondWaterArea();
	if (pWaterArea != NULL)
	{
		iNeededSeaWorkers += GET_PLAYER(getOwnerINLINE()).countUnimprovedBonuses(pWaterArea);
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	return iNeededSeaWorkers;
}


bool CvCityAI::AI_isDefended(int iExtra, bool bAllowAnyDefenders)
{
	PROFILE_FUNC();

	return getGarrisonStrength(bAllowAnyDefenders) + iExtra >= AI_neededDefenseStrength();
	//	Note - the isDefended range check is slightly flawed since its possible for units 2 away to not actually
	//	be able to get to the city (e.g. - over water) so this probably needs tweaking slightly
	//return ((plot()->plotCount(PUF_canDefend /*PUF_canDefendGroupHead*/, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType, -1, -1, iRange) + iExtra) >= AI_neededDefenders()); // XXX check for other team's units?
}

bool CvCityAI::AI_isAdequateHappinessMilitary(int iExtra)
{
	PROFILE_FUNC();

	return ((plot()->plotCount(PUF_isMilitaryHappiness, -1, -1, NULL, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType) + iExtra) >= AI_neededHappinessDefenders()); // XXX check for other team's units?
}

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						10/17/08		jdog5000		*/
/* 																			*/
/* 	Air AI																	*/
/********************************************************************************/
/* original BTS code
bool CvCityAI::AI_isAirDefended(int iExtra)
{
	PROFILE_FUNC();
	
	return ((plot()->plotCount(PUF_canAirDefend, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isDomainType, DOMAIN_AIR) + iExtra) >= AI_neededAirDefenders()); // XXX check for other team's units?
}
*/
// Function now answers question of whether city has enough ready air defense, no longer just counts fighters
bool CvCityAI::AI_isAirDefended(bool bCountLand, int iExtra)
{
	PROFILE_FUNC();
	
	int iAirIntercept = 0;
	int iLandIntercept = 0;

	CvPlot* pPlot = plot();
	for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
	{
		CvUnit* pLoopUnit = unitItr.ptr();

		if (pLoopUnit->getOwnerINLINE() == getOwnerINLINE() &&
			pLoopUnit->canAirDefend())
		{
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				// can find units which are already air patrolling using group activity
				if (pLoopUnit->getGroup()->getActivityType() == ACTIVITY_INTERCEPT)
				{
					iAirIntercept += pLoopUnit->currInterceptionProbability();
				}
				else
				{
					// Count air units which can air patrol
					if (pLoopUnit->getDamage() == 0 && !pLoopUnit->hasMoved())
					{
						if (pLoopUnit->AI_getUnitAIType() == UNITAI_DEFENSE_AIR)
						{
							iAirIntercept += pLoopUnit->currInterceptionProbability();
						}
						else
						{
							iAirIntercept += pLoopUnit->currInterceptionProbability() / 3;
						}
					}

				}
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				iLandIntercept += pLoopUnit->currInterceptionProbability();
			}
		}
	}

	int iAirDefenders = iExtra;
	iAirDefenders += (iAirIntercept/100);

	if( bCountLand )
	{
		iAirDefenders += (iLandIntercept/100);
	}

	int iNeededAirDefenders = AI_neededAirDefenders();
	bool bHaveEnough = (iAirDefenders >= iNeededAirDefenders);

	return bHaveEnough;
}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/


/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/25/10                                jdog5000      */
/*                                                                                              */
/* War strategy AI, Barbarian AI                                                                */
/************************************************************************************************/
int CvCityAI::AI_neededDefenders()
{
	PROFILE_FUNC();

	if ( m_neededDefenders != -1 )
	{
		return m_neededDefenders;
	}

	if (!(GET_TEAM(getTeam()).AI_isWarPossible()))
	{
		return 1;
	}

	if (isHominid())
	{
		int iDefenders = GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getBarbarianInitialDefenders();
		iDefenders += ((getPopulation() + 2) / 7);
		return std::max(1,iDefenders);
	}

	int iDefenders = 1;
	
	if (!GC.getGame().isEarlyGame() && (hasActiveWorldWonder() || isCapital() || isHolyCity()))
	{
		iDefenders++;

		if( GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_ALERT1) || GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_TURTLE) )
		{
			iDefenders++;
		}
	}
	
	if (!GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_CRUSH))
	{
		iDefenders += AI_neededFloatingDefenders();
	}
	else
	{
		iDefenders += (AI_neededFloatingDefenders() + 2) / 4;
	}

	bool bDefenseWar = ((area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE));
	if (bDefenseWar || GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_ALERT2))
	{
		if (!(plot()->isHills()))
		{
			iDefenders++;
		}
	}
	
	if (!GC.getGame().isEarlyGame() && (GC.getGame().getGameTurn() - getGameTurnAcquired()) < 10)
	{
/* original code
		if (bOffenseWar)
		{
			if (!hasActiveWorldWonder() && !isHolyCity())
			{
				iDefenders /= 2;
				iDefenders = std::max(1, iDefenders);
			}
		}		
	}
	
	if (GC.getGame().getGameTurn() - getGameTurnAcquired() < 10)
	{
		iDefenders = std::max(2, iDefenders);
		if (AI_isDanger())
		{
			iDefenders ++;
		}
		if (bDefenseWar)
		{
			iDefenders ++;
		}
*/
		iDefenders = std::max(2, iDefenders);

		bool bOffenseWar = ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
		if (bOffenseWar && getTotalDefense(true) > 0)
		{
			if (!hasActiveWorldWonder() && !isHolyCity())
			{
				iDefenders /= 2;
			}
		}		
		
		if (AI_isDanger())
		{
			iDefenders++;
		}
		if (bDefenseWar)
		{
			iDefenders++;
		}
	}
	
	if (GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_LAST_STAND))
	{
		iDefenders += 20;
	}

	if( GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3) )
	{
		if( findCommerceRateRank(COMMERCE_CULTURE) <= GC.getGameINLINE().culturalVictoryNumCultureCities() )
		{
			iDefenders += 4;

			if( bDefenseWar )
			{
				iDefenders += 2;
			}
		}
	}

	if( GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE3) )
	{
		if( isCapital() || isProductionProject())
		{
			iDefenders += 4;

			if( bDefenseWar )
			{
				iDefenders += 3;
			}
		}

		if( isCapital() && GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4) )
		{
			iDefenders += 6;
		}
	}
/************************************************************************************************/
/* RevolutionDCM Revolution AI          02/03/10                               Afforess         */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_REVOLUTION))
	{
		int iRevIndex = getRevolutionIndex();
		int iExtraRevDefenders = std::max(0, ((int)(std::pow((float)iRevIndex, 0.35f) - 6.5f)));
		iDefenders += iExtraRevDefenders;
		logBBAI("      City %S extra revolution defenders needed: %d", getName().GetCString(), iExtraRevDefenders);
	}
/************************************************************************************************/
/* RevolutionDCM	                     END                                                    */
/************************************************************************************************/
	
	int iTempDefenders = 0;
	int iHappiness = happyLevel() - unhappyLevel(0);
	if (GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit() > 0)
	{
		if (iHappiness < 0)
		{
			iTempDefenders -= GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit() * iHappiness;
		}
	}
	else if (GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit() < 0)
	{
		if (iHappiness < 0)
		{
			iTempDefenders -= GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit() * iHappiness;
		}
	}
	iTempDefenders /= (1 + GET_TEAM(getTeam()).getAtWarCount(true));
	iDefenders += iTempDefenders;
		
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


	iDefenders = std::max(iDefenders, AI_minDefenders());

	m_neededDefenders = iDefenders;

	return iDefenders;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

int CvCityAI::AI_neededHappinessDefenders()
{
	int iResult = 0;
	if (GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit() > 0)
	{
		int iCurrent = plot()->plotCount(PUF_isMilitaryHappiness, -1, -1, NULL, getOwnerINLINE(), NO_TEAM);
		int iHappiness = happyLevel() - unhappyLevel(0);
		int iBaseHappiness = iHappiness - GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit() * iCurrent;

		if (iBaseHappiness < 0)
		{
			iResult = -iBaseHappiness/GET_PLAYER(getOwnerINLINE()).getHappyPerMilitaryUnit();
		}
	}

	return iResult;
}

int CvCityAI::AI_minDefenders()
{
	int iDefenders = 4;

	//TB Option Control:
	if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		iDefenders += 3;
		//Intention is to drive the amount up to the amount needed to start merging/splitting to get at least one really tough defender.
		//May slow the AI a bit and get them building a lot of units but try to attack them and see how far we get!
	}

	int iEra = GET_PLAYER(getOwnerINLINE()).getCurrentEra();
	if (iEra > 0)
	{
		iDefenders++;
		//TB testing:
		iDefenders += (iEra/3);
	}
	if (((iEra - GC.getGame().getStartEra() / 2) >= GC.getNumEraInfos() / 2) && isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		iDefenders++;
	}

	return iDefenders;
}

int CvCityAI::getGarrisonStrength(bool bAllowAnyDefenders) const
{
	PROFILE_FUNC();

	//	Note - the range check is slightly flawed since its possible for units 2 away to not actually
	//	be able to get to the city (e.g. - over water) so this probably needs tweaking slightly
	return plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isCityGarrison, getID(), (int)bAllowAnyDefenders, 2);
}

int CvCityAI::AI_neededDefenseStrength()
{
	//	For now we just use the old calculation for needed defenders (a count)
	//	and multiply it by the base strength of the best defender we can currently build
	return AI_neededDefenders() * GET_PLAYER(getOwnerINLINE()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE);
}

int CvCityAI::AI_minDefenseStrength()
{
	//	For now we just use the old calculation for needed defenders (a count)
	//	and multiply it by the base strength of the best defender we can currently build
	return AI_minDefenders() * GET_PLAYER(getOwnerINLINE()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE);
}

int CvCityAI::AI_neededFloatingDefenders()
{
	if (m_iNeededFloatingDefendersCacheTurn != GC.getGame().getGameTurn())
	{
		AI_updateNeededFloatingDefenders();
	}
	return m_iNeededFloatingDefenders;	
}

void CvCityAI::AI_updateNeededFloatingDefenders()
{
	int iFloatingDefenders = GET_PLAYER(getOwnerINLINE()).AI_getTotalFloatingDefendersNeeded(area());
		
	int iTotalThreat = std::max(1, GET_PLAYER(getOwnerINLINE()).AI_getTotalAreaCityThreat(area()));
	
	int iThreatModifier = 0;

	//	Use the threat modifier (which is based on danger from higher-difficulty setting players
	//	which usually means humans!) to increase local floating defender needs
	iFloatingDefenders *= AI_cityThreat(NO_TEAM, &iThreatModifier);
	iFloatingDefenders = (iFloatingDefenders*(100+iThreatModifier))/100;
	iFloatingDefenders += (iTotalThreat / 2);
	iFloatingDefenders /= iTotalThreat;
	
	m_iNeededFloatingDefenders = iFloatingDefenders;
	m_iNeededFloatingDefendersCacheTurn = GC.getGame().getGameTurn();
}

int CvCityAI::AI_neededAirDefenders()
{
	if (!(GET_TEAM(getTeam()).AI_isWarPossible()))
	{
		return 0;
	}

	int iDefenders = 0;

	int iRange = 5;

	int iOtherTeam = 0;
	int iEnemyTeam = 0;
	for (int iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (int iDY = -(iRange); iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if ((pLoopPlot != NULL) && pLoopPlot->isOwned() && (pLoopPlot->getTeam() != getTeam()))
			{
				iOtherTeam++;
				if (GET_TEAM(getTeam()).AI_getWarPlan(pLoopPlot->getTeam()) != NO_WARPLAN)
				{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/01/09                                jdog5000      */
/*                                                                                              */
/* Air AI                                                                                       */
/************************************************************************************************/
					// If enemy has no bombers, don't need to defend as much
					if( GET_PLAYER(pLoopPlot->getOwner()).AI_totalUnitAIs(UNITAI_ATTACK_AIR) == 0 )
					{
						continue;
					}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
					iEnemyTeam += 2;
					if (pLoopPlot->isCity())
					{
						iEnemyTeam += 6;
					}
				}
			}
		}
	}
	
	iDefenders += (iOtherTeam + iEnemyTeam + 2) / 8;

	iDefenders = std::min((iEnemyTeam > 0) ? 4 : 2, iDefenders);
	
	if (iDefenders == 0)
	{
		if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			iDefenders++;
		}
	}
	return iDefenders;
}

#define	MAX_DANGER_EVALUATION_RANGE		6
#define	DISTANCE_ATTENUATION_FACTOR(x)	((2*(x))/5)
#define	NEUTRAL_BORDER_PLOT_DANGER		5
#define	OWNED_PLOT_NON_WAR_BASE_DANGER	10
#define	OWNED_PLOT_WAR_DANGER			30

//	Koshling - provide a numerical evaluation of city danger from 0 (very safe) to 100 (about to be lost)
int CvCityAI::evaluateDanger()
{
	PROFILE_FUNC();

	CvUnitSelectionCriteria	noGrowthCriteria;

	noGrowthCriteria.m_bIgnoreGrowth = true;

	UnitTypes				eDummyUnit = GET_PLAYER(getOwnerINLINE()).bestBuildableUnitForAIType(DOMAIN_LAND, UNITAI_ATTACK, &noGrowthCriteria);

	if ( eDummyUnit == NO_UNIT )
	{
		eDummyUnit = GET_PLAYER(getOwnerINLINE()).bestBuildableUnitForAIType(DOMAIN_LAND, UNITAI_CITY_DEFENSE, &noGrowthCriteria);

		FAssert(eDummyUnit != NO_UNIT);

		if ( eDummyUnit == NO_UNIT )
		{
			OutputDebugString("Cannot find defender to use for strength test\n");
		}
	}

	if ( eDummyUnit != NO_UNIT )
	{
		CvUnit*				pTempUnit = GET_PLAYER(getOwnerINLINE()).getTempUnit(eDummyUnit, getX_INLINE(), getY_INLINE());
		CvReachablePlotSet	plotSet(pTempUnit->getGroup(), 0, MAX_DANGER_EVALUATION_RANGE, false, 1);
		int					iBorderDanger = 0;
		//int					iGeneralDanger = 0;
		int					iDistanceModifier[MAX_DANGER_EVALUATION_RANGE];

		iDistanceModifier[0] = 1000;
		for(int iI = 1; iI < MAX_DANGER_EVALUATION_RANGE; iI++)
		{
			iDistanceModifier[iI] = DISTANCE_ATTENUATION_FACTOR(iDistanceModifier[iI-1]);
		}

		for(CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
		{
			CvPlot*		pLoopPlot = itr.plot();
			TeamTypes	ePlotTeam = pLoopPlot->getTeam();

			// AIAndy: This sums up iGeneralDanger but never uses it. Looks incorrect for city danger evaluation.
			//iGeneralDanger += pLoopPlot->getDangerCount(getOwnerINLINE());

			if ( ePlotTeam != getTeam() )
			{
				if ( ePlotTeam == NO_TEAM )
				{
					//	Neutral territory border
					iBorderDanger += NEUTRAL_BORDER_PLOT_DANGER*iDistanceModifier[itr.stepDistance()-1];
				}
				else
				{
					if ( GET_TEAM(getTeam()).isAtWar(ePlotTeam) )
					{
						//	Border with someone we are at war with
						iBorderDanger += OWNED_PLOT_WAR_DANGER*iDistanceModifier[itr.stepDistance()-1];
					}
					else
					{
						int iModifier;
						int iTempBorderDanger;

						//	Border with a player we are not at war with - weight by attitude
						iTempBorderDanger = (OWNED_PLOT_NON_WAR_BASE_DANGER*getPlayerDangerPercentage(pLoopPlot->getOwnerINLINE(), iModifier)*iDistanceModifier[itr.stepDistance()-1])/100;
						iTempBorderDanger = (iTempBorderDanger*(100+iModifier))/100;

						iBorderDanger += iTempBorderDanger;
					}
				}
			}
		}

		GET_PLAYER(getOwnerINLINE()).releaseTempUnit();

		return iBorderDanger/100;
	}
	else
	{
		//	Should never happen but empirically it does (very rarely) - needs future investigation
		return 100;	
	}
}

bool CvCityAI::AI_isDanger()
{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/20/09                                jdog5000      */
/*                                                                                              */
/* City AI, Efficiency                                                                          */
/************************************************************************************************/
	//return GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 2, false);
	return GET_PLAYER(getOwnerINLINE()).AI_getAnyPlotDanger(plot(), 2, false);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/	
}


int CvCityAI::AI_getEmphasizeAvoidGrowthCount()
{
	return m_iEmphasizeAvoidGrowthCount;
}


bool CvCityAI::AI_isEmphasizeAvoidGrowth()
{
	return (AI_getEmphasizeAvoidGrowthCount() > 0);
}


int CvCityAI::AI_getEmphasizeGreatPeopleCount()
{
	return m_iEmphasizeGreatPeopleCount;
}


bool CvCityAI::AI_isEmphasizeGreatPeople()
{
	return (AI_getEmphasizeGreatPeopleCount() > 0);
}


bool CvCityAI::AI_isAssignWorkDirty()
{
	return m_bAssignWorkDirty;
}


void CvCityAI::AI_setAssignWorkDirty(bool bNewValue)
{
	m_bAssignWorkDirty = bNewValue;
}


bool CvCityAI::AI_isChooseProductionDirty()
{
	return m_bChooseProductionDirty;
}


void CvCityAI::AI_setChooseProductionDirty(bool bNewValue)
{
	m_bChooseProductionDirty = bNewValue;
}


CvCity* CvCityAI::AI_getRouteToCity() const
{
	if ( !m_routeToCityUpdated )
	{
		AI_updateRouteToCity();

		m_routeToCityUpdated = true;
	}

	return getCity(m_routeToCity);
}


void CvCityAI::AI_updateRouteToCity() const
{
	PROFILE_FUNC();

	static PlayerTypes eCachedPlayer = NO_PLAYER;
	static stdext::hash_map<int,bool>* allAreaCitiesConnected = NULL;

	if ( allAreaCitiesConnected == NULL )
	{
		allAreaCitiesConnected = new stdext::hash_map<int,bool>();
	}

	if ( eCachedPlayer != getOwnerINLINE() )
	{
		eCachedPlayer = getOwnerINLINE();
		allAreaCitiesConnected->clear();
	}
	
	CvCity* pBestCity = NULL;
	stdext::hash_map<int,bool>::const_iterator itr;
	if ( (itr = allAreaCitiesConnected->find(area()->getID())) == allAreaCitiesConnected->end() ||
		 !(itr->second) )
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getRouteFinder());

		int iBestValue = MAX_INT;
		pBestCity = NULL;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				int iLoop = 0;
				for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					if (pLoopCity != this)
					{
						if (pLoopCity->area() == area())
						{
							if (!(gDLL->getFAStarIFace()->GeneratePath(&GC.getRouteFinder(), getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), false, getOwnerINLINE(), true)))
							{
								int iValue = plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());

								if (iValue < iBestValue)
								{
									iBestValue = iValue;
									pBestCity = pLoopCity;
								}
							}
						}
					}
				}
			}
		}

		if ( itr == allAreaCitiesConnected->end() )
		{
			(*allAreaCitiesConnected)[area()->getID()] = (pBestCity == NULL);
		}
	}

	if (pBestCity != NULL)
	{
		m_routeToCity = pBestCity->getIDInfo();
	}
	else
	{
		m_routeToCity.reset();
	}
}


int CvCityAI::AI_getEmphasizeYieldCount(YieldTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEmphasizeYieldCount[eIndex];
}


bool CvCityAI::AI_isEmphasizeYield(YieldTypes eIndex)
{
	return (AI_getEmphasizeYieldCount(eIndex) > 0);
}


int CvCityAI::AI_getEmphasizeCommerceCount(CommerceTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (m_aiEmphasizeCommerceCount[eIndex] > 0);
}


bool CvCityAI::AI_isEmphasizeCommerce(CommerceTypes eIndex)
{
	return (AI_getEmphasizeCommerceCount(eIndex) > 0);
}


bool CvCityAI::AI_isEmphasize(EmphasizeTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumEmphasizeInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_pbEmphasize != NULL, "m_pbEmphasize is not expected to be equal with NULL");
	return m_pbEmphasize[eIndex];
}

bool CvCityAI::AI_isEmphasizeSpecialist(SpecialistTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_pbEmphasizeSpecialist != NULL, "m_pbEmphasize is not expected to be equal with NULL");
	return m_pbEmphasizeSpecialist[eIndex];
}


void CvCityAI::AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumEmphasizeInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (AI_isEmphasize(eIndex) != bNewValue)
	{
		m_pbEmphasize[eIndex] = bNewValue;

		if (GC.getEmphasizeInfo(eIndex).isAvoidGrowth())
		{
			m_iEmphasizeAvoidGrowthCount += ((AI_isEmphasize(eIndex)) ? 1 : -1);
			FAssert(AI_getEmphasizeAvoidGrowthCount() >= 0);
		}

		if (GC.getEmphasizeInfo(eIndex).isGreatPeople())
		{
			m_iEmphasizeGreatPeopleCount += ((AI_isEmphasize(eIndex)) ? 1 : -1);
			FAssert(AI_getEmphasizeGreatPeopleCount() >= 0);
		}
/************************************************************************************************/
/* Afforess	                  Start		 02/10/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		if (GC.getEmphasizeInfo(eIndex).isAvoidAngryCitizens())
		{
			m_iEmphasizeAvoidAngryCitizensCount += ((AI_isEmphasize(eIndex)) ? 1 : -1);
			FAssert(AI_getEmphasizeAvoidAngryCitizensCount() >= 0);
		}

		if (GC.getEmphasizeInfo(eIndex).isAvoidUnhealthyCitizens())
		{
			m_iEmphasizeAvoidUnhealthyCitizensCount += ((AI_isEmphasize(eIndex)) ? 1 : -1);
			FAssert(AI_getEmphasizeAvoidUnhealthyCitizensCount() >= 0);
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (GC.getEmphasizeInfo(eIndex).getYieldChange(iI))
			{
				m_aiEmphasizeYieldCount[iI] += ((AI_isEmphasize(eIndex)) ? 1 : -1);
				FAssert(AI_getEmphasizeYieldCount((YieldTypes)iI) >= 0);
			}
		}

		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			if (GC.getEmphasizeInfo(eIndex).getCommerceChange(iI))
			{
				m_aiEmphasizeCommerceCount[iI] += ((AI_isEmphasize(eIndex)) ? 1 : -1);
				FAssert(AI_getEmphasizeCommerceCount((CommerceTypes)iI) >= 0);
			}
		}

		AI_assignWorkingPlots();

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}

		//	If we're using AI govenors and not part way through a build reflect
		//	the changes in a new production choice immediately
		if (isHuman() && (!isProduction() || getProduction() == 0) && isProductionAutomated() &&	GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_3))
		{
			AI_chooseProduction();
		}
	}
}

void CvCityAI::AI_setEmphasizeSpecialist(SpecialistTypes eIndex, bool bNewValue)
{

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (AI_isEmphasizeSpecialist(eIndex) != bNewValue)
	{
		m_pbEmphasizeSpecialist[eIndex] = bNewValue;

		AI_assignWorkingPlots();

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

void CvCityAI::AI_forceEmphasizeCulture(bool bNewValue)
{
	if (m_bForceEmphasizeCulture != bNewValue)
	{
		m_bForceEmphasizeCulture = bNewValue;

		m_aiEmphasizeCommerceCount[COMMERCE_CULTURE] += (bNewValue ? 1 : -1);
		FAssert(m_aiEmphasizeCommerceCount[COMMERCE_CULTURE] >= 0);
	}
}


int CvCityAI::AI_getBestBuildValue(int iIndex)
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiBestBuildValue[iIndex];
}


int CvCityAI::AI_totalBestBuildValue(CvArea* pArea)
{
	int iTotalValue = 0;

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pArea)
				{
					if ((pLoopPlot->getImprovementType() == NO_IMPROVEMENT) || !(GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION) && !(pLoopPlot->getImprovementType() == (GC.getDefineINT("RUINS_IMPROVEMENT"))) && !(GC.getImprovementInfo((ImprovementTypes)pLoopPlot->getImprovementType()).isDepletedMine())))
					{
						iTotalValue += AI_getBestBuildValue(iI);
					}
				}
			}
		}
	}

	return iTotalValue;
}

int CvCityAI::AI_clearFeatureValue(int iIndex)
{
	CvPlot* pPlot = plotCity(getX_INLINE(), getY_INLINE(), iIndex);
	FAssert(pPlot != NULL);
	
	FeatureTypes eFeature = pPlot->getFeatureType();
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*       CTD Fix                                                                                */
/************************************************************************************************/
	if (eFeature == NO_FEATURE)
		return 0;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	FAssert(eFeature != NO_FEATURE);
	
	CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);
	
	int iValue = 0;
	iValue += kFeatureInfo.getYieldChange(YIELD_FOOD) * 100;
	iValue += kFeatureInfo.getYieldChange(YIELD_PRODUCTION) * 60;
	iValue += kFeatureInfo.getYieldChange(YIELD_COMMERCE) * 40;
	
	if (iValue > 0 && pPlot->isBeingWorked())
	{
		iValue *= 3;
		iValue /= 2;
	}
	if (iValue != 0)
	{
		BonusTypes eBonus = pPlot->getBonusType(getTeam());
		if (eBonus != NO_BONUS)
		{
			iValue *= 3;
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eBonus))
				{
					iValue *= 4;
				}
			}
		}
	}
	
	int iHealthValue = 0;
	if (kFeatureInfo.getHealthPercent() != 0)
	{
		int iHealth = goodHealth() - badHealth();
		
/************************************************************************************************/
/* Afforess	                  Start		 06/17/10                                               */
/************************************************************************************************/
/*
		iHealthValue += (6 * kFeatureInfo.getHealthPercent()) / std::max(3, 1 + iHealth);
*/
	//speed up Jungle Clearing
		int iMultiplier = kFeatureInfo.getHealthPercent() > 0 ? 6 : 10;
		iHealthValue += (iMultiplier * kFeatureInfo.getHealthPercent()) / std::max(3, 1 + iHealth);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		if (iHealthValue > 0 && !pPlot->isBeingWorked())
		{
			iHealthValue *= 3;
			iHealthValue /= 2;
		}
	}
	iValue += iHealthValue;
	
	if (iValue > 0)
	{
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(pPlot->getImprovementType()).isRequiresFeature())
			{
				iValue += 500;
			}
		}
		
		if (GET_PLAYER(getOwnerINLINE()).getAdvancedStartPoints() >= 0)
		{
			iValue += 400;
		}
	}
	
	return -iValue;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      07/25/09                                jdog5000      */
/*                                                                                              */
/* Debug                                                                                        */
/************************************************************************************************/
int CvCityAI::AI_getGoodTileCount()
{
	int aiFinalYields[NUM_YIELD_TYPES];

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	int iGoodTileCount = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot*  pLoopPlot = getCityIndexPlot(iI);

			if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
			{
				int iCount = (kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD));

				bool bUseBaseValue = true;
				//If the tile has a BestBuild and is being improved, then use the BestBuild
				//determine if the tile is being improved.

				if (iCount > 0)
				{
					BuildTypes eBuild = NO_BUILD;
					if (m_aeBestBuild[iI] != NO_BUILD && m_aiBestBuildValue[iI] > 0)
					{
						eBuild = m_aeBestBuild[iI];
					}
					else
					{
						// This check is necessary to stop oscillation which can result
						// when best build changes food situation for city, changing the best build.
						for (CvPlot::unit_iterator unitItr = pLoopPlot->beginUnits(); unitItr != pLoopPlot->endUnits(); ++unitItr)
						{
							CvUnit* pLoopUnit = unitItr.ptr();
							if (unitItr->getBuildType() != NO_BUILD)
							{
								if( eBuild == NO_BUILD || pLoopPlot->getBuildTurnsLeft(eBuild,0,0) > pLoopPlot->getBuildTurnsLeft(unitItr->getBuildType(),0,0) )
								{
									eBuild = unitItr->getBuildType();
								}
							}
						}
					}

					if( eBuild != NO_BUILD)
					{
						ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
						if (eImprovement != NO_IMPROVEMENT)
						{
							bool bIgnoreFeature = false;
							if (pLoopPlot->getFeatureType() != NO_FEATURE)
							{
								if (GC.getBuildInfo(eBuild).isFeatureRemove(pLoopPlot->getFeatureType()))
								{
									bIgnoreFeature = true;
								}
							}

							bUseBaseValue = false;
							for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								aiFinalYields[iJ] = (pLoopPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature) + pLoopPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
							}
						}
					}
				}

				//Otherwise use the base value.
				if (bUseBaseValue)
				{
					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						//by default we'll use the current value
						aiFinalYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
						if (pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							aiFinalYields[iJ] += std::max(0, -GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange((YieldTypes)iJ));
						}
					}
				}
				
				if ( ((aiFinalYields[YIELD_FOOD]*10) + (aiFinalYields[YIELD_PRODUCTION]*6) + (aiFinalYields[YIELD_COMMERCE]*4)) > 21 )
				{
					iGoodTileCount++;
				}
			}
		}
	}

	return iGoodTileCount;
}

int CvCityAI::AI_countWorkedPoorTiles()
{
	int aiFinalYields[NUM_YIELD_TYPES];

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	int iWorkedPoorTileCount = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iI);

			if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this && pLoopPlot->isBeingWorked())
			{
				int iCount = (kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD));

				bool bUseBaseValue = true;
				//If the tile has a BestBuild and is being improved, then use the BestBuild
				//determine if the tile is being improved.

				if (iCount > 0)
				{
					BuildTypes eBuild = NO_BUILD;
					if (m_aeBestBuild[iI] != NO_BUILD && m_aiBestBuildValue[iI] > 0)
					{
						eBuild = m_aeBestBuild[iI];
					}
					else
					{
						// This check is necessary to stop oscillation which can result
						// when best build changes food situation for city, changing the best build.
						for (CvPlot::unit_iterator unitItr = pLoopPlot->beginUnits(); unitItr != pLoopPlot->endUnits(); ++unitItr)
						{
							if (unitItr->getBuildType() != NO_BUILD)
							{
								if( eBuild == NO_BUILD || pLoopPlot->getBuildTurnsLeft(eBuild,0,0) > pLoopPlot->getBuildTurnsLeft(unitItr->getBuildType(),0,0) )
								{
									eBuild = unitItr->getBuildType();
								}
							}
						}
					}

					if( eBuild != NO_BUILD )
					{
						ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
						if (eImprovement != NO_IMPROVEMENT)
						{
							bool bIgnoreFeature = false;
							if (pLoopPlot->getFeatureType() != NO_FEATURE)
							{
								if (GC.getBuildInfo(eBuild).isFeatureRemove(pLoopPlot->getFeatureType()))
								{
									bIgnoreFeature = true;
								}
							}

							bUseBaseValue = false;
							for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								aiFinalYields[iJ] = (pLoopPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature) + pLoopPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
							}
						}
					}
				}

				//Otherwise use the base value.
				if (bUseBaseValue)
				{
					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						//by default we'll use the current value
						aiFinalYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
						if (pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							aiFinalYields[iJ] += std::max(0, -GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange((YieldTypes)iJ));
						}
					}
				}
				
				if ( ((aiFinalYields[YIELD_FOOD]*10) + (aiFinalYields[YIELD_PRODUCTION]*6) + (aiFinalYields[YIELD_COMMERCE]*4)) <= 21 )
				{
					iWorkedPoorTileCount++;
				}
			}
		}
	}

	return iWorkedPoorTileCount;
}

int CvCityAI::AI_getTargetSize()
{
	PROFILE_FUNC();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	int iTargetSize = AI_getGoodTileCount();
	
/********************************************************************************/
/*	Better Evaluation							09.03.2010		Fuyu		    */
/********************************************************************************/
	int iHappyAdjust = 0;
	int iHealthAdjust = 0;
	if (getProductionBuilding() != NO_BUILDING)
	{
		iHappyAdjust += getAdditionalHappinessByBuilding(getProductionBuilding());
		iHealthAdjust += getAdditionalHealthByBuilding(getProductionBuilding());
	}
/********************************************************************************/
/*	BE	END																		*/
/********************************************************************************/
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      09/02/10                         jdog5000 & Fuyu      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
	//iTargetSize = std::min(iTargetSize, 2 + getPopulation() + goodHealth() - badHealth() + getEspionageHealthCounter() + std::max(0, iHealthAdjust));
	iTargetSize -= std::max(0, (iTargetSize - (1 + getPopulation() + goodHealth() - badHealth() + getEspionageHealthCounter() + std::max(0, iHealthAdjust))) / 2);
	
	if( iTargetSize < getPopulation() )
	{
		iTargetSize = std::max(iTargetSize, getPopulation() - (AI_countWorkedPoorTiles()/2));
	}
	
	// Target city size should not be perturbed by espionage, other short term effects
	iTargetSize = std::min(iTargetSize, getPopulation() + (happyLevel() - unhappyLevel() + getEspionageHappinessCounter() + std::max(0, iHappyAdjust)));
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if (kPlayer.getAdvancedStartPoints() >= 0)
	{
		iTargetSize += 2 + kPlayer.getCurrentEra();
	}

	return iTargetSize;
}


void CvCityAI::AI_getYieldMultipliers( int &iFoodMultiplier, int &iProductionMultiplier, int &iCommerceMultiplier, int &iDesiredFoodChange )
{
	iFoodMultiplier = 100;
	iCommerceMultiplier = 100;
	iProductionMultiplier = 100;
	iDesiredFoodChange = 0;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	int aiFinalYields[NUM_YIELD_TYPES];

	int iBonusFoodSurplus = 0;
	int iBonusFoodDeficit = 0;
	int iFeatureFoodSurplus = 0;
	int iHillFoodDeficit = 0;
	int iFoodTotal = GC.getYieldInfo(YIELD_FOOD).getMinCity();
	int iProductionTotal = GC.getYieldInfo(YIELD_PRODUCTION).getMinCity();

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iI);

			if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
			{
				int iCount = (kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD));

				bool bUseBaseValue = true;
				//If the tile has a BestBuild or is being improved, then use the BestBuild
				//determine if the tile is being improved.
				if (iCount > 0)
				{
					BuildTypes eBuild = NO_BUILD;
					if (m_aeBestBuild[iI] != NO_BUILD && m_aiBestBuildValue[iI] > 0)
					{
						eBuild = m_aeBestBuild[iI];
					}
					else
					{
						// This check is necessary to stop oscillation which can result
						// when best build changes food situation for city.
						for (CvPlot::unit_iterator unitItr = pLoopPlot->beginUnits(); unitItr != pLoopPlot->endUnits(); ++unitItr)
						{
							if (unitItr->getBuildType() != NO_BUILD)
							{
								if( eBuild == NO_BUILD || pLoopPlot->getBuildTurnsLeft(eBuild,0,0) > pLoopPlot->getBuildTurnsLeft(unitItr->getBuildType(),0,0) )
								{
									eBuild = unitItr->getBuildType();
								}
							}
						}
					}

					if( eBuild != NO_BUILD )
					{
						ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
						if (eImprovement != NO_IMPROVEMENT)
						{
							bool bIgnoreFeature = false;
							if (pLoopPlot->getFeatureType() != NO_FEATURE)
							{
								if (GC.getBuildInfo(eBuild).isFeatureRemove(pLoopPlot->getFeatureType()))
								{
									bIgnoreFeature = true;
								}
							}								

							bUseBaseValue = false;
							for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								aiFinalYields[iJ] = (pLoopPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature) + pLoopPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
							}
						}
					}
				}

				//Otherwise use the base value.
				if (bUseBaseValue)
				{
					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						//by default we'll use the current value
						aiFinalYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
						if (pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							aiFinalYields[iJ] += std::max(0, -GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange((YieldTypes)iJ));
						}
					}
				}

				if (pLoopPlot->isBeingWorked() || (((aiFinalYields[YIELD_FOOD]*10) + (aiFinalYields[YIELD_PRODUCTION]*6) + (aiFinalYields[YIELD_COMMERCE]*4)) > 21))
				{
					if (pLoopPlot->isBeingWorked())
					{
						iFoodTotal += aiFinalYields[YIELD_FOOD];
					}
					else
					{
						iFoodTotal += aiFinalYields[YIELD_FOOD] / 2;
					}
					if (aiFinalYields[YIELD_PRODUCTION] > 1)
					{	
						iProductionTotal += aiFinalYields[YIELD_PRODUCTION];
					}
				}

				if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				{
					int iNetFood = (aiFinalYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION());
					iBonusFoodSurplus += std::max(0, iNetFood);
					iBonusFoodDeficit += std::max(0, -iNetFood);
				}

				if ((pLoopPlot->getFeatureType()) != NO_FEATURE)
				{
					iFeatureFoodSurplus += std::max(0, pLoopPlot->calculateNatureYield(YIELD_FOOD, getTeam()) - GC.getFOOD_CONSUMPTION_PER_POPULATION());
				}

				if ((pLoopPlot->isHills()))
				{
					iHillFoodDeficit += std::max(0, GC.getFOOD_CONSUMPTION_PER_POPULATION() - pLoopPlot->calculateNatureYield(YIELD_FOOD, getTeam()));
				}
			}
		}
	}


	int iBonusFoodDiff = ((iBonusFoodSurplus + iFeatureFoodSurplus) - (iBonusFoodDeficit + iHillFoodDeficit / 2));
	if (iBonusFoodDiff < 2)
	{
		iFoodMultiplier += 10 * (2 - iBonusFoodDiff);
	}

	int iHealth = goodHealth() - badHealth();
	int iTargetSize = AI_getTargetSize();

	//int iExtraFoodForGrowth = (std::max(0, iTargetSize - getPopulation()) + 3) / 4;
	int iExtraFoodForGrowth = (std::max(0, iTargetSize - getPopulation() + 3)) / 4; // K-Mod!!
	if (getPopulation() < iTargetSize)
	{
		iExtraFoodForGrowth ++;
	}

	int iFoodDifference = iFoodTotal - ((iTargetSize * GC.getFOOD_CONSUMPTION_PER_POPULATION()) + iExtraFoodForGrowth);
	
	iDesiredFoodChange = -iFoodDifference + std::max(0, -iHealth);
	if (iTargetSize > getPopulation())
	{
		if (iDesiredFoodChange > 3)
		{
			iDesiredFoodChange = (iDesiredFoodChange + 3) / 2;
		}
	}

	if (iFoodDifference > 4)
	{
		iFoodMultiplier -= 8 + 4 * iFoodDifference;
	}

	if (iFoodDifference < 0)
	{
		iFoodMultiplier +=  -iFoodDifference * 4;
	}

	if (iProductionTotal < 10)
	{
		iProductionMultiplier += (80 - 8 * iProductionTotal);
	}
	int iProductionTarget = 1 + (std::min(getPopulation(), (iTargetSize * 3) / 5));
	
	if (iProductionTotal < iProductionTarget)
	{
		iProductionMultiplier += 8 * (iProductionTarget - iProductionTotal);
	}

	if ((iBonusFoodSurplus + iFeatureFoodSurplus > 5) && ((iBonusFoodDeficit + iHillFoodDeficit) > 5))
	{
		if ((iBonusFoodDeficit + iHillFoodDeficit) > 8)
		{
			//probably a good candidate for a wonder pump
			iProductionMultiplier += 40;
			iCommerceMultiplier += (kPlayer.AI_isFinancialTrouble()) ? 0 : -40;
		}
	}

	int iNetCommerce = 1 + kPlayer.getCommerceRate(COMMERCE_GOLD) + kPlayer.getCommerceRate(COMMERCE_RESEARCH) + std::max(0, kPlayer.getGoldPerTurn());
	int iNetExpenses = kPlayer.calculateInflatedCosts() + std::max(0, -kPlayer.getGoldPerTurn());
	int iRatio = (100 * iNetExpenses) / std::max(1, iNetCommerce);
	
	if (iRatio > 40)
	{
		iCommerceMultiplier += (33 * (iRatio - 40)) / 60;
	}
	
	// AI no longer uses emphasis except for short term boosts.
	if( isHuman() )
	{
		if (AI_isEmphasizeYield(YIELD_FOOD))
		{
			iFoodMultiplier *= 130;
			iFoodMultiplier /= 100;
		}
		if (AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			iProductionMultiplier *= 140;
			iProductionMultiplier /= 100;
		}
		if (AI_isEmphasizeYield(YIELD_COMMERCE))
		{
			iCommerceMultiplier *= 140;
			iCommerceMultiplier /= 100;
		}
	}

	int iProductionAdvantage = 100 * AI_yieldMultiplier(YIELD_PRODUCTION);
	iProductionAdvantage /= kPlayer.AI_averageYieldMultiplier(YIELD_PRODUCTION);
	iProductionAdvantage *= kPlayer.AI_averageYieldMultiplier(YIELD_COMMERCE);
	iProductionAdvantage /= AI_yieldMultiplier(YIELD_COMMERCE);
	
	//now we normalize the effect by # of cities
	
	int iNumCities = kPlayer.getNumCities();
	FAssert(iNumCities > 0);//superstisious?
	
	//in short in an OCC the relative multipliers should *never* make a difference
	//so this indeed equals "100" for the iNumCities == 0 case.
	iProductionAdvantage = ((iProductionAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1));
	
	iProductionMultiplier *= iProductionAdvantage;
	iProductionMultiplier /= 100;
	
	iCommerceMultiplier *= 100;
	iCommerceMultiplier /= iProductionAdvantage;
	
	int iGreatPeopleAdvantage = 100 * getTotalGreatPeopleRateModifier();
	iGreatPeopleAdvantage /= kPlayer.AI_averageGreatPeopleMultiplier();
	iGreatPeopleAdvantage = ((iGreatPeopleAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1));
	iGreatPeopleAdvantage += 200; //gpp multipliers are larger than others so lets not go overboard
	iGreatPeopleAdvantage /= 3;
	
	//With great people we want to slightly increase food priority at the expense of commerce
	//this gracefully handles both wonder and specialist based GPP...
	iCommerceMultiplier *= 100;
	iCommerceMultiplier /= iGreatPeopleAdvantage;
	iFoodMultiplier *= iGreatPeopleAdvantage;
	iFoodMultiplier /= 100;	
	
	// if leader flavor likes production, increase production, reduce commerce
	if (kPlayer.AI_isDoStrategy(AI_STRATEGY_PRODUCTION))
	{
		iProductionMultiplier += 10;
		iCommerceMultiplier -= 10;
	}

	if (iFoodMultiplier < 100)
	{
		iFoodMultiplier = 10000 / (200 - iFoodMultiplier);
	}
	if (iProductionMultiplier < 100)
	{
		iProductionMultiplier = 10000 / (200 - iProductionMultiplier);
	}
	if (iCommerceMultiplier < 100)
	{
		iCommerceMultiplier = 10000 / (200 - iCommerceMultiplier);
	}
	
	if (angryPopulation(1) > 0)
	{
		iFoodMultiplier /= 2;
	}
}


int CvCityAI::AI_getImprovementValue( CvPlot* pPlot, ImprovementTypes eImprovement, int iFoodPriority, int iProductionPriority, int iCommercePriority, int iFoodChange, bool bOriginal )
{
	BonusTypes eNonObsoleteBonus = pPlot->getNonObsoleteBonusType(getTeam());

	if (eNonObsoleteBonus != NO_BONUS)
	{
		if ( ! GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
		{
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus))
				{
					return 0; //Always prefer improvements which connect bonuses.
				}
			}
		}
	}

	bool bIgnoreFeature = false;
	bool bValid = false;
	BuildTypes eBestTempBuild = NO_BUILD;

	if (eImprovement == pPlot->getImprovementType())
	{
		bValid = true;
	}
	else
	{
		for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
		{
			if (GC.getBuildInfo(unitItr->getBuildType()).getImprovement() != NO_IMPROVEMENT)
			{
				BuildTypes eForcedBuild = unitItr->getBuildType();
				if (eForcedBuild != NO_BUILD && GC.getBuildInfo(eForcedBuild).getImprovement() == eImprovement)
				{
					eBestTempBuild = eForcedBuild; //If a worker is already building a build, force that Build.
				}
				break;
			}
		}
		if (eBestTempBuild == NO_BUILD)
		{
			int iBestTempBuildValue = 0;
			for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
			{
				BuildTypes eBuild = ((BuildTypes)iJ);

				if (GC.getBuildInfo(eBuild).getImprovement() == eImprovement)
				{
					if (GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild, false))
					{
						int iValue = 10000 / (GC.getBuildInfo(eBuild).getTime() + 1);

						// XXX feature production???

						if (iValue > iBestTempBuildValue)
						{
							iBestTempBuildValue = iValue;
							eBestTempBuild = eBuild;
						}
					}
				}
			}
		}

		if (eBestTempBuild != NO_BUILD)
		{
			bValid = true;

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				if (GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
				{
					bIgnoreFeature = true;

					if (GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) > 0)
					{
						if (eNonObsoleteBonus == NO_BONUS)
						{
							if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_LEAVE_FORESTS))
							{
								bValid = false;
							}
							else if (healthRate() < 0 && GC.getFeatureInfo(pPlot->getFeatureType()).getHealthPercent() > 0)
							{
								bValid = false;
							}
							else if (GET_PLAYER(getOwnerINLINE()).getFeatureHappiness(pPlot->getFeatureType()) > 0)
							{
								bValid = false;
							}
						}
					}
				}
			}
		}
	}

	int iValue = 0;

	if (bValid)
	{
		BonusTypes eBonus = pPlot->getBonusType(getTeam());
		ImprovementTypes eFinalImprovement = finalImprovementUpgrade(eImprovement);

		if (eFinalImprovement == NO_IMPROVEMENT)
		{
			eFinalImprovement = eImprovement;
		}

		if (eBonus != NO_BONUS)
		{
			if (eNonObsoleteBonus != NO_BONUS)
			{
				if (GC.getImprovementInfo(eFinalImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
				{
					iValue += (GET_PLAYER(getOwnerINLINE()).AI_bonusVal(eNonObsoleteBonus) * 10);
					iValue += 200;
				}
			}
		}
		else
		{
			for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
			{
				if (GC.getImprovementInfo(eFinalImprovement).getImprovementBonusDiscoverRand(iJ) > 0)
				{
					iValue++;
				}
			}
		}
		
		if (iValue >= 0)
		{
			int aiFinalYields[NUM_YIELD_TYPES];
			int aiDiffYields[NUM_YIELD_TYPES];
			iValue *= 2;
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				if( false && bOriginal )
				{
					aiFinalYields[iJ] = 2*(pPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
					if (bIgnoreFeature && pPlot->getFeatureType() != NO_FEATURE)
					{
						aiFinalYields[iJ] -= 2 * GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange((YieldTypes)iJ);							
					}
					aiDiffYields[iJ] = (aiFinalYields[iJ] - (2 * pPlot->getYield(((YieldTypes)iJ))));
				}
				else
				{
					aiFinalYields[iJ] = 2*(pPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));
					if (bIgnoreFeature && pPlot->getFeatureType() != NO_FEATURE)
					{
						aiFinalYields[iJ] -= 2 * GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange((YieldTypes)iJ);							
					}

					int iCurYield = 2*(pPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), false));

					ImprovementTypes eCurImprovement = pPlot->getImprovementType();
					if( eCurImprovement != NO_IMPROVEMENT )
					{
						ImprovementTypes eCurFinalImprovement = finalImprovementUpgrade(eCurImprovement);
						if (eCurFinalImprovement == NO_IMPROVEMENT)
						{
							eCurFinalImprovement = eCurImprovement;
						}
						iCurYield += (pPlot->calculateImprovementYieldChange(eCurFinalImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));
						iCurYield += (pPlot->calculateImprovementYieldChange(eCurImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));
					}

					aiDiffYields[iJ] = (aiFinalYields[iJ] - iCurYield);
				}
			}
			iValue += (aiDiffYields[YIELD_FOOD] * ((100 * iFoodPriority) / 100));
			iValue += (aiDiffYields[YIELD_PRODUCTION] * ((60 * iProductionPriority) / 100));
			iValue += (aiDiffYields[YIELD_COMMERCE] * ((40 * iCommercePriority) / 100));

			iValue /= 2;

			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				aiFinalYields[iJ] /= 2;
				aiDiffYields[iJ] /= 2;
			}
			
			if (iValue > 0)
			{
				// this is mainly to make it improve better tiles first
				//flood plain > grassland > plain > tundra
				iValue += (aiFinalYields[YIELD_FOOD] * 10);
				iValue += (aiFinalYields[YIELD_PRODUCTION] * 6);
				iValue += (aiFinalYields[YIELD_COMMERCE] * 4);

				if (aiFinalYields[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
				{
					//this is a food yielding tile
					if (iFoodPriority > 100)
					{
						iValue *= 100 + iFoodPriority;
						iValue /= 200;
					}
					if (iFoodChange > 0)
					{
						iValue += (10 * (1 + aiDiffYields[YIELD_FOOD]) * (1 + aiFinalYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION()) * iFoodChange * iFoodPriority) / 100;
					}
					if (iCommercePriority > 100)
					{
						iValue *= 100 + (((iCommercePriority - 100) * aiDiffYields[YIELD_COMMERCE]) / 2);
						iValue /= 100;
					}
				}
				else if (aiFinalYields[YIELD_FOOD] < GC.getFOOD_CONSUMPTION_PER_POPULATION())
				{
					if ((aiDiffYields[YIELD_PRODUCTION] > 0) && (aiFinalYields[YIELD_FOOD]+aiFinalYields[YIELD_PRODUCTION] > 3))
					{
						if (iFoodPriority < 100 || GET_PLAYER(getOwnerINLINE()).getCurrentEra() < 2)
						{
							//value booster for mines on hills
							iValue *= (100 + 25 * aiDiffYields[YIELD_PRODUCTION]);
							iValue /= 100;
						}
					}
					if (iFoodChange < 0)
					{
						iValue *= 4 - iFoodChange;
						iValue /= 3 + aiFinalYields[YIELD_FOOD];
					}
				}

				if ((iFoodPriority < 100) && (iProductionPriority > 100))
				{
					if( bOriginal )
					{
						iValue *= (200 + iProductionPriority);
					}
					else
					{
						iValue *= (200 + ((iProductionPriority - 100)*aiFinalYields[YIELD_PRODUCTION]));
					}
					iValue /= 200;
				}
				if (eBonus == NO_BONUS)
				{
					if (iFoodChange > 0)
					{
						//We want more food.
						iValue *= 2 + std::max(0, aiDiffYields[YIELD_FOOD]);
						iValue /= 2 * (1 + std::max(0, -aiDiffYields[YIELD_FOOD]));
					}/*
					else if (iFoodChange < 0)
					{
						//We want to soak up food.
						iValue *= 8;
						iValue /= 8 + std::max(0, aiDiffYields[YIELD_FOOD]);
					}*/
				}
			}/*
			if (bEmphasizeIrrigation && GC.getImprovementInfo(eFinalImprovement).isCarriesIrrigation())
			{
				iValue += 500;
			}*/

			if (getImprovementFreeSpecialists(eFinalImprovement) > 0)
			{
				iValue += 2000;
			}
			
			int iHappiness = GC.getImprovementInfo(eFinalImprovement).getHappiness();
			if ((iHappiness != 0) && !(GET_PLAYER(getOwnerINLINE()).getAdvancedStartPoints() >= 0))
			{
				//int iHappyLevel = iHappyAdjust + (happyLevel() - unhappyLevel(0));
				int iHappyLevel = 0 + (happyLevel() - unhappyLevel(0));
				if (eImprovement == pPlot->getImprovementType())
				{
					iHappyLevel -= iHappiness;
				}
				int iHealthLevel = (goodHealth() - badHealth(false, 0));
				
				int iHappyValue = 0;
				if (iHappyLevel <= 0)
				{
					iHappyValue += 400;
				}
				bool bCanGrow = true;// (getYieldRate(YIELD_FOOD) > foodConsumption());
				
				if (iHappyLevel <= iHealthLevel)
				{
					iHappyValue += 200 * std::max(0, (bCanGrow ? std::min(6, 2 + iHealthLevel - iHappyLevel) : 0) - iHappyLevel);
				}
				else
				{
					iHappyValue += 200 * std::max(0, (bCanGrow ? 1 : 0) - iHappyLevel);
				}
				if (!pPlot->isBeingWorked())
				{
					iHappyValue *= 4;
					iHappyValue /= 3;
				}
				iHappyValue += std::max(0, (pPlot->getCityRadiusCount() - 1)) * ((iHappyValue > 0) ? iHappyLevel / 2 : 200);
				iValue += iHappyValue * iHappiness;
			}
/************************************************************************************************/
/* Afforess	                  Start		 08/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			int iHealthPercent = GC.getImprovementInfo(eFinalImprovement).getHealthPercent();
			int iBadHealth = abs(getImprovementBadHealth());
			int iGoodHealth = getImprovementGoodHealth();
			//Bad Health AND will push over the edge and give more unhealthiness
			if ((iHealthPercent < 0 && (iBadHealth - iHealthPercent % 100 < iBadHealth % 100)) ||
			//OR Good Health AND will give us more healthiness
			(iHealthPercent > 0 && (iGoodHealth + iHealthPercent % 100 > iGoodHealth % 100)))
			{
				int iHealth = iHealthPercent > 0 ? std::max(1, iHealthPercent % 100) : std::min(-1, iHealthPercent % 100);
				int iHealthLevel = iHealth + (goodHealth() - badHealth(false, 0));
				if (eImprovement == pPlot->getImprovementType())
				{
					iHealthLevel -= iHealth;
				}
				
				int iHealthValue = 0;
				bool bCanGrow = (getYieldRate(YIELD_FOOD) > foodConsumption());
				if (iHealthLevel < 0)
				{
					iHealthValue -= 12 * iHealth;
				}
				if (!bCanGrow && iHealthLevel < 0)
				{
					iHealthValue -= 40 * iHealth;
				}
				if (iHealthLevel > 0)
				{
					iHealthValue += 10 * iHealth;
				}
				
				if (!pPlot->isBeingWorked())
				{
					iHealthValue *= 2;
				}
				iValue += iHealthValue * 3;
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

			if (!isHuman())
			{
				iValue *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getImprovementWeightModifier(eFinalImprovement) + 200));
				iValue /= 200;
			}

			if (pPlot->getImprovementType() == NO_IMPROVEMENT)
			{
				if (pPlot->isBeingWorked())
				{
					iValue *= 5;
					iValue /= 4;
				}

				if (eBestTempBuild != NO_BUILD)
				{
					if (pPlot->getFeatureType() != NO_FEATURE)
					{
						if (GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
						{
							CvCity* pCity = NULL;
							iValue += pPlot->getFeatureProduction(eBestTempBuild, getTeam(), &pCity) * 2;
							FAssert(pCity == this);

							//iValue += iClearFeatureValue;
						}
					}
				}
			}
			else
			{
				// cottage/villages (don't want to chop them up if turns have been invested)
				ImprovementTypes eImprovementDowngrade = (ImprovementTypes)GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage();
				while (eImprovementDowngrade != NO_IMPROVEMENT)
				{
					CvImprovementInfo& kImprovementDowngrade = GC.getImprovementInfo(eImprovementDowngrade);
					iValue -= kImprovementDowngrade.getUpgradeTime() * 8;
					eImprovementDowngrade = (ImprovementTypes)kImprovementDowngrade.getImprovementPillage();
				}
/************************************************************************************************/
/* Afforess	                  Start		 05/23/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
				if (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT)
*/
				if (GET_TEAM(getTeam()).getImprovementUpgrade((ImprovementTypes)pPlot->getImprovementType()) != NO_IMPROVEMENT)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				{
					iValue -= (GC.getImprovementInfo(pPlot->getImprovementType()).getUpgradeTime() * 8 * (pPlot->getUpgradeProgressHundredths())) / std::max(1, 100*GC.getGameINLINE().getImprovementUpgradeTime(pPlot->getImprovementType()));
				}

				if (eNonObsoleteBonus == NO_BONUS)
				{
					if (isWorkingPlot(pPlot))
					{
						if (((iFoodPriority < 100) && (aiFinalYields[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())) || (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage() != NO_IMPROVEMENT))
						{
							iValue -= 70;
							iValue *= 2;
							iValue /= 3;
						}
					}
				}
				
				if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION))
				{
					iValue /= 4;	//Greatly prefer builds which are legal.
				}
			}
		}
	}

	return iValue;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	
BuildTypes CvCityAI::AI_getBestBuild(int iIndex)
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aeBestBuild[iIndex];
}


int CvCityAI::AI_countBestBuilds(CvArea* pArea)
{
	int iCount = 0;

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pArea)
				{
					if (AI_getBestBuild(iI) != NO_BUILD)
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

void CvCityAI::AI_markBestBuildValuesStale()
{
	m_bestBuildValuesStale = true;
}

// Improved worker AI provided by Blake - thank you!
void CvCityAI::AI_updateBestBuild()
{
	PROFILE_FUNC();
	
	//	This is VERY expensive to update, and is pretty accurate from
	//	turn to turn except when suignificant things change (when we will mark it stale)
	if ( !m_bestBuildValuesStale )
	{
		OutputDebugString(CvString::format("City %S skips re-evaluation of build values\n", getName().GetCString()).c_str());
		return;
	}

	OutputDebugString(CvString::format("City %S re-evaluates build values\n", getName().GetCString()).c_str());
	m_bestBuildValuesStale = false;

	int aiFinalYields[NUM_YIELD_TYPES];

	int iBonusFoodSurplus = 0;
	int iBonusFoodDeficit = 0;
	int iFeatureFoodSurplus = 0;
	int iHillFoodDeficit = 0;
	int iFoodTotal = GC.getYieldInfo(YIELD_FOOD).getMinCity();
	int iProductionTotal = GC.getYieldInfo(YIELD_PRODUCTION).getMinCity();
	int iFoodMultiplier = 100;
	int iCommerceMultiplier = 100;
	int iProductionMultiplier = 100;
	int iWorkerCount = 0;
	
	int iWorkableFood = 0;
	int iWorkableFoodPlotCount = 0;
	
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	int iGoodTileCount = 0;
	
	int iSpecialistCount = getSpecialistPopulation() - totalFreeSpecialists();

	int iHappyAdjust = 0;
	int iHealthAdjust = 0;
	
	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iI);

			if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
			{
				int iCount = (kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD));
				iWorkerCount += iCount;

				bool bUseBaseValue = true;
				//If the tile has a BestBuild and is being improved, then use the BestBuild
				//determine if the tile is being improved.
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      06/25/09                                jdog5000      */
/*                                                                                              */
/* Worker AI, City AI                                                                           */
/************************************************************************************************/
				if (iCount > 0)
				{
					BuildTypes eBuild = NO_BUILD;
					if (m_aeBestBuild[iI] != NO_BUILD && m_aiBestBuildValue[iI] > 0)
					{
						eBuild = m_aeBestBuild[iI];
					}
					else
					{
						// This check is necessary to stop oscillation which can result
						// when best build changes food situation for city, changing the best build.
						for (CvPlot::unit_iterator unitItr = pLoopPlot->beginUnits(); unitItr != pLoopPlot->endUnits(); ++unitItr)
						{
							if (unitItr->getBuildType() != NO_BUILD)
							{
								if( eBuild == NO_BUILD || pLoopPlot->getBuildTurnsLeft(eBuild,0,0) > pLoopPlot->getBuildTurnsLeft(unitItr->getBuildType(),0,0) )
								{
									eBuild = unitItr->getBuildType();
								}
							}
						}
					}

					if( eBuild != NO_BUILD )
					{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
						ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
						if (eImprovement != NO_IMPROVEMENT)
						{
							bool bIgnoreFeature = false;
							if (pLoopPlot->getFeatureType() != NO_FEATURE)
							{
								if (GC.getBuildInfo(eBuild).isFeatureRemove(pLoopPlot->getFeatureType()))
								{
									bIgnoreFeature = true;
								}
							}
							
							iHappyAdjust += GC.getImprovementInfo(eImprovement).getHappiness();
							if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
							{
								iHappyAdjust -= GC.getImprovementInfo(pLoopPlot->getImprovementType()).getHappiness();
							}
								

							bUseBaseValue = false;
							for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								aiFinalYields[iJ] = (pLoopPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature) + pLoopPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
							}
						}
					}
				}

				//Otherwise use the base value.
				if (bUseBaseValue)
				{
					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						//by default we'll use the current value
						aiFinalYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
						if (pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							aiFinalYields[iJ] += std::max(0, -GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange((YieldTypes)iJ));
						}
					}
				}
				
				if (pLoopPlot->isBeingWorked())
				{
					//iWorkedFood += aiFinalYields[YIELD_FOOD];					
				}
				else
				{
					if (aiFinalYields[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
					{
						iWorkableFood += aiFinalYields[YIELD_FOOD];
						iWorkableFoodPlotCount++;
					}
				}
				
				if (pLoopPlot->isBeingWorked() || (((aiFinalYields[YIELD_FOOD]*10) + (aiFinalYields[YIELD_PRODUCTION]*6) + (aiFinalYields[YIELD_COMMERCE]*4)) > 21))
				{
					iGoodTileCount++;
					if (pLoopPlot->isBeingWorked())
					{
						iFoodTotal += aiFinalYields[YIELD_FOOD];
					}
					else
					{
						iFoodTotal += aiFinalYields[YIELD_FOOD] / 2;
					}
					if (aiFinalYields[YIELD_PRODUCTION] > 1)
					{	
						iProductionTotal += aiFinalYields[YIELD_PRODUCTION];
					}
				}

				if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				{
					int iNetFood = (aiFinalYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION());
					iBonusFoodSurplus += std::max(0, iNetFood);
					iBonusFoodDeficit += std::max(0, -iNetFood);
				}

				if ((pLoopPlot->getFeatureType()) != NO_FEATURE)
				{
					iFeatureFoodSurplus += std::max(0, pLoopPlot->calculateNatureYield(YIELD_FOOD, getTeam()) - GC.getFOOD_CONSUMPTION_PER_POPULATION());
				}

				if ((pLoopPlot->isHills()))
				{
					iHillFoodDeficit += std::max(0, GC.getFOOD_CONSUMPTION_PER_POPULATION() - pLoopPlot->calculateNatureYield(YIELD_FOOD, getTeam()));
				}
			}
		}
	}

	if (getProductionBuilding() != NO_BUILDING)
	{
/********************************************************************************/
/*	Better Evaluation							09.03.2010		Fuyu		    */
/********************************************************************************/
/* original code
		iHappyAdjust += getBuildingHappiness(getProductionBuilding());
		iHealthAdjust += getBuildingHealth(getProductionBuilding());    	
*/
		iHappyAdjust += getAdditionalHappinessByBuilding(getProductionBuilding());
		iHealthAdjust += getAdditionalHealthByBuilding(getProductionBuilding());
/********************************************************************************/
/*	BE	END																		*/
/********************************************************************************/
	}

	//XXX rewrite this to fix too many farms issue
	int iSpecialistAdjustment = 0;
	if (iWorkableFoodPlotCount > 0)
	{
		//Calculate approximately how much food the assigned specialists could work
		iSpecialistAdjustment = (std::min(iSpecialistCount, iWorkableFoodPlotCount) * iWorkableFood) / iWorkableFoodPlotCount;
	}
	iFoodTotal += iSpecialistAdjustment;
	
	int iBonusFoodDiff = ((iBonusFoodSurplus + iFeatureFoodSurplus) - (iBonusFoodDeficit + iHillFoodDeficit / 2));

	int iHealth = goodHealth() - badHealth();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      09/02/10                         jdog5000 & Fuyu      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
	int iTargetSize = iGoodTileCount;

	if( getEspionageHealthCounter() > 0 )
	{
		iTargetSize = std::min(iTargetSize, 2 + getPopulation());
	}
	else
	{
		iTargetSize = std::min(iTargetSize, 2 + getPopulation() + (iHealth)/2);
	}

	if( iTargetSize < getPopulation() )
	{
		iTargetSize = std::max(iTargetSize, getPopulation() - (AI_countWorkedPoorTiles()/2));
	}
	
	// Target city size should not be perturbed by espionage, other short term effects
	if( getEspionageHappinessCounter() > 0 )
	{
		iTargetSize = std::min(iTargetSize, getPopulation());
	}
	else
	{
		iTargetSize = std::min(iTargetSize, getPopulation()+(happyLevel()-unhappyLevel()));
	}
	
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/02/09                                jdog5000      */
/*                                                                                              */
/* Worker AI                                                                                    */
/************************************************************************************************/
/* original bts code
	if (GET_PLAYER(getOwnerINLINE()).getAdvancedStartPoints() >= 0)
	{
		iTargetSize += 2 + GET_PLAYER(getOwnerINLINE()).getCurrentEra() / 2;
	}
	
	if (kPlayer.getAdvancedStartPoints() >= 0)
	{
		iTargetSize += kPlayer.getCurrentEra() / 2;
	}
*/
	// WTF code duplication
	if (kPlayer.getAdvancedStartPoints() >= 0)
	{
		iTargetSize += 2 + kPlayer.getCurrentEra();
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if (iBonusFoodDiff < 2)
	{
		iFoodMultiplier += 10 * (2 - iBonusFoodDiff);
	}
	
	int iExtraFoodForGrowth = (std::max(0, iTargetSize - getPopulation()) + 3) / 4;
	if (getPopulation() < iTargetSize)
	{
		iExtraFoodForGrowth ++;
	}

	int iFoodDifference = iFoodTotal - ((iTargetSize * GC.getFOOD_CONSUMPTION_PER_POPULATION()) + iExtraFoodForGrowth);
	
	int iDesiredFoodChange = -iFoodDifference + std::max(0, -iHealth);
	if (iTargetSize > getPopulation())
	{
		if (iDesiredFoodChange > 3)
		{
			iDesiredFoodChange = (iDesiredFoodChange + 3) / 2;
		}
	}

	if (iFoodDifference < 0)
	{
		iFoodMultiplier +=  -iFoodDifference * 4;
	}
	
	if (iFoodDifference > 4)
	{
		iFoodMultiplier -= 8 + 4 * iFoodDifference;
	}

	if (iProductionTotal < 10)
	{
		iProductionMultiplier += (80 - 8 * iProductionTotal);
	}
	int iProductionTarget = 1 + (std::min(getPopulation(), (iTargetSize * 3) / 5));
	
	if (iProductionTotal < iProductionTarget)
	{
		iProductionMultiplier += 8 * (iProductionTarget - iProductionTotal);
	}

	if ((iBonusFoodSurplus + iFeatureFoodSurplus > 5) && ((iBonusFoodDeficit + iHillFoodDeficit) > 5))
	{
		if ((iBonusFoodDeficit + iHillFoodDeficit) > 8)
		{
			//probably a good candidate for a wonder pump
			iProductionMultiplier += 40;
			iCommerceMultiplier += (kPlayer.AI_isFinancialTrouble()) ? 0 : -40;
		}
	}
	
	
	int iNetCommerce = 1 + kPlayer.getCommerceRate(COMMERCE_GOLD) + kPlayer.getCommerceRate(COMMERCE_RESEARCH) + std::max(0, kPlayer.getGoldPerTurn());
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       06/11/09                       jdog5000 & DanF5771    */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* original BTS code
	int iNetExpenses = kPlayer.calculateInflatedCosts() + std::min(0, kPlayer.getGoldPerTurn());
*/
	int iNetExpenses = kPlayer.calculateInflatedCosts() + std::max(0, -kPlayer.getGoldPerTurn());
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
	int iRatio = (100 * iNetExpenses) / std::max(1, iNetCommerce);
	
	if (iRatio > 40)
	{
		iCommerceMultiplier += (33 * (iRatio - 40)) / 60;
	}
	
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/06/09                                jdog5000      */
/*                                                                                              */
/* Worker AI                                                                                    */
/************************************************************************************************/
	// AI no longer uses emphasis except for short term boosts.
	if( isHuman() )
	{
		if (AI_isEmphasizeYield(YIELD_FOOD))
		{
			iFoodMultiplier *= 130;
			iFoodMultiplier /= 100;
		}
		if (AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			iProductionMultiplier *= 140;
			iProductionMultiplier /= 100;
		}
		if (AI_isEmphasizeYield(YIELD_COMMERCE))
		{
			iCommerceMultiplier *= 140;
			iCommerceMultiplier /= 100;
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	
	int iProductionAdvantage = 100 * AI_yieldMultiplier(YIELD_PRODUCTION);
	iProductionAdvantage /= kPlayer.AI_averageYieldMultiplier(YIELD_PRODUCTION);
	iProductionAdvantage *= kPlayer.AI_averageYieldMultiplier(YIELD_COMMERCE);
	iProductionAdvantage /= AI_yieldMultiplier(YIELD_COMMERCE);
	
	//now we normalize the effect by # of cities
	
	int iNumCities = kPlayer.getNumCities();
	FAssert(iNumCities > 0);//superstitious?
	
	//in short in an OCC the relative multipliers should *never* make a difference
	//so this indeed equals "100" for the iNumCities == 0 case.
	iProductionAdvantage = ((iProductionAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1));
	
	iProductionMultiplier *= iProductionAdvantage;
	iProductionMultiplier /= 100;
	
	iCommerceMultiplier *= 100;
	iCommerceMultiplier /= std::max(1, iProductionAdvantage);
	
	int iGreatPeopleAdvantage = 100 * getTotalGreatPeopleRateModifier();
	iGreatPeopleAdvantage /= kPlayer.AI_averageGreatPeopleMultiplier();
	iGreatPeopleAdvantage = ((iGreatPeopleAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1));
	iGreatPeopleAdvantage += 200; //gpp multipliers are larger than others so lets not go overboard
	iGreatPeopleAdvantage /= 3;
	
	//With great people we want to slightly increase food priority at the expense of commerce
	//this gracefully handles both wonder and specialist based GPP...
	iCommerceMultiplier *= 100;
	iCommerceMultiplier /= iGreatPeopleAdvantage;
	iFoodMultiplier *= iGreatPeopleAdvantage;
	iFoodMultiplier /= 100;	
	
	// if leader flavor likes production, increase production, reduce commerce
	if (kPlayer.AI_isDoStrategy(AI_STRATEGY_PRODUCTION))
	{
		iProductionMultiplier += 10;
		iCommerceMultiplier -= 10;
	}
	
	if (iFoodMultiplier < 100)
	{
		iFoodMultiplier = 10000 / (200 - iFoodMultiplier);
	}
	if (iProductionMultiplier < 100)
	{
		iProductionMultiplier = 10000 / (200 - iProductionMultiplier);
	}
	if (iCommerceMultiplier < 100)
	{
		iCommerceMultiplier = 10000 / (200 - iCommerceMultiplier);
	}
	
	if (angryPopulation(1) > 0)
	{
		iFoodMultiplier /= 2;
	}

	ProjectTypes eProductionProject = getProductionProject();
	bool bChop = (eProductionProject != NO_PROJECT && AI_projectValue(eProductionProject) > 0);
	if (!bChop)
	{
		BuildingTypes eProductionBuilding = getProductionBuilding();
		bChop = (eProductionBuilding != NO_BUILDING && isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eProductionBuilding).getBuildingClassType())));
		if (!bChop)
		{
			UnitTypes eProductionUnit = getProductionUnit();
			bChop = (eProductionUnit != NO_UNIT && GC.getUnitInfo(eProductionUnit).isFoodProduction());
			if (!bChop)
			{
				bChop = ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
			}
		}
	}

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		m_aiBestBuildValue[iI] = 0;
		m_aeBestBuild[iI] = NO_BUILD;

		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
			{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/16/10                                jdog5000      */
/*                                                                                              */
/* City AI, Worker AI                                                                           */
/************************************************************************************************/
				int iLastBestBuildValue = m_aiBestBuildValue[iI];
				BuildTypes eLastBestBuildType = m_aeBestBuild[iI];

				AI_bestPlotBuild(pLoopPlot, &(m_aiBestBuildValue[iI]), &(m_aeBestBuild[iI]), iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, bChop, iHappyAdjust, iHealthAdjust, iDesiredFoodChange);
				m_aiBestBuildValue[iI] *= 4;
				m_aiBestBuildValue[iI] += 3 + iWorkerCount;  // to round up
				m_aiBestBuildValue[iI] /= (4 + iWorkerCount);
				
				if (m_aiBestBuildValue[iI] > 0)
				{
					FAssert(m_aeBestBuild[iI] != NO_BUILD);
				}
				if (m_aeBestBuild[iI] != NO_BUILD)
				{
					FAssert(m_aiBestBuildValue[iI] > 0);
				}

				if( eLastBestBuildType != NO_BUILD )
				{
					if( eLastBestBuildType != m_aeBestBuild[iI] )
					{
						if( iWorkerCount > 0 )
						{
							// BBAI TODO: Check workers are building the new thing

							if( gCityLogLevel >= 2 )
							{
								logBBAI( "      City %S switches best build on plot %d, %d from %S (%d) to %S (%d) with worker count %d",getName().GetCString(),pLoopPlot->getX(),pLoopPlot->getY(),GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eLastBestBuildType).getImprovement()).getDescription(),iLastBestBuildValue,GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(m_aeBestBuild[iI]).getImprovement()).getDescription(),m_aiBestBuildValue[iI],iWorkerCount);
							}
						}
					}
				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
			}
		}
	}
	
	
	{	//new experimental yieldValue calcuation
		short aiYields[NUM_YIELD_TYPES];
		int iBestPlot = -1;
		int iBestPlotValue = -1;
		int iValue;
		
		int iBestUnworkedPlotValue = 0;
		
		int aiValues[NUM_CITY_PLOTS];
		
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
			if (iI != CITY_HOME_PLOT)
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

				if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
				{
					if (m_aeBestBuild[iI] != NO_BUILD)
					{
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiYields[iJ] = pLoopPlot->getYieldWithBuild(m_aeBestBuild[iI], (YieldTypes)iJ, true);
						}
						
						iValue = AI_yieldValue(aiYields, NULL, false, false, false, false, true, true);
						aiValues[iI] = iValue;
						if ((iValue > 0) && (pLoopPlot->getRouteType() != NO_ROUTE))
						{
							iValue++;
						}
						//FAssert(iValue > 0);
						
						iValue = std::max(0, iValue);
						
						m_aiBestBuildValue[iI] *= iValue + 100;
						m_aiBestBuildValue[iI] /= 100;
						
						if (iValue > iBestPlotValue)
						{
							iBestPlot = iI;
							iBestPlotValue = iValue;							
						}
					}
					if (!pLoopPlot->isBeingWorked())
					{
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
						}
						
						iValue = AI_yieldValue(aiYields, NULL, false, false, false, false, true, true);					
						
						iBestUnworkedPlotValue = std::max(iBestUnworkedPlotValue, iValue);
					}
				}
			}
		}
		if (iBestPlot != -1)
		{
			m_aiBestBuildValue[iBestPlot] *= 2;
		}
		
		//Prune plots which are sub-par.
		if (iBestUnworkedPlotValue > 0)
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
				if (iI != CITY_HOME_PLOT)
				{
					CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

					if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
					{
						if (m_aeBestBuild[iI] != NO_BUILD) 
						{
							if (!pLoopPlot->isBeingWorked() && (pLoopPlot->getImprovementType() == NO_IMPROVEMENT))
							{
								if (GC.getBuildInfo(m_aeBestBuild[iI]).getImprovement() != NO_IMPROVEMENT)
								{
									if ((aiValues[iI] <= iBestUnworkedPlotValue) && (aiValues[iI] < 500))
									{
										m_aiBestBuildValue[iI] = 1;
									}
								}
							}
							else if ((pLoopPlot->getImprovementType() != NO_IMPROVEMENT) && (GC.getBuildInfo(m_aeBestBuild[iI]).getImprovement() != NO_IMPROVEMENT))
							{
								for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
								{
									aiYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
								}
								
								iValue = AI_yieldValue(aiYields, NULL, false, false, false, false, true, true);
								if (iValue > aiValues[iI])
								{
									m_aiBestBuildValue[iI] = 1;
								}
							}
						}
					}
				}
			}
		}
	}
}

// Protected Functions...

// Better drafting strategy by Blake - thank you!
void CvCityAI::AI_doDraft(bool bForce)
{
	PROFILE_FUNC();

	FAssert(!isHuman());
	if (isNPC())
	{
		return;
	}

	if (canConscript())
	{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      07/12/09                                jdog5000      */
/*                                                                                              */
/* City AI, War Strategy AI                                                                     */
/************************************************************************************************/
		if (GC.getGameINLINE().AI_combatValue(getConscriptUnit()) > 33)
		{
			if (bForce)
			{
				conscript();
				return;
			}
			bool bLandWar = ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
			bool bDanger = (!AI_isDefended() && AI_isDanger());

			// Don't go broke from drafting
			if( !bDanger && GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble() )
			{
				return;
			}

			// Don't shrink cities too much
			int iConscriptPop = getConscriptPopulation();
			if ( !bDanger && (3 * (getPopulation() - iConscriptPop) < getHighestPopulation() * 2) )
			{
				return;
			}

			// Large cities want a little spare happiness
			int iHappyDiff = GC.getDefineINT("CONSCRIPT_POP_ANGER") - iConscriptPop + getPopulation()/10;

			if (bLandWar && (0 == angryPopulation(iHappyDiff)))
			{
				bool bWait = true;

				if( GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_TURTLE) )
				{
					// Full out defensive
					if( bDanger || (getPopulation() >= std::max(5, getHighestPopulation() - 1)) )
					{
						bWait = false;
					}
					else if( (AI_countWorkedPoorTiles() + std::max(0,(visiblePopulation() - AI_countGoodSpecialists(true)))) >= 1 )
					{
						bWait = false;
					}
				}
				
				if( bWait && bDanger )
				{
					// If city might be captured, don't hold back
					int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(),0,true,false,true);
					int iEnemyOffense = GET_PLAYER(getOwnerINLINE()).AI_getEnemyPlotStrength(plot(),2,false,false);

					if( (iOurDefense == 0) || (3*iEnemyOffense > 2*iOurDefense) )
					{
						bWait = false;
					}
				}

				if( bWait )
				{
					// Non-critical, only burn population if population is not worth much
					if ((getConscriptAngerTimer() == 0) && ((AI_countWorkedPoorTiles() + std::max(0,(visiblePopulation() - AI_countGoodSpecialists(true)))) >= iConscriptPop))
					{
						if( (getPopulation() >= std::max(5, getHighestPopulation() - 1)) )
						{
							bWait = false;
						}
					}
				}

				if( !bWait && gCityLogLevel >= 2 )
				{
					logBBAI("      City %S (size %d, highest %d) chooses to conscript with danger: %d, land war: %d, poor tiles: %d, bad specialists: %d", getName().GetCString(), getPopulation(), getHighestPopulation(), bDanger, bLandWar, AI_countWorkedPoorTiles(), std::max(0,(visiblePopulation() - AI_countGoodSpecialists(true))) );
				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

				if (!bWait)
				{
					conscript();
				}
			}
		}
	}
}

// Better pop-rushing strategy by Blake - thank you!
void CvCityAI::AI_doHurry(bool bForce)
{
	PROFILE_FUNC();

	FAssert(!isHuman() || isProductionAutomated());
	
	if (isNPC())
	{
		return;
	}

	if ((getProduction() == 0) && !bForce)
	{
		return;
	}

	CvArea* pWaterArea = waterArea();

	UnitTypes eProductionUnit = getProductionUnit();
	UnitAITypes eProductionUnitAI = getProductionUnitAI();
	BuildingTypes eProductionBuilding = getProductionBuilding();

	bool bDanger = AI_isDanger();

	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		if (canHurry((HurryTypes)iI))
		{
			int iHurryAngerLength = hurryAngerLength((HurryTypes)iI);
			int iHurryPopulation = hurryPopulation((HurryTypes)iI);
			if (bForce && iHurryPopulation < 1)//TB The AI is whipping far too often and easily and should almost never do so.  Disabling validity of this action in most situations.
			{
				hurry((HurryTypes)iI);
				break;
			}

			int iMinTurns = MAX_INT;
			bool bEssential = false;
			bool bGrowth = false;

			// Whip to eliminate unhappiness - thank you Blake!
			//if (getProduction() > 0)
			//{
			//	//TBRUSHFIX
			//	//There may be a bug in this where health is being counted rather than happiness BUT
			//	//Regardless this should be a last resort in C2C.  
			//	//Happiness is simply not as hard to get on top of in C2C.
			//	//So first ensure that there's cause to panic.
			//	//Second, check to see if a quick building can resolve some of the unhappiness.
			//	//Third, check to see if military defenders can be quickly trained to resolve some of the unhappiness.
			//	//Fourth, ensure that properties aren't out of control and needing help there.
			//	//If all of the above is exhausted, we may have real cause to rush just to reduce population.
			//	int iHappyness = happyLevel() - unhappyLevel(0);

			//	if ( iHappyness < 0 )
			//	{
			//		//	In this case, any unhappiness needs to be addressed quickly if at all possible.
			//		if (!AI_chooseBuilding(BUILDINGFOCUS_HAPPY, 2, 0, -1, true))
			//		{
			//			//	Can we build military happyness units?
			//			if ( GET_PLAYER(getOwner()).getHappyPerMilitaryUnit() > 0 )
			//			{
			//				if (!AI_chooseUnit("emergency military happyness", UNITAI_CITY_DEFENSE))
			//				{
			//					if (AI_getHappyFromHurry((HurryTypes)iI) > 0)
			//					{
			//						if( gCityLogLevel >= 2 )
			//						{
			//							logBBAI("      City %S hurry to remove unhappiness", getName().GetCString() );
			//						}
			//						hurry((HurryTypes)iI);
			//						break;
			//					}
			//				}
			//			}
			//			//else
			//			//{
			//			//	PropertyTypes pProperty = (PropertyTypes)GC.getInfoTypeForString("PROPERTY_CRIME");
			//			//	if ( !AI_choosePropertyControlUnit(10, pProperty) )
			//			//	{
			//			//		if (AI_getHappyFromHurry((HurryTypes)iI) > 0)
			//			//		{
			//			//			if( gCityLogLevel >= 2 )
			//			//			{
			//			//				logBBAI("      City %S hurry to remove unhappiness", getName().GetCString() );
			//			//			}
			//			//			hurry((HurryTypes)iI);
			//			//			break;
			//			//		}
			//			//	}
			//			//}
			//		}
			//	}
			//}

			// Rush defenses when in big trouble
			//TBRUSHFIX
			// This routine is noted and I'll be looking for examples of its use ingame before doing any restructuring.  This might be appropriate.  Maybe.  The rushing could put them in bigger trouble when they can't then rush to get defenders when they need to.
			if ( (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) && GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true) > 150 )
			{
				if( eProductionUnit != NO_UNIT && GC.getGameINLINE().AI_combatValue(eProductionUnit) > 33 && getProduction() > 0 )
				{
					//if( (iHurryPopulation > 0) && (iHurryAngerLength == 0 || getHurryAngerTimer() < 2) && (iHurryPopulation < 3 && iHurryPopulation < getPopulation()/3))
					//{
					//	bool bWait = true;

					//	if( GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_TURTLE) )
					//	{
					//		bWait = false;
					//	}
					//	else if( (3*(getPopulation() - iHurryPopulation)) < getHighestPopulation()*2 )
					//	{
					//		bWait = true;
					//	}
					//	else if( GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble() )
					//	{
					//		bWait = true;
					//	}
					//	else
					//	{
					//		for( int iJ = 0; iJ < MAX_PC_TEAMS; iJ++ )
					//		{
					//			if( GET_TEAM((TeamTypes)iJ).isAlive() && !GET_TEAM((TeamTypes)iJ).isMinorCiv() )
					//			{
					//				if( GET_TEAM(getTeam()).isAtWar((TeamTypes)iJ) && GET_TEAM(getTeam()).AI_getAtWarCounter((TeamTypes)iJ) < 10 )
					//				{
					//					bWait = false;
					//					break;
					//				}
					//			}
					//		}
					//	}

					//	if( !bWait )
					//	{
					//		if( gCityLogLevel >= 2 )
					//		{
					//			logBBAI("      City %S hurry pop at %d to rush defenses for recent attack", getName().GetCString(), iHurryPopulation );
					//		}
					//		hurry((HurryTypes)iI);
					//		break;
					//	}
					//}
					//else
					{
						//TBRUSHFIX
						//I can appreciate nearly any gold rush.  These are more useful as they don't dramatically impede the AI so much.
						if( !(GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble()) )
						{
							int iHurryGold = hurryGold((HurryTypes)iI);
							if( iHurryGold > 0 && iHurryAngerLength == 0 )
							{
								bool bWait = true;

								if( GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_TURTLE) )
								{
									if( (bDanger ? 5 : 8)*iHurryGold < GET_PLAYER(getOwnerINLINE()).getEffectiveGold() )
									{
										bWait = false;
									}
								}
								else
								{
									if( (bDanger ? 8 : 12)*iHurryGold < GET_PLAYER(getOwnerINLINE()).getEffectiveGold() )
									{
										bWait = false;
									}
								}

								if( !bWait )
								{
									if( gCityLogLevel >= 2 )
									{
										logBBAI("      City %S hurry gold at %d to rush defenses for recent attack", getName().GetCString(), iHurryGold );
									}
									hurry((HurryTypes)iI);
									break;
								}
							}
						}
					}
				}
			}

			//TBRUSHFIX
			//Slaving won't take place here since these require no pop cost (which is the problem I'm trying to resolve anyhow.)
			if ((iHurryAngerLength == 0) && (iHurryPopulation == 0))
			{
				if (GET_PLAYER(getOwnerINLINE()).AI_avoidScience())
				{
					if (GET_PLAYER(getOwnerINLINE()).getEffectiveGold() > GET_PLAYER(getOwnerINLINE()).AI_goldTarget())
					{
						iMinTurns = std::min(iMinTurns, 10);
					}
				}
				if (eProductionBuilding != NO_BUILDING)
				{
					int iValuePerTurn = AI_buildingValueThreshold(eProductionBuilding, BUILDINGFOCUS_GOLD | BUILDINGFOCUS_MAINTENANCE | BUILDINGFOCUS_PRODUCTION);
					
					iValuePerTurn /= 3;
					
					if (iValuePerTurn > 0)
					{
						int iHurryGold = hurryGold((HurryTypes)iI);
/* original bts code
						if ((iHurryGold / iValuePerTurn) < getProductionTurnsLeft(eProductionBuilding, 1))
*/
						if ( (iHurryGold > 0) && ((iHurryGold / iValuePerTurn) < getProductionTurnsLeft(eProductionBuilding, 1)) )
						{
/* original bts code
							if (iHurryGold < (GET_PLAYER(getOwnerINLINE()).getGold() / 3))
*/
							int iGoldThreshold = GET_PLAYER(getOwnerINLINE()).getEffectiveGold();
							iGoldThreshold -= (GET_PLAYER(getOwnerINLINE()).AI_goldToUpgradeAllUnits() / ((GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0) ? 1 : 3));
							iGoldThreshold /= 3;
							if (iHurryGold < iGoldThreshold)
							{
								if( gCityLogLevel >= 2 )
								{
									logBBAI("      City %S hurry gold at %d < threshold %d", getName().GetCString(), iHurryGold, iGoldThreshold );
								}
								hurry((HurryTypes)iI);
								return;
							}								
						}
					}
				}
			}

			//TBRUSHFIX
			//Perhaps the most disturbing part of this is how it doesn't scale for longer games.
			if (eProductionBuilding != NO_BUILDING)
			{
				if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eProductionBuilding).getBuildingClassType())))
				{
					iMinTurns = std::min(iMinTurns, 10);
					bEssential = true;
				}

				if (GC.getBuildingInfo(eProductionBuilding).getDefenseModifier() > 0)
				{
					if (bDanger)
					{
						iMinTurns = std::min(iMinTurns, 3);
						bEssential = true;
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getBombardDefenseModifier() > 0)
				{
					if (bDanger)
					{
						iMinTurns = std::min(iMinTurns, 3);
						bEssential = true;
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getYieldModifier(YIELD_PRODUCTION) > 0)
				{
					if (getModifiedBaseYieldRate(YIELD_PRODUCTION) >= 6)
					{
						iMinTurns = std::min(iMinTurns, 10);
						bGrowth = true;
					}
				}

				if ((GC.getBuildingInfo(eProductionBuilding).getCommerceChange(COMMERCE_CULTURE) > 0) ||
					(GC.getBuildingInfo(eProductionBuilding).getCommercePerPopChange(COMMERCE_CULTURE) > 0) ||
						(GC.getBuildingInfo(eProductionBuilding).getObsoleteSafeCommerceChange(COMMERCE_CULTURE) > 0))
				{
					if ((getCommerceRateTimes100(COMMERCE_CULTURE) == 0) || (plot()->calculateCulturePercent(getOwnerINLINE()) < 40))
					{
						iMinTurns = std::min(iMinTurns, 10);
						if (getCommerceRateTimes100(COMMERCE_CULTURE) == 0)
						{
							bEssential = true;
							iMinTurns = std::min(iMinTurns, 5);
							if (AI_countNumBonuses(NO_BONUS, false, true, 2, true, true) > 0)
							{
								bGrowth = true;						    	
							}
						}
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getHappiness() > 0)
				{
					if (angryPopulation() > 0)
					{
						iMinTurns = std::min(iMinTurns, 10);
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getHealth() > 0)
				{
					if (healthRate() < 0)
					{
						iMinTurns = std::min(iMinTurns, 10);
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getSeaPlotYieldChange(YIELD_FOOD) > 0 || GC.getBuildingInfo(eProductionBuilding).getRiverPlotYieldChange(YIELD_FOOD) > 0)
				{
					
					iMinTurns = std::min(iMinTurns, 10);
					
					if (AI_buildingSpecialYieldChangeValue(eProductionBuilding, YIELD_FOOD) > (getPopulation() * 2))
					{
						bEssential = true;
						bGrowth = true;
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getFreeExperience() > 0)
				{
					if (bDanger)
					{
						iMinTurns = std::min(iMinTurns, 3);
						bEssential = true;
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getMaintenanceModifier() < 0)
				{
					if (getMaintenance() >= 10)
					{
						iMinTurns = std::min(iMinTurns, 10);
						bEssential = true;
					}
				}

				if (GC.getDefineINT("DEFAULT_SPECIALIST") != NO_SPECIALIST)
				{
					if (getSpecialistCount((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))) > 0)
					{
						for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
						{
							if (GC.getBuildingInfo(eProductionBuilding).getSpecialistCount(iJ) > 0)
							{
								iMinTurns = std::min(iMinTurns, 10);
								break;
							}
						}
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getCommerceModifier(COMMERCE_GOLD) > 0)
				{
					if (GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
					{
						if (getBaseCommerceRate(COMMERCE_GOLD) >= 16)
						{
							iMinTurns = std::min(iMinTurns, 10);
						}
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getCommerceModifier(COMMERCE_RESEARCH) > 0)
				{
					if (!(GET_PLAYER(getOwnerINLINE()).AI_avoidScience()))
					{
						if (getBaseCommerceRate(COMMERCE_RESEARCH) >= 16)
						{
							iMinTurns = std::min(iMinTurns, 10);
						}
					}
				}

				if (GC.getBuildingInfo(eProductionBuilding).getFoodKept() > 0)
				{
					iMinTurns = std::min(iMinTurns, 5);
					bEssential = true;
					bGrowth = true;
				}
			}

			if (eProductionUnit != NO_UNIT)
			{
				if (GC.getUnitInfo(eProductionUnit).getDomainType() == DOMAIN_LAND)
				{
					if ((GC.getUnitInfo(eProductionUnit).getCombat() + 
					GET_TEAM(getTeam()).getUnitClassStrengthChange((UnitClassTypes)GC.getUnitInfo(eProductionUnit).getUnitClassType())) > 0)
					{
						if (bDanger)
						{
							iMinTurns = std::min(iMinTurns, 3);
							bEssential = true;
						}
					}
				}
			}

			if (eProductionUnitAI == UNITAI_CITY_DEFENSE)
			{
				if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_SETTLE, -1, NULL, getOwnerINLINE()) != NULL)
				{
					if (!AI_isDefended(-2*GET_PLAYER(getOwnerINLINE()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE))) // XXX check for other team's units?
					{
						iMinTurns = std::min(iMinTurns, 5);
					}
				}
			}

			if (eProductionUnitAI == UNITAI_SETTLE)
			{
				if (area()->getNumAIUnits(getOwnerINLINE(), UNITAI_SETTLE) == 0)
				{
					if (!(GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble()))
					{
						if (area()->getBestFoundValue(getOwnerINLINE()) > 0)
						{
							iMinTurns = std::min(iMinTurns, 5);
							bEssential = true;
							bGrowth = true;
						}
					}
				}
			}

			if (eProductionUnitAI == UNITAI_SETTLER_SEA)
			{
				if (pWaterArea != NULL)
				{
					if (pWaterArea->getNumAIUnits(getOwnerINLINE(), UNITAI_SETTLER_SEA) == 0)
					{
						if (area()->getNumAIUnits(getOwnerINLINE(), UNITAI_SETTLE) > 0)
						{
							iMinTurns = std::min(iMinTurns, 5);
						}
					}
				}
			}

			if (eProductionUnitAI == UNITAI_WORKER)
			{
				if (GET_PLAYER(getOwnerINLINE()).AI_neededWorkers(area()) > (area()->getNumAIUnits(getOwnerINLINE(), UNITAI_WORKER) * 2))
				{
					iMinTurns = std::min(iMinTurns, 5);
					bEssential = true;
					bGrowth = true;
				}
			}

			if (eProductionUnitAI == UNITAI_WORKER_SEA)
			{
				if (AI_neededSeaWorkers() > 0)
				{
					iMinTurns = std::min(iMinTurns, 5);
					bEssential = true;
					bGrowth = true;
				}
			}

			// adjust for game speed
			if (NO_UNIT != getProductionUnit())
			{
				iMinTurns *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
			}
			else if (NO_BUILDING != getProductionBuilding())
			{
				iMinTurns *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
			}
			else if (NO_PROJECT != getProductionProject())
			{
				iMinTurns *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getCreatePercent();
			}
			else
			{
				iMinTurns *= 100;
			}

			iMinTurns /= 100;

			if (NO_UNIT != getProductionUnit())
			{
				iMinTurns *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getTrainPercent();
			}
			else if (NO_BUILDING != getProductionBuilding())
			{
				iMinTurns *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getConstructPercent();
			}
			else if (NO_PROJECT != getProductionProject())
			{
				iMinTurns *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getConstructPercent();
			}
			else
			{
				iMinTurns *= 100;
			}

			iMinTurns /= 100;

			//TBRUSHFIX: Specialists are super useful in C2C, arguably often moreso than tile working.
			/*if (AI_countGoodTiles((healthRate(0) == 0), false, 100) <= (getPopulation() - iHurryPopulation))*/
			int iThreshold = (getPopulation() - iHurryPopulation);
			int iThreshAdjust = (int)GET_PLAYER(getOwner()).getCurrentEra();
			iThreshold += iThreshAdjust;
			iThreshold *= iThreshAdjust;
			//This ought to make the AI much more reluctant to slave for frivolousness.

			//this overrides everything.
			if (bGrowth)
			{
				int iHurryGold = hurryGold((HurryTypes)iI);
				if ((iHurryGold > 0) && ((iHurryGold * 16) < GET_PLAYER(getOwnerINLINE()).getEffectiveGold()))
				{
					if( gCityLogLevel >= 2 )
					{
						logBBAI("      City %S hurry gold at %d for growth when rich at %d", getName().GetCString(), iHurryGold, GET_PLAYER(getOwnerINLINE()).getEffectiveGold() );
					}
					hurry((HurryTypes)iI);
					break;					
				}
/* original bts code
				if (AI_countGoodTiles((healthRate(0) == 0), false, 100) <= (getPopulation() - iHurryPopulation))
				{
					hurry((HurryTypes)iI);
					break;					
				}
			}
			if (AI_countGoodTiles((healthRate(0) == 0), false, 100) <= (getPopulation() - iHurryPopulation))
			{
*/
				// Only consider population hurry if that's actually what the city can do!!!
				//if( (iHurryPopulation > 0) && (getPopulation() > iHurryPopulation) )
				//{
				//	//BBAI TODO: could be (bEssential ? 100 : 80) instead
				//	if (AI_countGoodTiles((healthRate(0) == 0), false, bEssential ? 100 : 80) <= (iThreshold))
				//	{
				//		if( gCityLogLevel >= 2 )
				//		{
				//			logBBAI("      City %S hurry pop at %d for growth with bad tiles with pop %d", getName().GetCString(), iHurryPopulation, getPopulation() );
				//		}
				//		hurry((HurryTypes)iI);
				//		break;					
				//	}
				//}				
			}
		
			//if ((iHurryPopulation > 0) && (AI_countGoodTiles((healthRate(0) == 0), false, bEssential ? 100 : 80) <= (iThreshold)))
			//{
			//	if (getProductionTurnsLeft() > iMinTurns)
			//	{
			//		bWait = isHuman();
			//		
			//		if ((iHurryPopulation * 3) > (getProductionTurnsLeft() * 2))
			//		{
			//			bWait = true;
			//		}

			//		if (!bWait)
			//		{
			//			if (iHurryAngerLength > 0)
			//			{
			//				//is the whip just too small or the population just too reduced to bother?
			//				if (!bEssential && ((iHurryPopulation < (1 + GC.getDefineINT("HURRY_POP_ANGER"))) || ((getPopulation() - iHurryPopulation) <= std::max(3, (getHighestPopulation() / 2)))))
			//				{
			//					bWait = true;
			//				}
			//				else
			//				{
			//					//sometimes it's worth whipping even with existing anger
			//					if (getHurryAngerTimer() > 1)
			//					{
			//						if (!bEssential)
			//						{
			//							bWait = true;
			//						}
			//						else if (GC.getDefineINT("HURRY_POP_ANGER") == iHurryPopulation && angryPopulation() > 0)
			//						{
			//							//ideally we'll whip something more expensive
			//							bWait = true;
			//						}
			//					}
			//				}

			//				//if the city is just lame then don't whip the poor thing
			//				//(it'll still get whipped when unhappy/unhealthy)
			//				if (!bWait && !bEssential)
			//				{
			//					int iFoodSurplus = 0;
			//					CvPlot * pLoopPlot;

			//					for (int iJ = 0; iJ < getNumCityPlots(); iJ++)
			//					{
			//						if (iJ != CITY_HOME_PLOT)
			//						{
			//							pLoopPlot = getCityIndexPlot(iJ);

			//							if (pLoopPlot != NULL)
			//							{
			//								if (pLoopPlot->getWorkingCity() == this)
			//								{
			//									iFoodSurplus += std::max(0, pLoopPlot->getYield(YIELD_FOOD) - GC.getFOOD_CONSUMPTION_PER_POPULATION());
			//								}
			//							}
			//						}
			//					}

			//					if (iFoodSurplus < 3)
			//					{
			//						bWait = true;
			//					}
			//				}
			//			}
			//		}

			//		if (!bWait)
			//		{
			//			if( gCityLogLevel >= 2 )
			//			{
			//				logBBAI("      City %S hurry pop at %d with bad tiles and no reason to wait with pop %d", getName().GetCString(), iHurryPopulation, getPopulation() );
			//			}
			//			hurry((HurryTypes)iI);
			//			break;
			//		}
			//	}
			//}
		}
	}
}


// Improved use of emphasize by Blake, to go with his whipping strategy - thank you!
void CvCityAI::AI_doEmphasize()
{
	//Note from Blake:
	//Emphasis proved to be too clumsy to manage AI economies,
	//as such it's been nearly completely phased out by 
	//the AI_specialYieldMultiplier system which allows arbitary
	//value-boosts and works very well.
	//Ideally the AI should never use emphasis.

	PROFILE_FUNC();

	FAssert(!isHuman());

	bool bFirstTech = false;
	if (GET_PLAYER(getOwnerINLINE()).getCurrentResearch() != NO_TECH)
	{
		bFirstTech = GET_PLAYER(getOwnerINLINE()).AI_isFirstTech(GET_PLAYER(getOwnerINLINE()).getCurrentResearch());
	}

	bool bCultureVictory = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	int iPopulationRank = findPopulationRank();

	for (int iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		bool bEmphasize = false;

		if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getYieldChange(YIELD_FOOD) > 0)
		{

		}

		if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getYieldChange(YIELD_PRODUCTION) > 0)
		{

		}
		
		if (AI_specialYieldMultiplier(YIELD_PRODUCTION) < 50)
		{
			if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getYieldChange(YIELD_COMMERCE) > 0)
			{
				if (bFirstTech)
				{
					bEmphasize = true;
				}
			}

			if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getCommerceChange(COMMERCE_RESEARCH) > 0)
			{
				if (bFirstTech && !bCultureVictory)
				{
					if (iPopulationRank < ((GET_PLAYER(getOwnerINLINE()).getNumCities() / 4) + 1))
					{
						bEmphasize = true;
					}
				}
			}
			
			if (GC.getEmphasizeInfo((EmphasizeTypes)iI).isGreatPeople())
			{
				int iHighFoodTotal = 0;
				int iHighFoodPlotCount = 0;
				int iHighHammerPlotCount = 0;
				int iHighHammerTotal = 0;
				int iGoodFoodSink = 0;
				int iFoodPerPop = GC.getFOOD_CONSUMPTION_PER_POPULATION();
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
				for (int iPlot = 0; iPlot < getNumCityPlots(); iPlot++)
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
				{
					CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iPlot);
					if (pLoopPlot != NULL && pLoopPlot->getWorkingCity() == this)
					{
						int iFood = pLoopPlot->getYield(YIELD_FOOD);
						if (iFood > iFoodPerPop)
						{
							iHighFoodTotal += iFood;
							iHighFoodPlotCount++;
						}
						int iHammers = pLoopPlot->getYield(YIELD_PRODUCTION);
						if ((iHammers >= 3) && ((iHammers + iFood) >= 4))
						{
							iHighHammerPlotCount++;
							iHighHammerTotal += iHammers;
						}
						int iCommerce = pLoopPlot->getYield(YIELD_COMMERCE);
						if ((iCommerce * 2 + iHammers * 3) > 9)
						{
							iGoodFoodSink += std::max(0, iFoodPerPop - iFood);
						}
					}
				}
				
				if ((iHighFoodTotal + iHighFoodPlotCount - iGoodFoodSink) >= foodConsumption(true))
				{
					if ((iHighHammerPlotCount < 2) && (iHighHammerTotal < (getPopulation())))
					{
						if (AI_countGoodTiles(true, false, 100, true) < getPopulation())
						{
							bEmphasize = true;
						}
					}
				}
			}
		}

		AI_setEmphasize(((EmphasizeTypes)iI), bEmphasize);
	}
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/09/10                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
/********************************************************************************/
/* 	City Defenders						24.07.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bIgnoreNotUnitAIs
bool CvCityAI::AI_chooseUnit(const char* reason, UnitAITypes eUnitAI, int iOdds, int iUnitStrength, int iPriorityOverride, CvUnitSelectionCriteria* criteria)
{//Adding a unit type direct selection here...

#ifdef USE_UNIT_TENDERING
	//	Have we already contracted for a unit?
	if ( m_bRequestedUnit )
	{
		return false;
	}
#endif

	if( iOdds < 0 || GC.getGameINLINE().getSorenRandNum(100, "City AI choose unit") < iOdds )
	{
		if ( iPriorityOverride == -1 )
		{
			iPriorityOverride = m_iTempBuildPriority;
		}

		CvString unitAIType;
		if (eUnitAI != NO_UNITAI)
		{
			CvInfoBase& AIType = GC.getUnitAIInfo(eUnitAI);
			unitAIType = AIType.getType();
		}
		else
		{
			unitAIType = "NO_UNITAI";
		}

#ifdef USE_UNIT_TENDERING
		if ( !isNPC() )
		{
			//	Check someone can build a suitable unit before putting out a tender for one.
			if ( eUnitAI == NO_UNITAI || GET_PLAYER(getOwnerINLINE()).bestBuildableUnitForAIType(NO_DOMAIN, eUnitAI, criteria) != NO_UNIT )
			{
				if( gCityLogLevel >= 2 ) 
				{
					logBBAI("      City %S pop %d puts out tenders for %d unit strength of AIType: %s at priority %d (reason: %s)", getName().GetCString(), getPopulation(), iUnitStrength, unitAIType.c_str(), iPriorityOverride, reason);
				}

				//	Put out a tender for this unit
				GET_PLAYER(getOwnerINLINE()).getContractBroker().advertiseWork(iPriorityOverride,
																				NO_UNITCAPABILITIES,
																				getX_INLINE(),
																				getY_INLINE(),
																				NULL,
																				eUnitAI,
																				iUnitStrength,
																				criteria,
																				MAX_INT);

				m_bRequestedUnit = true;

				return m_bRequestedBuilding;
			}
			//	Looks like we probably can't build it, but fallback check if we can ourselves, and if so just
			//	queue it here (handles edge cases where building requirements mean it can be built locally but
			//	not in capital or best coastal).  Do this by falling through to local handling
		}
#endif

		return AI_chooseUnitImmediate(reason, eUnitAI, criteria, NO_UNIT);
	}

	return false;
}

bool CvCityAI::AI_chooseUnitImmediate(const char* reason, UnitAITypes eUnitAI, CvUnitSelectionCriteria* criteria, UnitTypes eUnitType)
{
	UnitTypes	eBestUnit = NO_UNIT;

	if (eUnitType == NO_UNIT)
	{
		if (eUnitAI != NO_UNITAI)
		{
			int iDummyValue = 0;
			eBestUnit = AI_bestUnitAI(eUnitAI, iDummyValue, false, false, criteria);
		}
		else
		{
			int iDummyValue = 0;
			eBestUnit = AI_bestUnit(iDummyValue, -1, NULL, false, &eUnitAI, false, false, criteria);
		}
	}
	else
	{
		eBestUnit = eUnitType;
	}

	if (eBestUnit != NO_UNIT)
	{
		if( gCityLogLevel >= 2 )
		{
			CvString	unitAIType;

			if (eUnitAI != NO_UNITAI)
			{
				CvInfoBase& AIType = GC.getUnitAIInfo(eUnitAI);
				unitAIType = AIType.getType();
			}
			else
			{
				unitAIType = "NO_UNITAI";
			}

			logBBAI("      City %S pop %d builds unit of AIType: %s at priority %d (reason: %s)", getName().GetCString(), getPopulation(), unitAIType.c_str(), m_iTempBuildPriority, reason);
		}

		pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI, false, false, false);
		return true;
	}

	return false;
}

/********************************************************************************/
/* 	City Defenders												END 			*/
/********************************************************************************/
bool CvCityAI::AI_chooseUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	if (eUnit != NO_UNIT)
	{
		pushOrder(ORDER_TRAIN, eUnit, eUnitAI, false, false, false);
		return true;
	}
	return false;	
}

bool CvCityAI::AI_chooseDefender(const char* reason)
{
	if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_CITY_SPECIAL, -1, NULL, getOwnerINLINE()) == NULL)
	{
		if (AI_chooseUnit(reason, UNITAI_CITY_SPECIAL))
		{
			return true;
		}
	}

	if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, getOwnerINLINE()) == NULL)
	{
		if (AI_chooseUnit(reason, UNITAI_CITY_COUNTER))
		{
			return true;
		}
	}

	if (AI_chooseUnit(reason, UNITAI_CITY_DEFENSE))
	{
		return true;
	}

	return false;
}

bool CvCityAI::AI_chooseLeastRepresentedUnit(const char* reason, UnitTypeWeightArray &allowedTypes, int iOdds)
{
	if ( iOdds < 0 || iOdds > GC.getGameINLINE().getSorenRandNum(100, "AI choose least represented unit overall odds") )
	{
		std::multimap<int, UnitAITypes, std::greater<int> > bestTypes;
		int iTotalWeight = 0;
		
		for (UnitTypeWeightArray::iterator it = allowedTypes.begin(); it != allowedTypes.end(); ++it)
		{
			int iValue = it->second;
			iValue *= 750 + GC.getGameINLINE().getSorenRandNum(250, "AI choose least represented unit");
			iValue /= std::max(1,GET_PLAYER(getOwnerINLINE()).AI_totalAreaUnitAIs(area(), it->first));
			bestTypes.insert(std::make_pair(iValue, it->first));

			iTotalWeight += iValue/100;
		}
		
		int iChoiceWeight = GC.getGameINLINE().getSorenRandNum(iTotalWeight, "AI choose least represented unit");

		for (std::multimap<int, UnitAITypes, std::greater<int> >::iterator best_it = bestTypes.begin(); best_it != bestTypes.end(); ++best_it)
		{
			iChoiceWeight -= best_it->first/100;

			CvString reasonString;

			reasonString.Format("least represented (%s)", reason);

			if ( iChoiceWeight <= 0 && AI_chooseUnit(reasonString.c_str(), best_it->second) )
			{
				return true;
			}
		}
	}

	return false;
}

bool CvCityAI::AI_bestSpreadUnit(bool bMissionary, bool bExecutive, int iBaseChance, UnitTypes* eBestSpreadUnit, int* iBestSpreadUnitValue)
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	CvGame& kGame = GC.getGame();
	
	FAssert(eBestSpreadUnit != NULL && iBestSpreadUnitValue != NULL);

	int iBestValue = 0;

	if (bMissionary)
	{
		for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); iReligion++)
		{
			ReligionTypes eReligion = (ReligionTypes)iReligion;
/************************************************************************************************/
/* Afforess	                  Start		 09/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			int iNeededMissionaries = kPlayer.AI_neededMissionaries(area(), eReligion);
/*
			if (isHasReligion(eReligion))
*/
			if (isHasReligion(eReligion) && iNeededMissionaries > 0)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
			{
				int iHasCount = kPlayer.getHasReligionCount(eReligion);
				FAssert(iHasCount > 0);
				int iRoll = (iHasCount > 4) ? iBaseChance : (((100 - iBaseChance) / iHasCount) + iBaseChance);
				if (kPlayer.AI_isDoStrategy(AI_STRATEGY_MISSIONARY))
				{
					iRoll *= (kPlayer.getStateReligion() == eReligion) ? 170 : 65;
					iRoll /= 100;
				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/08/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI                                                                          */
/************************************************************************************************/
				if (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
				{
					iRoll += 25;
				}
				else if (!kTeam.hasHolyCity(eReligion) && !(kPlayer.getStateReligion() == eReligion))
				{
					iRoll /= 2;
					if (kPlayer.isNoNonStateReligionSpread())
					{
						iRoll /= 2;
					}
				}

/************************************************************************************************/
/* RevDCM	                  Start		 5/1/09                                                 */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
				if (kPlayer.isPushReligiousVictory() && (kPlayer.getStateReligion() == eReligion))
				{
					iRoll += 25;
				} else if(kPlayer.isConsiderReligiousVictory() && (kPlayer.getStateReligion() == eReligion))
				{
					iRoll += 7;
				} else if( (kPlayer.isPushReligiousVictory() || kPlayer.isConsiderReligiousVictory())
				&& (kPlayer.getStateReligion() != eReligion) )
				{
					iRoll = 0;
				}
/************************************************************************************************/
/* Inquisitions	                     END                                                        */
/************************************************************************************************/

				if (iRoll > kGame.getSorenRandNum(100, "AI choose missionary"))
				{
					int iReligionValue = kPlayer.AI_missionaryValue(area(), eReligion);
					if (iReligionValue > 0)
					{
						for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
						{
							UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

							if (eLoopUnit != NO_UNIT)
							{
								CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
								if (kUnitInfo.getReligionSpreads(eReligion) > 0)
								{
									if (canTrain(eLoopUnit))
									{
										int iValue = iReligionValue;
										iValue /= kUnitInfo.getProductionCost();
										
										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											*eBestSpreadUnit = eLoopUnit;
											*iBestSpreadUnitValue = iReligionValue;
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
	
	if (bExecutive)
	{
		for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); iCorporation++)
		{
			CorporationTypes eCorporation = (CorporationTypes)iCorporation;
			if (isActiveCorporation(eCorporation) && kPlayer.getHasCorporationCount(eCorporation) > 0)
			{
				int iHasCount = kPlayer.getHasCorporationCount(eCorporation);
				FAssert(iHasCount > 0);
				int iRoll = (iHasCount > 4) ? iBaseChance : (((100 - iBaseChance) / iHasCount) + iBaseChance);
				if (!kTeam.hasHeadquarters(eCorporation))
				{
					iRoll /= 8;			
				}
				
				if (iRoll > kGame.getSorenRandNum(100, "AI choose executive"))
				{
					int iCorporationValue = kPlayer.AI_executiveValue(area(), eCorporation);
					if (iCorporationValue > 0)
					{
						for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
						{
							UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

							if (eLoopUnit != NO_UNIT)
							{
								CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
								if (kUnitInfo.getCorporationSpreads(eCorporation) > 0)
								{
									if (canTrain(eLoopUnit))
									{
										int iValue = iCorporationValue;
										iValue /= kUnitInfo.getProductionCost();

										int iLoop;
										int iTotalCount = 0;
										int iPlotCount = 0;
										for (CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
										{
											if ((pLoopUnit->AI_getUnitAIType() == UNITAI_MISSIONARY) && (pLoopUnit->getUnitInfo().getCorporationSpreads(eCorporation) > 0))
											{
												iTotalCount++;
												if (pLoopUnit->plot() == plot())
												{
													iPlotCount++;
												}
											}
										}
										iCorporationValue /= std::max(1, (iTotalCount / 4) + iPlotCount);

										int iCost = std::max(0, GC.getCorporationInfo(eCorporation).getSpreadCost() * (100 + GET_PLAYER(getOwnerINLINE()).calculateInflationRate()));
										iCost /= 100;
										
										if (kPlayer.getEffectiveGold() >= iCost)
										{
											iCost *= GC.getDefineINT("CORPORATION_FOREIGN_SPREAD_COST_PERCENT");
											iCost /= 100;
											if (kPlayer.getEffectiveGold() < iCost && iTotalCount > 1)
											{
												iCorporationValue /= 2;
											}
										}
										else if (iTotalCount > 1)
										{
											iCorporationValue /= 5;
										}
										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											*eBestSpreadUnit = eLoopUnit;
											*iBestSpreadUnitValue = iCorporationValue;
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
	
	return (*eBestSpreadUnit != NULL);
}

bool CvCityAI::AI_chooseBuilding(int iFocusFlags, int iMaxTurns, int iMinThreshold, int iOdds, bool bMaximizeFlaggedValue, PropertyTypes eProperty)
{
#ifdef USE_UNIT_TENDERING
	//	Have we already selected a building?
	if ( m_bRequestedBuilding )
	{
		return false;
	}
			
	m_iBuildPriority = m_iTempBuildPriority;
#endif

	BuildingTypes eBestBuilding = AI_bestBuildingThreshold(iFocusFlags, iMaxTurns, iMinThreshold, false, NO_ADVISOR, bMaximizeFlaggedValue, eProperty);

	if (eBestBuilding != NO_BUILDING)
	{
		if( iOdds < 0 || 
			getBuildingProduction(eBestBuilding) > 0 ||
			GC.getGameINLINE().getSorenRandNum(100,"City AI choose building") < iOdds )
		{
			pushOrder(ORDER_CONSTRUCT, eBestBuilding, -1, false, false, false);

#ifdef USE_UNIT_TENDERING
			m_bRequestedBuilding = true;

			return (isNPC() || m_bRequestedUnit);
#else
			return true;
#endif
		}
	}

	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


bool CvCityAI::AI_chooseProject()
{
#ifdef USE_UNIT_TENDERING
	if ( m_bRequestedBuilding )
	{
		return false;
	}

	m_iBuildPriority = m_iTempBuildPriority;
#endif

	ProjectTypes eBestProject = AI_bestProject();

	if (eBestProject != NO_PROJECT)
	{
		pushOrder(ORDER_CREATE, eBestProject, -1, false, false, false);

		return true;
	}

	return false;
}


bool CvCityAI::AI_chooseProcess(CommerceTypes eCommerceType, int* commerceWeights)
{
#ifdef USE_UNIT_TENDERING
	if ( m_bRequestedBuilding || m_bRequestedUnit)
	{
		return false;
	}

	m_iBuildPriority = m_iTempBuildPriority;
#endif

	ProcessTypes eBestProcess = AI_bestProcess(eCommerceType, commerceWeights);

	if (eBestProcess != NO_PROCESS)
	{
		pushOrder(ORDER_MAINTAIN, eBestProcess, -1, false, false, false);

		return true;
	}

	return false;
}


// Returns true if a worker was added to a plot...
bool CvCityAI::AI_addBestCitizen(bool bWorkers, bool bSpecialists, int* piBestPlot, SpecialistTypes* peBestSpecialist)
{
	PROFILE_FUNC();

	bool bAvoidGrowth = AI_avoidGrowth();
	bool bIgnoreGrowth = AI_ignoreGrowth();
	bool bIsSpecialistForced = false;

	int iBestSpecialistValue = 0;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;
	SpecialistTypes eBestForcedSpecialist = NO_SPECIALIST;

	if (bSpecialists)
	{
		// count the total forced specialists
		int iTotalForcedSpecialists = 0;
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			int iForcedSpecialistCount = getForceSpecialistCount((SpecialistTypes)iI);
			if (iForcedSpecialistCount > 0)
			{
				bIsSpecialistForced = true;
				iTotalForcedSpecialists += iForcedSpecialistCount;
			}
		}
		
		// if forcing any specialists, find the best one that we can still assign
		if (bIsSpecialistForced)
		{
			int iBestForcedValue = MIN_INT;
			
			int iTotalSpecialists = 1 + getSpecialistPopulation();
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				if (isSpecialistValid((SpecialistTypes)iI, 1))
				{
					int iForcedSpecialistCount = getForceSpecialistCount((SpecialistTypes)iI);
					if (iForcedSpecialistCount > 0)
					{
						int iSpecialistCount = getSpecialistCount((SpecialistTypes)iI);

						// the value is based on how close we are to our goal ratio forced/total
						int iForcedValue = ((iForcedSpecialistCount * 128) / iTotalForcedSpecialists) -  ((iSpecialistCount * 128) / iTotalSpecialists);
						if (iForcedValue >= iBestForcedValue)
						{
							int iSpecialistValue = AI_specialistValue((SpecialistTypes)iI, bAvoidGrowth, false);
							
							// if forced value larger, or if equal, does this specialist have a higher value
							if (iForcedValue > iBestForcedValue || iSpecialistValue > iBestSpecialistValue)
							{					
								iBestForcedValue = iForcedValue;
								iBestSpecialistValue = iSpecialistValue;
								eBestForcedSpecialist = ((SpecialistTypes)iI);
								eBestSpecialist = eBestForcedSpecialist;
							}
						}
					}
				}
			}
		}
		
		// if we do not have a best specialist yet, then just find the one with the best value
		if (eBestSpecialist == NO_SPECIALIST)
		{
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				if (isSpecialistValid((SpecialistTypes)iI, 1))
				{
					int iValue = AI_specialistValue(((SpecialistTypes)iI), bAvoidGrowth, false);
					if (iValue >= iBestSpecialistValue)
					{
						iBestSpecialistValue = iValue;
						eBestSpecialist = ((SpecialistTypes)iI);
					}
				}
			}
		}
	}

	int iBestPlotValue = 0;
	int iBestPlot = -1;
	if (bWorkers)
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				if (!isWorkingPlot(iI))
				{
					CvPlot* pLoopPlot = getCityIndexPlot(iI);

					if (pLoopPlot != NULL)
					{
						if (canWork(pLoopPlot))
						{
							int iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ false, /*bIgnoreFood*/ false, bIgnoreGrowth);

							if (iValue > iBestPlotValue)
							{
								iBestPlotValue = iValue;
								iBestPlot = iI;
							}
						}
					}
				}
			}
		}
	}
	
	// if we found a plot to work
	if (iBestPlot != -1)
	{
		// if the best plot value is better than the best specialist, or if we forcing and we could not assign a forced specialst
		if (iBestPlotValue > iBestSpecialistValue || (bIsSpecialistForced && eBestForcedSpecialist == NO_SPECIALIST))
		{
			// do not work the specialist
			eBestSpecialist = NO_SPECIALIST;
		}
	}
	
	if (eBestSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eBestSpecialist, 1);
		if (piBestPlot != NULL)
		{
			FAssert(peBestSpecialist != NULL);
			*peBestSpecialist = eBestSpecialist;
			*piBestPlot = -1;
		}
		return true;
	}
	else if (iBestPlot != -1)
	{
		setWorkingPlot(iBestPlot, true);
		if (piBestPlot != NULL)
		{
			FAssert(peBestSpecialist != NULL);
			*peBestSpecialist = NO_SPECIALIST;
			*piBestPlot = iBestPlot;
			
		}
		return true;
	}

	return false;
}


// Returns true if a worker was removed from a plot...
bool CvCityAI::AI_removeWorstCitizen(SpecialistTypes eIgnoreSpecialist)
{
	// if we are using more specialists than the free ones we get
	if (extraFreeSpecialists() < 0)
	{
		// does generic 'citizen' specialist exist?
		if (GC.getDefineINT("DEFAULT_SPECIALIST") != NO_SPECIALIST)
		{
			// is ignore something other than generic citizen?
			if (eIgnoreSpecialist != GC.getDefineINT("DEFAULT_SPECIALIST"))
			{
				// do we have at least one more generic citizen than we are forcing?
				if (getSpecialistCount((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))) > getForceSpecialistCount((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))))
				{
					// remove the extra generic citzen
					changeSpecialistCount(((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))), -1);
					return true;
				}
			}
		}
	}

	bool bAvoidGrowth = AI_avoidGrowth();
	bool bIgnoreGrowth = AI_ignoreGrowth();

	int iWorstValue = MAX_INT;
	SpecialistTypes eWorstSpecialist = NO_SPECIALIST;
	int iWorstPlot = -1;

	// if we are using more specialists than the free ones we get
	if (extraFreeSpecialists() < 0)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (eIgnoreSpecialist != iI)
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
			if (isWorkingPlot(iI))
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
		changeSpecialistCount(eWorstSpecialist, -1);
		return true;
	}
	else if (iWorstPlot != -1)
	{
		setWorkingPlot(iWorstPlot, false);
		return true;
	}
	
	// if we still have not removed one, then try again, but do not ignore the one we were told to ignore
	if (extraFreeSpecialists() < 0)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (getSpecialistCount((SpecialistTypes)iI) > 0)
			{
				int iValue = AI_specialistValue(((SpecialistTypes)iI), bAvoidGrowth, /*bRemove*/ true);

				if (iValue < iWorstValue)
				{
					iWorstValue = iValue;
					eWorstSpecialist = ((SpecialistTypes)iI);
				}
			}
		}
	}

	if (eWorstSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eWorstSpecialist, -1);
		return true;
	}

	return false;
}


void CvCityAI::AI_juggleCitizens()
{
	MEMORY_TRACK()

	bool bAvoidGrowth = AI_avoidGrowth();
	bool bIgnoreGrowth = AI_ignoreGrowth();
	
	// one at a time, remove the worst citizen, then add the best citizen
	// until we add back the same one we removed
	for (int iPass = 0; iPass < 2; iPass++)
	{
		bool bCompletedChecks = false;
		int iCount = 0;

		//std::vector<int> aWorstPlots;
		bool worstPlots[NUM_CITY_PLOTS];
		memset(worstPlots, 0, sizeof(worstPlots));

		while (!bCompletedChecks)
		{
			int iLowestValue = MAX_INT;
			int iWorstPlot = -1;
			int iValue;

			for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
			{
				if (iI != CITY_HOME_PLOT)
				{
					if (isWorkingPlot(iI))
					{
						CvPlot* pLoopPlot = getCityIndexPlot(iI);

						if (pLoopPlot != NULL)
						{
							iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ true, /*bIgnoreFood*/ false, bIgnoreGrowth, (iPass == 0));

							// use <= so that we pick the last one that is lowest, to avoid infinite loop with AI_addBestCitizen
							if (iValue <= iLowestValue)
							{
								iLowestValue = iValue;
								iWorstPlot = iI;
							}
						}
					}
				}
			}

			// if no worst plot, or we looped back around and are trying to remove the first plot we removed, stop
			//if (iWorstPlot == -1 || std::find(aWorstPlots.begin(), aWorstPlots.end(), iWorstPlot) != aWorstPlots.end())
			if (iWorstPlot == -1 || worstPlots[iWorstPlot])
			{
				bCompletedChecks = true;
			}
			else
			{
				// if this the first worst plot, remember it
				//aWorstPlots.push_back(iWorstPlot);
				worstPlots[iWorstPlot] = true;

				setWorkingPlot(iWorstPlot, false);

				if (AI_addBestCitizen(true, true))
				{
					if (isWorkingPlot(iWorstPlot))
					{
						bCompletedChecks = true;
					}
				}
			}

			iCount++;
			if (iCount > (NUM_CITY_PLOTS + 1))
			{
				FAssertMsg(false, "infinite loop");
				break; // XXX
			}
		}

		if ((iPass == 0) && (foodDifference(false) >= 0))
		{
			//good enough, the starvation code
			break;
		}
	}
}


bool CvCityAI::AI_potentialPlot(short* piYields)
{
	int iNetFood = piYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION();

	if (iNetFood < 0)
	{
		if (piYields[YIELD_FOOD] == 0)
		{
			if (piYields[YIELD_PRODUCTION] + piYields[YIELD_COMMERCE] < 2)
			{
				return false;
			}
		}
		else
		{
			if (piYields[YIELD_PRODUCTION] + piYields[YIELD_COMMERCE] == 0)
			{
				return false;
			}
		}
	}

	return true;
}


bool CvCityAI::AI_foodAvailable(int iExtra)
{
	PROFILE_FUNC();

	bool abPlotAvailable[NUM_CITY_PLOTS];
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		abPlotAvailable[iI] = false;
	}

	int iFoodCount = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (iI == CITY_HOME_PLOT)
			{
				iFoodCount += pLoopPlot->getYield(YIELD_FOOD);
			}
			else if ((pLoopPlot->getWorkingCity() == this) && AI_potentialPlot(pLoopPlot->getYield()))
			{
				abPlotAvailable[iI] = true;
			}
		}
	}

	int iPopulation = (getPopulation() + iExtra);

	while (iPopulation > 0)
	{
		int iBestValue = 0;
		int iBestPlot = CITY_HOME_PLOT;

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
			if (abPlotAvailable[iI])
			{
				int iValue = getCityIndexPlot(iI)->getYield(YIELD_FOOD);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					iBestPlot = iI;
				}
			}
		}

		if (iBestPlot != CITY_HOME_PLOT)
		{
			iFoodCount += iBestValue;
			abPlotAvailable[iBestPlot] = false;
		}
		else
		{
			break;
		}

		iPopulation--;
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iFoodCount += (GC.getSpecialistInfo((SpecialistTypes)iI).getYieldChange(YIELD_FOOD) * getFreeSpecialistCount((SpecialistTypes)iI));
	}

	if (iFoodCount < foodConsumption(false, iExtra))
	{
		return false;
	}

	return true;
}

#ifdef YIELD_VALUE_CACHING
void CvCityAI::AI_NoteSpecialistChange()
{
	ClearYieldValueCache();
}

void CvCityAI::AI_NoteWorkerChange()
{
	ClearYieldValueCache();
}

void CvCityAI::ClearYieldValueCache()
{
	ClearYieldValueCacheImpl();
}

void CvCityAI::ClearYieldValueCacheImpl()
{
	if ( yieldValueCache.currentUseCounter > 0 )
	{
		yieldValueCache.currentUseCounter = 0;

		for(int i = 0; i < YIELD_VALUE_CACHE_SIZE; i++)
		{
			yieldValueCache.entries[i].iLastUseCount = 0;
		}
	}
}

void CvCityAI::CheckYieldValueCache(char* label)
{
	if ( yieldValueCache.currentUseCounter > 0 )
	{
		for(int i = 0; i < YIELD_VALUE_CACHE_SIZE; i++)
		{
			struct yieldValueCacheEntry* entry = &yieldValueCache.entries[i];

			if ( entry->iLastUseCount != 0 )
			{
				int realValue = AI_yieldValueInternal(entry->aiYields, entry->aiCommerceYields, entry->bAvoidGrowth, entry->bRemove, entry->bIgnoreFood, entry->bIgnoreGrowth, entry->bIgnoreStarvation, entry->bWorkerOptimization);

				FAssertMsg( realValue == entry->iResult, CvString::format("CheckYieldValueCache %s failed\n", label).c_str());
			}
		}
	}
}

int CvCityAI::yieldValueCacheHits = 0;
int CvCityAI::yieldValueCacheReads = 0;
#endif

int CvCityAI::AI_yieldValue(short* piYields, short* piCommerceYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation, bool bWorkerOptimization)
{
#ifdef YIELD_VALUE_CACHING
#ifdef _DEBUG
//	Uncomment this to perform functional verification
//#define VERIFY_YIELD_CACHE_RESULTS
#endif

	//	Check cache first
	int worstLRU = 0x7FFFFFFF;

	struct yieldValueCacheEntry* worstLRUEntry = NULL;
	yieldValueCacheReads++;

	//OutputDebugString(CvString::format("AI_yieldValue (%d,%d,%d) at seq %d\n", piYields[0], piYields[1], piYields[2], yieldValueCacheReads).c_str());
	//PROFILE_STACK_DUMP

	for(int i = 0; i < YIELD_VALUE_CACHE_SIZE; i++)
	{
		struct yieldValueCacheEntry* entry = &yieldValueCache.entries[i];
		if ( entry->iLastUseCount == 0 )
		{
			worstLRUEntry = entry;
			break;
		}

		bool bMatch = true;

		for( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( piYields[j] != entry->aiYields[j] )
			{
				bMatch = false;
				break;
			}
		}
		if ( bMatch )
		{
			if ( piCommerceYields != NULL )
			{
				for( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
				{
					if ( piCommerceYields[j] != entry->aiCommerceYields[j] )
					{
						bMatch = false;
						break;
					}
				}
			}
			else
			{
				for( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
				{
					if ( entry->aiCommerceYields[j] != 0 )
					{
						bMatch = false;
						break;
					}
				}
			}
		}

		if ( bMatch &&
			 entry->bAvoidGrowth == bAvoidGrowth &&
			 entry->bRemove == bRemove &&
			 entry->bIgnoreFood == bIgnoreFood &&
			 entry->bIgnoreGrowth == bIgnoreGrowth &&
			 entry->bIgnoreStarvation == bIgnoreStarvation)
		{
			entry->iLastUseCount = ++yieldValueCache.currentUseCounter;
			yieldValueCacheHits++;
#ifdef VERIFY_YIELD_CACHE_RESULTS
			int realValue = AI_yieldValueInternal(piYields, piCommerceYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation, bWorkerOptimization);
			
			if ( realValue != entry->iResult )
			{
				OutputDebugString(CvString::format("Cache entry %08lx verification failed, turn is %d\n", entry, GC.getGameINLINE().getGameTurn()).c_str());
				FAssertMsg(false, "Yield value cache verification failure");
				CHECK_YIELD_VALUE_CACHE("Validation");
			}
#endif
			return entry->iResult;
		}
		else if ( entry->iLastUseCount < worstLRU )
		{
			worstLRU = entry->iLastUseCount;
			worstLRUEntry = entry;
		}
	}

	int iResult = AI_yieldValueInternal(piYields, piCommerceYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation, bWorkerOptimization);

	FAssertMsg(worstLRUEntry != NULL, "No plot value cache entry found to replace");
	if ( worstLRUEntry != NULL )
	{
		for( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			worstLRUEntry->aiYields[j] = piYields[j];
		}

		if ( piCommerceYields != NULL )
		{
			for( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
			{
				worstLRUEntry->aiCommerceYields[j] = piCommerceYields[j];
			}
		}
		else
		{
			for( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
			{
				worstLRUEntry->aiCommerceYields[j] = 0;
			}
		}

		worstLRUEntry->bAvoidGrowth = bAvoidGrowth;
		worstLRUEntry->bRemove = bRemove;
		worstLRUEntry->bIgnoreFood = bIgnoreFood;
		worstLRUEntry->bIgnoreGrowth = bIgnoreGrowth;
		worstLRUEntry->bIgnoreStarvation = bIgnoreStarvation;
		worstLRUEntry->bWorkerOptimization = bWorkerOptimization;
		worstLRUEntry->iResult = iResult;
		worstLRUEntry->iLastUseCount = ++yieldValueCache.currentUseCounter;
	}

	return iResult;
#else
	return AI_yieldValueInternal(piYields, piCommerceYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation, bWorkerOptimization);
#endif
}


int CvCityAI::AI_yieldValueInternal(short* piYields, short* piCommerceYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation, bool bWorkerOptimization)
{
	PROFILE_FUNC()

	const int iBaseProductionValue = 15;
	const int iBaseCommerceValue[NUM_COMMERCE_TYPES] = {7,10,7,7};	//	Koshling - boost science a bit
	
	const int iMaxFoodValue = (3 * iBaseProductionValue) - 1;

	int aiYields[NUM_YIELD_TYPES];
	int aiCommerceYieldsTimes100[NUM_COMMERCE_TYPES];

	int iExtraProductionModifier = 0;
	int iBaseProductionModifier = 100;
	
	bool bEmphasizeFood = AI_isEmphasizeYield(YIELD_FOOD);
	bool bFoodIsProduction = isFoodProduction();
	bool bCanPopRush = GET_PLAYER(getOwnerINLINE()).canPopRush();
	
	for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		aiCommerceYieldsTimes100[iJ] = 0;
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (piYields[iI] == 0)
		{
			aiYields[iI] = 0;
		}
		else
		{
			// Get yield for city after adding/removing the citizen in question
			//TB Traits begin
			int iOldCityYield = getModifiedBaseYieldRate((YieldTypes)iI);
			//TB Traits end
			int iNewCityYield = (bRemove ? (iOldCityYield - piYields[iI]) : (iOldCityYield + piYields[iI]));
			int iModifier = getBaseYieldRateModifier((YieldTypes)iI);
			if (iI == YIELD_PRODUCTION)
			{
				iBaseProductionModifier = iModifier;
				iExtraProductionModifier = getProductionModifier();
				iModifier += iExtraProductionModifier;
			}

			iNewCityYield = (iNewCityYield * iModifier) / 100;
			iOldCityYield = (iOldCityYield * iModifier) / 100;

			// The yield of the citizen in question is the difference of total yields
			// to account for rounding of modifiers
			aiYields[iI] = (bRemove ? (iOldCityYield - iNewCityYield) : (iNewCityYield - iOldCityYield));
		}
	}
			
	for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		int iModifier = getTotalCommerceRateModifier((CommerceTypes)iJ);
					
		int iCommerceTimes100 = aiYields[YIELD_COMMERCE] * GET_PLAYER(getOwnerINLINE()).getCommercePercent((CommerceTypes)iJ);
		if (piCommerceYields != NULL)
		{
			iCommerceTimes100 += piCommerceYields[iJ] * 100;
		}
		aiCommerceYieldsTimes100[iJ] += (iCommerceTimes100 * iModifier) / 100;
	}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       07/09/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
/* original BTS code
	if (isProductionProcess() && !bWorkerOptimization)
	{
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			aiCommerceYieldsTimes100[iJ] += GC.getProcessInfo(getProductionProcess()).getProductionToCommerceModifier(iJ) * aiYields[YIELD_PRODUCTION];
		}

		aiYields[YIELD_PRODUCTION] = 0;
	}
*/
	// Above code causes governor and AI to heavily weight food when building any form of commerce,
	// which is not expected by human and does not seem to produce better results for AI either.  
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
	
	// should not really use this much, but making it accurate
	aiYields[YIELD_COMMERCE] = 0;
	for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		aiYields[YIELD_COMMERCE] += aiCommerceYieldsTimes100[iJ] / 100;
	}

	int iValue = 0;
	int iSlaveryValue = 0;
	
	int iFoodGrowthValue = 0;
	int iFoodGPPValue = 0;
	
	if (!bIgnoreFood && aiYields[YIELD_FOOD] != 0)
	{
		// tiny food factor, to ensure that even when we don't want to grow, 
		// we still prefer more food if everything else is equal
		iValue += (aiYields[YIELD_FOOD] * 1);

		int iFoodPerTurn = (foodDifference(false) - ((bRemove) ? aiYields[YIELD_FOOD] : 0));
		int iFoodLevel = getFood();
		int iFoodToGrow = growthThreshold();
		int iHealthLevel = goodHealth() - badHealth(/*bNoAngry*/ false, 0);
		int iHappinessLevel = (isNoUnhappiness() ? std::max(3, iHealthLevel + 5) : happyLevel() - unhappyLevel(0));
		int iPopulation = getPopulation();
		int	iExtraPopulationThatCanWork = std::min(iPopulation - range(-iHappinessLevel, 0, iPopulation) + std::min(0, extraFreeSpecialists()) , NUM_CITY_PLOTS) - getWorkingPopulation() + ((bRemove) ? 1 : 0);

		int iPopulationExponent = (iPopulation - 1); // Each pop past the first increases consumption per population by .1, rounded down.  Each point of population means more actual people the higher the amount goes.
		int iConsumptionPerPopulationBase = iPopulationExponent + (GC.getFOOD_CONSUMPTION_PER_POPULATION() * 10);
		int iConsumptionbyPopulation = (iPopulation * iConsumptionPerPopulationBase) / 10;

		int iAdjustedFoodDifference = (getYieldRate(YIELD_FOOD) + std::min(0, iHealthLevel)) - iConsumptionbyPopulation;
		
		// if we not human, allow us to starve to half full if avoiding growth
		if (!bIgnoreStarvation)
		{
			int iStarvingAllowance = 0;
			if (bAvoidGrowth && !isHuman())
			{
				iStarvingAllowance = std::max(0, (iFoodLevel - std::max(1, ((9 * iFoodToGrow) / 10))));
			}
			
			if ((iStarvingAllowance < 1) && (iFoodLevel > ((iFoodToGrow * 75) / 100)))
			{
				iStarvingAllowance = 1;
			}
			
			// if still starving
			if ((iFoodPerTurn + iStarvingAllowance) < 0)
			{
				// if working plots all like this one will save us from starving
				if (std::max(0, iExtraPopulationThatCanWork * aiYields[YIELD_FOOD]) >= -iFoodPerTurn)
				{
					// if this is high food, then we want to pick it first, this will allow us to pick some great non-food later
					int iHighFoodThreshold = std::min(getBestYieldAvailable(YIELD_FOOD), iConsumptionbyPopulation + 1);
					if (iFoodPerTurn <= (AI_isEmphasizeGreatPeople() ? 0 : -iHighFoodThreshold) && aiYields[YIELD_FOOD] >= iHighFoodThreshold)
					{
						// value all the food that will contribute to not starving
						iValue += 2048 * std::min(aiYields[YIELD_FOOD], -iFoodPerTurn);
					}
					else
					{
						// give a huge boost to this plot, but not based on how much food it has
						// ie, if working a bunch of 1f 7h plots will stop us from starving, then do not force working unimproved 2f plot
						iValue += 2048;
					}
				}
				else
				{
					// value food high(32), but not forced
					iValue += 32 * std::min(aiYields[YIELD_FOOD], -iFoodPerTurn);
				}
			}
		}
		
		// if food isn't production, then adjust for growth
		if (bWorkerOptimization || !bFoodIsProduction)
		{
			if (!bAvoidGrowth)
			{
				int iPopToGrow = 0;
				// only do relative checks on food if we want to grow AND we not emph food
				// the emp food case will just give a big boost to all food under all circumstances
				if (bWorkerOptimization || (!bIgnoreGrowth))// && !bEmphasizeFood))
				{
					// also avail: iFoodLevel, iFoodToGrow
					
					// adjust iFoodPerTurn assuming that we work plots all equal to iConsumtionPerPop
					// that way it is our guesstimate of how much excess food we will have
					iFoodPerTurn += (iExtraPopulationThatCanWork * iConsumptionbyPopulation);
					
					// we have less than 10 extra happy, do some checks to see if we can increase it
					if (iHappinessLevel < 10)
					{
						// if we have anger becase no military, do not count it, on the assumption that it will 
						// be remedied soon, and that we still want to grow
						if (getMilitaryHappinessUnits() == 0)
						{
							if (GET_PLAYER(getOwnerINLINE()).getNumCities() > 2)
							{
								iHappinessLevel += ((GC.getDefineINT("NO_MILITARY_PERCENT_ANGER") * (iPopulation + 1)) / GC.getPERCENT_ANGER_DIVISOR());
							}
						}

						// currently we can at most increase happy by 2 in the following checks
						const int kMaxHappyIncrease = 2;

						// if happy is large enough so that it will be over zero after we do the checks
						int iNewFoodPerTurn = iFoodPerTurn + aiYields[YIELD_FOOD] - iConsumptionbyPopulation;
						if ((iHappinessLevel + kMaxHappyIncrease) > 0 && iNewFoodPerTurn > 0)
						{
							int iApproxTurnsToGrow = (iNewFoodPerTurn > 0) ? ((iFoodToGrow - iFoodLevel) / iNewFoodPerTurn) : MAX_INT;

							// do we have hurry anger?
							int iHurryAngerTimer = getHurryAngerTimer();
							if (iHurryAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iHurryAngerTimer % flatHurryAngerLength();
								
								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iHappinessLevel++;
								}
							}

							// do we have conscript anger?
							int iConscriptAngerTimer = getConscriptAngerTimer();
							if (iConscriptAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iConscriptAngerTimer % flatConscriptAngerLength();
								
								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iHappinessLevel++;
								}
							}

							// do we have defy resolution anger?
							int iDefyResolutionAngerTimer = getDefyResolutionAngerTimer();
							if (iDefyResolutionAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iDefyResolutionAngerTimer % flatDefyResolutionAngerLength();

								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iHappinessLevel++;
								}
							}
						}
					}
					
					if (bEmphasizeFood)
					{
						//If we are emphasize food, pay less heed to caps.
						iHealthLevel += 5;
						iHappinessLevel += 2;
					}

					bool bBarFull = (iFoodLevel + iFoodPerTurn /*+ aiYields[YIELD_FOOD]*/ > ((90 * iFoodToGrow) / 100));

					iPopToGrow = std::max(0, iHappinessLevel);
					int iGoodTiles = AI_countGoodTiles(iHealthLevel > 0, true, 50, true);
					iGoodTiles += AI_countGoodSpecialists(iHealthLevel > 0);
					iGoodTiles += bBarFull ? 0 : 1;

					if (!bEmphasizeFood)
					{
						iPopToGrow = std::min(iPopToGrow, iGoodTiles + ((bRemove) ? 1 : 0));
					}

					// if we have growth pontential, fill food bar to 85%
					bool bFillingBar = false;
					if (iPopToGrow == 0 && iHappinessLevel >= 0 && iGoodTiles >= 0 && iHealthLevel >= 0)
					{
						if (!bBarFull)
						{
							if (AI_specialYieldMultiplier(YIELD_PRODUCTION) < 50)
							{
								bFillingBar = true;
							}
						}
					}
					
					if (getPopulation() < 3)
					{
						iPopToGrow = std::max(iPopToGrow, 3 - getPopulation());
						iPopToGrow += 2;
					}

					// if we want to grow
					if (iPopToGrow > 0 || bFillingBar)
					{
						
						// will multiply this by factors
						iFoodGrowthValue = aiYields[YIELD_FOOD];
						if (iHealthLevel < (bFillingBar ? 0 : 1))
						{
							iFoodGrowthValue--;
						}

						// (range 1-25) - we want to grow more if we have a lot of growth to do
						// factor goes up like this: 0:1, 1:8, 2:9, 3:10, 4:11, 5:13, 6:14, 7:15, 8:16, 9:17, ... 17:25
						int iFactorPopToGrow;

						if (iPopToGrow < 1 || bFillingBar)
							iFactorPopToGrow = 20 - (10 * (iFoodLevel + iFoodPerTurn + aiYields[YIELD_FOOD])) / iFoodToGrow;
						else if (iPopToGrow < 7)
							iFactorPopToGrow = 17 + 3 * iPopToGrow;
						else
							iFactorPopToGrow = 41;
						
						iFoodGrowthValue *= iFactorPopToGrow;
						
						//If we already grow somewhat fast, devalue further food
						//Remember growth acceleration is not dependent on food eaten per 
						//pop, 4f twice as fast as 2f twice as fast as 1f...
						int iHighGrowthThreshold = 2 + std::max(std::max(0, 5 - getPopulation()), (iPopToGrow + 1) / 2);
						if (bEmphasizeFood)
						{
							iHighGrowthThreshold *= 2;
						}
						
						if (iFoodPerTurn > iHighGrowthThreshold)
						{
							iFoodGrowthValue *= 25 + ((75 * iHighGrowthThreshold) / iFoodPerTurn);
							iFoodGrowthValue /= 100;
						}
					}
				}
			
				//very high food override
				if ((isHuman()) && ((iPopToGrow > 0) || bCanPopRush))
				{
					//very high food override
					int iTempValue = std::max(0, 30 * aiYields[YIELD_FOOD] - 15 * iConsumptionbyPopulation);
					iTempValue *= std::max(0, 3 * iConsumptionbyPopulation - iAdjustedFoodDifference);
					iTempValue /= 3 * std::max(1,iConsumptionbyPopulation);
					if (iHappinessLevel < 0)
					{
						iTempValue *= 2;
						iTempValue /= 1 + 2 * -iHappinessLevel;
					}
					iFoodGrowthValue += iTempValue;
				}
				//Slavery Override
				if (bCanPopRush && (iHappinessLevel > 0))
				{
					iSlaveryValue = 30 * 14 * std::max(0, aiYields[YIELD_FOOD] - ((iHealthLevel < 0) ? 1 : 0));
					iSlaveryValue /= std::max(10, (growthThreshold() * (100 - getMaxFoodKeptPercent())));
					
					iSlaveryValue *= 100;
					iSlaveryValue /= getHurryCostModifier(true);
					
					iSlaveryValue *= iConsumptionbyPopulation * 2;
					iSlaveryValue /= iConsumptionbyPopulation * 2 + std::max(1, iAdjustedFoodDifference);
				}
				
				//Great People Override
				if ((iExtraPopulationThatCanWork > 1) && AI_isEmphasizeGreatPeople())
				{
					int iAdjust = iConsumptionbyPopulation;
					if (iFoodPerTurn == 0)
					{
						iAdjust -= 1;
					}
					iFoodGPPValue += std::max(0, aiYields[YIELD_FOOD] - iAdjust) * std::max(0, (12 + 5 * std::min(0, iHappinessLevel)));
				}
			}
		}
	}
	
	
	int iProductionValue = 0;
	int iCommerceValue = 0;
	int iFoodValue = std::min(iFoodGrowthValue, iMaxFoodValue * aiYields[YIELD_FOOD]);
	// if food is production, the count it
	int adjustedYIELD_PRODUCTION = (((bFoodIsProduction) ? aiYields[YIELD_FOOD] : 0) + aiYields[YIELD_PRODUCTION]);
			
	// value production medium(15)
	iProductionValue += (adjustedYIELD_PRODUCTION * iBaseProductionValue);
	if (!isProduction() && !isHuman())
	{
		iProductionValue /= 2;
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/18/09                                jdog5000      */
/*                                                                                              */
/* City AI				                                                                         */
/************************************************************************************************/
	// If city has more than enough food, but very little production, add large value to production
	// Particularly helps coastal cities with plains forests
	if( aiYields[YIELD_PRODUCTION] > 0 )
	{
		if( !bFoodIsProduction && isProduction() )
		{
			if( foodDifference(false) >= GC.getFOOD_CONSUMPTION_PER_POPULATION() )
			{
				if( getYieldRate(YIELD_PRODUCTION) < (1 + getPopulation()/3) )
				{
					iValue += 128 + 8 * aiYields[YIELD_PRODUCTION];
				}
			}
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	// value commerce low(6)

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (aiCommerceYieldsTimes100[iI] != 0)
		{
			int iCommerceWeight = GET_PLAYER(getOwnerINLINE()).AI_commerceWeight((CommerceTypes)iI);
			if (AI_isEmphasizeCommerce((CommerceTypes)iI))
			{
				iCommerceWeight *= 200;
				iCommerceWeight /= 100;
			}
			if (iI == COMMERCE_CULTURE)
			{
				if (getCultureLevel() <= (CultureLevelTypes) 1)
				{
					iCommerceValue += (15 * aiCommerceYieldsTimes100[iI]) / 100;
				}
			}
			//	Koshling - Removing the 'normalization' by AI_averageCommerceExchange() from this calculation.  Basically what this did was
			//	to say that commerce types we have a civilization-wide good multiplier for should be devalued relative to those we don't.
			//	However, within the context of city yield evaluation it's not being compared just with other commerce types, but also with other
			//	yield types which have no similar normalization.  The result was that as average science multipliers increased (via buildings and
			//	so on) production was valued relatively more and more over science
			//iCommerceValue += (iCommerceWeight * (aiCommerceYieldsTimes100[iI] * iBaseCommerceValue) * GET_PLAYER(getOwnerINLINE()).AI_averageCommerceExchange((CommerceTypes)iI)) / 1000000;
			iCommerceValue += (iCommerceWeight * (aiCommerceYieldsTimes100[iI] * iBaseCommerceValue[iI])) / 10000;
		}
	}
/*
	if (!bWorkerOptimization && bEmphasizeFood)
	{
		if (!bFoodIsProduction)
		{
			// value food extremely high(180)
			iFoodValue *= 125;
			iFoodValue /= 100;
		}
	}
	
	if (!bWorkerOptimization && AI_isEmphasizeYield(YIELD_PRODUCTION))
	{
		// value production high(80)
		iProductionValue += (adjustedYIELD_PRODUCTION * 80);
	}
*/
	//Slavery translation
	if ((iSlaveryValue > 0) && (iSlaveryValue > iFoodValue))
	{
		//treat the food component as production
		iFoodValue = 0;
	}
	else
	{
		//treat it as just food
		iSlaveryValue = 0;
	}
	
	iFoodValue += iFoodGPPValue;
/*
	if (!bWorkerOptimization && AI_isEmphasizeYield(YIELD_COMMERCE))
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			iCommerceValue += ((iCommerceYields[iI] * 40) * GET_PLAYER(getOwnerINLINE()).AI_averageCommerceExchange((CommerceTypes)iI)) / 100;
		}
	}

	for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		if (!bWorkerOptimization && AI_isEmphasizeCommerce((CommerceTypes) iJ))
		{
			// value the part of our commerce that goes to our emphasis medium (40)
			iCommerceValue += (iCommerceYields[iJ] * 40);
		}
	}
*/
	//Lets have some fun with the multipliers, this basically bluntens the impact of
	//massive bonuses.....
	
	//normalize the production... this allows the system to account for rounding
	//and such while preventing an "out to lunch smoking weed" scenario with
	//unusually high transient production modifiers.
	//Other yields don't have transient bonuses in quite the same way.

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       05/16/10                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
	// Rounding can be a problem, particularly for small commerce amounts.  Added safe guards to make
	// sure commerce is counted, even if just a tiny amount.
	if (AI_isEmphasizeYield(YIELD_PRODUCTION))
	{
		iProductionValue *= 130;
		iProductionValue /= 100;
		
		if (isFoodProduction())
		{
			iFoodValue *= 130;
			iFoodValue /= 100;
		}
		
		if (!AI_isEmphasizeYield(YIELD_COMMERCE) && iCommerceValue > 0)
		{
			iCommerceValue *= 60;
			iCommerceValue /= 100;
			iCommerceValue = std::max(1, iCommerceValue);
		}
		if (!AI_isEmphasizeYield(YIELD_FOOD) && iFoodValue > 0)
		{
			iFoodValue *= 75;
			iFoodValue /= 100;
			iFoodValue = std::max(1, iFoodValue);
		}
	}
	if (AI_isEmphasizeYield(YIELD_FOOD))
	{
		if (!isFoodProduction())
		{
			iFoodValue *= 130;
			iFoodValue /= 100;
			iSlaveryValue *= 130;
			iSlaveryValue /= 100;
		}
	}
	if (AI_isEmphasizeYield(YIELD_COMMERCE))
	{
		iCommerceValue *= 130;
		iCommerceValue /= 100;
		if (!AI_isEmphasizeYield(YIELD_PRODUCTION) && iProductionValue > 0)
		{
			iProductionValue *= 75;
			iProductionValue /= 100;
			iProductionValue = std::max(1,iProductionValue);
		}
		if (!AI_isEmphasizeYield(YIELD_FOOD) && iFoodValue > 0)
		{
			//Don't supress twice.
			if (!AI_isEmphasizeYield(YIELD_PRODUCTION))
			{
				iFoodValue *= 80;
				iFoodValue /= 100;
				iFoodValue = std::max(1, iFoodValue);
			}
		}
	}
		
	if( iProductionValue > 0 )
	{
		if (isFoodProduction())
		{
			iProductionValue *= 100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_PRODUCTION));
			iProductionValue /= 100;		
		}
		else
		{
			iProductionValue *= iBaseProductionModifier;
			iProductionValue /= (iBaseProductionModifier + iExtraProductionModifier);
			
			iProductionValue += iSlaveryValue;
			iProductionValue *= (100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_PRODUCTION)));
			
			iProductionValue /= GET_PLAYER(getOwnerINLINE()).AI_averageYieldMultiplier(YIELD_PRODUCTION);
		}
	
		iValue += std::max(1,iProductionValue);
	}
	
	if( iCommerceValue > 0 )
	{
		iCommerceValue *= (100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_COMMERCE)));
		iCommerceValue /= GET_PLAYER(getOwnerINLINE()).AI_averageYieldMultiplier(YIELD_COMMERCE);
		iValue += std::max(1, iCommerceValue);
	}
//	
	if( iFoodValue > 0 )
	{
		iFoodValue *= 100;
		iFoodValue /= GET_PLAYER(getOwnerINLINE()).AI_averageYieldMultiplier(YIELD_FOOD);
		iValue += std::max(1, iFoodValue);
	}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
	
	return iValue;
}


int CvCityAI::AI_plotValue(CvPlot* pPlot, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation)
{
	PROFILE_FUNC();

	int iTotalPotential = getYieldRate(YIELD_FOOD) + pPlot->getYield(YIELD_FOOD);

	short aiYields[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		//TB first plot logic
		if ((YieldTypes)iI == YIELD_FOOD && getPopulation() <= 1 && (iTotalPotential - foodConsumption() <= 0))
		{
			bIgnoreFood = true;
			bIgnoreGrowth = true;
			bIgnoreStarvation = true;
			aiYields[iI] = 0;
		}
		else
		{
			aiYields[iI] = pPlot->getYield((YieldTypes)iI);
		}
	}

	ImprovementTypes eCurrentImprovement = pPlot->getImprovementType();
	ImprovementTypes eFinalImprovement = NO_IMPROVEMENT;

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		eFinalImprovement = GET_TEAM(getTeam()).finalImprovementUpgrade(eCurrentImprovement);
	}
	
	int iYieldValue = (AI_yieldValue(aiYields, NULL, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation) * 100);

	if (eFinalImprovement != NO_IMPROVEMENT)
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			int iYieldDiff = (pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iI), getOwnerINLINE()) - pPlot->calculateImprovementYieldChange(eCurrentImprovement, ((YieldTypes)iI), getOwnerINLINE()));
			aiYields[iI] += iYieldDiff;
		}
		int iFinalYieldValue = (AI_yieldValue(aiYields, NULL, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation) * 100);
		
		if (iFinalYieldValue > iYieldValue)
		{
			iYieldValue = (40 * iYieldValue + 60 * iFinalYieldValue) / 100;
		}
		else
		{
			iYieldValue = (60 * iYieldValue + 40 * iFinalYieldValue) / 100;
		}
	}
	// unless we are emph food (and also food not production)
	if (!(AI_isEmphasizeYield(YIELD_FOOD) && !isFoodProduction()))
	{
		// if this plot is super bad (less than 2 food and less than combined 2 prod/commerce
		if (!AI_potentialPlot(aiYields))
		{
			// undervalue it even more!
			iYieldValue /= 16;
		}
	}
	int iValue = iYieldValue;

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		if (pPlot->getBonusType(getTeam()) == NO_BONUS) // XXX double-check CvGame::doFeature that the checks are the same...
		{
			for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes) iI).getTechReveal())))
				{
					if (GC.getImprovementInfo(eCurrentImprovement).getImprovementBonusDiscoverRand(iI) > 0)
					{
						iValue += 35;
					}
				}
			}
		}
	}

	if ((eCurrentImprovement != NO_IMPROVEMENT) && (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT))
	{
		iValue += 200;
		iValue -= pPlot->getUpgradeTimeLeft(eCurrentImprovement, NO_PLAYER);		
	}

	return iValue;
}


int CvCityAI::AI_experienceWeight()
{
	return ((getProductionExperience() + getDomainFreeExperience(DOMAIN_SEA)) * 2);
}


int CvCityAI::AI_buildUnitProb()
{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/29/10                                jdog5000      */
/*                                                                                              */
/* City AI, Barbarian AI                                                                        */
/************************************************************************************************/
	int iProb = (GC.getLeaderHeadInfo(getPersonalityType()).getBuildUnitProb() + AI_experienceWeight());

	if (!isNPC() && GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
	{
		iProb /= 2;
	}
	else if( GET_TEAM(getTeam()).getHasMetCivCount(false) == 0 )
	{
		iProb /= 2;
	}
	// more units from cities with military production bonuses
	else
	{
		// Not used apparently
		//int iMilProd = getMilitaryProductionModifier();
		//for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		//{
		//	iMilProd += getUnitCombatProductionModifier((UnitCombatTypes)iI) / 4;
		//}
		iProb += std::min(15,getMilitaryProductionModifier()/4);
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* REVOLUTION_MOD                         11/08/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if( GET_PLAYER(getOwnerINLINE()).isRebel() )
	{
		iProb *= 4;
		iProb /= 3;
	}

/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 10/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (AI_isMilitaryProductionCity())
	{
		iProb *= 4;
		iProb /= 3;
	}
	else if (GET_PLAYER(getOwnerINLINE()).AI_getMilitaryProductionCityCount() > 0)
	{
		iProb *= 3;
		iProb /= 4;
	}
	if (AI_isNavalMilitaryProductionCity())
	{
		iProb *= 4;
		iProb /= 3;
	}
	else if (GET_PLAYER(getOwnerINLINE()).AI_getNavalMilitaryProductionCityCount() > 0)
	{
		iProb *= 3;
		iProb /= 4;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return iProb;
}



// Improved worker AI provided by Blake - thank you!
void CvCityAI::AI_bestPlotBuild(CvPlot* pPlot, int* piBestValue, BuildTypes* peBestBuild, int iFoodPriority, int iProductionPriority, int iCommercePriority, bool bChop, int iHappyAdjust, int iHealthAdjust, int iFoodChange)
{
	PROFILE_FUNC();

	if (piBestValue != NULL)
	{
		*piBestValue = 0;
	}
	if (peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	if (pPlot->getWorkingCity() != this)
	{
		return;
	}
	
	//When improving new plots only, count emphasis twice
	//helps to avoid too much tearing up of old improvements.
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/06/09                                jdog5000      */
/*                                                                                              */
/* Worker AI                                                                                    */
/************************************************************************************************/
	// AI no longer uses emphasis really, except for short term boosts to commerce.
	// Inappropriate to base improvements on short term goals.
	if( isHuman() )
	{
		if (pPlot->getImprovementType() == NO_IMPROVEMENT)
		{
			if (AI_isEmphasizeYield(YIELD_FOOD))
			{
				iFoodPriority *= 130;
				iFoodPriority /= 100;
			}
			if (AI_isEmphasizeYield(YIELD_PRODUCTION))
			{
				iProductionPriority *= 180;
				iProductionPriority /= 100;
			}
			if (AI_isEmphasizeYield(YIELD_COMMERCE))
			{
				iCommercePriority *= 180;
				iCommercePriority /= 100;
			}
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	FAssertMsg(pPlot->getOwnerINLINE() == getOwnerINLINE(), "pPlot must be owned by this city's owner");

	BonusTypes eBonus = pPlot->getBonusType(getTeam());
	BonusTypes eNonObsoleteBonus = pPlot->getNonObsoleteBonusType(getTeam());

	bool bHasBonusImprovement = false;

	if (eNonObsoleteBonus != NO_BONUS)
	{
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
/*************************************************************************************************/
/**	Forts Connect Resources									02/12/10				Xienwolf	**/
/*************************************************************************************************/
/* original code
			if (GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus))
*/
			if (GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus) || GC.getImprovementInfo(pPlot->getImprovementType()).isUniversalTradeBonusProvider())
/*************************************************************************************************/
/**	Forts Connect Resources					END													**/
/*************************************************************************************************/
			{
				bHasBonusImprovement = true;
			}
/********************************************************************************/
/* 	Bonus Improvement: Wait for Upgrade							Fuyu		    */
/********************************************************************************/
			//Fuyu: patience. We can wait 10 turns for upgrade
			else if (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo((ImprovementTypes)(GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade())).isImprovementBonusTrade(eNonObsoleteBonus)
					|| GC.getImprovementInfo((ImprovementTypes)(GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade())).isUniversalTradeBonusProvider())
				{
					if (pPlot->getUpgradeTimeLeft(pPlot->getImprovementType(), getOwner()) <= 1 + ((9 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getImprovementPercent() * GC.getEraInfo(GC.getGameINLINE().getStartEra()).getImprovementPercent())/10000))
					{
						bHasBonusImprovement = true;
					}
				}
			}
/********************************************************************************/
/* 	Bonus Improvement: Wait for Upgrade							END			    */
/********************************************************************************/
		}
	}

	int aiBestDiffYields[NUM_YIELD_TYPES];
	for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
	{
		aiBestDiffYields[iJ] = 0;
	}

	BuildTypes eForcedBuild = NO_BUILD;
	
	//If a worker is already building a build, force that Build.
	for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
	{
		if (unitItr->getBuildType() != NO_BUILD)
		{
			if (GC.getBuildInfo(unitItr->getBuildType()).getImprovement() != NO_IMPROVEMENT)
			{
				eForcedBuild = unitItr->getBuildType();
				break;
			}
		}
	}

	int iBestValue = 0;
	BuildTypes eBestBuild = NO_BUILD;
	
	int iClearFeatureValue = 0;
	
	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		iClearFeatureValue = AI_clearFeatureValue(getCityPlotIndex(pPlot));
	}

	bool bEmphasizeIrrigation = false;
	if (!bHasBonusImprovement)
	{
		bEmphasizeIrrigation = false;

		//It looks unwieldly but the code has to be rigid to avoid "worker ADD"
		//where they keep connecting then disconnecting a crops resource or building
		//multiple farms to connect a single crop resource.
		//isFreshWater is used to ensure invalid plots are pruned early, the inner loop
		//really doesn't run that often.

		//using logic along the lines of "Will irrigating me make crops wet"
		//wont really work... it does have to "am i the tile the crops want to be irrigated"

		//I optimized through the use of "isIrrigated" which is just checking a bool...
		//once everything is nicely irrigated, this code should be really fast...
		if ((pPlot->isIrrigated()) || (pPlot->isFreshWater() && pPlot->canHavePotentialIrrigation()))
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

				if ((pAdjacentPlot != NULL) && (pAdjacentPlot->getOwner() == getOwner()) && (pAdjacentPlot->isCityRadius()))
				{
					if (!pAdjacentPlot->isFreshWater())
					{
						//check for a city? cities can conduct irrigation and that effect is quite
						//useful... so I think irrigate cities.
						if (pAdjacentPlot->isPotentialIrrigation())
						{
							CvPlot* eBestIrrigationPlot = NULL;


							for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
							{
								CvPlot* pAdjacentPlot2 = plotDirection(pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), ((DirectionTypes)iJ));
								if ((pAdjacentPlot2 != NULL) && (pAdjacentPlot2->getOwner() == getOwner()))
								{
									BonusTypes eTempBonus = pAdjacentPlot2->getNonObsoleteBonusType(getTeam());
									if (pAdjacentPlot->isIrrigated())
									{
										//the irrigation has to be coming from somewhere
										if (pAdjacentPlot2->isIrrigated())
										{
											//if we find a tile which is already carrying irrigation
											//then lets not replace that one...
											eBestIrrigationPlot = pAdjacentPlot2;

											if ((pAdjacentPlot2->isCity()) || (eTempBonus != NO_BONUS) || (!pAdjacentPlot2->isCityRadius()))
											{
												if (pAdjacentPlot2->isFreshWater())
												{
													//these are all ideal for irrigation chains so stop looking.
													break;
												}
											}
										}

									}
									else
									{
										if (pAdjacentPlot2->getNonObsoleteBonusType(getTeam()) == NO_BONUS)
										{
											if (pAdjacentPlot2->canHavePotentialIrrigation() && pAdjacentPlot2->isIrrigationAvailable())
											{
												//could use more sophisticated logic
												//however this would rely on things like smart irrigation chaining
												//of out-of-city plots
												eBestIrrigationPlot = pAdjacentPlot2;
												break;
											}
										}
									}
								}
							}

							if (pPlot == eBestIrrigationPlot)
							{
								bEmphasizeIrrigation = true;
								break;
							}
						}
					}
				}
			}
		}
	}
	
	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		ImprovementTypes eImprovement = ((ImprovementTypes)iI);

		int iBestTempBuildValue = 0;
		BuildTypes eBestTempBuild = NO_BUILD;

		bool bIgnoreFeature = false;
		bool bValid = false;

		if (eImprovement == pPlot->getImprovementType())
		{
			bValid = true;
		}
		//	Don't count forts - they have their own separate decision criteria
		else if (!GC.getImprovementInfo(eImprovement).isActsAsCity())
		{
			if (eForcedBuild != NO_BUILD)
			{
				if (GC.getBuildInfo(eForcedBuild).getImprovement() == eImprovement)
				{
					eBestTempBuild = eForcedBuild;
				}
			}
			else
			{
				for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
				{
					BuildTypes eBuild = ((BuildTypes)iJ);

					if (GC.getBuildInfo(eBuild).getImprovement() == eImprovement)
					{
						if (GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild, false, false, false))
						{
							int iValue = 10000;
							iValue /= (GC.getBuildInfo(eBuild).getTime() + 1);

							// XXX feature production???

							if (iValue > iBestTempBuildValue)
							{
								iBestTempBuildValue = iValue;
								eBestTempBuild = eBuild;
							}
						}
					}
				}
			}

			if (eBestTempBuild != NO_BUILD)
			{
				bValid = true;

				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					if (GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
					{
						bIgnoreFeature = true;

						if (GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) > 0)
						{
							if (eNonObsoleteBonus == NO_BONUS)
							{
								if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_LEAVE_FORESTS))
								{
									bValid = false;
								}
								else if (healthRate() < 0 && GC.getFeatureInfo(pPlot->getFeatureType()).getHealthPercent() > 0)
								{
									bValid = false;
								}
								else if (GET_PLAYER(getOwnerINLINE()).getFeatureHappiness(pPlot->getFeatureType()) > 0)
								{
									bValid = false;
								}
							}
						}
					}
				}
			}
		}

		if (bValid)
		{
/************************************************************************************************/
/* Afforess	                  Start		 05/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
			eFinalImprovement = finalImprovementUpgrade(eImprovement);
*/
			ImprovementTypes eFinalImprovement = GET_TEAM(getTeam()).finalImprovementUpgrade(eImprovement);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
			if (eFinalImprovement == NO_IMPROVEMENT)
			{
				eFinalImprovement = eImprovement;
			}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD																			*/
/*																								*/
/* City AI, FinalImprovement part moved here from inside yield loop (Fuyu)						*/
/************************************************************************************************/
			ImprovementTypes eCurImprovement = pPlot->getImprovementType();
			ImprovementTypes eCurFinalImprovement = NO_IMPROVEMENT;
			if( eCurImprovement != NO_IMPROVEMENT )
			{
/************************************************************************************************/
/* Afforess	                  Start		 05/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
				eCurFinalImprovement = finalImprovementUpgrade(eCurImprovement);
*/
				eCurFinalImprovement = GET_TEAM(getTeam()).finalImprovementUpgrade(eCurImprovement);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

				if (eCurFinalImprovement == NO_IMPROVEMENT)
				{
					eCurFinalImprovement = eCurImprovement;
				}
			}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

			int iValue = 0;
				
			if (eBonus != NO_BONUS)
			{
/*************************************************************************************************/
/**	Forts Connect Resources but should never be built					30.07.2010		Fuyu	**/
/*************************************************************************************************/
				if (eNonObsoleteBonus != NO_BONUS)
				{
					if (!bHasBonusImprovement)
					{
						if (GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
						{
							if (!(GC.getImprovementInfo(eFinalImprovement).isImprovementBonusTrade(eNonObsoleteBonus)))
							{
								//reduced value for temporary solutions
								if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo(eNonObsoleteBonus).getTechCityTrade()))))
								{
									iValue++;
								}
								else
								{
									iValue += (GET_PLAYER(getOwnerINLINE()).AI_bonusVal(eNonObsoleteBonus) * 10);
									iValue += 150;
								}
							}
							else
							{
								//full value for permanent solutions
								iValue += (GET_PLAYER(getOwnerINLINE()).AI_bonusVal(eNonObsoleteBonus) * 10);
								iValue += 200;
							}

							if (eBestBuild != NO_BUILD)
							{
								if ((GC.getBuildInfo(eBestBuild).getImprovement() == NO_IMPROVEMENT) || !(GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBestBuild).getImprovement()).isImprovementBonusTrade(eNonObsoleteBonus)))
								{
									//Always prefer improvements which connect bonuses.
									eBestBuild = NO_BUILD;
									iBestValue = 0;
								}
							}
						}
						else
						{
							if (eBestBuild != NO_BUILD)
							{
								if ((GC.getBuildInfo(eBestBuild).getImprovement() != NO_IMPROVEMENT) && (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBestBuild).getImprovement()).isImprovementBonusTrade(eNonObsoleteBonus)))
								{
									iValue -= 1000;
								}
							}
							if (GC.getImprovementInfo(eFinalImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
							{
								//very small incentive to build improvements that could later connect the bonus resource
								iValue++;
							}
						}
					}
					else if (!(GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus)))
					{
						iValue -= 1000;
					}
/*************************************************************************************************/
/**	Forts Connect Resources but should never be built					END						**/
/*************************************************************************************************/
				}
			}
			else
			{
				for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
				{
					if (GC.getImprovementInfo(eFinalImprovement).getImprovementBonusDiscoverRand(iJ) > 0)
					{
						iValue++;
					}
				}
			}
				
			if (iValue >= 0)
			{
				int aiFinalYields[NUM_YIELD_TYPES];
				int aiDiffYields[NUM_YIELD_TYPES];

				iValue *= 2;
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/06/09                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
/* original BTS code
					aiFinalYields[iJ] = 2*(pPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false));
					if (bIgnoreFeature && pPlot->getFeatureType() != NO_FEATURE)
					{
						aiFinalYields[iJ] -= 2 * GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange((YieldTypes)iJ);							
					}
					aiDiffYields[iJ] = (aiFinalYields[iJ] - (2 * pPlot->getYield(((YieldTypes)iJ))));
*/
					// 
					aiFinalYields[iJ] = 2*(pPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));
					aiFinalYields[iJ] += (pPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));

					//	Koshling - removed the following original code because it is double-counting since the
					//	call to calculateNatureYield() above ALREADY factors out a removed improvement
					//if (bIgnoreFeature && pPlot->getFeatureType() != NO_FEATURE)
					//{
					//	aiFinalYields[iJ] -= 2 * GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange((YieldTypes)iJ);							
					//}

					int iCurYield = 2*(pPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), false));

/********************************************************************************/
/*	Better Evaluation							19.03.2010		Fuyu		    */
/********************************************************************************/
/* Fuyu: moved FinalImprovement code up to before the loop
					ImprovementTypes eCurImprovement = pPlot->getImprovementType();
*/
					if( eCurImprovement != NO_IMPROVEMENT )
					{
/* moved
						ImprovementTypes eCurFinalImprovement = finalImprovementUpgrade(eCurImprovement);
						if (eCurFinalImprovement == NO_IMPROVEMENT)
						{
							eCurFinalImprovement = eCurImprovement;
						}
*/

						iCurYield += (pPlot->calculateImprovementYieldChange(eCurFinalImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));
						iCurYield += (pPlot->calculateImprovementYieldChange(eCurImprovement, ((YieldTypes)iJ), getOwnerINLINE(), false, true));
					}
/********************************************************************************/
/*	BE	END																		*/
/********************************************************************************/

					aiDiffYields[iJ] = (aiFinalYields[iJ] - iCurYield);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
				}
/************************************************************************************************/
/* Afforess	                  Start		 08/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
				int iHealth = AI_calculateActualImprovementHealth(eFinalImprovement) - AI_calculateActualImprovementHealth(eCurFinalImprovement);
				int iBaseHealthLevel = goodHealth() - badHealth() + getEspionageHealthCounter();
				//int iBaseFoodDifference = getYieldRate(YIELD_FOOD) - getPopulation()*GC.getFOOD_CONSUMPTION_PER_POPULATION() - std::max(0,-iBaseHealthLevel);
				int iBadHealthFromImprovement = std::max(0,(-iHealth));
				int iUnhealthyPopulationFromImprovement = std::min(0,(-iBaseHealthLevel)) + iBadHealthFromImprovement;
				//int iGoodHealthFromImprovement = std::max(0, iHealth);
				int iHealthyPopulationFromImprovement = 0;
				if (iBaseHealthLevel < 0 && iHealth > 0)
				{	
					iHealthyPopulationFromImprovement = std::min(iHealth, -iBaseHealthLevel);
				}
				aiDiffYields[YIELD_FOOD] -= iUnhealthyPopulationFromImprovement;
				aiDiffYields[YIELD_FOOD] += iHealthyPopulationFromImprovement;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

				iValue += (aiDiffYields[YIELD_FOOD] * ((100 * iFoodPriority) / 100));
				iValue += (aiDiffYields[YIELD_PRODUCTION] * ((60 * iProductionPriority) / 100));
				iValue += (aiDiffYields[YIELD_COMMERCE] * ((40 * iCommercePriority) / 100));

				iValue /= 2;

				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					aiFinalYields[iJ] /= 2;
					aiDiffYields[iJ] /= 2;
				}
					
				if (iValue > 0)
				{
					// this is mainly to make it improve better tiles first
					//flood plain > grassland > plain > tundra
					iValue += (aiFinalYields[YIELD_FOOD] * 10);
					iValue += (aiFinalYields[YIELD_PRODUCTION] * 6);
					iValue += (aiFinalYields[YIELD_COMMERCE] * 4);

					if (aiFinalYields[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
					{
						//this is a food yielding tile
						if (iFoodPriority > 100)
						{
							iValue *= 100 + iFoodPriority;
							iValue /= 200;							
						}
						if (iFoodChange > 0)
						{
							iValue += (10 * (1 + aiDiffYields[YIELD_FOOD]) * (1 + aiFinalYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION()) * iFoodChange * iFoodPriority) / 100;
						}
						if (iCommercePriority > 100)
						{
							iValue *= 100 + (((iCommercePriority - 100) * aiDiffYields[YIELD_COMMERCE]) / 2);
							iValue /= 100;
						}
					}
					else if (aiFinalYields[YIELD_FOOD] < GC.getFOOD_CONSUMPTION_PER_POPULATION())
					{
						if ((aiDiffYields[YIELD_PRODUCTION] > 0) && (aiFinalYields[YIELD_FOOD]+aiFinalYields[YIELD_PRODUCTION] > 3))
						{
							if (iFoodPriority < 100 || GET_PLAYER(getOwnerINLINE()).getCurrentEra() < 2)
							{
								//value booster for mines on hills
								iValue *= (100 + 25 * aiDiffYields[YIELD_PRODUCTION]);
								iValue /= 100;
							}
						}
						if (iFoodChange < 0)
						{
							iValue *= 4 - iFoodChange;
							iValue /= 3 + aiFinalYields[YIELD_FOOD];
						}
					}

					if ((iFoodPriority < iProductionPriority) && (iProductionPriority > 100))
					{
						iValue *= (200 + ((iProductionPriority - 100)*aiFinalYields[YIELD_PRODUCTION]));
						iValue /= 200;
					}
					if (eBonus == NO_BONUS)
					{
						if (iFoodChange > 0)
						{
							//We want more food.
							iValue *= 2 + std::max(0, aiDiffYields[YIELD_FOOD]);
							iValue /= 2 * (1 + std::max(0, -aiDiffYields[YIELD_FOOD]));
						}
//							else if (iFoodChange < 0)
//							{
//								//We want to soak up food.
//								iValue *= 8;
//								iValue /= 8 + std::max(0, aiDiffYields[YIELD_FOOD]);
//							}
					}
				}
					

				if (bEmphasizeIrrigation && GC.getImprovementInfo(eFinalImprovement).isCarriesIrrigation())
				{
					iValue += 500;
				}

				if (getImprovementFreeSpecialists(eFinalImprovement) > 0)
				{
					iValue += 2000;
				}
/********************************************************************************/
/*	Better Evaluation							19.03.2010		Fuyu		    */
/********************************************************************************/
				//int iNewHappiness = GC.getImprovementInfo(eFinalImprovement).getHappiness();
				//int iCurHappiness = GC.getImprovementInfo(eCurFinalImprovement).getHappiness();
				//int iHappinessDiff = iNewHappiness - iCurHappiness;

				int iHappinessDiff = GC.getImprovementInfo(eFinalImprovement).getHappiness() - ((eCurFinalImprovement != NO_IMPROVEMENT)? GC.getImprovementInfo(eCurFinalImprovement).getHappiness() : 0);
				if ((iHappinessDiff != 0) && !(GET_PLAYER(getOwnerINLINE()).getAdvancedStartPoints() >= 0))
				{
					bool bIsNegative = false;
					if (iHappinessDiff < 0)
					{
						bIsNegative = true;
						iHappinessDiff = -iHappinessDiff;
					}

					int iHappyLevel = iHappyAdjust + (happyLevel() - unhappyLevel(0)) + getEspionageHappinessCounter();
					int iHealthLevel = std::max(0, iHealthAdjust) + (goodHealth() - badHealth(false, 0)) + getEspionageHealthCounter();
					
					for (int iJ = 0; iJ < iHappinessDiff; iJ++)
					{
						int iHappyValue = 0;

						if (bIsNegative)
						{
							iHappyLevel--;
						}

						if (iHappyLevel <= 0)
						{
							iHappyValue += 400;
						}
						bool bCanGrow = true;// (getYieldRate(YIELD_FOOD) > foodConsumption());
						
						if (iHappyLevel <= iHealthLevel)
						{
							iHappyValue += 200 * std::max(0, (bCanGrow ? std::min(6, 2 + iHealthLevel - iHappyLevel) : 0) - iHappyLevel);
						}
						else
						{
							iHappyValue += 50 * std::max(0, (bCanGrow ? 4 : 0) - iHappyLevel);

						}
						if (!pPlot->isBeingWorked() || bIsNegative)
						{
							iHappyValue *= 4;
							iHappyValue /= 3;
						}

						if (iHappyValue == 0 && bIsNegative && iHappyLevel < 8)
						{
							//an unhappy improvement always has negative value, even if the city is really happy right now
							iHappyValue = 10;
						}

						//iHappyValue += std::max(0, (pPlot->getCityRadiusCount() - 1)) * ((iHappyValue > 0) ? iHappyLevel / 2 : 200);
						iHappyValue += (pPlot->getPlayerCityRadiusCount(getOwnerINLINE()) - 1) * ((iHappyValue > 0) ? 200 : 50);

						if (!bIsNegative)
						{
							iHappyLevel++;
							iValue += iHappyValue;
						}
						else
						{
							iValue -= iHappyValue;
						}
					}
				}
/********************************************************************************/
/*	BE	END																		*/
/********************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 05/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
				int iHealthPercent = GC.getImprovementInfo(eFinalImprovement).getHealthPercent();
				int iBadHealth = abs(getImprovementBadHealth());
				int iGoodHealth = getImprovementGoodHealth();
				//Bad Health AND will push over the edge and give more unhealthiness
				if ((iHealthPercent < 0 && (iBadHealth - iHealthPercent % 100 < iBadHealth % 100)) ||
				//OR Good Health AND will give us more healthiness
				(iHealthPercent > 0 && (iGoodHealth + iHealthPercent % 100 > iGoodHealth % 100)))
				{
					int iHealthChange = iHealthPercent > 0 ? std::max(1, iHealthPercent % 100) : std::min(-1, iHealthPercent % 100);
					int iHealthLevel = iHealthAdjust + (goodHealth() - badHealth(false, 0));
					if (eImprovement == pPlot->getImprovementType())
					{
						iHealthLevel -= iHealthChange;
					}
						
					int iHealthValue = 0;
					bool bCanGrow = (getYieldRate(YIELD_FOOD) > foodConsumption());
					if (iHealthLevel < 0)
					{
						iHealthValue -= 10;
					}
					if (!bCanGrow && iHealthLevel < 0)
					{
						iHealthValue -= 40;
					}
					if (iHealthLevel > 0)
					{
						iHealthValue += 10;
					}
						
					if (!pPlot->isBeingWorked())
					{
						iHealthValue *= 4;
						iHealthValue /= 3;
					}
					iValue += iHealthValue * 3;
				}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/						
				if (!isHuman())
				{
					iValue *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getImprovementWeightModifier(eFinalImprovement) + 200));
					iValue /= 200;
				}

				if (pPlot->getImprovementType() == NO_IMPROVEMENT)
				{
					if (pPlot->isBeingWorked())
					{
						iValue *= 5;
						iValue /= 4;
					}

					if (eBestTempBuild != NO_BUILD)
					{
						if (pPlot->getFeatureType() != NO_FEATURE)
						{
							if (GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
							{
								// BBAI TODO: extra boost in early game to chop rush
								CvCity* pCity = NULL;
								iValue += pPlot->getFeatureProduction(eBestTempBuild, getTeam(), &pCity) * 2;
								FAssert(pCity == this);

								iValue += iClearFeatureValue;
							}
						}
					}
				}
				else
				{
					// cottage/villages (don't want to chop them up if turns have been invested)
					ImprovementTypes eImprovementDowngrade = (ImprovementTypes)GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage();
					while (eImprovementDowngrade != NO_IMPROVEMENT)
					{
						CvImprovementInfo& kImprovementDowngrade = GC.getImprovementInfo(eImprovementDowngrade);
/********************************************************************************/
/*	Better Evaluation							19.03.2010		Fuyu		    */
/********************************************************************************/
//An attempt to make replacing hamlet+ less interesting
/* original code
						iValue -= kImprovementDowngrade.getUpgradeTime() * 8;
*/
						iValue -= kImprovementDowngrade.getUpgradeTime() * ((pPlot->isBeingWorked())? 20 : 10);
/********************************************************************************/
/*	BE	END																		*/
/********************************************************************************/
						eImprovementDowngrade = (ImprovementTypes)kImprovementDowngrade.getImprovementPillage();
					}
/************************************************************************************************/
/* Afforess	                  Start		 05/23/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
					if (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT)
*/
					if (GET_TEAM(getTeam()).getImprovementUpgrade(pPlot->getImprovementType()) != NO_IMPROVEMENT)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
					{
						iValue -= (GC.getImprovementInfo(pPlot->getImprovementType()).getUpgradeTime() * 8 * (pPlot->getUpgradeProgressHundredths())) / std::max(1, 100*GC.getGameINLINE().getImprovementUpgradeTime(pPlot->getImprovementType()));
					}

					if (eNonObsoleteBonus == NO_BONUS)
					{
						if (isWorkingPlot(pPlot))
						{
							if (((iFoodPriority < 100) && (aiFinalYields[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())) || (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage() != NO_IMPROVEMENT))
							{
								iValue -= 70;
								iValue *= 2;
								iValue /= 3;
							}
						}
					}
						
					if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION))
					{
						iValue /= 4;	//Greatly prefer builds which are legal.
					}
/************************************************************************************************/
/* Afforess	                  Start		 07/27/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
					if (eFinalImprovement != NO_IMPROVEMENT && (GC.getImprovementInfo(eFinalImprovement).isActsAsCity()))
					{
						iValue /= 2;
					}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestBuild = eBestTempBuild;
						
					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						aiBestDiffYields[iJ] = aiDiffYields[iJ];
					}
				}
			}
		}
	}

	if (iClearFeatureValue > 0)
	{
		FAssert(pPlot->getFeatureType() != NO_FEATURE);
		
		{
			if ((GC.getFeatureInfo(pPlot->getFeatureType()).getHealthPercent() < 0) ||
				((GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_FOOD) + GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) + GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_COMMERCE)) < 0))
			{
				for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
				{
					BuildTypes eBuild = ((BuildTypes)iI);

					if (GC.getBuildInfo(eBuild).getImprovement() == NO_IMPROVEMENT)
					{
						if (GC.getBuildInfo(eBuild).isFeatureRemove(pPlot->getFeatureType()))
						{
							if (GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild))
							{
								int iValue = iClearFeatureValue;
								CvCity* pCity = NULL;
								iValue += (pPlot->getFeatureProduction(eBuild, getTeam(), &pCity) * 10);
								
								iValue *= 400;
								iValue /= std::max(1, (GC.getBuildInfo(eBuild).getFeatureTime(pPlot->getFeatureType()) + 100));
								
								if ((iValue > iBestValue) || ((iValue > 0) && (eBestBuild == NO_BUILD)))
								{
									iBestValue = iValue;
									eBestBuild = eBuild;
								}
							}
						}
					}
				}
			}
		}
	}

	//Chop - maybe integrate this better with the other feature-clear code tho the logic
	//is kinda different
	if (bChop && (eBonus == NO_BONUS) && (pPlot->getFeatureType() != NO_FEATURE) &&
		(pPlot->getImprovementType() == NO_IMPROVEMENT) && !(GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_LEAVE_FORESTS)))
	{
		for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			BuildTypes eBuild = ((BuildTypes)iI);
			if (GC.getBuildInfo(eBuild).getImprovement() == NO_IMPROVEMENT)
			{
				if (GC.getBuildInfo(eBuild).isFeatureRemove(pPlot->getFeatureType()))
				{
					if (GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild))
					{
						CvCity* pCity = NULL;
						int iValue = (pPlot->getFeatureProduction(eBuild, getTeam(), &pCity)) * 10;
						FAssert(pCity == this);

						if (iValue > 0)
						{
							iValue += iClearFeatureValue;

							if (iValue > 0)
							{
								if (GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_DAGGER))
								{
									iValue += 20;
									iValue *= 2;
								}
								iValue *= 500;
								iValue /= std::max(1, (GC.getBuildInfo(eBuild).getFeatureTime(pPlot->getFeatureType()) + 100));

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									eBestBuild = eBuild;
								}
							}
						}
					}
				}
			}
		}
	}


	for (int iI = 0; iI < GC.getNumRouteInfos(); iI++)
	{
		RouteTypes eRoute = (RouteTypes)iI;
		RouteTypes eOldRoute = pPlot->getRouteType();

		if (eRoute != eOldRoute)
		{
			int iTempValue = 0;
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if ((eOldRoute == NO_ROUTE) || (GC.getRouteInfo(eRoute).getValue() > GC.getRouteInfo(eOldRoute).getValue()))
				{
					iTempValue += ((GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, YIELD_FOOD)) * 100);
					iTempValue += ((GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, YIELD_PRODUCTION)) * 60);
					iTempValue += ((GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, YIELD_COMMERCE)) * 40);
				}
				
				if (pPlot->isBeingWorked())
				{
					iTempValue *= 2;
				}
				//road up bonuses if sort of bored.
				if ((eOldRoute == NO_ROUTE) && (eBonus != NO_BONUS))
				{
					iTempValue += (pPlot->isConnectedToCapital() ? 10 : 30);
				}
			}
								
			if (iTempValue > 0)
			{
				for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
				{
					BuildTypes eBuild = ((BuildTypes)iJ);
					if (GC.getBuildInfo(eBuild).getRoute() == eRoute)
					{
						if (GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild, false))
						{
							//the value multiplier is based on the default time...
							int iValue = iTempValue * 5 * 300;
							iValue /= GC.getBuildInfo(eBuild).getTime();

							if ((iValue > iBestValue) || ((iValue > 0) && (eBestBuild == NO_BUILD)))
							{
								iBestValue = iValue;
								eBestBuild = eBuild;
							}
						}
					}
				}
			}
		}
	}



	if (eBestBuild != NO_BUILD)
	{
		FAssertMsg(iBestValue > 0, "iBestValue is expected to be greater than 0");
		
		//Now modify the priority for this build.
		if (GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
		{
			if (GC.getBuildInfo(eBestBuild).getImprovement() != NO_IMPROVEMENT)
			{
				iBestValue += (iBestValue * std::max(0, aiBestDiffYields[YIELD_COMMERCE])) / 4;
				iBestValue = std::max(1, iBestValue);
			}
			
		}

		if (piBestValue != NULL)
		{
			*piBestValue = iBestValue;
		}
		if (peBestBuild != NULL)
		{
			if ( *peBestBuild != NO_BUILD )
			{
				if( gCityLogLevel >= 2 )
				{
					logBBAI("    City %S, tile (%d,%d) best build changed from %S to %S", getName().GetCString(), pPlot->getX_INLINE(), pPlot->getY_INLINE(), GC.getBuildInfo(*peBestBuild).getDescription(), GC.getBuildInfo(eBestBuild).getDescription());
				}
			}

			*peBestBuild = eBestBuild;
		}
	}
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry)
{
	return AI_getHappyFromHurry(hurryPopulation(eHurry));
}

int CvCityAI::AI_getHappyFromHurry(int iHurryPopulation)
{
	int iHappyDiff = iHurryPopulation - GC.getDefineINT("HURRY_POP_ANGER");
	if (iHappyDiff > 0)
	{
		if (getHurryAngerTimer() <= 1)
		{
			if (2 * angryPopulation(1) - healthRate(false, 1) > 1)
			{
				return iHappyDiff;
			}
		}
	}

	return 0;
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew)
{
	return AI_getHappyFromHurry(getHurryPopulation(eHurry, getHurryCost(true, eUnit, bIgnoreNew)));
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew)
{
	return AI_getHappyFromHurry(getHurryPopulation(eHurry, getHurryCost(true, eBuilding, bIgnoreNew)));
}


int CvCityAI::AI_cityValue() const
{
	
	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
	if ((eAreaAI == AREAAI_OFFENSIVE) || (eAreaAI == AREAAI_MASSING) || (eAreaAI == AREAAI_DEFENSIVE))
	{
		return 0;
	}
	
	int iValue = 0;
	
	iValue += getCommerceRateTimes100(COMMERCE_GOLD);
	iValue += getCommerceRateTimes100(COMMERCE_RESEARCH);
	iValue += 100 * getYieldRate(YIELD_PRODUCTION);

	iValue -= 3 * calculateColonyMaintenanceTimes100();

	return iValue;
}

bool CvCityAI::AI_doPanic()
{
	
	bool bLandWar = ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
	
	if (bLandWar)
	{
		int iOurDefense = GET_PLAYER(getOwnerINLINE()).AI_getOurPlotStrength(plot(), 0, true, false);
		int iEnemyOffense = GET_PLAYER(getOwnerINLINE()).AI_getEnemyPlotStrength(plot(), 2, false, false);
		//TBRUSHFIX
		//iEnemyOffense was not keeping units that could not attack the city out of account.
		int iRatio = (100 * iEnemyOffense) / (std::max(1, iOurDefense));
		
		//TBRUSHFIX
		//Playtesting has shown this needs to be a bit less sensitive.
		if (iRatio > 200/*100*/)
		{
			UnitTypes eProductionUnit = getProductionUnit();

			if (eProductionUnit != NO_UNIT)
			{
				if (getProduction() > 0)
				{
/************************************************************************************************/
/* Afforess	                  Start		 6/11/11                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
					if ((GC.getUnitInfo(eProductionUnit).getCombat() + 
					GET_TEAM(getTeam()).getUnitClassStrengthChange((UnitClassTypes)GC.getUnitInfo(eProductionUnit).getUnitClassType())) > 0)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
					{
						AI_doHurry(true);
						return true;
					}
				}
			}
			else
			{
				if ((GC.getGameINLINE().getSorenRandNum(2, "AI choose panic unit") == 0) && AI_chooseUnitImmediate("panic defense", UNITAI_CITY_COUNTER))
				{
					AI_doHurry((iRatio > 250/*140*/));	
				}
				else if (AI_chooseUnitImmediate("panic defense", UNITAI_CITY_DEFENSE))
				{
					AI_doHurry((iRatio > 250/*140*/));
				}
				else if (AI_chooseUnitImmediate("panic defense", UNITAI_ATTACK))
				{
					AI_doHurry((iRatio > 250/*140*/));
				}
			}
		}
	}
	return false;
}

int CvCityAI::AI_calculateCulturePressure(bool bGreatWork)
{
	int iValue = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getOwnerINLINE() == NO_PLAYER)
			{
				iValue++;
			}
			else
			{
				int iTempValue = pLoopPlot->calculateCulturePercent(getOwnerINLINE());
				if (iTempValue == 100)
				{
					//do nothing
				}
				else if ((iTempValue == 0) || (iTempValue > 75))
				{
					iValue++;
				}
				else
				{
					iTempValue = (100 - iTempValue);
					FAssert(iTempValue > 0);
					FAssert(iTempValue <= 100);

					if (iI != CITY_HOME_PLOT)
					{
						iTempValue *= 4;
						iTempValue /= NUM_CITY_PLOTS;
					}

					BonusTypes  eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

					if (eNonObsoleteBonus != NO_BONUS)
					{
						iTempValue += (GET_PLAYER(getOwnerINLINE()).AI_bonusVal(eNonObsoleteBonus) * ((GET_PLAYER(getOwnerINLINE()).getNumTradeableBonuses(eNonObsoleteBonus) == 0) ? 4 : 2));
					}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/20/10                          denev & jdog5000    */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* original bts code
					if ((iTempValue > 80) && (pLoopPlot->getOwnerINLINE() == getID()))
*/
					if ((iTempValue > 80) && (pLoopPlot->getOwnerINLINE() == getOwnerINLINE()))
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
					{
						//captured territory special case
						iTempValue *= (100 - iTempValue);
						iTempValue /= 100;
					}

					if (pLoopPlot->getTeam() == getTeam())
					{
						iTempValue /= (bGreatWork ? 10 : 2);
					}
					else
					{
						iTempValue *= 2;
						if (bGreatWork)
						{
							if (GET_PLAYER(getOwnerINLINE()).AI_getAttitude(pLoopPlot->getOwnerINLINE()) == ATTITUDE_FRIENDLY)
							{
								iValue /= 10;
							}
						}
					}

					iValue += iTempValue;
				}
			}
		}
	}


	return iValue;
}


void CvCityAI::AI_buildGovernorChooseProduction()
{
	PROFILE_FUNC();
	bool bDanger = AI_isDanger();


	// only clear the dirty bit if we actually do a check, multiple items might be queued
	AI_setChooseProductionDirty(false);

	CvArea* pWaterArea = waterArea();

	bool bWasFoodProduction = isFoodProduction();
	int iCulturePressure = AI_calculateCulturePressure();
	int iMinValueDivisor = 1;
	if (getPopulation() < 3)
	{
		iMinValueDivisor = 3;
	}
	else if (getPopulation() < 7)
	{
		iMinValueDivisor = 2;
	}
	
	
	clearOrderQueue();

	if (bWasFoodProduction)
	{
		AI_assignWorkingPlots();
	}
	
//TB Build Mod (Bad Strategy)
#ifndef C2C_BUILD
	// if we need to pop borders, then do that immediately if we have drama and can do it
	// if we need to pop borders, then do that immediately if we have drama and can do it
	if ((getCultureLevel() <= (CultureLevelTypes)1) && ((getCommerceRate(COMMERCE_CULTURE) < 2) || (iCulturePressure > 0)))
	{
		if (AI_chooseProcess(COMMERCE_CULTURE))
		{
			return;
		}
	}
#endif
	
// BUG - Governor Builds Workboats - start
#ifdef _MOD_GOVWORKERS
	if (!isHuman() || GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_1))
	{
#endif
	//workboat
	if (pWaterArea != NULL)
	{
		if (GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_WORKER_SEA) == 0)
		{
			if (AI_neededSeaWorkers() > 0)
			{
				if (AI_chooseUnit("no sea workers for area", UNITAI_WORKER_SEA))
				{
					return;
				}
			}
		}
	}
#ifdef _MOD_GOVWORKERS
	}
#endif
// BUG - Governor Builds Workboats - end

	if ((AI_countNumBonuses(NO_BONUS, false, true, 10, true, true) > 0)
		&& (getPopulation() > AI_countNumBonuses(NO_BONUS, true, false, -1, true, true)))
	{
		if (getCommerceRate(COMMERCE_CULTURE) == 0)
		{
			AI_chooseBuilding(BUILDINGFOCUS_CULTURE);
			return;
		}
	}

	// pick granary or lighthouse, any duration
	if (AI_chooseBuilding(BUILDINGFOCUS_FOOD))
	{
		return;
	}
	
	if (angryPopulation(1) > 1)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_HAPPY, 40))
		{
			return;
		}
	}
	
//TB Build Mod (Move up in priority)    
#ifndef C2C_BUILD
	if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, 30, 10 / iMinValueDivisor))
	{
		return;
	}
#endif
	
	if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 8, 33))
	{
		return;
	}

	
	if (((getCommerceRateTimes100(COMMERCE_CULTURE) == 0) && (iCulturePressure != 0))
		|| (iCulturePressure > 100))
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
		{
			return;
		}
	}

	
	int iEconomyFlags = 0;
	iEconomyFlags |= BUILDINGFOCUS_GOLD;
	iEconomyFlags |= BUILDINGFOCUS_RESEARCH;
	iEconomyFlags |= BUILDINGFOCUS_MAINTENANCE;
	iEconomyFlags |= BUILDINGFOCUS_HAPPY;
	iEconomyFlags |= BUILDINGFOCUS_HEALTHY;
	iEconomyFlags |= BUILDINGFOCUS_SPECIALIST;
	if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		iEconomyFlags |= BUILDINGFOCUS_ESPIONAGE;
	}
	
	//20 means 5g or ~2 happiness...
	if (AI_chooseBuilding(iEconomyFlags, 20, 20 / iMinValueDivisor))
	{
		return;
	}
	
// BUG - Governor Builds Workers - start
#ifdef _MOD_GOVWORKERS
	if (!isHuman() || GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_1))
	{
#endif
	int iExistingWorkers = GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_WORKER);
	int iNeededWorkers = GET_PLAYER(getOwner()).AI_neededWorkers(area());
	
	if (!bDanger && (iExistingWorkers < ((iNeededWorkers + 1) / 2)))
	{
		if (AI_chooseUnit("no danger optional worker", UNITAI_WORKER))
		{
			return;
		}
	}	    
#ifdef _MOD_GOVWORKERS
	}
#endif
// BUG - Governor Builds Workers - end

	if (GC.getDefineINT("DEFAULT_SPECIALIST") != NO_SPECIALIST)
	{
		if (getSpecialistCount((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))) > 0)
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_SPECIALIST, 60))
			{
				return;
			}
		}
	}
	
	if (AI_chooseBuilding(iEconomyFlags, 40, 15 / iMinValueDivisor))
	{
		return;
	}
	
	if (AI_chooseBuilding(iEconomyFlags | BUILDINGFOCUS_CULTURE, 10, 10 / iMinValueDivisor))
	{
		return;
	}

	if (AI_chooseBuilding())
	{
		return;
	}


	if (AI_chooseProcess())
	{
		return;
	}
	
	if (AI_chooseUnit("crappy governor"))
	{
		return;
	}
}

int CvCityAI::AI_calculateWaterWorldPercent()
{
	int iTeamCityCount = 0;
	int iOtherCityCount = 0;
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI == getTeam() || GET_TEAM((TeamTypes)iI).isVassal(getTeam())
				|| GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
			{
				iTeamCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
			}
			else
			{
				iOtherCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
			}
		}
	}

	int iWaterPercent = 0;
	if (iOtherCityCount == 0)
	{
		iWaterPercent = 100;
	}
	else
	{
		iWaterPercent = 100 - ((iTeamCityCount + iOtherCityCount) * 100) / std::max(1, (GC.getGame().getNumCities()));
	}
	
	iWaterPercent *= 50;
	iWaterPercent /= 100;
	
	iWaterPercent += (50 * (2 + iTeamCityCount)) / (2 + iTeamCityCount + iOtherCityCount);
	
	iWaterPercent = std::max(1, iWaterPercent);
	
	
	return iWaterPercent;
}

//Please note, takes the yield multiplied by 100
int CvCityAI::AI_getYieldMagicValue(const int* piYieldsTimes100, bool bHealthy)
{
	FAssert(piYieldsTimes100 != NULL);

	int iPopEats = GC.getFOOD_CONSUMPTION_PER_POPULATION();
	iPopEats += (bHealthy ? 0 : 1);
	iPopEats *= 100;

	int iValue = ((piYieldsTimes100[YIELD_FOOD] * 100 + piYieldsTimes100[YIELD_PRODUCTION]*55 + piYieldsTimes100[YIELD_COMMERCE]*40) - iPopEats * 102);
	iValue /= 100;
	return iValue;
}

//The magic value is basically "Look at this plot, is it worth working"
//-50 or lower means the plot is worthless in a "workers kill yourself" kind of way.
//-50 to -1 means the plot isn't worth growing to work - might be okay with emphasize though.
//Between 0 and 50 means it is marginal.
//50-100 means it's okay.
//Above 100 means it's definitely decent - seriously question ever not working it.
//This function deliberately doesn't use emphasize settings.
int CvCityAI::AI_getPlotMagicValue(CvPlot* pPlot, bool bHealthy, bool bWorkerOptimization)
{
	FAssert(pPlot != NULL);

	int aiYields[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if ((bWorkerOptimization) && (pPlot->getWorkingCity() == this) && (AI_getBestBuild(getCityPlotIndex(pPlot)) != NO_BUILD))
		{
			aiYields[iI] = pPlot->getYieldWithBuild(AI_getBestBuild(getCityPlotIndex(pPlot)), (YieldTypes)iI, true);
		}
		else
		{
			aiYields[iI] = pPlot->getYield((YieldTypes)iI) * 100;
		}
	}

	ImprovementTypes eCurrentImprovement = pPlot->getImprovementType();

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		ImprovementTypes eFinalImprovement = finalImprovementUpgrade(eCurrentImprovement);

		if ((eFinalImprovement != NO_IMPROVEMENT) && (eFinalImprovement != eCurrentImprovement))
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				int iYieldDiff = 100 * pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iI), getOwnerINLINE());
				iYieldDiff -= 100 * pPlot->calculateImprovementYieldChange(eCurrentImprovement, ((YieldTypes)iI), getOwnerINLINE());
				aiYields[iI] += iYieldDiff / 2;
			}
		}
	}
	
	return AI_getYieldMagicValue(aiYields, bHealthy);
}

//useful for deciding whether or not to grow... or whether the city needs terrain
//improvement.
//if healthy is false it assumes bad health conditions.
int CvCityAI::AI_countGoodTiles(bool bHealthy, bool bUnworkedOnly, int iThreshold, bool bWorkerOptimization)
{
	int iCount = 0;
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
		CvPlot* pLoopPlot = plotCity(getX_INLINE(),getY_INLINE(), iI);
		if ((iI != CITY_HOME_PLOT) && (pLoopPlot != NULL))
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (!bUnworkedOnly || !(pLoopPlot->isBeingWorked()))
				{
					if (AI_getPlotMagicValue(pLoopPlot, bHealthy) > iThreshold)
					{
						iCount++;
					}                    
				}
			}
		}
	}
	return iCount;
}

int CvCityAI::AI_calculateTargetCulturePerTurn()
{
	/*
	int iTarget = 0;
	
	bool bAnyGoodPlotUnowned = false;
	bool bAnyGoodPlotHighPressure = false;	
	
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(),getY_INLINE(),iI);
		
		if (pLoopPlot != NULL)
		{
			if ((pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				|| (pLoopPlot->getYield(YIELD_FOOD) > GC.getFOOD_CONSUMPTION_PER_POPULATION()))
			{
				if (!pLoopPlot->isOwned())
				{
					bAnyGoodPlotUnowned = true;        			
				}
				else if (pLoopPlot->getOwnerINLINE() != getOwnerINLINE())
				{
					bAnyGoodPlotHighPressure = true;
				}
			}
		}
	}
	if (bAnyGoodPlotUnowned)
	{
		iTarget = 1;
	}
	if (bAnyGoodPlotHighPressure)
	{
		iTarget += getCommerceRate(COMMERCE_CULTURE) + 1;
	}
	return iTarget;
	*/
	return 1;
}
	
int CvCityAI::AI_countGoodSpecialists(bool bHealthy)
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		SpecialistTypes eSpecialist = (SpecialistTypes)iI;
		
		int iValue = 0;
		
		iValue += 100 * kPlayer.specialistYield(eSpecialist, YIELD_FOOD);
		iValue += 65 * kPlayer.specialistYield(eSpecialist, YIELD_PRODUCTION);
		iValue += 40 * kPlayer.specialistYield(eSpecialist, YIELD_COMMERCE);
		
		iValue += 40 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_RESEARCH);
		iValue += 40 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_GOLD);
		iValue += 20 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_ESPIONAGE);
		iValue += 15 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_CULTURE);
		iValue += 25 * GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();
/************************************************************************************************/
/* Specialists Enhancements, by Supercheese 10/12/09                                            */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/	
		iValue += GC.getSpecialistInfo(eSpecialist).getHealthPercent();
		iValue += GC.getSpecialistInfo(eSpecialist).getHappinessPercent();
/************************************************************************************************/
/* Specialists Enhancements                          END                                        */
/************************************************************************************************/
		iValue += GC.getSpecialistInfo(eSpecialist).getInvestigation();
		iValue -= GC.getSpecialistInfo(eSpecialist).getInsidiousness();
		
		if (iValue >= (bHealthy ? 200 : 300))
		{
			iCount += getMaxSpecialistCount(eSpecialist);
		}
	}
	iCount -= getFreeSpecialist();
	
	return iCount;
}
//0 is normal
//higher than zero means special.
int CvCityAI::AI_getCityImportance(bool bEconomy, bool bMilitary)
{
	int iValue = 0;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/08/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI                                                                          */
/************************************************************************************************/
	if (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	{
		int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
		int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();
		
		if (iCultureRateRank <= iCulturalVictoryNumCultureCities)
		{
			iValue += 100;
		
			if ((getCultureLevel() < (GC.getNumCultureLevelInfos() - 1)))
			{
				iValue += !bMilitary ? 100 : 0;
			}
			else
			{
				iValue += bMilitary ? 100 : 0;
			}
		}
	}
	
	return iValue;
}

void CvCityAI::AI_stealPlots()
{
	PROFILE_FUNC();
	
	int iImportance = AI_getCityImportance(true, false);

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		
		if (pLoopPlot != NULL)
		{
			if (iImportance > 0)
			{
				if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
				{
					CvCityAI* pWorkingCity = static_cast<CvCityAI*>(pLoopPlot->getWorkingCity());
					if ((pWorkingCity != this) && (pWorkingCity != NULL))
					{
						FAssert(pWorkingCity->getOwnerINLINE() == getOwnerINLINE());
						int iOtherImportance = pWorkingCity->AI_getCityImportance(true, false);
						if (iImportance > iOtherImportance)
						{
							pLoopPlot->setWorkingCityOverride(this);
						}
					}
				}
			}
				
			if (pLoopPlot->getWorkingCityOverride() == this)
			{
				if (pLoopPlot->getOwnerINLINE() != getOwnerINLINE())
				{
					pLoopPlot->setWorkingCityOverride(NULL);                    
				}
			}
		}
	}
}




// +1/+3/+5 plot based on base food yield (1/2/3)
// +4 if being worked.
// +4 if a bonus.
// Unworked ocean ranks very lowly. Unworked lake ranks at 3. Worked lake at 7.
// Worked bonus in ocean ranks at like 11
int CvCityAI::AI_buildingSpecialYieldChangeValue(BuildingTypes eBuilding, YieldTypes eYield)
{
	int iValue = 0;
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iWorkedCount = 0;
	
	int iYieldChange = kBuilding.getSeaPlotYieldChange(eYield);
	if (iYieldChange > 0)
	{
		int iWaterCount = 0;
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
			if (iI != CITY_HOME_PLOT)
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
				if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity() == this))
				{
					if (pLoopPlot->isWater())
					{
						iWaterCount++;
						int iFood = pLoopPlot->getYield(YIELD_FOOD);
						iFood += (eYield == YIELD_FOOD) ? iYieldChange : 0;
						
						iValue += std::max(0, iFood * 2 - 1);
						if (pLoopPlot->isBeingWorked())
						{
							iValue += 4;
							iWorkedCount++;
						}
						iValue += ((pLoopPlot->getBonusType(getTeam()) != NO_BONUS) ? 8 : 0);
					}
				}
			}
		}
	}
	if (iWorkedCount == 0)
	{
		SpecialistTypes eDefaultSpecialist = (SpecialistTypes)GC.getDefineINT("DEFAULT_SPECIALIST");
		if ((getPopulation() > 2) && ((eDefaultSpecialist == NO_SPECIALIST) || (getSpecialistCount(eDefaultSpecialist) == 0)))
		{
			iValue /= 2;		
		}
	}

	return iValue;
}


int CvCityAI::AI_yieldMultiplier(YieldTypes eYield)
{
	PROFILE_FUNC();
	
	int iMultiplier = getBaseYieldRateModifier(eYield);
	
	if (eYield == YIELD_PRODUCTION)
	{
		iMultiplier += (getMilitaryProductionModifier() / 2);
		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			iMultiplier += getUnitCombatProductionModifier((UnitCombatTypes)iI) / 4;
		}
	}
	
	if (eYield == YIELD_COMMERCE)
	{
		iMultiplier += (getTotalCommerceRateModifier(COMMERCE_RESEARCH) * 60) / 100;
		iMultiplier += (getTotalCommerceRateModifier(COMMERCE_GOLD) * 35) / 100;
		iMultiplier += (getTotalCommerceRateModifier(COMMERCE_CULTURE) * 15) / 100;
	}
	
	return iMultiplier;	
}
//this should be called before doing governor stuff.
//This is the function which replaces emphasis
//Could stand for a Commerce Variety to be added
//especially now that there is Espionage
void CvCityAI::AI_updateSpecialYieldMultiplier()
{
	PROFILE_FUNC();
	
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSpecialYieldMultiplier[iI] = 0;
	}
	
	UnitTypes eProductionUnit = getProductionUnit();
	if (eProductionUnit != NO_UNIT)
	{
		if (GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_WORKER_SEA)
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 50;				
		}
		if ((GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_WORKER) ||
			(GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_SETTLE))
		
		{
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 50;			
		}
	}

	BuildingTypes eProductionBuilding = getProductionBuilding();
	if (eProductionBuilding != NO_BUILDING)
	{
		if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eProductionBuilding).getBuildingClassType()))
			|| isProductionProject())
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 25;					
		}
		m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += std::max(-25, GC.getBuildingInfo(eProductionBuilding).getFoodKept());
		
		if ((GC.getBuildingInfo(eProductionBuilding).getCommerceChange(COMMERCE_CULTURE) > 0)
			|| (GC.getBuildingInfo(eProductionBuilding).getCommercePerPopChange(COMMERCE_CULTURE) > 0)
			|| (GC.getBuildingInfo(eProductionBuilding).getObsoleteSafeCommerceChange(COMMERCE_CULTURE) > 0))
		{
			int iTargetCultureRate = AI_calculateTargetCulturePerTurn();
			// cppcheck-suppress knownConditionTrueFalse
			if (iTargetCultureRate > 0)
			{
				if (getCommerceRate(COMMERCE_CULTURE) == 0)
				{
					m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
				}
				else if (getCommerceRate(COMMERCE_CULTURE) < iTargetCultureRate)
				{
					m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 20;					
				}
			}
		}
	}
	
	// non-human production value increase
	if (!isHuman())
	{
		CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
		AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());
		
		if ((kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER) && getPopulation() >= 4)
			|| (eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) 
			|| (eAreaAIType == AREAAI_MASSING) || (eAreaAIType == AREAAI_ASSAULT))
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 10;
			if (!kPlayer.AI_isFinancialTrouble())
			{
				m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 40;
			}
		}
		
		int iIncome = 1 + kPlayer.getCommerceRate(COMMERCE_GOLD) + kPlayer.getCommerceRate(COMMERCE_RESEARCH) + std::max(0, kPlayer.getGoldPerTurn());
		int iExpenses = 1 + kPlayer.calculateInflatedCosts() - std::min(0, kPlayer.getGoldPerTurn());
		FAssert(iIncome > 0);
		
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*       Divide By Zero Fix                                                                     */
/************************************************************************************************/
		int iRatio = (100 * iExpenses) / std::max(1, iIncome);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		//Gold -> Production Reduced To
		// 40- -> 100%
		// 60 -> 83%
		// 100 -> 28%
		// 110+ -> 14%
		m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 100;
		if (iRatio > 60)
		{
			//Greatly decrease production weight 
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] *= std::max(10, 120 - iRatio);
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] /= 72;
		}
		else if (iRatio > 40)
		{
			//Slightly decrease production weight.
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] *= 160 - iRatio;
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] /= 120;
		}
		m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] -= 100;
	}
}

int CvCityAI::AI_specialYieldMultiplier(YieldTypes eYield)
{
	return m_aiSpecialYieldMultiplier[eYield];
}


int CvCityAI::AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand, bool bWater)
{
	int iCount = 0;
	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		
		if (pLoopPlot != NULL)
		{
			if ((pLoopPlot->area() == area()) || (bWater && pLoopPlot->isWater()))
			{
				BonusTypes eLoopBonus = pLoopPlot->getBonusType(getTeam());
				if (eLoopBonus != NO_BONUS)
				{
					if ((eBonus == NO_BONUS) || (eBonus == eLoopBonus))
					{
						if (bIncludeOurs && (pLoopPlot->getOwnerINLINE() == getOwnerINLINE()) && (pLoopPlot->getWorkingCity() == this))
						{
							iCount++;                    
						}
						else if (bIncludeNeutral && (!pLoopPlot->isOwned()))
						{
							iCount++;
						}
						else if ((iOtherCultureThreshold > 0) && (pLoopPlot->isOwned() && (pLoopPlot->getOwnerINLINE() != getOwnerINLINE())))
						{
							if ((pLoopPlot->getCulture(pLoopPlot->getOwnerINLINE()) - pLoopPlot->getCulture(getOwnerINLINE())) < iOtherCultureThreshold)
							{
								iCount++;
							}                        
						}
					}
				}
			}
		}
	}
	
	
	return iCount;    
	
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/14/09                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
int CvCityAI::AI_countNumImprovableBonuses( bool bIncludeNeutral, TechTypes eExtraTech, bool bLand, bool bWater )
{
	int iCount = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		
		if (pLoopPlot != NULL)
		{
			if ((bLand && pLoopPlot->area() == area()) || (bWater && pLoopPlot->isWater()))
			{
				BonusTypes eLoopBonus = pLoopPlot->getBonusType(getTeam());
				if (eLoopBonus != NO_BONUS)
				{
					if ( ((pLoopPlot->getOwnerINLINE() == getOwnerINLINE()) && (pLoopPlot->getWorkingCity() == this)) || (bIncludeNeutral && (!pLoopPlot->isOwned())))
					{
						const std::vector<std::pair<ImprovementTypes,BuildTypes> >* improvements = GC.getBonusInfo(eLoopBonus).getTradeProvidingImprovements();

						for(std::vector<std::pair<ImprovementTypes,BuildTypes> >::const_iterator itr = improvements->begin(); itr != improvements->end(); ++itr)
						{
							if ( GET_PLAYER(getOwnerINLINE()).canBuild(pLoopPlot, itr->second) )
							{
								iCount++;
								break;
							}
							else if( (eExtraTech != NO_TECH) )
							{
								CvBuildInfo& kBuild = GC.getBuildInfo(itr->second);

								//	Koshling - not checking if eExtraTech obsoletes the build since we ARE checking is ENABLES it, and it
								//	makes no sense for the same tech to boh enable and obsolete a build.  However, we DO need to check that
								//	we do not ALREADY have an obsoleting tech
								if (kBuild.getTechPrereq() == eExtraTech &&
									(kBuild.getObsoleteTech() == NO_TECH || !GET_TEAM(getTeam()).isHasTech((TechTypes)kBuild.getObsoleteTech())))
								{
									iCount++;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	
	
	return iCount;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

int CvCityAI::AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance)
{
	FAssert(GET_PLAYER(eIndex).isAlive());
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/16/10                              jdog5000        */
/*                                                                                              */
/* War tactics AI                                                                               */
/************************************************************************************************/
/* original bts code
	FAssert(eIndex != getID());
*/
	// No point checking player type against city ID ... Firaxis copy and paste error from
	// CvPlayerAI version of this function
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if ((m_iCachePlayerClosenessTurn != GC.getGame().getGameTurn())
		|| (m_iCachePlayerClosenessDistance != iMaxDistance))
	{
		AI_cachePlayerCloseness(iMaxDistance);
	}
	
	return m_aiPlayerCloseness[eIndex];	
}

void CvCityAI::AI_cachePlayerCloseness(int iMaxDistance)
{
	PROFILE_FUNC();

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						5/16/10				jdog5000		*/
/* 																				*/
/* 	General AI, closeness changes												*/
/********************************************************************************/	
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && 
			((GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))))
		{
			int iValue = 0;
			int iBestValue = 0;
			int iLoop = 0;
			for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				if( pLoopCity == this )
				{
					continue;
				}

				int iDistance = stepDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
				
				if (area() != pLoopCity->area() )
				{
					iDistance += 1;
					iDistance /= 2;
				}
				if (iDistance <= iMaxDistance)
				{
					if ( getArea() == pLoopCity->getArea() )
					{
						int iPathDistance = GC.getMap().calculatePathDistance(plot(), pLoopCity->plot());
						if (iPathDistance > 0)
						{
							iDistance = iPathDistance;
						}
					}
					if (iDistance <= iMaxDistance)
					{
						// Weight by population of both cities, not just pop of other city
						//iTempValue = 20 + 2*pLoopCity->getPopulation();
						int iTempValue = 20 + pLoopCity->getPopulation() + getPopulation();

						iTempValue *= (1 + (iMaxDistance - iDistance));
						iTempValue /= (1 + iMaxDistance);
						
						//reduce for small islands.
						int iAreaCityCount = pLoopCity->area()->getNumCities();
						iTempValue *= std::min(iAreaCityCount, 5);
						iTempValue /= 5;
						if (iAreaCityCount < 3)
						{
							iTempValue /= 2;
						}
						
						if (pLoopCity->isHominid())
						{
							iTempValue /= 4;
						}
						
						iValue += iTempValue;					
						iBestValue = std::max(iBestValue, iTempValue);
					}
				}
			}
			m_aiPlayerCloseness[iI] = (iBestValue + iValue / 4);
		}
	}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END							        */
/********************************************************************************/
	
	m_iCachePlayerClosenessTurn = GC.getGame().getGameTurn();	
	m_iCachePlayerClosenessDistance = iMaxDistance;
}

int	CvCityAI::getPlayerDangerPercentage(PlayerTypes ePlayer, int& iModifier) const
{
	iModifier = 0;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	bool bCrushStrategy = GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_CRUSH);

	int	iResult = 0;
	if ((bCrushStrategy) && (GET_TEAM(getTeam()).AI_getWarPlan(kPlayer.getTeam()) != NO_WARPLAN))
	{
		iResult = 400;						
	}
	else if (atWar(getTeam(), kPlayer.getTeam()))
	{
		iResult = 300;
	}
	// Beef up border security before starting war, but not too much
	else if ( GET_TEAM(getTeam()).AI_getWarPlan(kPlayer.getTeam()) != NO_WARPLAN )
	{
		iResult = 180;
	}
	// Extra trust of/for Vassals, regardless of relations
	else if ( GET_TEAM(kPlayer.getTeam()).isVassal(getTeam()) ||
				GET_TEAM(getTeam()).isVassal(kPlayer.getTeam()))
	{
		iResult = 30;
	}
	else
	{
		switch (GET_PLAYER(getOwnerINLINE()).AI_getAttitude(ePlayer))
		{
		case ATTITUDE_FURIOUS:
			iResult = 180;
			break;

		case ATTITUDE_ANNOYED:
			iResult = 130;
			break;

		case ATTITUDE_CAUTIOUS:
			iResult = 100;
			break;

		case ATTITUDE_PLEASED:
			iResult = 50;
			break;

		case ATTITUDE_FRIENDLY:
			iResult = 20;
			break;

		default:
			FAssert(false);
			break;
		}

/********************************************************************************/
/*	RevDCM Better BUG AI changes	28.10.2010							Fuyu	*/
/********************************************************************************/
		// Beef up border security next to powerful rival, (Fuyu) just not too much if our units are weaker on average
		if( kPlayer.getPower() > GET_PLAYER(getOwnerINLINE()).getPower() )
		{
			int iTempMultiplier = std::min( 400, (100 * kPlayer.getPower())/std::max(1, GET_PLAYER(getOwnerINLINE()).getPower()) );
			iTempMultiplier += range(( (100 * kPlayer.getNumMilitaryUnits())/std::max(1, GET_PLAYER(getOwnerINLINE()).getNumMilitaryUnits()) ), 100, iTempMultiplier);
			iTempMultiplier /= 2;
			iResult *= iTempMultiplier;
			iResult /= 100;
		}
/********************************************************************************/
/*	RevDCM Better BUG AI changes	28.10.2010							END		*/
/********************************************************************************/


/************************************************************************************************/
/* UNOFFICIAL_PATCH                       01/04/09                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* orginal bts code
		if (bCrushStrategy)
		{
			iValue /= 2;
		}
*/
		if (bCrushStrategy)
		{
			iResult /= 2;
		}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
	}

	//	Koshling - modify by the difference in difficulty levels between us and the player
	//	concerned (upwards only). This is to encourage the AI to be more prepared against humnas
	//	playing on higher difficulties
	int	iDifficultyDifference = (GET_PLAYER(getOwnerINLINE()).getHandicapType() - GET_PLAYER(ePlayer).getHandicapType());
	if ( iDifficultyDifference < 0 )
	{
		iModifier = -10*iDifficultyDifference;
		iResult = (iResult*(100 + iModifier))/100;
	}

	return iResult;
}

int CvCityAI::AI_cityThreat(TeamTypes eTargetTeam, int* piThreatModifier)
{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/04/10                                jdog5000      */
/*                                                                                              */
/* War tactics AI                                                                               */
/************************************************************************************************/
	PROFILE_FUNC();

	if ( m_iCityThreat != -1 && eTargetTeam == NO_TEAM )
	{
		if ( piThreatModifier != NULL )
		{
			*piThreatModifier = m_iCityThreatModifier;
		}
		return m_iCityThreat;
	}

	int iValue = (eTargetTeam == NO_TEAM ? evaluateDanger() : 0);
	int iModifier = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if ((iI != getOwner()) && GET_PLAYER((PlayerTypes)iI).isAlive() && (eTargetTeam == NO_TEAM || GET_PLAYER((PlayerTypes)iI).getTeam() == eTargetTeam))
		{
			int iTempValue = AI_playerCloseness((PlayerTypes)iI, DEFAULT_PLAYER_CLOSENESS);
			if (iTempValue > 0)
			{
				int iTempModifier;

				iTempValue *= getPlayerDangerPercentage((PlayerTypes)iI, iTempModifier);
				iTempValue /= 100;
				iValue += iTempValue;

				iModifier += iTempModifier;
			}
		}
	}
	
	if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		int iCurrentEra = GET_PLAYER(getOwnerINLINE()).getCurrentEra();
		iValue += std::max(0, ((10 * iCurrentEra) / 3) - 6); //there are better ways to do this
	}
	
	iValue += getNumActiveWorldWonders() * 5;

	if (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
	{
		iValue += 5;
		iValue += getTotalCommerceRateModifier(COMMERCE_CULTURE) / 20;
		if (getCultureLevel() >= (GC.getNumCultureLevelInfos() - 2))
		{
			iValue += 20;
			if (getCultureLevel() >= (GC.getNumCultureLevelInfos() - 1))
			{
				iValue += 30;
			}
		}
	}
	
	iValue += 2 * GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 3, false);
/************************************************************************************************/
/* REVOLUTION_MOD                         05/22/08                                jdog5000      */
/*                                                                                              */
/* Revolution AI                                                                                */
/************************************************************************************************/
	if( getRevolutionIndex() > 750 )
	{
		iValue += getRevolutionIndex() / 25;
	}
	else
	{
		iValue += getRevolutionIndex() / 100;
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	
	m_iCityThreat = iValue;
	m_iCityThreatModifier = iModifier;
	if ( piThreatModifier != NULL )
	{
		*piThreatModifier = m_iCityThreatModifier;
	}

	return iValue;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
}

//Workers have/needed is not intended to be a strict
//target but rather an indication.
//if needed is at least 1 that means a worker
//will be doing something useful
int CvCityAI::AI_getWorkersHave()
{
	return m_iWorkersHave;	
}

int CvCityAI::AI_getWorkersNeeded()
{
	return m_iWorkersNeeded;
}

void CvCityAI::AI_changeWorkersHave(int iChange)
{
	m_iWorkersHave += iChange;
	//FAssert(m_iWorkersHave >= 0);
	m_iWorkersHave = std::max(0, m_iWorkersHave);
}
	
//This needs to be serialized for human workers.
void CvCityAI::AI_updateWorkersNeededHere()
{
	PROFILE_FUNC();

	short aiYields[NUM_YIELD_TYPES];
	
	int iWorkersNeeded = 0;
	int iWorkersHave = 0;
	int iUnimprovedWorkedPlotCount = 0;
	int iUnimprovedUnworkedPlotCount = 0;
	int iWorkedUnimprovableCount = 0;
	int iImprovedUnworkedPlotCount = 0;
	
	int iSpecialCount = 0;
	
	int iWorstWorkedPlotValue = MAX_INT;
	int iBestUnworkedPlotValue = 0;
	
	iWorkersHave = 0;
	
	if (getProductionUnit() != NO_UNIT)
	{
		if (getProductionUnitAI() == UNITAI_WORKER)
		{
			if (getProductionTurnsLeft() <= 2)
			{
				iWorkersHave++;
			}
		}
	}

/********************************************************************************/
/* 	Worker Counting						03.08.2010				Fuyu			*/
/********************************************************************************/
	//iWorkersHave += iWorkersHaveByPlotTargetMissionAI;
	int iWorkersHaveByPlotTargetMissionAI = AI_workingCityPlotTargetMissionAIs(getOwnerINLINE(), MISSIONAI_BUILD, UNITAI_WORKER, true);
	int iWorkersHaveNewlyBuilt = 0;
/********************************************************************************/
/* 	Worker Counting 											END 			*/
/********************************************************************************/

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);
		if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
		{
			if (pLoopPlot->getArea() == getArea())
			{
				// BBAI TODO: Check late game slowness 

				//How slow is this? It could be almost NUM_CITY_PLOT times faster
				//by iterating groups and seeing if the plot target lands in this city
				//but since this is only called once/turn i'm not sure it matters.

/********************************************************************************/
/* 	Worker Counting							03.08.2010			Fuyu			*/
/********************************************************************************/
//Fuyu: it might matter, so if we can make it NUM_CITY_PLOT times faster
//then lets do it. Moved to before the loop
/* original code
				iWorkersHave += (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD));
*/

//was counting workers twice because missionais are never reset
/* original code
				iWorkersHave += pLoopPlot->plotCount(PUF_isUnitAIType, UNITAI_WORKER, -1, getOwner(), getTeam(), PUF_isNoMission, -1, -1);
*/
				//iWorkersHave += pLoopPlot->plotCount(PUF_isUnitAIType, UNITAI_WORKER, -1, getOwner(), getTeam(), PUF_isNoMissionAI, -1, -1);
				iWorkersHaveNewlyBuilt += pLoopPlot->plotCount(PUF_isUnitAIType, UNITAI_WORKER, -1, NULL, getOwner(), getTeam(), PUF_isNoMissionAI, -1, -1);
/********************************************************************************/
/* 	Worker Counting 											END 			*/
/********************************************************************************/

				if (iI != CITY_HOME_PLOT)
				{
					if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
					{
						if (pLoopPlot->isBeingWorked())
						{
							if (AI_getBestBuild(iI) != NO_BUILD)
							{
								iUnimprovedWorkedPlotCount++;
							}
							else
							{
								iWorkedUnimprovableCount++;
							}
						}
						else
						{
							if (AI_getBestBuild(iI) != NO_BUILD)
							{
								iUnimprovedUnworkedPlotCount++;
							}
						}
					}
					else
					{
						if (!pLoopPlot->isBeingWorked())
						{
							iImprovedUnworkedPlotCount++;
						}
					}

					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						aiYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
					}

					if (pLoopPlot->isBeingWorked())
					{
						int iPlotValue = AI_yieldValue(aiYields, NULL, false, false, false, false, true, true);
						iWorstWorkedPlotValue = std::min(iWorstWorkedPlotValue, iPlotValue);					
					}
					else
					{
						int iPlotValue = AI_yieldValue(aiYields, NULL, false, false, false, false, true, true);
						iBestUnworkedPlotValue = std::max(iBestUnworkedPlotValue, iPlotValue);
					}
				}
			}
		}
	}
	//specialists?
	
	iUnimprovedWorkedPlotCount += std::min(iUnimprovedUnworkedPlotCount, iWorkedUnimprovableCount) / 2;
	
	iWorkersNeeded += 2 * iUnimprovedWorkedPlotCount;
	
	int iBestPotentialPlotValue = -1;
	if (iWorstWorkedPlotValue != MAX_INT)
	{
		//Add an additional citizen to account for future growth.
		int iBestPlot = -1;
		SpecialistTypes eBestSpecialist = NO_SPECIALIST;
		
		if (angryPopulation() == 0)
		{
			AI_addBestCitizen(true, true, &iBestPlot, &eBestSpecialist);
		}
		
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iI);

				if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this && pLoopPlot->getArea() == getArea())
				{
					if (AI_getBestBuild(iI) != NO_BUILD)
					{
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiYields[iJ] = pLoopPlot->getYieldWithBuild(m_aeBestBuild[iI], (YieldTypes)iJ, true);
						}
						
						int iPlotValue = AI_yieldValue(aiYields, NULL, false, false, false, false, true, true);
						ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(AI_getBestBuild(iI)).getImprovement();
						if (eImprovement != NO_IMPROVEMENT)
						{
							if ((getImprovementFreeSpecialists(eImprovement) > 0) || (GC.getImprovementInfo(eImprovement).getHappiness() > 0))
							{
								iSpecialCount ++;
							}
						}
						iBestPotentialPlotValue = std::max(iBestPotentialPlotValue, iPlotValue);
					}
				}
			}
		}
		
		if (iBestPlot != -1)
		{
			setWorkingPlot(iBestPlot, false);
		}
		if (eBestSpecialist != NO_SPECIALIST)
		{
			changeSpecialistCount(eBestSpecialist, -1);
		}
		
		if (iBestPotentialPlotValue > iWorstWorkedPlotValue)
		{
			iWorkersNeeded += 2;			
		}
	}
	
	iWorkersNeeded += (std::max(0, iUnimprovedWorkedPlotCount - 1) * (GET_PLAYER(getOwnerINLINE()).getCurrentEra())) / 3;
	
	if (GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
	{
		iWorkersNeeded *= 3;
		iWorkersNeeded /= 2;
	}
	
	if (iWorkersNeeded > 0)
	{
		iWorkersNeeded++;
		iWorkersNeeded = std::max(1, iWorkersNeeded / 3);
	}
	
	int iSpecialistExtra = std::min((getSpecialistPopulation() - totalFreeSpecialists()), iUnimprovedUnworkedPlotCount);
	iSpecialistExtra -= iImprovedUnworkedPlotCount;
	
	iWorkersNeeded += std::max(0, 1 + iSpecialistExtra) / 2;
	
	if (iWorstWorkedPlotValue <= iBestUnworkedPlotValue && iBestUnworkedPlotValue >= iBestPotentialPlotValue)
	{
		iWorkersNeeded /= 2;
	}
	if (angryPopulation(1) > 0)
	{
		iWorkersNeeded++;
		iWorkersNeeded /= 2;
	}
	
	iWorkersNeeded += (iSpecialCount + 1) / 2;
	
	iWorkersNeeded = std::max((iUnimprovedWorkedPlotCount + 1) / 2, iWorkersNeeded);
/********************************************************************************/
/* 	Worker Counting						03.08.2010				Fuyu			*/
/********************************************************************************/
	//if( gCityLogLevel >= 3 ) logBBAI("      City %S has %d workers: %d from plotTarget, %d newly built, %d finished soon", getName().GetCString(), (iWorkersHave + iWorkersHaveNewlyBuilt + iWorkersHaveByPlotTargetMissionAI), iWorkersHaveByPlotTargetMissionAI, iWorkersHaveNewlyBuilt, iWorkersHave);
	iWorkersHave += iWorkersHaveNewlyBuilt;
	iWorkersHave += iWorkersHaveByPlotTargetMissionAI;
/********************************************************************************/
/* 	Worker Counting 											END 			*/
/********************************************************************************/

	m_iWorkersNeeded = iWorkersNeeded;
	m_iWorkersHave = iWorkersHave;
}
/********************************************************************************/
/* 	Worker Counting						03.08.2010				Fuyu			*/
/********************************************************************************/
int CvCityAI::AI_workingCityPlotTargetMissionAIs(PlayerTypes ePlayer, MissionAITypes eMissionAI, UnitAITypes eUnitAI, bool bSameAreaOnly) const
{
	PROFILE_FUNC();

	if (this == NULL)
	{
		return 0;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	bool bCanMoveAllTerrain = bSameAreaOnly; //only check if bSameAreaOnly
	int iCount = 0;

	int iLoop = 0;
	for(CvSelectionGroup* pLoopSelectionGroup = kPlayer.firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = kPlayer.nextSelectionGroup(&iLoop))
	{
		CvPlot* pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

		if (pMissionPlot != NULL)
		{
			if (eMissionAI == NO_MISSIONAI || pLoopSelectionGroup->AI_getMissionAIType() == eMissionAI)
			{
				if (pMissionPlot->getWorkingCity() == this)
				{
					if (eUnitAI == NO_UNITAI && !bSameAreaOnly)
					{
						iCount += pLoopSelectionGroup->getNumUnits();
					}
					else
					{
						CvUnit* pHeadUnit = pLoopSelectionGroup->getHeadUnit();
						if (pHeadUnit != NULL)
						{
							int iCorrectUnitAICount = 0;
							for (CvSelectionGroup::unit_iterator unitItr = pLoopSelectionGroup->beginValidUnits(); unitItr != pLoopSelectionGroup->endValidUnits(); ++unitItr)
							{
								if (bCanMoveAllTerrain && !(unitItr->canMoveAllTerrain()))
								{
									bCanMoveAllTerrain = false;
								}
								if (eUnitAI == NO_UNITAI || unitItr->AI_getUnitAIType() == eUnitAI)
								{
									iCorrectUnitAICount++;
								}
							}
							if (!bSameAreaOnly || bCanMoveAllTerrain || pHeadUnit->getArea() == pMissionPlot->getArea())
							{
								iCount += iCorrectUnitAICount;
							}
						}
					}
				}
			}
		}
	}

	return iCount;
}
/********************************************************************************/
/* 	Worker Counting 											END 			*/
/********************************************************************************/

BuildingTypes CvCityAI::AI_bestAdvancedStartBuilding(int iPass)
{
	int iFocusFlags = 0;
/************************************************************************************************/
/* Afforess	                  Start		 06/07/10                                               */
/************************************************************************************************/
	if (iPass >= 0)
	{
		iFocusFlags |= BUILDINGFOCUS_CULTURE;
	}
	if (iPass >= 1)
	{
		iFocusFlags |= BUILDINGFOCUS_FOOD;
	}
	if (iPass >= 2)
	{
		iFocusFlags |= BUILDINGFOCUS_PRODUCTION;
	}
	if (iPass >= 3)
	{
		iFocusFlags |= BUILDINGFOCUS_EXPERIENCE;
	}
	if (iPass >= 4)
	{
		iFocusFlags |= (BUILDINGFOCUS_HAPPY | BUILDINGFOCUS_HEALTHY);
	}
	if (iPass >= 5)
	{
		iFocusFlags |= (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH | BUILDINGFOCUS_MAINTENANCE);
		if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
		{
			iFocusFlags |= BUILDINGFOCUS_ESPIONAGE;
		}
	}
			
	return AI_bestBuildingThreshold(iFocusFlags, 0, std::max(0, 25 - iPass * 5));
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}

//
//
//
void CvCityAI::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	CvCity::read(pStream);

	uint uiFlag=0;
	WRAPPER_READ(wrapper, "CvCityAI", &uiFlag);	// flags for expansion

	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeAvoidGrowthCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeGreatPeopleCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bAssignWorkDirty);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bChooseProductionDirty);
/************************************************************************************************/
/* Afforess	                  Start		 02/10/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeAvoidAngryCitizensCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeAvoidUnhealthyCitizensCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bMilitaryProductionCity);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	WRAPPER_READ(wrapper, "CvCityAI", (int*)&m_routeToCity.eOwner);
	WRAPPER_READ(wrapper, "CvCityAI", &m_routeToCity.iID);

	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_COMMERCE_TYPES, m_aiEmphasizeCommerceCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bForceEmphasizeCulture);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_CITY_PLOTS, m_aiBestBuildValue);
	WRAPPER_READ_CLASS_ENUM_ARRAY(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_BUILDS, NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", GC.getNumEmphasizeInfos(), m_pbEmphasize);

	//	Old saves incorrectly had this as a plain array - allow either on load - whichever is not present will be skipped
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", GC.getNumSpecialistInfos(), m_pbEmphasizeSpecialist);
	WRAPPER_READ_CLASS_ARRAY_DECORATED(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_pbEmphasizeSpecialist, "EmphasizeSpecialist");

	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiSpecialYieldMultiplier);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iCachePlayerClosenessTurn);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iCachePlayerClosenessDistance);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", MAX_PLAYERS, m_aiPlayerCloseness);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iNeededFloatingDefenders);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iNeededFloatingDefendersCacheTurn);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iWorkersNeeded);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iWorkersHave);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iTempBuildPriority);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iBuildPriority);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bNavalMilitaryProductionCity);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iUnitSummonedLastEstablishedTurn);

	WRAPPER_READ_OBJECT_END(wrapper);
}

//
//
//
void CvCityAI::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	CvCity::write(pStream);

	uint uiFlag=0;
	WRAPPER_WRITE(wrapper, "CvCityAI", uiFlag);		// flag for expansion

	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeAvoidGrowthCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeGreatPeopleCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bAssignWorkDirty);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bChooseProductionDirty);
/************************************************************************************************/
/* Afforess	                  Start		 02/10/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeAvoidAngryCitizensCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeAvoidUnhealthyCitizensCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bMilitaryProductionCity);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvCityAI", m_routeToCity.eOwner);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_routeToCity.iID);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_COMMERCE_TYPES, m_aiEmphasizeCommerceCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bForceEmphasizeCulture);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_CITY_PLOTS, m_aiBestBuildValue);
	WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_BUILDS, NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", GC.getNumEmphasizeInfos(), m_pbEmphasize);
	WRAPPER_WRITE_CLASS_ARRAY_DECORATED(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_pbEmphasizeSpecialist, "EmphasizeSpecialist");
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiSpecialYieldMultiplier);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iCachePlayerClosenessTurn);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iCachePlayerClosenessDistance);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", MAX_PLAYERS, m_aiPlayerCloseness);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iNeededFloatingDefenders);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iNeededFloatingDefendersCacheTurn);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iWorkersNeeded);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iWorkersHave);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iTempBuildPriority);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iBuildPriority);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bNavalMilitaryProductionCity);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iUnitSummonedLastEstablishedTurn);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

/************************************************************************************************/
/* RevDCM                  Start		 5/2/09                                                 */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
bool CvCityAI::AI_trainInquisitor()
{
	CvPlayerAI& kPlayerAI = GET_PLAYER(getOwnerINLINE());

	if( GC.getGameINLINE().isOption(GAMEOPTION_NO_INQUISITIONS)
	|| !kPlayerAI.isInquisitionConditions() )
	{
		return false;
	}
	if(!kPlayerAI.hasInquisitionTarget())
	{
		return false;
	}
	if(getPopulation() < GC.getDefineINT("MIN_POP_CONSIDER_INQUISITORS"))
	{
		return false;
	}

	int iUnitCost = MAX_INT;
	UnitTypes eBestUnit = NO_UNIT;
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(kPlayerAI.getCivilizationType());

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)kCivilization.getCivilizationUnits((UnitClassTypes)iI);
		if (eLoopUnit == NO_UNIT || !GC.getUnitInfo(eLoopUnit).isInquisitor())
		{
			continue;
		}
		if (canTrain(eLoopUnit, false, false))
		{
			if (GC.getUnitInfo(eLoopUnit).getProductionCost() < iUnitCost)
			{
				eBestUnit = eLoopUnit;
				iUnitCost = GC.getUnitInfo(eBestUnit).getProductionCost();
			}
		}
	}
	if (eBestUnit == NO_UNIT)
	{
		return false;
	}

	int iBuildOdds = 0;
	if(kPlayerAI.isPushReligiousVictory())
	{
		iBuildOdds += 30;
	} else if(kPlayerAI.isConsiderReligiousVictory())
	{
		iBuildOdds += 7;
	}
	if(kPlayerAI.countCityReligionRevolts() > 3)
	{
		iBuildOdds += kPlayerAI.countCityReligionRevolts() * 20;
	}else 
	{
		iBuildOdds += kPlayerAI.countCityReligionRevolts() * 10;
	}
	if(kPlayerAI.isNonStateReligionCommerce())
	{
		iBuildOdds /= 2;
	}

	iBuildOdds -= 3;
	if (iBuildOdds > 0)
	{
		if (GC.getGameINLINE().getSorenRandNum(100, "AI choose Inquisitor") < iBuildOdds)
		{
			pushOrder(ORDER_TRAIN, eBestUnit, -1, false, false, false);
			return true;
		}
	}

	return false;
}
/************************************************************************************************/
/* Inquisitions	                     END                                                        */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 05/28/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvCityAI::AI_getEmphasizeAvoidAngryCitizensCount()
{
	return m_iEmphasizeAvoidAngryCitizensCount;
}

bool CvCityAI::AI_isEmphasizeAvoidAngryCitizens()
{
	return (AI_getEmphasizeAvoidAngryCitizensCount() > 0);
}

int CvCityAI::AI_getEmphasizeAvoidUnhealthyCitizensCount()
{
	return m_iEmphasizeAvoidUnhealthyCitizensCount;
}

bool CvCityAI::AI_isEmphasizeAvoidUnhealthyCitizens()
{
	return (AI_getEmphasizeAvoidUnhealthyCitizensCount() > 0);
}

//	Koshling  - this method is unused!!
bool CvCityAI::AI_buildCaravan()
{
	FAssert(false);
	
	int iAveProduction = 0;
	int iLoop = 0;
	for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		iAveProduction += pLoopCity->getYieldRate(YIELD_PRODUCTION);
	}
	int iNumCities = GET_PLAYER(getOwnerINLINE()).getNumCities();
	iAveProduction /= iNumCities;

	UnitTypes eBestUnit = NO_UNIT;
	if (((2 * getYieldRate(YIELD_PRODUCTION)) / 3) > iAveProduction)
	{
		int iBestHurry = 0;
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (GC.getUnitInfo((UnitTypes)iI).getBaseHurry() > iBestHurry)
			{
				if (canTrain((UnitTypes)iI, false, true, false))
				{
					iBestHurry = GC.getUnitInfo((UnitTypes)iI).getBaseHurry();
					eBestUnit = (UnitTypes)iI;
				}
			}
		}
		if (eBestUnit != NO_UNIT)
		{
			int iOdds = 100;
			if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
				iOdds *= 2;
			iOdds /= iNumCities;
			iOdds *= iAveProduction;
			iOdds /= std::max(1, getYieldRate(YIELD_PRODUCTION));
			iOdds = std::max(1, iOdds);
			if (GC.getGameINLINE().getSorenRandNum(iOdds, "Caravan Production") == 0)
			{
				pushOrder(ORDER_TRAIN, eBestUnit, -1, false, false, false);
				//GC.getGameINLINE().logMsg("City %S built a caravan", getName().GetCString());
				return true;
			}
		}
	}
	return false;
}

int CvCityAI::AI_getPromotionValue(PromotionTypes ePromotion) const
{
	int iPromotionCount = 0;

	int iCanTrainCount = 0;
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);
		if (eLoopUnit != NO_UNIT)
		{
/************************************************************************************************/
/* Afforess	                  Start		 6/11/11                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		if ((GC.getUnitInfo(eLoopUnit).getCombat() + 
		GET_TEAM(getTeam()).getUnitClassStrengthChange((UnitClassTypes)GC.getUnitInfo(eLoopUnit).getUnitClassType())) > 0)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
			{
				if (GC.getUnitInfo(eLoopUnit).getUnitCombatType() != NO_UNITCOMBAT)
				{
					if (canTrain(eLoopUnit))
					{
						iCanTrainCount++;
						if (GC.getPromotionInfo(ePromotion).getUnitCombat(GC.getUnitInfo(eLoopUnit).getUnitCombatType()))
						{
							iPromotionCount++;
						}
						//TB SubCombat Mod Begin
						for (int iJ = 0; iJ < GC.getUnitInfo(eLoopUnit).getNumSubCombatTypes(); iJ++)
						{
							UnitCombatTypes eSubCombatType = ((UnitCombatTypes)GC.getUnitInfo(eLoopUnit).getSubCombatType(iJ));
							if (GC.getPromotionInfo(ePromotion).getUnitCombat(eSubCombatType))
							{
								iPromotionCount++;
							}
						}

						//TB SubCombat Mod End
					}
				}
			}
		}
	}
	if (iCanTrainCount == 0 || iPromotionCount == 0)
	{
		return 0; //Avoid division by 0, and if the city can't train units or the promotion never applies, it's useless
	}

	//Increased based on the number of units that can receive this promotion
	int iMultiplier = 100;
	iMultiplier *= iPromotionCount;
	iMultiplier /= iCanTrainCount;
	iMultiplier *= (GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0) ? 3 : 2;
	
	int iValue = 0;
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);
		if (eLoopUnit != NO_UNIT)
		{
			if (GC.getUnitInfo(eLoopUnit).getCombat() > 0)
			{
				if (GC.getUnitInfo(eLoopUnit).getUnitCombatType() != NO_UNITCOMBAT)
				{
					bool bUnitCanGetPromotion = false;

					if (GC.getPromotionInfo(ePromotion).getUnitCombat(GC.getUnitInfo(eLoopUnit).getUnitCombatType()))
					{
						bUnitCanGetPromotion = true;
					}
					else
					{
						//TB SubCombat Mod Begin
						for (int iJ = 0; iJ < GC.getUnitInfo(eLoopUnit).getNumSubCombatTypes(); iJ++)
						{
							UnitCombatTypes eSubCombatType = ((UnitCombatTypes)GC.getUnitInfo(eLoopUnit).getSubCombatType(iJ));
							if (GC.getPromotionInfo(ePromotion).getUnitCombat(eSubCombatType))
							{
								bUnitCanGetPromotion = true;
								break;
							}
						}
					}
					//TB SubCombat Mod End	
					
					if ( bUnitCanGetPromotion )
					{
						if (canTrain(eLoopUnit))
						{
							iValue += GET_PLAYER(getOwnerINLINE()).AI_promotionValue(ePromotion, eLoopUnit);
						}
					}
				}
			}
		}
	}
	//iValue becomes the average value of the promotion
	iValue /= iPromotionCount;
	
	iValue *= iMultiplier;
	iValue /= 100;
	
	return iValue;
	
}
 
SpecialistTypes CvCity::getBestSpecialist(int iExtra)
{
	int iBestValue = 0;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		if (isSpecialistValid((SpecialistTypes)iI, iExtra))
		{
			int iValue = AI_specialistValue((SpecialistTypes)iI, false, false);
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestSpecialist = (SpecialistTypes)iI;
			}
		}
	}
	
	return eBestSpecialist;
}

int CvCityAI::AI_calculateActualImprovementHealth(ImprovementTypes eImprovement) const
{
	if (eImprovement == NO_IMPROVEMENT) return 0;
	int iHealthPercent = GC.getImprovementInfo(eImprovement).getHealthPercent();
	int iBadHealthPercent = abs(getImprovementBadHealth());
	int iGoodHealthPercent = getImprovementGoodHealth();
	int iHealthChange;
	if (iHealthPercent < 0)
	{
		iHealthChange = iBadHealthPercent - iHealthPercent;
		iHealthChange /= 100;
		return -(iHealthChange - (iBadHealthPercent / 100));
	}
	else
	{
		iHealthChange = iGoodHealthPercent + iHealthPercent;
		iHealthChange /= 100;
		return iHealthChange - (iGoodHealthPercent / 100);
	}
}

bool CvCityAI::AI_isMilitaryProductionCity() const
{
	return m_bMilitaryProductionCity;
}

void CvCityAI::AI_setMilitaryProductionCity(bool bNewVal)
{
	if (bNewVal != AI_isMilitaryProductionCity())
	{
		m_bMilitaryProductionCity = bNewVal;
		AI_setChooseProductionDirty(true);
	}
}

int CvCityAI::AI_getMilitaryProductionRateRank() const
{
	PROFILE_FUNC();

	int iRate = getPopulation() + getYieldRate(YIELD_PRODUCTION) - getYieldRate(YIELD_COMMERCE);
	
	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		if ((DomainTypes)iI != DOMAIN_SEA)
		{
			iRate += ((getDomainProductionModifier((DomainTypes)iI))/10);
			iRate += (getDomainFreeExperience((DomainTypes)iI));
		}
	}

	iRate += (getProductionModifier()/10);
	iRate += getFreeExperience();
	
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)iI).isForMilitary())
		{
			iRate += (getUnitCombatProductionModifier((UnitCombatTypes)iI)/10);
			iRate += getUnitCombatFreeExperience((UnitCombatTypes)iI);
		}
	}
		
	int iRank = 1;

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		int iLoopRate = pLoopCity->getPopulation() + pLoopCity->getYieldRate(YIELD_PRODUCTION) - pLoopCity->getYieldRate(YIELD_COMMERCE);
		if ((iLoopRate > iRate) ||
			((iLoopRate == iRate) && (pLoopCity->getID() < getID())))
		{
			iRank++;
		}
	}
	return iRank;
}

bool CvCityAI::AI_isNavalMilitaryProductionCity() const
{
	return m_bNavalMilitaryProductionCity;
}

void CvCityAI::AI_setNavalMilitaryProductionCity(bool bNewVal)
{
	if (bNewVal != AI_isNavalMilitaryProductionCity())
	{
		m_bNavalMilitaryProductionCity = bNewVal;
		AI_setChooseProductionDirty(true);
	}
}

int CvCityAI::AI_getNavalMilitaryProductionRateRank() const
{
	PROFILE_FUNC();

	if (!isCoastal(8))
	{
		return 0;
	}

	int iRate = getPopulation() + getYieldRate(YIELD_PRODUCTION) - getYieldRate(YIELD_COMMERCE);
	
	
	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		if ((DomainTypes)iI == DOMAIN_SEA)
		{
			iRate += ((getDomainProductionModifier((DomainTypes)iI))/10);
			iRate += (getDomainFreeExperience((DomainTypes)iI));
		}
	}

	iRate += (getProductionModifier()/10);
	iRate += getFreeExperience();
	
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)iI).isForNavalMilitary())
		{
			iRate += (getUnitCombatProductionModifier((UnitCombatTypes)iI)/10);
			iRate += getUnitCombatFreeExperience((UnitCombatTypes)iI);
		}
	}
		
	int iRank = 1;

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		int iLoopRate = pLoopCity->getPopulation() + pLoopCity->getYieldRate(YIELD_PRODUCTION) - pLoopCity->getYieldRate(YIELD_COMMERCE);
		if ((iLoopRate > iRate) ||
			((iLoopRate == iRate) && (pLoopCity->getID() < getID())))
		{
			iRank++;
		}
	}
	return iRank;
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

//	KOSHLING Mod - pre-calculate and cache building values for all focuses
//
//	Simple class to hold the calculated values for one building type and city
#ifdef _DEBUG
//#define VALIDATE_BUILDING_CACHE_CONSISTENCY
#endif

class FocusValueSet
{
public:
	FocusValueSet()
	{
		for(int i = 0; i < NUM_ALL_BUILDINGFOCUS_FLAGS; i++)
		{
			m_focusValues[i] = 0;
		}
	}

	void AccumulateTo(int iFocusIndex, int value)
	{
		FAssertMsg( iFocusIndex < NUM_ALL_BUILDINGFOCUS_FLAGS, "iFocusIndex out of range" )
		m_focusValues[iFocusIndex] += value;
	}

	void AccumulateToAny(int value)
	{
		AccumulateTo(BUILDINGFOUCSINDEX_ANY, value);
	}

	void Accumulate(int iFocusIndex, int value, bool toAny)
	{
		if ( toAny )
		{
			AccumulateToAny(value);
		}
		else
		{
			AccumulateTo(iFocusIndex, value);
		}
	}

	void SetValue(int iFocusIndex, int value)
	{
		m_focusValues[iFocusIndex] = value;
	}

	void SetValueAll(int value)
	{
		for( int i = 0; i < NUM_ALL_BUILDINGFOCUS_FLAGS; i++ )
		{
			m_focusValues[i] = value;
		}
	}

	int	m_focusValues[NUM_ALL_BUILDINGFOCUS_FLAGS];
};

class OneBuildingValueCache
{
public:
	OneBuildingValueCache()
	{
	}

	void AccumulateTo(int iFocusIndex, int value, bool isThresholdSet)
	{
		MEMORY_TRACK_EXEMPT();

		if ( isThresholdSet )
		{
			m_thresholdSet.AccumulateTo(iFocusIndex, value);
		}
		else
		{
			m_noThresholdSet.AccumulateTo(iFocusIndex, value);
		}
	}

	void AccumulateToAny(int value, bool isThresholdSet)
	{
		AccumulateTo(BUILDINGFOUCSINDEX_ANY, value, isThresholdSet);
	}

	void Accumulate(int iFocusIndex, int value)
	{
		AccumulateToAny(value, false);
		AccumulateTo(iFocusIndex, value, true);
	}

	void SetValue(int iFocusIndex, int value, bool isThresholdSet)
	{
		if ( isThresholdSet )
		{
			m_thresholdSet.SetValue(iFocusIndex, value);
		}
		else
		{
			m_noThresholdSet.SetValue(iFocusIndex, value);
		}
	}

	void SetValueAll(int value, bool isThresholdSet)
	{
		for( int i = 0; i < NUM_ALL_BUILDINGFOCUS_FLAGS; i++ )
		{
			SetValue(i, value, isThresholdSet);
		}
	}

	FocusValueSet	m_thresholdSet;
	FocusValueSet	m_noThresholdSet;
};

class BuildingValueCache
{
public:
	explicit BuildingValueCache(CvCityAI* cachedCity)
	{
		m_iCachedFlags	= 0;
		m_bIncomplete	= false;
	}

	virtual ~BuildingValueCache()
	{
		std::map<int,OneBuildingValueCache*>::const_iterator itr;

		for(itr = m_buildingValues.begin(); itr != m_buildingValues.end(); ++itr)
		{
			delete itr->second;
		}
	}

	bool HasValues(BuildingTypes eType)
	{
		return (m_buildingValues.find(eType) != m_buildingValues.end());
	}

	int GetValue(BuildingTypes eType, int iFocusFlags, int threshold, bool bMaximizeFlaggedValue)
	{

		std::map<int,OneBuildingValueCache*>::const_iterator itr = m_buildingValues.find(eType);
		if ( itr == m_buildingValues.end() )
		{
			//	No cached value
			return -1;
		}
		else
		{
			int iValueThreshold = 0;
			int iValueNoThreshold = 0;
			OneBuildingValueCache* valueCache = itr->second;

			for(int i = 0; i < NUM_REAL_BUILDINGFOCUS_FLAGS; i++)
			{
				if ( (iFocusFlags & 1 << i) != 0 )
				{
					iValueThreshold += valueCache->m_thresholdSet.m_focusValues[i];
					iValueNoThreshold += valueCache->m_noThresholdSet.m_focusValues[i];
				}
			}

			//	Add in pseudo-flag contributions
			if ( (iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH)) != 0 )
			{
				iValueThreshold += valueCache->m_thresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDRESEARCH];
				iValueNoThreshold += valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDRESEARCH];
			}
			if ( (iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_MAINTENANCE)) != 0 )
			{
				iValueThreshold += valueCache->m_thresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE];
				iValueNoThreshold += valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE];
			}

			int iPass1Value = iValueThreshold + valueCache->m_thresholdSet.m_focusValues[BUILDINGFOUCSINDEX_ANY];
			if ( iValueThreshold < threshold )
			{
				iPass1Value = 0;
			}

			int iPass2Value;
			if ( (iFocusFlags & BUILDINGFOCUS_CONSIDER_ANY) != 0 )
			{
				iPass2Value = valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOUCSINDEX_ANY];
			}
			else if ( iPass1Value > 0 )
			{
				iPass2Value = iValueNoThreshold + valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOUCSINDEX_ANY];
			}
			else
			{
				iPass2Value = 0;
			}

			//	If we are really seeking an answer specifically on the values from the specific flags
			//	then the rest is just a tie-breaker so dramatically boost the value of the flag-specific
			//	stuff from pass1
			if ( bMaximizeFlaggedValue )
			{
				iPass1Value *= 20;
			}
			else
			{
				//	Even when not maximizing with all the extra effects on buildings pass1 was getting a
				//	bit dominated by pass2 so provide a little extra pas1 weight anyway
				iPass1Value *= 2;
			}

			return std::max(0, iPass1Value + iPass2Value);
		}
	}

	OneBuildingValueCache* GetOneBuildingCache(int eBuilding)
	{
		OneBuildingValueCache* result;

		std::map<int,OneBuildingValueCache*>::const_iterator itr = m_buildingValues.find(eBuilding);
		if ( itr == m_buildingValues.end() )
		{
			MEMORY_TRACK_EXEMPT();

			//	New entry
			result = new OneBuildingValueCache();

			m_buildingValues[eBuilding] = result;
		}
		else
		{
			result = itr->second;
		}

		return result;
	}

	//	One city is cached at a time
	int			m_iCachedFlags;
	bool		m_bIncomplete;
	std::map<int,OneBuildingValueCache*>	m_buildingValues;
};

static bool buildingHasTradeRouteValue(BuildingTypes eBuilding)
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	return (kBuilding.getTradeRoutes() > 0 ||
			kBuilding.getCoastalTradeRoutes() > 0 ||
			kBuilding.getGlobalTradeRoutes() > 0 ||
			kBuilding.getTradeRouteModifier() > 0 ||
			kBuilding.getForeignTradeRouteModifier() > 0);
}

bool CvCityAI::buildingMayHaveAnyValue(BuildingTypes eBuilding, int iFocusFlags)
{
	PROFILE_FUNC();

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if ( kBuilding.isAutoBuild() || kBuilding.getProductionCost() <= 0)
	{
		return false;
	}

	if ( (iFocusFlags & BUILDINGFOCUS_CONSIDER_ANY) != 0 )
	{
		return true;
	}

	bool bIsWonder = isWorldWonderClass((BuildingClassTypes)kBuilding.getBuildingClassType()) || 
					 isTeamWonderClass((BuildingClassTypes)kBuilding.getBuildingClassType()) || 
					  isNationalWonderClass((BuildingClassTypes)kBuilding.getBuildingClassType()) ||
					   isLimitedWonderClass((BuildingClassTypes)kBuilding.getBuildingClassType());

	if ( (iFocusFlags & ~BUILDINGFOCUS_WONDEROK) == BUILDINGFOCUS_WORLDWONDER )
	{
		return bIsWonder;
	}

	if ( (iFocusFlags & BUILDINGFOCUS_WONDEROK) != 0 )
	{
		if ( bIsWonder )
		{
			return true;
		}
	}

	if ( (iFocusFlags & BUILDINGFOCUS_BIGCULTURE) != 0 )
	{
		iFocusFlags |= BUILDINGFOCUS_CULTURE;
	}

	//	Some things cause us to consider the building in any role
	if ( kBuilding.getFreeSpecialist() > 0 ||
		 kBuilding.getAreaFreeSpecialist() > 0 ||
		 kBuilding.getGlobalFreeSpecialist() > 0 ||
		 (kBuilding.getPropertyManipulators() != NULL && kBuilding.getPropertyManipulators()->getNumSources() > 0) ||
		 kBuilding.getProperties()->getNumProperties() > 0 )
	{
		return true;
	}
	if (kBuilding.isPower() || ((kBuilding.getPowerBonus() != NO_BONUS) && hasBonus((BonusTypes)(kBuilding.getPowerBonus()))) || kBuilding.isAreaCleanPower())
	{
		return true;
	}
	if ( kBuilding.getFoundsCorporation() != NO_CORPORATION || kBuilding.getGlobalReligionCommerce() > 0 || kBuilding.getGlobalCorporationCommerce() > 0 )
	{
		return true;
	}

	bool buildingModifiesGenericYields =
			 (kBuilding.getTechYieldChange(NO_TECH, NO_COMMERCE) > 0 ||
			  kBuilding.getBonusYieldModifier(NO_BONUS, NO_COMMERCE) > 0 ||
			  kBuilding.getBonusYieldChanges(NO_BONUS, NO_COMMERCE) > 0 ||
			  kBuilding.getVicinityBonusYieldChanges(NO_BONUS, NO_COMMERCE) > 0);

	bool buildingModifiesCommerceYields =
			 (buildingModifiesGenericYields ||
			  kBuilding.getYieldModifier(YIELD_COMMERCE) > 0 ||
			  GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, YIELD_COMMERCE) > 0 ||
			  kBuilding.getPowerYieldModifier(YIELD_COMMERCE) > 0 ||
			  kBuilding.getSeaPlotYieldChange(YIELD_COMMERCE) > 0 ||
			  kBuilding.getRiverPlotYieldChange(YIELD_COMMERCE) > 0 ||
			  kBuilding.getYieldChange(YIELD_COMMERCE) > 0 ||
			  kBuilding.getYieldPerPopChange(YIELD_COMMERCE) > 0 ||
			  GET_TEAM(getTeam()).getBuildingYieldChange(eBuilding, YIELD_COMMERCE) > 0);

	if ( (iFocusFlags & BUILDINGFOCUS_FOOD) != 0 )
	{
		if ( buildingModifiesGenericYields ||
			 buildingHasTradeRouteValue(eBuilding) ||
			 kBuilding.getFoodKept() > 0 ||
			 kBuilding.getSeaPlotYieldChange(YIELD_FOOD) > 0 ||
			 GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, YIELD_FOOD) > 0 ||
			 kBuilding.getYieldChange(YIELD_FOOD) > 0 ||
			 kBuilding.getYieldPerPopChange(YIELD_FOOD) > 0 ||
			 GET_TEAM(getTeam()).getBuildingYieldChange(eBuilding, YIELD_FOOD) > 0 ||
			 kBuilding.getYieldModifier(YIELD_FOOD) > 0 ||
			 kBuilding.getRiverPlotYieldChange(YIELD_FOOD) > 0 ||
			 kBuilding.getHurryCostModifier() < 0)
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_PRODUCTION) != 0 )
	{
		if ( buildingModifiesGenericYields ||
			 buildingHasTradeRouteValue(eBuilding) ||
			 kBuilding.getYieldModifier(YIELD_PRODUCTION) > 0 ||
			 GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, YIELD_PRODUCTION) > 0 ||
			 kBuilding.getPowerYieldModifier(YIELD_PRODUCTION) > 0 ||
			 kBuilding.getSeaPlotYieldChange(YIELD_PRODUCTION) > 0 ||
			 kBuilding.getRiverPlotYieldChange(YIELD_PRODUCTION) > 0 ||
			 kBuilding.getYieldChange(YIELD_PRODUCTION) > 0 ||
			 kBuilding.getYieldPerPopChange(YIELD_PRODUCTION) > 0 ||
			 GET_TEAM(getTeam()).getBuildingYieldChange(eBuilding, YIELD_PRODUCTION) > 0 ||
			 kBuilding.getHurryCostModifier() < 0 ||
			 kBuilding.getMilitaryProductionModifier() > 0 ||
			 kBuilding.getSpaceProductionModifier() > 0 ||
			 kBuilding.getGlobalSpaceProductionModifier() > 0 ||
			 kBuilding.getDomainProductionModifier(NO_DOMAIN) > 0 ||
			 kBuilding.getNumUnitCombatProdModifiers() > 0)
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_GOLD) != 0 )
	{
		iFocusFlags |= BUILDINGFOCUS_MAINTENANCE;

		if ( buildingModifiesCommerceYields ||
			 buildingHasTradeRouteValue(eBuilding) ||
			 kBuilding.getCommerceChange(COMMERCE_GOLD) > 0 ||
			 kBuilding.getCommercePerPopChange(COMMERCE_GOLD) > 0 ||
			 kBuilding.getCommerceModifier(COMMERCE_GOLD) > 0 ||
			 kBuilding.getGlobalCommerceModifier(COMMERCE_GOLD) > 0 ||
			 kBuilding.getSpecialistExtraCommerce(COMMERCE_GOLD) > 0 ||
			 kBuilding.getStateReligionCommerce(COMMERCE_GOLD) > 0 ||
			 kBuilding.getObsoleteSafeCommerceChange(COMMERCE_GOLD) > 0 ||
			 kBuilding.getCommerceAttacks(COMMERCE_GOLD) > 0 ||
			 kBuilding.getTechCommerceChange(NO_TECH, COMMERCE_GOLD) > 0 ||
			 kBuilding.getTechCommerceModifier(NO_TECH, COMMERCE_GOLD) > 0 )
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_RESEARCH) != 0 )
	{
		if ( buildingModifiesCommerceYields ||
			 buildingHasTradeRouteValue(eBuilding) ||
			 kBuilding.getCommerceChange(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getCommercePerPopChange(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getCommerceModifier(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getGlobalCommerceModifier(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getSpecialistExtraCommerce(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getStateReligionCommerce(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getObsoleteSafeCommerceChange(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getCommerceAttacks(COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getTechCommerceChange(NO_TECH, COMMERCE_RESEARCH) > 0 ||
			 kBuilding.getTechCommerceModifier(NO_TECH, COMMERCE_RESEARCH) > 0)
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_CULTURE) != 0 )
	{
		if ( buildingModifiesCommerceYields ||
			 buildingHasTradeRouteValue(eBuilding) ||
			 kBuilding.getCommerceChange(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getCommercePerPopChange(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getCommerceModifier(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getGlobalCommerceModifier(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getSpecialistExtraCommerce(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getStateReligionCommerce(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getObsoleteSafeCommerceChange(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getCommerceAttacks(COMMERCE_CULTURE) > 0 ||
			 kBuilding.getTechCommerceChange(NO_TECH, COMMERCE_CULTURE) > 0 ||
			 kBuilding.getTechCommerceModifier(NO_TECH, COMMERCE_CULTURE) > 0)
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_DEFENSE) != 0 )
	{
		bool bSAD = false;
		bool bSIN = false;
		bool bZOC = false;

		if (GC.getGameINLINE().isOption(GAMEOPTION_SAD))
		{
			bSAD = (kBuilding.getLocalDynamicDefense() > 0);
		}

		if (GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
		{
			bSIN =  ((kBuilding.getFrontSupportPercentModifier() > 0) || 
			(kBuilding.getShortRangeSupportPercentModifier() > 0) || 
			(kBuilding.getMediumRangeSupportPercentModifier() > 0) || 
			(kBuilding.getLongRangeSupportPercentModifier() > 0) || 
			(kBuilding.getFlankSupportPercentModifier() > 0)) ;
		}

		if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ZOC))
		{
			bZOC = kBuilding.isZoneOfControl();
		}

		if ( kBuilding.getDefenseModifier() > 0 ||
			 kBuilding.getBombardDefenseModifier() > 0 ||
			 kBuilding.getAllCityDefenseModifier() >  0 ||
			 kBuilding.isNeverCapture() ||
			 kBuilding.isNukeImmune() ||
			 bZOC ||
			 kBuilding.getLineOfSight() > 0 ||
			 kBuilding.getUnitCombatExtraStrength(NO_UNITCOMBAT) > 0 ||
			 kBuilding.getAdjacentDamagePercent() > 0 ||
			 kBuilding.isProtectedCulture() ||
			 kBuilding.getOccupationTimeModifier() > 0 ||
			 kBuilding.getNoEntryDefenseLevel() > 0 ||
			 kBuilding.getNumUnitFullHeal() > 0 ||
			 kBuilding.isAreaBorderObstacle() ||
			 bSAD ||
			 bSIN ||
			 kBuilding.getLocalCaptureProbabilityModifier() > 0 ||
			 kBuilding.getLocalCaptureResistanceModifier() > 0 ||
			 kBuilding.getNationalCaptureResistanceModifier() > 0 ||
			 kBuilding.getNumUnitCombatRepelModifiers() > 0 ||
			 kBuilding.getRiverDefensePenalty() < 0 || 
			 kBuilding.getLocalRepel() > 0 || 
			 kBuilding.getMinDefense() > 0 || 
			 kBuilding.getBuildingDefenseRecoverySpeedModifier() > 0 || 
			 kBuilding.getCityDefenseRecoverySpeedModifier() > 0 ||
			 kBuilding.getNumUnitCombatRepelAgainstModifiers() > 0 ||
			 kBuilding.getNumUnitCombatDefenseAgainstModifiers() > 0 ||
			 (kBuilding.getDamageAttackerChance() > 0 && kBuilding.getDamageToAttacker() > 0))
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_HAPPY) != 0 )
	{
		bool bTechHapp = false;
		if (kBuilding.getNumTechHappinessTypes() > 0)
		{
			for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)iI))
				{
					if (kBuilding.getTechHappinessType(iI) > 0)
					{
						bTechHapp = true;
					}
				}
			}
		}
		if ( kBuilding.getHappiness() > 0 ||
			 kBuilding.getAreaHappiness() > 0 ||
			 kBuilding.getGlobalHappiness() > 0 ||
			 kBuilding.getStateReligionHappiness() > 0 ||
			 kBuilding.isNoUnhappiness() ||
			 kBuilding.getWarWearinessModifier() < 0 ||
			 kBuilding.getGlobalWarWearinessModifier() < 0 ||
			 kBuilding.getCommerceHappiness(NO_COMMERCE) > 0 ||
			 kBuilding.getBonusHappinessChanges(NO_BONUS) > 0 ||
			 kBuilding.getBuildingHappinessChanges(NO_BUILDING) > 0 ||
			 kBuilding.getTechHappinessChanges(NO_TECH) > 0 ||
			 GET_PLAYER(getOwnerINLINE()).getExtraBuildingHappiness(eBuilding) > 0 ||
			 bTechHapp
			 )
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_HEALTHY) != 0 )
	{
		bool bTechHealth = false;
		if (kBuilding.getNumTechHealthTypes() > 0)
		{
			for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)iI))
				{
					if (kBuilding.getTechHealthType(iI) > 0)
					{
						bTechHealth = true;
					}
				}
			}
		}
		if ( kBuilding.getHealth() > 0 ||
			 kBuilding.getAreaHealth() > 0 ||
			 kBuilding.getGlobalHealth() > 0 ||
			 kBuilding.isNoUnhealthyPopulation() ||
			 kBuilding.isBuildingOnlyHealthy() ||
			 kBuilding.getBonusHealthChanges(NO_BONUS) > 0 ||
			 kBuilding.getTechHealthChanges(NO_TECH) > 0 ||
			 kBuilding.getHealthPercentPerPopulation() > 0 ||
			 GET_PLAYER(getOwnerINLINE()).getExtraBuildingHealth(eBuilding) > 0 ||
			 bTechHealth
			 )
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_DOMAINSEA) != 0 )
	{
		iFocusFlags |= BUILDINGFOCUS_EXPERIENCE;

		if ( kBuilding.getDomainProductionModifier(DOMAIN_SEA) > 0 ||
			 kBuilding.getDomainFreeExperience(DOMAIN_SEA) > 0)
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_EXPERIENCE) != 0 )
	{
		if ( kBuilding.getFreeExperience() > 0 ||
			 kBuilding.getGlobalFreeExperience() > 0 ||
			 kBuilding.getFreePromotion() != NO_PROMOTION ||
			 kBuilding.getUnitCombatFreeExperience(NO_UNITCOMBAT) > 0 ||
			 kBuilding.getDomainFreeExperience(NO_DOMAIN) > 0 ||
			 kBuilding.getFreePromotion_2() != NO_PROMOTION ||
			 kBuilding.getFreePromotion_3() != NO_PROMOTION ||
			 kBuilding.isApplyFreePromotionOnMove() ||
			 kBuilding.EnablesUnits() ||
			 kBuilding.getNumFreePromoTypes() > 0 ||
			 kBuilding.getNumUnitCombatRetrainTypes() > 0 ||
			 kBuilding.getNationalCaptureProbabilityModifier() > 0 ||
			 kBuilding.getNumUnitCombatOngoingTrainingDurations() > 0 ||
			 kBuilding.isAnyUnitCombatFreeExperience() ||
			 kBuilding.isAnyDomainFreeExperience())
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_MAINTENANCE) != 0 )
	{
		if ( kBuilding.getMaintenanceModifier() < 0 ||
			 kBuilding.getGlobalMaintenanceModifier() < 0 ||
			 kBuilding.getAreaMaintenanceModifier() < 0 ||
			 kBuilding.getOtherAreaMaintenanceModifier() < 0 ||
			 kBuilding.getDistanceMaintenanceModifier() < 0 ||
			 kBuilding.getNumCitiesMaintenanceModifier() < 0 ||
			 kBuilding.getCoastalDistanceMaintenanceModifier() < 0 ||
			 kBuilding.getConnectedCityMaintenanceModifier() )
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_SPECIALIST) != 0 )
	{
		if ( kBuilding.getSpecialistCount(NO_SPECIALIST) > 0 ||
			 kBuilding.getFreeSpecialistCount(NO_SPECIALIST) > 0 ||
			 kBuilding.getImprovementFreeSpecialist(NO_IMPROVEMENT) > 0 ||
			 kBuilding.getTechSpecialistChange(NO_TECH, NO_SPECIALIST) > 0)
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_ESPIONAGE) != 0 )
	{
		if ( kBuilding.getEspionageDefenseModifier() > 0 )
		{
			return true;
		}
		if ( buildingModifiesCommerceYields ||
			 buildingHasTradeRouteValue(eBuilding) ||
			 kBuilding.getCommerceChange(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getCommercePerPopChange(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getCommerceModifier(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getGlobalCommerceModifier(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getSpecialistExtraCommerce(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getStateReligionCommerce(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getObsoleteSafeCommerceChange(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getCommerceAttacks(COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getTechCommerceChange(NO_TECH, COMMERCE_ESPIONAGE) > 0 ||
			 kBuilding.getTechCommerceModifier(NO_TECH, COMMERCE_ESPIONAGE) > 0)
		{
			return true;
		}
	}
	if ( (iFocusFlags & BUILDINGFOCUS_CAPITAL) != 0 )
	{
		if ( kBuilding.isCapital() )
		{
			return true;
		}
	}

	return false;
}

int	CvCityAI::GetBuildingValue(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bMaximizeFlaggedValue, bool bIgnoreCanConstruct)
{
	int iResult = -1;

	//	We only pre-calculate and cache values for buildings this city can build
	//	Only trigger a change in cached city from iFocusFlags != 0 which happens at the start
	//	of building search to choose construction.  Other requests (with 0 flags) can occur
	//	occassionally from other areas (such as civic evaluation) without implying a lot of locality
	//	of reference that makes the caching worthwhile
	if ( (bIgnoreCanConstruct || canConstruct(eBuilding)) && (cachedBuildingValues != NULL || iFocusFlags != 0) )
	{
		if ( cachedBuildingValues == NULL )
		{
			{
				MEMORY_TRACK_EXEMPT();

				OutputDebugString(CvString::format("Rebuilding building value cache for City %S\n", getName().GetCString()).c_str());
				cachedBuildingValues = new BuildingValueCache(this);
			}
		}

		if ( iFocusFlags == 0 )
		{
			iFocusFlags = BUILDINGFOCUS_CONSIDER_ANY;
		}
		if ( (cachedBuildingValues->m_iCachedFlags & iFocusFlags) != iFocusFlags )
		{
			CalculateAllBuildingValues( ~cachedBuildingValues->m_iCachedFlags & iFocusFlags );
		}

retry:
		iResult = cachedBuildingValues->GetValue(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue);

		if ( iResult == -1 )
		{
			if ( cachedBuildingValues->m_bIncomplete )
			{
				int iCachedFlags = cachedBuildingValues->m_iCachedFlags;
				cachedBuildingValues->m_iCachedFlags = 0;
				
				CalculateAllBuildingValues( iCachedFlags );

				cachedBuildingValues->m_bIncomplete = false;
				goto retry;
			}
			else if ( bIgnoreCanConstruct )
			{
				iResult = AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue);
			}
			else
			{
				iResult = 0;
				FAssert(AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue) == 0);
			}
		}
		else
		{
			if (!isHuman())
			{
				if (iResult > 0)
				{
					CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
					CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

					for (int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
					{
						iResult += (kOwner.AI_getFlavorValue((FlavorTypes)iI) * kBuilding.getFlavorValue(iI));
					}
				}
			}

#ifdef VALIDATE_BUILDING_CACHE_CONSISTENCY
			int realResult = AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, iThreshold);
			if ( realResult != iResult )
			{
				FAssertMsg(false,"Cached building value result mismatch!!\n");
			}
#endif
		}
	}
	else
	{
		PROFILE("GetBuildingValue.CacheMiss");

		iResult = AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, iThreshold);
	}

	return iResult;
}

void CvCityAI::AI_FlushBuildingValueCache(bool bRetainValues)
{
	PROFILE_FUNC();

	if ( !bRetainValues )
	{
		OutputDebugString("Flush building cache\n");

		SAFE_DELETE(cachedBuildingValues);
	}
	else if ( cachedBuildingValues != NULL )
	{
		cachedBuildingValues->m_bIncomplete = true;
	}
}

void CvCityAI::CalculateAllBuildingValues(int iFocusFlags)
{
	PROFILE_FUNC()

	//	KOSHLING optimisation - moved what we could outside of the building loop
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	ReligionTypes eStateReligion = kOwner.getStateReligion();
	bool bAreaAlone = kOwner.AI_isAreaAlone(area());
	int iHasMetCount = GET_TEAM(getTeam()).getHasMetCivCount(true);

	int iFoodDifference = foodDifference(false);

	// Reduce reaction to espionage induced happy/health problems
	int iHappinessLevel = happyLevel() - unhappyLevel(1) + getEspionageHappinessCounter()/2;
	int iAngryPopulation = range(-iHappinessLevel, 0, (getPopulation() + 1));
	int iHealthLevel = goodHealth() - badHealth(/*bNoAngry*/ false, std::max(0, (iHappinessLevel + 1) / 2)) + getEspionageHealthCounter()/2;
	int iBadHealth = std::max(0, -iHealthLevel);

	int iHappyModifier = (iHappinessLevel <= iHealthLevel && iHappinessLevel <= 6) ? 6 : 3;
	int iHealthModifier = (iHealthLevel < iHappinessLevel && iHealthLevel <= 4) ? 4 : 2;
	if (iHappinessLevel >= 10)
	{
		iHappyModifier = 1;
	}
	if (iHealthLevel >= 8)
	{
		iHealthModifier = 0;
	}

	int iBaseHappinessLevel = happyLevel() - unhappyLevel() + getEspionageHappinessCounter();
	int iBaseHealthLevel = goodHealth() - badHealth() + getEspionageHealthCounter();

	int iPopulationExponent = (getPopulation() - 1); // Each pop past the first increases consumption per population by .1, rounded down.  Each point of population means more actual people the higher the amount goes.
	int iConsumptionPerPopulationBase = iPopulationExponent + (GC.getFOOD_CONSUMPTION_PER_POPULATION() * 10);
	int iConsumptionbyPopulation = (getPopulation() * iConsumptionPerPopulationBase) / 10;

	int iBaseFoodDifference = getYieldRate(YIELD_FOOD) - iConsumptionbyPopulation - std::max(0, -iHealthLevel);

	int iGoldValueAssessmentModifier = GET_PLAYER(getOwnerINLINE()).AI_goldValueAssessmentModifier();

	//Temporary unhappiness that will mostly not be ignored
	int iTemporaryUnhappiness = 0;
	int iAngerPercent = 0;
	iAngerPercent += getHurryPercentAnger();
	iAngerPercent += getConscriptPercentAnger();
	iAngerPercent += getDefyResolutionPercentAnger();
	iAngerPercent += getWarWearinessPercentAnger();
	iTemporaryUnhappiness += ((iAngerPercent * (getPopulation())) / GC.getPERCENT_ANGER_DIVISOR());

	int iTotalPopulation = kOwner.getTotalPopulation();
	int iNumCities = kOwner.getNumCities();
	int iNumCitiesInArea = area()->getCitiesPerPlayer(getOwnerINLINE());
	
	int aiYieldRank[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYieldRank[iI] = MAX_INT;
	}

	int aiCommerceRank[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerceRank[iI] = MAX_INT;
	}

	int aiBaseCommerceRate[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiBaseCommerceRate[iI] = MAX_INT;
	}

	int aiPlayerCommerceRate[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiPlayerCommerceRate[iI] = kOwner.getTotalCityBaseCommerceRate((CommerceTypes)iI);
	}

	aiYieldRank[YIELD_PRODUCTION] = findBaseYieldRateRank(YIELD_PRODUCTION);
	bool bIsHighProductionCity = (aiYieldRank[YIELD_PRODUCTION] <= std::max(3, (iNumCities / 2)));
	
	int iCultureRank = findCommerceRateRank(COMMERCE_CULTURE);
	aiCommerceRank[COMMERCE_CULTURE] = iCultureRank;

	int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();

	bool bFinancialTrouble = GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble();

	int iAllowedShrinkRate = (iConsumptionPerPopulationBase * (0
		+ std::max(0, -iBaseHappinessLevel - iTemporaryUnhappiness)
		+ std::min(1, std::max(0, (getWorkingPopulation() - AI_countGoodTiles(true, false, 50))))
		+ std::max(0, (visiblePopulation() - AI_countGoodSpecialists(false))))) / 10;

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      03/08/10                                jdog5000      */
	/*                                                                                              */
	/* Victory Strategy AI                                                                          */
	/************************************************************************************************/
	bool bCulturalVictory1 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1);
	bool bCulturalVictory2 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	bool bCulturalVictory3 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);

	bool bSpaceVictory1 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE1);
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/	
	std::vector<bool> cityHasVicinityBonus;

	for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
	{
		cityHasVicinityBonus.push_back( hasBonus((BonusTypes)iJ) && (hasVicinityBonus((BonusTypes)iJ)||hasRawVicinityBonus((BonusTypes)iJ)));
	}

	logBBAI("      City %S CalculateAllBuildingValues for flags %08lx (already has %08lx)", getName().GetCString(), iFocusFlags, cachedBuildingValues->m_iCachedFlags);

	//	Either wonder flag forces all wonders to be included in what gets calculated so we
	//	can mark them both off as done
	if ( (iFocusFlags & (BUILDINGFOCUS_WORLDWONDER | BUILDINGFOCUS_WONDEROK)) != 0 )
	{
		iFocusFlags |= (BUILDINGFOCUS_WORLDWONDER | BUILDINGFOCUS_WONDEROK);
	}

	cachedBuildingValues->m_iCachedFlags |= iFocusFlags;

	std::set<BuildingTypes> buildingsToCalculate;
	int numNumBuildingClassInfos = GC.getNumBuildingClassInfos();
	
	{
		PROFILE("CvCityAI::CalculateAllBuildingValues.PreLoop");
		for (int iBuilding = 0; iBuilding < numNumBuildingClassInfos; iBuilding++)
		{
			BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iBuilding)));

			if (NO_BUILDING != eBuilding)
			{
				if ( !canConstruct(eBuilding) )
				{
					continue;
				}

				buildingsToCalculate.insert(eBuilding);

				for (int iJ = 0; iJ < numNumBuildingClassInfos; iJ++)
				{
					BuildingTypes eJLoopBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes)iJ).getDefaultBuildingIndex()));
					if (eJLoopBuilding != NO_BUILDING && buildingsToCalculate.find(eJLoopBuilding) == buildingsToCalculate.end())
					{
						if (GC.getBuildingInfo(eJLoopBuilding).isReplaceBuildingClass(iBuilding))
						{
							if (getNumBuilding(eJLoopBuilding) > 0)
							{
								buildingsToCalculate.insert(eJLoopBuilding);
							}
						}
					}
				}

				if ( GC.getBuildingInfo(eBuilding).EnablesOtherBuildings() )
				{
					for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
					{
						if (buildingsToCalculate.find((BuildingTypes)iJ) == buildingsToCalculate.end() &&
							(GC.getBuildingInfo((BuildingTypes)iJ).isBuildingClassNeededInCity(iBuilding) ||
							 GC.getBuildingInfo((BuildingTypes)iJ).isPrereqOrBuildingClass(iBuilding)))
						{
							if ( getNumBuilding((BuildingTypes)iJ) == 0 &&
								 canConstructInternal((BuildingTypes)iJ, false, false, false, true, (BuildingClassTypes)iBuilding) )
							{
								buildingsToCalculate.insert((BuildingTypes)iJ);
							}
						}
					}
				}
			}
		}
	}

	{
		PROFILE("CvCityAI::CalculateAllBuildingValues.Loop");
		//	Calculate all possible (by focus) values for each building type
		for(std::set<BuildingTypes>::const_iterator itr = buildingsToCalculate.begin(); itr != buildingsToCalculate.end(); ++itr )
		{
			BuildingTypes eBuilding = *itr;
	#if 0
		for (iBuilding = 0; iBuilding < GC.getNumBuildingClassInfos(); iBuilding++)
		{
			BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iBuilding)));
	#endif
			if (NO_BUILDING != eBuilding)
			{
				if ( cachedBuildingValues->HasValues(eBuilding) || !buildingMayHaveAnyValue(eBuilding, iFocusFlags))
				{
					continue;
				}

				OneBuildingValueCache* valuesCache = cachedBuildingValues->GetOneBuildingCache(eBuilding);

				//	What follows is essentially a copy of the code in AI_buildingValueThreshold with the
				//	modification that it accumulates all focus values to the cache's focus value array

				PROFILE("CvCityAI::CalculateAllBuildingValues.building");

				CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
				BuildingClassTypes eBuildingClass = (BuildingClassTypes) kBuilding.getBuildingClassType();
				int iLimitedWonderLimit = limitedWonderClassLimit(eBuildingClass);
				bool bIsLimitedWonder = (iLimitedWonderLimit >= 0);

				logBBAI("          Calc value for %S", kBuilding.getDescription());
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD                      02/24/10                        jdog5000 & Fuyu       */
				/*                                                                                              */
				/* City AI, Bugfix                                                                              */
				/************************************************************************************************/

				bool bProvidesPower = (kBuilding.isPower() || ((kBuilding.getPowerBonus() != NO_BONUS) && hasBonus((BonusTypes)(kBuilding.getPowerBonus()))) || kBuilding.isAreaCleanPower());
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD                       END                                                  */
				/************************************************************************************************/

				/********************************************************************************/
				/* 	Alternative Building Evaluation				20.02.2010		Fuyu		    */
				/* 	 - requires Actual Building Effects (BULL)								    */
				/* 																			    */
				/* 	TODO:																		*/
				/* 		value riverPlotYieldChanges higher when we are already working on river	*/
				/* 		add getBuildingYieldChange() ?										    */
				/* 		more consistancy (values from yield changes should not differ so much	*/
				/* 			depending on where they come from)									*/
				/********************************************************************************/
				//Don't consider a building if it causes the city to immediately start shrinking from unhealthiness
				//For that purpose ignore bad health and unhappiness from Espionage.
				int iBuildingActualHappiness = getAdditionalHappinessByBuilding(eBuilding);
				int iBuildingActualHealth = getAdditionalHealthByBuilding(eBuilding);

				int iBadHealthFromBuilding = std::max(0,(-iBuildingActualHealth));
				int iUnhealthyPopulationFromBuilding = std::min(0,(-iBaseHealthLevel)) + iBadHealthFromBuilding;
				//	int iTotalHealth = iBaseHealthLevel + iBuildingActualHealth;
				bool bShrinksWithPower = false;
				//Allow a bit of shrinking: Population is expendable if angry, working a bad tile, or running a not-so-good specialist
				if (iUnhealthyPopulationFromBuilding > 0 && (iBaseFoodDifference + iAllowedShrinkRate < iUnhealthyPopulationFromBuilding ))
				{
					continue;
				}
				else if (!(bProvidesPower || isPower()))  //if the city is still without power after building this
				{
					int iUnhealthyPopulationFromBuildingPlusPower = std::min(0,(badHealth() - goodHealth() - getEspionageHealthCounter())) + iBadHealthFromBuilding - GC.getDefineINT("DIRTY_POWER_HEALTH_CHANGE");
					if (iUnhealthyPopulationFromBuildingPlusPower > 0 && (iBaseFoodDifference + iAllowedShrinkRate < iUnhealthyPopulationFromBuildingPlusPower ))
					{
						bShrinksWithPower = true;
					}
				}
				/********************************************************************************/
				/* 	ABE END																	    */
				/********************************************************************************/

				/************************************************************************************************/
				/* Afforess	                  Start		 07/28/10                                               */
				/*                                                                                              */
				/*                                                                                              */
				/************************************************************************************************/
				int iSpecialistGreatPeopleRate = 0;
				int iSpecialistExtraHealth = 0;
				int iSpecialistExtraHappy = 0;
				
				
				int* paiFreeSpecialistCommerce = new int[NUM_COMMERCE_TYPES];
				int* paiFreeSpecialistYield = new int[NUM_YIELD_TYPES];
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					paiFreeSpecialistYield[iI] = 0;
				}
				for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
				{
					paiFreeSpecialistCommerce[iI] = 0;
				}

				if (kBuilding.getNumPopulationEmployed() > 0)
				{
					removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iSpecialistGreatPeopleRate, iSpecialistExtraHappy, iSpecialistExtraHealth, paiFreeSpecialistYield, paiFreeSpecialistCommerce);
				}
				
				int aiFreeSpecialistYield[NUM_YIELD_TYPES];
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					aiFreeSpecialistYield[iI] = paiFreeSpecialistYield[iI];
				}

				int aiFreeSpecialistCommerce[NUM_COMMERCE_TYPES];
				for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
				{
					aiFreeSpecialistCommerce[iI] = paiFreeSpecialistCommerce[iI];
				}
				
				SAFE_DELETE_ARRAY(paiFreeSpecialistCommerce);
				SAFE_DELETE_ARRAY(paiFreeSpecialistYield);
				
				for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
				{
					SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
					if (eNewSpecialist == NO_SPECIALIST) break;
					CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eNewSpecialist);
					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						aiFreeSpecialistYield[iJ] += GET_PLAYER(getOwnerINLINE()).specialistYield(eNewSpecialist, (YieldTypes)iJ);
					}
					for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
					{
						aiFreeSpecialistCommerce[iJ] += GET_PLAYER(getOwnerINLINE()).specialistCommerce(eNewSpecialist, (CommerceTypes)iJ);
					}
					iSpecialistGreatPeopleRate += kSpecialist.getGreatPeopleRateChange();
					iSpecialistExtraHealth += kSpecialist.getHealthPercent();
					iSpecialistExtraHappy += kSpecialist.getHappinessPercent();
				}
				iSpecialistExtraHappy /= 100;
				iSpecialistExtraHealth /= 100;
				iBuildingActualHappiness += iSpecialistExtraHappy;
				iBuildingActualHealth += iSpecialistExtraHealth;
				/************************************************************************************************/
				/* Afforess	                     END                                                            */
				/************************************************************************************************/
				bool bCanPopRush = GET_PLAYER(getOwnerINLINE()).canPopRush();

				bool bForeignTrade = false;
				int iNumTradeRoutes = getTradeRoutes();
				for (int iI = 0; iI < iNumTradeRoutes; ++iI)
				{
					CvCity* pTradeCity = getTradeCity(iI);
					if (NULL != pTradeCity)
					{
						if (GET_PLAYER(pTradeCity->getOwnerINLINE()).getTeam() != getTeam() || pTradeCity->area() != area())
						{
							bForeignTrade = true;
							break;
						}
					}
				}

				if (kBuilding.isCapital())
				{
					continue;
				}
				
				/************************************************************************************************/
				/* Afforess	                  Start		 01/24/10                                               */
				/*                                                                                              */
				/* If the AI can build the replacement building...                                              */
				/************************************************************************************************/
				bool bSkipThisBuilding = false;

				for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
				{
					BuildingTypes eLoopBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI);
					if (kBuilding.isReplaceBuildingClass((BuildingClassTypes)iI))
					{
						if (eLoopBuilding != NO_BUILDING)
						{
							if(canConstruct(eLoopBuilding))
							{
								bSkipThisBuilding = true;
							}
						}
					}
				}

				if ( bSkipThisBuilding )
				{
					continue;
				}
				/************************************************************************************************/
				/* Afforess	                     END                                                            */
				/************************************************************************************************/

				for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
				{
					if (kBuilding.getReligionChange(iI) > 0)
					{
						if (!(GET_TEAM(getTeam()).hasHolyCity((ReligionTypes)iI)))
						{
							bSkipThisBuilding = true;
						}
					}
				}

				if ( bSkipThisBuilding )
				{
					continue;
				}

				//int iValue = 0;

				//	KOSHLING - optimisation - no need to loop round this twice repeating stuff
				//	since we are now accumulating the two passes to separate counters anyway
				//int iPass;
				//for (iPass = 0; iPass < 2; iPass++)
				{
					// (iValue > 0) || (iPass == 0) always evaluates TRUE
					//if ((iFocusFlags == 0) || (iValue > 0) || (iPass == 0))
					{
						//if ((iFocusFlags & BUILDINGFOCUS_WORLDWONDER) || (iPass > 0))
						{
							if (isWorldWonderClass(eBuildingClass))
							{
								if (aiYieldRank[YIELD_PRODUCTION] <= 3)
								{
									valuesCache->Accumulate(BUILDINGFOCUSINDEX_WORLDWONDER,1);
									//iValue++;
								}
							}
						}
						
						//if ((iFocusFlags & BUILDINGFOCUS_DEFENSE) || (iPass > 0))
						{
							PROFILE("CalculateAllBuildingValues.Defense");
							int iValue = 0;

							if (!bAreaAlone)
							{
								if ((GC.getGameINLINE().getBestLandUnit() == NO_UNIT) || !(GC.getUnitInfo(GC.getGameINLINE().getBestLandUnit()).isIgnoreBuildingDefense()))
								{
									iValue += (std::max(0, std::min(((kBuilding.getDefenseModifier() + getBuildingDefense()) - getNaturalDefense() - 10), kBuilding.getDefenseModifier())) / 4);

			/************************************************************************************************/
			/* Afforess	                  Start		 06/15/10                                               */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
									for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
									{
										if (hasBonus((BonusTypes)iI))
										{
											iValue += (kBuilding.getBonusDefenseChanges(iI) / 4);
										}
									}
								}
								
							
								if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ZOC))
								{
									iValue += kBuilding.isZoneOfControl() ? 50 : 0;
								}
									
								iValue += kBuilding.getLineOfSight() * 15;

								for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
								{
									if (kBuilding.getUnitCombatExtraStrength(iI) != 0)
									{
										bool bValidUnit = false;
										int iValidUnitCount = 0;
										CvPlot* pPlot = plot();
										for (CvPlot::unit_iterator unitItr = pPlot->beginUnits(); unitItr != pPlot->endUnits(); ++unitItr)
										{
											CvUnit* pLoopUnit = unitItr.ptr();
											if (pLoopUnit->getTeam() == getTeam())
											{
												if (pLoopUnit->getUnitCombatType() == iI)
												{
													bValidUnit = true;
												}
												else
												{
													//TB SubCombat Mod Begin(may cause some unexpected imbalance though it could also imbalance to bypass... a place to watch)
													for (int iJ = 0; iJ < pLoopUnit->getUnitInfo().getNumSubCombatTypes(); iJ++)
													{
														if (pLoopUnit->getUnitInfo().getSubCombatType(iJ) == iI)
														{
															bValidUnit = true;
															break;
														}
													}
												}
												if (bValidUnit)
												{
													iValidUnitCount++;
													bValidUnit = false;
												}
												//TB SubCombat Mod End
											}
										}
										iValue += iValidUnitCount * kBuilding.getUnitCombatExtraStrength(iI) / 6;
									}
								}
								
								bool bDefense = (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
								bool bLandWar = (bDefense || (area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
								bool bDanger = AI_isDanger();

								if (bDanger || bLandWar)
								{
									iValue += kBuilding.getAdjacentDamagePercent() * 10;
								}
								
								iValue -= kBuilding.getInvasionChance() * 5;
								
								iValue += kBuilding.isProtectedCulture() ? 50 : 0;
								iValue += kBuilding.getOccupationTimeModifier() / 20;
								
								if (kBuilding.getNoEntryDefenseLevel() > 0 && kBuilding.getNoEntryDefenseLevel() < getTotalDefense(false))
								{
									iValue += getTotalDefense(false) - kBuilding.getNoEntryDefenseLevel() / 2;
								}
								if (kBuilding.getNumUnitFullHeal() > 0)
								{
									iValue += kBuilding.getNumUnitFullHeal() * 50;
								}
								
								iValue += kBuilding.getBombardDefenseModifier() / 4;

								if (GC.getGameINLINE().isOption(GAMEOPTION_SAD))
								{
									iValue += kBuilding.getLocalDynamicDefense() / 2;
								}
								if (GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
								{
									iValue += kBuilding.getFrontSupportPercentModifier() / 4;
									iValue += kBuilding.getShortRangeSupportPercentModifier() / 4;
									iValue += kBuilding.getMediumRangeSupportPercentModifier() / 4;
									iValue += kBuilding.getLongRangeSupportPercentModifier() / 4;
									iValue += kBuilding.getFlankSupportPercentModifier() / 4;
								}

								iValue += kBuilding.getLocalCaptureProbabilityModifier() / 6;
								iValue += kBuilding.getLocalCaptureResistanceModifier() / 3;
								iValue -= kBuilding.getRiverDefensePenalty() / 2;
								iValue += kBuilding.getLocalRepel();
								iValue += kBuilding.getMinDefense();
								iValue += kBuilding.getBuildingDefenseRecoverySpeedModifier() / 20;
								iValue += kBuilding.getCityDefenseRecoverySpeedModifier() / 5;
							}

							iValue += kBuilding.getNationalCaptureResistanceModifier();

							//TB Note: Once we have improved methods of evaluating the value of a given combat class and an ability to evaluate the strength
							//of a given combat class as it exists in volume among perceived or real enemies, we need to update the following to something
							//more intricate and accurate.  These are just semi-sufficient patches 'for now'.
							for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
							{
								iValue += kBuilding.getUnitCombatRepelModifier(iI) / 2;
								iValue += kBuilding.getUnitCombatRepelAgainstModifier(iI) / 2;
								iValue += kBuilding.getUnitCombatDefenseAgainstModifier(iI) / 2;
								if (kBuilding.isMayDamageAttackingUnitCombatType(iI))
								{
									iValue += (kBuilding.getDamageAttackerChance()*kBuilding.getDamageToAttacker())/4;
								}
							}
							if (kBuilding.isDamageAllAttackers())
							{
								iValue += (kBuilding.getDamageAttackerChance()*kBuilding.getDamageToAttacker());
							}
							iValue -= kBuilding.getRiverDefensePenalty() / 2;				
							
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/
							
							iValue += -kBuilding.getAirModifier() / 4;
							iValue += -kBuilding.getNukeModifier() / 4;
							
							iValue += ((kBuilding.getAllCityDefenseModifier() * iNumCities) / 5);
							
							iValue += kBuilding.getAirlift() * 25;
			/************************************************************************************************/
			/* Afforess	                  Start		 03/31/10                                               */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
							if (kBuilding.isAreaBorderObstacle())
							{
								//The great wall is much more valuable with more barbarian activity.
								int areaBorderObstacleValue = (iNumCitiesInArea);
								if(GC.getGameINLINE().isOption(GAMEOPTION_RAGING_BARBARIANS))
									areaBorderObstacleValue *=2;
								if (GC.getGameINLINE().isOption(GAMEOPTION_BARBARIAN_WORLD))
									areaBorderObstacleValue *=2;
								if (GC.getGameINLINE().isOption(GAMEOPTION_BARBARIAN_GENERALS))
									areaBorderObstacleValue *=2;
								if (GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS))
									areaBorderObstacleValue = 0;
								iValue += areaBorderObstacleValue;
							}

							valuesCache->Accumulate(BUILDINGFOCUSINDEX_DEFENSE,
													iValue);
						}
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/
						//if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) || (iPass > 0))
						{
							//iValue += kBuilding.getEspionageDefenseModifier() / 8;
							valuesCache->Accumulate(BUILDINGFOCUSINDEX_ESPIONAGE,
													kBuilding.getEspionageDefenseModifier() / 8);
						}

						//if (((iFocusFlags & BUILDINGFOCUS_HAPPY) || (iPass > 0)) && !isNoUnhappiness())
						if (!isNoUnhappiness())
						{
							PROFILE("CalculateAllBuildingValues.Happy");
							int iBestHappy = 0;
							int iValue = 0;

							for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
							{
								if (canHurryBuilding((HurryTypes)iI, eBuilding, true))
								{
									int iHappyFromHurry = AI_getHappyFromHurry((HurryTypes)iI, eBuilding, true);
									if (iHappyFromHurry > iBestHappy)
									{
										iBestHappy = iHappyFromHurry;
									}
								}
							}
							iValue += iBestHappy * 10;

			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      02/24/10                              jdog5000        */
			/*                                                                                              */
			/* City AI                                                                                      */
			/************************************************************************************************/

			/********************************************************************************/
			/* 	Alternative Building Evaluation				09.03.2010		Fuyu		    */
			/********************************************************************************/
			/* original Better AI code
							if( iBuildingActualHappiness < 0 )
							{
								// Building causes net decrease in city happiness
								iValue -= (-iBuildingActualHappiness + iAngryPopulation) * 6
									+ (-iBuildingActualHappiness) * iHappyModifier;

								// BBAI TODO: Check for potential shrink in population
								
							}
							else if( iBuildingActualHappiness > 0 )
							{
								// Building causes net increase in city happiness
								iValue += (std::min(iBuildingActualHappiness, iAngryPopulation) * 10) 
									+ (std::max(0, iBuildingActualHappiness - iAngryPopulation) * iHappyModifier);
							}
			*/

							//Fuyu ToDo: How to handle Globe Theater national wonder?
							//For now just give massive boost if city is high food yet not one of the main production or commerce cities
							if (kBuilding.isNoUnhappiness() && bIsLimitedWonder)
							{
								iValue += (iAngryPopulation * 10) + getPopulation();
								aiYieldRank[YIELD_FOOD] = findBaseYieldRateRank(YIELD_FOOD);
								aiYieldRank[YIELD_COMMERCE] = findBaseYieldRateRank(YIELD_COMMERCE);
								if (aiYieldRank[YIELD_FOOD] <= (2 + iLimitedWonderLimit))
								{
									if (!(isNationalWonderClass(eBuildingClass)) ||
										((aiYieldRank[YIELD_PRODUCTION] > (2 + iLimitedWonderLimit)) 
										&& (aiYieldRank[YIELD_COMMERCE] > (2 + iLimitedWonderLimit))))
									{
										iValue += getPopulation() * 5;
									}
								}
							}
							else
							{
								iValue += happynessValue(iBuildingActualHappiness, iBaseHappinessLevel, iBaseHealthLevel);
							}
			/********************************************************************************/
			/* 	ABE END																	    */
			/********************************************************************************/

							iValue += (-kBuilding.getHurryAngerModifier() * getHurryPercentAnger()) / 100;

							for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
							{
								iValue += (kBuilding.getCommerceHappiness(iI) * iHappyModifier) / 4;
							}

							int iWarWearinessModifer = kBuilding.getWarWearinessModifier();
							if (iWarWearinessModifer != 0)
							{
								if ( GET_TEAM(getTeam()).getAtWarCount(true) == 0 )
								{
									iWarWearinessModifer /= 2;
								}

								iValue += (-iWarWearinessModifer * iHappyModifier) / 16;
							}

							iValue += (kBuilding.getAreaHappiness() * (iNumCitiesInArea - 1) * 8);
							iValue += (kBuilding.getGlobalHappiness() * (iNumCities - 1) * 8);

							int iWarWearinessPercentAnger = kOwner.getWarWearinessPercentAnger();
							int iGlobalWarWearinessModifer = kBuilding.getGlobalWarWearinessModifier();
							if (iGlobalWarWearinessModifer != 0)
							{
								iValue += (-(((iGlobalWarWearinessModifer * iWarWearinessPercentAnger / 100) / GC.getPERCENT_ANGER_DIVISOR())) * (iNumCities - 1));
								iValue += (-iGlobalWarWearinessModifer * iHappyModifier) / 16;
							}

							CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());
							for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
							{
								int iBuildingHappinessChanges = kBuilding.getBuildingHappinessChanges(iI);
								if (iBuildingHappinessChanges != 0)
								{
									BuildingTypes eLoopBuilding = (BuildingTypes)kCivilization.getCivilizationBuildings(iI);
									if (eLoopBuilding != NO_BUILDING)
									{
										iValue += (iBuildingHappinessChanges * (kOwner.getBuildingClassCount((BuildingClassTypes)iI) - getNumBuilding(eLoopBuilding)) * 8);
									}
									else
									{
										iValue += (iBuildingHappinessChanges * kOwner.getBuildingClassCount((BuildingClassTypes)iI) * 8);
									}
								}
							}
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/

			/************************************************************************************************/
			/* Afforess	                  Start		 07/12/10                                               */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
							if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_REVOLUTION))
							{
								if (kBuilding.getRevIdxLocal() != 0)
								{
									int localRevIdx = getLocalRevIndex();

									//	Use the more serious of the before and after values if this building were to be built
									if ( kBuilding.getRevIdxLocal() > 0 )
									{
										localRevIdx += kBuilding.getRevIdxLocal();
									}

									//	Treat instability seriously as it goes up - not just linear
									int localRevScaling = (localRevIdx < 0 ? 0 : std::min(localRevIdx*localRevIdx/50 + localRevIdx/2, 100));

									iValue -= (kBuilding.getRevIdxLocal() * localRevScaling)/4;
								}
								if (kBuilding.getRevIdxNational() != 0)
								{
									iValue -= (8 * GET_PLAYER(getOwnerINLINE()).getNumCities()) * kBuilding.getRevIdxNational();
								}
								if (kBuilding.getRevIdxDistanceModifier() != 0)
								{
									if (!isCapital())
									{
										CvCity* pCapital = GET_PLAYER(getOwnerINLINE()).getCapitalCity();
										if (pCapital != NULL)
										{
											int iCapitalDistance = ::plotDistance(getX_INLINE(), getY_INLINE(), pCapital->getX_INLINE(), pCapital->getY_INLINE());
											int iOldCapitalDistance = iCapitalDistance;
											iCapitalDistance *= 100 + kBuilding.getRevIdxDistanceModifier();
											iCapitalDistance /= 100;
											
											iValue += ((iOldCapitalDistance - iCapitalDistance) * (10+std::max(0,getLocalRevIndex())));
										}
									}
								}
							}
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/


							valuesCache->Accumulate(BUILDINGFOCUSINDEX_HAPPY,
													iValue);
						}

						//if (((iFocusFlags & BUILDINGFOCUS_HEALTHY) || (iPass > 0)) && !isNoUnhealthyPopulation())
						if (!isNoUnhealthyPopulation())
						{
							PROFILE("CalculateAllBuildingValues.Health");
							int iValue = 0;
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      02/24/10                          jdog5000 & Fuyu     */
			/*                                                                                              */
			/* City AI                                                                                      */
			/************************************************************************************************/

			/********************************************************************************/
			/* 	Alternative Building Evaluation				09.03.2010		Fuyu		    */
			/********************************************************************************/
			/* original Better AI code
							if( iBuildingActualHealth < 0 )
							{
								// Building causes net decrease in city health
								iValue -= (-iBuildingActualHealth + iBadHealth) * 6
									+ (-iBuildingActualHealth) * iHealthModifier;

								// BBAI TODO: Check for potential shrink in population
								
							}
							else if( iBuildingActualHealth > 0 )
							{
								// Building causes net increase in city health
								iValue += (std::min(iBuildingActualHealth, iBadHealth) * 10)
									+ (std::max(0, iBuildingActualHealth - iBadHealth) * iHealthModifier);
							}
			*/

							iValue += healthValue(iBuildingActualHealth, iBaseHappinessLevel - (getEspionageHappinessCounter() / 2) + std::max(0, iBuildingActualHappiness), iBaseHealthLevel, iBaseFoodDifference);
			/********************************************************************************/
			/* 	ABE END																	    */
			/********************************************************************************/

							iValue += (kBuilding.getAreaHealth() * (iNumCitiesInArea - 1 ) * 4);
							iValue += (kBuilding.getGlobalHealth() * (iNumCities - 1) * 4);
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/
							valuesCache->Accumulate(BUILDINGFOCUSINDEX_HEALTHY,
													iValue);
						}

						//if ((iFocusFlags & BUILDINGFOCUS_EXPERIENCE) || (iPass > 0))
						{
							PROFILE("CalculateAllBuildingValues.Experience");
							int iValue = 0;
							iValue += (kBuilding.getFreeExperience() * ((iHasMetCount > 0) ? 12 : 6));

							for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
							{
								if (kBuilding.getUnitCombatFreeExperience((UnitCombatTypes)iI) != 0 && canTrain((UnitCombatTypes)iI))
								{
									iValue += (kBuilding.getUnitCombatFreeExperience(iI) * ((iHasMetCount > 0) ? 6 : 3));
								}

								if (kBuilding.isUnitCombatRetrainType((UnitCombatTypes)iI))
								{
									iValue += 20;
								}

								if (kBuilding.getNumUnitCombatOngoingTrainingDurations() > 0)
								{
									if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iI) > 0)
									{
										if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iI) < getUnitCombatOngoingTrainingTimeIncrement((UnitCombatTypes)iI))
										{
											iValue = (20 - kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iI)) * 10;
										}
									}
								}
							}

							for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
							{
								int iDomainExpValue = 0;
								if (iI == DOMAIN_SEA)
								{
									iDomainExpValue = 7;
								} 
								else if (iI == DOMAIN_LAND)
								{
									iDomainExpValue = 12;
								}
								else
								{
									iDomainExpValue = 6;
								}
								iValue += (kBuilding.getDomainFreeExperience(iI) * ((iHasMetCount > 0) ? iDomainExpValue : iDomainExpValue / 2));
							}
							/************************************************************************************************/
							/* Afforess	                  Start		 06/04/10                                               */
							/*                                                                                              */
							/*                                                                                              */
							/************************************************************************************************/
							int promotionValue = 0;
							bool bWarPlan = (GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0);
							if (kBuilding.getFreePromotion() != NO_PROMOTION)
							{
								promotionValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromotion());
							}
							if (kBuilding.getFreePromotion_2() != NO_PROMOTION)
							{
								promotionValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromotion_2());
							}
							if (kBuilding.getFreePromotion_3() != NO_PROMOTION)
							{
								promotionValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromotion_3());
							}
							for (int iI = 0; iI < kBuilding.getNumFreePromoTypes(); iI++)
							{
								if (kBuilding.getFreePromoType(iI).ePromotion)
								{
									if (kBuilding.getFreePromoType(iI).m_pExprFreePromotionCondition)
									{
										promotionValue += (AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromoType(iI).ePromotion)/2);
									}
									else
									{
										promotionValue += AI_getPromotionValue((PromotionTypes)kBuilding.getFreePromoType(iI).ePromotion);
									}
								}
							}
							for (int iI = 0; iI < kBuilding.getNumFreeTraitTypes(); iI++)
							{
								TraitTypes eTrait = kBuilding.getFreeTraitType(iI).eTrait;
								if (GC.getTraitInfo(eTrait).isCivilizationTrait())
								{
									if (!GC.getTraitInfo(eTrait).isNegativeTrait())
									{
										for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
										{
											promotionValue += (GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ));
										}
									}
									else
									{
										for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
										{
											promotionValue -= (GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ));
										}
									}
								}
							}
							if (kBuilding.isApplyFreePromotionOnMove())
							{
								promotionValue *= 2;
							}

							iValue += kBuilding.getNationalCaptureProbabilityModifier() * 2;
							if ((iFocusFlags & BUILDINGFOCUS_EXPERIENCE))
							{
								promotionValue *= 2;
							}
							iValue += promotionValue;

							int iInvestigation = kBuilding.getInvestigation();
							if (iInvestigation > 0 && (iFocusFlags & BUILDINGFOCUS_INVESTIGATION))
							{
								iValue += iInvestigation * 5;
							}
							else
							{
								iValue += iInvestigation;
							}

							int iPropValue = buildingPropertiesValue(kBuilding);
							if (iFocusFlags & BUILDINGFOCUS_PROPERTY)
							{
								if (iPropValue > 0)
								{
										iValue += iPropValue * 100;
								}
							}
							else
							{
								iValue += iPropValue;
							}


							bool bDevelopingCity = isDevelopingCity();
							if ((!bDevelopingCity || isCapital()) && kBuilding.EnablesUnits())
							{
								PROFILE("CalculateAllBuildingValues.NotDeveloping");

								for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
								{
									CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);
									bool bUnitIsEnabler = (kUnit.getPrereqBuilding() == eBuilding);
									bool bUnitIsOtherwiseEnabled = (bUnitIsEnabler ||
																	(BuildingTypes)kUnit.getPrereqBuilding() == NO_BUILDING ||
																	getNumBuilding((BuildingTypes)kUnit.getPrereqBuilding()) > 0) &&
																   GET_TEAM(getTeam()).isHasTech((TechTypes)kUnit.getPrereqAndTech());

									if ( bUnitIsOtherwiseEnabled )
									{
										bool bUnitIsBonusEnabled = true;
										if ( kUnit.getPrereqAndBonus() != NO_BONUS )
										{
											if ( isFreeBonusOfBuilding(kBuilding, (BonusTypes)kUnit.getPrereqAndBonus()) )
											{
												if ( !hasBonus((BonusTypes)kUnit.getPrereqAndBonus()) )
												{
													bUnitIsEnabler = true;
												}
											}
											else
											{
												if ( !hasBonus((BonusTypes)kUnit.getPrereqAndBonus()) )
												{
													bUnitIsBonusEnabled = false;
												}
											}
										}

										bool	bHasORBonusAlready = false;
										bool	bFreeBonusIsORBonus = false;
										int		iFreeExtraBonusCount = 0;

										for (int iK = 0; iK < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); iK++)
										{
											BonusTypes eXtraFreeBonus = (BonusTypes)kUnit.getPrereqOrBonuses(iK);

											if ( eXtraFreeBonus != NO_BONUS )
											{
												iFreeExtraBonusCount++;

												if ( hasBonus(eXtraFreeBonus) )
												{
													bHasORBonusAlready = true;
												}
												else if ( isFreeBonusOfBuilding(kBuilding, eXtraFreeBonus) )
												{
													bFreeBonusIsORBonus = true;
												}
											}
										}

										if ( iFreeExtraBonusCount > 0 && !bHasORBonusAlready )
										{
											if ( bFreeBonusIsORBonus )
											{
												bUnitIsEnabler = true;
											}
											else
											{
												bUnitIsBonusEnabled = false;
											}
										}


										if ( bUnitIsEnabler )
										{
											PROFILE("CalculateAllBuildingValues.IsUnitPrereq");

											if (kUnit.getDefaultUnitAIType() != NO_UNITAI && kOwner.AI_totalAreaUnitAIs(area(), ((UnitAITypes)(kUnit.getDefaultUnitAIType()))) == 0)
											{
												iValue += iNumCitiesInArea;
											}
											//This forces the AI to build necessary buildings for units. 
											
											UnitAITypes eUnitAI = (UnitAITypes)(kUnit.getDefaultUnitAIType());
											
											int unitsEnabledValue = 0;
											
											switch (eUnitAI)
											{
												case UNITAI_UNKNOWN:
												break;
												case UNITAI_ANIMAL:
													unitsEnabledValue += kUnit.getCombat() / 5;
													break;
												case UNITAI_SETTLE:
												case UNITAI_SETTLER_SEA:
													unitsEnabledValue += 25;
													break;
												case UNITAI_WORKER:
												case UNITAI_WORKER_SEA:
													unitsEnabledValue += std::max(0, AI_getWorkersNeeded()) * 10;
													break;
												case UNITAI_ATTACK:
												case UNITAI_ATTACK_CITY:
												case UNITAI_COLLATERAL:
												case UNITAI_PILLAGE:
												case UNITAI_RESERVE:
												case UNITAI_COUNTER:
												case UNITAI_CITY_DEFENSE:
												case UNITAI_CITY_COUNTER:
												case UNITAI_CITY_SPECIAL:
												case UNITAI_ATTACK_SEA:
												case UNITAI_RESERVE_SEA:
												case UNITAI_ESCORT_SEA:
												case UNITAI_ASSAULT_SEA:
												case UNITAI_CARRIER_SEA:
												case UNITAI_MISSILE_CARRIER_SEA:
												case UNITAI_PIRATE_SEA:
												case UNITAI_ATTACK_AIR:
												case UNITAI_DEFENSE_AIR:
												case UNITAI_CARRIER_AIR:
												case UNITAI_MISSILE_AIR:
												case UNITAI_PARADROP:
												case UNITAI_ATTACK_CITY_LEMMING:
													unitsEnabledValue += std::max(6, (kUnit.getCombat() * kUnit.getCombat())) / 6;
													break;
												case UNITAI_INFILTRATOR:
												case UNITAI_INVESTIGATOR:
												case UNITAI_PROPERTY_CONTROL:
												case UNITAI_HEALER:
												case UNITAI_PROPERTY_CONTROL_SEA:
												case UNITAI_HEALER_SEA:
												case UNITAI_EXPLORE:
												case UNITAI_MISSIONARY:
												case UNITAI_PROPHET:
												case UNITAI_ARTIST:
												case UNITAI_SCIENTIST:
												case UNITAI_GENERAL:
												case UNITAI_GREAT_HUNTER:
												case UNITAI_GREAT_ADMIRAL:
												case UNITAI_MERCHANT:
												case UNITAI_ENGINEER:
												case UNITAI_SPY:
												case UNITAI_SPY_SEA:
												case UNITAI_MISSIONARY_SEA:
												case UNITAI_EXPLORE_SEA:
													unitsEnabledValue += 20;
													break;
												case UNITAI_ICBM:
													unitsEnabledValue += (kUnit.getNukeRange() != -1 ? kUnit.getNukeRange() * 50: 0);
													break;
											}
											if (bWarPlan)
												unitsEnabledValue *= 2;

											if (unitsEnabledValue > 0)
											{
												//	Just because we can build new units here doesn't add much if we can already
												//	build them a ton of other places
												unitsEnabledValue = (unitsEnabledValue*3)/(3+std::min(10,kOwner.countNumBuildings(eBuilding)));
											}

											iValue += bUnitIsBonusEnabled ? unitsEnabledValue : unitsEnabledValue/5;
										}
									}
								}
								valuesCache->Accumulate(BUILDINGFOCUSINDEX_EXPERIENCE,
														iValue);
							}
						}
						/************************************************************************************************/
						/* Afforess	                     END                                                            */
						/************************************************************************************************/				
						// since this duplicates BUILDINGFOCUS_EXPERIENCE checks, do not repeat on pass 1
						//if ((iFocusFlags & BUILDINGFOCUS_DOMAINSEA))
						{
							PROFILE("CalculateAllBuildingValues.Sea");
							int iValue = 0;
							iValue += (kBuilding.getFreeExperience() * ((iHasMetCount > 0) ? 16 : 8));

							for (int iUnitIndex = 0; iUnitIndex < GC.getNumUnitClassInfos(); iUnitIndex++)
							{
								UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iUnitIndex);

								if (NO_UNIT != eUnit)
								{
									CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);
									int iCombatType = kUnitInfo.getUnitCombatType();
									if (kUnitInfo.getDomainType() == DOMAIN_SEA && canTrain(eUnit) && iCombatType != NO_UNITCOMBAT)
									{
										iValue += (kBuilding.getUnitCombatFreeExperience(iCombatType) * ((iHasMetCount > 0) ? 6 : 3));

										if (kBuilding.isUnitCombatRetrainType((UnitCombatTypes)iCombatType))
										{
											iValue += 20;
										}

										if (kBuilding.getNumUnitCombatOngoingTrainingDurations() > 0)
										{
											if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iUnitIndex) > 0)
											{
												if (kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iUnitIndex) < getUnitCombatOngoingTrainingTimeIncrement((UnitCombatTypes)iUnitIndex))
												{
													iValue = (20 - kBuilding.getUnitCombatOngoingTrainingDuration((UnitCombatTypes)iUnitIndex)) * 10;
												}
											}
										}
									}
								}
							}

							iValue += (kBuilding.getDomainFreeExperience(DOMAIN_SEA) * ((iHasMetCount > 0) ? 16 : 8));

							iValue += (kBuilding.getDomainProductionModifier(DOMAIN_SEA) / 4);
							valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_DOMAINSEA,iValue, false);
							valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_DOMAINSEA,iValue, true);
						}

						//if ((iFocusFlags & BUILDINGFOCUS_MAINTENANCE) || (iFocusFlags & BUILDINGFOCUS_GOLD) || (iPass > 0))
						{
							PROFILE("CalculateAllBuildingValues.Maintenance");
							int iValue = 0;
							int iExtraMaintenance = 0;
							if ( kBuilding.getCommerceChange(COMMERCE_GOLD) < 0 && GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE"))
							{
								iExtraMaintenance = -kBuilding.getCommerceChange(COMMERCE_GOLD);
								iExtraMaintenance *= GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent();
							}

							int iBaseMaintenance = getMaintenanceTimes100();
							int iExistingUpkeep = (iBaseMaintenance * std::max(0, 100 + getEffectiveMaintenanceModifier())) / 100;
							int iNewUpkeep = ((iBaseMaintenance + iExtraMaintenance)* std::max(0, 100 + getEffectiveMaintenanceModifier() + kBuilding.getMaintenanceModifier())) / 100;
							
							int maintainanceValue = (iExistingUpkeep - iNewUpkeep) / 16;
							
							if (bFinancialTrouble)
							{
								maintainanceValue *= 2;
							}
							else
							{
								maintainanceValue = (iGoldValueAssessmentModifier*maintainanceValue)/100;
							}

							iValue += maintainanceValue;

							valuesCache->Accumulate(BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE,
													iValue);
						}

						//if ((iFocusFlags & BUILDINGFOCUS_SPECIALIST) || (iPass > 0))
						{
							PROFILE("CalculateAllBuildingValues.Specialist");
							int iValue = 0;
							int iSpecialistsValue = 0;
							int iCurrentSpecialistsRunnable = 0;
							for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
							{
								if (iI != GC.getDefineINT("DEFAULT_SPECIALIST"))
								{
									bool bUnlimited = (GET_PLAYER(getOwnerINLINE()).isSpecialistValid((SpecialistTypes)iI));
									int iRunnable = (getMaxSpecialistCount((SpecialistTypes)iI) > 0);
									
									if (bUnlimited || (iRunnable > 0))
									{
										if (bUnlimited)
										{
											iCurrentSpecialistsRunnable += 5;
										}
										else
										{
											iCurrentSpecialistsRunnable += iRunnable;
										}
									}
									
									
			/************************************************************************************************/
			/* Afforess	                  Start		 12/7/09                                                */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/		
									if ((kBuilding.getSpecialistCount(iI) + GET_TEAM(getTeam()).getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI)) > 0)	
									/* Was: 
									if (kBuilding.getSpecialistCount(iI) > 0)
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/	
									{
										if ((!bUnlimited) && (iRunnable < 5))
										{
											int specialistValue = AI_specialistValue(((SpecialistTypes)iI), false, false);

			/************************************************************************************************/
			/* Afforess	                  Start		 12/7/09                                                */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
											specialistValue *= (20 + (40 * (kBuilding.getSpecialistCount(iI) + GET_TEAM(getTeam()).getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI))));
											/* Was:
											specialistValue *= (20 + (40 * kBuilding.getSpecialistCount(iI)));
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/	
					
											specialistValue /= 100;

			/************************************************************************************************/
			/* UNOFFICIAL_PATCH                       01/09/10                                jdog5000      */
			/*                                                                                              */
			/* Bugfix                                                                                       */
			/************************************************************************************************/
			/* original bts code
											if (iFoodDifference < 2)
											{
												iValue /= 4;
											}
											if (iRunnable > 0)
											{
												iValue /= 1 + iRunnable;
											}
			*/
											if (iFoodDifference < 2)
											{
												specialistValue /= 4;
											}
											if (iRunnable > 0)
											{
												specialistValue /= 1 + iRunnable;
											}
			/************************************************************************************************/
			/* UNOFFICIAL_PATCH                        END                                                  */
			/************************************************************************************************/

											iSpecialistsValue += std::max(12, (specialistValue / 100));
										}
									}
								}
							}
							
							if (iSpecialistsValue > 0)
							{
								iValue += iSpecialistsValue / std::max(2, iCurrentSpecialistsRunnable);
							}
							valuesCache->Accumulate(BUILDINGFOCUSINDEX_SPECIALIST,
													iValue);
						}
						
						//if ((iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH)) || iPass > 0)
						{
							PROFILE("CalculateAllBuildingValues.Pass1CommerceYields");

							// commerce yield
							int iCommerceYieldValue = AI_buildingYieldValue(YIELD_COMMERCE, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[YIELD_COMMERCE]);
							int iValue;

							//if ((iFocusFlags & BUILDINGFOCUS_GOLD) || iPass > 0)
							{
								iValue = (kOwner.getCommercePercent(COMMERCE_GOLD)*iCommerceYieldValue*(bFinancialTrouble ? 200 : iGoldValueAssessmentModifier))/10000;

								valuesCache->Accumulate(BUILDINGFOCUSINDEX_GOLD, iValue);
							}
							
							//if ((iFocusFlags & BUILDINGFOCUS_RESEARCH) || iPass > 0)
							{
								iValue = (kOwner.getCommercePercent(COMMERCE_RESEARCH)*iCommerceYieldValue)/100;

								valuesCache->Accumulate(BUILDINGFOCUSINDEX_RESEARCH, iValue);
							}
							
							//if ((iFocusFlags & BUILDINGFOCUS_CULTURE) || iPass > 0)
							{
								iValue = (kOwner.getCommercePercent(COMMERCE_CULTURE)*iCommerceYieldValue)/100;

								valuesCache->Accumulate(BUILDINGFOCUSINDEX_CULTURE, iValue);
							}
							
							//if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) || iPass > 0)
							{
								iValue = (kOwner.getCommercePercent(COMMERCE_ESPIONAGE)*iCommerceYieldValue)/100;

								valuesCache->Accumulate(BUILDINGFOCUSINDEX_ESPIONAGE, iValue);
							}
						}

						//if (iPass > 0)
						{
							int iValue = 0;

							for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
							{
								if (kBuilding.isHurry(iI))
								{
									iValue += 50;
								}
							}
							iValue -= kBuilding.getRiverDefensePenalty() / 4;
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      02/24/10                       jdog5000 & Afforess    */
			/*                                                                                              */
			/* City AI                                                                                      */
			/************************************************************************************************/
							if (kBuilding.isAreaCleanPower() && !(area()->isCleanPower(getTeam())))
							{
								int iLoop;
								for( CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop) )
								{
									if( pLoopCity->area() == area() )
									{
										if( pLoopCity->isDirtyPower() )
										{
											//Fuyu: less value for cities that don't already have power
											iValue += 8;
										}
										else if( !(pLoopCity->isPower()) )
										{
											iValue += 12;
										}
									}
								}
							}

							if (kBuilding.getDomesticGreatGeneralRateModifier() != 0)
							{
								iValue += (kBuilding.getDomesticGreatGeneralRateModifier() / 10);
							}

							if (kBuilding.isAreaBorderObstacle() && !(area()->isBorderObstacle(getTeam())))
							{
								if( !GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS) )
								{
									iValue += (iNumCitiesInArea);

									if(GC.getGameINLINE().isOption(GAMEOPTION_RAGING_BARBARIANS))
									{
										iValue += (iNumCitiesInArea);
									}
								}
							}
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/

							if (kBuilding.isGovernmentCenter())
							{
								FAssert(!(kBuilding.isCapital()));
								iValue += ((calculateDistanceMaintenance() - 3) * iNumCitiesInArea);
							}

							if (kBuilding.isMapCentering())
							{
								iValue++;
							}

							if (kBuilding.getFreeBonus() != NO_BONUS)
							{
								//	If we have spares adding another doesn't do anything that scales by city count - only
								//	the first one does that.  Furthermore as spares rack up even their trade value decreases
								iValue += (kOwner.AI_bonusVal((BonusTypes)(kBuilding.getFreeBonus()), 1) *
											 ((kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getFreeBonus())) == 0) ? iNumCities : 1) *
											 kBuilding.getNumFreeBonuses() *
											 100)/(100*std::max(1,kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getFreeBonus()))));
							}

							for (int iI = 0; iI < kBuilding.getNumExtraFreeBonuses(); iI++)
							{
								//	If we have spares adding another doesn't do anything that scales by city count - only
								//	the first one does that.  Furthermore as spares rack up even their trade value decreases
								iValue += (kOwner.AI_bonusVal((BonusTypes)(kBuilding.getExtraFreeBonus(iI)), 1) *
											 ((kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getExtraFreeBonus(iI))) == 0) ? iNumCities : 1) *
											 kBuilding.getExtraFreeBonusNum(iI) *
											 100)/(100*std::max(1,kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getExtraFreeBonus(iI)))));
							}

							if (kBuilding.getNoBonus() != NO_BONUS)
							{
								iValue -= kOwner.AI_bonusVal((BonusTypes)kBuilding.getNoBonus());
							}



							if (kBuilding.getCivicOption() != NO_CIVICOPTION)
							{
								for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
								{
									if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == kBuilding.getCivicOption())
									{
										if (!(kOwner.canDoCivics((CivicTypes)iI)))
										{
											iValue += (kOwner.AI_civicValue((CivicTypes)iI) / 10);
										}
									}
								}
							}
							
							int iGreatPeopleRateModifier = kBuilding.getGreatPeopleRateModifier();
							if (iGreatPeopleRateModifier > 0)
							{
								int iGreatPeopleRate = getBaseGreatPeopleRate();
								const int kTargetGPRate = 10;

								// either not a wonder, or a wonder and our GP rate is at least the target rate
								if (!bIsLimitedWonder || iGreatPeopleRate >= kTargetGPRate)
								{
									iValue += ((iGreatPeopleRateModifier * iGreatPeopleRate) / 16);
								}
								// otherwise, this is a limited wonder (aka National Epic), we _really_ do not want to build this here
								// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
								else
								{
									iValue -= ((iGreatPeopleRateModifier * (kTargetGPRate - iGreatPeopleRate)) / 12);
								}
							}

							iValue += ((kBuilding.getGlobalGreatPeopleRateModifier() * iNumCities) / 8);

							iValue += (-(kBuilding.getAnarchyModifier()) / 4);

							iValue += (-(kBuilding.getGlobalHurryModifier()) * 2);

							iValue += (kBuilding.getGlobalFreeExperience() * iNumCities * ((iHasMetCount > 0) ? 6 : 3));

							if (bCanPopRush)
							{
								iValue += kBuilding.getFoodKept() / 2;
							}

							iValue += kBuilding.getAirlift() * (getPopulation() * 3 + 10);
							
							int iAirDefense = -kBuilding.getAirModifier();
							if (iAirDefense > 0)
							{
								if (((kOwner.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) > 0) && (kOwner.AI_totalUnitAIs(UNITAI_ATTACK_AIR) > 0)) || (kOwner.AI_totalUnitAIs(UNITAI_MISSILE_AIR) > 0))
								{
									iValue += iAirDefense / ((iHasMetCount > 0) ? 2 : 4);
								}
							}

							iValue += kBuilding.getAirUnitCapacity() * (getPopulation() * 2 + 10);

							iValue += (-(kBuilding.getNukeModifier()) / ((iHasMetCount > 0) ? 10 : 20));

							//iValue += (kBuilding.getFreeSpecialist() * 16);
							iValue += (kBuilding.getAreaFreeSpecialist() * iNumCitiesInArea * 12);
							iValue += (kBuilding.getGlobalFreeSpecialist() * iNumCities * 12);

							iValue += ((kBuilding.getWorkerSpeedModifier() * kOwner.AI_getNumAIUnits(UNITAI_WORKER)) / 10);

							int iMilitaryProductionModifier = kBuilding.getMilitaryProductionModifier();

							for (int iJ = 0; iJ < kBuilding.getNumUnitCombatProdModifiers(); iJ++)
							{
								iMilitaryProductionModifier += kBuilding.getUnitCombatProdModifier(iJ) / 4;
							}

							if (iHasMetCount > 0 && iMilitaryProductionModifier > 0)
							{
								// either not a wonder, or a wonder and we are a high production city
								if (!bIsLimitedWonder || bIsHighProductionCity)
								{
									iValue += (iMilitaryProductionModifier / 4);

									// if a wonder, then pick one of the best cities
									if (bIsLimitedWonder)
									{
										// if one of the top 3 production cities, give a big boost
										if (aiYieldRank[YIELD_PRODUCTION] <= (2 + iLimitedWonderLimit))
										{
											iValue += (2 * iMilitaryProductionModifier) / (2 + aiYieldRank[YIELD_PRODUCTION]);
										}
									}
									// otherwise, any of the top half of cities will do
									else if (bIsHighProductionCity)
									{
										iValue += iMilitaryProductionModifier / 4;
									}
									iValue += ((iMilitaryProductionModifier * (getFreeExperience() + getSpecialistFreeExperience())) / 10);
								}
								// otherwise, this is a limited wonder (aka Heroic Epic), we _really_ do not want to build this here
								// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
								else
								{
									iValue -= (iMilitaryProductionModifier * aiYieldRank[YIELD_PRODUCTION]) / 5;
								}
							}
							iValue += (kBuilding.getSpaceProductionModifier() / 5);
							iValue += ((kBuilding.getGlobalSpaceProductionModifier() * iNumCities) / 20);
							

							if (kBuilding.getGreatPeopleUnitClass() != NO_UNITCLASS)
							{
								iValue++; // XXX improve this for diversity...
							}
							
							// prefer to build great people buildings in places that already have some GP points
			/************************************************************************************************/
			/* Afforess	                  Start		 07/28/10                                               */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
			/*
							iValue += (kBuilding.getGreatPeopleRateChange() * 10) * (1 + (getBaseGreatPeopleRate() / 2));
			*/
							iValue += ((kBuilding.getGreatPeopleRateChange() + iSpecialistGreatPeopleRate ) * 10) * (1 + (getBaseGreatPeopleRate() / 2));
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/


							if (!bAreaAlone)
							{
								for (int iI = 0; iI < kBuilding.getNumHealUnitCombatTypes(); iI++)
								{
									iValue += (kBuilding.getHealUnitCombatType(iI).iHeal /4);
								}
								iValue += (kBuilding.getHealRateChange() / 2);
								for (int iI = 0; iI < kBuilding.getNumAidRateChanges(); iI++)
								{
									iValue += (kBuilding.getAidRateChange(iI).iChange / 3);//Update
								}
								for (int iI = 0; iI < kBuilding.getNumBonusAidModifiers(); iI++)
								{
									if (hasBonus(kBuilding.getBonusAidModifier(iI).eBonusType))
									{
										iValue += kBuilding.getBonusAidModifier(iI).iModifier / 3;
									}
								}
							}

							iValue += (kBuilding.getGlobalPopulationChange() * iNumCities * 4);

							iValue += (kBuilding.getFreeTechs() * 80);

							iValue += kBuilding.getEnemyWarWearinessModifier() / 2;

							for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
							{
								if (kBuilding.getFreeSpecialistCount(iI) > 0)
								{
									iValue += ((AI_specialistValue(((SpecialistTypes)iI), false, false) * kBuilding.getFreeSpecialistCount(iI)) / 50);
								}
							}

							for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
							{
								if (kBuilding.getImprovementFreeSpecialist(iI) > 0)
								{
									iValue += kBuilding.getImprovementFreeSpecialist(iI) * countNumImprovedPlots((ImprovementTypes)iI, true) * 50;
								}
							}

							for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
							{
								iValue += (kBuilding.getDomainProductionModifier(iI) / 5);
								
								if (bIsHighProductionCity)
								{
									iValue += (kBuilding.getDomainProductionModifier(iI) / 5);
								}
							}
			/************************************************************************************************/
			/* Afforess	                  Start		 12/7/09                                                */
			/*                                                                                              */
			/*                                                                                              */
			/************************************************************************************************/
							int forceTradeRoutesValue = 0;
							if (kBuilding.isForceAllTradeRoutes())
							{
								for (int iI = 0; iI < MAX_TEAMS; iI++)
								{
									if (GET_TEAM((TeamTypes)iI).isAlive())
									{
										if (!GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isVassal((TeamTypes)iI))
										{
											if (!GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isOpenBorders((TeamTypes)iI))
											{
												forceTradeRoutesValue += GET_TEAM((TeamTypes)iI).getNumCities() * 2;
											}
										}
									}
								}
							}
							iValue += forceTradeRoutesValue;
							if (kBuilding.getUnitClassProductionModifier(NO_UNITCLASS) != 0)
							{
								int unitProductionModifierValue = 0;
								for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
								{
									int iModifier = kBuilding.getUnitClassProductionModifier(iI);
									if (iModifier != 0)
									{
										UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);
									
										if (canTrain(eLoopUnit))
										{
											UnitAITypes eUnitAI = (UnitAITypes) GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType();
											//unitProductionModifierValue += GET_PLAYER(getOwnerINLINE()).AI_unitValue(eLoopUnit, eUnitAI, area()) * iModifier / 100;

											UnitTypes eBestUnit = kOwner.bestBuildableUnitForAIType((DomainTypes)GC.getUnitInfo(eLoopUnit).getDomainType(), eUnitAI);
											int iBuildCost;

											if ( eBestUnit == NO_UNIT )
											{
												iBuildCost = GC.getUnitInfo(eLoopUnit).getProductionCost();
											}
											else
											{
												int	iBestUnitAIValue = kOwner.AI_unitValue(eBestUnit, eUnitAI, area());
												int	iThisUnitAIValue = kOwner.AI_unitValue(eLoopUnit, eUnitAI, area());

												//	Value as cost of production of the unit we can build scaled by their relative AI value (non-linear - we're squaring the ratio)
												int	iComparisonToBestFactor = (10*iThisUnitAIValue)/std::max(1,iBestUnitAIValue);

												iBuildCost = (iComparisonToBestFactor * iComparisonToBestFactor * GC.getUnitInfo(eBestUnit).getProductionCost())/100;
											}

											unitProductionModifierValue += (iBuildCost*2*iModifier)/100;
										}
									}
								}
								if (bIsHighProductionCity)
								{
									unitProductionModifierValue *= 3; // adding 10% and 20% is same as adding 30% or 3 * 10%
								}	

								iValue += unitProductionModifierValue;
							}
							if (kBuilding.getPopulationgrowthratepercentage() != 0)
							{
								int iCityHappy = happyLevel() - unhappyLevel();
								int iCurrentFoodToGrow = growthThreshold();
								int iFoodPerTurn = foodDifference();

								if ( iFoodPerTurn > 0 )
								{
									if ( iCityHappy >= 0 )
									{
										int popGrowthValue = (std::min(3,iCityHappy+1)*iCurrentFoodToGrow)/iFoodPerTurn;
										iValue -= (kBuilding.getPopulationgrowthratepercentage()*popGrowthValue) / 100;
									}
								}

								

							}
							if (kBuilding.getGlobalPopulationgrowthratepercentage() != 0)
							{
								int iLoop;
								int iCityCount = 0;
								int popGrowthRateGlobalValue = 0;
								for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
								{
									int iCityHappy = pLoopCity->happyLevel() - pLoopCity->unhappyLevel();
									int iCurrentFoodToGrow = pLoopCity->growthThreshold();
									int iFoodPerTurn = pLoopCity->foodDifference();

									iCityCount++;

									if ( iFoodPerTurn > 0 )
									{
										if ( iCityHappy >= 0 )
										{
											popGrowthRateGlobalValue -= (std::min(3,iCityHappy+1)*iCurrentFoodToGrow)/iFoodPerTurn;
										}
									}
								}

								if ( iCityCount > 0 )
								{
									iValue += (kBuilding.getGlobalPopulationgrowthratepercentage()*popGrowthRateGlobalValue)/(iCityCount*100);
								}
							}

							if (kBuilding.getBuildingClassProductionModifier(NO_BUILDINGCLASS) != 0)
							{
								int buildingProductionModifierValue = 0;
								for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
								{
									if (kBuilding.getBuildingClassProductionModifier(iI) != 0)
									{
										BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));
										if (eLoopBuilding != NO_BUILDING)
										{
											if (canConstruct(eLoopBuilding))
											{
												int iModifier = kBuilding.getBuildingClassProductionModifier(iI);
												int iOriginalCost = getHurryCost(true, eLoopBuilding, false);
												if (iModifier > -100)
												{
													int iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
													buildingProductionModifierValue +=  (iOriginalCost - iNewCost) / 10;
												}
												else
												{//If the modifier is less than -100, avoid it like the plague
													buildingProductionModifierValue -= 100;
												}
											}
										}
									}
								}
								iValue += buildingProductionModifierValue;
							}

							if (kBuilding.getGlobalBuildingClassProductionModifier(NO_BUILDINGCLASS) != 0)
							{
								int globalBuildingProductionModifierValue = 0;
								for (int iI = 0; iI < numNumBuildingClassInfos; iI++)
								{
									if (kBuilding.getGlobalBuildingClassProductionModifier(iI) != 0)
									{
										BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));
										if (eLoopBuilding != NO_BUILDING)
										{
											if (canConstruct(eLoopBuilding))
											{
												int iModifier = kBuilding.getGlobalBuildingClassProductionModifier(iI);
												int iOriginalCost = getHurryCost(true, eLoopBuilding, false);

												if (iModifier > -100)
												{
													int iLoop;
													int iCount = 0;

													for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
													{
														if ( pLoopCity->getNumBuilding(eLoopBuilding) == 0 )
														{
															iCount++;
														}
													}

													int iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
													globalBuildingProductionModifierValue += ((iOriginalCost - iNewCost)*iCount) / 10;
												}
												else
												{//If the modifier is less than -100, avoid it like the plague
													globalBuildingProductionModifierValue -= 100;
												}
											}
										}
									}
								}
								iValue += globalBuildingProductionModifierValue;
							}
							if (kBuilding.isProvidesFreshWater() && !plot()->isFreshWater())
							{
								int freshWaterModifier = 0;
								CvPlot* pLoopPlot;
								for (int iI = 0; iI < NUM_CITY_PLOTS_1; iI++)
								{
									if (iI != CITY_HOME_PLOT)
									{
										pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
										if (pLoopPlot != NULL &&
											!pLoopPlot->isWater() &&
											!pLoopPlot->isFreshWater() &&
											!pLoopPlot->isHills() &&
											!pLoopPlot->isImpassable() )
										{
											if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
											{//if there are no improvements, farms become much more likely
												freshWaterModifier += 20;
											}
											else
											{//we are not likely to tear down existing improvements for a farm
												freshWaterModifier += 5;
											}
										}
									}
								}
								iValue += freshWaterModifier;
							}
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/	

							int religiousBuildingValue = 0;
							for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
							{
								if (GC.getUnitInfo((UnitTypes)iI).getPrereqBuilding() == eBuilding)
								{
									// BBAI TODO: Smarter monastary construction, better support for mods

									if (kOwner.AI_totalAreaUnitAIs(area(), ((UnitAITypes)(GC.getUnitInfo((UnitTypes)iI).getDefaultUnitAIType()))) == 0)
									{
										religiousBuildingValue += iNumCitiesInArea;
									}

									religiousBuildingValue++;

									ReligionTypes eReligion = (ReligionTypes)(GC.getUnitInfo((UnitTypes)iI).getPrereqReligion());
									if (eReligion != NO_RELIGION)
									{
										//encouragement to get some minimal ability to train special units
										if (bCulturalVictory1 || isHolyCity(eReligion) || isCapital())
										{
											religiousBuildingValue += (2 + iNumCitiesInArea);
										}
										
										if (bCulturalVictory2 && GC.getUnitInfo((UnitTypes)iI).getReligionSpreads(eReligion))
										{
											//this gives a very large extra value if the religion is (nearly) unique
											//to no extra value for a fully spread religion.
											//I'm torn between huge boost and enough to bias towards the best monastery type.
											int iReligionCount = GET_PLAYER(getOwnerINLINE()).getHasReligionCount(eReligion);
											religiousBuildingValue += (100 * (iNumCities - iReligionCount)) / (iNumCities * (iReligionCount + 1));
										}
									}
								}
							}

							if (religiousBuildingValue > 0)
							{
								//	Just because we can build new units here doesn't add much if we can already
								//	build them a ton of other places
								religiousBuildingValue = (religiousBuildingValue*3)/(3+std::min(10,kOwner.countNumBuildings(eBuilding)));
							}

							iValue += religiousBuildingValue;
							
							// is this building needed to build other buildings?
							for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
							{
								int iPrereqBuildings = kOwner.getBuildingClassPrereqBuilding(((BuildingTypes) iI), eBuildingClass);

								// if we need some of us to build iI building, and we dont need more than we have cities
								if (iPrereqBuildings > 0 && iPrereqBuildings <= iNumCities)
								{
									// do we need more than what we are currently building?
									if (iPrereqBuildings > kOwner.getBuildingClassCountPlusMaking(eBuildingClass))
									{
										iValue += (iNumCities * 3);

										if (bCulturalVictory1)
										{
											BuildingTypes eLoopBuilding = (BuildingTypes) iI;
											CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
											int iLoopBuildingCultureModifier = kLoopBuilding.getCommerceModifier(COMMERCE_CULTURE);
											if (iLoopBuildingCultureModifier > 0)
											{
												int iLoopBuildingsBuilt = kOwner.getBuildingClassCount((BuildingClassTypes) kLoopBuilding.getBuildingClassType());
												
												// if we have less than the number needed in culture cities
												//		OR we are one of the top cities and we do not have the building
												if (iLoopBuildingsBuilt < iCulturalVictoryNumCultureCities || 
													(iCultureRank <= iCulturalVictoryNumCultureCities && 0 == getNumBuilding(eLoopBuilding)))
												{
													iValue += iLoopBuildingCultureModifier;

													if (bCulturalVictory3)
													{
														iValue += iLoopBuildingCultureModifier * 2;
													}
												}
											}
										}
									}
								}
							}
							
							for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
							{
								if (kBuilding.getVoteSourceType() == iI)
								{
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                      05/24/10                              jdog5000        */
			/*                                                                                              */
			/* City AI, Victory Strategy AI                                                                 */
			/************************************************************************************************/					
									int votesValue = 0;
									if (kBuilding.isStateReligion())
									{
										int iShareReligionCount = 0;
										int iPlayerCount = 0;
										for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
										{
											CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
											if ((iPlayer != getOwner()) && kLoopPlayer.isAlive())
											{
												iPlayerCount++;
												if (GET_PLAYER(getOwnerINLINE()).getStateReligion() == kLoopPlayer.getStateReligion())
												{
													iShareReligionCount++;										
												}
											}
										}
										votesValue += (200 * (1 + iShareReligionCount)) / (1 + iPlayerCount);
									}
									else
									{
										votesValue += 100;
									}

									iValue += (votesValue * (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_DIPLOMACY1) ? 5 : 1));
								}
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD                       END                                                  */
			/************************************************************************************************/
							}
							valuesCache->AccumulateToAny(iValue,false);
						}

						//if (iPass > 0)
						{
							int iValue = 0;

							for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
							{
								int yieldValue = 0;

								//Trade route yields are now acounted in pass1
								//iValue += ((kBuilding.getTradeRouteModifier() * getTradeYield((YieldTypes)iI)) / 12);
								//if (bForeignTrade)
								//{
								//	iValue += ((kBuilding.getForeignTradeRouteModifier() * getTradeYield((YieldTypes)iI)) / 12);
								//}

								if (iFoodDifference > 0)
								{
									iValue += kBuilding.getFoodKept() / 2;
								}

								iValue += AI_buildingYieldValue((YieldTypes)iI, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[iI]);

								int iGlobalModifier = kBuilding.getGlobalYieldModifier(iI);
								int iAreaModifier = kBuilding.getAreaYieldModifier(iI);
								int iYieldModiferValue = 0;

								if ( iGlobalModifier > 0 || iAreaModifier > 0 )
								{
									int iLoop = 0;
									for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
									{
										int iCityValue = pLoopCity->getBaseYieldRate((YieldTypes)iI);
										iYieldModiferValue += iCityValue*(iGlobalModifier + (pLoopCity->area() == area() ? iAreaModifier : 0));
									}
								}
								yieldValue += iYieldModiferValue/12;
								//yieldValue += ((kBuilding.getAreaYieldModifier(iI) * iNumCitiesInArea) / 3);
								//yieldValue += ((kBuilding.getGlobalYieldModifier(iI) * iNumCities) / 3);

								for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
								{
									yieldValue += ((kBuilding.getSpecialistYieldChange(iJ, iI) * kOwner.getTotalPopulation()) / 5);
								}
								
								if (yieldValue != 0)
								{
									if (bFinancialTrouble && iI == YIELD_COMMERCE)
									{
										yieldValue *= 2;
									}

									yieldValue *= kOwner.AI_yieldWeight((YieldTypes)iI);
									yieldValue /= 100;
									
									if (aiYieldRank[iI] == MAX_INT)
									{
										aiYieldRank[iI] = findBaseYieldRateRank((YieldTypes) iI);
									}

									// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
									// we do _not_ want to build this here (unless the value was small anyway)
									// Exempt unit build from this test however
									if (bIsLimitedWonder && canConstruct(eBuilding) && (aiYieldRank[iI] > (3 + iLimitedWonderLimit)))
									{
										yieldValue *= -1;
									}

									iValue += yieldValue;
								}
							}

							valuesCache->AccumulateToAny(iValue,false);
						}
						//else
						{
							//if (iFocusFlags & BUILDINGFOCUS_FOOD)
							{
								PROFILE("CalculateAllBuildingValues.Food");
								int iValue = 0;
								
								iValue += AI_buildingYieldValue(YIELD_FOOD, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[YIELD_FOOD]);

								iValue += kBuilding.getFoodKept();

								valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_FOOD,iValue, true);
							}

							//if (iFocusFlags & BUILDINGFOCUS_PRODUCTION)
							{
								PROFILE("CalculateAllBuildingValues.Production");
								int iValue = 0;

								int directYieldValue = AI_buildingYieldValue(YIELD_PRODUCTION, eBuilding, kBuilding, bForeignTrade, iFoodDifference, aiFreeSpecialistYield[YIELD_PRODUCTION]);
								
								// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
								// we do _not_ want to build this here (unless the value was small anyway)
								if (bIsLimitedWonder && (aiYieldRank[YIELD_PRODUCTION] > (3 + iLimitedWonderLimit)))
								{
									directYieldValue *= -1;
								}

								iValue += directYieldValue;

								// Koshling - modify direct production value if the base yield rate for this city (for production) is low
								// in relation to the amount this would add
								// i.e. - boost the value of extra production especially when we donit already have much
								if ( directYieldValue > getBaseYieldRate(YIELD_PRODUCTION)/3 )
								{
									//	directYieldValue is roughly 8*<direct production gain>, so if that implies a net gain
									//	of 4% (ish) or more boost the value
									int iBoost = (directYieldValue*12 - getBaseYieldRate(YIELD_PRODUCTION)*4);

									//	Examples -
									//	1) City poduction is 1, building adds 1 (directYieldValue 8), so iBoost is 92 - value almost trebbled
									//	2) Same building when prodcution is already 10, iBoost is 56 - value roughly doubled
									//	3) Same building when production >= 24 - no boost
									directYieldValue += (directYieldValue*iBoost)/50;
								}

								iValue += directYieldValue;

								valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_PRODUCTION,iValue, true);
							}
						}

						//if (iPass > 0)
						{
							int iValue = 0;

							//	Deal with properties
							iValue += buildingPropertiesValue(kBuilding);

							for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
							{
								int directCommerceValue = getBuildingCommerceValue(eBuilding, iI, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);

								for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
								{
									directCommerceValue += ((kBuilding.getSpecialistCommerceChange(iJ, iI) * kOwner.getTotalPopulation()) / 6);
								}
								if (directCommerceValue != 0)
								{
									//	Make sure we don't reduce 1 to 0!
									if ( directCommerceValue >= 2 )
									{
										directCommerceValue /= 2;
									}

									if (bFinancialTrouble && iI == COMMERCE_GOLD)
									{
										directCommerceValue *= 2;
									}

									directCommerceValue *= kOwner.AI_commerceWeight(((CommerceTypes)iI), this);
									directCommerceValue = (directCommerceValue + 99) / 100;

									// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
									// we do _not_ want to build this here (unless the value was small anyway)
									if (MAX_INT == aiCommerceRank[iI])
									{
										aiCommerceRank[iI] = findCommerceRateRank((CommerceTypes) iI);
									}
									if (bIsLimitedWonder && ((aiCommerceRank[iI] > (3 + iLimitedWonderLimit)))
										|| (bCulturalVictory1 && (iI == COMMERCE_CULTURE) && (aiCommerceRank[iI] == 1)))
									{
										directCommerceValue *= -1;

										// for culture, just set it to zero, not negative, just about every wonder gives culture
										if (iI == COMMERCE_CULTURE)
										{
											directCommerceValue = 0;
										}
									}
									iValue += directCommerceValue;
								}
							}

							for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
							{
								if (kBuilding.getReligionChange(iI) > 0)
								{
									if (GET_TEAM(getTeam()).hasHolyCity((ReligionTypes)iI))
									{
										iValue += (kBuilding.getReligionChange(iI) * ((eStateReligion == iI) ? 10 : 1));
									}
								}
							}

							if (NO_VOTESOURCE != kBuilding.getVoteSourceType())
							{
								iValue += 100;
							}

							valuesCache->AccumulateToAny(iValue,false);
						}
						//else
						{
							valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_GOLD, getBuildingCommerceValue(eBuilding, COMMERCE_GOLD, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate),true);

							valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_RESEARCH, getBuildingCommerceValue(eBuilding, COMMERCE_RESEARCH, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate),true);

							valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_CULTURE, getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate,aiPlayerCommerceRate),true);

							valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_BIGCULTURE, getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate)/5,true);
							
							if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
							{
								valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_CULTURE, getBuildingCommerceValue(eBuilding, COMMERCE_ESPIONAGE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate),true);
							}
							else
							{
								valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_ESPIONAGE, getBuildingCommerceValue(eBuilding, COMMERCE_ESPIONAGE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate),true);
							}
						}
						
						//if ((iThreshold > 0) && (iPass == 0))
						//{
						//	if (iValue < iThreshold)
						//	{
						//		iValue = 0;
						//	}
						//}
						//if ( iPass != 0 && !isHuman())
						if ( !isHuman())
						{
							valuesCache->AccumulateToAny(kBuilding.getAIWeight(),false);

							//	Flavor calculation is non-linear and cannot be calculated in the
							//	caching - it is calculated post-cache retrieval
						}
					}
				}
			}
		}
	}
}

int CvCityAI::getBuildingCommerceValue(BuildingTypes eBuilding, int iI, int* aiFreeSpecialistYield, int* aiFreeSpecialistCommerce, int* aiBaseCommerceRate, int* aiPlayerCommerceRate)
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	int iLimitedWonderLimit = limitedWonderClassLimit((BuildingClassTypes)kBuilding.getBuildingClassType());
	bool bCulturalVictory1 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1);
	// unused: bool bCulturalVictory2 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	bool bCulturalVictory3 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);
	ReligionTypes eStateReligion = kOwner.getStateReligion();
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 3/24/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iBaseCommerceChange = GC.getBuildingInfo(eBuilding).getCommerceChange((CommerceTypes)iI);

	if ( iBaseCommerceChange < 0 && iI == COMMERCE_GOLD && GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE") )
	{
		iBaseCommerceChange = 0;
	}
	iBaseCommerceChange += ((GC.getBuildingInfo(eBuilding).getCommercePerPopChange((CommerceTypes)iI) * getPopulation())/100);

	int iResult = 0;
	iResult += ((iBaseCommerceChange + GET_TEAM(getTeam()).getBuildingCommerceChange(eBuilding, (CommerceTypes)iI) + aiFreeSpecialistCommerce[iI]) * 4);
	/* Was:
	iResult += (kBuilding.getCommerceChange(iI) * 4);
	*/		
	iResult += (kBuilding.getObsoleteSafeCommerceChange(iI) * 4);
	iResult += (kBuilding.getCommerceAttacks(iI) * 2);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

	if ((CommerceTypes)iI == COMMERCE_CULTURE)
	{
		// K-Mod. + 2 * foreign culture percent.
		iResult *= 300 - 2 * calculateCulturePercent(getOwner());
		iResult /= 100;
		// K-Mod end
		if (bCulturalVictory1)
		{
			iResult *= 2;					        
		}
	}

	if (kBuilding.getCommerceChangeDoubleTime(iI) > 0)
	{
		iResult += (iResult*500)/(500+kBuilding.getCommerceChangeDoubleTime(iI));
	}

	//	Multiply up by the city's current commerce rate modifier
	iResult = (iResult*getTotalCommerceRateModifier((CommerceTypes)iI))/100;

	//	Factor in yield changes

/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iTempValue = (((kBuilding.getYieldModifier(YIELD_COMMERCE) + GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, YIELD_COMMERCE)) * getBaseYieldRate(YIELD_COMMERCE)));	
	/* Was:
	iTempValue = ((kBuilding.getYieldModifier(YIELD_COMMERCE) * getBaseYieldRate(YIELD_COMMERCE)));
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	iTempValue *= kOwner.getCommercePercent((CommerceTypes)iI);
	
	iTempValue /= 3000;
	
	// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
	// we do _not_ want to build this here (unless the value was small anyway)
	if (iLimitedWonderLimit >= 0 && (findCommerceRateRank((CommerceTypes)iI) > (3 + iLimitedWonderLimit)))
	{
		iTempValue *= -1;
	}

	iResult += iTempValue;
/************************************************************************************************/
/* Afforess	                  Start		 4/20/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	iTempValue = 0;
	iTempValue += ((kBuilding.getYieldModifier(YIELD_COMMERCE) + GET_TEAM(getTeam()).getBuildingYieldModifier(eBuilding, YIELD_COMMERCE)) * getBaseYieldRate(YIELD_COMMERCE) + aiFreeSpecialistYield[YIELD_COMMERCE]) / 8;
	iTempValue += (kBuilding.getYieldChange(YIELD_COMMERCE) + ((kBuilding.getYieldPerPopChange(YIELD_COMMERCE)*getPopulation())/100) + GET_TEAM(getTeam()).getBuildingYieldChange(eBuilding, YIELD_COMMERCE)) * 8;
	for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
	{
		if (hasBonus((BonusTypes)iJ))
		{
			//TB Traits begin
			iTempValue += (kBuilding.getBonusYieldModifier(iJ, YIELD_COMMERCE) * getModifiedBaseYieldRate(YIELD_COMMERCE) / 12);
			//TB Traits end
			iTempValue += (kBuilding.getBonusYieldChanges(iJ, YIELD_COMMERCE) * 8);
			if (kBuilding.getVicinityBonusYieldChanges(iJ, YIELD_COMMERCE) != 0 && hasVicinityBonus((BonusTypes)iJ))
			{
				iTempValue += (kBuilding.getVicinityBonusYieldChanges(iJ, YIELD_COMMERCE) * 8);
			}
		}
	}
	iTempValue *= GET_PLAYER(getOwnerINLINE()).getCommercePercent((CommerceTypes)iI);
	iTempValue /= 100;
	iResult += iTempValue;
	
	// add value for a commerce modifier
/************************************************************************************************/
/* Afforess	                  Start		 12/7/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int iCommerceModifier = kBuilding.getCommerceModifier(iI) + GET_TEAM(getTeam()).getBuildingCommerceModifier(eBuilding, (CommerceTypes)iI);
	/* Was:
	int iCommerceModifier = kBuilding.getCommerceModifier(iI);
	*/
	for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
	{
		iCommerceModifier += kBuilding.getBonusCommerceModifier(iJ,iI);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	if ( aiBaseCommerceRate[iI] == MAX_INT )
	{
		aiBaseCommerceRate[iI] = getBaseCommerceRate((CommerceTypes) iI);
	}
	int iBaseCommerceRate = aiBaseCommerceRate[iI];
	int iCommerceMultiplierValue = iCommerceModifier * iBaseCommerceRate;

	//	Koshling - % modifiers are undervalued by the AI for two reasons:
	//	1)	It only calculates its efect at the time of construction, but that discounts future increases as
	//		base reserach goes up
	//	2)	In the early game because this calculation only takes account of the absolute increase, not its proportion of the
	//		civilization whole, which will be substantial early on (indeed 100% with 1 city).
	//	We therefore apply a small boost to reflect futures, and compare to the whole
	//	and make an upward adjustment proportional to the ratio with the civilization whole.
	//
	//	We don't apply this fo culture, since it isn't really a civ-wide quantity for most purposes
	if ( iCommerceMultiplierValue > 0 && iI != COMMERCE_CULTURE)
	{
		iCommerceMultiplierValue += 120;	//	Discounted futures
		iCommerceMultiplierValue += 2*iCommerceMultiplierValue*std::max(1,aiBaseCommerceRate[iI])/std::max(1,aiPlayerCommerceRate[iI]);	//	Proportion of whole
	}

	if (((CommerceTypes) iI) == COMMERCE_CULTURE && iCommerceModifier != 0)
	{
		int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
		int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();

		if (bCulturalVictory1)
		{							
			// if this is one of our top culture cities, then we want to build this here first!
			if (iCultureRateRank <= iCulturalVictoryNumCultureCities)
			{					        
				iCommerceMultiplierValue /= 8;					        
				
				// if we at culture level 3, then these need to get built asap
				if (bCulturalVictory3)
				{
					// its most important to build in the lowest rate city, but important everywhere
					iCommerceMultiplierValue += std::max(100, 500 - iBaseCommerceRate) * iCommerceModifier;					        
				}
			}
			else
			{
				int iCountBuilt = kOwner.getBuildingClassCountPlusMaking((BuildingClassTypes)kBuilding.getBuildingClassType());

				// do we have enough buildings to build extras?
				bool bHaveEnough = true;

				// if its limited and the limit is less than the number we need in culture cities, do not build here
				if (iLimitedWonderLimit >= 0 && (iLimitedWonderLimit <= iCulturalVictoryNumCultureCities))
				{
					bHaveEnough = false;
				}

				for (int iJ = 0; bHaveEnough && iJ < GC.getNumBuildingClassInfos(); iJ++)
				{
					// count excess the number of prereq buildings which do not have this building built for yet
					int iPrereqBuildings = kOwner.getBuildingClassPrereqBuilding(eBuilding, (BuildingClassTypes) iJ, -iCountBuilt);
					
					// do we not have enough built (do not count ones in progress)
					if (iPrereqBuildings > 0 && kOwner.getBuildingClassCount((BuildingClassTypes) iJ) <  iPrereqBuildings)
					{
						bHaveEnough = false;
					}
				}

				// if we have enough and our rank is close to the top, then possibly build here too
				if (bHaveEnough && (iCultureRateRank - iCulturalVictoryNumCultureCities) <= 3)
				{
					iCommerceMultiplierValue /= 12;
				}
				// otherwise, we really do not want to build this here
				else
				{
					iCommerceMultiplierValue /= 30;
				}
			}
		}
		else
		{
			iCommerceMultiplierValue /= 15;

			// increase priority if we need culture oppressed city
			iCommerceMultiplierValue *= (100 - calculateCulturePercent(getOwnerINLINE()));
		}
	}
	else
	{
		iCommerceMultiplierValue /= 15;
	}
	iResult += iCommerceMultiplierValue;

	iResult += (kBuilding.getGlobalCommerceModifier(iI) * GET_PLAYER(getOwnerINLINE()).getCommerceRate((CommerceTypes) iI))/8;
	//iResult += ((kBuilding.getGlobalCommerceModifier(iI) * kOwner.getNumCities()) / 4);
	iResult += ((kBuilding.getSpecialistExtraCommerce(iI) * kOwner.getTotalPopulation()) / 3);

	if (eStateReligion != NO_RELIGION)
	{
		iResult += (kBuilding.getStateReligionCommerce(iI) * kOwner.getHasReligionCount(eStateReligion) * 3);
	}

	if (kBuilding.getGlobalReligionCommerce() != NO_RELIGION)
	{
		iResult += (GC.getReligionInfo((ReligionTypes)(kBuilding.getGlobalReligionCommerce())).getGlobalReligionCommerce(iI) * GC.getGameINLINE().countReligionLevels((ReligionTypes)(kBuilding.getGlobalReligionCommerce())) * 2);
		if (eStateReligion == (ReligionTypes)(kBuilding.getGlobalReligionCommerce()))
		{
			iResult += 10;
		}
	}

	CorporationTypes eCorporation = (CorporationTypes)kBuilding.getFoundsCorporation();
	int iCorpValue = 0;
	if (NO_CORPORATION != eCorporation)
	{
		iCorpValue = kOwner.AI_corporationValue(eCorporation, this);
		
		for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); iCorp++)
		{
			if (iCorp != eCorporation)
			{
				if (kOwner.hasHeadquarters((CorporationTypes)iCorp))
				{
					if (GC.getGame().isCompetingCorporation(eCorporation, (CorporationTypes)iCorp))
					{
						if (kOwner.AI_corporationValue((CorporationTypes)iCorp, this) > iCorpValue)
						{
							iCorpValue = -1;
							break;											
						}
						else
						{
							if (!isHasCorporation((CorporationTypes)iCorp))
							{
								iCorpValue = -1;
							}
						}
					}
				}
			}
		}
		
		iResult += iCorpValue / 25;
	}
	
	if (iCorpValue >= 0)//Don't build if it'll hurt us.
	{
		if (kBuilding.getGlobalCorporationCommerce() != NO_CORPORATION)
		{
			int iGoldValue = (GC.getCorporationInfo((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())).getHeadquarterCommerce(iI) * GC.getGameINLINE().countCorporationLevels((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())) * 2);
			
			iGoldValue += GC.getCorporationInfo((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())).getHeadquarterCommerce(iI);
			if (iGoldValue > 0)
			{
				iGoldValue += 2 + (kOwner.getNumCities() / 4); 
				iGoldValue += std::min(iGoldValue, getBuildingCommerce((CommerceTypes)iI) / 2) / 2;							
			}
			iGoldValue *= 2;
			iGoldValue *= getTotalCommerceRateModifier((CommerceTypes)iI);
			iGoldValue *= std::max(50, getTotalCommerceRateModifier((CommerceTypes)iI) - 150);
			iGoldValue /= 5000;
			iCorpValue += iGoldValue;
		}
	}
	
	if (iCorpValue > 0)
	{
		if (kOwner.isNoCorporations())
		{
			iCorpValue /= 2;
		}
		iResult += iCorpValue;
	}

	if (kBuilding.isCommerceFlexible(iI))
	{
		if (!(kOwner.isCommerceFlexible((CommerceTypes)iI)))
		{
			iResult += 40;
		}
	}

	if (kBuilding.isCommerceChangeOriginalOwner(iI))
	{
		if ((kBuilding.getCommerceChange(iI) > 0) || 
			(kBuilding.getCommercePerPopChange(iI) > 0) ||
			(kBuilding.getObsoleteSafeCommerceChange(iI) > 0))
		{
			iResult++;
		}
	}
	
	if ( iI == COMMERCE_GOLD && kOwner.AI_isFinancialTrouble())
	{
		iResult *= 2;
	}
	
	if (iI == COMMERCE_RESEARCH && kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE1))
	{
		iResult *= 3;		
		iResult /= 2;
	}

	//	If we are desperate for SOME culture boost non-trivial producers
	// cppcheck-suppress knownConditionTrueFalse
	if (iI == COMMERCE_CULTURE && getCommerceRate(COMMERCE_CULTURE) == 0 && AI_calculateTargetCulturePerTurn() == 1)
	{
		if (iResult >= 3)
		{
			iResult += 7;
		}						
	}

	//	For research if this is a significant boost to our total (civ wide) then give it significant
	//	extra weight
	if ( iI == COMMERCE_RESEARCH )
	{
		int iPlayerTotal = GET_PLAYER(getOwnerINLINE()).getCommerceRate(COMMERCE_RESEARCH);

		if ( iPlayerTotal <= 0 )
		{
			iPlayerTotal = 1;
		}

		//	iResult should be approx 8*<research gain> so
		//	we are adding a multiplier based on the proportion of the total
		//	doubling our total will multiply by 9 (+= 8*)
		iResult += (iResult*iResult)/iPlayerTotal;
	}

	return iResult;
}

int CvCityAI::tradeRouteValue(CvBuildingInfo& kBuilding, YieldTypes eYield, bool bForeignTrade) const
{
	PROFILE_FUNC();

	int	iExtraTradeRoutes = kBuilding.getTradeRoutes();
	int iCurrentTradeRoutes = getTradeRoutes();
	CvPlayerAI&	kOwner = GET_PLAYER(getOwnerINLINE());

	int	iTradeRouteValue = 0;
	if ( iCurrentTradeRoutes <= -iExtraTradeRoutes )
	{
		iTradeRouteValue = 0;
	}
	else if ( iCurrentTradeRoutes > 0 )
	{
		iTradeRouteValue = getTradeYield(eYield)/iCurrentTradeRoutes;
	}
	else
	{
		CvCity* pCity = kOwner.getCapitalCity();
		if (!pCity)
		{
			int iLoop;
			pCity = kOwner.firstCity(&iLoop);
		}
		
		iTradeRouteValue = calculateTradeYield(eYield, calculateTradeProfit(pCity));

		if (kOwner.isNoForeignTrade())
		{
			iTradeRouteValue /= 3;
		}
	}

	int iValue = std::min(iExtraTradeRoutes, iCurrentTradeRoutes+iExtraTradeRoutes)*4*iTradeRouteValue;
	iValue += (kBuilding.getCoastalTradeRoutes() * kOwner.countNumCoastalCities()*4*iTradeRouteValue);
	iValue += (kBuilding.getGlobalTradeRoutes() * kOwner.getNumCities()*4*iTradeRouteValue);
	
	int iUnmodifiedCurrentValue = (100*getTradeYield(eYield))/(100+getTradeRouteModifier());

	iValue += ((kBuilding.getTradeRouteModifier() * iUnmodifiedCurrentValue) / (bForeignTrade ? 12 : 25));
	if (bForeignTrade)
	{
		iValue += ((kBuilding.getForeignTradeRouteModifier() * iUnmodifiedCurrentValue) / 12);
	}
		
	return iValue;
#ifdef OLD_TRADE_ROUTE_CALCULATION
	iTempValue = ((kBuilding.getTradeRoutes() * ((8 * std::max(0, (totalTradeModifier() + 100))) / 100))
					* (getPopulation() / 5 + 1));
	int iGlobalTradeValue = (((6 * iTotalPopulation) / 5) / iNumCities);
	iTempValue += (kBuilding.getCoastalTradeRoutes() * kOwner.countNumCoastalCities() * iGlobalTradeValue);
	iTempValue += (kBuilding.getGlobalTradeRoutes() * iNumCities * iGlobalTradeValue);
	
	iTempValue += ((kBuilding.getTradeRouteModifier() * getTradeYield(YIELD_COMMERCE)) / (bForeignTrade ? 12 : 25));
	if (bForeignTrade)
	{
		iTempValue += ((kBuilding.getForeignTradeRouteModifier() * getTradeYield(YIELD_COMMERCE)) / 12);
	}
		
	if (bFinancialTrouble)
	{
		iTempValue *= 2;
	}
		
	if (kOwner.isNoForeignTrade())
	{
		iTempValue /= 3;
	}
#endif
}

//	Evaluate a building we are considering building here in terms of its
//	effect on properties
int CvCityAI::buildingPropertiesValue(CvBuildingInfo& kBuilding) const
{
	//	Evaluate building properties
	std::map<int,int> effectivePropertyChanges;

	CvPropertyManipulators* pBuildingPropertyManipulators = kBuilding.getPropertyManipulators();
	int num = pBuildingPropertyManipulators->getNumSources();
	for (int iI = 0; iI < num; iI++)
	{
		CvPropertySource* pSource = pBuildingPropertyManipulators->getSource(iI);

		//	For now we're only interested in constant sources
		//	TODO - expand this as buildings add other types
		if ( pSource->getType() == PROPERTYSOURCE_CONSTANT )
		{
			//	Convert to an effective absolute amount by looking at the steady state value
			//	given current
			PropertyTypes eProperty = pSource->getProperty();
			//	Only count half the unit source as we want to encourage building sources over unit ones
			int iCurrentSourceSize = getTotalBuildingSourcedProperty(eProperty) + getTotalUnitSourcedProperty(eProperty)/2 + getPropertyNonBuildingSource(eProperty);
			int iNewSourceSize = iCurrentSourceSize + static_cast<CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObjectConst());
			int iDecayPercent = getPropertyDecay(eProperty);

			//	Steady state occurs at a level where the decay removes as much per turn as the sources add
			//	Decay can be 0 if the current level is below the threshold at which decay cuts in, so for the
			//	purposes of calculation just treat this as very slow decay
			int	iCurrentSteadyStateLevel = (100*iCurrentSourceSize)/std::max(1,iDecayPercent);
			int	iNewSteadyStateLevel = (100*iNewSourceSize)/std::max(1,iDecayPercent);

			std::map<int,int>::iterator itr = effectivePropertyChanges.find(eProperty);
			if ( itr == effectivePropertyChanges.end() )
			{
				effectivePropertyChanges[eProperty] = (iNewSteadyStateLevel - iCurrentSteadyStateLevel);
			}
			else
			{
				itr->second += (iNewSteadyStateLevel - iCurrentSteadyStateLevel);
			}
		}
	}

	const CvProperties* buildingProperties = kBuilding.getProperties();
	num = buildingProperties->getNumProperties();
	for (int iI = 0; iI < num; iI++)
	{
		PropertyTypes eProperty = (PropertyTypes)buildingProperties->getProperty(iI);
		int	iBuildingValue = buildingProperties->getValue(iI);

		std::map<int,int>::iterator itr = effectivePropertyChanges.find(eProperty);
		if ( itr == effectivePropertyChanges.end() )
		{
			effectivePropertyChanges[eProperty] = iBuildingValue;
		}
		else
		{
			itr->second += iBuildingValue;
		}
	}

	int iValue = 0;
	for(std::map<int,int>::const_iterator itr = effectivePropertyChanges.begin(); itr != effectivePropertyChanges.end(); ++itr)
	{
		iValue += getPropertySourceValue((PropertyTypes)itr->first, itr->second)/100;
	}

	return iValue;
}

int CvCityAI::getPropertySourceValue(PropertyTypes eProperty, int iSourceValue) const
{
	const CvProperties* cityProperties = getPropertiesConst();
	CvPropertyInfo& kProperty = GC.getPropertyInfo(eProperty);
	int	iOperationalLow = kProperty.getOperationalRangeMin();
	int	iOperationalHigh = kProperty.getOperationalRangeMax();
	int iCurrentValue = cityProperties->getValueByProperty(eProperty);
	int iWouldBeValue = iCurrentValue + iSourceValue;

	//	Normalize to operation range
	int iCurrentNormalizedValue = ((range(iCurrentValue, iOperationalLow, iOperationalHigh) - iOperationalLow)*100)/(iOperationalHigh - iOperationalLow);
	int iWouldBeNormalizedValue = ((range(iWouldBeValue, iOperationalLow, iOperationalHigh) - iOperationalLow)*100)/(iOperationalHigh - iOperationalLow);

	int iValue = kProperty.getAIWeight()*(iWouldBeNormalizedValue - iCurrentNormalizedValue);

	//	Increase value as we move towards the nasty end of the range (cautious approach)
	int nastiness;

	if ( kProperty.getAIWeight() > 0 )
	{
		nastiness = 100 - std::min(iCurrentNormalizedValue, iWouldBeNormalizedValue);
	}
	else
	{
		nastiness = std::max(iCurrentNormalizedValue, iWouldBeNormalizedValue);
	}

	iValue *= (nastiness+100);
	iValue /= 100;

	switch(kProperty.getAIScaleType())
	{
		case AISCALE_CITY:
			break;
		case AISCALE_AREA:
			iValue *= area()->getCitiesPerPlayer(getOwnerINLINE());
			break;
		case AISCALE_PLAYER:
			iValue *= GET_PLAYER(getOwnerINLINE()).getNumCities();
			break;
		case AISCALE_TEAM:
			iValue *= GET_TEAM(getTeam()).getNumCities();
			break;
	}

	return iValue;
}

int CvCityAI::getPropertyDecay(PropertyTypes eProperty) const
{
	CvPropertyInfo& kProperty = GC.getPropertyInfo(eProperty);
	CvPropertyManipulators* pManipulators = kProperty.getPropertyManipulators();
	int	iTotalDecay = 0;
	int current = getPropertiesConst()->getValueByProperty(eProperty);
	int iLowestThresholdDecay = 0;

	for(int iI = 0; iI < pManipulators->getNumSources(); iI++)
	{
		CvPropertySource* pSource = pManipulators->getSource(iI);

		if ( pSource->getType() == PROPERTYSOURCE_DECAY )
		{
			CvPropertySourceDecay* pDecaySource = static_cast<CvPropertySourceDecay*>(pSource);

			//	For AI evaluation purposes the no-decay threshold is largely
			//	irrelevant since we are only interested in EVENTUAL equilibrium
			//	points.  If there are multiple decay sources we should count
			//	only those in current force OR the single lowest is none are
			int iNoDecayThreshold = pDecaySource->getNoDecayAmount();
			if ( current > iNoDecayThreshold )
			{
				iTotalDecay += pDecaySource->getPercent();
			}
			else if ( iTotalDecay == 0 )
			{
				iLowestThresholdDecay = pDecaySource->getPercent();
			}
		}
	}

	if ( iTotalDecay == 0 )
	{
		iTotalDecay = iLowestThresholdDecay;
	}

	return iTotalDecay;
}

int CvCityAI::getPropertyNonBuildingSource(PropertyTypes eProperty) const
{
	CvPropertyInfo& kProperty = GC.getPropertyInfo(eProperty);
	CvPropertyManipulators* pManipulators = kProperty.getPropertyManipulators();
	int	iTotal = 0;
	int current = getPropertiesConst()->getValueByProperty(eProperty);

	for(int iI = 0; iI < pManipulators->getNumSources(); iI++)
	{
		CvPropertySource* pSource = pManipulators->getSource(iI);

		if ( pSource->getType() == PROPERTYSOURCE_ATTRIBUTE_CONSTANT &&
			 pSource->getObjectType() == GAMEOBJECT_CITY &&
			 pSource->getProperty() == eProperty )
		{
			CvPropertySourceAttributeConstant* pCitySource = static_cast<CvPropertySourceAttributeConstant*>(pSource);

			iTotal += pCitySource->getSourcePredict(getGameObjectConst(), current);
		}
	}

	//	Add in contribution from specialists
	for(int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		int iNum = getSpecialistCount((SpecialistTypes)iI) + getFreeSpecialistCount((SpecialistTypes)iI);

		if ( iNum > 0 )
		{
			CvPropertyManipulators* pMani = GC.getSpecialistInfo((SpecialistTypes)iI).getPropertyManipulators();
			int iContribution = 0;

			for (int i=0; i < pMani->getNumSources(); i++)
			{
				CvPropertySource* pSource = pMani->getSource(i);

				//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
				//	to the city for most properties anyway
				if (pSource->getType() == PROPERTYSOURCE_CONSTANT &&
					(pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT) &&
					pSource->getProperty() == eProperty)
				{
					iContribution += static_cast<CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObjectConst());
				}
			}

			iTotal += iNum*iContribution;
		}
	}

	//	Add oplayer-based/global sources (population, traits, handicap, ...)
	iTotal += getGlobalSourcedProperty(eProperty);

	return iTotal;
}

int CvCityAI::AI_getBuildPriority() const
{
	return m_iBuildPriority;
}

bool CvCityAI::AI_choosePropertyControlUnit(int iTriggerPercentOfPropertyOpRange, PropertyTypes pProperty)
{
	if (AI_isPropertyControlBuildingQueued())
	{
		return false;
	}

	bool bSuccessful = false;
	bool bAnySuccessful = false;
	CvPlot* pPlot = plot();
	int iGameTurn = GC.getGameINLINE().getGameTurn();

	for( int iI= 0; iI < GC.getNumPropertyInfos(); iI++)
	{
		PropertyTypes eProperty = (PropertyTypes)iI;
		
		if (pProperty == NO_PROPERTY || pProperty == eProperty)
		{
			if ( GC.getPropertyInfo(eProperty).getAIWeight() != 0 && GC.getPropertyInfo(eProperty).isSourceDrain() )
			{
				int iCurrentValue = getPropertiesConst()->getValueByProperty(eProperty);
				int iOperationRangeMin = GC.getPropertyInfo(eProperty).getOperationalRangeMin();
				int iOperationRangeMax = GC.getPropertyInfo(eProperty).getOperationalRangeMax();
				int iCurrentPercent = ((iCurrentValue - iOperationRangeMin)*100)/(iOperationRangeMax - iOperationRangeMin);

				int iEval = getPropertyNeed(eProperty);
				//
				int iTrainReluctance = GC.getPropertyInfo(eProperty).getTrainReluctance();
				int iCheck = iTriggerPercentOfPropertyOpRange;
				iEval /= iCheck;
				iCheck *= iTrainReluctance;
				//TBNote: May still need to take a count of units ordered... there's only so many units the AI should be willing to train at once for some tasks.  I'm thinking of polution control here.
				//	Don't bother trying to build units for hopelessly out-of-control properties
				//	or else we'll spam units endlessly in cases we cannot really control
				int iLimit = 500 * (1+(int)GET_PLAYER(getOwner()).getCurrentEra());
				if (iTrainReluctance > 0)
				{
					iLimit /= iTrainReluctance;
				}
				if (iEval > iCheck && iCurrentPercent < iLimit )
				{
					//	Order a suitable unit if possible
					CvUnitSelectionCriteria criteria;
					CvString	str;

					criteria.m_eProperty = eProperty;
					criteria.m_bPropertyBeneficial = true;

					//eventually let's get coastal cities ordering some coast guard contracts.
					if (AI_isNavalMilitaryProductionCity())
					{
						if ( m_iUnitSummonedLastEstablishedTurn != iGameTurn )
						{
							int iResponders = GET_PLAYER(getOwnerINLINE()).getContractBroker().numRequestsOutstanding(UNITAI_PROPERTY_CONTROL_SEA, false, pPlot);
							if (iResponders < 1)
							{	
								str.Format("Property control for %S", GC.getPropertyInfo(eProperty).getDescription());
								bSuccessful = AI_chooseUnitImmediate(str.c_str(), UNITAI_PROPERTY_CONTROL_SEA, &criteria);
								if (bSuccessful)
								{
									bAnySuccessful = true;
									m_iUnitSummonedLastEstablishedTurn = iGameTurn;
								}
							}
						}
					}
					if (!bSuccessful)
					{
						//First try for a building that's quick to build and has a
						int iMaxTurns = (5*GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getConstructPercent())/100;
						if (AI_chooseBuilding(BUILDINGFOCUS_PROPERTY, iMaxTurns, 0, -1, true, eProperty))
						{
							if( gCityLogLevel >= 2 ) logBBAI("      City %S selects a property control building", getName().GetCString());
							AI_setPropertyControlBuildingQueued(true);
							return true;
						}

						if ( m_iUnitSummonedLastEstablishedTurn != iGameTurn )
						{
							int iResponders = GET_PLAYER(getOwnerINLINE()).getContractBroker().numRequestsOutstanding(UNITAI_PROPERTY_CONTROL, false, pPlot);
							if (iResponders < 2)//if there aren't already a couple or more property control units being trained and/or on the way
							{
								//then lets order the best type we can get from any city we have:
								str.Format("Contracts Property control for %S", GC.getPropertyInfo(eProperty).getDescription());
								bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_PROPERTY_CONTROL, -1, -1, HIGH_PRIORITY_ESCORT_PRIORITY, &criteria);
							}
							else
							{
								//otherwise, let's see what we can build here in this city and let's train that.
								//if this causes us to overtrain a little then it won't hurt too much and should only happen because the city is super effective.
								int iUnitValue = 0;
								UnitTypes eBestUnit = AI_bestUnitAI(UNITAI_PROPERTY_CONTROL, iUnitValue, false, false, &criteria);
								if (eBestUnit != NO_UNIT)
								{
									str.Format("Local Property control for %S", GC.getPropertyInfo(eProperty).getDescription());
									bSuccessful = AI_chooseUnitImmediate(str.c_str(), UNITAI_PROPERTY_CONTROL, &criteria, eBestUnit);
								}
							}
							if (bSuccessful)
							{
								bAnySuccessful = true;
								m_iUnitSummonedLastEstablishedTurn = iGameTurn;
							}
						}
					}
				}
			}
		}
	}

	return bAnySuccessful;
}

bool CvCityAI::AI_chooseHealerUnit(int iMinNeeded)
{
	int iBestUnitCombatValue = 0;
	UnitCombatTypes eBestUnitCombat = NO_UNITCOMBAT;
	int iSecondBestUnitCombatValue = 0;
	UnitCombatTypes eSecondBestUnitCombat = NO_UNITCOMBAT;
	DomainTypes eBestDomain = NO_DOMAIN;
	DomainTypes eSecondBestDomain = NO_DOMAIN;

	for( int iI= 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)iI).isHealsAs())
		{
			UnitCombatTypes eUnitCombat = (UnitCombatTypes)iI;
			PlayerTypes ePlayer = getOwner();
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);
			int iOverloadCount = 0;
			int iOverloadCountLand = 0;
			int iOverloadCountSea = 0;
			int iCount = 0;
			int iCountLand = 0;
			int iCountSea = 0;
			CvPlot* pLoopPlot;
			for (int iJ = 0; iJ < NUM_CITY_PLOTS_1; iJ++)
			{
				pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iJ);
				if (pLoopPlot != NULL)
				{
					for (int iK = 0; iK < NUM_DOMAIN_TYPES; iK++)
					{
						DomainTypes eDomain = (DomainTypes)iK;
						int iOverload = pLoopPlot->getOverloadedUnitCombatsSupportedByHealer(ePlayer, eUnitCombat, eDomain);
						iOverloadCount += iOverload;
						if (eDomain == DOMAIN_LAND)
						{
							iOverloadCountLand += iOverload;
						}
						else if (eDomain == DOMAIN_SEA)
						{
							iOverloadCountSea += iOverload;
						}
						else if (eDomain == DOMAIN_AIR)
						{
							if (pLoopPlot->isWater())
							{
								iOverloadCountSea += iOverload;
							}
							else
							{
								iOverloadCountLand += iOverload;
							}
						}

						int iResult = pLoopPlot->getUnitCombatsUnsupportedByHealer(ePlayer, eUnitCombat, eDomain);
						iCount += iResult;
						if (eDomain == DOMAIN_LAND)
						{
							iCountLand += iResult;
						}
						else if (eDomain == DOMAIN_SEA)
						{
							iCountSea += iResult;
						}
						else if (eDomain == DOMAIN_AIR)
						{
							if (pLoopPlot->isWater())
							{
								iCountSea += iResult;
							}
							else
							{
								iCountLand += iResult;
							}
						}
					}
				}
			}
			iCount -= iOverloadCount;
			iCountSea -= iOverloadCountSea;
			iCountLand -= iOverloadCountLand;
			if (iCount >= iMinNeeded)
			{
				if (iCount > iBestUnitCombatValue)
				{
					iBestUnitCombatValue = iCount;
					eBestUnitCombat = eUnitCombat;
					if (iCountLand >= iCountSea)
					{
						eBestDomain = DOMAIN_LAND;
					}
					else
					{
						eBestDomain = DOMAIN_SEA;
					}
				}
				if ((iCount > iSecondBestUnitCombatValue) && iCount < iBestUnitCombatValue)
				{
					iSecondBestUnitCombatValue = iCount;
					eSecondBestUnitCombat = eUnitCombat;
					if (iCountLand >= iCountSea)
					{
						eSecondBestDomain = DOMAIN_LAND;
					}
					else
					{
						eSecondBestDomain = DOMAIN_SEA;
					}
				}
			}
		}
	}
	bool bSuccessful = false;
	if (eBestUnitCombat != NO_UNITCOMBAT)
	{
		//	Order a suitable unit if possible
		CvUnitSelectionCriteria criteria;
		CvString	str;

		criteria.m_eHealUnitCombat = eBestUnitCombat;
		criteria.m_bIsHealer = true;

		if (eBestDomain == DOMAIN_SEA && AI_isNavalMilitaryProductionCity())
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER_SEA, -1, -1, -1, &criteria);
		}
		if (!bSuccessful && eBestDomain == DOMAIN_LAND)
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER, -1, -1, -1, &criteria);
		}
	}
	if (!bSuccessful && eSecondBestUnitCombat != NO_UNITCOMBAT)
	{
		//	Order a suitable unit if possible
		CvUnitSelectionCriteria criteria;
		CvString	str;

		criteria.m_eHealUnitCombat = eSecondBestUnitCombat;
		criteria.m_bIsHealer = true;

		if (eSecondBestDomain == DOMAIN_SEA && AI_isNavalMilitaryProductionCity())
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eSecondBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER_SEA, -1, -1, -1, &criteria);
		}
		if (!bSuccessful && eSecondBestDomain == DOMAIN_LAND)
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eSecondBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER, -1, -1, -1, &criteria);
		}
	}
	return bSuccessful;
}

int CvCityAI::happynessValue(int iAddedHappyness, int iBaseHappinessLevel, int iBaseHealthLevel)
{
	bool bIsNegative = false;

	if (iAddedHappyness < 0)
	{
		bIsNegative = true;
		iAddedHappyness = -iAddedHappyness;
	}

	int iValue = 0;

	if (iAddedHappyness != 0)
	{
		int iTempHealthLevel = iBaseHealthLevel - (getEspionageHealthCounter() / 2) + std::max(0, iAddedHappyness);
		int iTempHappinessLevel = iBaseHappinessLevel - (getEspionageHappinessCounter() / 2);
		int iTempCurrentHappinessLevel = iBaseHappinessLevel;
		int iTempHappyChangeValue = 0;
		for (int iI = 0; iI < iAddedHappyness; iI++)
		{
			if (bIsNegative) { iTempHappinessLevel--; iTempCurrentHappinessLevel--; iTempHappyChangeValue += 1;}
			
			//The Value of Happiness
			if (iTempHappinessLevel < 0)
			{
				iTempHappyChangeValue += 10;
				if (bIsNegative) {
					iTempHappyChangeValue += 9;
					if (iTempCurrentHappinessLevel < 0)
					{
						iTempHappyChangeValue += 10;
					}								
				} 
			}
			else if (iTempHappinessLevel <= iTempHealthLevel && iTempHappinessLevel <= 6) {
				iTempHappyChangeValue += 6;
				if (bIsNegative) { iTempHappyChangeValue += 3;}
			}
			else if (iTempHappinessLevel < 10 ) { iTempHappyChangeValue += 3; }
			else { iTempHappyChangeValue += 1; }
			
			if (!bIsNegative) { iTempHappinessLevel++; iTempCurrentHappinessLevel++; }
		}
		
		if (bIsNegative)
		{
			iValue -= iTempHappyChangeValue;

			//	If we're already unhappy then exach extar unhappy will force
			//	use to remove a worked plot as well
			if ( iBaseHappinessLevel <= 0 )
			{
				iValue -= (iAddedHappyness*worstWorkedPlotValue())/100;
			}
		}
		else 
		{
			iValue += iTempHappyChangeValue;
		}
	}

	return iValue;
}

int CvCityAI::healthValue(int iAddedHealth, int iUseHappinessLevel, int iBaseHealthLevel, int iBaseFoodDifference)
{
	bool bIsNegative = false;
	if (iAddedHealth < 0)
	{
		bIsNegative = true;
		iAddedHealth = -iAddedHealth;
	}
	int iValue = 0;
	if (iAddedHealth != 0)
	{
		int iTempHappinessLevel = iUseHappinessLevel;
		int iTempHealthLevel = (goodHealth() - badHealth(/*bNoAngry*/ false, std::max(0, (iTempHappinessLevel + 1) / 2))) + ((getEspionageHealthCounter() + 1) / 2 ); 
		int iTempCurrentHealthLevel = iBaseHealthLevel;
		int iTempHealthChangeValue = 0;
		for (int iI = 0; iI < iAddedHealth; iI++)
		{
			if (bIsNegative) { iTempHealthLevel--; iTempCurrentHealthLevel--; iTempHealthChangeValue += 1;}
		
			//Health Values
			if (iTempCurrentHealthLevel < 0) {
				iTempHealthChangeValue += 10;
				if (bIsNegative) {
					iTempHealthChangeValue += 9;
					if(std::max(0,iBaseFoodDifference-1) < -iTempCurrentHealthLevel)
					{
						iTempHealthChangeValue += 10;
					}									
				}
			}
			else if (iTempHealthLevel < 0) { iTempHealthChangeValue += 8; }
			else if (iTempHealthLevel < iTempHappinessLevel && iTempHealthLevel <= 4) {
				iTempHealthChangeValue += 4;
				if (bIsNegative) { iTempHealthChangeValue += 1;}
			}
			else if (iTempHealthLevel < 8 ) { iTempHealthChangeValue += 2; }
			else if (!bIsNegative) { iTempHealthChangeValue += 1; }
		
			if (!bIsNegative) { iTempHealthLevel++; iTempCurrentHealthLevel++; }
		}
		if (bIsNegative)
		{
			iValue -= iTempHealthChangeValue;
		}
		else
		{
			iValue += iTempHealthChangeValue;
		}
	}

	return iValue;
}

int CvCityAI::worstWorkedPlotValue()
{
	int iWorstPlotValue = MAX_INT;

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			if (isWorkingPlot(iI))
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iI);

				if (pLoopPlot != NULL)
				{
					int iValue = AI_plotValue(pLoopPlot, false, /*bRemove*/ false, /*bIgnoreFood*/ false, false);

					if (iValue < iWorstPlotValue)
					{
						iWorstPlotValue = iValue;
					}
				}
			}
		}
	}

	return (iWorstPlotValue == MAX_INT ? 0 : iWorstPlotValue);
}

bool CvCityAI::AI_establishSeeInvisibleCoverage()
{
	CvPlot* pPlot = plot();

	if (pPlot != NULL)
	{
		for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
		{
			UnitTypes eBestUnit = NO_UNIT;
			int iUnitValue = 0;
			InvisibleTypes eVisible = (InvisibleTypes)iI;
			CvUnitSelectionCriteria criteria;
			criteria.m_eVisibility = eVisible;
			criteria.m_bNoNegativeProperties = true;

			//continue to use this best unit method so as to establish a rare coastal response.
			//if (plot()->isCoastalLand())
			//{
			//	if (!isCachedBestSeeInvisibleUnit(eVisible, true))
			//	{ 
			//		eBestUnit = AI_bestUnitAI(UNITAI_SEE_INVISIBLE_SEA, iUnitValue, false, false, &criteria);
			//		setBestSeeInvisibleUnitCache(eVisible, eBestUnit, true);
			//	}
			//	else
			//	{
			//		eBestUnit = getCachedBestSeeInvisibleUnit(eVisible, true);
			//	}
			//	
			//	if (eBestUnit != NULL)
			//	{
			//		iResponders = GET_PLAYER(getOwnerINLINE()).getContractBroker().numRequestsOutstanding(UNITAI_SEE_INVISIBLE_SEA, false, eBestUnit, pPlot);
			//		iResponders += pPlot->countSeeInvisibleActive(getOwner(), eVisible);
			//		if (iResponders < 1)
			//		{			
			//			//end result
			//			if (AI_chooseUnit("See Invisible Sea needed", UNITAI_SEE_INVISIBLE_SEA, -1, -1, -1, &criteria, eBestUnit) )
			//			{
			//				return true;
			//			}
			//		}
			//	}
			//}
			//if (!isCachedBestSeeInvisibleUnit(eVisible))
			//{ 
			//	eBestUnit = AI_bestUnitAI(UNITAI_SEE_INVISIBLE, iUnitValue, false, false, &criteria);
			//	setBestSeeInvisibleUnitCache(eVisible, eBestUnit);
			//}
			//else
			//{
			//	eBestUnit = getCachedBestSeeInvisibleUnit(eVisible);
			//}
			//
			//if (eBestUnit != NO_UNIT)
			//{
			int iResponders = GET_PLAYER(getOwnerINLINE()).getContractBroker().numRequestsOutstanding(UNITAI_SEE_INVISIBLE, false, pPlot);//This will tend to space out the AI's sense of immediate need to prepare for each type.
			iResponders += pPlot->countSeeInvisibleActive(getOwner(), eVisible);
			if (iResponders < 1)
			{			
				//end result
				if (AI_chooseUnit("See Invisible needed", UNITAI_SEE_INVISIBLE, -1, -1, -1, &criteria) )
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CvCityAI::AI_establishInvestigatorCoverage()
{
	//Fox in the henhouse protocol
	int iNumLocalCriminals = plot()->getNumCriminals();
	CvPlot* pPlot = plot();
	CvUnitSelectionCriteria criteria;
	criteria.m_bNoNegativeProperties = true;
	int iLocalInvestigators = GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plot(), MISSIONAI_INVESTIGATOR_MAINTAIN, NULL, 0);//MISSIONAI_INVESTIGATOR_MAINTAIN is probably not working as designed due to the way the contract broker works.  Once answered, the unit can really only be tracked by its AI type as mission type has been generically set.
	iLocalInvestigators += pPlot->getNumPlayerUnitAI(UNITAI_INVESTIGATOR, getOwnerINLINE());
	iLocalInvestigators += GET_PLAYER(getOwnerINLINE()).getContractBroker().numRequestsOutstanding(UNITAI_INVESTIGATOR, false, pPlot);

	if (iNumLocalCriminals > 0 && pPlot != NULL)//Yes, the first round the city has trained its first criminal it will run into this, even if no spawns ever occur.  Can never be too ready, right?
	{
		//First try for a building that's quick to build and has investigation
		int iMaxTurns = iNumLocalCriminals * 5;
		iMaxTurns *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getConstructPercent();
		iMaxTurns /= 100;
		if (AI_chooseBuilding(BUILDINGFOCUS_INVESTIGATION, iMaxTurns, 0, -1, true))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S selects an investigation enhancing building", getName().GetCString());
			return true;
		}
		else if (iLocalInvestigators < iNumLocalCriminals)
		{
			if (iLocalInvestigators < 1)
			{
				if (AI_chooseUnit("Investigator needed", UNITAI_INVESTIGATOR, -1, -1, -1, &criteria) )
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CvCityAI::AI_isNegativePropertyUnit(UnitTypes eUnit)
{
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	bool bAnswer = false;
	CvPropertyManipulators* propertyManipulators = GC.getUnitInfo(eUnit).getPropertyManipulators();
	if (propertyManipulators != NULL)
	{
		for(int iI = 0; iI < propertyManipulators->getNumSources(); iI++)
		{
			CvPropertySource* pSource = propertyManipulators->getSource(iI);
			//	We have a source for a property - value is crudely just the AIweight of that property times the source size (which is expected to only depend on the player)
			PropertyTypes eProperty = pSource->getProperty();

			if ( pSource->getType() == PROPERTYSOURCE_CONSTANT)
			{
				if (GC.getPropertyInfo(eProperty).getAIWeight() < 0)
				{
					int iAmount = static_cast<CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObjectConst());
					if ( iAmount > 0 )
					{
						bAnswer = true;
						break;
					}
				}
			}
		}
	}
	return bAnswer;
}

bool CvCityAI::AI_meetsUnitSelectionCriteria(UnitTypes eUnit, CvUnitSelectionCriteria* criteria)
{
	//Add more here as needs demand - Some cleanup could be nice too.  Consolidate some of the other checks and possible redundancies into this location?
	if (eUnit != NO_UNIT && criteria != NULL)
	{
		CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);
		//No Negative Properties
		if (criteria->m_bNoNegativeProperties)
		{
			if (AI_isNegativePropertyUnit(eUnit))
			{
				return false;
			}
		}
		//See Invisible type match
		if (criteria->m_eUnitAI == UNITAI_SEE_INVISIBLE || criteria->m_eUnitAI == UNITAI_SEE_INVISIBLE_SEA)
		{
			InvisibleTypes eVisibilityRequested = criteria->m_eVisibility;
			if (GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
			{
				if (kUnitInfo.getVisibilityIntensityType(eVisibilityRequested) <= 0)
				{
					return false;
				}
			}
			else
			{
				bool bFound = false;
				for (int iI = 0; iI < kUnitInfo.getNumSeeInvisibleTypes(); ++iI)
				{
					if (kUnitInfo.getSeeInvisibleType(iI) == (int)eVisibilityRequested)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					return false;
				}
			}
			if (kUnitInfo.isFound())
			{
				return false;
			}
		}
	}
	return true;
}