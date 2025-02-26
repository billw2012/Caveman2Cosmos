//	Internal path generation engine

#include "CvGameCoreDLL.h"
#include "CvSelectionGroup.h"
#include "CvBugOptions.h"
#include "CvPathGenerator.h"

#include <boost/bind.hpp>

#include "CvGameAI.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvGameCoreUtils.h"

#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
#define	VALIDATE_TREE(x,y,z)	ValidateTree(x,y,z);
#else
#define	VALIDATE_TREE(x,y,z)	;
#endif

#define SIGNIFICANT_PATHING_FLAGS	(~(MOVE_DIRECT_ATTACK | MOVE_MAX_MOVES | MOVE_RECONSIDER_ON_LEAVING_OWNED | MOVE_AVOID_ENEMY_UNITS | MOVE_WITH_CAUTION | MOVE_HEAL_AS_NEEDED25))

#define	MAX_DEFAULT_OPTIMIZATION_PERCENT		10000
#define	OPTIMIZATION_PERCENT_LOSS_PER_DISTANCE	200
#define	CEILING_ITERATIONS						20000
#define	MIN_OPTIMIZATION_ITERATIONS				200

#ifndef FINAL_RELEASE
static bool g_tracePathing = false;
#define	TRACE_PATHING	g_tracePathing
#else
#define TRACE_PATHING	false
#endif
//#define	DETAILED_TRACE

//	Helper class representing a path tree node
class CvPathNode
{
public:
	CvPathNode()
	{
		m_firstChild	= NULL;
		m_prevSibling	= NULL;
		m_nextSibling	= NULL;
		m_plot			= NULL;
	}

	~CvPathNode()
	{
	}

	int			m_iPathTurns;
	int			m_iMovementRemaining;
	CvPathNode*	m_parent;
	CvPathNode*	m_firstChild;
	CvPathNode*	m_prevSibling;
	CvPathNode*	m_nextSibling;
	CvPlot*		m_plot;
	int			m_iBestToEdgeCost;
	int			m_iCostTo;
	int			m_iCostFrom;
	int			m_iLowestPossibleCostFrom;
	int			m_iPathSeq;	//	Data updated for path generation that matches this seq
	int			m_iEdgesIncluded;	//	Bitmap by direction out
	bool		m_bProcessedAsTerminus;
	int			m_iModificationSeq;	//	Incremented each time this node's info is modified
	int			m_iLowestDequeueCost;
	int			m_iRecalcThreshold;
	bool		m_bIsKnownRoute;
#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
	int			m_iValidationSeq;
	bool		m_bIsQueued;
#endif
};

CvPath::const_iterator::const_iterator(CvPathNode* cursorNode)
{
	m_cursorNode = cursorNode;
}

CvPath::const_iterator& CvPath::const_iterator::operator++()
{
	if ( m_cursorNode != NULL )
	{	
		if ( m_cursorNode->m_bProcessedAsTerminus )
		{
			m_cursorNode = NULL;
		}
		else
		{
			m_cursorNode = m_cursorNode->m_firstChild;
		}
	}

	return (*this);
}

bool CvPath::const_iterator::operator==(const_iterator& other)
{
	return m_cursorNode == other.m_cursorNode;
}

bool CvPath::const_iterator::operator!=(const_iterator& other)
{
	return m_cursorNode != other.m_cursorNode;
}

CvPlot*	CvPath::const_iterator::plot()
{
	return (m_cursorNode == NULL ? NULL : m_cursorNode->m_plot);
}

int	CvPath::const_iterator::turn()
{
	return (m_cursorNode == NULL ? NULL : m_cursorNode->m_iPathTurns);
}

CvPath::CvPath()
{
	m_startNode = NULL;
}

void CvPath::Set(CvPathNode* startNode)
{
	m_startNode = startNode;
	m_endNode = m_startNode;

	if ( startNode != NULL )
	{
		if ( TRACE_PATHING )
		{
			OutputDebugString("Path generated:\n");
		}

		while(m_endNode != NULL && m_endNode->m_firstChild != NULL && !m_endNode->m_bProcessedAsTerminus )
		{
			if ( TRACE_PATHING )
			{
				OutputDebugString(CvString::format("\t->(%d,%d) [cost %d, turns %d]\n", m_endNode->m_plot->getX_INLINE(), m_endNode->m_plot->getY_INLINE(), m_endNode->m_iCostTo, m_endNode->m_iPathTurns).c_str());
			}
			m_endNode = m_endNode->m_firstChild;
		}
	}
}

CvPath::const_iterator CvPath::begin()
{
	return CvPath::const_iterator(m_startNode);
}

CvPath::const_iterator CvPath::end()
{
	return CvPath::const_iterator(NULL);
}

int	CvPath::length() const
{
	return (m_endNode == NULL ? MAX_INT : m_endNode->m_iPathTurns);
}

CvPlot*	CvPath::lastPlot() const
{
	return (m_endNode == NULL ? NULL : m_endNode->m_plot);
}

int	CvPath::cost() const
{
	return (m_endNode == NULL ? -1 : m_endNode->m_iCostTo);
}
	
bool	CvPath::containsEdge(const CvPlot* pFromPlot, const CvPlot* pToPlot) const
{
	CvPathNode*	pNode = m_startNode;

	while( pNode != NULL && pNode->m_firstChild != NULL )
	{
		if ( pNode->m_plot == pFromPlot && pNode->m_firstChild->m_plot == pToPlot )
		{
			return true;
		}

		pNode = pNode->m_firstChild;
	}

	return false;
}
	
bool	CvPath::containsNode(const CvPlot* pPlot) const
{
	CvPathNode*	pNode = m_startNode;

	while( pNode != NULL )
	{
		if ( pNode->m_plot == pPlot )
		{
			return true;
		}

		pNode = pNode->m_firstChild;
	}

	return false;
}

void	CvPath::trimBefore(const CvPlot* pPlot)
{
	CvPathNode*	pNode = m_startNode;

	while( pNode != NULL )
	{
		if ( pNode->m_plot == pPlot )
		{
			m_startNode = pNode;
			break;
		}

		pNode = pNode->m_firstChild;
	}
}

int		CvPath::movementRemaining() const
{
	if ( m_endNode == NULL )
	{
		return 0;
	}
	else
	{
		return m_endNode->m_iMovementRemaining;
	}
}

class CvPathGeneratorPlotInfo
{
public:
	CvPathGeneratorPlotInfo() {}

	CvPathNode* pNode;
	int			m_iEdgesValidated;	//	Bitmap by direction out
	bool		bKnownInvalidNode;
};

class CvPathPlotInfoStore
{
public:
	CvPathPlotInfoStore(CvMap* pMap, int iStartSeq)
	{
		m_seq = iStartSeq;
	}

	~CvPathPlotInfoStore()
	{
	}

	void reset()
	{
		m_seq++;
		m_allocationPool.reset();
	}

	CvPathGeneratorPlotInfo*	getPlotInfo(const CvPlot* pPlot, bool bCreateIfNotFound = true)
	{
		PROFILE_FUNC();

		if ( pPlot->m_pathGenerationSeq != m_seq )
		{
			if ( bCreateIfNotFound )
			{
				pPlot->m_currentPathInfo = m_allocationPool.allocateNode();
				pPlot->m_pathGenerationSeq = m_seq;

				pPlot->m_currentPathInfo->pNode = NULL;
				pPlot->m_currentPathInfo->bKnownInvalidNode = false;
				pPlot->m_currentPathInfo->m_iEdgesValidated = 0;
			}
			else
			{
				return NULL;
			}
		}

		return pPlot->m_currentPathInfo;
	}

	static void clearPlotInfo(const CvPlot* pPlot)
	{
		if ( pPlot->m_currentPathInfo != NULL )
		{
			pPlot->m_currentPathInfo->pNode = NULL;
		}
	}

private:
	int											m_seq;
	CvAllocationPool<CvPathGeneratorPlotInfo>	m_allocationPool;
};

//	Unlink a node (well strictly a subtree)
static void UnlinkNode(CvPathNode* node)
{
	if ( node->m_parent != NULL )
	{
		if ( node->m_parent->m_firstChild == node )
		{
			node->m_parent->m_firstChild = node->m_nextSibling;
		}
		else
		{
			FAssert(node->m_prevSibling != node->m_nextSibling);

			if ( node->m_prevSibling == NULL )
			{
				OutputDebugString("Non-first node has no prev sibling!\n");
			}
			node->m_prevSibling->m_nextSibling = node->m_nextSibling;

			FAssert(node->m_prevSibling->m_prevSibling == NULL || node->m_prevSibling->m_prevSibling != node->m_prevSibling->m_nextSibling);
		}

		if ( node->m_nextSibling != NULL )
		{
			FAssert(node->m_nextSibling != node->m_prevSibling);

			node->m_nextSibling->m_prevSibling = node->m_prevSibling;

			FAssert(node->m_nextSibling->m_prevSibling == NULL || node->m_nextSibling->m_prevSibling != node->m_nextSibling->m_nextSibling);
		}
	}

	node->m_iModificationSeq++;
}

void CvPathGenerator::ValidatePlotInfo(CvPathGeneratorPlotInfo* pPlotInfo)
{
	if ( pPlotInfo->pNode != NULL )
	{
		FAssert(pPlotInfo->pNode->m_plot == NULL || m_plotInfo->getPlotInfo(pPlotInfo->pNode->m_plot, false) == pPlotInfo);
	}
}

CvPathGenerator::CvPathGenerator(CvMap* pMap)
{
	m_map = pMap;
	//m_priorityQueue = new std::multimap<int, CvPathNode*>();
	m_plotInfo = new CvPathPlotInfoStore(pMap, 0);
	m_nodeAllocationPool = new CvAllocationPool<CvPathNode>();

	m_nodesProcessed = 0;
	m_nodesCosted = 0;

	reset();
}

CvPathGenerator::~CvPathGenerator()
{
	//SAFE_DELETE(m_priorityQueue);
	SAFE_DELETE(m_plotInfo);
	SAFE_DELETE(m_nodeAllocationPool);
}

void CvPathGenerator::reset()
{
	m_iSeq = 0;
	m_iTurn = -1;
	m_currentGroupMembershipChecksum = 0;
	m_pReplacedNonTerminalNode = NULL;
	m_pBestTerminalNode = NULL;
	m_pFrom = NULL;
}

void CvPathGenerator::Initialize(HeuristicCost HeuristicFunc, EdgeCost CostFunc, EdgeValidity ValidFunc, TerminusValidity TerminusValidFunc, TurnEndValidityCheckRequired TurnEndValidCheckNeeded)
{
	m_HeuristicFunc		= HeuristicFunc;
	m_CostFunc			= CostFunc;
	m_ValidFunc			= ValidFunc;
	m_TerminusValidFunc = TerminusValidFunc;
	m_TurnEndValidCheckNeeded = TurnEndValidCheckNeeded;
}

#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
bool CvPathGenerator::ValidateTreeInternal(CvPathNode* root, int& iValidationSeq, CvPathNode* unreferencedNode, CvPathNode* referencedNode, int& iQueuedCount)
{
	bool bResult = (referencedNode == NULL || root == referencedNode);
	int	iStartSeq = iValidationSeq;

	if ( root != NULL )
	{
		CvPathNode*	child;

		FAssert(m_nodeAllocationPool->isAllocated(root));
		FAssert(root != unreferencedNode);

		if (root->m_bIsQueued)
		{
			iQueuedCount++;
		}

		for(child = root->m_firstChild; child != NULL; child = child->m_nextSibling)
		{
			FAssert(child->m_iValidationSeq == 0 || iStartSeq - child->m_iValidationSeq >= 0);	//	Couched this way to allow for seq wrapping
			FAssert(child->m_parent == root);

			if ( child->m_prevSibling == NULL )
			{
				FAssert(child == root->m_firstChild);
			}
			else
			{
				FAssert(child == child->m_prevSibling->m_nextSibling);
			}

			child->m_iValidationSeq = ++iValidationSeq;

			bResult |= ValidateTreeInternal(child, iValidationSeq, unreferencedNode, referencedNode, iQueuedCount);
		}
	}

	return bResult;
}

void CvPathGenerator::ValidateTree(CvPathNode* root, CvPathNode* unreferencedNode, CvPathNode* referencedNode)
{
	static int iSeq = 0;
	int iQueuedCount = 0;

	FAssert(ValidateTreeInternal(root, iSeq, unreferencedNode, referencedNode, iQueuedCount));
	//FAssert(iQueuedCount == m_priorityQueue.size());
}
#endif

//	Link a node into a specified parent
void CvPathGenerator::LinkNode(CvPathNode* node, CvPathNode* parent)
{
	FAssert(node != m_pReplacedNonTerminalNode);

	node->m_prevSibling = NULL;
	
	FAssert(parent->m_firstChild == NULL || (parent->m_firstChild != node && parent->m_firstChild->m_nextSibling != node));
	if ( (node->m_nextSibling = parent->m_firstChild) != NULL )
	{
		parent->m_firstChild->m_prevSibling = node;
	}

	parent->m_firstChild = node;
	node->m_parent = parent;
}

//	Move node to some other point in the tree
void CvPathGenerator::RelinkNode(CvPathNode* node, CvPathNode* parent)
{
	UnlinkNode(node);
	LinkNode(node, parent);
}

CvPathNode*	CvPathGenerator::allocatePathNode()
{
	CvPathNode* node = m_nodeAllocationPool->allocateNode();

	node->m_parent = NULL;
	node->m_firstChild = NULL;
	node->m_nextSibling = NULL;
	node->m_prevSibling = NULL;
	node->m_iPathTurns = 0;
	node->m_iMovementRemaining = 0;
	node->m_plot = NULL;
	node->m_iCostTo = 0;
	node->m_iCostFrom = 0;
	node->m_iEdgesIncluded = 0;
	node->m_bProcessedAsTerminus = false;
	node->m_iPathSeq = -1;
	node->m_iLowestDequeueCost = MAX_INT;
	node->m_iRecalcThreshold = MAX_INT;
	node->m_bIsKnownRoute = false;
#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
	node->m_iValidationSeq = 0;
	node->m_bIsQueued = false;
#endif

	return node;
}

bool CvPathGenerator::isDescendantOfReplacedNode(CvPathNode* node) const
{
	if ( m_pReplacedNonTerminalNode != NULL &&
		 m_pReplacedNonTerminalNode->m_iPathTurns <= node->m_iPathTurns )
	{
		while(node != NULL)
		{
			if (node == m_pReplacedNonTerminalNode)
			{
				return true;
			}

			node = node->m_parent;
		}
	}

	return false;
}

void CvPathGenerator::AdjustChildTreeCosts(CvPathNode* node, int iAmount, bool bHasQueued)
{
	if ( iAmount != 0 )
	{
		node->m_iCostTo += iAmount;
		node->m_iPathTurns = node->m_parent->m_iPathTurns + (node->m_parent->m_iMovementRemaining == 0 ? 1 : 0);

		if ( TRACE_PATHING )
		{
			OutputDebugString(CvString::format("\tAdjust costTo (%d,%d) by %d, new cost/turns: %d/%d\n", node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE(), iAmount, node->m_iCostTo, node->m_iPathTurns).c_str());
		}

		bool bNeedsRequeue = (-iAmount > node->m_iRecalcThreshold);

		node->m_iRecalcThreshold += iAmount;
		if ( node->m_iRecalcThreshold < 0 )
		{
			node->m_iRecalcThreshold = 0;
		}

		if ( bNeedsRequeue )
		{
			if ( !bHasQueued )
			{
				if ( TRACE_PATHING )
				{
					OutputDebugString(CvString::format("Requeue in cost adjustment (%d,%d) with new cost %d\n", node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE(), node->m_iCostTo).c_str());
				}
				MEMORY_TRACK_EXEMPT();

				priorityQueueEntry	entry;

				node->m_iPathSeq = m_iSeq;

				entry.node = node;
				entry.iQueuedCost = node->m_iCostTo;

				m_priorityQueue.push(entry);

				bHasQueued = true;
			}
			else
			{
				node->m_iPathSeq = -1;
			}

			if ( NULL != node->m_plot )
			{
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++ )
				{
					CvPlot* pAdjacentPlot = plotDirection(node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE(), (DirectionTypes)iI);
					if ( pAdjacentPlot != NULL )
					{
						CvPathGeneratorPlotInfo* pAdjacentInfo = m_plotInfo->getPlotInfo(pAdjacentPlot, false);

						if ( pAdjacentInfo != NULL && pAdjacentInfo->pNode != NULL )
						{
							pAdjacentInfo->pNode->m_iEdgesIncluded &= ~(1 << iI);
						}
					}
				}
			}
		}

		CvPathNode* child;

		for(child = node->m_firstChild; child != NULL ; child = child->m_nextSibling)
		{
			AdjustChildTreeCosts(child, iAmount, bHasQueued);
		}
	}
}

void CvPathGenerator::OrphanChildTree(CvPathNode* node)
{
	CvPathNode*	child;

	node->m_parent = NULL;

	for(child = node->m_firstChild; child != NULL ; child = child->m_nextSibling)
	{
		OrphanChildTree(child);
	}
}

void CvPathGenerator::DeleteChildTree(CvPathNode* node, bool bIsDeletionRoot)
{
	CvPathNode* child;

	if ( !bIsDeletionRoot )
	{
		if ( TRACE_PATHING )
		{
			OutputDebugString(CvString::format("\tDelete child (%d,%d)\n", node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE()).c_str());
		}
		if ( m_pBestTerminalNode == node )
		{
			m_pBestTerminalNode = NULL;
		}
		if ( m_pReplacedNonTerminalNode == node )
		{
			m_pReplacedNonTerminalNode = NULL;
		}

		FAssert(NULL != node->m_plot);

		if (m_plotInfo->getPlotInfo(node->m_plot, false) != NULL && m_plotInfo->getPlotInfo(node->m_plot, false)->pNode == node)
		{
			CvPathPlotInfoStore::clearPlotInfo(node->m_plot);
		}
	}
	else
	{
		for(child = node->m_firstChild; child != NULL ; child = child->m_nextSibling)
		{
			OrphanChildTree(child);
		}
	}

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++ )
	{
		CvPlot* pAdjacentPlot = plotDirection(node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE(), (DirectionTypes)iI);
		if ( pAdjacentPlot != NULL )
		{
			CvPathGeneratorPlotInfo* pAdjacentInfo = m_plotInfo->getPlotInfo(pAdjacentPlot, false);

			if ( pAdjacentInfo != NULL && pAdjacentInfo->pNode != NULL )
			{
				if ( (pAdjacentInfo->pNode->m_iEdgesIncluded & (1 << iI)) != 0 &&
					 pAdjacentInfo->pNode->m_parent != NULL &&
					 pAdjacentInfo->pNode->m_iPathSeq == m_iSeq &&
					 !bIsDeletionRoot )
				{
					if ( TRACE_PATHING )
					{
						OutputDebugString(CvString::format("Requeue after subtree deletion (%d,%d) with cost %d\n", pAdjacentInfo->pNode->m_plot->getX_INLINE(), pAdjacentInfo->pNode->m_plot->getY_INLINE(), pAdjacentInfo->pNode->m_iCostTo).c_str());
					}
					MEMORY_TRACK_EXEMPT();

					priorityQueueEntry	entry;

					node->m_iPathSeq = m_iSeq;

					entry.node = pAdjacentInfo->pNode;
					entry.iQueuedCost = pAdjacentInfo->pNode->m_iCostTo;

					m_priorityQueue.push(entry);
				}

				pAdjacentInfo->pNode->m_iEdgesIncluded &= ~(1 << iI);
			}
		}
	}

	if ( m_pReplacedNonTerminalNode != NULL && m_pReplacedNonTerminalNode->m_parent == node )
	{
		DeleteChildTree(m_pReplacedNonTerminalNode, false);
	}

	for(child = node->m_firstChild; child != NULL ; child = child->m_nextSibling)
	{
		DeleteChildTree(child, false);
	}

	node->m_firstChild = NULL;
}

bool	CvPathGenerator::groupMatches(CvSelectionGroup* pGroup, int iFlags, unsigned int& iGroupMembershipChecksum)
{
	iGroupMembershipChecksum = 0;
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = pGroup->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pGroup->nextUnitNode(pUnitNode);

		CheckSum(iGroupMembershipChecksum, pLoopUnit->getID());
	}

	if ( m_currentGroupMembershipChecksum != iGroupMembershipChecksum || m_iFlags != iFlags )
	{
		return false;
	}

	return (m_iTurn == GC.getGameINLINE().getGameTurn() && m_currentGroupMembershipChecksum == iGroupMembershipChecksum && m_iFlags == iFlags);
}

bool	CvPathGenerator::haveRouteLength(const CvPlot* pTo, CvSelectionGroup* pGroup, int iFlags, int& iRouteLen)
{
	unsigned int dummy;
	//	Only consider flags that effect the calculated path
	iFlags &= SIGNIFICANT_PATHING_FLAGS;

	if ( groupMatches(pGroup, iFlags, dummy) )
	{
		CvPathGeneratorPlotInfo* pPlotInfo = m_plotInfo->getPlotInfo(pTo, false);

		if ( pPlotInfo != NULL && pPlotInfo->pNode != NULL )
		{
			iRouteLen = pPlotInfo->pNode->m_iPathTurns;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


bool CvPathGenerator::isBetterPath(
	 CvPathNode*	existing,
	 CvPathNode*	from,
	 int			iEdgeCost,
	 int			iMovementRemaining) const
{
	if ( existing->m_iCostTo < from->m_iCostTo + iEdgeCost )
	{
		return false;
	}
	else if ( existing->m_iCostTo == from->m_iCostTo + iEdgeCost )
	{
		//	Path length as next tie breaker
		int iProposedPathTurns = from->m_iPathTurns + (from->m_iMovementRemaining == 0 ? 1 : 0);

		if ( existing->m_iPathTurns < iProposedPathTurns )
		{
			return false;
		}
		else if ( existing->m_iPathTurns == iProposedPathTurns )
		{
			return (existing->m_iMovementRemaining < iMovementRemaining ||
					(existing->m_iPathSeq != m_iSeq && existing->m_iMovementRemaining == iMovementRemaining) ||
					existing->m_parent == from);
		}
	}
	
	return true;
}

const CvPlot* CvPathGenerator::getTerminalPlot() const
{
	return m_pTerminalPlot;
}

//#define LIGHT_VALIDATION
#ifdef LIGHT_VALIDATION
static void ValidatePathNode(CvPathNode* node)
{
	bool bResult = true;

	if ( node->m_parent != NULL )
	{
		if ( node->m_prevSibling == NULL )
		{
			if ( node->m_parent->m_firstChild != node )
			{
				OutputDebugString("node->m_parent->m_firstChild != node\n");
				bResult = false;
			}
		}
		else
		{
			if ( node->m_prevSibling->m_nextSibling != node )
			{
				OutputDebugString("node->m_prevSibling->m_nextSibling != node\n");
				bResult = false;
			}

			if ( node->m_parent->m_firstChild == NULL || node->m_parent->m_firstChild == node )
			{
				OutputDebugString("node->m_parent->m_firstChild == NULL || node->m_parent->m_firstChild == node\n");
				bResult = false;
			}
		}

		if ( node->m_nextSibling != NULL )
		{
			if ( node->m_nextSibling->m_prevSibling != node )
			{
				OutputDebugString("node->m_nextSibling->m_prevSibling != node\n");
				bResult = false;
			}
		}

		if ( node->m_iPathTurns != node->m_parent->m_iPathTurns && node->m_parent->m_iMovementRemaining != 0 )
		{
			OutputDebugString("node->m_iPathTurns != node->m_parent->m_iPathTurns && node->m_parent->m_iMovementRemaining != 0\n");
			bResult = false;
		}
	}
	else
	{
		if ( node->m_prevSibling != NULL || node->m_nextSibling != NULL )
		{
			OutputDebugString("node->m_prevSibling != NULL || node->m_nextSibling != NULL\n");
			bResult = false;
		}
	}

	if ( node->m_firstChild != NULL )
	{
		if ( node->m_firstChild->m_parent != node )
		{
			OutputDebugString("node->m_firstChild->m_parent != node\n");
			bResult = false;
		}
		if ( node->m_firstChild->m_prevSibling != NULL )
		{
			OutputDebugString("node->m_firstChild->m_prevSibling != NULL\n");
			bResult = false;
		}
	}

	if ( !bResult )
	{
		OutputDebugString("Node validation failure\n");
	}
}
#endif

bool	CvPathGenerator::generatePath(const CvPlot* pFrom, const CvPlot* pTo, CvSelectionGroup* pGroup, int iFlags, int iMaxTurns, int iOptimizationLimit)
{
	CvPathNode*	root;
	bool bResult;
#ifdef LIGHT_VALIDATION
	bool bValidate = true;
#endif

	PROFILE_FUNC();

	PROFILE_BEGIN_CONDITIONAL("CvPathGenerator::generatePath.Success","CvPathGenerator::generatePath.Failure");

	//	Only consider flags that effect the calculated path
	iFlags &= SIGNIFICANT_PATHING_FLAGS;

	m_bFoundRoute = false;

	m_iSeq++;

	bool bRequiresWar;

	if ( !m_TerminusValidFunc(pGroup, pTo->getX_INLINE(), pTo->getY_INLINE(), iFlags, bRequiresWar) )
	{
		m_generatedPath.Set(NULL);
		bResult = false;
	}
	else
	{
		m_pTerminalPlot = pTo;

		if ( bRequiresWar )
		{
			iFlags |= MOVE_TERMINUS_DECLARES_WAR;
		}

		unsigned int	iGroupMembershipChecksum;
		bool bSameGroup = groupMatches(pGroup, iFlags, iGroupMembershipChecksum);

		if ( !bSameGroup )
		{
			CvSelectionGroup::setGroupToCacheFor(pGroup);
			CvPlot::NextCachePathEpoch();

			if ( !pGroup->AI_isControlled() )
			{
				m_useAIPathingAlways = getBugOptionBOOL("MainInterface__UseAIPathing", false);
			}
		}

		if ( TRACE_PATHING )
		{
			OutputDebugString(CvString::format("Generate path from (%d,%d) to (%d,%d)\n", pFrom->getX_INLINE(), pFrom->getY_INLINE(), pTo->getX_INLINE(), pTo->getY_INLINE()).c_str());
		}

		bool bExistingValid = false;

		//	Optimize the case where we'e just stepping along the previously calculated path (as continueMission() does)
		//  If it is a MPOPTION_SIMULTANEOUS_TURNS game, don't apply that optimization to avoid sync issues
		// TBOOSDEBUGNOTE : Changed !isHuman() to MPOPTION_SIMULTANEOUS_TURNS because it wasn't protecting against AI opponents withdrawing then re-evaluating Safety checks.
		if ( bSameGroup && (m_generatedPath.lastPlot() == pTo))
		{
			if ( m_generatedPath.containsNode(pFrom) )
			{
				bool bValid = true;

				m_generatedPath.trimBefore(pFrom);

				//	Validate we can still follow this path (visibility may have changed)
				for(CvPath::const_iterator itr = m_generatedPath.begin(); itr != m_generatedPath.end(); ++itr)
				{
					if ( itr.plot() != pFrom && !moveToValid(pGroup, itr.plot(), iFlags) )
					{
						bValid = false;
						break;
					}
				}

				if ( bValid )
				{
					bExistingValid = true;
				}
			}
		}

		if ( bExistingValid )
		{
			bResult = true;
		}
		else
		{
			m_generatedPath.Set(NULL);

			if ( !bSameGroup || pFrom != m_pFrom )
			{
				m_nodeAllocationPool->reset();
				m_plotInfo->reset();
				m_iTurn = GC.getGameINLINE().getGameTurn();

				m_pReplacedNonTerminalNode = NULL;
				m_pBestTerminalNode = NULL;
			}

			CvPathGeneratorPlotInfo* plotInfo = m_plotInfo->getPlotInfo(pFrom);

			if ( m_pBestTerminalNode == NULL || m_pBestTerminalNode->m_plot != pTo )
			{
				if ( plotInfo->pNode == NULL )
				{
					root = allocatePathNode();

					root->m_plot = (CvPlot*)pFrom;
					root->m_iCostFrom = m_HeuristicFunc(pGroup, pFrom->getX_INLINE(), pFrom->getY_INLINE(), pTo->getX_INLINE(), pTo->getY_INLINE(), root->m_iLowestPossibleCostFrom);
					root->m_iMovementRemaining = pGroup->movesLeft();
					plotInfo->pNode = root;
				}
				else
				{
					root = plotInfo->pNode;
					root->m_iCostFrom = m_HeuristicFunc(pGroup, pFrom->getX_INLINE(), pFrom->getY_INLINE(), pTo->getX_INLINE(), pTo->getY_INLINE(), root->m_iLowestPossibleCostFrom);
					root->m_iMovementRemaining = pGroup->movesLeft();

					FAssert(pFrom == root->m_plot);
				}
				//GC.getGameINLINE().logOOSSpecial(50, pGroup->getID(), root->m_iMovementRemaining);

				ValidatePlotInfo(plotInfo);

				VALIDATE_TREE(root, m_pReplacedNonTerminalNode, m_pBestTerminalNode);

				//GC.getGameINLINE().logOOSSpecial(51, m_pBestTerminalNode->m_plot->getX(), m_pBestTerminalNode->m_plot->getY());
				//GC.getGameINLINE().logOOSSpecial(52, pTo->getX(), pTo->getY());
				if ( m_pBestTerminalNode != NULL && m_pBestTerminalNode->m_plot != pTo )
				{
					if ( m_pBestTerminalNode->m_plot != pFrom )
					{
						if (m_pReplacedNonTerminalNode != NULL)
						{
							FAssert(m_pReplacedNonTerminalNode->m_plot == m_pBestTerminalNode->m_plot);
						}

						//	Put back a non-terminal variant of the old terminal node if we have one
						CvPathGeneratorPlotInfo* oldTerminalPlotInfo = m_plotInfo->getPlotInfo(m_pBestTerminalNode->m_plot);
						
						FAssert(oldTerminalPlotInfo->pNode == m_pBestTerminalNode);

						oldTerminalPlotInfo->pNode = m_pReplacedNonTerminalNode;

						if ( m_pBestTerminalNode->m_parent != NULL )
						{
							FAssert( m_pBestTerminalNode->m_firstChild == NULL );

							UnlinkNode(m_pBestTerminalNode);
						}
						
						//	Edges that are valid into a terminal plot may not be to a non-terminal (direct attacks)
						oldTerminalPlotInfo->m_iEdgesValidated = 0;

						CvPathNode* replacedTerminalNode = m_pReplacedNonTerminalNode;

						if ( m_pReplacedNonTerminalNode != NULL )
						{
							if ( m_pReplacedNonTerminalNode->m_parent != NULL )
							{
								m_pReplacedNonTerminalNode = NULL;

								LinkNode(replacedTerminalNode, replacedTerminalNode->m_parent);
							}
						}

						ValidatePlotInfo(oldTerminalPlotInfo);

						VALIDATE_TREE(root, m_pBestTerminalNode, replacedTerminalNode);

#ifdef LIGHT_VALIDATION
						if ( bValidate && replacedTerminalNode != NULL)
						{
							ValidatePathNode(replacedTerminalNode);
						}
#endif
					}

					m_pBestTerminalNode = NULL;
					m_pReplacedNonTerminalNode = NULL;
				}
				else if ( m_pBestTerminalNode == NULL )
				{
					m_pReplacedNonTerminalNode = NULL;
				}

				//GC.getGameINLINE().logOOSSpecial(53, m_pBestTerminalNode->m_plot->getX(), m_pBestTerminalNode->m_plot->getY());
				//GC.getGameINLINE().logOOSSpecial(54, pTo->getX(), pTo->getY());

				CvPathGeneratorPlotInfo* terminalPlotInfo = m_plotInfo->getPlotInfo(pTo);

				//GC.getGameINLINE().logOOSSpecial(55, pFrom->getX(), pFrom->getY());
				if ( terminalPlotInfo->pNode != NULL && pFrom != pTo && !terminalPlotInfo->pNode->m_bProcessedAsTerminus )
				{
					VALIDATE_TREE(root, NULL, terminalPlotInfo->pNode);

					//	Check its within the allowed range - INVALID CHECK AS IT STANDS
					//if ( terminalPlotInfo->pNode->m_iPathTurns > iMaxTurns )
					//{
					//	return false;
					//}

					//	If we already know a route to the terminal plot recalculate its final
					//	edge for the plot now being terminal and seed the best route with it
					m_pReplacedNonTerminalNode = terminalPlotInfo->pNode;

					CvPlot* pParentPlot = m_pReplacedNonTerminalNode->m_parent->m_plot;
					int		iMovementRemaining = m_pReplacedNonTerminalNode->m_parent->m_iMovementRemaining;
					int		iPathTurns = m_pReplacedNonTerminalNode->m_parent->m_iPathTurns;

					UnlinkNode(m_pReplacedNonTerminalNode);

					int iNodeCost;
					int	iEdgeCost = m_CostFunc( this,
												pGroup,
												pParentPlot->getX_INLINE(),
												pParentPlot->getY_INLINE(),
												pTo->getX_INLINE(),
												pTo->getY_INLINE(),
												iFlags,
												iMovementRemaining,
												iPathTurns,
												iNodeCost,
												true);
					
					m_pBestTerminalNode = allocatePathNode();

					m_pBestTerminalNode->m_iBestToEdgeCost = iEdgeCost;
					m_pBestTerminalNode->m_iCostTo = iEdgeCost + m_pReplacedNonTerminalNode->m_parent->m_iCostTo;
					m_pBestTerminalNode->m_iPathTurns = m_pReplacedNonTerminalNode->m_parent->m_iPathTurns + (m_pReplacedNonTerminalNode->m_parent->m_iMovementRemaining == 0 ? 1 : 0);
					m_pBestTerminalNode->m_plot = (CvPlot*)pTo;
					m_pBestTerminalNode->m_bProcessedAsTerminus = true;
					m_pBestTerminalNode->m_bIsKnownRoute = m_pReplacedNonTerminalNode->m_bIsKnownRoute;
					m_pBestTerminalNode->m_iMovementRemaining = iMovementRemaining;

					LinkNode(m_pBestTerminalNode, m_pReplacedNonTerminalNode->m_parent);

					terminalPlotInfo->pNode = m_pBestTerminalNode;
					m_iTerminalNodeCost = iNodeCost;

					ValidatePlotInfo(terminalPlotInfo);
					FAssert(m_pReplacedNonTerminalNode->m_plot == m_pBestTerminalNode->m_plot);

					VALIDATE_TREE(root, m_pReplacedNonTerminalNode, m_pBestTerminalNode);

					//DeleteChildTree(m_pReplacedNonTerminalNode,true);
					//m_pReplacedNonTerminalNode = NULL;
#ifdef LIGHT_VALIDATION
					if ( bValidate )
					{
						ValidatePathNode(m_pBestTerminalNode);
					}
#endif
				}

				//GC.getGameINLINE().logOOSSpecial(56, pFrom->getX(), pFrom->getY(), iFlags);
				//GC.getGameINLINE().logOOSSpecial(57, iGroupMembershipChecksum, m_iSeq);
				m_iFlags = iFlags;
				m_currentGroupMembershipChecksum = iGroupMembershipChecksum;
				m_pFrom = pFrom;

				root->m_iPathSeq = m_iSeq;

				//	Special-case pFrom == pTo
				if ( pFrom == pTo )
				{
					m_pBestTerminalNode = root;

					root->m_bProcessedAsTerminus = true;
					FAssert(m_pReplacedNonTerminalNode == NULL || m_pReplacedNonTerminalNode->m_plot == m_pBestTerminalNode->m_plot);
				}
				else
				{
					root->m_bProcessedAsTerminus = false;

					if ( m_pBestTerminalNode == NULL || !m_pBestTerminalNode->m_bIsKnownRoute )
					{
						MEMORY_TRACK_EXEMPT();

						priorityQueueEntry	entry;

						entry.node = root;
						entry.iQueuedCost = 0;

						m_priorityQueue.push(entry);
			#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
						root->m_bIsQueued = true;
			#endif
					}
					else if ( m_pBestTerminalNode->m_iMovementRemaining != 0 )
					{
						int iBestCostTo = MAX_INT;

#ifdef LIGHT_VALIDATION
						if ( bValidate )
						{
							OutputDebugString("Validate initial best path...\n");
							for(CvPathNode* node = m_pBestTerminalNode; node != NULL; node = node->m_parent)
							{
								OutputDebugString(CvString::format("Validate %08lx (%d,%d)\n", node, node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE()).c_str());
								ValidatePathNode(node);
							}
						}
#endif
						//	Re-evaluate which adjacent plot to come at this plot from if the known node is not
						//	turn ending
						for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pTo->getX_INLINE(), pTo->getY_INLINE(), (DirectionTypes)iI);
							if (pAdjacentPlot != NULL)
							{
								CvPathGeneratorPlotInfo*	pAdjacentPlotInfo = m_plotInfo->getPlotInfo(pAdjacentPlot, false);

								if ( pAdjacentPlotInfo != NULL &&
									 pAdjacentPlotInfo->pNode != NULL &&
									 !isDescendantOfReplacedNode(pAdjacentPlotInfo->pNode) &&
									 pAdjacentPlotInfo->pNode != m_pBestTerminalNode->m_parent &&
									 pAdjacentPlotInfo->pNode->m_bIsKnownRoute )
								{
									int iMovementRemaining = pAdjacentPlotInfo->pNode->m_iMovementRemaining;
									int	iPathTurns = pAdjacentPlotInfo->pNode->m_iPathTurns;
									int iNodeCost;
									int	iEdgeCost = m_CostFunc( this,
																pGroup,
																pAdjacentPlot->getX_INLINE(),
																pAdjacentPlot->getY_INLINE(),
																pTo->getX_INLINE(),
																pTo->getY_INLINE(),
																iFlags,
																iMovementRemaining,
																iPathTurns,
																iNodeCost,
																true);

									if ( pAdjacentPlotInfo->pNode->m_iCostTo + iEdgeCost < m_pBestTerminalNode->m_iCostTo )
									{
										m_pBestTerminalNode->m_iCostTo = pAdjacentPlotInfo->pNode->m_iCostTo + iEdgeCost;
										m_pBestTerminalNode->m_iMovementRemaining = iMovementRemaining;
										m_pBestTerminalNode->m_iPathTurns = pAdjacentPlotInfo->pNode->m_iPathTurns + (pAdjacentPlotInfo->pNode->m_iMovementRemaining == 0 ? 1 : 0);

#ifdef LIGHT_VALIDATION
										if (bValidate)
										{
											OutputDebugString("Validate path to new better parent...\n");
											for(CvPathNode* node = pAdjacentPlotInfo->pNode; node != NULL; node = node->m_parent)
											{
												OutputDebugString(CvString::format("Validate %08lx (%d,%d)\n", node, node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE()).c_str());
												ValidatePathNode(node);
											}
										}
#endif

										RelinkNode(m_pBestTerminalNode, pAdjacentPlotInfo->pNode);

#ifdef LIGHT_VALIDATION
										if ( bValidate )
										{
											OutputDebugString("Validate relinked best path...\n");
											for(CvPathNode* node = m_pBestTerminalNode; node != NULL; node = node->m_parent)
											{
												OutputDebugString(CvString::format("Validate %08lx (%d,%d)\n", node, node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE()).c_str());
												ValidatePathNode(node);
											}
										}
#endif
									}
								}
							}
						}
					}
				}

				bool bTurnEndValidCheckNeeded = m_TurnEndValidCheckNeeded(pGroup, iFlags);
				int iIterations = 0;
				int iMaxIterations = -1;

				//GC.getGameINLINE().logOOSSpecial(58, pTo->getX(), pTo->getY(), iFlags);
				//GC.getGameINLINE().logOOSSpecial(59, iGroupMembershipChecksum, m_iSeq);
				if ( iOptimizationLimit == -1 )
				{
					//	Set a default for optimization processing dependent on the step distance
					//	(accept less perfect paths as they get longer)
					int iStepDistance = stepDistance(pFrom->getX_INLINE(), pFrom->getY_INLINE(), pTo->getX_INLINE(), pTo->getY_INLINE());

					if ( iStepDistance > 8 )
					{
						iOptimizationLimit = std::max(0, MAX_DEFAULT_OPTIMIZATION_PERCENT - iStepDistance*OPTIMIZATION_PERCENT_LOSS_PER_DISTANCE);
					}
				}

				//while(m_priorityQueue->size() > 0)
				while(!m_priorityQueue.empty())
				{
					CvPathNode* node;

					m_nodesProcessed++;
					iIterations++;

					if ( iMaxIterations >= 0 )
					{
						if ( iIterations > iMaxIterations && m_pBestTerminalNode != NULL) 
						{
							if ( TRACE_PATHING )
							{
								OutputDebugString(CvString::format("Path optimization terminated after %d iterations (max %d based on optimization allowance percentage %d)\n", iIterations, iMaxIterations,iOptimizationLimit).c_str());
								OutputDebugString(CvString::format("Best path length is %d\n", m_pBestTerminalNode->m_iPathTurns).c_str());
							}
							break;
						}
					}
					else if ( iIterations > CEILING_ITERATIONS )
					{
						if ( TRACE_PATHING )
						{
							OutputDebugString(CvString::format("Path optimization terminated after %d iterations (ceiling reached)\n", iIterations).c_str());
						}
						break;
					}

					//	Dequeue this node now we are dealing with it
					{
						PROFILE("CvPathGenerator::generatePath.popNode");

						priorityQueueEntry entry = m_priorityQueue.top();
						m_priorityQueue.pop();
						node = entry.node;
		#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
						node->m_bIsQueued = false;
		#endif
						if ( TRACE_PATHING )
						{
							OutputDebugString(CvString::format("Dequeue (%d,%d): %d\n", node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE(), node->m_iCostTo).c_str());
						}

						//if ( node->m_iCostTo != entry.iQueuedCost )
						if ( node->m_iLowestDequeueCost < entry.iQueuedCost )
						{
							if ( TRACE_PATHING )
							{
								OutputDebugString("    ...discarded since already superseded\n");
							}

							continue;
						}

						node->m_iLowestDequeueCost = node->m_iCostTo;
						node->m_iRecalcThreshold = MAX_INT;
					}

					const CvPlot*				nodePlot = node->m_plot;
					//CvPathGeneratorPlotInfo*	nodePlotInfo = m_plotInfo->getPlotInfo(nodePlot);
					int							iPathTurns = node->m_iPathTurns + (node->m_iMovementRemaining == 0 ? 1 : 0);

					if(m_plotInfo->getPlotInfo(nodePlot)->pNode != node)
					{
						//	This queued node has already been superseded
						continue;
					}

					if ( node->m_iPathTurns <= iMaxTurns )
					{
						//PROFILE("CvPathGenerator::generatePath.processNode");

#ifdef LIGHT_VALIDATION
						if ( bValidate )
						{
							ValidatePathNode(node);
						}
#endif
						if ( m_pBestTerminalNode != NULL )
						{
							if ( node->m_iCostTo + node->m_iLowestPossibleCostFrom + m_iTerminalNodeCost >= m_pBestTerminalNode->m_iCostTo )
							{
								if ( TRACE_PATHING )
								{

									OutputDebugString(CvString::format("Reject because costTo(%d)+minCostFrom(%d)+finalNodeCost(%d) > existingPathCost(%d)\n",
													  node->m_iCostTo,
													  node->m_iLowestPossibleCostFrom,
													  m_iTerminalNodeCost,
													  m_pBestTerminalNode->m_iCostTo).c_str());
								}
								//	This branch cannot lead to a better solution than the one we already have
								continue;
							}
						}

						for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							VALIDATE_TREE(root, m_pReplacedNonTerminalNode, m_pBestTerminalNode);
							VALIDATE_TREE(root, NULL, node);

							CvPlot* pAdjacentPlot = plotDirection(nodePlot->getX_INLINE(), nodePlot->getY_INLINE(), (DirectionTypes)iI);
							if (pAdjacentPlot != NULL && pAdjacentPlot != pFrom)
							{
								CvPathGeneratorPlotInfo*	pAdjacentPlotInfo = m_plotInfo->getPlotInfo(pAdjacentPlot);
								bool isTerminus = (pAdjacentPlot == pTo);

								if ( isTerminus || !pAdjacentPlotInfo->bKnownInvalidNode )
								{
									PROFILE("CvPathGenerator::ProcessNode.NotKnownInvalid");

									int iMovementRemaining = node->m_iMovementRemaining;
									bool bValid;
									bool bUseExistingNode = false;

									m_nodesCosted++;

									FAssert( pAdjacentPlotInfo->pNode == NULL || pAdjacentPlotInfo->pNode->m_plot == pAdjacentPlot );

									if ( pAdjacentPlotInfo->pNode != NULL && pAdjacentPlotInfo->pNode->m_iCostTo <= node->m_iCostTo )
									{
										if ( TRACE_PATHING )
										{
											OutputDebugString(CvString::format("\tReject (%d,%d) - already as cheap as this parent\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE()).c_str());
										}
										continue;
									}

									if ( pAdjacentPlotInfo->pNode != NULL &&
										 (pAdjacentPlotInfo->pNode->m_iEdgesIncluded & (1<<iI)) != 0 &&
										 !isDescendantOfReplacedNode(pAdjacentPlotInfo->pNode) )
									{
										//	If the adjacent node already has a tree node associated with it and this edge
										//	has already been taken into account then either:
										//	1) This edge is the one that is to the lowest cost route to the adjacent node, in which case use it
										//	2) This edge was not the lowest cost route to the adjacent route in which case we don't need to
										//	   process it
										if ( pAdjacentPlotInfo->pNode->m_parent == node )
										{
											bValid = true;
											bUseExistingNode = true;
										}
										else
										{
											if ( TRACE_PATHING )
											{
												if ( pAdjacentPlotInfo->pNode->m_parent != NULL )
												{
													OutputDebugString(CvString::format("\tReject (%d,%d) - lower cost route (%d) known from (%d,%d) [%d]\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), pAdjacentPlotInfo->pNode->m_iCostTo, pAdjacentPlotInfo->pNode->m_parent->m_plot->getX_INLINE(), pAdjacentPlotInfo->pNode->m_parent->m_plot->getY_INLINE(), pAdjacentPlotInfo->pNode->m_parent->m_iCostTo).c_str());
												}
												else
												{
													FAssert(false);
												}
											}
											continue;
										}
									}
									else
									{
										if ( (pAdjacentPlotInfo->m_iEdgesValidated & (1<<iI)) != 0 )
										{
											bValid = true;
										}
										else
										{
											bValid = m_ValidFunc(pGroup,
																 nodePlot->getX_INLINE(),
																 nodePlot->getY_INLINE(),
																 pAdjacentPlot->getX_INLINE(),
																 pAdjacentPlot->getY_INLINE(),
																 iFlags,
																 isTerminus,
																 false,
																 iPathTurns,
																 pAdjacentPlotInfo->bKnownInvalidNode);

											if ( TRACE_PATHING )
											{
												if ( !bValid )
												{
													OutputDebugString(CvString::format("\tReject (%d,%d) - invalid node\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE()).c_str());
												}
											}
										}
									}

									if ( bValid )
									{
										CvPathNode* newNode;

										pAdjacentPlotInfo->m_iEdgesValidated |= (1<<iI);

										if ( bUseExistingNode )
										{
											PROFILE("CvPathGenerator::ProcessNode.UseExisting");

											newNode = pAdjacentPlotInfo->pNode;

											if ( m_pBestTerminalNode != NULL )
											{
												int iMinFinalCost = newNode->m_iCostTo + (isTerminus ? 0 : m_iTerminalNodeCost);

												if ( iMinFinalCost > m_pBestTerminalNode->m_iCostTo )
												{
													int iThreshold = iMinFinalCost - m_pBestTerminalNode->m_iCostTo;

													if ( node->m_iRecalcThreshold > iThreshold )
													{
														node->m_iRecalcThreshold = iThreshold;
													}

													//	This branch cannot lead to a better solution than the one we already have
													if ( TRACE_PATHING )
													{
														OutputDebugString(CvString::format("\tReject (%d,%d): min final (existing node) cost %d > %d\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), iMinFinalCost, m_pBestTerminalNode->m_iCostTo).c_str());
													}
													continue;
												}
											}

											newNode->m_iCostTo = newNode->m_iBestToEdgeCost + node->m_iCostTo;
										}
										else
										{
											PROFILE("CvPathGenerator::ProcessNode.MeasureCost");

											int iNodeCost;
											int	iEdgeCost = m_CostFunc( this,
																		pGroup,
																		nodePlot->getX_INLINE(),
																		nodePlot->getY_INLINE(),
																		pAdjacentPlot->getX_INLINE(),
																		pAdjacentPlot->getY_INLINE(),
																		iFlags,
																		iMovementRemaining,
																		iPathTurns,
																		iNodeCost,
																		(pAdjacentPlot == pTo));

											if ( bTurnEndValidCheckNeeded &&
												 iMovementRemaining == 0 &&
		 										 !m_ValidFunc(pGroup,
															 nodePlot->getX_INLINE(),
															 nodePlot->getY_INLINE(),
															 pAdjacentPlot->getX_INLINE(),
															 pAdjacentPlot->getY_INLINE(),
															 iFlags,
															 isTerminus,
															 true,
															 iPathTurns,
															 pAdjacentPlotInfo->bKnownInvalidNode))
											{
												//	Re-evaluation of edge, knowing we would end a turn there shows it
												//	to be invalid
												if ( TRACE_PATHING )
												{
													OutputDebugString(CvString::format("\tReject (%d,%d): invalid as turn end\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE()).c_str());
												}
												continue;
											}

											if ( m_pBestTerminalNode != NULL )
											{
												int iMinFinalCost = node->m_iCostTo + std::max(iEdgeCost, node->m_iLowestPossibleCostFrom) + (isTerminus ? 0 : m_iTerminalNodeCost);

												if ( iMinFinalCost > m_pBestTerminalNode->m_iCostTo )
												{
													int iThreshold = iMinFinalCost - m_pBestTerminalNode->m_iCostTo;

													if ( node->m_iRecalcThreshold > iThreshold )
													{
														node->m_iRecalcThreshold = iThreshold;
													}

													//	This branch cannot lead to a better solution than the one we already have
													if ( TRACE_PATHING )
													{
														OutputDebugString(CvString::format("\tReject (%d,%d): min final cost %d > %d\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), iMinFinalCost, m_pBestTerminalNode->m_iCostTo).c_str());
													}
													continue;
												}
											}

											VALIDATE_TREE(root, NULL, NULL);

											if ( pAdjacentPlotInfo->pNode == NULL )
											{
												//	Not yet visited - queue a new node for it
												newNode = allocatePathNode();

												VALIDATE_TREE(root, newNode, NULL);

												LinkNode(newNode, node);

												VALIDATE_TREE(root, NULL, newNode);
											}
											else
											{
												newNode = pAdjacentPlotInfo->pNode;

												//	Visited previously - is this route here superior to the one we already
												//	have
												if ( newNode->m_iCostTo > node->m_iCostTo + iEdgeCost )
												{
													//	For now just trim the old tree rooted here and recalculate it,
													//	but (TODO) adjust the existing tree nodes if end turn boundaries
													//	align
													newNode = pAdjacentPlotInfo->pNode;
												}
												else
												{
													//	Equal cost is considered in precisely one case - retracing the (best) steps
													//	taken by a previous path
													if ( !isBetterPath(newNode, node, iEdgeCost, iMovementRemaining) )
													{
														int iThreshold = node->m_iCostTo + iEdgeCost - newNode->m_iCostTo;

														if ( node->m_iRecalcThreshold > iThreshold )
														{
															node->m_iRecalcThreshold = iThreshold;
														}

														if ( TRACE_PATHING )
														{

															OutputDebugString(CvString::format("\tReject (%d,%d): %d > %d\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), node->m_iCostTo + iEdgeCost, newNode->m_iCostTo).c_str());
														}
														continue;
													}
												}

												FAssert(!newNode->m_bIsKnownRoute || node->m_iCostTo + iEdgeCost == newNode->m_iCostTo);
												RelinkNode(newNode, node);

												if ( (newNode->m_iMovementRemaining != iMovementRemaining) ||
													 isDescendantOfReplacedNode(newNode))
												{
													PROFILE("CvPathGenerator::ProcessNode.DeleteChildTree");

													//	Need to recalculate the descendent tree since turn ends won't occur
													//	in the same places
													DeleteChildTree(newNode, true);

													newNode->m_iPathSeq = -1;	//	Force reprocess
												}
												else
												{
													AdjustChildTreeCosts(newNode, node->m_iCostTo + iEdgeCost - newNode->m_iCostTo, false);
												}

												VALIDATE_TREE(root, NULL, newNode);
											}

											pAdjacentPlotInfo->pNode = newNode;

											newNode->m_iCostTo = node->m_iCostTo + iEdgeCost;
											newNode->m_iPathTurns = node->m_iPathTurns + (node->m_iMovementRemaining == 0 ? 1 : 0);
											newNode->m_iMovementRemaining = iMovementRemaining;
											newNode->m_plot = pAdjacentPlot;
											newNode->m_iBestToEdgeCost = iEdgeCost;
											if ( TRACE_PATHING )
											{
												OutputDebugString(CvString::format("Adjust costTo (%d,%d): %d\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), newNode->m_iCostTo).c_str());
											}
#ifdef LIGHT_VALIDATION
											if ( bValidate )
											{
												ValidatePathNode(newNode);
											}
#endif

											ValidatePlotInfo(pAdjacentPlotInfo);

											if ( isTerminus )
											{
												m_iTerminalNodeCost = iNodeCost;
											}

											VALIDATE_TREE(root, m_pReplacedNonTerminalNode, m_pBestTerminalNode);
										}

										newNode->m_iEdgesIncluded |= (1<<iI);
										newNode->m_iCostFrom = m_HeuristicFunc(pGroup, pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), pTo->getX_INLINE(), pTo->getY_INLINE(), newNode->m_iLowestPossibleCostFrom);

										//	If this reaches the destination then set current least cost info
										if ( isTerminus )
										{
											newNode->m_bProcessedAsTerminus = true;

											if ( iOptimizationLimit != -1 && iMaxIterations == -1 )
											{
												if ( iOptimizationLimit == 0 )
												{
													iMaxIterations = iIterations;
												}
												else
												{
													iMaxIterations = std::max(iIterations + MIN_OPTIMIZATION_ITERATIONS, ((100+iOptimizationLimit)*iIterations)/100);
												}

												if ( TRACE_PATHING )
												{
													OutputDebugString(CvString::format("Setting iteration limit of %d after finding terminus on iteration %d (optimization allowance percent %d)\n", iMaxIterations, iIterations, iOptimizationLimit).c_str());
												}
											}

											if ( m_pBestTerminalNode == NULL || isBetterPath(m_pBestTerminalNode, node, newNode->m_iCostTo - node->m_iCostTo, iMovementRemaining) )
											{
												if ( TRACE_PATHING )
												{

													OutputDebugString(CvString::format("New best cost to terminus @(%d,%d): %d\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), newNode->m_iCostTo).c_str());
												}
												m_pBestTerminalNode = newNode;
												FAssert(m_pReplacedNonTerminalNode == NULL || m_pReplacedNonTerminalNode->m_plot == m_pBestTerminalNode->m_plot);

												//	TODO - maybe back-propagate actual costs to neighbour heuristic costs at this point
											}
											else if ( TRACE_PATHING )
											{
												OutputDebugString(CvString::format("New route to terminus @(%d,%d): %d, greater than existing best cost %d\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), newNode->m_iCostTo, m_pBestTerminalNode->m_iCostTo).c_str());
											}
										}
										//	Queue up the node unless it's the terminus
										else
										{
											newNode->m_bProcessedAsTerminus = false;

											if ( newNode->m_iPathSeq != m_iSeq )
											{
												PROFILE("CvPathGenerator::generatePath.insertNode");

												if ( TRACE_PATHING )
												{
													OutputDebugString(CvString::format("\tQueue (%d,%d): %d\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), newNode->m_iCostTo).c_str());
												}
		#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
												newNode->m_bIsQueued = true;
		#endif
												newNode->m_iPathSeq = m_iSeq;

												{
													MEMORY_TRACK_EXEMPT();

													priorityQueueEntry	entry;

													entry.node = newNode;
													entry.iQueuedCost = newNode->m_iCostTo;

													m_priorityQueue.push(entry);
												}
											}
										}
									}
								}
								else
								{
									if ( TRACE_PATHING )
									{
										OutputDebugString(CvString::format("\tReject (%d,%d): previously found to be invalid\n", pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE()).c_str());
									}
								}
							}
						}
					}
				}

				//GC.getGameINLINE().logOOSSpecial(60, m_pBestTerminalNode->m_plot->getX(), m_pBestTerminalNode->m_plot->getY());
				VALIDATE_TREE(root, m_pReplacedNonTerminalNode, m_pBestTerminalNode);
			}
			else
			{
				//GC.getGameINLINE().logOOSSpecial(61, m_pBestTerminalNode->m_plot->getX(), m_pBestTerminalNode->m_plot->getY());
				root = plotInfo->pNode;

				FAssert(pFrom == root->m_plot);

				VALIDATE_TREE(root, m_pReplacedNonTerminalNode, m_pBestTerminalNode);
			}

			//GC.getGameINLINE().logOOSSpecial(62, pTo->getX(), pTo->getY(), iMaxTurns);
			//	Have to check max turns here since even if we know a route it might be too far if it is known due to
			//	caching of previous paths rather than freshly calculated
			if ( m_pBestTerminalNode != NULL && m_pBestTerminalNode->m_iPathTurns <= iMaxTurns )
			{
				PROFILE("CvPathGenerator::generatePath.finalizePath");

				//	Relink the optimal path so that the best branch is the first child at each step
				CvPathNode*	node;
				CvPathNode*	descendantNode = NULL;

				FAssert( m_pBestTerminalNode->m_bProcessedAsTerminus );

#ifdef LIGHT_VALIDATION
				for(node = m_pBestTerminalNode; node != NULL; node = node->m_parent)
				{
					if ( bValidate )
					{
						OutputDebugString(CvString::format("Validate %08lx (%d,%d)\n", node, node->m_plot->getX_INLINE(), node->m_plot->getY_INLINE()).c_str());
						ValidatePathNode(node);
					}
				}
#endif
				for(node = m_pBestTerminalNode; node != NULL; node = node->m_parent)
				{
#ifdef LIGHT_VALIDATION
					if ( bValidate )
					{
						ValidatePathNode(node);
					}
#endif
					if ( descendantNode != NULL && descendantNode != node->m_firstChild )
					{
						FAssert(descendantNode->m_prevSibling != NULL);

						UnlinkNode(descendantNode);
						LinkNode(descendantNode, node);
					}

					node->m_bIsKnownRoute = true;
					descendantNode = node;
					//GC.getGameINLINE().logOOSSpecial(63, node->m_plot->getX(), node->m_plot->getY(), node->m_iPathTurns);
#ifdef LIGHT_VALIDATION
					if ( bValidate )
					{
						ValidatePathNode(node);
					}
#endif
				}

				FAssert(descendantNode != NULL);
				FAssert(descendantNode == root);

				m_generatedPath.Set(descendantNode);
				//GC.getGameINLINE().logOOSSpecial(64, descendantNode->m_plot->getX(), descendantNode->m_plot->getY(), descendantNode->m_iPathTurns);

				VALIDATE_TREE(root, m_pReplacedNonTerminalNode, m_pBestTerminalNode);
				bResult = true;
			}
			else
			{
				bResult = false;
			}
		}
	}

	PROFILE_END_CONDITIONAL(bResult);
	//GC.getGameINLINE().logOOSSpecial(65, (int)bResult);

	return bResult;
}

bool CvPathGenerator::CvPathNodeComparer::operator() (const priorityQueueEntry& lhs, const priorityQueueEntry& rhs) const
{
	return lhs.node->m_iCostFrom + lhs.iQueuedCost > rhs.node->m_iCostFrom + rhs.iQueuedCost;
}

CvPath&	CvPathGenerator::getLastPath()
{
	return m_generatedPath;
}

bool CvPathGenerator::generatePathForHypotheticalUnit(const CvPlot* pFrom, const CvPlot* pTo, PlayerTypes ePlayer, UnitTypes eUnit, int iFlags, int iMaxTurns)
{
	PROFILE_FUNC();

	bool bResult;
	CvUnit*	pTempUnit = GET_PLAYER(ePlayer).getTempUnit(eUnit, pFrom->getX_INLINE(), pFrom->getY_INLINE());

	pTempUnit->finishMoves();

	//	Force flush of pathing cache since although we are using the same (pseudo) group
	//	all the time its starting plot varies
	m_currentGroupMembershipChecksum = 0;

	//	FUTURE - might want to move the no-land-units-across-water flags up to the callers once they
	//	become aware of it.  For now it's here to prevent paths Python generates to build roads etc
	//	crossing water
	bResult = generatePath(pFrom, pTo, pTempUnit->getGroup(), iFlags | MOVE_NO_LAND_UNITS_ACROSS_WATER, iMaxTurns);

	GET_PLAYER(ePlayer).releaseTempUnit();

	return bResult;
}
//#endif

typedef struct
{
	int		iStartX;
	int		iStartY;
	int		iEndX;
	int		iEndY;
	int		iPathLen;
	int		iEndCost;
	bool	bResult;
} attemptedPathRecord;

void CvPathGenerator::SelfTest()
{
	PROFILE_FUNC();

	CvMap&		map = GC.getMapINLINE();
	int			iMapSize = map.numPlotsINLINE();
	CvRandom	rand;
	int			iPathsSuccessful = 0;
#define NUM_PATHS		10000
#define	MAX_DISTANCE	30
#define	NUM_PATHS_PER_START_POINT	50
	char buffer[100];
	attemptedPathRecord	attemptTrace[NUM_PATHS_PER_START_POINT];

	rand.init(0);

	int	iPathsRemaining = NUM_PATHS;
	//	Pick an arbitrary unit with more than 1 movement point
	UnitTypes eLandUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_WORKER");

	EnableMaxPerformance(true);

	while( iPathsRemaining > 0 )
	{
		CvPlot*	pStartPlot = map.plotByIndexINLINE(rand.getInt()%iMapSize);
		//	For now just land paths considered
		if ( !pStartPlot->isWater() )
		{
			CvUnit*	pTempUnit = GET_PLAYER((PlayerTypes)0).getTempUnit(eLandUnit, pStartPlot->getX_INLINE(), pStartPlot->getY_INLINE());

			pTempUnit->finishMoves();

			//	Force flush of pathing cache since although we are using the same (pseudo) group
			//	all the time its starting plot varies
			reset();

			for(int iI = 0; iI < NUM_PATHS_PER_START_POINT; iI++)
			{
				CvPlot*	pEndPlot = map.plotINLINE((pStartPlot->getX_INLINE() + rand.getInt()%(MAX_DISTANCE*2) + map.getGridWidthINLINE() - MAX_DISTANCE)%map.getGridWidthINLINE(),(std::max(0,(int)(pStartPlot->getY_INLINE() - MAX_DISTANCE + rand.getInt()%(MAX_DISTANCE*2))))%map.getGridHeightINLINE());

				attemptTrace[iI].iStartX = pStartPlot->getX_INLINE();
				attemptTrace[iI].iStartY = pStartPlot->getY_INLINE();
				attemptTrace[iI].iEndX = pEndPlot->getX_INLINE();
				attemptTrace[iI].iEndY = pEndPlot->getY_INLINE();

				//	For now just land paths considered
				if ( !pEndPlot->isWater() )
				{
					iPathsRemaining--;

#ifdef _DEBUG
					sprintf(buffer, "Attempt to path from (%d,%d) to (%d,%d)\r\n", pStartPlot->getX_INLINE(), pStartPlot->getY_INLINE(), pEndPlot->getX_INLINE(), pEndPlot->getY_INLINE());
					OutputDebugString(buffer);

					if ( 113 == attemptTrace[iI].iStartX && 21 == attemptTrace[iI].iStartY && 133 == attemptTrace[iI].iEndX && 9 == attemptTrace[iI].iEndY)
					{
						g_tracePathing = true;
					}
#endif
					attemptTrace[iI].bResult = generatePath(pStartPlot, pEndPlot, pTempUnit->getGroup(), 0, MAX_INT);
#ifdef _DEBUG
					g_tracePathing = false;
#endif

					if ( attemptTrace[iI].bResult )
					{
#ifdef _DEBUG
						OutputDebugString("    successful\r\n");
#endif
						iPathsSuccessful++;

						CvPath&	path = getLastPath();

						attemptTrace[iI].iPathLen = path.length();
						attemptTrace[iI].iEndCost = path.cost();
					}
#ifdef _DEBUG
					else
					{
						OutputDebugString("    failed\r\n");
					}
#endif
				}
			}

			GET_PLAYER((PlayerTypes)0).releaseTempUnit();
		}
	}

	EnableMaxPerformance(false);

	sprintf(buffer,"%d paths out of %d successful\r\n", iPathsSuccessful, NUM_PATHS);
	OutputDebugString(buffer);

	PROFILE_SET_COUNT(CvPathGeneratorNodesProcessed, m_nodesProcessed);
	PROFILE_SET_COUNT(CvPathGeneratorNodesCosted, m_nodesCosted);
}