// selectionGroup.cpp

#include "CvGameCoreDLL.h"
#include "CvReachablePlotSet.h"

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/30/10                                jdog5000      */
/*                                                                                              */
/* AI Logging                                                                                   */
/************************************************************************************************/
#include "BetterBTSAI.h"
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

CvSelectionGroup* CvSelectionGroup::m_pCachedMovementGroup = NULL;
std::map<int,edgeCosts>* CvSelectionGroup::m_pCachedNonEndTurnEdgeCosts = NULL;
std::map<int,edgeCosts>* CvSelectionGroup::m_pCachedEndTurnEdgeCosts = NULL;
CvPathGenerator*	CvSelectionGroup::m_generator = NULL;

// Public Functions...

CvSelectionGroup::CvSelectionGroup()
{
	reset(0, NO_PLAYER, true);
}


CvSelectionGroup::~CvSelectionGroup()
{
	uninit();
}


void CvSelectionGroup::init(int iID, PlayerTypes eOwner)
{
	//--------------------------------
	// Init saved data
	reset(iID, eOwner);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	AI_init();
}


void CvSelectionGroup::uninit()
{
	m_units.clear();

	m_missionQueue.clear();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvSelectionGroup::reset(int iID, PlayerTypes eOwner, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iMissionTimer = 0;

	m_bForceUpdate = false;
	m_bIsMidMove = false;

	m_bIsChoosingNewLeader = false;

	m_eOwner = eOwner;

	m_eActivityType = ACTIVITY_AWAKE;
	m_eAutomateType = NO_AUTOMATE;
	m_bIsBusyCache = false;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/19/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
	m_bIsStrandedCache = false;
	m_bIsStrandedCacheValid = false;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
// BUG - Safe Move - start
	m_bLastPathPlotChecked = false;
	m_bLastPlotVisible = false;
	m_bLastPlotRevealed = false;
// BUG - Safe Move - end


	if (!bConstructorCall)
	{
		AI_reset();
	}
}


void CvSelectionGroup::kill()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssertMsg(getID() != FFreeList::INVALID_INDEX, "getID() is not expected to be equal with FFreeList::INVALID_INDEX");
	FAssertMsg(getNumUnits() == 0, "The number of units is expected to be 0");

	GET_PLAYER(getOwnerINLINE()).removeGroupCycle(getID());

	GET_PLAYER(getOwnerINLINE()).deleteSelectionGroup(getID());
}

bool CvSelectionGroup::sentryAlert() const
{
	CvUnit* pHeadUnit = NULL;
	int iMaxRange = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		int iRange = pLoopUnit->visibilityRange() + 1;

		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
			pHeadUnit = pLoopUnit;
		}
	}

	if (NULL != pHeadUnit)
	{
		for (int iX = -iMaxRange; iX <= iMaxRange; ++iX)
		{
			for (int iY = -iMaxRange; iY <= iMaxRange; ++iY)
			{
				CvPlot* pPlot = ::plotXY(pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), iX, iY);
				if (NULL != pPlot)
				{
					if (pHeadUnit->plot()->canSeePlot(pPlot, pHeadUnit->getTeam(), iMaxRange - 1, NO_DIRECTION))
					{
						if (pPlot->isVisibleEnemyUnit(pHeadUnit))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
/*
 * Similar to sentryAlert() except only checks water/land plots based on the domain type of the head unit.
 */
bool CvSelectionGroup::sentryAlertSameDomainType() const
{
	int iMaxRange = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	int iIndex = -1;

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		int iRange = pLoopUnit->visibilityRange() + 1;

		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
			iIndex = getUnitIndex(pLoopUnit);
		}
	}

	CvUnit* pHeadUnit = ((iIndex == -1) ? NULL : getUnitAt(iIndex));
	if (NULL != pHeadUnit)
	{
		for (int iX = -iMaxRange; iX <= iMaxRange; ++iX)
		{
			for (int iY = -iMaxRange; iY <= iMaxRange; ++iY)
			{
				CvPlot* pPlot = ::plotXY(pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), iX, iY);
				if (NULL != pPlot)
				{
					if (pHeadUnit->plot()->canSeePlot(pPlot, pHeadUnit->getTeam(), iMaxRange - 1, NO_DIRECTION))
					{
						if (pPlot->isVisibleEnemyUnit(pHeadUnit))
						{
							if ((getDomainType() == DOMAIN_SEA) && (pPlot->isWater()))
							{
								return true;
							}
							else if ((getDomainType() == DOMAIN_LAND) && (!(pPlot->isWater())))
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
#endif
// BUG - Sentry Actions - end

void CvSelectionGroup::doTurn()
{
	PROFILE("CvSelectionGroup::doTurn()")

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iWaitTurns;
	int iBestWaitTurns;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/19/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
		invalidateIsStrandedCache();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

		bool bHurt = false;
		
		// do unit's turns (checking for damage)
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			//	Units can be forced out of plots due to war/culture changes.  If a group
			//	has a mix of visible/invisible or nationality-hidden units this can act
			//	differently on different units within the group, effectively splitting it
			if ( pLoopUnit->plot() != plot() )
			{
				pLoopUnit->joinGroup(NULL);
			}

			pLoopUnit->doTurn();

			if (pLoopUnit->isHurt())
			{
				bHurt = true;
			}
		}

		ActivityTypes eActivityType = getActivityType();

		//	If the unit has a mission queued but not yet started set its activity
		//	to ACTIVITY_MISSION as that best reflects what it will be doign once its automation is
		//	run
		if ( eActivityType == ACTIVITY_HOLD && headMissionQueueNode() != NULL )
		{
			setActivityType(ACTIVITY_MISSION);
		}
		// wake unit if skipped last turn 
		//		or healing and automated or no longer hurt (automated healing is one turn at a time)
		//		or on sentry and there is danger
		else if ((eActivityType == ACTIVITY_HOLD) ||
				((eActivityType == ACTIVITY_HEAL) && (AI_isControlled() || !bHurt)) ||
				((eActivityType == ACTIVITY_SENTRY) && (sentryAlert())))
		{
			setActivityType(ACTIVITY_AWAKE);
		}

// BUG - Sentry Healing and Explorering Units - start
		if (isHuman())
		{
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
			if (((eActivityType == ACTIVITY_SENTRY_NAVAL_UNITS) && (sentryAlertSameDomainType())) ||
				((eActivityType == ACTIVITY_SENTRY_LAND_UNITS) && (sentryAlertSameDomainType())) ||
				((eActivityType == ACTIVITY_SENTRY_WHILE_HEAL) && (sentryAlertSameDomainType() || AI_isControlled() || !bHurt)))
			{
				setActivityType(ACTIVITY_AWAKE);
			}
#endif
// BUG - Sentry Actions - end

// BUG - Sentry Exploring Units - start
			if (isAutomated() && getAutomateType() == AUTOMATE_EXPLORE && getBugOptionBOOL("Actions__SentryHealing", true, "BUG_SENTRY_HEALING") && sentryAlert())
			{
				if (!(getBugOptionBOOL("Actions__SentryHealingOnlyNeutral", true, "BUG_SENTRY_HEALING_ONLY_NEUTRAL") && plot()->isOwned()))
				{
					setActivityType(ACTIVITY_AWAKE);
				}
			}
// BUG - Sentry Exploring Units - end

// BUG - Sentry Healing Units - start
// AIAndy: This is pointless when there is the separate sentry while heal button
#ifndef _MOD_SENTRY
			if (eActivityType == ACTIVITY_HEAL && getBugOptionBOOL("Actions__SentryHealing", true, "BUG_SENTRY_HEALING") && sentryAlert())
			{
				if (!(getBugOptionBOOL("Actions__SentryHealingOnlyNeutral", true, "BUG_SENTRY_HEALING_ONLY_NEUTRAL") && plot()->isOwned()))
				{
					setActivityType(ACTIVITY_AWAKE);
				}
			}
#endif
		}
// BUG - Sentry Healing and Explorering Units - end
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      09/21/08                                jdog5000      */
/*                                                                                              */
/* Air AI                                                                                       */
/************************************************************************************************/
		// with improvements to launching air patrols, now can wake every turn
		if ( (eActivityType == ACTIVITY_INTERCEPT) && !isHuman() )
		{
			setActivityType(ACTIVITY_AWAKE);
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/	

		if (AI_isControlled())
		{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/20/09                                jdog5000      */
/*                                                                                              */
/* Unit AI, Efficiency                                                                          */
/************************************************************************************************/
			//if ((getActivityType() != ACTIVITY_MISSION) || (!canFight() && (GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 2) > 0)))
			if ((getActivityType() != ACTIVITY_MISSION) || (!canDefend() && (GET_PLAYER(getOwnerINLINE()).AI_getAnyPlotDanger(plot(), 2))))
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/	
			{
				setForceUpdate(true);
			}
		}
		else
		{
			if (getActivityType() == ACTIVITY_MISSION)
			{
				bool bNonSpy = false;
				for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					if (!pLoopUnit->isSpy())
					{
						bNonSpy = true;
						break;
					}
				}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/20/09                                jdog5000      */
/*                                                                                              */
/* Unit AI, Efficiency                                                                          */
/************************************************************************************************/
				//if (bNonSpy && GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 2) > 0)
				if (bNonSpy && GET_PLAYER(getOwnerINLINE()).AI_getAnyPlotDanger(plot(), 2))
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
				{
					clearMissionQueue();
				}
			}
		}

		if (isHuman())
		{
			if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
			{
				iBestWaitTurns = 0;

				pUnitNode = headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);

					iWaitTurns = (GC.getDefineINT("MIN_TIMER_UNIT_DOUBLE_MOVES") - (GC.getGameINLINE().getTurnSlice() - pLoopUnit->getLastMoveTurn()));

					if (iWaitTurns > iBestWaitTurns)
					{
						iBestWaitTurns = iWaitTurns;
					}
				}

				setMissionTimer(std::max(iBestWaitTurns, getMissionTimer()));

				if (iBestWaitTurns > 0)
				{
					// Cycle selection if the current group is selected
					CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
					if (pSelectedUnit && pSelectedUnit->getGroup() == this)
					{
						gDLL->getInterfaceIFace()->selectGroup(pSelectedUnit, false, false, false);
					}
				}
			}
		}
	}

	doDelayedDeath();
}

void CvSelectionGroup::resetHealing()
{
	PROFILE("CvSelectionGroup::resetHealing()")

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;		//reset all healers

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		pLoopUnit->setHealSupportUsed(0);
	}
}

bool CvSelectionGroup::showMoves() const
{
	if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || GC.getGameINLINE().isSimultaneousTeamTurns())
	{
		return false;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.isHuman())
			{
				CvUnit* pHeadUnit = getHeadUnit();

				if (NULL != pHeadUnit)
				{
					if (pHeadUnit->isEnemy(kLoopPlayer.getTeam()))
					{
						if (kLoopPlayer.isOption(PLAYEROPTION_SHOW_ENEMY_MOVES))
						{
							return true;
						}
					}
					else
					{
						if (kLoopPlayer.isOption(PLAYEROPTION_SHOW_FRIENDLY_MOVES))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


void CvSelectionGroup::updateTimers()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	bool bCombat;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		bCombat = false;

		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->isCombat())
			{
				if (pLoopUnit->isAirCombat())
				{
					pLoopUnit->updateAirCombat();
				}
				else
				{
					pLoopUnit->updateCombat();
				}

				bCombat = true;
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
				// Dale - SA: Stack Attack START
				if (!GC.isDCM_STACK_ATTACK())
				{
					break;
				}
				// Dale - SA: Stack Attack END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
			}
		}

		if (!bCombat)
		{
			updateMission();
		}
	}

	doDelayedDeath();
}


// Returns true if group was killed...
bool CvSelectionGroup::doDelayedDeath()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (isBusy())
	{
		return false;
	}

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && !GET_PLAYER(getOwnerINLINE()).isTempUnit(pLoopUnit) )
		{
			pLoopUnit->doDelayedDeath();
		}
	}

	if (getNumUnits() == 0)
	{
		kill();
		return true;
	}

	return false;
}


void CvSelectionGroup::playActionSound()
{
	// Pitboss should not be playing sounds!
#ifndef PITBOSS

	CvUnit *pHeadUnit;
	int iScriptId = -1;

	pHeadUnit = getHeadUnit();
	if ( pHeadUnit )
	{
		iScriptId = pHeadUnit->getArtInfo(0, GET_PLAYER(getOwnerINLINE()).getCurrentEra())->getActionSoundScriptId();
	}

	if ( (iScriptId == -1) && pHeadUnit )
	{
		CvCivilizationInfo *pCivInfo;
		pCivInfo = &GC.getCivilizationInfo( pHeadUnit->getCivilizationType() );
		if ( pCivInfo )
		{
			iScriptId = pCivInfo->getActionSoundScriptId();
		}
	}

	if ( (iScriptId != -1) && pHeadUnit )
	{
		CvPlot *pPlot = GC.getMapINLINE().plotINLINE(pHeadUnit->getX_INLINE(),pHeadUnit->getY_INLINE());
		if ( pPlot )
		{
			gDLL->Do3DSound( iScriptId, pPlot->getPoint() );
		}
	}

#endif // n PITBOSS
}


void CvSelectionGroup::pushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
{
	if ( eMission == MISSION_SKIP && eMissionAI == NO_MISSIONAI)
	{
		//	No longer targeting any mission - make sure we don't keep reciord of the fact that we were
		//	previously
		((CvSelectionGroupAI*)this)->AI_setMissionAI(NO_MISSIONAI,NULL,NULL);
	}
	pushMissionInternal(eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, pMissionAIPlot, pMissionAIUnit);
}

bool CvSelectionGroup::pushMissionInternal(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
{
	bool bResult = true;

	PROFILE_FUNC();

	MissionData mission;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (!bAppend)
	{
		if (isBusy())
		{
			return true;
		}

		clearMissionQueue();
	}

	if (bManual)
	{
		setAutomateType(NO_AUTOMATE);
	}

	mission.eMissionType = eMission;
	mission.iData1 = iData1;
	mission.iData2 = iData2;
	mission.iFlags = iFlags;
	mission.iPushTurn = GC.getGameINLINE().getGameTurn();

	AI_setMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);

	bResult = insertAtEndMissionQueue(mission, !bAppend);

	if (bManual)
	{
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			if (isBusy() && GC.getMissionInfo(eMission).isSound())
			{
				playActionSound();
			}

			gDLL->getInterfaceIFace()->setHasMovedUnit(true);
		}

		CvEventReporter::getInstance().selectionGroupPushMission(this, eMission);

		doDelayedDeath();
	}

	return bResult;
}


void CvSelectionGroup::popMission()
{
	CLLNode<MissionData>* pTailNode;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	pTailNode = tailMissionQueueNode();

	if (pTailNode != NULL)
	{
		deleteMissionQueueNode(pTailNode);
	}
}


void CvSelectionGroup::autoMission()
{
	PROFILE_FUNC();

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		if (headMissionQueueNode() != NULL)
		{
			if (!isBusy())
			{
				bool bVisibleHuman = false;
				if (isHuman())
				{
					for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						if (!pLoopUnit->alwaysInvisible())
						{
							bVisibleHuman = true;
							break;
						}
					}
				}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/20/09                                jdog5000      */
/*                                                                                              */
/* Unit AI, Efficiency                                                                          */
/************************************************************************************************/
				//if (bVisibleHuman && GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 1) > 0)
				if (bVisibleHuman && GET_PLAYER(getOwnerINLINE()).AI_getAnyPlotDanger(plot(), 1))
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/	
				{
					clearMissionQueue();
				}
				else
				{
					if (getActivityType() == ACTIVITY_MISSION)
					{
						continueMission();
					}
					else
					{
						startMission();
					}
				}
			}
		}
	}

	doDelayedDeath();
}


void CvSelectionGroup::updateMission()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getMissionTimer() > 0)
	{
		changeMissionTimer(-1);

		if (getMissionTimer() == 0)
		{
			if (getActivityType() == ACTIVITY_MISSION)
			{
				continueMission();
			}
			else
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == NULL)
					{
						gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
					}
				}
			}
		}
	}
}


CvPlot* CvSelectionGroup::lastMissionPlot()
{
	CLLNode<MissionData>* pMissionNode;
	CvUnit* pTargetUnit;

	pMissionNode = tailMissionQueueNode();

	while (pMissionNode != NULL)
	{
		switch (pMissionNode->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		case MISSION_MOVE_TO_SENTRY:
#endif
// BUG - Sentry Actions - end
		case MISSION_ROUTE_TO:
			return GC.getMapINLINE().plotINLINE(pMissionNode->m_data.iData1, pMissionNode->m_data.iData2);
			break;

		case MISSION_MOVE_TO_UNIT:
			pTargetUnit = GET_PLAYER((PlayerTypes)pMissionNode->m_data.iData1).getUnit(pMissionNode->m_data.iData2);
			if (pTargetUnit != NULL)
			{
				return pTargetUnit->plot();
			}
			break;

		case MISSION_SKIP:
		case MISSION_SLEEP:
		case MISSION_FORTIFY:
		//case MISSION_ESTABLISH:
		//case MISSION_ESCAPE:
		case MISSION_BUILDUP:
		case MISSION_AUTO_BUILDUP:
		case MISSION_HEAL_BUILDUP:
		case MISSION_PLUNDER:
		case MISSION_AIRPATROL:
		case MISSION_SEAPATROL:
		case MISSION_HEAL:
		case MISSION_SENTRY:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		case MISSION_SENTRY_WHILE_HEAL:
		case MISSION_SENTRY_NAVAL_UNITS:
		case MISSION_SENTRY_LAND_UNITS:
#endif
// BUG - Sentry Actions - end
		case MISSION_AIRLIFT:
		case MISSION_NUKE:
		// < M.A.D. Nukes Start >
		case MISSION_PRETARGET_NUKE:
		// < M.A.D. Nukes End   >

		case MISSION_RECON:
		case MISSION_PARADROP:
		case MISSION_AIRBOMB:
		case MISSION_BOMBARD:
		case MISSION_RANGE_ATTACK:
		case MISSION_PILLAGE:
		case MISSION_SABOTAGE:
		case MISSION_DESTROY:
		case MISSION_STEAL_PLANS:
		case MISSION_FOUND:
		case MISSION_SPREAD:
		case MISSION_SPREAD_CORPORATION:
		case MISSION_JOIN:
		case MISSION_CONSTRUCT:
		case MISSION_DISCOVER:
		case MISSION_HURRY:
		case MISSION_TRADE:
		case MISSION_GREAT_WORK:
		case MISSION_INFILTRATE:
		case MISSION_GOLDEN_AGE:
		case MISSION_BUILD:
		case MISSION_LEAD:
		case MISSION_ESPIONAGE:
		case MISSION_DIE_ANIMATION:
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
		// Dale - AB: Bombing START
		case MISSION_AIRBOMB1:
		case MISSION_AIRBOMB2:
		case MISSION_AIRBOMB3:
		case MISSION_AIRBOMB4:
		case MISSION_AIRBOMB5:
		// Dale - AB: Bombing END
		// Dale - RB: Field Bombard START
		case MISSION_RBOMBARD:
		// Dale - RB: Field Bombard END
		// Dale - ARB: Archer Bombard START
		case MISSION_ABOMBARD:
		// Dale - ARB: Archer Bombard END
		// Dale - FE: Fighters START
		case MISSION_FENGAGE:
		// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 06/11/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		case MISSION_CLAIM_TERRITORY:
		case MISSION_HURRY_FOOD:
		case MISSION_INQUISITION:
		case MISSION_ESPIONAGE_SLEEP:
		case MISSION_SHADOW:
		case MISSION_GREAT_COMMANDER:
		case MISSION_WAIT_FOR_TECH:
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		//TB Combat Mods (Cure) begin
		case MISSION_CURE:
		case MISSION_AMBUSH:
		case MISSION_ASSASSINATE:
			break;

		default:
			// AIAndy: Assumed to be an outcome mission
			// FAssert(false);
			break;
		}

		pMissionNode = prevMissionQueueNode(pMissionNode);
	}

	return plot();
}


bool CvSelectionGroup::canStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible, bool bUseCache)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pTargetUnit;
	CvUnit* pLoopUnit;

	//cache isBusy
	if(bUseCache)
	{
		if(m_bIsBusyCache)
		{
			return false;
		}
	}
	else
	{
		if (isBusy())
		{
			return false;
		}
	}

	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		switch (iMission)
		{
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		case MISSION_MOVE_TO_SENTRY:
			if (!pLoopUnit->canSentry(NULL))
			{
				return false;
			}
			// fall through to next case
#endif
// BUG - Sentry Actions - end

		case MISSION_MOVE_TO:
			if (!(pPlot->at(iData1, iData2)))
			{
				return true;
			}
			break;

		case MISSION_ROUTE_TO:
			if (!(pPlot->at(iData1, iData2)) || (getBestBuildRoute(pPlot) != NO_ROUTE))
			{
				return true;
			}
			break;

		case MISSION_MOVE_TO_UNIT:
			FAssertMsg(iData1 != NO_PLAYER, "iData1 should be a valid Player");
			pTargetUnit = GET_PLAYER((PlayerTypes)iData1).getUnit(iData2);
			if ((pTargetUnit != NULL) && !(pTargetUnit->atPlot(pPlot)))
			{
				return true;
			}
			break;

		case MISSION_SKIP:
			if (pLoopUnit->canHold(pPlot))
			{
				return true;
			}
			break;

		case MISSION_SLEEP:
			if (pLoopUnit->canSleep(pPlot))
			{
				return true;
			}
			break;

		case MISSION_FORTIFY:
			if (pLoopUnit->canFortify(pPlot))
			{
				return true;
			}
			break;

		case MISSION_ESTABLISH:
			//if (pLoopUnit->canEstablish(pPlot))
			//{
			//	return true;
			//}
			return false;
			break;

		case MISSION_ESCAPE:
			//if (pLoopUnit->canEscape(pPlot))
			//{
			//	return true;
			//}
			return false;
			break;

		case MISSION_BUILDUP:
			if (pLoopUnit->canBuildUp(pPlot))
			{
				return true;
			}
			break;

		case MISSION_AUTO_BUILDUP:
			if (pLoopUnit->canBuildUp(pPlot) || pLoopUnit->canFortify(pPlot) || pLoopUnit->canSleep(pPlot))
			{
				return true;
			}
			break;

		case MISSION_HEAL_BUILDUP:
			if (pLoopUnit->canHeal(pPlot) && pPlot->getTotalTurnDamage(this) <= 0)
			{
				return true;
			}
			break;

		case MISSION_AIRPATROL:
			if (pLoopUnit->canAirPatrol(pPlot))
			{
				return true;
			}
			break;

		case MISSION_SEAPATROL:
			if (pLoopUnit->canSeaPatrol(pPlot))
			{
				return true;
			}
			break;
		
		case MISSION_HEAL:
			//ls612: Fix for Terrain damage, apparently that wasn't factored in anywhere else.
			if (pLoopUnit->canHeal(pPlot) && pPlot->getTotalTurnDamage(this) <= 0)
			{
				return true;
			}
			break;

		case MISSION_SENTRY:
			if (pLoopUnit->canSentry(pPlot))
			{
				return true;
			}
			break;
			
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		case MISSION_SENTRY_WHILE_HEAL:
			if ((pLoopUnit->canSentry(pPlot)) && (pLoopUnit->canHeal(pPlot)))
			{
				return true;
			}
			break;

		case MISSION_SENTRY_NAVAL_UNITS:
			if ((getDomainType() == DOMAIN_SEA) && (pLoopUnit->canSentry(pPlot)))
			{
				return true;
			}
			break;

		case MISSION_SENTRY_LAND_UNITS:
			if ((getDomainType() == DOMAIN_LAND) && (pLoopUnit->canSentry(pPlot)))
			{
				return true;
			}
			break;
#endif
// BUG - Sentry Actions - end

		case MISSION_AIRLIFT:
			if (pLoopUnit->canAirliftAt(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_NUKE:
			if (pLoopUnit->canNukeAt(pPlot, iData1, iData2))
			{
				return true;
			}
			break;
		// < M.A.D. Nukes Start >
		case MISSION_PRETARGET_NUKE:
			if (pPlot->isCity() && GET_PLAYER(pLoopUnit->getOwnerINLINE()).isEnabledMAD() && pLoopUnit->canNukeAt(pPlot, iData1, iData2, false) && !getHeadUnit()->isMADEnabled() && getHeadUnit()->getUnitInfo().getUnitAIType(UNITAI_ICBM))
			{
				return true;
			}
			break;
		// < M.A.D. Nukes End   >
		case MISSION_RECON:
			if (pLoopUnit->canReconAt(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_PARADROP:
			if (pLoopUnit->canParadropAt(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_AIRBOMB:
			if (pLoopUnit->canAirBombAt(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_BOMBARD:
			if (pLoopUnit->canBombard(pPlot))
			{
				return true;
			}
			break;
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
		// Dale - RB: Field Bombard START
		case MISSION_RBOMBARD:
			if(GC.isDCM_RANGE_BOMBARD())
			{
				if (pLoopUnit->canBombardAtRanged(pPlot, iData1, iData2))
				{
					return true;
				}
			}
			break;
		// Dale - RB: Field Bombard END

		// Dale - ARB: Archer Bombard START
		case MISSION_ABOMBARD:
			if(GC.isDCM_ARCHER_BOMBARD())
			{
				if (pLoopUnit->canArcherBombardAt(pPlot, iData1, iData2))
				{
					return true;
				}
			}
			break;
		// Dale - ARB: Archer Bombard END
/************************************************************************************************/
/* DCM                                     END                                    Johny Smith  */
/************************************************************************************************/

		case MISSION_RANGE_ATTACK:
			if (pLoopUnit->canRangeStrikeAt(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_PLUNDER:
			if (pLoopUnit->canPlunder(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_PILLAGE:
			if (pLoopUnit->canPillage(pPlot))
			{
				return true;
			}
			break;

		case MISSION_SABOTAGE:
			if (pLoopUnit->canSabotage(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_DESTROY:
			if (pLoopUnit->canDestroy(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_STEAL_PLANS:
			if (pLoopUnit->canStealPlans(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_FOUND:
			if (pLoopUnit->canFound(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_SPREAD:
			if (pLoopUnit->canSpread(pPlot, ((ReligionTypes)iData1), bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_SPREAD_CORPORATION:
			if (pLoopUnit->canSpreadCorporation(pPlot, ((CorporationTypes)iData1), bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_JOIN:
			if (pLoopUnit->canJoin(pPlot, ((SpecialistTypes)iData1)))
			{
				return true;
			}
			break;

		case MISSION_CONSTRUCT:
			if (pLoopUnit->canConstruct(pPlot, ((BuildingTypes)iData1), bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_DISCOVER:
			if (pLoopUnit->canDiscover(pPlot))
			{
				return true;
			}
			break;

		case MISSION_HURRY:
			if (pLoopUnit->canHurry(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_TRADE:
			if (pLoopUnit->canTrade(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_GREAT_WORK:
			if (pLoopUnit->canGreatWork(pPlot))
			{
				return true;
			}
			break;

		case MISSION_INFILTRATE:
			if (pLoopUnit->canInfiltrate(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_GOLDEN_AGE:
			//this means to play the animation only			
			if (iData1 != -1)
			{
				return true;
			}

			if (pLoopUnit->canGoldenAge(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_BUILD:

            FAssertMsg(((BuildTypes)iData1) < GC.getNumBuildInfos(), "Invalid Build");
/************************************************************************************************/
/* Afforess	                  Start		 06/01/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
			if (pLoopUnit->canBuild(pPlot, ((BuildTypes)iData1), bTestVisible))
*/
			if (pLoopUnit->canBuild(pPlot, ((BuildTypes)iData1), bTestVisible && !GET_PLAYER(pLoopUnit->getOwnerINLINE()).isModderOption(MODDEROPTION_HIDE_UNAVAILBLE_BUILDS)))
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/			

            {
                return true;
            }
			break;

		case MISSION_LEAD:
			if (pLoopUnit->canLead(pPlot, iData1))
			{
				return true;
			}
			break;

		case MISSION_ESPIONAGE:
			if (pLoopUnit->canEspionage(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_CURE:
			if (pLoopUnit->canCure(pPlot, ((PromotionLineTypes)iData1)))
			{
				return true;
			}
			break;

		//ls612: Viewports Go To City mission
		case MISSION_GOTO:
			if (pLoopUnit->getDomainType() == DOMAIN_LAND || pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				return true;
			}
			break;

		case MISSION_DIE_ANIMATION:
			return false;
			break;

		case MISSION_BEGIN_COMBAT:
		case MISSION_END_COMBAT:
		case MISSION_AIRSTRIKE:
			if (pLoopUnit->canAirStrike(pPlot))
			{
				return true;
			}
			break;
		case MISSION_SURRENDER:
		case MISSION_IDLE:
		case MISSION_DIE:
		case MISSION_DAMAGE:
		case MISSION_MULTI_SELECT:
		case MISSION_MULTI_DESELECT:
			break;
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
		// Dale - AB: Bombing START
		case MISSION_AIRBOMB1:
			if (pLoopUnit->canAirBomb1At(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_AIRBOMB2:
			if (pLoopUnit->canAirBomb2At(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_AIRBOMB3:
			if (pLoopUnit->canAirBomb3At(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_AIRBOMB4:
			if (pLoopUnit->canAirBomb4At(pPlot, iData1, iData2))
			{
				return true;
			}
			break;

		case MISSION_AIRBOMB5:
			if (pLoopUnit->canAirBomb5At(pPlot, iData1, iData2))
			{
				return true;
			}
			break;
		// Dale - AB: Bombing END

		// Dale - FE: Fighters START
		case MISSION_FENGAGE:
			if (pLoopUnit->canFEngageAt(pPlot, iData1, iData2))
			{
				return true;
			}
			break;
		// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	/************************************************************************************************/
/* Afforess	                  Start		 02/14/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		case MISSION_CLAIM_TERRITORY:
			if (pLoopUnit->canClaimTerritory(pPlot))
			{
				return true;
			}
			break;
			
		case MISSION_HURRY_FOOD:
			if (pLoopUnit->canHurryFood(pPlot))
			{
				return true;
			}
			break;
		case MISSION_INQUISITION:
			if (pLoopUnit->canPerformInquisition(pPlot))
			{
				return true;
			}
			break;
		case MISSION_ESPIONAGE_SLEEP:
			if (pLoopUnit->plot()->isCity() && pLoopUnit->canSleep(pPlot) && pLoopUnit->canEspionage(pPlot, true) && pLoopUnit->getFortifyTurns() != GC.getDefineINT("MAX_FORTIFY_TURNS"))
			{
				return true;
			}
			break;
		case MISSION_SHADOW:
			{
				CvPlot* pShadowPlot = GC.getMapINLINE().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2);
				CvUnit* pLoopShadow = NULL;
				CLLNode<IDInfo>* pUnitShadowNode = NULL;
				int iValidShadowUnits = 0;
				if (pShadowPlot != NULL)
				{
					pUnitShadowNode = pShadowPlot->headUnitNode();
					while (pUnitShadowNode != NULL)
					{
						pLoopShadow = ::getUnit(pUnitShadowNode->m_data);
						pUnitShadowNode = pShadowPlot->nextUnitNode(pUnitShadowNode);
						if (pLoopUnit->canShadowAt(pShadowPlot, pLoopShadow))
						{
							iValidShadowUnits++;
						}
					}
					if (iValidShadowUnits > 0)
					{
						return true;
					}
				}
				if (iValidShadowUnits > 0)
				{
					return true;
				}
			}
			break;
		case MISSION_GREAT_COMMANDER:
			if (GC.getGameINLINE().isOption(GAMEOPTION_GREAT_COMMANDERS) && pLoopUnit->getUnitInfo().isGreatGeneral() && !pLoopUnit->isCommander())
			{
				return true;
			}
			break;
		case MISSION_WAIT_FOR_TECH:
			if (pLoopUnit->canDiscover(NULL))
			{
				return true;
			}
			break;

		case MISSION_ASSASSINATE:
			if (pLoopUnit->canAmbush(pPlot, true))
			{
				return true;
			}
			break;

		case MISSION_AMBUSH:
			if (pLoopUnit->canAmbush(pPlot, false))
			{
				return true;
			}
			break;
		default:
			// AIAndy: Assumed to be an outcome mission
			// FAssert(false);
			CvOutcomeMission* pOutcomeMission = pLoopUnit->getUnitInfo().getOutcomeMissionByMission((MissionTypes)iMission);
			if (pOutcomeMission)
			{
				if (pOutcomeMission->isPossible(pLoopUnit, bTestVisible))
				{
					return true;
				}
			}
			// Outcome missions on unit combats
			for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
			{
				if (pLoopUnit->isHasUnitCombat((UnitCombatTypes)iI))
				{
					UnitCombatTypes eCombat = (UnitCombatTypes)iI;
					CvOutcomeMission* pOutcomeMission = GC.getUnitCombatInfo(eCombat).getOutcomeMissionByMission((MissionTypes)iMission);
					if (pOutcomeMission)
					{
						if (pOutcomeMission->isPossible(pLoopUnit, bTestVisible))
						{
							return true;
						}
					}
				}
			}
			break;
		}
	}

	return false;
}


bool CvSelectionGroup::startMission()
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit = NULL;
	bool bDelete;
	bool bAction;
	bool bNuke;
	bool bNotify;
	bool bResult;
	int iUnitValue = 0;
	int iBestUnitValue = 0;

	FAssert(!isBusy());
	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssert(headMissionQueueNode() != NULL);

	if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		if (!GET_PLAYER(getOwnerINLINE()).isTurnActive())
		{
			if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
			{
				if (IsSelected())
				{
					gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
				}
			}

			return false;
		}
	}

	if (canAllMove())
	{
		setActivityType(ACTIVITY_MISSION);
	}
	else
	{
		setActivityType(ACTIVITY_HOLD);
	}


	bDelete = false;
	bAction = false;
	bNuke = false;
	bNotify = false;
	bResult = true;

	if (!canStartMission(headMissionQueueNode()->m_data.eMissionType, headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, plot()))
	{
		bDelete = true;
		bResult = false;
	}
	else
	{
		FAssertMsg(GET_PLAYER(getOwnerINLINE()).isTurnActive() || GET_PLAYER(getOwnerINLINE()).isHuman(), "It's expected that either the turn is active for this player or the player is human");

		switch (headMissionQueueNode()->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		case MISSION_MOVE_TO_SENTRY:
#endif
// BUG - Sentry Actions - end
// BUG - Safe Move - start
			// if player is human, save the visibility and reveal state of the last plot of the move path from the initial plot
			if (isHuman())
			{
				checkLastPathPlot(GC.getMapINLINE().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2));
			}
			break;
// BUG - Safe Move - end

		case MISSION_ROUTE_TO:
		case MISSION_MOVE_TO_UNIT:
			break;

		case MISSION_SKIP:
			setActivityType(ACTIVITY_HOLD);
			bDelete = true;
			break;

		case MISSION_SLEEP:
			setActivityType(ACTIVITY_SLEEP, MISSION_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_FORTIFY:
			setActivityType(ACTIVITY_SLEEP, MISSION_FORTIFY);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_ESTABLISH:
			setActivityType(ACTIVITY_SLEEP, /*MISSION_ESTABLISH*/MISSION_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_ESCAPE:
			setActivityType(ACTIVITY_SLEEP, /*MISSION_ESCAPE*/MISSION_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_BUILDUP:
			setActivityType(ACTIVITY_SLEEP, MISSION_BUILDUP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_AUTO_BUILDUP:
			setActivityType(ACTIVITY_SLEEP, MISSION_AUTO_BUILDUP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_HEAL_BUILDUP:
			setActivityType(ACTIVITY_HEAL, MISSION_HEAL_BUILDUP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_PLUNDER:
			setActivityType(ACTIVITY_PLUNDER);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_AIRPATROL:
			setActivityType(ACTIVITY_INTERCEPT);
			bDelete = true;
			break;

		case MISSION_SEAPATROL:
			setActivityType(ACTIVITY_PATROL);
			bDelete = true;
			break;

		case MISSION_HEAL:
			setActivityType(ACTIVITY_HEAL, MISSION_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_SENTRY:
			setActivityType(ACTIVITY_SENTRY);
			bNotify = true;
			bDelete = true;
			break;

// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		case MISSION_SENTRY_WHILE_HEAL:
			setActivityType(ACTIVITY_SENTRY_WHILE_HEAL);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_SENTRY_NAVAL_UNITS:
			setActivityType(ACTIVITY_SENTRY_NAVAL_UNITS);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_SENTRY_LAND_UNITS:
			setActivityType(ACTIVITY_SENTRY_LAND_UNITS);
			bNotify = true;
			bDelete = true;
			break;
#endif
// BUG - Sentry Actions - end

		case MISSION_AIRLIFT:
		case MISSION_NUKE:
		// < M.A.D. Nukes Start >
		case MISSION_PRETARGET_NUKE:
		// < M.A.D. Nukes End   >
		case MISSION_RECON:
		case MISSION_PARADROP:
		case MISSION_AIRBOMB:
		case MISSION_BOMBARD:
		case MISSION_RANGE_ATTACK:
		case MISSION_PILLAGE:
		case MISSION_SABOTAGE:
		case MISSION_DESTROY:
		case MISSION_STEAL_PLANS:
		case MISSION_FOUND:
		case MISSION_SPREAD:
		case MISSION_SPREAD_CORPORATION:
		case MISSION_JOIN:
		case MISSION_CONSTRUCT:
		case MISSION_DISCOVER:
		case MISSION_HURRY:
		case MISSION_TRADE:
		case MISSION_GREAT_WORK:
		case MISSION_INFILTRATE:
		case MISSION_GOLDEN_AGE:
		case MISSION_BUILD:
		case MISSION_LEAD:
		case MISSION_ESPIONAGE:
		case MISSION_DIE_ANIMATION:
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
		// Dale - AB: Bombing START
		case MISSION_AIRBOMB1:
		case MISSION_AIRBOMB2:
		case MISSION_AIRBOMB3:
		case MISSION_AIRBOMB4:
		case MISSION_AIRBOMB5:
		// Dale - AB: Bombing END
		// Dale - RB: Field Bombard START
		case MISSION_RBOMBARD:
		// Dale - RB: Field Bombard END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 06/05/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		case MISSION_CLAIM_TERRITORY:
		case MISSION_HURRY_FOOD:
		case MISSION_INQUISITION:
		case MISSION_GREAT_COMMANDER:
			break;
		case MISSION_ESPIONAGE_SLEEP:
		case MISSION_WAIT_FOR_TECH:
		case MISSION_AMBUSH:
		case MISSION_ASSASSINATE:
			bDelete = true;
			break;
		case MISSION_SHADOW:
			break;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		default:
			// AIAndy: Assumed to be an outcome mission
			// FAssert(false);
			break;
		}

		if ( bNotify )
		{
			NotifyEntity( headMissionQueueNode()->m_data.eMissionType );
		}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/30/10                                jdog5000      */
/*                                                                                              */
/* War tactics AI                                                                               */
/************************************************************************************************/
		if( headMissionQueueNode()->m_data.eMissionType == MISSION_PILLAGE )
		{
			// Fast units pillage first
			pUnitNode = headUnitNode();
			int iMaxMovesLeft = 1;

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

/************************************************************************************************/
/* Afforess	                  Start		 07/27/10                                               */
/*                                                                                              */
/* Infinite Loop Fix                                                                            */
/************************************************************************************************/
/*
				if( pLoopUnit->canMove() && pLoopUnit->canPillage(plot()) )
*/
				if( pLoopUnit->canMove() && pLoopUnit->canPillage(pLoopUnit->plot()) )
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				{
					int iMovesLeft = pLoopUnit->movesLeft();
					if( pLoopUnit->bombardRate() > 0 )
					{
						iMovesLeft /= 2;
					}
					//iMovesLeft *= pLoopUnit->currHitPoints();
					//iMovesLeft /= std::max(1, pLoopUnit->maxHitPoints());
					iMovesLeft /= 100;

					iMaxMovesLeft = std::max( iMaxMovesLeft, iMovesLeft );
				}
			}

			bool bDidPillage = false;
			while( iMaxMovesLeft > 0 && !bDidPillage )
			{
				pUnitNode = headUnitNode();
				int iNextMaxMovesLeft = 0;
				//int iTries = getNumUnits() * 3;
				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
/************************************************************************************************/
/* Afforess	                  Start		 07/27/10                                               */
/*                                                                                              */
/* Infinite Loop Fix                                                                            */
/************************************************************************************************/
/*
					if( pLoopUnit->canMove() && pLoopUnit->canPillage(plot()) )
*/
					if( pLoopUnit->canMove() && pLoopUnit->canPillage(pLoopUnit->plot()) )
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
					{
						int iMovesLeft = pLoopUnit->movesLeft();
						if( pLoopUnit->bombardRate() > 0 )
						{
							iMovesLeft /= 2;
						}
						//int iCurrentHP = pLoopUnit->currHitPoints();
						//iMovesLeft *= iCurrentHP;
						//int iMaximumHP = pLoopUnit->maxHitPoints();
						//int iDivisor = std::max(1, iMaximumHP);
						//iMovesLeft /= iDivisor;
						iMovesLeft /= 100;

						iMovesLeft = std::max(iMaxMovesLeft, iMovesLeft);

						if( iMovesLeft >= iMaxMovesLeft )
						{
							if (pLoopUnit->pillage())
							{
								bAction = true;
								if( isHuman() || canAllMove() )
								{
									bDidPillage = true;
									iMovesLeft -= 1;
									break;
								}
							}
						}

						iNextMaxMovesLeft = std::min( iNextMaxMovesLeft, iMovesLeft );
					}
				}

				iMaxMovesLeft = iNextMaxMovesLeft;
			}
		}
		else
		{
			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->canMove())
				{
					switch (headMissionQueueNode()->m_data.eMissionType)
					{
					case MISSION_ASSASSINATE:
						if (pLoopUnit->canAmbush(plot(), true))
						{
							iUnitValue = pLoopUnit->AI_attackOdds(plot(), false, 0, true);
							if (iUnitValue >= iBestUnitValue)
							{
								pBestUnit = pLoopUnit;
								iBestUnitValue = iUnitValue;
							}
						}
						break;

					case MISSION_AMBUSH:
						if (pLoopUnit->canAmbush(plot(), false))
						{
							iUnitValue = pLoopUnit->AI_attackOdds(plot(), false, 0, false);
							if (iUnitValue >= iBestUnitValue)
							{
								pBestUnit = pLoopUnit;
								iBestUnitValue = iUnitValue;
							}
						}
						break;

					default:
						break;
					}
				}
			}

			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->canMove())
				{
					switch (headMissionQueueNode()->m_data.eMissionType)
					{
					case MISSION_MOVE_TO:
					case MISSION_ROUTE_TO:
					case MISSION_MOVE_TO_UNIT:
					case MISSION_SKIP:
					case MISSION_SLEEP:
					case MISSION_FORTIFY:
					//case MISSION_ESTABLISH:
					//case MISSION_ESCAPE:
					case MISSION_BUILDUP:
					case MISSION_AUTO_BUILDUP:
					case MISSION_HEAL_BUILDUP:
					case MISSION_AIRPATROL:
					case MISSION_SEAPATROL:
					case MISSION_HEAL:
					case MISSION_SENTRY:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
					case MISSION_MOVE_TO_SENTRY:
					case MISSION_SENTRY_WHILE_HEAL:
					case MISSION_SENTRY_NAVAL_UNITS:
					case MISSION_SENTRY_LAND_UNITS:
#endif
// BUG - Sentry Actions - end
						break;

					case MISSION_AIRLIFT:
						if (pLoopUnit->airlift(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_NUKE:
						if (pLoopUnit->nuke(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;

							if (GC.getMapINLINE().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2)->isVisibleToWatchingHuman())
							{
								bNuke = true;
							}
						}
						break;
				// < M.A.D. Nukes Start >
					case MISSION_PRETARGET_NUKE:
						if (pLoopUnit->setMADTargetPlot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							gDLL->getInterfaceIFace()->clearSelectionList();
							bAction = true;
						}
						break;
				// < M.A.D. Nukes End   >
					case MISSION_RECON:
						if (pLoopUnit->recon(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_PARADROP:
						if (pLoopUnit->paradrop(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_AIRBOMB:
						if (pLoopUnit->airBomb(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_BOMBARD:
						if (pLoopUnit->bombard())
						{
							bAction = true;
						}
						break;

/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
					case MISSION_RBOMBARD:
						// Dale - RB: Field Bombard START
						if(GC.isDCM_RANGE_BOMBARD())
						{
							if (pLoopUnit->bombardRanged(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
						}
						break;

					case MISSION_RANGE_ATTACK:
						if (pLoopUnit->rangeStrike(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/

					case MISSION_PILLAGE:
						if (pLoopUnit->pillage())
						{
							bAction = true;
						}
						break;

					case MISSION_PLUNDER:
						if (pLoopUnit->plunder())
						{
							bAction = true;
						}
						break;

					case MISSION_SABOTAGE:
						if (pLoopUnit->sabotage())
						{
							bAction = true;
						}
						break;

					case MISSION_DESTROY:
						if (pLoopUnit->destroy())
						{
							bAction = true;
						}
						break;

					case MISSION_STEAL_PLANS:
						if (pLoopUnit->stealPlans())
						{
							bAction = true;
						}
						break;

					case MISSION_FOUND:
						if (pLoopUnit->found())
						{
							bAction = true;
						}
						break;

					case MISSION_SPREAD:
						if (pLoopUnit->spread((ReligionTypes)(headMissionQueueNode()->m_data.iData1)))
						{
							bAction = true;
						}
						break;

					case MISSION_SPREAD_CORPORATION:
						if (pLoopUnit->spreadCorporation((CorporationTypes)(headMissionQueueNode()->m_data.iData1)))
						{
							bAction = true;
						}
						break;

					//TB Combat Mod Begin (Cure)
					case MISSION_CURE:
						if (pLoopUnit->CureAffliction((PromotionLineTypes)(headMissionQueueNode()->m_data.iData1)))
						{
							bAction = true;
						}
						break;
					//TB Combat Mod end (Cure)
					case MISSION_JOIN:
						if (pLoopUnit->join((SpecialistTypes)(headMissionQueueNode()->m_data.iData1)))
						{
							bAction = true;
						}
						break;

					case MISSION_CONSTRUCT:
						if (pLoopUnit->construct((BuildingTypes)(headMissionQueueNode()->m_data.iData1)))
						{
							bAction = true;
						}
						break;

					case MISSION_DISCOVER:
						if (pLoopUnit->discover())
						{
							bAction = true;
						}
						break;

					case MISSION_HURRY:
						if (pLoopUnit->hurry())
						{
							bAction = true;
						}
						break;

					case MISSION_TRADE:
						if (pLoopUnit->trade())
						{
							bAction = true;
						}
						break;

					case MISSION_GREAT_WORK:
						if (pLoopUnit->greatWork())
						{
							bAction = true;
						}
						break;

					case MISSION_INFILTRATE:
						if (pLoopUnit->infiltrate())
						{
							bAction = true;
						}
						break;

					case MISSION_GOLDEN_AGE:
						//just play animation, not golden age - JW
						if (headMissionQueueNode()->m_data.iData1 != -1)
						{
							CvMissionDefinition kMission;
							kMission.setMissionTime(GC.getMissionInfo(MISSION_GOLDEN_AGE).getTime() * gDLL->getSecsPerTurn());
							kMission.setUnit(BATTLE_UNIT_ATTACKER, pLoopUnit);
							kMission.setUnit(BATTLE_UNIT_DEFENDER, NULL);
							kMission.setPlot(pLoopUnit->plot());
							kMission.setMissionType(MISSION_GOLDEN_AGE);
							pLoopUnit->addMission(&kMission);
							pLoopUnit->NotifyEntity(MISSION_GOLDEN_AGE);
							bAction = true;
						}
						else
						{
							if (pLoopUnit->goldenAge())
							{
								bAction = true;
							}
						}
						break;

					case MISSION_BUILD:
						break;

					case MISSION_LEAD:
						if (pLoopUnit->lead(headMissionQueueNode()->m_data.iData1))
						{
							bAction = true;
						}
						break;

					case MISSION_ESPIONAGE:
						if (pLoopUnit->espionage((EspionageMissionTypes)headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						pUnitNode = NULL; // allow one unit at a time to do espionage
						break;

/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
				// Dale - AB: Bombing START
					case MISSION_AIRBOMB1:
						if (pLoopUnit->airBomb1(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_AIRBOMB2:
						if (pLoopUnit->airBomb2(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_AIRBOMB3:
						if (pLoopUnit->airBomb3(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_AIRBOMB4:
						if (pLoopUnit->airBomb4(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;

					case MISSION_AIRBOMB5:
						if (pLoopUnit->airBomb5(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
						{
							bAction = true;
						}
						break;
						// Dale - AB: Bombing END

					case MISSION_ABOMBARD:
					// Dale - ARB: Archer Bombard START
						if(GC.isDCM_ARCHER_BOMBARD())
						{
							if (pLoopUnit->archerBombard(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
						}
					// Dale - ARB: Archer Bombard END
						break;

					// Dale - FE: Fighters START
					case MISSION_FENGAGE:
						if(GC.isDCM_FIGHTER_ENGAGE())
						{
							if (pLoopUnit->fighterEngage(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
						}
						break;
					// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 02/14/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
					case MISSION_CLAIM_TERRITORY:
						if (pLoopUnit->claimTerritory())
						{
							bAction = true;
						}
						pUnitNode = NULL; // allow only one unit to claim territory (no need for more)
						break;
						
					case MISSION_HURRY_FOOD:
						if (pLoopUnit->hurryFood())
						{
							bAction = true;
						}
						break;

					case MISSION_INQUISITION:
						if (pLoopUnit->performInquisition())
						{
							bAction = true;
						}
						break;

					case MISSION_ESPIONAGE_SLEEP:
						if (pLoopUnit->sleepForEspionage())
						{
							//bAction = true;
						}
						break;

					case MISSION_WAIT_FOR_TECH:
						pLoopUnit->waitForTech(headMissionQueueNode()->m_data.iFlags, headMissionQueueNode()->m_data.iData1);
						break;

					case MISSION_GREAT_COMMANDER:
						pLoopUnit->setCommander(true);
						pUnitNode = NULL;
						break;

					case MISSION_SHADOW:
						{
							CvPlot* pShadowPlot = GC.getMapINLINE().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2);
							if (pShadowPlot != NULL)
							{
								//if (pLoopUnit->canShadowAt(pShadowPlot))
								{
									//Check for multiple valid units
									CvUnit* pLoopShadow = NULL;
									CLLNode<IDInfo>* pUnitShadowNode = NULL;
									int iValidShadowUnits = 0;
									pUnitShadowNode = pShadowPlot->headUnitNode();
									while (pUnitShadowNode != NULL)
									{
										pLoopShadow = ::getUnit(pUnitShadowNode->m_data);
										pUnitShadowNode = pShadowPlot->nextUnitNode(pUnitShadowNode);
										if (pLoopUnit->canShadowAt(pShadowPlot, pLoopShadow))
										{
											iValidShadowUnits++;
										}
									}
									//Strange Handling to ensure MP works
									if (headMissionQueueNode()->m_data.iFlags == 0 && iValidShadowUnits > 1)
									{
										CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_SELECT_UNIT, pLoopUnit->getID(), pShadowPlot->getX(), pShadowPlot->getY());
										if (pInfo)
										{
											gDLL->getInterfaceIFace()->addPopup(pInfo, pLoopUnit->getOwnerINLINE(), true);
										}
									}
									else if (iValidShadowUnits > 0)
									{
										if (iValidShadowUnits == 1)
										{
											pLoopUnit->setShadowUnit(pShadowPlot->getCenterUnit());
										}
										else
										{
											pLoopUnit->setShadowUnit(GET_PLAYER(pLoopUnit->getOwnerINLINE()).getUnit(headMissionQueueNode()->m_data.iFlags));
										}
										setAutomateType(AUTOMATE_SHADOW);
										bAction = true;
									}
								}
							}
						}
						break;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

					case MISSION_DIE_ANIMATION:
						bAction = true;
						break;

					case MISSION_ASSASSINATE:
						if (pLoopUnit == pBestUnit)
						{
							if (pLoopUnit->doAmbush(true))
							{
								bAction = true;
							}
						}
						break;

					case MISSION_AMBUSH:
						if (pLoopUnit == pBestUnit)
						{
							if (pLoopUnit->doAmbush(false))
							{
								bAction = true;
							}
						}
						break;

					default:
						// AIAndy: Assumed to be an outcome mission
						// FAssert(false);
						if (pLoopUnit->doOutcomeMission(headMissionQueueNode()->m_data.eMissionType))
						{
							bAction = true;
						}
						break;
					}

					if (getNumUnits() == 0)
					{
						break;
					}

					if (headMissionQueueNode() == NULL)
					{
						break;
					}
				}
			}
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if ((getNumUnits() > 0) && (headMissionQueueNode() != NULL))
	{
		if (bAction)
		{
			if (isHuman())
			{
				if (plot()->isVisibleToWatchingHuman())
				{
					updateMissionTimer();
				}
			}
		}

		if (bNuke)
		{
			setMissionTimer(GC.getMissionInfo(MISSION_NUKE).getTime());
		}

		if (!isBusy())
		{
			if (bDelete)
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (IsSelected())
					{
						gDLL->getInterfaceIFace()->changeCycleSelectionCounter((GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_MOVES)) ? 1 : 2);
					}
				}

				deleteMissionQueueNode(headMissionQueueNode());
			}
			else if (getActivityType() == ACTIVITY_MISSION)
			{
				return continueMission();
			}
		}
	}

	return bResult;
}


bool CvSelectionGroup::continueMission(int iSteps)
{
	PROFILE_FUNC();

	CvUnit* pTargetUnit;
	bool bDone;
	bool bAction;
	bool bFailed;
	CLLNode<MissionData>* headQueueNode = headMissionQueueNode();

	FAssert(!isBusy());
	FAssert(headQueueNode != NULL);
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getActivityType() == ACTIVITY_AWAKE)
	{
		return false;
	}

	FAssert(getActivityType() == ACTIVITY_MISSION);

	if (headQueueNode == NULL ||
		((headQueueNode->m_data.eMissionType == MISSION_MOVE_TO ||
		/*headQueueNode->m_data.eMissionType == MISSION_MOVE_TO_SENTRY ||*/ 
		 headQueueNode->m_data.eMissionType == MISSION_ROUTE_TO ||
		 headQueueNode->m_data.eMissionType == MISSION_MOVE_TO_UNIT) && (headQueueNode->m_data.iData1 == -1 || headQueueNode->m_data.iData2 == -1)))
	{
		// just in case...
		setActivityType(ACTIVITY_AWAKE);
		return false;
	}

	bDone = false;
	bAction = false;
	bFailed = false;

	if (headQueueNode->m_data.iPushTurn == GC.getGameINLINE().getGameTurn() || (headQueueNode->m_data.iFlags & MOVE_THROUGH_ENEMY))
	{
		if (headQueueNode->m_data.eMissionType == MISSION_MOVE_TO && canAllMove())
		{
			bool bFailedAlreadyFighting;
			if (groupAttack(headQueueNode->m_data.iData1, headQueueNode->m_data.iData2, headQueueNode->m_data.iFlags, bFailedAlreadyFighting))
			{
				bDone = true;
			}
		}
	}

	if ((headQueueNode->m_data.iFlags & (MOVE_RECONSIDER_ON_LEAVING_OWNED | MOVE_AVOID_ENEMY_UNITS)))
	{
		if ((headQueueNode->m_data.eMissionType == MISSION_MOVE_TO || headQueueNode->m_data.eMissionType == MISSION_ROUTE_TO) && canAllMove())
		{
			if ( !checkMoveSafety(headQueueNode->m_data.iData1, headQueueNode->m_data.iData2, headQueueNode->m_data.iFlags) )
			{
				OutputDebugString(CvString::format("%S (%d) interrupted cautious move while moving to (%d,%d)...\n",getHeadUnit()->getDescription().c_str(),getHeadUnit()->getID(), headQueueNode->m_data.iData1, headQueueNode->m_data.iData2).c_str());
				bDone = true;
				bAction = false;
				bFailed = true;
			}
		}
	}

	// extra crash protection, should never happen (but a previous bug in groupAttack was causing a NULL here)
	// while that bug is fixed, no reason to not be a little more careful
	headQueueNode = headMissionQueueNode();

	if ( headQueueNode == NULL)
	{
		pushMission(MISSION_SKIP);
		return true;	//	It did something so the mission was not a failure
	}

	if (!bDone)
	{
		if (getNumUnits() > 0)
		{
			if (canAllMove())
			{
				switch (headQueueNode->m_data.eMissionType)
				{
				case MISSION_MOVE_TO:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
				case MISSION_MOVE_TO_SENTRY:
#endif
// BUG - Sentry Actions - end
// BUG - Safe Move - start
					// if player is human, save the visibility and reveal state of the last plot of the move path from the initial plot
					// if it hasn't been saved already to handle units in motion when loading a game
					if (isHuman() && !isLastPathPlotChecked())
					{
						checkLastPathPlot(GC.getMapINLINE().plotINLINE(headQueueNode->m_data.iData1, headQueueNode->m_data.iData2));
					}
// BUG - Safe Move - end

					if (getDomainType() == DOMAIN_AIR)
					{
						groupPathTo(headQueueNode->m_data.iData1, headQueueNode->m_data.iData2, headQueueNode->m_data.iFlags);
						bDone = true;
					}
					else if (groupPathTo(headQueueNode->m_data.iData1, headQueueNode->m_data.iData2, headQueueNode->m_data.iFlags))
					{
						bAction = true;

						if (getNumUnits() > 0)
						{
							if (!canAllMove())
							{
								if (headMissionQueueNode() != NULL)
								{
									if (groupAmphibMove(GC.getMapINLINE().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2), headMissionQueueNode()->m_data.iFlags))
									{
										bAction = false;
										bDone = true;
									}
								}
							}
						}
					}
					else
					{
						if (getNumUnits() > 0)
						{
							if (headMissionQueueNode() != NULL)
							{
								if (groupAmphibMove(GC.getMapINLINE().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2), headMissionQueueNode()->m_data.iFlags))
								{
									bAction = false;
									bDone = true;
									break;
								}
							}
						}

						bFailed = true;	//	Can't move there! (e.g. - breaches stacking limit)
						bDone = true;
					}
					break;

				case MISSION_ROUTE_TO:
					if (groupRoadTo(headQueueNode->m_data.iData1, headQueueNode->m_data.iData2, headQueueNode->m_data.iFlags))
					{
						bAction = true;
					}
					else
					{
						bFailed = true;	//	Can't move there! (e.g. - breaches stacking limit)
						bDone = true;
					}
					break;

				case MISSION_MOVE_TO_UNIT:
					if ((getHeadUnitAI() == UNITAI_CITY_DEFENSE) && plot()->isCity() && (plot()->getTeam() == getTeam()))
					{
						if (plot()->getBestDefender(getOwnerINLINE())->getGroup() == this)
						{
							bAction = false;
							bDone = true;
							break;
						}
					}
					pTargetUnit = GET_PLAYER((PlayerTypes)headQueueNode->m_data.iData1).getUnit(headQueueNode->m_data.iData2);
					if (pTargetUnit != NULL)
					{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/07/08                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
						// Handling for mission to retrieve a unit
						if( AI_getMissionAIType() == MISSIONAI_PICKUP )
						{
							if( !(pTargetUnit->getGroup()->isStranded()) || isFull() || (pTargetUnit->plot() == NULL) )
							{
								bDone = true;
								bAction = false;
								break;
							}

							CvPlot* pPickupPlot = NULL;
							CvPlot* pAdjacentPlot = NULL;
							int iPathTurns;
							int iBestPathTurns = MAX_INT;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/08/09                        Maniac & jdog5000     */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
/* original bts code
							if( (pTargetUnit->plot()->isWater() || pTargetUnit->plot()->isFriendlyCity(*getHeadUnit(), true)) && generatePath(plot(), pTargetUnit->plot(), 0, false, &iPathTurns) )
*/
							if( (canMoveAllTerrain() || pTargetUnit->plot()->isWater() || pTargetUnit->plot()->isFriendlyCity(*getHeadUnit(), true)) && generatePath(plot(), pTargetUnit->plot(), 0, false, &iPathTurns) )
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
							{
								pPickupPlot = pTargetUnit->plot();
							}
							else
							{
								for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
								{
									pAdjacentPlot = plotDirection(pTargetUnit->plot()->getX_INLINE(), pTargetUnit->plot()->getY_INLINE(), ((DirectionTypes)iI));

									if (pAdjacentPlot != NULL)
									{
										if( atPlot(pAdjacentPlot) )
										{
											pPickupPlot = pAdjacentPlot;
											break;
										}

										if( pAdjacentPlot->isWater() || pAdjacentPlot->isFriendlyCity(*getHeadUnit(), true) )
										{
											if( generatePath(plot(), pAdjacentPlot, 0, true, &iPathTurns) )
											{
												if( iPathTurns < iBestPathTurns )
												{
													pPickupPlot = pAdjacentPlot;
													iBestPathTurns = iPathTurns;
												}
											}
										}
									}
								}
							}

							if( pPickupPlot != NULL )
							{
								if( atPlot(pPickupPlot) )
								{
									CLLNode<IDInfo>* pEntityNode;
									CvUnit* pLoopUnit;

									pEntityNode = headUnitNode();

									while (pEntityNode != NULL)
									{
										pLoopUnit = ::getUnit(pEntityNode->m_data);
										pEntityNode = nextUnitNode(pEntityNode);

										if( !(pLoopUnit->isFull()) )
										{
											pTargetUnit->getGroup()->setRemoteTransportUnit(pLoopUnit);
										}
									}

									bAction = true;
									bDone = true;
								}
								else
								{
									if (groupPathTo(pPickupPlot->getX_INLINE(), pPickupPlot->getY_INLINE(), headMissionQueueNode()->m_data.iFlags))
									{
										bAction = true;
									}
									else
									{
										bDone = true;
									}
								}
							}
							else
							{
								bDone = true;
							}
							break;
						}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

						if (AI_getMissionAIType() != MISSIONAI_SHADOW && AI_getMissionAIType() != MISSIONAI_GROUP)
						{
							if (!plot()->isOwned() || plot()->getOwnerINLINE() == getOwnerINLINE())
							{
								CvPlot* pMissionPlot = pTargetUnit->getGroup()->AI_getMissionAIPlot();
								if (pMissionPlot != NULL && NO_TEAM != pMissionPlot->getTeam())
								{
									if (pMissionPlot->isOwned() && pTargetUnit->isPotentialEnemy(pMissionPlot->getTeam(), pMissionPlot))
									{
										bAction = false;
										bDone = true;
										break;								
									}
								}
							}
						}
							 
						if (groupPathTo(pTargetUnit->getX_INLINE(), pTargetUnit->getY_INLINE(), headMissionQueueNode()->m_data.iFlags))
						{
							bAction = true;
						}
						else
						{
							bFailed = true;	//	Can't move there! (e.g. - breaches stacking limit)
							bDone = true;
						}
					}
					else
					{
						bDone = true;
					}
					break;

				case MISSION_SKIP:
				case MISSION_SLEEP:
				case MISSION_FORTIFY:
				//case MISSION_ESTABLISH:
				//case MISSION_ESCAPE:
				case MISSION_BUILDUP:
				case MISSION_AUTO_BUILDUP:
				case MISSION_HEAL_BUILDUP:
				case MISSION_PLUNDER:
				case MISSION_AIRPATROL:
				case MISSION_SEAPATROL:
				case MISSION_HEAL:
				case MISSION_SENTRY:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
				case MISSION_SENTRY_WHILE_HEAL:
				case MISSION_SENTRY_NAVAL_UNITS:
				case MISSION_SENTRY_LAND_UNITS:
#endif
// BUG - Sentry Actions - end
					FAssert(false);
					break;

				case MISSION_AIRLIFT:
				case MISSION_NUKE:
				// < M.A.D. Nukes Start >
				case MISSION_PRETARGET_NUKE:
				// < M.A.D. Nukes End   >
				case MISSION_RECON:
				case MISSION_PARADROP:
				case MISSION_AIRBOMB:
				case MISSION_BOMBARD:
				case MISSION_RANGE_ATTACK:
				case MISSION_PILLAGE:
				case MISSION_SABOTAGE:
				case MISSION_DESTROY:
				case MISSION_STEAL_PLANS:
				case MISSION_FOUND:
				case MISSION_SPREAD:
				case MISSION_SPREAD_CORPORATION:
				case MISSION_CURE:
				case MISSION_JOIN:
				case MISSION_CONSTRUCT:
				case MISSION_DISCOVER:
				case MISSION_HURRY:
				case MISSION_TRADE:
				case MISSION_GREAT_WORK:
				case MISSION_INFILTRATE:
				case MISSION_GOLDEN_AGE:
				case MISSION_LEAD:
				case MISSION_ESPIONAGE:
				case MISSION_DIE_ANIMATION:
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
				// Dale - AB: Bombing START
				case MISSION_AIRBOMB1:
				case MISSION_AIRBOMB2:
				case MISSION_AIRBOMB3:
				case MISSION_AIRBOMB4:
				case MISSION_AIRBOMB5:
				case MISSION_RBOMBARD:
					break;

				case MISSION_BUILD:
					if (!groupBuild((BuildTypes)(headQueueNode->m_data.iData1)))
					{
						bFailed = true;
						bDone = true;
					}
					break;

				case MISSION_HURRY_FOOD:
				case MISSION_INQUISITION:
				case MISSION_CLAIM_TERRITORY:
				case MISSION_ESPIONAGE_SLEEP:
				case MISSION_GREAT_COMMANDER:
				case MISSION_SHADOW:
				case MISSION_WAIT_FOR_TECH:
				case MISSION_AMBUSH:
				case MISSION_ASSASSINATE:
					break;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				default:
					// AIAndy: Assumed to be an outcome mission
					// FAssert(false);
					break;
				}
			}
		}
	}

	if ((getNumUnits() > 0) && (headMissionQueueNode() != NULL))
	{
		if (!bDone)
		{
			switch (headMissionQueueNode()->m_data.eMissionType)
			{
			case MISSION_MOVE_TO:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
			case MISSION_MOVE_TO_SENTRY:
#endif
// BUG - Sentry Actions - end
				if (at(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
				{
// BUG - Safe Move - start
					clearLastPathPlot();
// BUG - Safe Move - end
					bDone = true;
				}
				break;

			case MISSION_ROUTE_TO:
				if (at(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
				{
					if (getBestBuildRoute(plot()) == NO_ROUTE)
					{
						bDone = true;
					}
				}
				break;

			case MISSION_MOVE_TO_UNIT:
				pTargetUnit = GET_PLAYER((PlayerTypes)headMissionQueueNode()->m_data.iData1).getUnit(headMissionQueueNode()->m_data.iData2);
				if ((pTargetUnit == NULL) || atPlot(pTargetUnit->plot()))
				{
					bDone = true;
				}
				break;

			case MISSION_SKIP:
			case MISSION_SLEEP:
			case MISSION_FORTIFY:
			//case MISSION_ESTABLISH:
			//case MISSION_ESCAPE:
			case MISSION_BUILDUP:
			case MISSION_AUTO_BUILDUP:
			case MISSION_HEAL_BUILDUP:
			case MISSION_PLUNDER:
			case MISSION_AIRPATROL:
			case MISSION_SEAPATROL:
			case MISSION_HEAL:
			case MISSION_SENTRY:
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
			case MISSION_SENTRY_WHILE_HEAL:
			case MISSION_SENTRY_NAVAL_UNITS:
			case MISSION_SENTRY_LAND_UNITS:
#endif
// BUG - Sentry Actions - end
				FAssert(false);
				break;

			case MISSION_AIRLIFT:
			case MISSION_NUKE:
			// < M.A.D. Nukes Start >
			case MISSION_PRETARGET_NUKE:
			// < M.A.D. Nukes End   >

			case MISSION_RECON:
			case MISSION_PARADROP:
			case MISSION_AIRBOMB:
			case MISSION_BOMBARD:
			case MISSION_RANGE_ATTACK:
			case MISSION_PILLAGE:
			case MISSION_SABOTAGE:
			case MISSION_DESTROY:
			case MISSION_STEAL_PLANS:
			case MISSION_FOUND:
			case MISSION_SPREAD:
			case MISSION_SPREAD_CORPORATION:
			case MISSION_CURE:
			case MISSION_JOIN:
			case MISSION_CONSTRUCT:
			case MISSION_DISCOVER:
			case MISSION_HURRY:
			case MISSION_TRADE:
			case MISSION_GREAT_WORK:
			case MISSION_INFILTRATE:
			case MISSION_GOLDEN_AGE:
			case MISSION_LEAD:
			case MISSION_ESPIONAGE:
			case MISSION_DIE_ANIMATION:
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
			// Dale - AB: Bombing START
			case MISSION_AIRBOMB1:
			case MISSION_AIRBOMB2:
			case MISSION_AIRBOMB3:
			case MISSION_AIRBOMB4:
			case MISSION_AIRBOMB5:
			// Dale - AB: Bombing END
			// Dale - RB: Field Bombard START
			case MISSION_RBOMBARD:
			// Dale - RB: Field Bombard END
			// Dale - ARB: Archer Bombard START
			case MISSION_ABOMBARD:
			// Dale - ARB: Archer Bombard END
			// Dale - FE: Fighters START
			case MISSION_FENGAGE:
			// Dale - FE: Fighters END
				bDone = true;
				break;

			case MISSION_BUILD:
				// XXX what happens if two separate worker groups are both building the mine...
				/*if (plot()->getBuildType() != ((BuildTypes)(headMissionQueueNode()->m_data.iData1)))
				{
					bDone = true;
				}*/
				break;
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 06/05/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			case MISSION_HURRY_FOOD:
			case MISSION_INQUISITION:
			case MISSION_CLAIM_TERRITORY:
			case MISSION_GREAT_COMMANDER:
				bDone = true;
				break;
			case MISSION_ESPIONAGE_SLEEP:
				break;
			case MISSION_SHADOW:
			case MISSION_WAIT_FOR_TECH:
			case MISSION_AMBUSH:
			case MISSION_ASSASSINATE:
				bDone = true;
				break;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
			default:
				// AIAndy: Assumed to be an outcome mission
				// FAssert(false);
				bDone = true;
				break;
			}
		}
	}

	if ((getNumUnits() > 0) && (headMissionQueueNode() != NULL))
	{
		if (bAction)
		{
			if (bDone || !canAllMove())
			{
				if (plot()->isVisibleToWatchingHuman())
				{
					updateMissionTimer(iSteps);

					if (showMoves())
					{
						if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
						{
							if (getOwnerINLINE() != GC.getGameINLINE().getActivePlayer())
							{
								if (plot()->isActiveVisible(false) && !isInvisible(GC.getGameINLINE().getActiveTeam()) && plot()->isInViewport())
								{
									gDLL->getInterfaceIFace()->lookAt(plot()->getPoint(), CAMERALOOKAT_NORMAL);
								}
							}
						}
					}
				}
			}
		}

		if (bDone)
		{
			if (!isBusy())
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (IsSelected())
					{
						if ((headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO) ||
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
							(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_SENTRY) ||
#endif
// BUG - Sentry Actions - end
							(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO) ||
							(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT))
						{
							gDLL->getInterfaceIFace()->changeCycleSelectionCounter((GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_MOVES)) ? 1 : 2);
						}
					}
				}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       08/04/09                                jdog5000      */
/*                                                                                              */
/* Player interface                                                                             */
/************************************************************************************************/
/* original bts code
				deleteMissionQueueNode(headMissionQueueNode());
*/
				if (!isHuman() || (headMissionQueueNode()->m_data.eMissionType != MISSION_MOVE_TO))
				{
					deleteMissionQueueNode(headMissionQueueNode());

					if ( !isHuman() )
					{
						//	We executed this one ok but if we have no moves left don't leave
						//	it with ACTIVITY_AWAKE as its state as that'll just cause the AI to spin wheels
						//	Preserve the missionAI and target plot/unit though as other units may search
						//	on them
						if ( !canAllMove() && headMissionQueueNode() == NULL)
						{
							pushMission(MISSION_SKIP, -1, -1, 0, false, false, AI_getMissionAIType(), AI_getMissionAIPlot(), AI_getMissionAIUnit());
						}
					}
				}
				else
				{
					if (canAllMove() || (nextMissionQueueNode(headMissionQueueNode()) == NULL))
					{
						deleteMissionQueueNode(headMissionQueueNode());
					}
				}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/				
			}
		}
		else
		{
			if (canAllMove())
			{
				//	If the recursion fails that is not an overall failure, since this step did
				//	something
				continueMission(iSteps + 1);
			}
			else if (!isBusy())
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (IsSelected())
					{
						gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
					}
				}
			}
		}
	}

	return !bFailed;
}

bool CvSelectionGroup::checkMoveSafety(int iX, int iY, int iFlags)
{
	CvPlot* pDestPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	if ( getHeadUnit() == NULL )
	{
		return false;
	}

	if (generatePath(plot(), pDestPlot, iFlags))
	{
		pDestPlot = getPathEndTurnPlot();
	}

	if ( (iFlags & MOVE_RECONSIDER_ON_LEAVING_OWNED) != 0 && !canDefend() )
	{
		if ( pDestPlot->getOwnerINLINE() != getOwnerINLINE() )
		{
			return false;
		}
	}
		
	if ( (iFlags &  MOVE_AVOID_ENEMY_UNITS) != 0 )
	{
		return !((CvUnitAI*)getHeadUnit())->exposedToDanger(pDestPlot, 80);
	}

	return true;
}

void CvSelectionGroup::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		pLoopUnit->doCommand(eCommand, iData1, iData2);

		pUnitNode = nextUnitNode(pUnitNode);

	}
}

bool CvSelectionGroup::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache, bool bAll)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	//cache isBusy
	if(bUseCache)
	{
		if(m_bIsBusyCache)
		{
			return false;
		}
	}
	else
	{
		if (isBusy())
		{
			return false;
		}
	}

	if(!canEverDoCommand(eCommand, iData1, iData2, bTestVisible, bUseCache))
		return false;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
		{
			if (!bAll)
			{
				return true;
			}
		}
		else if (bAll)
		{
			return false;
		}
	}
	if (bAll)
	{
		return true;
	}
	return false;
}

bool CvSelectionGroup::canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache)
{
	if(eCommand == COMMAND_LOAD)
	{
		CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);

			if (!pLoopUnit->isFull())
			{
				return true;
			}
		}

		//no cargo space on this plot
		return false;
	}
	else if(eCommand == COMMAND_UNLOAD)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->isCargo())
			{
				return true;
			}
		}

		//no loaded unit
		return false;
	}
	else if(eCommand == COMMAND_UPGRADE)
	{
		if(bUseCache)
		{
			//see if any of the different units can upgrade to this unit type
			for(int i=0;i<(int)m_aDifferentUnitCache.size();i++)
			{
				CvUnit *unit = m_aDifferentUnitCache[i];
				if(unit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
					return true;
			}

			return false;
		}
	}

	return true;
}

void CvSelectionGroup::setupActionCache()
{
	//cache busy calculation
	m_bIsBusyCache = isBusy();

    //cache different unit types
	m_aDifferentUnitCache.erase(m_aDifferentUnitCache.begin(), m_aDifferentUnitCache.end());
	CLLNode<IDInfo> *pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit *unit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if(unit->isReadyForUpgrade())
		{
			UnitTypes unitType = unit->getUnitType();
			bool bFound = false;
			for(int i=0;i<(int)m_aDifferentUnitCache.size();i++)
			{
				if(unitType == m_aDifferentUnitCache[i]->getUnitType())
				{
					bFound = true;
					break;
				}
			}

			if(!bFound)
				m_aDifferentUnitCache.push_back(unit);
		}
	}
}

// Returns true if one of the units can support the interface mode...
bool CvSelectionGroup::canDoInterfaceMode(InterfaceModeTypes eInterfaceMode)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	if (isBusy())
	{
		return false;
	}

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		switch (eInterfaceMode)
		{
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		case INTERFACEMODE_GO_TO_SENTRY:
			if (sentryAlertSameDomainType())
			{
				return false;
			}
			// fall through to next case
#endif
// BUG - Sentry Actions - end
		case INTERFACEMODE_GO_TO:
			if ((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE))
			{
				return true;
			}
			break;

		case INTERFACEMODE_GO_TO_TYPE:
			if ((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE))
			{
				if (pLoopUnit->plot()->plotCount(PUF_isUnitType, pLoopUnit->getUnitType(), -1, NULL, pLoopUnit->getOwnerINLINE()) > 1)
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_GO_TO_ALL:
			if ((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE))
			{
				if (pLoopUnit->plot()->plotCount(NULL, -1, -1, NULL, pLoopUnit->getOwnerINLINE()) > 1)
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_ROUTE_TO:
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA)
			{
				if (pLoopUnit->canBuildRoute())
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_AIRLIFT:
			if (pLoopUnit->canAirlift(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_NUKE:
			// < M.A.D. Nukes Start >
			if (pLoopUnit->canNuke(pLoopUnit->plot()) && pLoopUnit->canMove())
			// < M.A.D. Nukes End   >
			{
				return true;
			}
			break;

		// < M.A.D. Nukes Start >
		case INTERFACEMODE_PRETARGET_NUKE:
			if (pLoopUnit->canNuke(pLoopUnit->plot()) && GET_PLAYER(pLoopUnit->getOwnerINLINE()).isEnabledMAD() && !pLoopUnit->isMADEnabled() && getHeadUnit()->getUnitInfo().getUnitAIType(UNITAI_ICBM) && pLoopUnit->canMove())
			{
				return true;
			}
			break;
		// < M.A.D. Nukes End   >

		case INTERFACEMODE_RECON:
			if (pLoopUnit->canRecon(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_PARADROP:
			if (pLoopUnit->canParadrop(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRBOMB:
			if (pLoopUnit->canAirBomb(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_RANGE_ATTACK:
			if (pLoopUnit->canRangeStrike())
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRSTRIKE:
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				if (pLoopUnit->canAirAttack())
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_REBASE:
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				return true;
			}
			break;
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
		// Dale - AB: Bombing START
		case INTERFACEMODE_AIRBOMB1:
			if (pLoopUnit->canAirBomb1(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRBOMB2:
			if (pLoopUnit->canAirBomb2(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRBOMB3:
			if (pLoopUnit->canAirBomb3(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRBOMB4:
			if (pLoopUnit->canAirBomb4(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRBOMB5:
			if (pLoopUnit->canAirBomb5(pLoopUnit->plot()))
			{
				return true;
			}
			break;
		// Dale - AB: Bombing END

		// Dale - RB: Field Bombard START
		case INTERFACEMODE_BOMBARD:
			if (pLoopUnit->canRBombard())
			{
				return true;
			}
			break;
		// Dale - RB: Field Bombard END

		// Dale - ARB: Archer Bombard START
		case INTERFACEMODE_ABOMBARD:
			if (pLoopUnit->canArcherBombard(pLoopUnit->plot()) && GC.isDCM_ARCHER_BOMBARD())
			{
				return true;
			}
			break;
		// Dale - ARB: Archer Bombard END

		// Dale - FE: Fighters START
		case INTERFACEMODE_FENGAGE:
			if (pLoopUnit->canFEngage(pLoopUnit->plot()) && GC.isDCM_FIGHTER_ENGAGE())
			{
				return true;
			}
			break;
		// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 09/16/10                                               */
/*                                                                                              */
/* Advanced Automations                                                                         */
/************************************************************************************************/
		case INTERFACEMODE_SHADOW_UNIT:
			if (pLoopUnit->canShadow())
			{
				return true;
			}
			break;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}


// Returns true if one of the units can execute the interface mode at the specified plot...
bool CvSelectionGroup::canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		switch (eInterfaceMode)
		{
		case INTERFACEMODE_AIRLIFT:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canAirliftAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_NUKE:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canNukeAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
				{
					return true;
				}
			}
			break;
		// < M.A.D. Nukes Start >
		case INTERFACEMODE_PRETARGET_NUKE:
			if (pLoopUnit != NULL)
			{
				if (pPlot->isRevealed(pLoopUnit->getTeam(), false) && pPlot->isCity() && pLoopUnit->canNukeAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE(), false) && GET_PLAYER(pLoopUnit->getOwnerINLINE()).isEnabledMAD() && !pLoopUnit->isMADEnabled() && getHeadUnit()->getUnitInfo().getUnitAIType(UNITAI_ICBM) && pLoopUnit->canMove())
				{
					return true;
				}
			}
			break;
		// < M.A.D. Nukes End   >
		case INTERFACEMODE_RECON:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canReconAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_PARADROP:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canParadropAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_AIRBOMB:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canAirBombAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_RANGE_ATTACK:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canRangeStrikeAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_AIRSTRIKE:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canMoveInto(pPlot, true))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_REBASE:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canMoveInto(pPlot))
				{
					return true;
				}
			}
			break;
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
		// Dale - AB: Bombing START
		case INTERFACEMODE_AIRBOMB1:
			if (pLoopUnit != NULL)
			{
				if (GC.isDCM_AIR_BOMBING())
				{
					if (pLoopUnit->canAirBomb1At(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;

		case INTERFACEMODE_AIRBOMB2:
			if (pLoopUnit != NULL)
			{
				if (GC.isDCM_AIR_BOMBING())
				{
					if (pLoopUnit->canAirBomb2At(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;

		case INTERFACEMODE_AIRBOMB3:
			if (pLoopUnit != NULL)
			{
				if (GC.isDCM_AIR_BOMBING())
				{
					if (pLoopUnit->canAirBomb3At(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;

		case INTERFACEMODE_AIRBOMB4:
			if (pLoopUnit != NULL)
			{
				if (GC.isDCM_AIR_BOMBING())
				{
					if (pLoopUnit->canAirBomb4At(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;

		case INTERFACEMODE_AIRBOMB5:
			if (pLoopUnit != NULL)
			{
				if (GC.isDCM_AIR_BOMBING())
				{
					if (pLoopUnit->canAirBomb5At(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;
		// Dale - AB: Bombing END

		// Dale - RB: Field Bombard START
		case INTERFACEMODE_BOMBARD:
			if (pLoopUnit != NULL)
			{
				if(GC.isDCM_RANGE_BOMBARD())
				{
					if (pLoopUnit->canBombardAtRanged(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;
		// Dale - RB: Field Bombard END

		// Dale - ARB: Archer Bombard START
		case INTERFACEMODE_ABOMBARD:
			if (pLoopUnit != NULL)
			{
				if(GC.isDCM_ARCHER_BOMBARD())
				{
					if (pLoopUnit->canArcherBombardAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;
		// Dale - ARB: Archer Bombard END

		// Dale - FE: Fighters START
		case INTERFACEMODE_FENGAGE:
			if (pLoopUnit != NULL)
			{
				if(GC.isDCM_FIGHTER_ENGAGE())
				{
					if (pLoopUnit->canFEngageAt(pLoopUnit->plot(), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
					{
						return true;
					}
				}
			}
			break;
		// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 09/18/10                                               */
/*                                                                                              */
/* Advanced Automations                                                                         */
/************************************************************************************************/
		case INTERFACEMODE_SHADOW_UNIT:
			if (pLoopUnit != NULL)
			{
				CvUnit* pLoopShadow = NULL;
				CLLNode<IDInfo>* pUnitShadowNode = NULL;
				int iValidShadowUnits = 0;
				pUnitShadowNode = pPlot->headUnitNode();
				while (pUnitShadowNode != NULL)
				{
					pLoopShadow = ::getUnit(pUnitShadowNode->m_data);
					pUnitShadowNode = pPlot->nextUnitNode(pUnitShadowNode);
					if (pLoopUnit->canShadowAt(pPlot, pLoopShadow))
					{
						iValidShadowUnits++;
					}
				}
				if (iValidShadowUnits > 0)
				{
					return true;
				}
			}
			break;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

		default:
			return true;
			break;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}


bool CvSelectionGroup::isHuman() const
{
	if (getOwnerINLINE() != NO_PLAYER)
	{
		return GET_PLAYER(getOwnerINLINE()).isHuman();
	}

	return true;
}


bool CvSelectionGroup::isBusy()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	//CvPlot* pPlot;

	if (getNumUnits() == 0)
	{
		return false;
	}

	if (getMissionTimer() > 0)
	{
		return true;
	}

	//pPlot = plot();

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->isCombat())
			{
				return true;
			}
		}
	}

	return false;
}


bool CvSelectionGroup::isCargoBusy()
{
	CLLNode<IDInfo>* pUnitNode1;
	CLLNode<IDInfo>* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	CvPlot* pPlot;

	if (getNumUnits() == 0)
	{
		return false;
	}

	pPlot = plot();

	pUnitNode1 = headUnitNode();

	while (pUnitNode1 != NULL)
	{
		pLoopUnit1 = ::getUnit(pUnitNode1->m_data);
		pUnitNode1 = nextUnitNode(pUnitNode1);

		if (pLoopUnit1 != NULL)
		{
			if (pLoopUnit1->hasCargo())
			{
				pUnitNode2 = pPlot->headUnitNode();

				while (pUnitNode2 != NULL)
				{
					pLoopUnit2 = ::getUnit(pUnitNode2->m_data);
					pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

					if (pLoopUnit2->getTransportUnit() == pLoopUnit1)
					{
						if (pLoopUnit2->getGroup()->isBusy())
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


int CvSelectionGroup::baseMoves()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iValue;
	int iBestValue;

	iBestValue = MAX_INT;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		iValue = pLoopUnit->baseMoves();

		if (iValue < iBestValue)
		{
			iBestValue = iValue;
		}
	}

	return iBestValue;
}


bool CvSelectionGroup::isWaiting() const
{
	return ((getActivityType() == ACTIVITY_HOLD) ||
		      (getActivityType() == ACTIVITY_SLEEP) ||
					(getActivityType() == ACTIVITY_HEAL) ||
					(getActivityType() == ACTIVITY_SENTRY) ||
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
					(getActivityType() == ACTIVITY_SENTRY_WHILE_HEAL) ||
					(getActivityType() == ACTIVITY_SENTRY_NAVAL_UNITS) ||
					(getActivityType() == ACTIVITY_SENTRY_LAND_UNITS) ||
#endif
// BUG - Sentry Actions - end
					(getActivityType() == ACTIVITY_PATROL) ||
					(getActivityType() == ACTIVITY_PLUNDER) ||
					(getActivityType() == ACTIVITY_INTERCEPT));
}


bool CvSelectionGroup::isFull()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		// do two passes, the first pass, we ignore units with speical cargo
		int iSpecialCargoCount = 0;
		int iCargoCount = 0;
		
		// first pass, count but ignore special cargo units
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			
			if (pLoopUnit->cargoSpace() > 0)
			{
				iCargoCount++;
			}

			if (pLoopUnit->specialCargo() != NO_SPECIALUNIT)
			{
				iSpecialCargoCount++;
			}
			else if (!(pLoopUnit->isFull()))
			{
				return false;
			}
		}
		
		// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
		if (iSpecialCargoCount >= iCargoCount)
		{
			pUnitNode = headUnitNode();
			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				
				if (!(pLoopUnit->isFull()))
				{
					return false;
				}
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::hasCargo()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->hasCargo())
		{
			return true;
		}
	}

	return false;
}

//Call for volume when you want the total cargo volume held by the group
int CvSelectionGroup::getCargo(bool bVolume) const
{
	int iCargoCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		
		if (bVolume && GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			iCargoCount += pLoopUnit->SMgetCargo();
		}
		else
		{
			iCargoCount += pLoopUnit->getCargo();
		}
	}

	return iCargoCount;
}

bool CvSelectionGroup::canAllMove()
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			FAssertMsg(pLoopUnit != NULL, "existing node, but NULL unit");

			if (pLoopUnit != NULL && !(pLoopUnit->canMove()))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::canAnyMove(bool bValidate)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	bool bResult = false;
	bool bFoundNonMoveCapable = false;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canMove())
		{
			bResult = true;
			if ( !bValidate )
			{
				break;
			}
		}
		else
		{
			bFoundNonMoveCapable = true;
		}
	}

	if ( bResult && bValidate && bFoundNonMoveCapable )
	{
		//	Selection group has both units that can and cannot move further
		//	If we're validating the state then such groups should be busy (else
		//	a WFoC will result), so if they are not set them to sleep for this
		//	turn
		if ( !isBusy() && getActivityType() == ACTIVITY_AWAKE )
		{
			setActivityType(ACTIVITY_SLEEP);
		}
	}

	return bResult;
}

bool CvSelectionGroup::hasMoved()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->hasMoved())
		{
			return true;
		}
	}

	return false;
}


bool CvSelectionGroup::canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (!(pLoopUnit->canEnterTerritory(eTeam, bIgnoreRightOfPassage)))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool CvSelectionGroup::canEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (!(pLoopUnit->canEnterArea(eTeam, pArea, bIgnoreRightOfPassage)))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::canMoveInto(CvPlot* pPlot, bool bAttack)
{
	return canMoveIntoWithWar(pPlot, bAttack, false);
}

bool CvSelectionGroup::canMoveIntoWithWar(CvPlot* pPlot, bool bAttack, bool bDeclareWar)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->canMoveInto(pPlot, bAttack, false, false, false))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvSelectionGroup::canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		bool bTryCanAttackUnits = true;

		do
		{
			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if ( (!bTryCanAttackUnits || pLoopUnit->canAttack()) && pLoopUnit->canMoveOrAttackInto(pPlot, bDeclareWar))
				{
					return true;
				}
			}

			bTryCanAttackUnits = !bTryCanAttackUnits;
		} while(!bTryCanAttackUnits);
	}

	return false;
}


bool CvSelectionGroup::canMoveThrough(CvPlot* pPlot, bool bDeclareWar)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
#define	MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED	3
	int	unitCharacteristics[MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED];
	int numUniqueUnitCategories = 0;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			int unitMovementCharacteristics = pLoopUnit->getMovementCharacteristicsHash();
			int iI;

			//	If we've already considred a unit with these characteristics no need to
			//	check this one too
			for(iI = 0; iI < numUniqueUnitCategories; iI++)
			{
				if ( unitMovementCharacteristics == unitCharacteristics[iI] )
				{
					break;
				}
			}

//#define TEST_NEW_ALGORITHM_VALIDITY
#ifndef TEST_NEW_ALGORITHM_VALIDITY
			if ( iI == numUniqueUnitCategories )
#endif
			{
				if (!(pLoopUnit->canMoveThrough(pPlot, bDeclareWar)))
				{
					FAssert( iI == numUniqueUnitCategories );
					return false;
				}

#ifdef TEST_NEW_ALGORITHM_VALIDITY
				if ( iI == numUniqueUnitCategories )
#endif
				if ( numUniqueUnitCategories < MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED )
				{
					unitCharacteristics[numUniqueUnitCategories++] = unitMovementCharacteristics;
				}
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::canFight()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canFight())
		{
			return true;
		}
	}

	return false;
}


bool CvSelectionGroup::canDefend()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canDefend())
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::hasBombardCapability() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->bombardRate() > 0)
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::hasCollateralDamage() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->collateralDamage() > 0)
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::canBombard(const CvPlot* pPlot, bool bCheckCanReduceOnly)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canBombard(pPlot, bCheckCanReduceOnly))
		{
			return true;
		}
/************************************************************************************************/
/* DCM                               04/19/09                           Johny Smith      */
/************************************************************************************************/		
		// Dale - RB: Field Bombard START
		if (!bCheckCanReduceOnly && pLoopUnit->canRBombard())
		{
			return true;
		}
		// Dale - RB: Field Bombard END
		// Dale - ARB: Archer Bombard START
		if (!bCheckCanReduceOnly && pLoopUnit->canArcherBombard(pPlot))
		{
			return true;
		}
		// Dale - ARB: Archer Bombard END
/************************************************************************************************/
/* DCM                               End                               Johny Smith        */
/************************************************************************************************/		
	}

	return false;
}

bool CvSelectionGroup::canPillage(const CvPlot* pPlot)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canPillage(pPlot))
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::canBombardAtRanged(const CvPlot* pPlot, int iX, int iY)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canBombardAtRanged(pPlot, iX, iY))
		{
			return true;
		}
	}

	return false;
}

int CvSelectionGroup::getMinimumRBombardDamageLimit() const
{
	int iLowest = MAX_INT;
	int iTemp = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		iTemp = pLoopUnit->rBombardDamageLimit();
		if (iTemp > 0 && iTemp < iLowest)
		{
			iLowest = pLoopUnit->rBombardDamageLimit();
		}
	}

	if (iLowest == MAX_INT)
	{
		iLowest = 0;
	}
	return iLowest;
}

int CvSelectionGroup::getMinimumRBombardRange() const
{
	int iLowest = MAX_INT;
	int iTemp = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		iTemp = pLoopUnit->rBombardDamageLimit();
		if (iTemp > 0 && iTemp < iLowest)
		{
			iLowest = pLoopUnit->getDCMBombRange();
		}
	}

	if (iLowest == MAX_INT)
	{
		iLowest = 0;
	}
	return iLowest;
}

int CvSelectionGroup::getRBombardDamageMaxUnits() const
{
	int iHighest = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->rBombardDamageLimit() > iHighest)
		{
			iHighest = pLoopUnit->rBombardDamageMaxUnits();
		}
	}

	return iHighest;
}

bool CvSelectionGroup::visibilityRange()
{
	int iMaxRange = 0;
	
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		int iRange = pLoopUnit->visibilityRange();
		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
		}
	}

	return iMaxRange;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/30/10                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
//
// Approximate how many turns this group would take to reduce pCity's defense modifier to zero
//
int CvSelectionGroup::getBombardTurns(CvCity* pCity)
{
	PROFILE_FUNC();

	bool bHasBomber = (getOwnerINLINE() != NO_PLAYER ? (GET_PLAYER(getOwnerINLINE()).AI_calculateTotalBombard(DOMAIN_AIR) > 0) : false);
	bool bIgnoreBuildingDefense = bHasBomber;
	int iTotalBombardRate = (bHasBomber ? 16 : 0);
	int iUnitBombardRate = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if( pLoopUnit->bombardRate() > 0 )
		{
			iUnitBombardRate = pLoopUnit->bombardRate();

			if( pLoopUnit->ignoreBuildingDefense() )
			{
				bIgnoreBuildingDefense = true;
			}
			else
			{
				iUnitBombardRate *= std::max(25, (100 - pCity->getBuildingBombardDefense()));
				iUnitBombardRate /= 100;
			}

			iTotalBombardRate += iUnitBombardRate;
		}
	}


	if( pCity->getTotalDefense(bIgnoreBuildingDefense) == 0 )
	{
		return 0;
	}

	int iBombardTurns = pCity->getTotalDefense(bIgnoreBuildingDefense);

	if( iTotalBombardRate > 0 )
	{
		iBombardTurns = (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDefenseDamage());
		iBombardTurns *= pCity->getTotalDefense(false);
		iBombardTurns += (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate) - 1;
		iBombardTurns /= std::max(1, (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate));
	}

	//if( gUnitLogLevel > 2 ) logBBAI("      Bombard of %S will take %d turns at rate %d and current damage %d with bombard def %d", pCity->getName().GetCString(), iBombardTurns, iTotalBombardRate, pCity->getDefenseDamage(), (bIgnoreBuildingDefense ? 0 : pCity->getBuildingBombardDefense()));

	return iBombardTurns;
}

bool CvSelectionGroup::isHasPathToAreaPlayerCity( PlayerTypes ePlayer, int iFlags, bool bGo)
{
	PROFILE_FUNC();

	CvCity* pLoopCity = NULL;
	int iLoop;

	CvReachablePlotSet plotSet(this, iFlags, MAX_INT, false);

	for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if( pLoopCity->area() == area() && plotSet.find(pLoopCity->plot()) != plotSet.end() )
		{
			if ( (iFlags & MOVE_IGNORE_DANGER) != 0 || getHeadUnit()->canAttack() || getHeadUnit()->isBlendIntoCity())
			{
				if (generatePath(plot(), pLoopCity->plot(), iFlags, true) )
				{
					if (!bGo)
					{
						return true;
					}
					else
					{
						int iX = pLoopCity->plot()->getX_INLINE();
						int iY = pLoopCity->plot()->getY_INLINE();
						return pushMissionInternal(MISSION_MOVE_TO, iX, iY, iFlags);
					}
				}
			}
			else if ( generatePath(plot(), pLoopCity->plot(), iFlags, true) )
			{
				return true;
			}
			else
			{
				FAssertMsg(false, "Pathing failed to apparently recahable city");
			}
		}
	}

	return false;
}

bool CvSelectionGroup::isHasPathToAreaEnemyCity( bool bIgnoreMinors, int iFlags, bool bGo )
{
	PROFILE_FUNC();

	//int iPass = 0;

	for(int iI = 0; iI < MAX_PLAYERS; iI++ )
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && isPotentialEnemy(getTeam(), GET_PLAYER((PlayerTypes)iI).getTeam()) )
		{
			if( !bIgnoreMinors || (!GET_PLAYER((PlayerTypes)iI).isNPC() && !GET_PLAYER((PlayerTypes)iI).isMinorCiv()) )
			{
				if( isHasPathToAreaPlayerCity((PlayerTypes)iI, iFlags, bGo) )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvSelectionGroup::isStranded()
{
	PROFILE_FUNC();

	if( !(m_bIsStrandedCacheValid) )
	{
		m_bIsStrandedCache = calculateIsStranded();
		m_bIsStrandedCacheValid = true;
	}
		
	return m_bIsStrandedCache;
}

void CvSelectionGroup::invalidateIsStrandedCache()
{
	m_bIsStrandedCacheValid = false;
}

bool CvSelectionGroup::calculateIsStranded()
{
	PROFILE_FUNC();

	if( getNumUnits() <= 0 )
	{
		return false;
	}

	if( plot() == NULL )
	{
		return false;
	}

	if( getDomainType() != DOMAIN_LAND )
	{
		return false;
	}

	if( (getActivityType() != ACTIVITY_AWAKE) && (getActivityType() != ACTIVITY_HOLD) )
	{
		return false;
	}

	if( AI_getMissionAIType() != NO_MISSIONAI )
	{
		return false;
	}

	if( getLengthMissionQueue() > 0 )
	{
		return false;
	}
	
	if( !canAllMove() )
	{
		return false;
	}

	if( getHeadUnit()->isCargo() )
	{
		return false;
	}

	if( plot()->area()->getNumUnrevealedTiles(getTeam()) > 0 )
	{
		if( (getHeadUnitAI() == UNITAI_ATTACK) || (getHeadUnitAI() == UNITAI_EXPLORE) )
		{
			return false;
		}
	}

	int iBestValue;
	if( (getHeadUnitAI() == UNITAI_SETTLE) && (GET_PLAYER(getOwner()).AI_getNumAreaCitySites(getArea(), iBestValue) > 0) )
	{
		return false;
	}

	if( plot()->area()->getCitiesPerPlayer(getOwner()) == 0 )
	{
		int iBestValue;
		if( (plot()->area()->getNumAIUnits(getOwner(),UNITAI_SETTLE) > 0) && (GET_PLAYER(getOwner()).AI_getNumAreaCitySites(getArea(), iBestValue) > 0) )
		{
			return false;
		}
	}

	if( plot()->area()->getNumCities() > 0 )
	{
		if( getHeadUnit()->AI_getUnitAIType() == UNITAI_SPY )
		{
			return false;
		}

		if( plot()->getImprovementType() != NO_IMPROVEMENT )
		{
			if( GC.getImprovementInfo(plot()->getImprovementType()).isActsAsCity() && canDefend() )
			{
				return false;
			}
		}

		if( plot()->isCity() && (plot()->getOwner() == getOwner()) )
		{
			return false;
		}

		//	Since we are considering a reachable enemy city as not stranded it follows
		//	that we expect situations where we can only attack are not considered stranded,
		//	so include the MOVE_THROUGH_ENEMY flag for consistency
		if( isHasPathToAreaPlayerCity(getOwner(), MOVE_THROUGH_ENEMY) )
		{
			return false;
		}

		if( isHasPathToAreaEnemyCity(false, MOVE_IGNORE_DANGER | MOVE_THROUGH_ENEMY) )
		{
			return false;
		}
	}

	return true;
}

bool CvSelectionGroup::canMoveAllTerrain() const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!(pLoopUnit->canMoveAllTerrain()))
		{
			return false;
		}
	}

	return true;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

void CvSelectionGroup::unloadAll()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->unloadAll();
		}
	}
}

bool CvSelectionGroup::alwaysInvisible() const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (!(pLoopUnit->alwaysInvisible()))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::isInvisible(TeamTypes eTeam) const
{
	if (getNumUnits() > 0)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (!pLoopUnit->isInvisible(eTeam, false))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}


int CvSelectionGroup::countNumUnitAIType(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	FAssertMsg(headUnitNode() != NULL, "headUnitNode() is not expected to be equal with NULL");

	iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		
		// count all units if NO_UNITAI passed in
		if (NO_UNITAI == eUnitAI || pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			iCount++;
		}
	}

	return iCount;
}


bool CvSelectionGroup::hasWorker()
{
	return ((countNumUnitAIType(UNITAI_WORKER) > 0) || (countNumUnitAIType(UNITAI_WORKER_SEA) > 0));
}


bool CvSelectionGroup::IsSelected()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->IsSelected())
		{
			return true;
		}
	}

	return false;
}


void CvSelectionGroup::NotifyEntity(MissionTypes eMission)
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		::getUnit(pUnitNode->m_data)->NotifyEntity(eMission);
		pUnitNode = nextUnitNode(pUnitNode);
	}
}


void CvSelectionGroup::airCircle(bool bStart)
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		::getUnit(pUnitNode->m_data)->airCircle(bStart);
		pUnitNode = nextUnitNode(pUnitNode);
	}
}


void CvSelectionGroup::setBlockading(bool bStart)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		::getUnit(pUnitNode->m_data)->setBlockading(bStart);
		pUnitNode = nextUnitNode(pUnitNode);
	}
}


int CvSelectionGroup::getX() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getX_INLINE();
	}
	else
	{
		return INVALID_PLOT_COORD;
	}
}


int CvSelectionGroup::getY() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getY_INLINE();
	}
	else
	{
		return INVALID_PLOT_COORD;
	}
}


bool CvSelectionGroup::at(int iX, int iY) const
{
	return((getX() == iX) && (getY() == iY));
}


bool CvSelectionGroup::atPlot( const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvSelectionGroup::plot() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->plot();
	}
	else
	{
		return NULL;
	}
}

CvPlot* CvSelectionGroup::plotExternal() const
{
	CvPlot* pResult = plot();

	FAssert(pResult == NULL || pResult->isInViewport());

	return pResult;
}


int CvSelectionGroup::getArea() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getArea();
	}
	else
	{
		return NULL;
	}
}

CvArea* CvSelectionGroup::area() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->area();
	}
	else
	{
		return NULL;
	}
}


DomainTypes CvSelectionGroup::getDomainType() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getDomainType();
	}
	else
	{
		return NO_DOMAIN;
	}
}


RouteTypes CvSelectionGroup::getBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	RouteTypes eRoute;
	RouteTypes eBestRoute;
	int iValue;
	int iBestValue;
	int iI;

	if (peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	iBestValue = 0;
	eBestRoute = NO_ROUTE;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		for (iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			eRoute = ((RouteTypes)(GC.getBuildInfo((BuildTypes) iI).getRoute()));

			if (eRoute != NO_ROUTE)
			{
				if (pLoopUnit->canBuild(pPlot, ((BuildTypes)iI)))
				{
					iValue = GC.getRouteInfo(eRoute).getValue();

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestRoute = eRoute;
						if (peBestBuild != NULL)
						{
							*peBestBuild = ((BuildTypes)iI);
						}
					}
				}
			}
		}
	}

	return eBestRoute;
}

bool CvSelectionGroup::canIgnoreZoneofControl()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();
	if (getNumUnits() > 0)
	{
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (!pLoopUnit->canIgnoreZoneofControl())
			{
				return false;
			}
		}
		
		return true;
	}
	return false;
}

// Returns true if group was bumped...
bool CvSelectionGroup::groupDeclareWar(CvPlot* pPlot, bool bForce)
{
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	TeamTypes ePlotTeam = pPlot->getTeam();
	
	if (!AI_isDeclareWar(pPlot))
	{
		return false;
	}

	int iNumUnits = getNumUnits();

	if (bForce || !canEnterArea(ePlotTeam, pPlot->area(), true))
	{
		if (ePlotTeam != NO_TEAM && kTeam.AI_isSneakAttackReady(ePlotTeam))
		{
			if (kTeam.canDeclareWar(ePlotTeam))
			{
				kTeam.declareWar(ePlotTeam, true, NO_WARPLAN);
			}
		}
	}

	return (iNumUnits != getNumUnits());
}


// Returns true if attack was made...
bool CvSelectionGroup::groupAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting, bool bStealth, CvPlot* pOrigPlot)
{
	PROFILE_FUNC();

	// Dale - SA: Stack Attack START
	if (GC.isDCM_STACK_ATTACK())
	{
		return groupStackAttack(iX, iY, iFlags, bFailedAlreadyFighting, bStealth);
	}
	// Dale - SA: Stack Attack END

	if (pOrigPlot == NULL)
	{
		pOrigPlot = plot();
	}
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pDestPlot = GC.getMapINLINE().plotINLINE(iX, iY);
	// RevolutionDCM - end

	if (iFlags & MOVE_THROUGH_ENEMY)
	{
		if (generatePath(plot(), pDestPlot, iFlags))
		{
			pDestPlot = getPathFirstPlot();
		}
	}
	FAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	bool bStack = (isHuman() && ((getDomainType() == DOMAIN_AIR) || GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_STACK_ATTACK)));
    
	bool bAttack = false;
	bFailedAlreadyFighting = false;
	bool bStealthDefense = bStealth;
	bool bAffixFirstAttacker = false;
	bool bAffixFirstDefender = false;

	if (getNumUnits() > 0)
	{
		if ((getDomainType() == DOMAIN_AIR) || (stepDistance(getX(), getY(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()) <= 1))
		{
			if ((iFlags & MOVE_DIRECT_ATTACK) || (getDomainType() == DOMAIN_AIR) || (iFlags & MOVE_THROUGH_ENEMY) || (generatePath(plot(), pDestPlot, iFlags) && (getPathFirstPlot() == pDestPlot) || bStealth))
			{
				int iAttackOdds = 0;
				CvUnit* pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, true, iAttackOdds, bStealthDefense, false, 0, false, bStealthDefense);
				if (pBestAttackUnit)
				{
					// if there are no defenders, do not attack
					//TB Surprise Defense
					// The battle must be set differently so that stealth combat factors apply. But only in battle with THIS defender...
					if (!pDestPlot->hasDefender(false, NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, true))
					{
						if (pDestPlot->hasStealthDefender(pBestAttackUnit) && !pDestPlot->isCity(false))
						{
							//reveal!
							pDestPlot->revealBestStealthDefender(pBestAttackUnit);
							bStealthDefense = true;
							bAffixFirstAttacker = true;
						}
					}

					if (isHuman())
					{
						if (!(isLastPathPlotVisible()) && (getDomainType() != DOMAIN_AIR))
						{
							return false;
						}
					}

					CvUnit* pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, false, bStealthDefense);
					if (pBestDefender != NULL)
					{
						bAffixFirstDefender = true;
					}

					//if (isHuman())
					//{
					//	if (!(isLastPathPlotVisible()) && (getDomainType() != DOMAIN_AIR))
					//	{
					//		return false;
					//	}
					//}
// BUG - Safe Move - end

					bool bNoBlitz = (!pBestAttackUnit->isBlitz() && pBestAttackUnit->isMadeAttack() && !bStealth);

					if (groupDeclareWar(pDestPlot))
					{
						return true;
					}

					if (NULL == pBestDefender)//Not a part of groupStackAttack - unsure if this is necessary or performing a function.
					{
						return false;
					}

					// Dale - BE: Battle Effect START
					pBestAttackUnit->setBattlePlot(pDestPlot, pBestDefender);

					// RevolutionDCM - attack support
					if (GC.isDCM_ATTACK_SUPPORT())
					{
						if (pDestPlot->getNumUnits() > 0 && !bStealth)
						{
							pUnitNode = pDestPlot->headUnitNode();
							while (pUnitNode != NULL)
							{
								pLoopUnit = ::getUnit(pUnitNode->m_data);
								pUnitNode = pDestPlot->nextUnitNode(pUnitNode);
								if (pLoopUnit == NULL)
									break;
								if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(getTeam()))
								{
									if (plot() != NULL)
									{
										if (pLoopUnit->canArcherBombardAt(pDestPlot, plot()->getX_INLINE(), plot()->getY_INLINE()))
										{
											if (pLoopUnit->archerBombard(plot()->getX_INLINE(), plot()->getY_INLINE(), true))
											{
											}
										}
										else if (pLoopUnit->canBombardAtRanged(pDestPlot, plot()->getX_INLINE(), plot()->getY_INLINE()))
										{
											if (pLoopUnit->bombardRanged(plot()->getX_INLINE(), plot()->getY_INLINE(), true))
											{
											}
										}
										else if (pLoopUnit->getDomainType() == DOMAIN_AIR && !pLoopUnit->isSuicide())
										{
											pLoopUnit->updateAirStrike(plot(), false, true);//airStrike(plot()))
										}
										pLoopUnit->setMadeAttack(false);
									}
								}
							}
						} 
						else 
						{
							return bAttack;
						}
						if (pOrigPlot->getNumUnits() > 0 && !bStealth)
						{
							pUnitNode = pOrigPlot->headUnitNode();
							while (pUnitNode != NULL)
							{
								pLoopUnit = ::getUnit(pUnitNode->m_data);
								pUnitNode = pOrigPlot->nextUnitNode(pUnitNode);
								if (pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
								{
									if (pLoopUnit != NULL && this != NULL && pDestPlot != NULL && plot() != NULL)
									{
										if (pLoopUnit->canArcherBombardAt(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
										{
											if (pLoopUnit->archerBombard(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), true))
											{
											}
										}
										else if (pLoopUnit->canBombardAtRanged(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
										{
											if (pLoopUnit->bombardRanged(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), true))
											{
											}
										}
										else if (pLoopUnit->getDomainType() == DOMAIN_AIR && !pLoopUnit->isSuicide())
										{
											if (plotDistance(plot()->getX_INLINE(), plot()->getY_INLINE(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()) == 1)
											{
												pLoopUnit->updateAirStrike(pDestPlot, false, false);//airStrike(pDestPlot))
											}
										}
										pLoopUnit->setMadeAttack(false);
									}
								}
							}
						} 
						else 
						{
							return bAttack;
						}
					}
					// RevolutionDCM - attack support end

					bool bBombardExhausted = false;
					bool bLoopStealthDefense = false;
					while (true)
					{
						PROFILE("CvSelectionGroup::groupAttack.StackLoop");
						if (bLoopStealthDefense)
						{
							bStealthDefense = false;
						}
						if (bStealthDefense)
						{
							bLoopStealthDefense = true;
						}
						if (!bAffixFirstAttacker)
						{
							pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, false, iAttackOdds, bLoopStealthDefense, bNoBlitz, 0, false, bStealth);
						}
						if (pBestAttackUnit == NULL/* || !pBestAttackUnit->canMoveInto(pDestPlot, true, false, false, false, false, false, 0, false, false, bStealthDefense)*/)//TB: I realize this was probably placed here for a reason, BUT, that reason may have been negated at a point by a later debug effort AND if it is NOT necessary then it is a major waste of processing time.  groupStackAttack has been getting away without it.
						{
							break;
						}
						else
						{
							// if there are no defenders, do not attack
							if (!bAffixFirstAttacker && !pDestPlot->hasDefender(false, NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, true))
							{
								if (pDestPlot->hasStealthDefender(pBestAttackUnit) && !pDestPlot->isCity(false))
								{
									pDestPlot->revealBestStealthDefender(pBestAttackUnit);
									bStealth = true;
								}
							}
							if (!bAffixFirstDefender)
							{
								pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, false, true);
							}
							bAffixFirstAttacker = false;
							bAffixFirstDefender = false;
						}

						//if (!pBestAttackUnit->canMoveInto(pDestPlot, true, false, false, false, false, false, 0, false, false, bStealthDefense))
						//{
						//	break;
						//}//TB again, not sure this is necessary

						if (iAttackOdds < 68 && !isHuman() && !bStealth)
						{
							if (bBombardExhausted)
							{
								CvUnit * pBestSacrifice = AI_getBestGroupSacrifice(pDestPlot, false, false, bNoBlitz, bStealth);
								if (pBestSacrifice != NULL && pBestSacrifice->canMoveInto(pDestPlot, true, false, false, false, false, false, 0, false, false, bStealthDefense))
								{
									pBestAttackUnit = pBestSacrifice;
								}
							}
							else
							{
								bool bFoundBombard = false;

								OutputDebugString("Attempting to bombard tough plot\n");
								if (pOrigPlot->getNumUnits() > 0)
								{
									pUnitNode = headUnitNode();
									while (pUnitNode != NULL)
									{
										pLoopUnit = ::getUnit(pUnitNode->m_data);
										pUnitNode = nextUnitNode(pUnitNode);

										if (pLoopUnit != NULL && this != NULL && pDestPlot != NULL && plot() != NULL)
										{
											/*if (pLoopUnit->canArcherBombardAt(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
											{
												bFoundBombard = true;
												pLoopUnit->archerBombard(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), false);
											}
											else */if (pLoopUnit->canBombardAtRanged(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
											{
												bFoundBombard = true;
												pLoopUnit->bombardRanged(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), false);
											}
										}
									}
								}

								bBombardExhausted = !bFoundBombard;

								continue;
							}
						}

						bAttack = true;

						if (GC.getUSE_CAN_DO_COMBAT_CALLBACK())
						{

							PROFILE("CvSelectionGroup::groupAttack.Python");

							CySelectionGroup* pyGroup = new CySelectionGroup(this);
							CyPlot* pyPlot = new CyPlot(pDestPlot);
							CyArgsList argsList;
							argsList.add(gDLL->getPythonIFace()->makePythonObject(pyGroup));	// pass in Selection Group class
							argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));	// pass in Plot class
							long lResult=0;
							PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doCombat", argsList.makeFunctionArgs(), &lResult);
							delete pyGroup;	// python fxn must not hold on to this pointer 
							delete pyPlot;	// python fxn must not hold on to this pointer 
							if (lResult == 1)
							{
								break;
							} 
						}
						
						if (getNumUnits() > 1)
						{
							if ((!bLoopStealthDefense) && (pBestAttackUnit->plot()->isFighting() || pDestPlot->isFighting()))
							{
								bFailedAlreadyFighting = true;
							}
							else
							{
								//TB: This is a fix for (standard bts) stack attack.
								//For more information, see the notes in updateAttack that start with the same wording as the above line.
								int iPlus = 0;
								if (pDestPlot->hasStealthDefender(pBestAttackUnit))
								{
									iPlus++;
								}
								bool bMore = pDestPlot->getNumVisiblePotentialEnemyDefenders(pBestAttackUnit) + iPlus > 1;
								bool bQuick = (bStack || bMore || bLoopStealthDefense);
								if (pBestDefender)
								{
									pBestAttackUnit->attack(pDestPlot, bQuick, bLoopStealthDefense);
								}
								else
								{
									break;
								}
							}
						}
						else if (pBestDefender)
						{
							pBestAttackUnit->attack(pDestPlot, false, bLoopStealthDefense);
							break;
						}
						else
						{
							break;
						}
/************************************************************************************************/
/* Afforess	                  Start		 6/20/11                                                */
/*                                                                                              */
/* Attempt to break stack apart for human after initial attack                                  */
/* Allows for the human to continue to use the rest of the stack to attack                      */
/************************************************************************************************/
						
						if (bFailedAlreadyFighting && bStack && isHuman())
						{
							pBestAttackUnit->joinGroup(NULL);
						}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

						if (bFailedAlreadyFighting || !bStack)
						{
							// if this is AI stack, follow through with the attack to the end
							//Allow Automated Units to Stack Attack
							if ((!isHuman() || isAutomated()) && getNumUnits() > 1)
							{
								AI_queueGroupAttack(iX, iY);
							}
							
							break;
						}
					}
				}
			}
		}
	}

	return bAttack;
}


void CvSelectionGroup::groupMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove)
{
	PROFILE_FUNC();

	//	Inhibit recalculation of the appropriate center unit to display on the
	//	start and end plots until after all units have moved, so it only has to be done
	//	once
	CvPlot* pStartPlot = plot();

	pStartPlot->enableCenterUnitRecalc(false);
	pPlot->enableCenterUnitRecalc(false);
	int iX = pPlot->getX();
	int iY = pPlot->getY();

	m_bIsMidMove = true;

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();
	
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
	bool bSentryAlert = isHuman() && NULL != headMissionQueueNode() && headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_SENTRY && sentryAlertSameDomainType();
#endif
// BUG - Sentry Actions - end

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ( pLoopUnit == NULL)
		{
			//	Koshling  - this really isn't a valid condition I think but it's been seen
			//	and until we can understand the underlying cause, mitigate by ignoring the
			//	invalid unit
			continue;
		}
		if (pLoopUnit->at(iX,iY))
		{
			continue;
		}
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
		// don't move if bSentryAlert set to true above
		if ((!bSentryAlert && pLoopUnit->canMove() && ((bCombat && (!(pLoopUnit->isNoCapture()) || !(pPlot->isEnemyCity(*pLoopUnit)))) ? pLoopUnit->canMoveOrAttackInto(pPlot) : pLoopUnit->canMoveInto(pPlot))) || (pLoopUnit == pCombatUnit))
#else
		//TBNote: Need to make this an option perhaps.  Groups probably shouldn't be automatically splitting up to continue the planned move, particularly for human players.
		//Would check if the whole group can move into the plot first.  This warrants more study before acting on this.
		if ((pLoopUnit->canMove() && ((bCombat && (!(pLoopUnit->isNoCapture()) || !(pPlot->isEnemyCity(*pLoopUnit)))) ? pLoopUnit->canMoveOrAttackInto(pPlot) : pLoopUnit->canMoveInto(pPlot))) || (pLoopUnit == pCombatUnit))
#endif
// BUG - Sentry Actions - end
		{
			pLoopUnit->move(pPlot, true);//next statementperhaps should be an if is dead kind of protection against reporting the move elsewhere//TBFIXHERE
			if (pLoopUnit->isDead())
			{
				pLoopUnit->joinGroup(NULL,true);
				pLoopUnit->finishMoves();
				continue;
			}
		}
		else
		{
			pLoopUnit->joinGroup(NULL, true);
			pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);
/************************************************************************************************/
/* Afforess	                  Start		 07/31/10                                               */
/*                                                                                              */
/* Units Seem to be getting stuck here                                                          */
/************************************************************************************************/
			if (GC.iStuckUnitID != pLoopUnit->getID())
			{
				GC.iStuckUnitID = pLoopUnit->getID();
				GC.iStuckUnitCount = 0;
			}
			else
			{
				GC.iStuckUnitCount++;
				if (GC.iStuckUnitCount > 5)
				{
					FAssertMsg(false, "Unit Stuck in Loop!");
					CvUnit* pHeadUnit = getHeadUnit();
					if (NULL != pHeadUnit)
					{
						TCHAR szOut[1024];
						CvWString szTempString;
						getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
						sprintf(szOut, "Unit stuck in loop: %S(%S)[%d, %d] (%S)\n", pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getName(),
							pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), szTempString.GetCString());
						gDLL->messageControlLog(szOut);
					}
					pLoopUnit->finishMoves();
				}
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
			
		}
	}

	pStartPlot->enableCenterUnitRecalc(true);
	pPlot->enableCenterUnitRecalc(true);

	//execute move
	if(bEndMove || !canAllMove())
	{
		pUnitNode = headUnitNode();
		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);
		}
	}

	m_bIsMidMove = false;
}


// Returns true if move was made...
bool CvSelectionGroup::groupPathTo(int iX, int iY, int iFlags)
{
	PROFILE_FUNC();

	CvPlot* pDestPlot;
	CvPlot* pPathPlot;

	if (at(iX, iY))
	{
		return false; // XXX is this necessary?
	}

	FAssert(!isBusy());
	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssert(headMissionQueueNode() != NULL);

	pDestPlot = GC.getMapINLINE().plotINLINE(iX, iY);
	FAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	FAssertMsg(canAllMove(), "canAllMove is expected to be true");

	if (getDomainType() == DOMAIN_AIR)
	{
		if (!canMoveInto(pDestPlot) && !canMoveInto(pDestPlot, true))
		{
			return false;
		}

		pPathPlot = pDestPlot;
	}
	else
	{
		if (!generatePath(plot(), pDestPlot, iFlags))
		{
			return false;
		}

		pPathPlot = getPathFirstPlot();

		//	If the first plot IS the destination it's possible that generatePath
		//	can suceed, but the group cannot actually move there because the move
		//	would require an attack
		if (!canMoveInto(pPathPlot))
		{
			return false;
		}

		if (groupAmphibMove(pPathPlot, iFlags))
		{
			return false;
		}

		if ( (iFlags & MOVE_WITH_CAUTION) && !canDefend() )
		{
			CvPlot*	endTurnPlot = getPathEndTurnPlot();

			//	If the next plot we'd go to has a danger count above a threshold
			//	consider it not safe and abort so we can reconsider
			if ( endTurnPlot->getDangerCount(getTeam()) > 20 )
			{
				return false;
			}

			//	Also for non-owned territory check for nearby enemies
			if ( endTurnPlot->getOwnerINLINE() != getOwnerINLINE() &&
				 GET_PLAYER(getOwnerINLINE()).AI_getVisiblePlotDanger(endTurnPlot, 2, false) )
			{
				return false;
			}
		}

		if ( (iFlags & MOVE_HEAL_AS_NEEDED25) && getHeadUnit()->getDamagePercent() > 25 )
		{
			if ( !GET_PLAYER(getOwnerINLINE()).AI_getVisiblePlotDanger(plot(), 2, false) &&
				 plot()->getTotalTurnDamage(this) <= 0)
			{
				return false;
			}
		}
	}
	
	bool bForce = false;
	MissionAITypes eMissionAI = AI_getMissionAIType();
	
	/*** Dexy - Fixed Borders START ****/
	if (eMissionAI == MISSIONAI_BLOCKADE || eMissionAI == MISSIONAI_PILLAGE || eMissionAI == MISSIONAI_CLAIM_TERRITORY)
//	OLD CODE
//	if (eMissionAI == MISSIONAI_BLOCKADE || eMissionAI == MISSIONAI_PILLAGE)
	/*** Dexy - Fixed Borders  END  ****/
	{
		bForce = true;
	}

	if (groupDeclareWar(pPathPlot, bForce))
	{
		return false;
	}

	bool bEndMove = false;
	if(pPathPlot == pDestPlot)
		bEndMove = true;
    
	groupMove(pPathPlot, iFlags & MOVE_THROUGH_ENEMY, NULL, bEndMove);

	return true;
}


// Returns true if move was made...
bool CvSelectionGroup::groupRoadTo(int iX, int iY, int iFlags)
{
	CvPlot* pPlot;
	RouteTypes eBestRoute;
	BuildTypes eBestBuild;

	if (!AI_isControlled() || !at(iX, iY) || (getLengthMissionQueue() == 1))
	{
		pPlot = plot();

		eBestRoute = getBestBuildRoute(pPlot, &eBestBuild);

		if (eBestBuild != NO_BUILD)
		{
			groupBuild(eBestBuild);
			return true;
		}
	}

	return groupPathTo(iX, iY, iFlags);
}


// Returns true if build should continue...
bool CvSelectionGroup::groupBuild(BuildTypes eBuild)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	bool bContinue;

	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	bContinue = false;

	pPlot = plot();

    ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
	if (eImprovement != NO_IMPROVEMENT)
	{
		if (AI_isControlled())
		{
			if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION))
			{
				if ((pPlot->getImprovementType() != NO_IMPROVEMENT) && (pPlot->getImprovementType() != (ImprovementTypes)(GC.getDefineINT("RUINS_IMPROVEMENT"))))
				{
				    BonusTypes eBonus = (BonusTypes)pPlot->getNonObsoleteBonusType(GET_PLAYER(getOwnerINLINE()).getTeam());
				    if ((eBonus == NO_BONUS) || !GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eBonus))
				    {
                        if (GC.getImprovementInfo(eImprovement).getImprovementPillage() != NO_IMPROVEMENT)
                        {
                            return false;
                        }
				    }
				}
			}
//
//			if (AI_getMissionAIType() == MISSION_BUILD)
//			{
//                CvCity* pWorkingCity = pPlot->getWorkingCity();
//                if ((pWorkingCity != NULL) && (AI_getMissionAIPlot() == pPlot))
//                {
//                    if (pWorkingCity->AI_getBestBuild(pWorkingCity->getCityPlotIndex(pPlot)) != eBuild)
//                    {
//                        return false;
//                    }
//                }
//			}
		}
	}
	
// BUG - Pre-Chop - start
	bool bCheckChop = false;
	bool bStopOtherWorkers = false;

	FeatureTypes eFeature = pPlot->getFeatureType();
	CvBuildInfo& kBuildInfo = GC.getBuildInfo(eBuild);
	if (eFeature != NO_FEATURE && isHuman() && kBuildInfo.isFeatureRemove(eFeature) && kBuildInfo.getFeatureProduction(eFeature) != 0)
	{
		if (kBuildInfo.getImprovement() == NO_IMPROVEMENT)
		{
			// clearing a forest or jungle
			if (getBugOptionBOOL("Actions__PreChopForests", true, "BUG_PRECHOP_FORESTS"))
			{
				bCheckChop = true;
			}
		}
		else
		{
			if (getBugOptionBOOL("Actions__PreChopImprovements", true, "BUG_PRECHOP_IMPROVEMENTS"))
			{
				bCheckChop = true;
			}
		}
	}
// BUG - Pre-Chop - end

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		FAssertMsg(pLoopUnit->atPlot(pPlot), "pLoopUnit is expected to be at pPlot");

		if (pLoopUnit->canBuild(pPlot, eBuild))
		{
			bContinue = true;

			if (pLoopUnit->build(eBuild))
			{
				bContinue = false;
				break;
			}

// BUG - Pre-Chop - start
			if (bCheckChop && pPlot->getBuildTurnsLeft(eBuild, getOwnerINLINE()) == 1)
			{
				// TODO: stop other worker groups
				CvCity* pCity;
				int iProduction = plot()->getFeatureProduction(eBuild, getTeam(), &pCity);

				if (iProduction > 0)
				{
					MEMORY_TRACK_EXEMPT();

					CvWString szBuffer = gDLL->getText("TXT_KEY_BUG_PRECLEARING_FEATURE_BONUS", GC.getFeatureInfo(eFeature).getTextKeyWide(), iProduction, pCity->getNameKey());
					AddDLLMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, GC.getFeatureInfo(eFeature).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX(), getY(), true, true);
				}
				bContinue = false;
				bStopOtherWorkers = true;
				break;
			}
// BUG - Pre-Chop - end
		}
	}

// BUG - Pre-Chop - start
	if (bStopOtherWorkers)
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
			CvSelectionGroup* pSelectionGroup = pLoopUnit->getGroup();

			if (pSelectionGroup != NULL && pSelectionGroup != this && pSelectionGroup->getOwnerINLINE() == getOwnerINLINE()
					&& pSelectionGroup->getActivityType() == ACTIVITY_MISSION && pSelectionGroup->getLengthMissionQueue() > 0 
					&& pSelectionGroup->getMissionType(0) == kBuildInfo.getMissionType() && pSelectionGroup->getMissionData1(0) == eBuild)
			{
				pSelectionGroup->deleteMissionQueueNode(pSelectionGroup->headMissionQueueNode());
			}
		}
	}
// BUG - Pre-Chop - end

	return bContinue;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/18/10                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
void CvSelectionGroup::setTransportUnit(CvUnit* pTransportUnit, CvSelectionGroup** pOtherGroup)
{
	// if we are loading
	if (pTransportUnit != NULL)
	{
		CvUnit* pHeadUnit = getHeadUnit();
		FAssertMsg(pHeadUnit != NULL, "non-zero group without head unit");
		if (pHeadUnit == NULL)
		{
			return;
		}
		
		int iCargoSpaceAvailable = 0;
		if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			iCargoSpaceAvailable = pTransportUnit->SMcargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());
		}
		else
		{
			iCargoSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());
		}
		// if no space at all, give up
		if (iCargoSpaceAvailable < 1)
		{
			return;
		}

		// if there is space, but not enough to fit whole group, then split us, and set on the new group
		//Definitely only applicable in mechanism to non-sm
		if (!GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			if (iCargoSpaceAvailable < getNumUnits())
			{
				CvSelectionGroup* pSplitGroup = splitGroup(iCargoSpaceAvailable, NULL, pOtherGroup);
				if (pSplitGroup != NULL)
				{
					pSplitGroup->setTransportUnit(pTransportUnit);
				}
				return;
			}
			
			FAssertMsg(iCargoSpaceAvailable >= getNumUnits(), "cargo size too small");
		}
		else if (iCargoSpaceAvailable < getNumUnitCargoVolumeTotal()/*pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType())*/)
		{
			CvSelectionGroup* pSplitGroup = splitGroup(1, NULL, pOtherGroup);
			if (pSplitGroup != NULL)
			{
				if (iCargoSpaceAvailable < pSplitGroup->getHeadUnit()->SMCargoVolume())
				{
					pSplitGroup->setTransportUnit(pTransportUnit);
				}
				else if (pSplitGroup->getHeadUnit()->canSplit())
				{
					pSplitGroup->getHeadUnit()->doSplit();
					setTransportUnit(pTransportUnit);
				}//this could lead to interesting results getting boats really packed with smaller units.
				return;
			}
			FAssertMsg(iCargoSpaceAvailable >= getNumUnitCargoVolumeTotal(), "cargo size too small");
		}
					
		
		// setTransportUnit removes the unit from the current group (at least currently), so we have to be careful in the loop here
		// so, loop until we do not load one
		bool bLoadedOne;
		do
		{
			bLoadedOne = false;

			// loop over all the units, finding one to load
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			while (pUnitNode != NULL && !bLoadedOne)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				
				// just in case implementation of setTransportUnit changes, check to make sure this unit is not already loaded
				if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() != pTransportUnit)
				{
					// if there is room, load the unit and stop the loop (since setTransportUnit ungroups this unit currently)
					bool bSpaceAvailable = 0; 
					if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
					{
						bSpaceAvailable = (pTransportUnit->SMcargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType()) >= pLoopUnit->SMCargoVolume());
					}
					else
					{
						bSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType());
					}
					if (bSpaceAvailable)
					{
						pLoopUnit->setTransportUnit(pTransportUnit);
						bLoadedOne = true;
					}
				}
			}
		}
		while (bLoadedOne);
	}
	// otherwise we are unloading
	else
	{
		// loop over all the units, unloading them
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			
			if (pLoopUnit != NULL)
			{
				// unload unit
				pLoopUnit->setTransportUnit(NULL);
			}
		}
	}
}


/// \brief Function for loading stranded units onto an offshore transport
///
void CvSelectionGroup::setRemoteTransportUnit(CvUnit* pTransportUnit)
{
	// if we are loading
	if (pTransportUnit != NULL)
	{
		CvUnit* pHeadUnit = getHeadUnit();
		if (pHeadUnit == NULL)
		{
			return;
		}
		FAssertMsg(pHeadUnit != NULL, "non-zero group without head unit");
		
		int iCargoSpaceAvailable = 0;
		if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			iCargoSpaceAvailable = pTransportUnit->SMcargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());
		}
		else
		{
			iCargoSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());
		}
		// if no space at all, give up
		if (iCargoSpaceAvailable < 1)
		{
			return;
		}

		// if there is space, but not enough to fit whole group, then split us, and set on the new group
		if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			if (iCargoSpaceAvailable < getNumUnitCargoVolumeTotal())
			{
				CvSelectionGroup* pSplitGroup = splitGroup(1);
				if (pSplitGroup != NULL)
				{
					if (iCargoSpaceAvailable > pSplitGroup->getNumUnitCargoVolumeTotal())
					{
						pSplitGroup->setRemoteTransportUnit(pTransportUnit);
					}
					else if (pSplitGroup->getHeadUnit()->canSplit())
					{
						pSplitGroup->getHeadUnit()->doSplit();
						setRemoteTransportUnit(pTransportUnit);
					}
					//I may be really needing to develop out a lot more to splitting and merging to help transfer ai data...
					//Maybe simply at the beginning of a split or merge, record the mission of the original group to which the unit was assigned then transfer that data to the new unit(s) after the merge/split...
				}
				return;
			}
		}
		else if (iCargoSpaceAvailable < getNumUnits())
		{
			CvSelectionGroup* pSplitGroup = splitGroup(iCargoSpaceAvailable);
			if (pSplitGroup != NULL)
			{
				pSplitGroup->setRemoteTransportUnit(pTransportUnit);
			}
			return;
		}
		
		FAssertMsg(iCargoSpaceAvailable >= getNumUnits(), "cargo size too small");

		bool bLoadedOne;
		do
		{
			bLoadedOne = false;

			// loop over all the units on the plot, looping through this selection group did not work
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			while (pUnitNode != NULL && !bLoadedOne)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				
				if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() != pTransportUnit && pLoopUnit->getOwnerINLINE() == pTransportUnit->getOwnerINLINE())
				{
					bool bSpaceAvailable = 0;
					if (!GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
					{
						bSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType());
					}
					else
					{
						bSpaceAvailable = (pTransportUnit->SMcargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType()) > pLoopUnit->SMCargoVolume());
					}
					if (bSpaceAvailable)
					{
						if( !(pLoopUnit->atPlot(pTransportUnit->plot())) )
						{
							// Putting a land unit on water automatically loads it
							//GC.getGameINLINE().logOOSSpecial(12, pLoopUnit->getID(), pTransportUnit->getX_INLINE(), pTransportUnit->getY_INLINE());
							pLoopUnit->setXY(pTransportUnit->getX_INLINE(),pTransportUnit->getY_INLINE());
						}

						if( pLoopUnit->getTransportUnit() != pTransportUnit ) 
						{
							pLoopUnit->setTransportUnit(pTransportUnit);
						}

						bLoadedOne = true;
					}
					else if (getHeadUnit()->canSplit())
					{
						getHeadUnit()->doSplit();
						setRemoteTransportUnit(pTransportUnit);
					}
				}
			}
		}
		while (bLoadedOne);
	}
	// otherwise we are unloading
	else
	{
		// loop over all the units, unloading them
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			
			if (pLoopUnit != NULL)
			{
				// unload unit
				pLoopUnit->setTransportUnit(NULL);
			}
		}
	}
}

bool CvSelectionGroup::isAmphibPlot(const CvPlot* pPlot) const
{
	bool bFriendly = true;
	CvUnit* pUnit = getHeadUnit();
	if (NULL != pUnit)
	{
		bFriendly = pPlot->isFriendlyCity(*pUnit, true);
	}

	//return ((getDomainType() == DOMAIN_SEA) && pPlot->isCoastalLand() && !bFriendly && !canMoveAllTerrain());

	if (getDomainType() == DOMAIN_SEA)
	{
		if (pPlot->isCity() && !bFriendly && (pPlot->isCoastalLand() || pPlot->isWater() || canMoveAllTerrain()))
		{
			return true;
		}
		return (pPlot->isCoastalLand() && !bFriendly && !canMoveAllTerrain());
	}
	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

// Returns true if attempted an amphib landing...
bool CvSelectionGroup::groupAmphibMove(CvPlot* pPlot, int iFlags)
{
	CLLNode<IDInfo>* pUnitNode1;
	CvUnit* pLoopUnit1;
	bool bLanding = false;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (groupDeclareWar(pPlot))
	{
		return true;
	}

	if (isAmphibPlot(pPlot))
	{
// BUG - Safe Move - start
		// don't perform amphibious landing on plot that was unrevealed when goto order was issued
		if (isHuman())
		{
			if (!isLastPathPlotRevealed())
			{
				return false;
			}
		}
// BUG - Safe Move - end

		if (stepDistance(getX(), getY(), pPlot->getX_INLINE(), pPlot->getY_INLINE()) == 1)
		{
			pUnitNode1 = headUnitNode();

			// BBAI TODO: Bombard with warships if invading

			while (pUnitNode1 != NULL)
			{
				pLoopUnit1 = ::getUnit(pUnitNode1->m_data);
				pUnitNode1 = nextUnitNode(pUnitNode1);

				if ((pLoopUnit1->hasCargo()) && (pLoopUnit1->domainCargo() == DOMAIN_LAND))
				{
					std::vector<CvUnit*> aCargoUnits;
					pLoopUnit1->getCargoUnits(aCargoUnits);
					std::vector<CvSelectionGroup*> aCargoGroups;
					for (uint i = 0; i < aCargoUnits.size(); ++i)
					{
						CvSelectionGroup* pGroup = aCargoUnits[i]->getGroup();
						if (std::find(aCargoGroups.begin(), aCargoGroups.end(), pGroup) == aCargoGroups.end())
						{
							aCargoGroups.push_back(aCargoUnits[i]->getGroup());
						}
					}

					for (uint i = 0; i < aCargoGroups.size(); ++i)
					{
						CvSelectionGroup* pGroup = aCargoGroups[i];
						if (pGroup->canAllMove())
						{
							FAssert(!pGroup->at(pPlot->getX_INLINE(), pPlot->getY_INLINE()));
							bLanding = pGroup->pushMissionInternal(MISSION_MOVE_TO, pPlot->getX_INLINE(), pPlot->getY_INLINE(), (MOVE_IGNORE_DANGER | iFlags));
						}
					}
				}
			}
		}
	}

	return bLanding;
}


bool CvSelectionGroup::readyToSelect(bool bAny)
{
	return (readyToMove(bAny) && !isAutomated());
}


bool CvSelectionGroup::readyToMove(bool bAny, bool bValidate)
{
	return (((bAny) ? canAnyMove(bValidate) : canAllMove()) && (headMissionQueueNode() == NULL) && (getActivityType() == ACTIVITY_AWAKE) && !isBusy() && !isCargoBusy());
}


bool CvSelectionGroup::readyToAuto()
{
	return (canAllMove() && (headMissionQueueNode() != NULL));
}


int CvSelectionGroup::getID() const
{
	return m_iID;
}


void CvSelectionGroup::setID(int iID)
{
	m_iID = iID;
}


PlayerTypes CvSelectionGroup::getOwner() const
{
	return getOwnerINLINE();
}


TeamTypes CvSelectionGroup::getTeam() const
{
	if (getOwnerINLINE() != NO_PLAYER)
	{
		return GET_PLAYER(getOwnerINLINE()).getTeam();
	}

	return NO_TEAM;
}


int CvSelectionGroup::getMissionTimer() const
{
	return m_iMissionTimer;
}


void CvSelectionGroup::setMissionTimer(int iNewValue)
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	m_iMissionTimer = iNewValue;
	FAssert(getMissionTimer() >= 0);
}


void CvSelectionGroup::changeMissionTimer(int iChange)
{
	setMissionTimer(getMissionTimer() + iChange);
}


void CvSelectionGroup::updateMissionTimer(int iSteps)
{
	PROFILE_FUNC();

	CvUnit* pTargetUnit;
	CvPlot* pTargetPlot;
	int iTime;

	if (!isHuman() && !showMoves())
	{
		iTime = 0;
	}
	else if (headMissionQueueNode() != NULL)
	{
		iTime = GC.getMissionInfo((MissionTypes)(headMissionQueueNode()->m_data.eMissionType)).getTime();

		if ((headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO) ||
// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
				(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_SENTRY) ||
#endif
// BUG - Sentry Actions - end
				(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO) ||
				(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT))
		{
			if (headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT)
			{
				pTargetUnit = GET_PLAYER((PlayerTypes)headMissionQueueNode()->m_data.iData1).getUnit(headMissionQueueNode()->m_data.iData2);
				if (pTargetUnit != NULL)
				{
					pTargetPlot = pTargetUnit->plot();
				}
				else
				{
					pTargetPlot = NULL;
				}
			}
			else
			{
				pTargetPlot = GC.getMapINLINE().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2);
			}

			if (atPlot(pTargetPlot))
			{
				iTime += iSteps;
			}
			else
			{
				iTime = std::min(iTime, 2);
			}
		}

		if (isHuman() && (isAutomated() || (GET_PLAYER((GC.getGameINLINE().isNetworkMultiPlayer()) ? getOwnerINLINE() : GC.getGameINLINE().getActivePlayer()).isOption(PLAYEROPTION_QUICK_MOVES))))
		{
			iTime = std::min(iTime, 1);
		}
	}
	else
	{
		iTime = 0;
	}

	setMissionTimer(iTime);
}


bool CvSelectionGroup::isForceUpdate()
{
	return m_bForceUpdate;
}


void CvSelectionGroup::setForceUpdate(bool bNewValue)
{
	m_bForceUpdate = bNewValue;
}


ActivityTypes CvSelectionGroup::getActivityType() const
{
	return m_eActivityType;
}


void CvSelectionGroup::setActivityType(ActivityTypes eNewValue, MissionTypes eSleepType)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	MissionTypes eMission = NO_MISSION;

	FAssert(getOwnerINLINE() != NO_PLAYER);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/28/10                                jdog5000      */
/*                                                                                              */
/* Unit AI                                                                                      */
/************************************************************************************************/
	// For debugging activities, but can cause crashes very occasionally times
	//FAssert(isHuman() || getHeadUnit()->isCargo() || eNewValue != ACTIVITY_SLEEP);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	ActivityTypes eOldActivity = getActivityType();

	if (eOldActivity != eNewValue)
	{
		pPlot = plot();

		if (eOldActivity == ACTIVITY_INTERCEPT)
		{
			airCircle(false);
		}

		setBlockading(false);

		//Clear Buildups
		if ((eOldActivity == ACTIVITY_SLEEP || eOldActivity == ACTIVITY_HEAL) && eNewValue == ACTIVITY_AWAKE)
		{
			pUnitNode = headUnitNode();
			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				if (pLoopUnit->isBuildUp())
				{
					pLoopUnit->setFortifyTurns(0);
				}
			}
		}

		m_eActivityType = eNewValue;

		if (getActivityType() == ACTIVITY_INTERCEPT)
		{
			airCircle(true);
		}

		if (getActivityType() != ACTIVITY_MISSION)
		{
			pUnitNode = headUnitNode();

			if (getActivityType() != ACTIVITY_INTERCEPT)
			{
				//don't idle intercept animation
				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);

					pLoopUnit->NotifyEntity(MISSION_IDLE);

					//determine proper Sleep type
					if (!isHuman()||((getActivityType() == ACTIVITY_SLEEP || getActivityType() == ACTIVITY_HEAL) && eSleepType != NO_MISSION))
					{
						eMission = MISSION_SLEEP;
						if (eSleepType == MISSION_BUILDUP || eSleepType == MISSION_AUTO_BUILDUP|| eSleepType == MISSION_HEAL_BUILDUP|| eSleepType == NO_MISSION)
						{
							if (pLoopUnit->isBuildUpable())
							{
								if (pLoopUnit->getBuildUpType() == NO_PROMOTIONLINE)
								{
									pLoopUnit->setBuildUpType(NO_PROMOTIONLINE, false, eSleepType);
								}
								if (isHuman() && eSleepType != MISSION_AUTO_BUILDUP && eSleepType != MISSION_HEAL_BUILDUP)
								{
									eMission = MISSION_BUILDUP;
								}
								else
								{
									//Then find out if the set check came up with a good buildup
									if (pLoopUnit->getBuildUpType() != NO_PROMOTIONLINE)
									{
										eMission = MISSION_BUILDUP;
									}//and if not...
									//else if (pLoopUnit->isEstablishable())
									//{
									//	eMission = MISSION_ESTABLISH;
									//}
									else if (pLoopUnit->isFortifyable())
									{
										eMission = MISSION_FORTIFY;
									}
									//else if (pLoopUnit->isEscapable())
									//{
									//	eMission = MISSION_ESCAPE;
									//}
									else
									{
										eMission = MISSION_SLEEP;
									}
								}
							}
							//else if (pLoopUnit->isEstablishable())
							//{
							//	eMission = MISSION_ESTABLISH;
							//}
							else if (pLoopUnit->isFortifyable())
							{
								eMission = MISSION_FORTIFY;
							}
							//else if (pLoopUnit->isEscapable())
							//{
							//	eMission = MISSION_ESCAPE;
							//}
							else
							{
								eMission = MISSION_SLEEP;
							}
						}
						//else if (eSleepType == MISSION_ESTABLISH)
						//{
						//	if (pLoopUnit->isEstablishable())
						//	{
						//		eMission = MISSION_ESTABLISH;
						//	}
						//	else if (pLoopUnit->isFortifyable())
						//	{
						//		eMission = MISSION_FORTIFY;
						//	}
						//	else if (pLoopUnit->isEscapable())
						//	{
						//		eMission = MISSION_ESCAPE;
						//	}
						//	else
						//	{
						//		eMission = MISSION_SLEEP;
						//	}
						//}
						//else if (eSleepType == MISSION_ESCAPE)
						//{
						//	if (pLoopUnit->isEscapable())
						//	{
						//		eMission = MISSION_ESCAPE;
						//	}
						//	else if (pLoopUnit->isEstablishable())
						//	{
						//		eMission = MISSION_ESTABLISH;
						//	}
						//	else if (pLoopUnit->isFortifyable())
						//	{
						//		eMission = MISSION_FORTIFY;
						//	}
						//	else
						//	{
						//		eMission = MISSION_SLEEP;
						//	}
						//}
						else if (eSleepType == MISSION_FORTIFY)
						{
							if (pLoopUnit->isFortifyable())
							{
								eMission = MISSION_FORTIFY;
							}
							//else if (pLoopUnit->isEstablishable())
							//{
							//	eMission = MISSION_ESTABLISH;
							//}
							//else if (pLoopUnit->isEscapable())
							//{
							//	eMission = MISSION_ESCAPE;
							//}
							else
							{
								eMission = MISSION_SLEEP;
							}
						}
						else
						{
							eMission = MISSION_SLEEP;
						}
						pLoopUnit->setSleepType(eMission);
					}
					if (pLoopUnit->getSleepType() != MISSION_BUILDUP && pLoopUnit->getBuildUpType() != NO_PROMOTIONLINE)
					{
						PromotionLineTypes ePromotionLine = pLoopUnit->getBuildUpType();
						for (int iI = 0; iI < GC.getPromotionLineInfo(ePromotionLine).getNumPromotions(); iI++)
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getPromotionLineInfo(ePromotionLine).getPromotion(iI);
							if (pLoopUnit->isHasPromotion(ePromotion))
							{
								pLoopUnit->setHasPromotion(ePromotion, false, true, false, false);
							}
						}
						pLoopUnit->setBuildUpType(NO_PROMOTIONLINE, true);
						pLoopUnit->setFortifyTurns(0);
					}
				}
			}

			if (getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				if (pPlot != NULL)
				{
					pPlot->setFlagDirty(true);
				}
			}
		}

		if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


AutomateTypes CvSelectionGroup::getAutomateType() const														
{
	return m_eAutomateType;
}


bool CvSelectionGroup::isAutomated()
{
	return (getAutomateType() != NO_AUTOMATE);
}


void CvSelectionGroup::setAutomateType(AutomateTypes eNewValue)
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getAutomateType() != eNewValue)
	{
		m_eAutomateType = eNewValue;

		clearMissionQueue();
		setActivityType(ACTIVITY_AWAKE);

		// if canceling automation, cancel on cargo as well
		if (eNewValue == NO_AUTOMATE)
		{
			CvPlot* pPlot = plot();
			if (pPlot != NULL)
			{
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pCargoUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);
					
					CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
					if (pTransportUnit != NULL && pTransportUnit->getGroup() == this)
					{
						pCargoUnit->getGroup()->setAutomateType(NO_AUTOMATE);
						pCargoUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
					}
				}
			}
		}
	}
}

#ifdef USE_OLD_PATH_GENERATOR
FAStarNode* CvSelectionGroup::getPathLastNode() const
{
	return gDLL->getFAStarIFace()->GetLastNode(&GC.getPathFinder());
}
#endif


CvPlot* CvSelectionGroup::getPathFirstPlot() const
{
#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* pNode;

	pNode = getPathLastNode();

	if (pNode->m_pParent == NULL)
	{
		return GC.getMapINLINE().plotSorenINLINE(pNode->m_iX, pNode->m_iY);
	}

	while (pNode != NULL)
	{
		if (pNode->m_pParent->m_pParent == NULL)
		{
			return GC.getMapINLINE().plotSorenINLINE(pNode->m_iX, pNode->m_iY);
		}

		pNode = pNode->m_pParent;
	}

	FAssert(false);

	return NULL;
#else
	CvPath::const_iterator itr = getPath().begin();

	//	CvPath stores the node the unit started on first, but the 'first plot' required is the
	//	first one moved to
	if ( itr != getPath().end() )
	{
		return (++itr).plot();
	}
	else
	{
		return NULL;
	}
#endif
}

CvPath&	CvSelectionGroup::getPath() const
{
	return getPathGenerator()->getLastPath();
}

CvPlot* CvSelectionGroup::getPathEndTurnPlot() const
{
#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* pNode;

	pNode = getPathLastNode();

	if (NULL != pNode)
	{
		if ((pNode->m_pParent == NULL) || (pNode->m_iData2 == 1))
		{
			return GC.getMapINLINE().plotSorenINLINE(pNode->m_iX, pNode->m_iY);
		}

		while (pNode->m_pParent != NULL)
		{
			if (pNode->m_pParent->m_iData2 == 1)
			{
				return GC.getMapINLINE().plotSorenINLINE(pNode->m_pParent->m_iX, pNode->m_pParent->m_iY);
			}

			pNode = pNode->m_pParent;
		}
	}

	FAssert(false);

	return NULL;
#else
	CvPath::const_iterator itr = getPath().begin();

	//	CvPath stores the node the unit started on first, but the 'first plot' required is the
	//	first one moved to
	if ( itr != getPath().end() )
	{
		int iStart = itr.turn();
		CvPlot*	pPlot = itr.plot();

		while ( itr.turn() == iStart && itr != getPath().end() )
		{
			pPlot = itr.plot();

			++itr;
		}

		if(pPlot == getPath().begin().plot() && pPlot != getPath().lastPlot())
		{
			OutputDebugString(CvString::format("Bad path movement calc on path from (%d,%d) to (%d,%d)\n",pPlot->getX_INLINE(), pPlot->getY_INLINE(),getPath().lastPlot()->getX_INLINE(), getPath().lastPlot()->getY_INLINE()).c_str());
		}

		FAssert(pPlot != getPath().begin().plot() || pPlot == getPath().lastPlot());
		return pPlot;
	}
	else
	{
		return NULL;
	}
#endif
}

//TB OOS Debug: Actually... adding the bAsync here has only been implemented so as to set myself up for some conditional tracking here.
bool CvSelectionGroup::generatePath( const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns, int iMaxPathLen, int iOptimizationLimit, bool bAsync) const
{
	bool bSuccess;

	PROFILE("CvSelectionGroup::generatePath()")

#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* pNode;
/************************************************************************************************/
/* Afforess	                  Start		 04/09/10                                               */
/*                                                                                              */
/* CTD Fix                                                                                      */
/************************************************************************************************/
	if (pFromPlot == NULL || pToPlot == NULL) return false;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	gDLL->getFAStarIFace()->SetData(&GC.getPathFinder(), this);

	//	Note - we NEVER allow the pathing engine to cache  any more, since that reuses end-of-turn costs/validity
	//	in non-end-of-turn considerations of inter-tile moves.  This appears to always have been a bug, but with 
	//	the new AI pathing it became more significant.  The callbacks we use in path generation now do much more
	//	extensive caching of their own, which works out about performance neutral
	switch(getHeadUnit()->getDomainType())
	{
		case DOMAIN_LAND:
		{
			PROFILE("generatePath.Land");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), false, iFlags, false);
			break;
		}
		case DOMAIN_AIR:
		{
			PROFILE("generatePath.Air");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), false, iFlags, false);
			break;
		}
		case DOMAIN_SEA:
		{
			PROFILE("generatePath.Sea");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), false, iFlags, bReuse);
			break;
		}
		default:
		{
			PROFILE("generatePath.Other");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), false, iFlags, bReuse);
			break;
		}
	}

	if (piPathTurns != NULL)
	{
		*piPathTurns = MAX_INT;

		if (bSuccess)
		{
			pNode = getPathLastNode();

			if (pNode != NULL)
			{
				*piPathTurns = pNode->m_iData2;
			}
		}
	}
#else
	bSuccess = getPathGenerator()->generatePath(pFromPlot, pToPlot, (CvSelectionGroup*)this, iFlags, iMaxPathLen, iOptimizationLimit);

	if (piPathTurns != NULL)
	{
		*piPathTurns = MAX_INT;

		if (bSuccess)
		{
			*piPathTurns = m_generator->getLastPath().length();
		}
	}

#endif

	return bSuccess;
}

CvPathGenerator*	CvSelectionGroup::getPathGenerator()
{
	if ( m_generator == NULL )
	{
		m_generator = new	CvPathGenerator(&GC.getMapINLINE());

		m_generator->Initialize(NewPathHeuristicFunc, NewPathCostFunc, NewPathValidFunc, NewPathDestValid, NewPathTurnEndValidityCheckRequired);
	}

	return m_generator;
}



void CvSelectionGroup::setGroupToCacheFor(CvSelectionGroup* group)
{
	MEMORY_TRACK_EXEMPT();
	PROFILE_FUNC();

	m_pCachedMovementGroup = group;

	if ( m_pCachedNonEndTurnEdgeCosts == NULL )
	{
		m_pCachedNonEndTurnEdgeCosts = new std::map<int,edgeCosts>();
	}
	if ( m_pCachedEndTurnEdgeCosts == NULL )
	{
		m_pCachedEndTurnEdgeCosts = new std::map<int,edgeCosts>();
	}

	m_pCachedNonEndTurnEdgeCosts->clear();
	m_pCachedEndTurnEdgeCosts->clear();
}

bool CvSelectionGroup::HaveCachedPathEdgeCosts(CvPlot* pFromPlot, CvPlot* pToPlot, bool bIsEndTurnElement, int& iResult, int& iBestMoveCost, int& iWorstMoveCost, int& iToPlotNodeCost)
{
	//	Could use Zobrist hashes of the plots, but actually since we're only combining two sets of coordinates we can
	//	fit it all in an int for any reasonable map
	int cacheKey = GC.getMapINLINE().plotNumINLINE(pFromPlot->getX_INLINE(),pFromPlot->getY_INLINE()) + (GC.getMapINLINE().plotNumINLINE(pToPlot->getX_INLINE(),pToPlot->getY_INLINE()) << 16);

	if ( this != m_pCachedMovementGroup )
	{
		return false;
	}
	bool result;

	if ( bIsEndTurnElement )
	{
		std::map<int,edgeCosts>::const_iterator itr = m_pCachedEndTurnEdgeCosts->find(cacheKey);

		if ( itr == m_pCachedEndTurnEdgeCosts->end() )
		{
			result = false;
		}
		else
		{
			iResult = (itr->second).iCost;
			iBestMoveCost = (itr->second).iBestMoveCost;
			iWorstMoveCost = (itr->second).iWorstMoveCost;
			iToPlotNodeCost = (itr->second).iToPlotNodeCost;
#ifdef _DEBUG
			FAssert((itr->second).pFromPlot == pFromPlot);
			FAssert((itr->second).pToPlot == pToPlot);
#endif
			result = true;
		}
	}
	else
	{
		std::map<int,edgeCosts>::const_iterator itr = m_pCachedNonEndTurnEdgeCosts->find(cacheKey);

		if ( itr == m_pCachedNonEndTurnEdgeCosts->end() )
		{
			result = false;
		}
		else
		{
			iResult = (itr->second).iCost;
			iBestMoveCost = (itr->second).iBestMoveCost;
			iWorstMoveCost = (itr->second).iWorstMoveCost;
			iToPlotNodeCost = (itr->second).iToPlotNodeCost;
#ifdef _DEBUG
			FAssert((itr->second).pFromPlot == pFromPlot);
			FAssert((itr->second).pToPlot == pToPlot);
#endif
			result = true;
		}
	}

	return result;
}

void CvSelectionGroup::CachePathEdgeCosts(CvPlot* pFromPlot, CvPlot* pToPlot, bool bIsEndTurnElement, int iCost, int iBestMoveCost, int iWorstMoveCost, int iToPlotNodeCost)
{
	MEMORY_TRACK_EXEMPT();

	if ( this == m_pCachedMovementGroup )
	{
		//	Could use Zobrist hashes of the plots, but actually since we're only combining two sets of coordinates we can
		//	fit it all in an int for any reasonable map
		FAssert(GC.getMapINLINE().getGridHeightINLINE()*GC.getMapINLINE().getGridHeightINLINE()*GC.getMapINLINE().getGridWidthINLINE()*(GC.getMapINLINE().getGridWidthINLINE()/2) < MAX_INT);
		int cacheKey = GC.getMapINLINE().plotNumINLINE(pFromPlot->getX_INLINE(),pFromPlot->getY_INLINE()) + (GC.getMapINLINE().plotNumINLINE(pToPlot->getX_INLINE(),pToPlot->getY_INLINE()) << 16);

		edgeCosts costs;

		costs.iCost = iCost;
		costs.iBestMoveCost = iBestMoveCost;
		costs.iWorstMoveCost = iWorstMoveCost;
		costs.iToPlotNodeCost = iToPlotNodeCost;
#ifdef _DEBUG
		costs.pFromPlot = pFromPlot;
		costs.pToPlot = pToPlot;
#endif

		if ( bIsEndTurnElement )
		{
			(*m_pCachedEndTurnEdgeCosts)[cacheKey] = costs;
		}
		else
		{
			(*m_pCachedNonEndTurnEdgeCosts)[cacheKey] = costs;
		}

	}
}

void CvSelectionGroup::resetPath()
{
	gDLL->getFAStarIFace()->ForceReset(&GC.getPathFinder());
}

bool CvSelectionGroup::canPathDirectlyTo(CvPlot* pFromPlot, CvPlot* pToPlot)
{
	return canPathDirectlyToInternal(pFromPlot, pToPlot, -1);
}

bool CvSelectionGroup::canPathDirectlyToInternal(CvPlot* pFromPlot, CvPlot* pToPlot, int movesRemaining)
{
	PROFILE_FUNC();

	if ( movesRemaining == 0 )
	{
		return false;
	}

	//	Avoid path searching - just test paths that monotonically move towards the destination
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), ((DirectionTypes)iI));
		if( pAdjacentPlot != NULL )
		{
			if ( stepDistance(pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), pToPlot->getX_INLINE(), pToPlot->getY_INLINE()) <
				 stepDistance(pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), pToPlot->getX_INLINE(), pToPlot->getY_INLINE()) )
			{
				if ( canMoveInto(pAdjacentPlot, (pAdjacentPlot == pToPlot)) )
				{
					if ( pAdjacentPlot == pToPlot )
					{
						return true;
					}
					else
					{
						return canPathDirectlyToInternal(pAdjacentPlot, pToPlot, movesRemainingAfterMovingTo(movesRemaining, pFromPlot, pAdjacentPlot));
					}
				}
			}
		}
	}
	return false;
}

int CvSelectionGroup::movesRemainingAfterMovingTo(int iStartMoves, CvPlot* pFromPlot, CvPlot* pToPlot) const
{
	int iResult = MAX_INT;

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		int iUnitMoves = (iStartMoves == -1 ? pLoopUnit->maxMoves() : iStartMoves);
		iUnitMoves -= pToPlot->movementCost(pLoopUnit, pFromPlot);
		iUnitMoves = std::max(iUnitMoves, 0);
		
		iResult = std::min(iResult, iUnitMoves);
	}

	return iResult;
}

int CvSelectionGroup::movesLeft() const
{
	int iResult = MAX_INT;

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		int iUnitMoves = pLoopUnit->movesLeft();
		
		iResult = std::min(iResult, iUnitMoves);
	}

	return iResult;
}


void CvSelectionGroup::clearUnits()
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pUnitNode = deleteUnitNode(pUnitNode);
	}
}

bool CvSelectionGroup::hasUnitOfAI(UnitAITypes eUnitAI) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if ( pLoopUnit->AI_getUnitAIType() == eUnitAI )
		{
			return true;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

int	CvSelectionGroup::getWorstDamagePercent(UnitCombatTypes eIgnoreUnitCombat) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iWorstDamage = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (eIgnoreUnitCombat == NO_UNITCOMBAT || !pLoopUnit->isHasUnitCombat(eIgnoreUnitCombat))
		{
			if ( pLoopUnit->getDamage() > iWorstDamage )
			{
				iWorstDamage = pLoopUnit->getDamagePercent();
			}
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	return iWorstDamage;
}

// Returns true if the unit is added...
bool CvSelectionGroup::addUnit(CvUnit* pUnit, bool bMinimalChange)
{
	//PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pOldHeadUnit = getHeadUnit();
	CvPlot* pPlot = pUnit->plot();
	bool bAdded;
 
	if (pPlot!=NULL && ! (pUnit->canJoinGroup(pUnit->plot(), this)))
	{
		return false;
	}

#ifdef _DEBUG
	if (pPlot!=NULL)
	{
		validateLocations();
	}
#endif

	bAdded = false;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (!bAdded && ((pUnit->AI_groupFirstVal() > pLoopUnit->AI_groupFirstVal()) ||
			  ((pUnit->AI_groupFirstVal() == pLoopUnit->AI_groupFirstVal()) &&
				 (pUnit->AI_groupSecondVal() > pLoopUnit->AI_groupSecondVal()))))
		{
			m_units.insertBefore(pUnit->getIDInfo(), pUnitNode);
			bAdded = true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (!bAdded)
	{
		m_units.insertAtEnd(pUnit->getIDInfo());
	}
	int iVolume = pUnit->getCargoVolume();
	AI_noteSizeChange(1, iVolume);

	if(!bMinimalChange)
	{
		if (getOwnerINLINE() == NO_PLAYER)
		{
			if (getNumUnits() > 0)
			{
				pUnitNode = headUnitNode();
				while (pUnitNode != NULL)
				{
					//if (pUnitNode != headUnitNode())
					//{
						::getUnit(pUnitNode->m_data)->NotifyEntity(MISSION_MULTI_SELECT);
					//}
					pUnitNode = nextUnitNode(pUnitNode);
				}
			}
		}
	}

	if ( pOldHeadUnit != getHeadUnit() )
	{
		if ( GC.getDefineINT("ENABLE_DYNAMIC_UNIT_ENTITIES") )
		{
			if ( pOldHeadUnit != NULL )
			{
				pOldHeadUnit->reloadEntity();
			}

			getHeadUnit()->reloadEntity();
		}
	}

#ifdef _DEBUG
	if (pPlot != NULL)
	{
		validateLocations();
	}
#endif
	return true;
}

bool CvSelectionGroup::containsUnit(const CvUnit* pUnit) const
{
	//CvUnit* pOldHeadUnit = getHeadUnit();

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			return true;
		}
		else
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	return false;
}

void CvSelectionGroup::removeUnit(CvUnit* pUnit)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pOldHeadUnit = getHeadUnit();

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			deleteUnitNode(pUnitNode);
			break;
		}
		else
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	if ( pOldHeadUnit != getHeadUnit() )
	{
		if ( GC.getENABLE_DYNAMIC_UNIT_ENTITIES() )
		{
			if ( pOldHeadUnit != NULL )
			{
				pOldHeadUnit->reloadEntity();
			}

			if ( getHeadUnit() != NULL )
			{
				getHeadUnit()->reloadEntity();
			}
		}
	}
}


CLLNode<IDInfo>* CvSelectionGroup::deleteUnitNode(CLLNode<IDInfo>* pNode)
{
	CLLNode<IDInfo>* pNextUnitNode;

	if (getOwnerINLINE() != NO_PLAYER)
	{
/************************************************************************************************/
/* Afforess	                  Start		 07/12/10                                               */
/*                                                                                              */
/*  Allow Automated Units to Stack Attack                                                       */
/************************************************************************************************/
/*
		setAutomateType(NO_AUTOMATE);
*/
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		clearMissionQueue();

		switch (getActivityType())
		{
		case ACTIVITY_SLEEP:
		case ACTIVITY_INTERCEPT:
		case ACTIVITY_PATROL:
		case ACTIVITY_PLUNDER:
			break;
		default:
			setActivityType(ACTIVITY_AWAKE);
			break;
		}
	}

	CvUnit* pLoopUnit = ::getUnit(pNode->m_data);
	int iVolume = pLoopUnit->getCargoVolume();

	pNextUnitNode = m_units.deleteNode(pNode);

	AI_noteSizeChange(-1, iVolume);

	return pNextUnitNode;
}


CLLNode<IDInfo>* CvSelectionGroup::nextUnitNode(CLLNode<IDInfo>* pNode) const
{
	return m_units.next(pNode);
}


int CvSelectionGroup::getNumUnits() const
{
	return m_units.getLength();
}

int CvSelectionGroup::getNumUnitCargoVolumeTotal() const
{
	int iTotal = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		iTotal += pLoopUnit->SMCargoVolume();
	}

	return iTotal;
}

int CvSelectionGroup::getLeastCargoVolume() const
{
	int iLowest = MAX_INT;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		if (pLoopUnit->SMCargoVolume() < iLowest)
		{
			iLowest = pLoopUnit->SMCargoVolume();
		}
	}
	return iLowest;
}


bool CvSelectionGroup::meetsUnitSelectionCriteria(CvUnitSelectionCriteria* criteria) const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ( pLoopUnit->meetsUnitSelectionCriteria(criteria) )
		{
			return true;
		}
	}

	return false;
}

void CvSelectionGroup::mergeIntoGroup(CvSelectionGroup* pSelectionGroup)
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	// merge groups, but make sure we do not change the head unit AI
	// this means that if a new unit is going to become the head, change its AI to match, if possible
	// AI_setUnitAIType removes the unit from the current group (at least currently), so we have to be careful in the loop here
	// so, loop until we have not changed unit AIs
	bool bChangedUnitAI;
	do
	{
		bChangedUnitAI = false;

		// loop over all the units, moving them to the new group, 
		// stopping if we had to change a unit AI, because doing so removes that unit from our group, so we have to start over
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL && !bChangedUnitAI)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			//pUnitNode = nextUnitNode(pUnitNode);
			
			if (pLoopUnit != NULL)
			{
				UnitAITypes eUnitAI = pLoopUnit->AI_getUnitAIType();

				// if the unitAIs are different, and the loop unit has a higher val, then the group unitAI would change
				// change this UnitAI to the old group UnitAI if possible
				CvUnit* pNewHeadUnit = pSelectionGroup->getHeadUnit();
				UnitAITypes eNewHeadUnitAI = pSelectionGroup->getHeadUnitAI();
				if (pNewHeadUnit!= NULL && eUnitAI != eNewHeadUnitAI && pLoopUnit->AI_groupFirstVal() > pNewHeadUnit->AI_groupFirstVal())
				{
					// non-zero AI_unitValue means that this UnitAI is valid for this unit (that is the check used everywhere)
					if (kPlayer.AI_unitValue(pLoopUnit->getUnitType(), eNewHeadUnitAI, NULL) > 0)
					{
						FAssert(pLoopUnit->AI_getUnitAIType() != UNITAI_HUNTER);
						// this will remove pLoopUnit from the current group
						pLoopUnit->AI_setUnitAIType(eNewHeadUnitAI);

						bChangedUnitAI = true;
					}
				}
				if (pLoopUnit->canJoinGroup(pLoopUnit->plot(), pSelectionGroup))
				{
					pLoopUnit->joinGroup(pSelectionGroup);
					pUnitNode = headUnitNode();
				}
				else
				{
					pUnitNode = nextUnitNode(pUnitNode);
				}
			}
		}
	}
	while (bChangedUnitAI);
}

// split this group into two new groups, one of iSplitSize, the other the remaining units
// split up each unit AI type as evenly as possible
CvSelectionGroup* CvSelectionGroup::splitGroup(int iSplitSize, CvUnit* pNewHeadUnit, CvSelectionGroup** ppOtherGroup)
{
	FAssertMsg(iSplitSize > 0, "non-positive splitGroup size");
	if (!(iSplitSize > 0))
	{
		return NULL;
	}

	// are we already small enough?
	if (getNumUnits() <= iSplitSize)
	{
		return this;
	}
	
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pOldHeadUnit = ::getUnit(pUnitNode->m_data);
	FAssertMsg(pOldHeadUnit != NULL, "non-zero group without head unit");
	if (pOldHeadUnit == NULL)
	{
		return NULL;
	}

	UnitAITypes eOldHeadAI = pOldHeadUnit->AI_getUnitAIType();

	// if pNewHeadUnit NULL, then we will use our current head to head the new split group of target size
	if (pNewHeadUnit == NULL)
	{
		pNewHeadUnit = pOldHeadUnit;
	}

	// the AI of the new head (the remainder will get the AI of the old head)
	// UnitAITypes eNewHeadAI = pNewHeadUnit->AI_getUnitAIType();

	// pRemainderHeadUnit is the head unit of the group that contains the remainder of units 
	CvUnit* pRemainderHeadUnit = NULL;

	// if the new head is not the old head, then make the old head the remainder head
	bool bSplitingHead = (pOldHeadUnit == pNewHeadUnit);
	if (!bSplitingHead)
	{
		pRemainderHeadUnit = pOldHeadUnit;
	}
	
	// try to find remainder head with same AI as head, if we cannot find one, we will split the rest of the group up
	if (pRemainderHeadUnit == NULL)
	{
		// loop over all the units
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL && pRemainderHeadUnit == NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			
			if (pLoopUnit != NULL && pLoopUnit != pNewHeadUnit)
			{
				UnitAITypes eLoopUnitAI = pLoopUnit->AI_getUnitAIType();
				if (eLoopUnitAI == eOldHeadAI)
				{
					pRemainderHeadUnit = pLoopUnit;
				}
			}
		}
	}
	
	CvSelectionGroup* pSplitGroup = NULL;
	CvSelectionGroup* pRemainderGroup = NULL;
	
	// make the new group for the new head
	pNewHeadUnit->joinGroup(NULL);
	pSplitGroup = pNewHeadUnit->getGroup();
	FAssertMsg(pSplitGroup != NULL, "join resulted in NULL group");

	// make a new group for the remainder, if non-null
	if (pRemainderHeadUnit != NULL)
	{
		pRemainderHeadUnit->joinGroup(NULL);
		pRemainderGroup = pRemainderHeadUnit->getGroup();
		FAssertMsg(pRemainderGroup != NULL, "join resulted in NULL group");
		FAssertMsg(pRemainderGroup != this, "join resulted in same group");
	}

	// loop until this group is empty, trying to move different AI types each time
	
	
	//Exhibit of why i HATE iustus code sometimes
	//unsigned int unitAIBitField = 0;
	//setBit(unitAIBitField, eNewHeadAI);
	
	bool abUnitAIField[NUM_UNITAI_TYPES];
	for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		abUnitAIField[iI] = false;
	}
	
	while (getNumUnits())
	{
		UnitAITypes eTargetUnitAI = NO_UNITAI;
	
		// loop over all the units, find the next different UnitAI and move one of each
		bool bDestinationSplit = (pSplitGroup->getNumUnits() < iSplitSize);
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			
			if (pLoopUnit != NULL)
			{
				UnitAITypes eLoopUnitAI = pLoopUnit->AI_getUnitAIType();
				
				// if we have not found a new type to move, is this a new unitai?
				// note, if there eventually are unitAIs above 31, we will just always move those, which is fine
				if (eTargetUnitAI == NO_UNITAI && !abUnitAIField[eLoopUnitAI])
				{
					eTargetUnitAI =  eLoopUnitAI;
					abUnitAIField[eLoopUnitAI] = true;
				}
				
				// is this the right UnitAI?
				if (eLoopUnitAI == eTargetUnitAI)
				{
					// move this unit to the appropriate group 
					if (bDestinationSplit)
					{
						pLoopUnit->joinGroup(pSplitGroup);
					}
					else
					{
						pLoopUnit->joinGroup(pRemainderGroup);
						// (if pRemainderGroup NULL, it gets its own group)
						pRemainderGroup = pLoopUnit->getGroup();

						FAssertMsg(pRemainderGroup != NULL, "join resulted in NULL group");
						FAssertMsg(pRemainderGroup != this, "join resulted in same group");
					}
					
					// if we moved to remainder, try for next unit AI
					if (!bDestinationSplit)
					{
						eTargetUnitAI = NO_UNITAI;

						bDestinationSplit = (pSplitGroup->getNumUnits() < iSplitSize);
					}
					else
					{
						// next unit goes to the remainder group
						bDestinationSplit = false;
					}
				}
			}

		}
		
		// clear bitfield, all types are valid again
		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			abUnitAIField[iI] = false;
		}
	}

	FAssertMsg(pSplitGroup->getNumUnits() <= iSplitSize, "somehow our split group is too large");
	
	if (ppOtherGroup != NULL)
	{
		*ppOtherGroup = pRemainderGroup;
	}

	return pSplitGroup;
}


//------------------------------------------------------------------------------------------------
// FUNCTION:    CvSelectionGroup::getUnitIndex
//! \brief      Returns the index of the given unit in the selection group
//! \param      pUnit The unit to find the index of within the group
//! \retval     The zero-based index of the unit within the group, or -1 if it is not in the group.
//------------------------------------------------------------------------------------------------
int CvSelectionGroup::getUnitIndex(CvUnit* pUnit, int maxIndex /* = -1 */) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iIndex;

	iIndex = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit == pUnit)
		{
			return iIndex;
		}

		iIndex++;

		//early out if not interested beyond maxIndex
		if((maxIndex >= 0) && (iIndex >= maxIndex))
			return -1;
	}

	return -1;
}

CLLNode<IDInfo>* CvSelectionGroup::headUnitNode() const
{
	return m_units.head();
}


CvUnit* CvSelectionGroup::getHeadUnit() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	if (pUnitNode != NULL)
	{
		return ::getUnit(pUnitNode->m_data);
	}
	else
	{
		return NULL;
	}
}

CvUnit* CvSelectionGroup::getUnitAt(int index) const
{
	int numUnits = getNumUnits();
	if(index >= numUnits)
	{
		FAssertMsg(false, "[Jason] Selectiongroup unit index out of bounds.");
		return NULL;
	}
	else
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		for(int i=0;i<index;i++)
			pUnitNode = nextUnitNode(pUnitNode);
		
		CvUnit *pUnit = ::getUnit(pUnitNode->m_data);
		return pUnit;
	}
}


UnitAITypes CvSelectionGroup::getHeadUnitAI() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->AI_getUnitAIType();
	}

	return NO_UNITAI;
}


PlayerTypes CvSelectionGroup::getHeadOwner() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getOwnerINLINE();
	}

	return NO_PLAYER;
}


TeamTypes CvSelectionGroup::getHeadTeam() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getTeam();
	}

	return NO_TEAM;
}


void CvSelectionGroup::clearMissionQueue()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	deactivateHeadMission();

	m_missionQueue.clear();

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}

void CvSelectionGroup::setMissionPaneDirty()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);	

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


int CvSelectionGroup::getLengthMissionQueue() const																
{
	return m_missionQueue.getLength();
}


MissionData* CvSelectionGroup::getMissionFromQueue(int iIndex) const
{
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = m_missionQueue.nodeNum(iIndex);

	if (pMissionNode != NULL)
	{
		return &(pMissionNode->m_data);
	}
	else
	{
		return NULL;
	}
}


bool CvSelectionGroup::insertAtEndMissionQueue(MissionData mission, bool bStart)
{
	PROFILE_FUNC();

	FAssert(getOwnerINLINE() != NO_PLAYER);

	m_missionQueue.insertAtEnd(mission);

	if ((getLengthMissionQueue() == 1) && bStart)
	{
		if ( !activateHeadMission())
		{
			return false;
		}
	}

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}

	return true;
}


CLLNode<MissionData>* CvSelectionGroup::deleteMissionQueueNode(CLLNode<MissionData>* pNode)
{
	CLLNode<MissionData>* pNextMissionNode;

	FAssertMsg(pNode != NULL, "Node is not assigned a valid value");
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (pNode == headMissionQueueNode())
	{
		deactivateHeadMission();
	}

	pNextMissionNode = m_missionQueue.deleteNode(pNode);

	if (pNextMissionNode == headMissionQueueNode())
	{
		activateHeadMission();
	}

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}

	return pNextMissionNode;
}


CLLNode<MissionData>* CvSelectionGroup::nextMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.next(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::prevMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.prev(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::headMissionQueueNodeExternal() const
{
	static CLLNode<MissionData>	transformedMission;
	CLLNode<MissionData>*	pHeadMissionData = m_missionQueue.head();

	if ( pHeadMissionData != NULL )
	{
		transformedMission = *pHeadMissionData;
		CvViewport* pCurrentViewport;

		switch(transformedMission.m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
		case MISSION_ROUTE_TO:
		case MISSION_RANGE_ATTACK:
			pCurrentViewport = GC.getCurrentViewport();

			transformedMission.m_data.iData1 = pCurrentViewport->getViewportXFromMapX(transformedMission.m_data.iData1);
			transformedMission.m_data.iData2 = pCurrentViewport->getViewportYFromMapY(transformedMission.m_data.iData2);
			break;
		default:
			break;
		}

		return &transformedMission;
	}
	else
	{
		return NULL;
	}
}

CLLNode<MissionData>* CvSelectionGroup::headMissionQueueNode() const
{
	return m_missionQueue.head();
}


CLLNode<MissionData>* CvSelectionGroup::tailMissionQueueNode() const
{
	return m_missionQueue.tail();
}


int CvSelectionGroup::getMissionType(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if ( iNode == iCount )
		{
			return pMissionNode->m_data.eMissionType;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}


int CvSelectionGroup::getMissionData1(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if ( iNode == iCount )
		{
			return pMissionNode->m_data.iData1;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}


int CvSelectionGroup::getMissionData2(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if ( iNode == iCount )
		{
			return pMissionNode->m_data.iData2;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}


void CvSelectionGroup::read(FDataStreamBase* pStream)
{
	// Init saved data
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iID);
	pStream->Read(&m_iMissionTimer);

	pStream->Read(&m_bForceUpdate);

	pStream->Read((int*)&m_eOwner);
	pStream->Read((int*)&m_eActivityType);
	pStream->Read((int*)&m_eAutomateType);

	m_units.Read(pStream);
	m_missionQueue.Read(pStream);

	//	The order queue itself is not a streamable type so is serialized in raw
	//	binary image, which means we need to do some explicit translation on load
	//	if we are using the tagged format
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	if ( wrapper.isUsingTaggedFormat() )
	{
		CLLNode<MissionData>* pNode = headMissionQueueNode();
		while (pNode != NULL)
		{
			switch(pNode->m_data.eMissionType)
			{
			case MISSION_BUILD:
				if ( (pNode->m_data.iData1 = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDS,pNode->m_data.iData1)) == (int)NO_BUILD )
				{
					//	This build has been removed - delete this queue element
					pNode = deleteMissionQueueNode(pNode);
				}
				else
				{
					pNode = nextMissionQueueNode(pNode);
				}
				break;
			case MISSION_MOVE_TO:
			case MISSION_ROUTE_TO:
			case MISSION_NUKE:
			case MISSION_RECON:
			case MISSION_PARADROP:
			case MISSION_AIRBOMB:
			case MISSION_RANGE_ATTACK:
			case MISSION_AIRBOMB1:
			case MISSION_AIRBOMB2:
			case MISSION_AIRBOMB3:
			case MISSION_AIRBOMB4:
			case MISSION_AIRBOMB5:
			case MISSION_RBOMBARD:
			case MISSION_ABOMBARD:
			case MISSION_FENGAGE:
			case MISSION_CLAIM_TERRITORY:
			case MISSION_PRETARGET_NUKE:
				//	Fixup for viewports, old versions of which can leave things un-normalized
				pNode->m_data.iData1 = (pNode->m_data.iData1 + GC.getMapINLINE().getGridWidthINLINE()) % GC.getMapINLINE().getGridWidthINLINE();
				pNode->m_data.iData2 = (pNode->m_data.iData2 + GC.getMapINLINE().getGridHeightINLINE()) % GC.getMapINLINE().getGridHeightINLINE();
				// Drop through
			default:
				pNode = nextMissionQueueNode(pNode);
				break;
			}
		}
	}
}


void CvSelectionGroup::write(FDataStreamBase* pStream)
{
	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iID);
	pStream->Write(m_iMissionTimer);

	pStream->Write(m_bForceUpdate);

	pStream->Write(m_eOwner);
	pStream->Write(m_eActivityType);
	pStream->Write(m_eAutomateType);

	m_units.Write(pStream);
	m_missionQueue.Write(pStream);
}

// Protected Functions...

bool CvSelectionGroup::activateHeadMission()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (headMissionQueueNode() != NULL)
	{
		if (!isBusy())
		{
			//	We don't currently allow missions to execute in parallel because the (singleton) pathing
			//	engine won't (yet) support it.
			//	Note - only protecting the STARTING of missions here - higher level logic means
			//	we won't be running automated mission continuance in parallel

			return startMission();
		}
	}

	return true;
}


void CvSelectionGroup::deactivateHeadMission()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (headMissionQueueNode() != NULL)
	{
		if (getActivityType() == ACTIVITY_MISSION)
		{
			setActivityType(ACTIVITY_AWAKE);
		}

		setMissionTimer(0);

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
			}
		}
	}
}

/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
// Dale - SA: Stack Attack START
bool CvSelectionGroup::groupStackAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting, bool bStealth)
{
	CvPlot* pDestPlot = GC.getMapINLINE().plotINLINE(iX, iY);
	CvPlot* pOrigPlot = plot();
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	if (iFlags & MOVE_THROUGH_ENEMY)
	{
		if (generatePath(plot(), pDestPlot, iFlags))
		{
			pDestPlot = getPathFirstPlot();
		}
	}
	FAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");
	bool bStack = (isHuman() && ((getDomainType() == DOMAIN_AIR) || true));//GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_STACK_ATTACK)));
	bool bAttack = false;
	bool bAction = false;
	bFailedAlreadyFighting = false;
	bool bAffixFirstAttacker = false;
	bool bAffixFirstDefender = false;
	if (getNumUnits() > 0)
	{
		if ((getDomainType() == DOMAIN_AIR) || (stepDistance(getX(), getY(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()) <= 1))
		{
			if ((iFlags & MOVE_DIRECT_ATTACK) || (getDomainType() == DOMAIN_AIR) || (iFlags & MOVE_THROUGH_ENEMY) || (generatePath(plot(), pDestPlot, iFlags) && (getPathFirstPlot() == pDestPlot) || bStealth))
			{
				int iAttackOdds;
				CvUnit* pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, true, iAttackOdds);
				if (pBestAttackUnit)
				{
					// if there are no defenders, do not attack
					if (!pDestPlot->hasDefender(false, NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, true))
					{
						if (pDestPlot->hasStealthDefender(pBestAttackUnit) && !pDestPlot->isCity(false))
						{
							pDestPlot->revealBestStealthDefender(pBestAttackUnit);
							bStealth = true;
							bAffixFirstAttacker = true;
						}
					}

					if (isHuman())
					{
						if (!(isLastPathPlotVisible()) && (getDomainType() != DOMAIN_AIR))
						{
							return false;
						}
					}

					CvUnit* pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, false, bStealth);
					if (pBestDefender != NULL)
					{
						bAffixFirstDefender = true;
					}

					bool bNoBlitz = (!pBestAttackUnit->isBlitz() && pBestAttackUnit->isMadeAttack() && !bStealth);

					if (groupDeclareWar(pDestPlot))
					{
						return true;
					}
					// RevDCM Battle Effects option
					pBestAttackUnit->setBattlePlot(pDestPlot, pBestDefender);
					
					// RevolutionDCM - attack support
					if (GC.isDCM_ATTACK_SUPPORT())
					{
						if (pDestPlot->getNumUnits() > 0 && !bStealth)
						{
							pUnitNode = pDestPlot->headUnitNode();
							while (pUnitNode != NULL)
							{
								pLoopUnit = ::getUnit(pUnitNode->m_data);
								pUnitNode = pDestPlot->nextUnitNode(pUnitNode);
								if (pLoopUnit == NULL)
									break;
								if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(getTeam()))
								{
									if (plot() != NULL)
									{
										if (pLoopUnit->canArcherBombardAt(pDestPlot, plot()->getX_INLINE(), plot()->getY_INLINE()))
										{
											if (pLoopUnit->archerBombard(plot()->getX_INLINE(), plot()->getY_INLINE(), true))
											{
												bAction = true;
											}
										}
										else if (pLoopUnit->canBombardAtRanged(pDestPlot, plot()->getX_INLINE(), plot()->getY_INLINE()))
										{
											if (pLoopUnit->bombardRanged(plot()->getX_INLINE(), plot()->getY_INLINE(), true))
											{
												bAction = true;
											}
										}
										else if (pLoopUnit->getDomainType() == DOMAIN_AIR && !pLoopUnit->isSuicide())
										{
											pLoopUnit->updateAirStrike(plot(), false, true);//airStrike(plot()))
										}
										pLoopUnit->setMadeAttack(false);
									}
								}
							}
						} 
						else 
						{
							return bAttack;
						}
						if (pOrigPlot->getNumUnits() > 0 && !bStealth)
						{
							pUnitNode = pOrigPlot->headUnitNode();
							while (pUnitNode != NULL)
							{
								pLoopUnit = ::getUnit(pUnitNode->m_data);
								pUnitNode = pOrigPlot->nextUnitNode(pUnitNode);
								if (pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
								{
									if (pLoopUnit != NULL && this != NULL && pDestPlot != NULL && plot() != NULL)
									{
										if (pLoopUnit->canArcherBombardAt(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
										{
											if (pLoopUnit->archerBombard(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), true))
											{
												bAction = true;
											}
										}
										else if (pLoopUnit->canBombardAtRanged(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
										{
											if (pLoopUnit->bombardRanged(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), true))
											{
												bAction = true;
											}
										}
										else if (pLoopUnit->getDomainType() == DOMAIN_AIR && !pLoopUnit->isSuicide())
										{
											if (plotDistance(plot()->getX_INLINE(), plot()->getY_INLINE(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()) == 1)
											{
												pLoopUnit->updateAirStrike(pDestPlot, false, false);//airStrike(pDestPlot))
											}
										}
										pLoopUnit->setMadeAttack(false);
									}
								}
							}
						}
						else 
						{
							return bAttack;
						}
					}
					// RevolutionDCM - attack support end
					bool bBombardExhausted = false;
					while (true)
					{
						if (!bAffixFirstAttacker)
						{
							pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, false, iAttackOdds, false, bNoBlitz, 0, false, bStealth);
						}
						if (pBestAttackUnit == NULL)
						{
							break;
						}				
						else
						{
							// if there are no defenders, do not attack
							if (!bAffixFirstAttacker && !pDestPlot->hasDefender(false, NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, true))
							{
								if (pDestPlot->hasStealthDefender(pBestAttackUnit) && !pDestPlot->isCity(false))
								{
									pDestPlot->revealBestStealthDefender(pBestAttackUnit);
									bStealth = true;
								}
							}
							if (!bAffixFirstDefender)
							{
								pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true, false, false, false, true);
							}
							bAffixFirstAttacker = false;
							bAffixFirstDefender = false;
						}
						if (iAttackOdds < 68 && !bStealth && !isHuman())
						{
							if (bBombardExhausted)
							{
								CvUnit * pBestSacrifice = AI_getBestGroupSacrifice(pDestPlot, false, false, bNoBlitz, bStealth);
								if (pBestSacrifice != NULL)
								{
									pBestAttackUnit = pBestSacrifice;
								}
							}
							else
							{
								bool bFoundBombard = false;

								OutputDebugString("Attempting to bombard tough plot\n");
								if (pOrigPlot->getNumUnits() > 0)
								{
									pUnitNode = headUnitNode();
									while (pUnitNode != NULL)
									{
										pLoopUnit = ::getUnit(pUnitNode->m_data);
										pUnitNode = nextUnitNode(pUnitNode);

										if (pLoopUnit != NULL && this != NULL && pDestPlot != NULL && plot() != NULL)
										{
											if (pLoopUnit->canArcherBombardAt(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
											{
												bFoundBombard = true;
												pLoopUnit->archerBombard(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), false);
											}
											else if (pLoopUnit->canBombardAtRanged(plot(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()))
											{
												bFoundBombard = true;
												pLoopUnit->bombardRanged(pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), false);
											}
										}
									}
								}

								bBombardExhausted = !bFoundBombard;

								continue;
							}
						}
						bAttack = true;

						if (GC.getUSE_CAN_DO_COMBAT_CALLBACK())
						{

							CySelectionGroup* pyGroup = new CySelectionGroup(this);
							CyPlot* pyPlot = new CyPlot(pDestPlot);
							CyArgsList argsList;
							argsList.add(gDLL->getPythonIFace()->makePythonObject(pyGroup));	// pass in Selection Group class
							argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));	// pass in Plot class
							long lResult=0;
							PYTHON_CALL_FUNCTION4(__FUNCTION__, PYGameModule, "doCombat", argsList.makeFunctionArgs(), &lResult);
							delete pyGroup;	// python fxn must not hold on to this pointer 
							delete pyPlot;	// python fxn must not hold on to this pointer 
							if (lResult == 1)
							{
								break;
							}
						}

						bool bQuick = (bStack || bStealth);
						pBestAttackUnit->attack(pDestPlot, bQuick, bStealth);
						if (bFailedAlreadyFighting || !bStack)
						{
							// if this is AI or automated stack, follow through with the attack to the end
							if ((!isHuman() || isAutomated()) && getNumUnits() > 1)
							{
								AI_queueGroupAttack(iX, iY);
							}
							
							break;
						}
					}
				}
			}
		}
	}

	return bAttack;
}
// Dale - SA: Stack Attack END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/

// BUG - All Units Actions - start
bool CvSelectionGroup::allMatch(UnitTypes eUnit) const
{
	FAssertMsg(eUnit >= 0, "eUnit expected to be >= 0");
	FAssertMsg(eUnit < GC.getNumUnitInfos(), "eUnit expected to be < GC.getNumUnitInfos()");

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;

	FAssertMsg(pUnitNode != NULL, "headUnitNode() expected to be non-NULL");

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->getUnitType() != eUnit)
		{
			return false;
		}
	}

	return true;
}
// BUG - All Units Actions - end

// BUG - Safe Move - start
void CvSelectionGroup::checkLastPathPlot(CvPlot* pPlot)
{
	m_bLastPathPlotChecked = true;
	if (pPlot != NULL)
	{
		m_bLastPlotVisible = pPlot->isVisible(getTeam(), false);
		m_bLastPlotRevealed = pPlot->isRevealed(getTeam(), false);
	}
	else
	{
		m_bLastPlotVisible = false;
		m_bLastPlotRevealed = false;
	}
}

void CvSelectionGroup::clearLastPathPlot()
{
	m_bLastPathPlotChecked = false;
}

bool CvSelectionGroup::isLastPathPlotChecked() const
{
	return m_bLastPathPlotChecked;
}

bool CvSelectionGroup::isLastPathPlotVisible() const
{
	return m_bLastPathPlotChecked ? m_bLastPlotVisible : false;
}

bool CvSelectionGroup::isLastPathPlotRevealed() const
{
	return m_bLastPathPlotChecked ? m_bLastPlotRevealed : false;
}
// BUG - Safe Move - end

int CvSelectionGroup::defensiveModifierAtPlot(CvPlot* pPlot) const
{
	int	iModifier = pPlot->defenseModifier(getTeam(), false);

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;
	int iBestStrength = 0;
	int iBestExtraModifier = 0;

	FAssertMsg(pUnitNode != NULL, "headUnitNode() expected to be non-NULL");

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ( pLoopUnit->canDefend() )
		{
			int iStrength = pLoopUnit->currHitPoints();
			int iUnitModifier = 0;

			if (pPlot->isHills())
			{
				iUnitModifier += pLoopUnit->hillsDefenseModifier();
			}

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				iUnitModifier += pLoopUnit->featureDefenseModifier(pPlot->getFeatureType());
			}

			iUnitModifier += pLoopUnit->terrainDefenseModifier(pPlot->getTerrainType());

			iStrength = (iStrength * (100 + iUnitModifier))/100;

			if ( iStrength > iBestStrength )
			{
				iBestStrength = iStrength;
				iBestExtraModifier = iUnitModifier;
			}
		}
	}

	return iModifier + iBestExtraModifier;
}

int CvSelectionGroup::getStrength() const
{
	int iStrength = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;

	FAssertMsg(pUnitNode != NULL, "headUnitNode() expected to be non-NULL");

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		iStrength += pLoopUnit->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);
	}

	return iStrength/100;
}

bool CvSelectionGroup::hasCommander() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;

	FAssertMsg(pUnitNode != NULL, "headUnitNode() expected to be non-NULL");

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ( pLoopUnit->isCommander() )
		{
			return true;
		}
	}

	return false;
}

void CvSelectionGroup::validateLocations(bool bFixup) const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;
	CvPlot* pPlot = NULL;
	CvUnit* pTransportUnit;
	bool	bTransportUnitSet = false;

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ( !bTransportUnitSet )
		{
			bTransportUnitSet = true;
			pTransportUnit = pLoopUnit->getTransportUnit();
		}

		if (pTransportUnit != pLoopUnit->getTransportUnit())
		{
			if ( bFixup )
			{
				FAssertMsg(false, "Group split in cargo state - fixing");

				pLoopUnit->joinGroup(NULL);
			}
			else
			{
				if(!pLoopUnit->isHuman())
				{
					FAssertMsg(false, "Group split in cargo state");
				}
			}
		}

		if ( pPlot == NULL )
		{
			pPlot = pLoopUnit->plot();
		}
		else
		{
			if ( bFixup )
			{
				if ( pPlot != pLoopUnit->plot() )
				{
					FAssertMsg(false, "Incorrect plot on unit of group - fixing");

					//	Drop the errant unit from the group
					pLoopUnit->joinGroup(NULL);
				}
			}
			else
			{
				if(!pLoopUnit->isHuman())
				{
					FAssertMsg(pPlot == pLoopUnit->plot(), "Incorrect plot on unit of group");
				}
			}
		}
	}
}

bool CvSelectionGroup::findNewLeader(UnitAITypes eAIType)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit = NULL;
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (kPlayer.AI_unitValue(pLoopUnit->getUnitType(), eAIType, NULL) > 0)
		{
			pBestUnit = pLoopUnit;
			break;
		}
		else if (eAIType == UNITAI_ATTACK_CITY && pLoopUnit->canAttack())
		{
			//	If there is no unit that explicitly supports the attack city role
			//	accept any that can attack so as to keep the stack's intent
			pBestUnit = pLoopUnit;
		}
	}

	if (pBestUnit != NULL)
	{
		m_bIsChoosingNewLeader = true;

		pBestUnit->AI_setUnitAIType(eAIType);

		m_bIsChoosingNewLeader = false;

		pBestUnit->joinGroup(this);

		return true;
	}

	return false;
}

bool CvSelectionGroup::doMergeCheck()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;
	//CvUnit* pSplitUnit;
	//TB Notes: I'd like to add that a unit should only merge with units of the same AI to avoid problems there.
	bool bInhibitMerge = false; //placing this here so as to prep for developing a setting for the unit.
	bool bHasMerged = false;

	while (pUnitNode != NULL && !bHasMerged)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		bInhibitMerge = false; //placing this here so as to prep for developing a setting for the unit.
		if (!bInhibitMerge && pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			//inhibit workers from this function since some sacrifice themselves (which would require much more programming to manage for the ai) and worker merging is only a way for players to manage less units really.
			bInhibitMerge = true;
		}
		//Note: this canMerge(true) check will be ordering a 4th potentially mergable unit to split instead during it's check processing.
		//The thinking behind this method is that when we merge 3 units we want a 4th one that was capable of it to be present and to split so that the unit count remains the same and for the alternative strategy of splitting to be equally expressed.
		//For defense this means you create fodder flak to hold off minimalist unit count armies, buying time, and a strong lead defender to make a tough stand
		//For attack you have a strong lead attacker to bust through stiff opposition and some smaller units to wipe up defenders weakened by collateral (or splitting strategies to delay the capture of the city or position.)
		//After a few round of such merging among particular types in the same location will create a nice gradient of unit group sizes.  Should be interesting to see its effect in play.
		//TBSPLIT
		if (pLoopUnit->canMerge(true) && !bInhibitMerge)
		{
			//if (GET_PLAYER(getOwner()).getSplittingUnit() != NO_UNIT)
			//{
			//	pSplitUnit = GET_PLAYER(getOwner()).getUnit(GET_PLAYER(getOwner()).getSplittingUnit());
			//	pSplitUnit->doSplit();
			//}
			pLoopUnit->doMerge();
			bHasMerged = true;
		}
	}
	if (bHasMerged)
	{
		if (doMergeCheck())
		{
			return true;
		}
		return true;
	}
	return false;
}

int CvSelectionGroup::getCargoSpace()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCargoCount = 0;

	if (getNumUnits() > 0)
	{
		UnitAITypes eUnitAI = getHeadUnitAI();
		
		// first pass, count but ignore special cargo units
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				if (pLoopUnit->cargoSpace() > 0 || pLoopUnit->SMcargoSpace() > 0)
				{
					if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
					{
						iCargoCount += pLoopUnit->SMcargoSpace();
					}
					else
					{
						iCargoCount += pLoopUnit->cargoSpace();
					}
				}
			}
		}
	}

	return iCargoCount;	
}

int CvSelectionGroup::getCargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCargoCount = 0;

	if (getNumUnits() > 0)
	{
		UnitAITypes eUnitAI = getHeadUnitAI();
		
		// first pass, count but ignore special cargo units
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
				{
					iCargoCount += pLoopUnit->SMcargoSpaceAvailable(eSpecialCargo, eDomainCargo);
				}
				else
				{
					iCargoCount += pLoopUnit->cargoSpaceAvailable(eSpecialCargo, eDomainCargo);
				}
			}
		}
	}

	return iCargoCount;	
}

int CvSelectionGroup::countSeeInvisibleActive(UnitAITypes eUnitAI, InvisibleTypes eInvisibleType) const
{
	int iCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			if (GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
			{
				if (pLoopUnit->visibilityIntensityTotal(eInvisibleType) > 0)
				{
					iCount++;
				}
			}
			else
			{
				for (int iI = 0; iI < pLoopUnit->getNumSeeInvisibleTypes(); ++iI)
				{
					if (pLoopUnit->getSeeInvisibleType(iI) == (int)eInvisibleType)
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

void CvSelectionGroup::releaseUnitAIs(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (eUnitAI == pLoopUnit->AI_getUnitAIType())
		{
			pLoopUnit->joinGroup(NULL);
		}
	}
	return;
}

CvUnit* CvSelectionGroup::unit_iterator::resolve(const IDInfo& info) const
{
	return ::getUnit(info);
}
