## Sid Meier's Civilization 4
## Copyright Firaxis Games 2006
##
## CvEventManager
## This class is passed an argsList from CvAppInterface.onEvent
## The argsList can contain anything from mouse location to key info
## The EVENTLIST that are being notified can be found


from CvPythonExtensions import *
import CvUtil
import CvScreensInterface
import Popup as PyPopup
import CvAdvisorUtils
import DebugUtils

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

class CvEventManager:
	def __init__(self):
		self.bNotReady = True
		self.bNetworkMP = False
		self.bCtrl = False
		self.bShift = False
		self.bAlt = False
		self.bAllowCheats = False
		self.aWonderTuple = [[],[],[],[],[]]

		# OnEvent Enums
		# EventLButtonDown = 1
		# EventLcButtonDblClick = 2
		# EventRButtonDown = 3
		# EventBack = 4
		# EventForward = 5
		self.EventKeyDown = 6
		# EventKeyUp = 7

		## EVENT MAP
		self.EventHandlerMap = {
			'mouseEvent'				: self.onMouseEvent,
			'kbdEvent'					: self.onKbdEvent,
			'ModNetMessage'				: self.onModNetMessage,
			'Init'						: self.onInit,
#			'UnInit'					: self.onUnInit,
#			'Update'					: self.onUpdate,
#			'OnSave'					: self.onSaveGame,
#			'OnPreSave'					: self.onPreSave,
			'OnLoad'					: self.onLoadGame,
			'GameStart'					: self.onGameStart,
#			'GameEnd'					: self.onGameEnd,
#			'plotRevealed'				: self.onPlotRevealed,
#			'plotFeatureRemoved'		: self.onPlotFeatureRemoved,
#			'plotPicked'				: self.onPlotPicked,
			'nukeExplosion'				: self.onNukeExplosion,
#			'gotoPlotSet'				: self.onGotoPlotSet,
			'BeginGameTurn'				: self.onBeginGameTurn,
#			'EndGameTurn'				: self.onEndGameTurn,
			'BeginPlayerTurn'			: self.onBeginPlayerTurn,
			'EndPlayerTurn'				: self.onEndPlayerTurn,
#			'endTurnReady'				: self.onEndTurnReady,
			'combatResult'				: self.onCombatResult,
			'combatLogCalc'				: self.onCombatLogCalc,
			'combatLogHit'				: self.onCombatLogHit,
			'improvementBuilt'			: self.onImprovementBuilt,
#			'improvementDestroyed'		: self.onImprovementDestroyed,
#			'routeBuilt'				: self.onRouteBuilt,
#			'firstContact'				: self.onFirstContact,
			'cityBuilt'					: self.onCityBuilt,
			'cityAcquired' 				: self.onCityAcquired,
			'cityAcquiredAndKept'		: self.onCityAcquiredAndKept,
			'cityRazed'					: self.onCityRazed,
			'cityLost'					: self.onCityLost,
#			'cultureExpansion'			: self.onCultureExpansion,
#			'cityGrowth'				: self.onCityGrowth,
			'cityDoTurn'				: self.onCityDoTurn,
#			'cityBuildingUnit'			: self.onCityBuildingUnit,
#			'cityBuildingBuilding'		: self.onCityBuildingBuilding,
#			'cityBuildingProcess'		: self.onCityBuildingProcess,
			'cityRename'				: self.onCityRename,
#			'cityHurry'					: self.onCityHurry,
#			'selectionGroupPushMission'	: self.onSelectionGroupPushMission,
#			'unitMove'					: self.onUnitMove,
#			'unitSetXY'					: self.onUnitSetXY,
			'unitCreated'				: self.onUnitCreated,
			'unitBuilt'					: self.onUnitBuilt,
			'unitKilled'				: self.onUnitKilled,
#			'unitLost'					: self.onUnitLost,
			'unitPromoted'				: self.onUnitPromoted,
#			'unitSelected'				: self.onUnitSelected,
			'UnitRename'				: self.onUnitRename,
#			'unitPillage'				: self.onUnitPillage,
			'unitSpreadReligionAttempt'	: self.onUnitSpreadReligionAttempt,
#			'unitGifted'				: self.onUnitGifted,
#			'unitBuildImprovement'		: self.onUnitBuildImprovement,
#			'goodyReceived'				: self.onGoodyReceived,
			'greatPersonBorn'			: self.onGreatPersonBorn,
			'buildingBuilt'				: self.onBuildingBuilt,
			'projectBuilt'				: self.onProjectBuilt,
			'techAcquired'				: self.onTechAcquired,
#			'techSelected'				: self.onTechSelected,
			'religionFounded'			: self.onReligionFounded,
#			'religionSpread'			: self.onReligionSpread,
#			'religionRemove'			: self.onReligionRemove,
#			'corporationFounded'		: self.onCorporationFounded,
#			'corporationSpread'			: self.onCorporationSpread,
#			'corporationRemove'			: self.onCorporationRemove,
#			'goldenAge'					: self.onGoldenAge,
#			'endGoldenAge'				: self.onEndGoldenAge,
#			'chat' 						: self.onChat,
			'victory'					: self.onVictory,
			'vassalState'				: self.onVassalState,
			'changeWar'					: self.onChangeWar,
#			'setPlayerAlive'			: self.onSetPlayerAlive,
#			'playerChangeStateReligion'	: self.onPlayerChangeStateReligion,
#			'playerGoldTrade'			: self.onPlayerGoldTrade,
			'windowActivation'			: self.onWindowActivation,
			'gameUpdate'				: self.onGameUpdate
		}
		################## Events List ###############################
		# Dictionary of Events, indexed by EventID (also used at popup context id)
		#	entries have name, beginFunction, applyFunction [, randomization weight...]
		#
		# Enums values less than 1000 are reserved for popup events
		# Enum values greater than 5049 are reserved for CvUtil.getNewEventID().
		#
		self.OverrideEventApply = {}
		import CvMainInterface
		debugUtils = DebugUtils.debugUtils
		self.Events = {
			5	 : ('EffectViewer', debugUtils.applyEffectViewer, debugUtils.initEffectViewer),
			4999 : ('CityTabOptions', CvMainInterface.applyCityTabOptions, None),
			5000 : ('EditCityName', self.__eventEditCityNameApply, self.__eventEditCityNameBegin),
			5001 : ('EditCity', self.__eventEditCityApply, self.__eventEditCityBegin),
			5002 : ('PlaceObject', debugUtils.applyUnitPicker, debugUtils.initUnitPicker),
			5003 : ('AwardTechsAndGold', debugUtils.applyTechCheat, debugUtils.initTechsCheat),
			5006 : ('EditUnitName', self.__eventEditUnitNameApply, self.__eventEditUnitNameBegin),
			5008 : ('WBAllPlotsPopup', self.__eventWBAllPlotsPopupApply, self.__eventWBAllPlotsPopupBegin),
			5009 : ('WBLandmarkPopup', self.__eventWBLandmarkPopupApply, self.__eventWBLandmarkPopupBegin),
			5010 : ('WBScriptPopup', self.__eventWBScriptPopupApply, self.__eventWBScriptPopupBegin),
			5011 : ('WBStartYearPopup', self.__eventWBStartYearPopupApply, self.__eventWBStartYearPopupBegin),
			5012 : ('ShowWonder', debugUtils.applyWonderMovie, debugUtils.initWonderMovie),
		}
	###****************###
	### EVENT STARTERS ###
	''' Overwritten by the one in BugEventManager.py
	def handleEvent(self, argsList):
		# extract the last 6 args in the list, the first arg has already been consumed
		tag = argsList[0]				# event type string
		idx = len(argsList)-6
		self.bDbg, bDummy, self.bAlt, self.bCtrl, self.bShift, self.bAllowCheats = argsList[idx:]
		ret = 0
		if self.EventHandlerMap.has_key(tag):
			fxn = self.EventHandlerMap[tag]
			ret = fxn(argsList[1:idx])
		return ret
	'''

	###*************###
	### EVENT APPLY ###
	def beginEvent(self, iD, argsList = -1):
		entry = self.Events[iD]
		if entry and entry[2]:
			if DebugUtils.bDebugMode:
				print "Begin event " + entry[0]
			return entry[2](argsList)
		elif DebugUtils.bDebugMode:
			print "EventBegin - Unknown event ID " + str(iD)

	def applyEvent(self, argsList):
		iD, iPlayer, netUserData, popupReturn = argsList
		entry = self.Events[iD]

		if entry and entry[1]:
			if DebugUtils.bDebugMode:
				print "Apply event " + entry[0]
			if iD in self.OverrideEventApply:
				return self.OverrideEventApply[iD](iPlayer, netUserData, popupReturn)

			return entry[1](iPlayer, netUserData, popupReturn)
		elif DebugUtils.bDebugMode:
			print "EventApply - Unknown event ID " + str(iD)

	###***********###
	### ON EVENTS ###

	def onInit(self, argsList):
		print "CvEventManager.onInit - initialize python"
		import SystemPaths
		dir = SystemPaths.init()
		import ScreenResolution
		ScreenResolution.init(dir)
		print "CvEventManager.onInit - END"

	#def onUnInit(self, argsList): return


	def onWindowActivation(self, argsList):
		# Alt+TAB
		bActive = argsList[0]
		if bActive:
			if self.bNotReady:
				CvScreensInterface.lateInit()
				# Cache game constants.
				self.MAX_PLAYERS	= GC.getMAX_PLAYERS()
				self.MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
				self.iWAR_PRIZES	= GC.getDefineINT("WAR_PRIZES")
				self.iArcologyCityID = -1
				self.iOldCityID = -1
				self.mapBuildingType = {
					"ZIZKOV"				: GC.getInfoTypeForString("BUILDING_ZIZKOV"),
					"HELSINKI"				: GC.getInfoTypeForString("BUILDING_HELSINKI"),
					"TAIPEI_101"			: GC.getInfoTypeForString('BUILDING_TAIPEI_101'),
					"LOTUS_TEMPLE"			: GC.getInfoTypeForString('BUILDING_LOTUS_TEMPLE'),
					"DJENNE"				: GC.getInfoTypeForString("BUILDING_DJENNE"),
					"NANITE_DEFUSER"		: GC.getInfoTypeForString("BUILDING_NANITE_DEFUSER"),
					"MARCO_POLO"			: GC.getInfoTypeForString('BUILDING_MARCO_POLO'),
					"APPIAN_WAY"			: GC.getInfoTypeForString('BUILDING_APPIAN_WAY'),
					"GOLDEN_SPIKE"			: GC.getInfoTypeForString('BUILDING_GOLDEN_SPIKE'),
					"FIELD_GOLD"			: GC.getInfoTypeForString('BUILDING_FIELD_GOLD'),
					"MACHU_PICCHU"			: GC.getInfoTypeForString("BUILDING_MACHU_PICCHU"),
					"MAGINOTLINE"			: GC.getInfoTypeForString('BUILDING_MAGINOTLINE'),
					"ROUTE_66"				: GC.getInfoTypeForString('BUILDING_ROUTE_66'),
					"SILK_ROAD"				: GC.getInfoTypeForString('BUILDING_SILK_ROAD'),
					"CLEOPATRA_NEEDLE"		: GC.getInfoTypeForString("BUILDING_CLEOPATRA_NEEDLE"),
					"FA_MEN_SI"				: GC.getInfoTypeForString("BUILDING_FA_MEN_SI"),
					"LASCAUX"				: GC.getInfoTypeForString("BUILDING_LASCAUX"),
					"ARCOLOGY"				: GC.getInfoTypeForString("BUILDING_ARCOLOGY"),
					"ARCOLOGY_SHIELDING"	: GC.getInfoTypeForString("BUILDING_ARCOLOGY_SHIELDING"),
					"ADVANCED_SHIELDING"	: GC.getInfoTypeForString("BUILDING_ADVANCED_SHIELDING"),
					"GREAT_ZIMBABWE"		: GC.getInfoTypeForString("BUILDING_GREAT_ZIMBABWE"),
					"CRUSADE"				: GC.getInfoTypeForString("BUILDING_CRUSADE"),
					"KENTUCKY_DERBY"		: GC.getInfoTypeForString("BUILDING_KENTUCKY_DERBY"),
					"ALAMO"					: GC.getInfoTypeForString("BUILDING_ALAMO"),
					"WORLD_BANK"			: GC.getInfoTypeForString("BUILDING_WORLD_BANK"),
					"CYRUS_CYLINDER"		: GC.getInfoTypeForString("BUILDING_CYRUS_CYLINDER"),
					"CYRUS_TOMB"			: GC.getInfoTypeForString("BUILDING_CYRUS_TOMB"),
					"WEMBLEY"				: GC.getInfoTypeForString("BUILDING_WEMBLEY"),
					"PERGAMON"				: GC.getInfoTypeForString("BUILDING_PERGAMON"),
					"TSUKIJI"				: GC.getInfoTypeForString("BUILDING_TSUKIJI"),
					"BIODOME"				: GC.getInfoTypeForString("BUILDING_BIODOME"),
					"NAZCA_LINES"			: GC.getInfoTypeForString("BUILDING_NAZCA_LINES"),
					"THE_MOTHERLAND_CALLS"	: GC.getInfoTypeForString("BUILDING_THE_MOTHERLAND_CALLS"),
					"ISHTAR"				: GC.getInfoTypeForString('BUILDING_ISHTAR'),
					"GREAT_JAGUAR_TEMPLE"	: GC.getInfoTypeForString('BUILDING_GREAT_JAGUAR_TEMPLE'),
					"GREAT_BATH"			: GC.getInfoTypeForString('BUILDING_GREAT_BATH'),
					"TOPKAPI_PALACE"		: GC.getInfoTypeForString("BUILDING_TOPKAPI_PALACE")
				}
				self.mapUnitType = {
					"MONK"		: GC.getInfoTypeForString("UNIT_MONK"),
					"FANATIC"	: GC.getInfoTypeForString("UNIT_FANATIC")
				}
				self.mapImpType = {
					"IMPROVEMENT_TREE_NURSERY"	: GC.getInfoTypeForString('IMPROVEMENT_TREE_NURSERY'),
					"IMPROVEMENT_YOUNG_FOREST"	: GC.getInfoTypeForString('IMPROVEMENT_YOUNG_FOREST'),
					"IMPROVEMENT_PLANT_FOREST"	: GC.getInfoTypeForString('IMPROVEMENT_PLANT_FOREST'),
					"IMPROVEMENT_PLANT_BAMBOO"	: GC.getInfoTypeForString('IMPROVEMENT_PLANT_BAMBOO'),
					"IMPROVEMENT_PLANT_SAVANNA"	: GC.getInfoTypeForString('IMPROVEMENT_PLANT_SAVANNA'),
					"IMPROVEMENT_FARM"			: GC.getInfoTypeForString('IMPROVEMENT_FARM')
				}
				self.mapPromoType = {
					"PROMOTION_LIVE1"				: GC.getInfoTypeForString('PROMOTION_LIVE1'),
					"PROMOTION_LIVE2"				: GC.getInfoTypeForString('PROMOTION_LIVE2'),
					"PROMOTION_HUNTER1"				: GC.getInfoTypeForString('PROMOTION_HUNTER1'),
					"PROMOTION_HUNTER2"				: GC.getInfoTypeForString('PROMOTION_HUNTER2'),
					"PROMOTION_HUNTER3"				: GC.getInfoTypeForString('PROMOTION_HUNTER3'),
					"PROMOTION_HUNTER_GREAT"		: GC.getInfoTypeForString('PROMOTION_HUNTER_GREAT'),
					"PROMOTION_SEA_HUNTER1"			: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER1'),
					"PROMOTION_SEA_HUNTER2"			: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER2'),
					"PROMOTION_SEA_HUNTER3"			: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER3'),
					"PROMOTION_SEA_HUNTER_GREAT"	: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER_GREAT'),
					"PROMOTION_SNEAK"				: GC.getInfoTypeForString('PROMOTION_SNEAK'),
					"PROMOTION_MARAUDER"			: GC.getInfoTypeForString('PROMOTION_MARAUDER'),
					"PROMOTION_INDUSTRYESPIONAGE"	: GC.getInfoTypeForString('PROMOTION_INDUSTRYESPIONAGE')
				}
				self.mapDomain = {
					"DOMAIN_LAND"		: GC.getInfoTypeForString('DOMAIN_LAND'),
					"DOMAIN_SEA"		: GC.getInfoTypeForString('DOMAIN_SEA')
				}
				# onUnitBuilt
				self.mapSettlerPop = {
					GC.getInfoTypeForString("UNIT_COLONIST")	: 1,
					GC.getInfoTypeForString("UNIT_PIONEER")		: 2,
					GC.getInfoTypeForString("UNIT_AIRSETTLER")	: 4
				}
				self.PROMOTION_SENSORS	= GC.getInfoTypeForString('PROMOTION_SENSORS')
				self.UNIT_IMMIGRANT		= GC.getInfoTypeForString("UNIT_IMMIGRANT")
				self.TECH_SMART_DUST	= GC.getInfoTypeForString("TECH_SMART_DUST")
				self.CIVICOPTION_POWER	= GC.getInfoTypeForString('CIVICOPTION_POWER')
				self.CIVIC_TECHNOCRACY	= GC.getInfoTypeForString('CIVIC_TECHNOCRACY')
				# onCombatResult
				self.UNITCOMBAT_EXPLORER = GC.getInfoTypeForString('UNITCOMBAT_EXPLORER')
				# onCityBuilt
				self.PROMO_GUARDIAN_TRIBAL	= GC.getInfoTypeForString("PROMOTION_GUARDIAN_TRIBAL")
				# onTechAcquired
				self.TECH_GATHERING = GC.getInfoTypeForString("TECH_GATHERING")
				# Beastmaster
				self.UNIT_BEASTMASTER			= GC.getInfoTypeForString("UNIT_BEASTMASTER")
				self.UNIT_FEMALE_BEASTMASTER	= GC.getInfoTypeForString("UNIT_FEMALE_BEASTMASTER")
				# Biodome
				self.aBiodomeList = aList = []
				for iUnit in xrange(GC.getNumUnitInfos()):
					if GC.getUnitInfo(iUnit).getType().find("UNIT_SUBDUED") > -1:
						aList.append(iUnit)
				# Militia
				self.CIVIC_CONSCRIPTION = GC.getInfoTypeForString("CIVIC_CONSCRIPTION1")
				iMedieval = GC.getInfoTypeForString('UNIT_MILITIA_MEDIEVAL')
				iModern = GC.getInfoTypeForString('UNIT_MILITIA_MODERN')
				self.mapMilitiaByEra = {
					GC.getInfoTypeForString('ERA_CLASSICAL')	: iMedieval,
					GC.getInfoTypeForString('ERA_MEDIEVAL')		: iMedieval,
					GC.getInfoTypeForString('ERA_RENAISSANCE')	: GC.getInfoTypeForString('UNIT_MILITIA_RENAISSANCE'),
					GC.getInfoTypeForString('ERA_INDUSTRIAL')	: GC.getInfoTypeForString('UNIT_MILITIA_INDUSTRIAL'),
					GC.getInfoTypeForString('ERA_MODERN')		: iModern,
					GC.getInfoTypeForString('ERA_INFORMATION')	: iModern
				}
				# Only needs to be done once.
				self.bNotReady = False


	def onMouseEvent(self, argsList):
		'mouse handler - returns 1 if the event was consumed'
		eventType, mx, my, px, py, interfaceConsumed, screens = argsList

		if not interfaceConsumed and not self.bNetworkMP and (self.bAllowCheats or DebugUtils.bDebugMode):
			if eventType == 1 and px != -1 and py != -1 and self.bCtrl:
				if self.bAlt and GC.getMap().plot(px, py).isCity():
					# Launch Edit City Event
					self.beginEvent(5001, (px,py))
					return 1
				elif self.bShift:
					# Launch Place Object Event
					self.beginEvent(5002, (px, py))
					return 1

		if eventType == 4:
			CvScreensInterface.handleBack(screens)
		elif eventType == 5:
			CvScreensInterface.handleForward(screens)
		return 0


	def onKbdEvent(self, argsList):
		'keypress handler - return 1 if the event was consumed'
		eventType, key, mx, my, px, py = argsList
		bAlt = self.bAlt
		bCtrl = self.bCtrl
		bShift = self.bShift

		# Screen specific input handlers
		iCode = eventType + 10
		if iCode in (16, 17):
			iCode = CvScreensInterface.handleInput([iCode, key, 0, 0, CvScreensInterface.g_iScreenActive, "", 0, 0, 0, px, py, 35, 0, 0, 0])
			if iCode:
				return 1

		iModifiers = bAlt + bCtrl + bShift

		if eventType == 6: # Key down

			if iModifiers == 1:
				if bCtrl:
					if key == InputTypes.KB_F1:
						CvScreensInterface.showForgetfulScreen()
						return 1
					if key == InputTypes.KB_F2:
						import GameFontScreen
						GameFontScreen.GameFontScreen()
						return 1
					if key == InputTypes.KB_F3:
						import TimeKeeper
						TimeKeeper.TimeKeeper()
						return 1

				if DebugUtils.bDebugMode:
					if bCtrl:
						if key == InputTypes.KB_F4:
							CvScreensInterface.replayScreen.showScreen(False)
							return 1
						if key == InputTypes.KB_F5:
							CvScreensInterface.showDebugInfoScreen()
							return 1
						if key == InputTypes.KB_F6:
							CvScreensInterface.showDanQuayleScreen(())
							return 1
						if key == InputTypes.KB_F7:
							CvScreensInterface.showUnVictoryScreen(())
							return 1

					elif bShift:
						if key == InputTypes.KB_T:
							self.beginEvent(5003)
							return 1
						elif key == InputTypes.KB_W:
							self.beginEvent(5012)
							return 1
						elif key == InputTypes.KB_Z:
							CyInterface().addImmediateMessage("Dll Debug Mode: %s" %(not GAME.isDebugMode()), "AS2D_GOODY_MAP")
							GAME.toggleDebugMode()
							return 1
						elif key == InputTypes.KB_E:
							self.beginEvent(5, (px, py))
							return 1

			elif iModifiers == 2:

				if DebugUtils.bDebugMode:

					if bShift and bCtrl:

						if key == InputTypes.KB_P:
							import ChangePlayer
							ChangePlayer.changeCivPopup()
							return 1

						elif key == InputTypes.KB_L:
							import ChangePlayer
							ChangePlayer.changeHumanPopup()
							return 1

						elif key == InputTypes.KB_U:
							import ChangePlayer
							ChangePlayer.updateGraphics()
							return 1

			elif iModifiers == 3:

				if key == 16: # D
					DebugUtils.toggleDebugMode()
					return 1

				if DebugUtils.bDebugMode:
					if key == InputTypes.KB_U:
						DebugUtils.putOneOfEveryUnit()
						return 1
		return 0


	# Called globally as a result of a local CyMessageControl().sendModNetMessage() call - Avoids OOS errors in MP games.
	def onModNetMessage(self, argsList):
		ID, iData2, iData3, iData4, iData5 = argsList

		# Kill Deal
		if ID == 666:
			if iData2 > -1:
				if GAME.getActivePlayer() != iData2:
					GAME.getDeal(iData3).kill()
			else:
				GAME.getDeal(iData3).kill()

		# City Demolish
		elif ID in (901, 902, 903, 904, 905, 906):
			iPlayer = iData2
			if ID == 901:
				# DiploPenalty
				if iData4:
					GC.getPlayer(iData3).AI_changeMemoryCount(iPlayer, MemoryTypes.MEMORY_RAZED_CITY, iData5)
				else:
					GC.getPlayer(iData3).AI_changeMemoryCount(iPlayer, MemoryTypes.MEMORY_RAZED_HOLY_CITY, iData5)
			elif ID == 902:
				# Kill Guardian
				GC.getPlayer(iPlayer).getUnit(iData3).kill(False, -1)
			else:
				CyPlayer = GC.getPlayer(iPlayer)
				CyCity = CyPlayer.getCity(iData3)
				# 903 Demolish Building || 904 Abandon City || 905/906 Abandon->Units
				if ID == 903:
					CyCity.setNumRealBuilding(iData4, 0)
					CyPlayer.changeGold(iData5)
					CvScreensInterface.mainInterface.buildCityListLeft()
					CyInterface().setDirty(InterfaceDirtyBits.SelectionButtons_DIRTY_BIT, True)
					if GC.getBuildingInfo(iData4).getReligionType() >= 0:
						CyCity.changeHurryAngerTimer(CyCity.flatHurryAngerLength())
				elif ID == 904:
					CyPlayer.changeGold(iData5)
					self.onCityRazed((CyCity, iPlayer))
					CyCity.kill()
				elif ID == 905 or ID == 906:
					X = CyCity.getX()
					Y = CyCity.getY()
					CyUnit = CyPlayer.initUnit(iData5, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					if iData4 == -1:
						CyCity.addProductionExperience(CyUnit, True)
					else:
						if iData4 > 0:
							CyUnit.setExperience(iData4, iData4)
						if ID == 906 and self.PROMO_GUARDIAN_TRIBAL > -1:
							CyUnit.setHasPromotion(self.PROMO_GUARDIAN_TRIBAL, True)
					CyUnit.setMoves(CyUnit.maxMoves()-1)

		elif ID == 999:
			GC.getPlayer(iData2).changeCommercePercent(CommerceTypes(iData3), iData4)


	def gameStart(self, bNewGame=False):
		############################
		# Cache game specific data #
		if GAME.isNetworkMultiPlayer():
			self.bNetworkMP = True
		else:
			self.bNetworkMP = False

		# This is too early to actually set the initial camera zoom level, so raise a flag so it will happen later.
		CvScreensInterface.mainInterface.bSetStartZoom = True

		self.iTurnTopCiv = GAME.getGameTurn()
		self.iTurnsToTopCiv = 49 - (self.iTurnTopCiv % 50)

		CvAdvisorUtils.resetNoLiberateCities()

		self.GO_1_CITY_TILE_FOUNDING	= GAME.isOption(GameOptionTypes.GAMEOPTION_1_CITY_TILE_FOUNDING)
		self.GO_START_AS_MINORS			= GAME.isOption(GameOptionTypes.GAMEOPTION_START_AS_MINORS)
		self.GO_INFINITE_XP				= GAME.isOption(GameOptionTypes.GAMEOPTION_INFINITE_XP)

		if bNewGame and self.GO_START_AS_MINORS:
			for iTeam in xrange(GC.getMAX_PC_TEAMS()):
				CyTeam = GC.getTeam(iTeam)
				if CyTeam.isAlive() and not CyTeam.isOpenBordersTrading():
					CyTeam.setIsMinorCiv(True, False)

		CvGameSpeedInfo = GC.getGameSpeedInfo(GAME.getGameSpeedType())
		self.fTrainPrcntGS = CvGameSpeedInfo.getTrainPercent() / 100.0
		self.fVictoryDelayPrcntGS = CvGameSpeedInfo.getVictoryDelayPercent() / 100.0
		# Find special buildings built where by whom.
		mapBuildingType = self.mapBuildingType
		aList0 = [ # Only meant for world wonders
			"CRUSADE",			"KENTUCKY_DERBY",	"GREAT_ZIMBABWE",		"HELSINKI",				"ALAMO",
			"LASCAUX",			"WORLD_BANK",		"TAIPEI_101",			"ZIZKOV",				"CYRUS_CYLINDER",
			"FA_MEN_SI",		"WEMBLEY",			"PERGAMON",				"CYRUS_TOMB",			"TSUKIJI",
			"BIODOME",			"NAZCA_LINES",		"THE_MOTHERLAND_CALLS",	"GREAT_JAGUAR_TEMPLE",	"GREAT_BATH",
			"TOPKAPI_PALACE"
		] # KEY
		aList1 = [] # iBuilding
		aList2 = [] # iTech (Obsolete)
		i = 0
		while aList0[i:]:
			KEY = aList0[i]
			iBuilding = mapBuildingType[KEY]
			if iBuilding > -1:
				aList1.append(iBuilding)
				aList2.append(GC.getBuildingInfo(iBuilding).getObsoleteTech())
				i += 1
			else:
				print "\nWarning, CvEventManager expected a building with this KEY end [%s] to exist.\n" %KEY
				del aList0[i]

		aList3 = [-2] * len(aList0) # iCityID
		aList4 = list(aList3) # iOwner
		for iPlayer in xrange(self.MAX_PLAYERS):
			CyPlayer = GC.getPlayer(iPlayer)
			temp = list(aList4)
			for k, entry in enumerate(temp):
				if entry != -2:
					continue
				iBuilding = aList1[k]
				if CyPlayer.countNumBuildings(iBuilding):
					if GC.getTeam(CyPlayer.getTeam()).isObsoleteBuilding(iBuilding):
						aList4[k] = -1
					else:
						aList4[k] = iPlayer
					CyCity, i = CyPlayer.firstCity(False)
					while CyCity:
						if CyCity.getNumBuilding(iBuilding):
							if aList0[k] == "CRUSADE" and CyCity.getBuildingOriginalOwner(iBuilding) != iPlayer:
								# The Crusade is tied to the player who initiated it.
								aList4[k] = -1
							aList3[k] = CyCity.getID()
							break
						CyCity, i = CyPlayer.nextCity(i, False)
		# Get rid of wonders that is lost when a city is razed.
		# This can save memory and increase performance, a little.
		n = 0
		temp = list(aList4)
		for i, entry in enumerate(temp):
			if entry == -2:
				idx = i - n
				if GAME.isBuildingClassMaxedOut(GC.getBuildingInfo(aList1[idx]).getBuildingClassType(), 0):
					del aList0[idx], aList1[idx], aList2[idx], aList3[idx], aList4[idx]
					n += 1
		# Store the values.
		self.aWonderTuple = [aList0, aList1, aList2, aList3, aList4]
		# [0][X] = KEY		[1][X] = iBuilding		[2][X] = iTech (Obsolete)		[3][X] = iCityID		[4][X] = iOwner
		''' X:
		[0]  Crusade			[1]  Kentucky Derby			[2]  Great Zimbabwe			[3]  Helsinki			[4]  Alamo
		[5]  Lascaux			[6]  World Bank				[7]  Taipei 101				[8]  Zizkov				[9]  Cyrus Cylinder
		[10] Fa Men Si			[11] Wembley Stadium		[12] Pergamon Altar			[13] Tomb of Cyrus		[14] Tsukiji
		[15] Biodome			[16] Nazca Lines			[17] The Motherland Calls	[18] Jaguar Temple		[19] Great Bath
		[20] Topkapi Palace
		'''


	def onGameStart(self, argsList):
		# Called when a game is created the moment you can see the map.
		self.gameStart(True)
		G = GAME
		bPrehistoricStart = GC.getDefineINT("START_YEAR") == G.getGameTurnYear()

		if G.getGameTurn() == G.getStartTurn():
			if G.isHotSeat() or G.isPbem():
				for iPlayer in xrange(self.MAX_PC_PLAYERS):
					CyPlayer = GC.getPlayer(iPlayer)
					if CyPlayer.isHuman() and CyPlayer.isAlive():
						popup = CyPopupInfo()
						popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
						popup.setText("showDawnOfMan")
						popup.addPopup(iPlayer)
				if G.isPbem():
					popup = CyPopupInfo()
					popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_DETAILS)
					popup.setOption1(true)
					popup.addPopup(iPlayer)
			else:
				iPlayer = G.getActivePlayer()
				CyPlayer = GC.getPlayer(iPlayer)
				if CyPlayer.isHuman() and CyPlayer.isAlive():
					popup = CyPopupInfo()
					popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popup.setText('showDawnOfMan')
					popup.addPopup(iPlayer)
				szText = ""
				if not bPrehistoricStart:
					szText += "\n\n" + TRNSLTR.getText("TXT_KEY_MOD_GAMESTART_NOT_PREHISTORIC", ())
				if G.isOption(GameOptionTypes.GAMEOPTION_ADVANCED_START):
					szText += "\n\n" + TRNSLTR.getText("TXT_KEY_MOD_GAMESTART_ADVANCED_START", ())
				if szText:
					szText = TRNSLTR.getText("TXT_KEY_MOD_HEADER", ()) + szText
					popup = CyPopupInfo()
					popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_TEXT)
					popup.setText(szText)
					popup.addPopup(iPlayer)
		else:
			CyInterface().setSoundSelectionReady(true)

		if bPrehistoricStart:
			for iPlayer in xrange(self.MAX_PC_PLAYERS):
				CyPlayer = GC.getPlayer(iPlayer)
				CyUnit, i = CyPlayer.firstUnit(False)
				while CyUnit:
					if CyUnit.isFound():
						CyUnit.setHasPromotion(self.PROMO_GUARDIAN_TRIBAL, True)
						break
					CyUnit, i = CyPlayer.nextUnit(i, False)

	def onLoadGame(self, argsList):
		self.gameStart()


	'''
	def onPreSave(self, argsList): return


	def onSaveGame(self, argsList):
		# return the string to be saved - Must be a string
		return ""


	def onGameEnd(self, argsList):
		print 'Called at the End of the game', ("argsList", argsList)


	def onEndGameTurn(self, argsList):
		# Called at the end of the "end turn" cycle.
		iGameTurn = argsList[0]
	'''


	def onBeginGameTurn(self, argsList):
		# Called at the beginning of the end of each turn
		iGameTurn, = argsList
		if not self.bNetworkMP and not GAME.getAIAutoPlay(GAME.getActivePlayer()):
			if iGameTurn - self.iTurnTopCiv >= self.iTurnsToTopCiv:
				CyPlayer = GC.getActivePlayer()
				if CyPlayer and CyPlayer.isAlive():
					self.iTurnTopCiv = iGameTurn
					self.iTurnsToTopCiv = 40 + GAME.getSorenRandNum(21, "40-60")
					CvScreensInterface.showTopCivs()


	def onBeginPlayerTurn(self, argsList):
		# Called when a player end its turn.
		# The turn is not really handed over to the next player before after onEndPlayerTurn has been called.
		iGameTurn, iPlayer = argsList
		CyPlayer = GC.getPlayer(iPlayer)
		# Building effects
		aWonderTuple = self.aWonderTuple
		if iPlayer in aWonderTuple[4]:
			for i, iPlayerX in enumerate(aWonderTuple[4]):
				if iPlayerX != iPlayer: continue
				KEY = aWonderTuple[0][i]
				if KEY == "WORLD_BANK":
					CyPlayer.changeGold(CyPlayer.getGold()//200)
				elif KEY == "ZIZKOV":
					MAP = GC.getMap()
					iPlayerAct = GAME.getActivePlayer()
					TECH_SATELLITES = GC.getInfoTypeForString("TECH_SATELLITES")
					iWorldSize = MAP.getWorldSize() + 1
					iTeam = CyPlayer.getTeam()
					for iPlayerX in xrange(self.MAX_PC_PLAYERS):
						CyPlayerX = GC.getPlayer(iPlayerX)
						if not CyPlayerX.isAlive():
							continue
						iTeamX = CyPlayerX.getTeam()
						if iTeamX == iTeam:
							continue
						CyTeamX = GC.getTeam(iTeamX)
						if CyTeamX.isVassal(iTeam):
							continue
						if TECH_SATELLITES > -1 and CyTeamX.isHasTech(TECH_SATELLITES):
							continue

						if not GAME.getSorenRandNum(5 * iWorldSize, "Zizkov"):
							GC.getMap().resetRevealedPlots(iTeamX)
							if iPlayer == iPlayerAct:
								CvUtil.sendMessage(TRNSLTR.getText("TXT_ZIZKOV1", (CyPlayerX.getCivilizationDescription(0),)), iPlayer)
							elif iPlayerX == iPlayerAct:
								CvUtil.sendMessage(TRNSLTR.getText("TXT_ZIZKOV2",()), iPlayerX)

				elif KEY == "CYRUS_CYLINDER":
					if not iGameTurn % int(4*self.fVictoryDelayPrcntGS):
						iTeam = CyPlayer.getTeam()
						Value = iGameTurn * 2
						for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
							CyTeamX = GC.getTeam(iTeamX)
							if CyTeamX.isAlive() and CyTeamX.isVassal(iTeam):
								Value -= Value / 8
						iGGP = int(GAME.getPlayerScore(iPlayer) / Value)
						CyPlayer.changeCombatExperience(iGGP)
				elif KEY == "TOPKAPI_PALACE":
					iTeam = CyPlayer.getTeam()
					for iPlayerX in xrange(self.MAX_PC_PLAYERS):
						if iPlayerX == iPlayer: continue
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isAlive() and GC.getTeam(CyPlayerX.getTeam()).isVassal(iTeam):
							iGold = int(((1000000 * CyPlayerX.getGreaterGold() + CyPlayerX.getGold()) ** 0.5)/2500)
							if iGold:
								CyPlayerX.changeGold(iGold)
							CyPlayerX.changeCombatExperience(1)

		# Aging Animals
		if not CyPlayer.isNPC() or CyPlayer.isHominid():
			return
		fModGS = self.fVictoryDelayPrcntGS
		iMinorIncrement = int(16 * fModGS)
		iMajorIncrement = int(128 * fModGS)
		bMinor = not iGameTurn % iMinorIncrement
		bMajor = not iGameTurn % iMajorIncrement

		if bMinor or bMajor:
			CyUnit, i = CyPlayer.firstUnit(False)
			while CyUnit:
				if not CyUnit.isDead() and CyUnit.isAnimal():
					if not GAME.getSorenRandNum(15 - bMajor*10, "Aging"): # 1 in 15
						if not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT1")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT1"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT2")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT2"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT3")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT3"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT4")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT4"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT5")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT5"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT6")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT6"), True)
						elif bMajor:
							CyUnit.setBaseCombatStr(CyUnit.baseCombatStr() + 1)
						else:
							CyUnit.setExperience(CyUnit.getExperience() + 3, -1)
				CyUnit, i = CyPlayer.nextUnit(i, False)


	def onEndPlayerTurn(self, argsList):
		# Called right before the turn is handed over to the next player.
		iGameTurn, iPlayer = argsList

		if not iGameTurn:
			CyPlayer = GC.getPlayer(iPlayer)
			if CyPlayer.isHuman():
				if CyPlayer.canRevolution(0):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_CHANGECIVIC)
					popupInfo.addPopup(iPlayer)

		aWonderTuple = self.aWonderTuple
		if "WEMBLEY" in aWonderTuple[0]:
			i = aWonderTuple[0].index("WEMBLEY")
			if iPlayer == aWonderTuple[4][i]:
				CyPlayer = GC.getPlayer(iPlayer)
				ExcessHappy = CyPlayer.calculateTotalCityHappiness() - CyPlayer.calculateTotalCityUnhappiness()
				if ExcessHappy > 0:
					CyPlayer.changeGold(ExcessHappy * 2)

		CvAdvisorUtils.resetAdvisorNags()
		CvAdvisorUtils.endTurnFeats(iPlayer)


	'''
	def onEndTurnReady(self, argsList):
		iGameTurn = argsList[0]

	def onFirstContact(self, argsList):
		iTeamX, iHasMetTeamY = argsList
	'''


	def onCombatResult(self, argsList):
		CyUnitW, CyUnitL = argsList
		iPlayerW = CyUnitW.getOwner()
		CyPlayerW = GC.getPlayer(iPlayerW)
		iPlayerL = CyUnitL.getOwner()
		CyPlayerL = GC.getPlayer(iPlayerL)
		CyTeamW = None
		iPlayerAct = None
		# WarPrizes
		if self.iWAR_PRIZES:
			iValid = GC.getInfoTypeForString("UNITCOMBAT_NAVAL_COMBATANT")
			if iValid > -1 and CyUnitW.isHasUnitCombat(iValid) and CyUnitL.isHasUnitCombat(iValid):
				# 20% chance
				if GAME.getSorenRandNum(100, "WarPrize") < 20:

					iUnitL = CyUnitL.getUnitType()
					iX = CyUnitW.getX()
					iY = CyUnitW.getY()
					CyUnitWP = CyPlayerW.initUnit(iUnitL, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					CyUnitWP.finishMoves()
					CyUnitWP.setDamage(GAME.getSorenRandNum(40, "XP") + 40, iPlayerW)
					if CyUnitW.isHiddenNationality():
						CyUnitWP.doHNCapture()
					# Message
					iPlayerAct = GAME.getActivePlayer()
					if iPlayerAct in (iPlayerW, iPlayerL):
						CvUnitInfoL = GC.getUnitInfo(iUnitL)
						szWP = CvUnitInfoL.getDescription()
					if iPlayerW == iPlayerAct:
						eColor = ColorTypes(GC.getInfoTypeForString("COLOR_GREEN"))
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_SUCCESS", (szWP,)), iPlayerW, 16, CvUnitInfoL.getButton(), eColor, iX, iY, True, True, bForce=False)
					elif iPlayerL == iPlayerAct:
						iX = CyUnitL.getX()
						iY = CyUnitL.getY()
						eColor = ColorTypes(GC.getInfoTypeForString("COLOR_RED"))
						artPath = 'Art/Interface/Buttons/General/warning_popup.dds'
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_FAILURE", (szWP,)), iPlayerL, 16, artPath, eColor, iX, iY, True, True, bForce=False)
					# Booty
					iValid = GC.getInfoTypeForString("UNITCOMBAT_TRANSPORT")
					if iValid > -1 and CyUnitL.isHasUnitCombat(iValid):
						Loot = int(GAME.getSorenRandNum(100, "Loot") * self.fTrainPrcntGS)
						if CyPlayerL.getGold() >= Loot:
							CyPlayerW.changeGold(Loot)
							CyPlayerL.changeGold(-Loot)
							if iPlayerW == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_SUCCESS_GOLD_GAINED", (szWP,)), iPlayerW, 24,
									'Art/Interface/Buttons/process/processwealth.dds', eColor, iX, iY, True, True, bForce=False
								)
							elif iPlayerL == iPlayerAct:
								CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_FAILURE_GOLD_LOST", (szWP,)), iPlayerL, 24, artPath, eColor, iX, iY, True, True, bForce=False)

		# Respawn Promotion
		mapPromoType = self.mapPromoType
		iRespawn2 = mapPromoType['PROMOTION_LIVE2']
		iRespawn1 = mapPromoType['PROMOTION_LIVE1']
		bRespawn2 = CyUnitL.isHasPromotion(iRespawn2)

		if CyUnitL.isHasPromotion(iRespawn1) or bRespawn2:
			iUnit = CyUnitL.getUnitType()
			iX = -1
			if CyUnitL.getDomainType() == self.mapDomain['DOMAIN_SEA']:
				CyCity, i = CyPlayer.firstCity(False)
				while CyCity:
					if CyCity.isCoastal(0):
						iX = CyCity.getX()
						iY = CyCity.getY()
						break
					CyCity, i = CyPlayer.nextCity(i, False)
			else:
				CyCity = CyPlayerL.getCapitalCity()
				if not CyCity.isNone():
					iX = CyCity.getX()
					iY = CyCity.getY()
			if iX == -1:
				iX = CyUnitL.getX()
				iY = CyUnitL.getY()
			CyUnit = CyPlayerL.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_NORTH)
			CyUnit.convert(CyUnitL)
			CyUnit.setDamage(GAME.getSorenRandNum(40, "Damage") + 20, False)

			CyUnit.finishMoves()

			if bRespawn2:
				CyUnit.setHasPromotion(iRespawn2, False)
			else:
				CyUnit.setHasPromotion(iRespawn1, False)

			if iPlayerAct is None:
				iPlayerAct = GAME.getActivePlayer()
			if iPlayerL == iPlayerAct:
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_REBORN",()), iPlayerL, 16, 'Art/Interface/Buttons/Phoenix1.dds', ColorTypes(44), iX, iY, True, True, bForce=False)

		if CyUnitL.isNPC():
			# EXP boost for AI
			if not CyUnitW.isHuman():
				CyUnitW.changeExperience(2, 100, False, False, False)

			# Exp limit loophole
			if not self.GO_INFINITE_XP:
				iExpLimit = -1
				if CyUnitL.isAnimal():
					if CyUnitW.isHasPromotion(GC.getInfoTypeForString("PROMOTION_ANIMAL_HUNTER")):
						iExpLimit = GC.getDefineINT("ANIMAL_MAX_XP_VALUE")

				elif CyUnitW.isHasPromotion(GC.getInfoTypeForString("PROMOTION_BARBARIAN_HUNTER")):
					iExpLimit = GC.getDefineINT("BARBARIAN_MAX_XP_VALUE")

				if iExpLimit != -1 and CyUnitW.getExperience() >= iExpLimit:
					bInBorders = iPlayerW == GC.getMap().plot(CyUnitW.getX(), CyUnitW.getY()).getOwner()
					CyUnitW.changeExperience(1, 9999, True, bInBorders, True)
		else:
			bSneak = CyUnitW.isHasPromotion(mapPromoType['PROMOTION_SNEAK'])
			bMarauder = CyUnitW.isHasPromotion(mapPromoType['PROMOTION_MARAUDER'])
			bIndustrySpy = CyUnitW.isHasPromotion(mapPromoType['PROMOTION_INDUSTRYESPIONAGE'])
			# Commerce Theft
			if bSneak or bMarauder or bIndustrySpy:

				if bSneak or bIndustrySpy:
					iTeamW = CyPlayerW.getTeam()
					iTeamL = CyPlayerL.getTeam()
					CyTeamW = GC.getTeam(iTeamW)
					CyTeamL = GC.getTeam(iTeamL)

				# Factor in winners handicap versus losers handicap
				fHandicapFactor = 1
				if CyPlayerW.isHuman():
					if CyPlayerL.isHuman():
						iHandicapFactorW = GC.getHandicapInfo(CyPlayerW.getHandicapType()).getCivicUpkeepPercent()
						iHandicapFactorL = GC.getHandicapInfo(CyPlayerL.getHandicapType()).getCivicUpkeepPercent()
					else:
						iHandicapFactorW = GC.getHandicapInfo(CyPlayerW.getHandicapType()).getCivicUpkeepPercent()
						iHandicapFactorL = 100

					if iHandicapFactorW > iHandicapFactorL:
						fHandicapFactor = 4 * (iHandicapFactorW - iHandicapFactorL + 100) / 100.0
				# Message
				if iPlayerAct is None:
					iPlayerAct = GAME.getActivePlayer()
				if iPlayerAct in (iPlayerW, iPlayerL):
					iX = CyUnitW.getX()
					iY = CyUnitW.getY()
					if iPlayerW == iPlayerAct:
						if CyUnitL.isHiddenNationality():
							szTxt = TRNSLTR.getText("TXT_KEY_SNEAK_HIDDEN",())
						else:
							szTxt = CyPlayerL.getName()
					elif iPlayerL == iPlayerAct:
						if CyUnitW.isHiddenNationality():
							szTxt = TRNSLTR.getText("TXT_KEY_SNEAK_HIDDEN",())
						else:
							szTxt = CyPlayerW.getName()
				# Sneak promo
				if bSneak:
					iStolen = (CyTeamL.getEspionagePointsAgainstTeam(iTeamW) * 2 - CyTeamW.getEspionagePointsAgainstTeam(iTeamL))
					if iStolen > 1:
						iStolen = int((iStolen ** 0.5) / fHandicapFactor)
					if iStolen > 0:
						CyTeamW.changeEspionagePointsAgainstTeam(iTeamL, iStolen)
						CyTeamL.changeEspionagePointsAgainstTeam(iTeamW,-iStolen)
						# Message
						if iPlayerW == iPlayerAct:
							CvUtil.sendMessage(
								TRNSLTR.getText("TXT_KEY_SNEAK_YOU", (szTxt, iStolen)), iPlayerW, 16,
								'Art/Interface/Buttons/Process/spyprocessmeager.dds',
								ColorTypes(44), iX, iY, True, True, bForce=False
							)
						elif iPlayerL == iPlayerAct:
							CvUtil.sendMessage(
								TRNSLTR.getText("TXT_KEY_SNEAK_FROM",(szTxt, iStolen)), iPlayerL, 16,
								'Art/Interface/Buttons/Process/spyprocessmeager.dds',
								ColorTypes(44), iX, iY, True, True, bForce=False
							)
				# Marauder promo
				if bMarauder:
					iStolen = int((((1000000 * CyPlayerL.getGreaterGold() + CyPlayerL.getGold()) / (CyPlayerL.getNumUnits() + 1)) ** .3) / fHandicapFactor)
					if iStolen > 0:
						CyPlayerL.changeGold(-iStolen)
						CyPlayerW.changeGold(iStolen)
						# Message
						if iPlayerW == iPlayerAct:
							CvUtil.sendMessage(
								TRNSLTR.getText("TXT_KEY_MARAUDER_YOU", (szTxt, iStolen)), iPlayerW, 16,
								'Art/Interface/Buttons/Process/processwealthmeagre.dds',
								ColorTypes(44), iX, iY, True, True, bForce=False
							)
						elif iPlayerL == iPlayerAct:
							CvUtil.sendMessage(
								TRNSLTR.getText("TXT_KEY_MARAUDER_FROM",(szTxt, iStolen)), iPlayerL, 16,
								'Art/Interface/Buttons/Process/processwealthmeagre.dds',
								ColorTypes(44), iX, iY, True, True, bForce=False
							)
				# Industry Espionage promo
				if bIndustrySpy:
					iTechW = CyPlayerW.getCurrentResearch()
					iTechL = CyPlayerL.getCurrentResearch()

					if CyTeamL.isHasTech(iTechW) or not CyTeamW.isHasTech(iTechL):

						iStolen = int((CyPlayerL.calculateBaseNetResearch() ** 0.5)/fHandicapFactor)
						if iStolen:
							CyTeamW.changeResearchProgress(iTechW, iStolen, iPlayerW)
							CyTeamL.changeResearchProgress(iTechL,-iStolen, iPlayerL)
							# Message
							if iPlayerW == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_INDUSTRYESPIONAGE_YOU", (szTxt, iStolen)), iPlayerW, 16,
									'Art/Interface/Buttons/Process/processresearchmeagre.dds',
									ColorTypes(44), iX, iY, True, True, bForce=False
								)
							elif iPlayerL == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_INDUSTRYESPIONAGE_FROM",(szTxt, iStolen)), iPlayerL, 16,
									'Art/Interface/Buttons/Process/processresearchmeagre.dds',
									ColorTypes(44), iX, iY, True, True, bForce=False
								)

		iUnitW = CyUnitW.getUnitType()
		mapUnitType = self.mapUnitType
		# Warriors Of God - Monk
		if iUnitW == mapUnitType["MONK"]:
			X = CyUnitW.getX()
			Y = CyUnitW.getY()
			CyPlot = GC.getMap().plot(X, Y)
			if CyPlot.isCity():
				CyCity = CyPlot.getPlotCity()
				iReligion = CyPlayerW.getStateReligion()
				if iReligion != -1 and not CyCity.isHasReligion(iReligion):
					CyCity.setHasReligion(iReligion, True, True, False)
		# Warriors Of God - Fanatic
		elif iUnitW == mapUnitType["FANATIC"]:
			iReligion = CyPlayerW.getStateReligion()
			if iReligion != -1:
				X = CyUnitL.getX()
				Y = CyUnitL.getY()
				CyPlot = GC.getMap().plot(X, Y)
				if CyPlot.isCity():
					CyCity = CyPlot.getPlotCity()
					if not CyCity.isHasReligion(iReligion):
						if not CyTeamW:
							iTeamW = CyPlayerW.getTeam()
							CyTeamW = GC.getTeam(iTeamW)
						if CyTeamW.isAtWar(GC.getPlayer(CyCity.getOwner()).getTeam()):
							NumDef = 0
							for i in range(self.MAX_PLAYERS):
								if i == iWinner: continue
								CyPlayer = GC.getPlayer(i)
								iTeam = CyPlayer.getTeam()
								if iTeam == iTeamW: continue
								if CyTeamW.isAtWar(iTeam):
									NumDef += CyPlot.getNumDefenders(i)
									if NumDef > 1:
										break
							if NumDef <= 1:
								CyCity.setHasReligion(i, False, False, False)

		aWonderTuple = self.aWonderTuple
		if iPlayerW in aWonderTuple[4]:
			for i, iPlayer in enumerate(aWonderTuple[4]):
				if iPlayer != iPlayerW: continue
				KEY = aWonderTuple[0][i]
				if KEY == "PERGAMON":
					if CyUnitL.isNPC(): continue
					iGGP = int(CyUnitL.getExperience() ** 0.5)
					if iGGP:
						CyPlayerW.changeCombatExperience(iGGP)
						CyPlayerL.changeCombatExperience(-iGGP)
				elif KEY == "GREAT_JAGUAR_TEMPLE":
					if CyUnitL.isNPC(): continue
					iChance = GAME.getSorenRandNum(5, "Jaguar")
					if not iChance:
						CyUnitW.setDamage(0, False)


	def onCombatLogCalc(self, argsList):
		genericArgs = argsList[0][0]
		cdAttacker = genericArgs[0]
		cdDefender = genericArgs[1]
		iCombatOdds = genericArgs[2]
		CvUtil.combatMessageBuilder(cdAttacker, cdDefender, iCombatOdds)


	def onCombatLogHit(self, argsList):
		genericArgs = argsList[0][0]
		cdAttacker = genericArgs[0]
		cdDefender = genericArgs[1]
		iIsAttacker = genericArgs[2]
		iDamage = genericArgs[3]

		if cdDefender.eOwner == cdDefender.eVisualOwner:
			szDefenderName = GC.getPlayer(cdDefender.eOwner).getNameKey()
		else:
			szDefenderName = TRNSLTR.getText("TXT_KEY_TRAIT_PLAYER_UNKNOWN", ())
		if cdAttacker.eOwner == cdAttacker.eVisualOwner:
			szAttackerName = GC.getPlayer(cdAttacker.eOwner).getNameKey()
		else:
			szAttackerName = TRNSLTR.getText("TXT_KEY_TRAIT_PLAYER_UNKNOWN", ())

		if not iIsAttacker:
			combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_HIT", (szDefenderName, cdDefender.sUnitName, iDamage, cdDefender.iCurrHitPoints, cdDefender.iMaxHitPoints))
			CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
			CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
			if cdDefender.iCurrHitPoints <= 0:
				combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_DEFEATED", (szAttackerName, cdAttacker.sUnitName, szDefenderName, cdDefender.sUnitName))
				CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
				CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
		elif iIsAttacker == 1:
			combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_HIT", (szAttackerName, cdAttacker.sUnitName, iDamage, cdAttacker.iCurrHitPoints, cdAttacker.iMaxHitPoints))
			CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
			CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
			if cdAttacker.iCurrHitPoints <= 0:
				combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_DEFEATED", (szDefenderName, cdDefender.sUnitName, szAttackerName, cdAttacker.sUnitName))
				CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
				CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)


	def onImprovementBuilt(self, argsList):
		iImprovement, iX, iY = argsList
		if iImprovement < 0:
			print "[INFO] CvEventManager.onImprovementBuilt\n\tUnexpected input argument"
			return

		# Worker placed bonus
		aList = GC.getImprovementInfo(iImprovement).getType().split("_")
		if aList[1] == "BONUS":
			CyPlot = GC.getMap().plot(iX, iY)
			CyPlot.setImprovementType(-1)
			CyPlot.setBonusType(GC.getInfoTypeForString("BONUS_" + aList[2]))
			return
		# Worker placed feature
		mapImpType = self.mapImpType
		if iImprovement == mapImpType['IMPROVEMENT_TREE_NURSERY']:
			CyPlot = GC.getMap().plot(iX, iY)
			CyPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_FOREST_NEW'), 0)

		elif iImprovement == mapImpType['IMPROVEMENT_YOUNG_FOREST']:
			CyPlot = GC.getMap().plot(iX, iY)
			iFeatureNewForest = GC.getInfoTypeForString('FEATURE_FOREST_NEW')
			if CyPlot.getFeatureType() == iFeatureNewForest:
				CyPlot.setFeatureType(iFeatureNewForest, 0)

			iFeatureForest = GC.getInfoTypeForString('FEATURE_FOREST')
			iChance = GAME.getSorenRandNum(100, "FEATURE_FOREST")
			lat = CyPlot.getLatitude()

			if lat > 60: # POLAR
				if CyPlot.getTerrainType() == GC.getInfoTypeForString('TERRAIN_TAIGA'):
					CyPlot.setFeatureType(iFeatureForest, 2) # snowy forest
				else:
					CyPlot.setFeatureType(iFeatureForest, 1) # evergreen forest
			elif lat > 25: # TEMPERATE
				if iChance < 50:
					CyPlot.setFeatureType(iFeatureForest, 0) # leafy forest
				else:
					CyPlot.setFeatureType(iFeatureForest, 1) # evergreen forest
			else: # EQUATOR
				if iChance < 30:
					CyPlot.setFeatureType(iFeatureForest, 0) # leafy forest
				elif iChance < 60:
					CyPlot.setFeatureType(iFeatureForest, 1) # evergreen forest
				else:
					if CyPlot.getTerrainType() == GC.getInfoTypeForString('TERRAIN_GRASS'):
						CyPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_JUNGLE'), 0) # jungle
					else:
						CyPlot.setFeatureType(iFeatureForest, 0) # leafy forest
			CyPlot.setImprovementType(-1)

		elif iImprovement == mapImpType['IMPROVEMENT_PLANT_FOREST']:
			CyPlot = GC.getMap().plot(iX, iY)
			iFeatureNewForest = GC.getInfoTypeForString('FEATURE_FOREST_NEW')
			if CyPlot.getFeatureType() == iFeatureNewForest:
				CyPlot.setFeatureType(iFeatureNewForest, 0)
			CyPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_FOREST'), 0)
			CyPlot.setImprovementType(-1)

		elif iImprovement == mapImpType['IMPROVEMENT_PLANT_BAMBOO']:
			CyPlot = GC.getMap().plot(iX, iY)
			CyPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_BAMBOO'), 0)
			CyPlot.setImprovementType(-1)

		elif iImprovement == mapImpType['IMPROVEMENT_PLANT_SAVANNA']:
			CyPlot = GC.getMap().plot(iX, iY)
			CyPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_SAVANNA'), 0)
			CyPlot.setImprovementType(-1)

		elif iImprovement == mapImpType['IMPROVEMENT_FARM']:
			iPlayer = GC.getMap().plot(iX, iY).getOwner()
			if iPlayer != -1:
				CyPlayer = GC.getPlayer(iPlayer)
				if CyPlayer.isCivic(self.CIVIC_CONSCRIPTION):
					iEra = CyPlayer.getCurrentEra()
					if iEra != -1 and iEra in self.mapMilitiaByEra and self.mapMilitiaByEra[iEra] != -1:
						CyPlayer.initUnit(self.mapMilitiaByEra[iEra], iX, iY, UnitAITypes.UNITAI_RESERVE, DirectionTypes.NO_DIRECTION)
						if iPlayer == GAME.getActivePlayer():
							CvUtil.sendMessage(TRNSLTR.getText("TXT_RECRUITED",()), iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iX, iY, True, True, bForce=False)


	'''
	def onImprovementDestroyed(self, argsList):
		iImprovement, iOwner, iX, iY = argsList


	def onRouteBuilt(self, argsList):
		iRoute, iX, iY = argsList


	def onPlotRevealed(self, argsList):
		pPlot = argsList[0]
		iTeam = argsList[1]


	def onPlotFeatureRemoved(self, argsList):
		pPlot = argsList[0]
		iFeatureType = argsList[1]
		pCity = argsList[2] # This can be null


	def onPlotPicked(self, argsList):
		pPlot = argsList[0]
	'''


	def onNukeExplosion(self, argsList):
		CyPlot, CyUnit = argsList
		if not CyUnit:
			return
		GC = CyGlobalContext()
		iX = CyPlot.getX()
		iY = CyPlot.getY()
		iUnit = CyUnit.getUnitType()
		if iUnit == GC.getInfoTypeForString('UNIT_TURN'):
			if CyPlot.isCity():
				GC.getPlayer(CyUnit.getOwner()).acquireCity(CyPlot.getPlotCity(), False, False)
			for iiX in range(iX-1, iX+2, 1):
				for iiY in range(iY-1, iY+2, 1):
					numUnits = CyPlot.getNumUnits()
					for e in xrange(numUnits,0,-1):
						pUnit = CyPlot.getUnit(e)
						pUnit.kill(False, -1)
					pNukedPlot = GC.getMap().plot(iiX, iiY)
					if pNukedPlot.getFeatureType() == GC.getInfoTypeForString('FEATURE_FALLOUT'):
						pNukedPlot.setFeatureType(-1, -1)

		elif iUnit == GC.getInfoTypeForString('UNIT_FUSION_NUKE'):
			for iXLoop in range(iX - 0, iX + 1, 1):
				for iYLoop in range(iY - 0, iY + 1, 1):
					CyPlot = GC.getMap().plot(iXLoop, iYLoop)
					if CyPlot.isPeak():
						CyPlot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
					elif CyPlot.isHills():
						CyPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)

		elif iUnit == GC.getInfoTypeForString('UNIT_FUSION_NOVA'):
			for iXLoop in range(iX - 1, iX + 2, 1):
				for iYLoop in range(iY - 1, iY + 2, 1):
					CyPlot = GC.getMap().plot(iXLoop, iYLoop)
					if CyPlot.isPeak():
						CyPlot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
					elif CyPlot.isHills():
						CyPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)

		elif iUnit == GC.getInfoTypeForString('UNIT_POISON_NUKE'):
			for iXLoop in range(iX - 1, iX + 2, 1):
				for iYLoop in range(iY - 1, iY + 2, 1):
					CyPlot = GC.getMap().plot(iXLoop, iYLoop)
					CyPlot.setFeatureType(GC.getInfoTypeForString("FEATURE_BIOGAS"), 1)

		elif iUnit == GC.getInfoTypeForString('UNIT_POISON_NOVA'):
			for iXLoop in range(iX - 5, iX + 6, 1):
				for iYLoop in range(iY - 5, iY + 6, 1):
					CyPlot = GC.getMap().plot(iXLoop, iYLoop)
					CyPlot.setFeatureType(GC.getInfoTypeForString("FEATURE_PLAGUEGAS"), 1)
		else:
			print "CvEventManager.onNukeExplosion\n\tNuke with no special effects: " + CyUnit.getName()
		'''
		# Nuclear Non-Proliferation Treaty (NPT)
		if GAME.getProjectCreatedCount(GC.getInfoTypeForString("PROJECT_NPT")):
			iPlayer = CyUnit.getOwner()
			iTeam = GC.getPlayer(iPlayer).getTeam()
			iAttitude = GC.getInfoTypeForString("ATTITUDE_FURIOUS")
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				if iPlayerX == iPlayer: continue
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isHuman() or not CyPlayerX.isAlive(): continue
				CyPlayerX.AI_changeAttitudeExtra(iTeam, -1)
				if CyPlayerX.AI_getAttitude(iPlayer) == iAttitude:
					CyTeamX = GC.getTeam(CyPlayerX.getTeam())
					if CyTeamX.canDeclareWar(iTeam):
						CyTeamX.declareWar(iTeam, True, -1)
		'''


	'''
	def onGotoPlotSet(self, argsList):
		pPlot, iPlayer = argsList
	'''


	def onBuildingBuilt(self, argsList):
		CyCity, iBuilding = argsList
		CvAdvisorUtils.buildingBuiltFeats(CyCity, iBuilding)
		iPlayer = CyCity.getOwner()
		G = GAME
		if not self.bNetworkMP and iPlayer == G.getActivePlayer():
			if GC.getBuildingInfo(iBuilding).getMovie():
				if GC.getPlayer(iPlayer).countNumBuildings(iBuilding) == 1:
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setData1(iBuilding)
					popupInfo.setData2(CyCity.getID())
					popupInfo.setData3(0)
					popupInfo.setText('showWonderMovie')
					popupInfo.addPopup(iPlayer)

		mapBuildingType = self.mapBuildingType
		aWonderTuple = self.aWonderTuple
		if iBuilding in aWonderTuple[1]:
			i = aWonderTuple[1].index(iBuilding)
			KEY = aWonderTuple[0][i]
			aWonderTuple[3][i] = CyCity.getID()
			aWonderTuple[4][i] = iPlayer
			if KEY == "CRUSADE":
				iUnit = GC.getInfoTypeForString("UNIT_CRUSADER")
				if iUnit > -1:
					CyUnit = GC.getPlayer(iPlayer).initUnit(iUnit, CyCity.getX(), CyCity.getY(), UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.NO_DIRECTION)
					CyCity.addProductionExperience(CyUnit, False)
			elif KEY == "KENTUCKY_DERBY":
				iUnit = GC.getInfoTypeForString("UNIT_SUBDUED_HORSE")
				if iUnit > -1:
					CyUnit = GC.getPlayer(iPlayer).initUnit(iUnit, CyCity.getX(), CyCity.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					CyCity.addProductionExperience(CyUnit, False)
			elif KEY == "TAIPEI_101":
				iTeam = GC.getPlayer(iPlayer).getTeam()
				for iPlayerX in xrange(self.MAX_PC_PLAYERS):
					if iPlayer == iPlayerX:
						continue
					CyPlayerX = GC.getPlayer(iPlayerX)
					if CyPlayerX.isAlive():
						CyPlayerX.AI_changeAttitudeExtra(iTeam, 3)
			elif KEY == "ZIZKOV":
				TECH_SATELLITES = GC.getInfoTypeForString("TECH_SATELLITES")
				iTeam = GC.getPlayer(iPlayer).getTeam()
				# Reveals whole map for owner
				for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
					if iTeamX == iTeam:
						continue
					CyTeamX = GC.getTeam(iTeamX)
					if not CyTeamX.isEverAlive() or CyTeamX.isVassal(iTeam):
						continue
					if TECH_SATELLITES > -1 and CyTeamX.isHasTech(TECH_SATELLITES):
						continue
					# Covers whole map for others
					GC.getMap().resetRevealedPlots(iTeamX)
				CvUtil.sendImmediateMessage(TRNSLTR.getText("TXT_GLOBAL_JAM",()))
			elif KEY == "TSUKIJI":
				CyTeam = GC.getTeam(GC.getPlayer(iPlayer).getTeam())
				BOAT = GC.getInfoTypeForString("IMPROVEMENT_FISHING_BOATS")
				if BOAT > -1:
					CyTeam.changeImprovementYieldChange(BOAT, 0, 1)
					CyTeam.changeImprovementYieldChange(BOAT, 2, 1)

				aBonusList = []
				aList = [
					GC.getInfoTypeForString("BONUS_FISH"),
					GC.getInfoTypeForString("BONUS_CLAM"),
					GC.getInfoTypeForString("BONUS_CRAB"),
					GC.getInfoTypeForString("BONUS_SHRIMP"),
					GC.getInfoTypeForString("BONUS_LOBSTER")
				]
				iBonuses = 0
				for iBonus in aList:
					if iBonus > -1:
						aBonusList.append(iBonus)
						iBonuses += 1

				aPlotList = []
				if aBonusList:
					iPlots = 0
					iX = CyCity.getX()
					iY = CyCity.getY()
					for x in xrange(iX - 3, iX + 4):
						for y in xrange(iY - 3, iY + 4):
							CyPlot = GC.getMap().plot(x, y)
							if not CyPlot.isWater() or CyPlot.getBonusType(-1) != -1:
								continue
							aPlotList.append(CyPlot)
							iPlots += 1

				if aPlotList:
					if iPlayer == GAME.getActivePlayer():
						bMessage = True
					else:
						bMessage = False
					bFirst = True
					while aPlotList:
						CyPlot = aPlotList.pop(GAME.getSorenRandNum(iPlots, "TSUKIJI"))
						iPlots -= 1
						if bFirst or not GAME.getSorenRandNum(5, "TSUKIJI"):
							bFirst = False
							BONUS = aBonusList[GAME.getSorenRandNum(iBonuses, "TSUKIJI")]
							CyPlot.setBonusType(BONUS)
							if BOAT > -1:
								CyPlot.setImprovementType(BOAT)
							if bMessage:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_TSUKIJI_BONUS", (GC.getBonusInfo(BONUS).getDescription(),)),
									iPlayer, 16, GC.getBonusInfo(BONUS).getButton(), ColorTypes(11), iX, iY, True, True
								)

			elif KEY == "NAZCA_LINES":
				NAZCA_LINES = GC.getInfoTypeForString("BUILDINGCLASS_NAZCA_LINES")
				if NAZCA_LINES > -1:
					iEra = GC.getPlayer(iPlayer).getCurrentEra() + 1
					for i in xrange(iEra):
						iRandom = GAME.getSorenRandNum(8, "NazcaLines")
						if not iRandom:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 0)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 0, iBase + 4)
						elif iRandom == 1:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 1)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 1, iBase + 4)
						elif iRandom == 2:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 2)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 2, iBase + 4)
						elif iRandom == 3:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 3)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 3, iBase + 4)
						elif iRandom == 4:
							iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 0)
							CyCity.setBuildingYieldChange(NAZCA_LINES, 0, iBase + 4)
						elif iRandom == 5:
							iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 1)
							CyCity.setBuildingYieldChange(NAZCA_LINES, 1, iBase + 4)
						elif iRandom == 6:
							iBase = CyCity.getBuildingHappyChange(NAZCA_LINES)
							CyCity.setBuildingHappyChange(NAZCA_LINES, iBase + 2)
						else:
							iBase = CyCity.getBuildingHealthChange(NAZCA_LINES)
							CyCity.setBuildingHealthChange(NAZCA_LINES, iBase + 2)
		# Ishtar or Marco Polo
		elif iBuilding in (mapBuildingType["ISHTAR"], mapBuildingType["MARCO_POLO"]):
			CyArea = CyCity.area()
			iTeam = GC.getPlayer(iPlayer).getTeam()
			CyTeam = GC.getTeam(iTeam)
			for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
				if iTeamX != iTeam and not CyTeam.isHasMet(iTeamX):
					CyTeamX = GC.getTeam(iTeamX)
					if CyTeamX.countNumCitiesByArea(CyArea) > 0:
						CyTeamX.meet(iTeam, True)
		# Djenne
		elif iBuilding == mapBuildingType["DJENNE"]:
			MAP = GC.getMap()
			iX = CyCity.getX()
			iY = CyCity.getY()
			DESERT	= GC.getInfoTypeForString('TERRAIN_DESERT')
			DUNES	= GC.getInfoTypeForString('TERRAIN_DUNES')
			for x in xrange(iX - 2, iX + 3, 1):
				for y in xrange(iY - 2, iY + 3, 1):
					CyPlot = MAP.plot(x, y)
					if CyPlot.isPlayerCityRadius(iPlayer):
						iTerrain = CyPlot.getTerrainType()
						if iTerrain == DESERT:
							G.setPlotExtraYield(x, y, YieldTypes.YIELD_COMMERCE, 2)
						elif iTerrain == DUNES:
							G.setPlotExtraYield(x, y, YieldTypes.YIELD_COMMERCE, 1)
			if iPlayer == GAME.getActivePlayer():
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_DJENNE_PYTHON",()), iPlayer, 16, 'Art/Interface/Buttons/Great_Wonders/great_mosque_of_djenne.dds', ColorTypes(44), iX, iY, True, True)
		# Lotus Temple
		elif iBuilding == mapBuildingType["LOTUS_TEMPLE"]:
			CyPlayer = GC.getPlayer(iPlayer)
			iTeam = CyPlayer.getTeam()
			bHuman = CyPlayer.isHuman()
			iAttitude = GC.getInfoTypeForString("ATTITUDE_CAUTIOUS")
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				if iPlayer == iPlayerX:
					continue
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					if not bHuman:
						iTeamX = CyPlayerX.getTeam()
						while CyPlayer.AI_getAttitudeExtra(iPlayerX) < iAttitude:
							CyPlayer.AI_changeAttitudeExtra(iTeamX, 1)
					if not CyPlayerX.isHuman():
						while CyPlayerX.AI_getAttitudeExtra(iPlayer) < iAttitude:
							CyPlayerX.AI_changeAttitudeExtra(iTeam, 1)
		# Cleopatra's Needle
		elif iBuilding == mapBuildingType["CLEOPATRA_NEEDLE"]:
			from operator import itemgetter
			CyPlayer = GC.getPlayer(iPlayer)
			iCityID = CyCity.getID()
			aList = []
			CyCityX, i = CyPlayer.firstCity(False)
			while CyCityX:
				if CyCityX.getID() != iCityID:
					aList.append((CyCityX, CyCityX.getCulture(iPlayer)))
				CyCityX, i = CyPlayer.nextCity(i, False)
			if aList:
				# Sort by descending culture
				aList.sort(key=itemgetter(1), reverse=True)
				for i, entry in enumerate(aList):
					entry[0].setNumRealBuilding(iBuilding, 1)
					if i == 1: # Max. 2 other cities will get the needle for free
						break
		# NANITE DEFUSER - destroyes all nukes from all players
		elif iBuilding == mapBuildingType["NANITE_DEFUSER"]:
			CyPlayer = GC.getPlayer(iPlayer)
			for iPlayerX in xrange(self.MAX_PLAYERS):
				CyUnit, i = CyPlayer.firstUnit(False)
				while CyUnit:
					if CyUnit.isNone() or CyUnit.isDead():
						print "CvEventManager\onBuildingBuilt", ("CyUnit.isDead()", CyUnit.isDead()), ("CyUnit.isNone()", CyUnit.isNone())
					elif CyUnit.nukeRange() > -1:
						CyUnit.kill(0, -1)
					CyUnit, i = CyPlayer.nextUnit(i, False)
				# Global message
				iPlayerAct = GAME.getActivePlayer()
				if iPlayerAct > -1:
					CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_NANITE_DEFUSER_PYTHON",()), iPlayerAct, 24, 'art/interface/buttons/great_wonders/nanite_defuser.dds', ColorTypes(44), bForce=False)
		# Machu Picchu
		elif iBuilding == mapBuildingType["MACHU_PICCHU"]:
			iImprovement = GC.getInfoTypeForString("IMPROVEMENT_MACHU_PICCHU")
			if iImprovement > -1:
				MAP = GC.getMap()
				X = CyCity.getX()
				Y = CyCity.getY()
				aList = []
				iCount = -1
				for x in xrange(X - 3, X + 4):
					for y in xrange(Y - 3, Y + 4):
						CyPlot = MAP.plot(x, y)
						if CyPlot.isPeak():
							x = CyPlot.getX()
							y = CyPlot.getY()
							G.setPlotExtraYield(x, y, YieldTypes.YIELD_FOOD, 1)
							G.setPlotExtraYield(x, y, YieldTypes.YIELD_PRODUCTION, 2)
							G.setPlotExtraYield(x, y, YieldTypes.YIELD_COMMERCE, 1)
							aList.append(CyPlot)
							iCount += 1
				if aList:
					CyPlot = aList[G.getSorenRandNum(iCount, "Random Peak")]
					CyPlot.setImprovementType(iImprovement)
				else:
					print ("Warning CvEventManager.onBuildingBuilt\n\tMachu Picchu has been built in %s where there is no peaks in vicinity." % CyCity.getName())
			else:
				print "Warning CvEventManager.onBuildingBuilt\n\tIMPROVEMENT_MACHU_PICCHU doesn't exist"
		# Field of the Cloth of Gold
		elif iBuilding == mapBuildingType["FIELD_GOLD"]:
			iTeam = GC.getPlayer(iPlayer).getTeam()
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					iTeamX = CyPlayerX.getTeam()
					if iTeam != iTeamX and GC.getTeam(iTeamX).isHasMet(iTeam):
						CyPlayerX.AI_changeAttitudeExtra(iTeam, 3)
		# Maginot Line
		elif iBuilding == mapBuildingType["MAGINOTLINE"]:
			iBunker = GC.getInfoTypeForString("IMPROVEMENT_BUNKER")
			if iBunker > -1:
				MAP = GC.getMap()
				iArea = CyCity.plot().getArea()
				iGridX = MAP.getGridWidth()
				iGridY = MAP.getGridHeight()
				bWrapX = MAP.isWrapX()
				bWrapY = MAP.isWrapY()

				bOK = False
				for i in xrange(MAP.numPlots()):
					if bOK: # No adjacent bunkers
						bOK = False
						continue
					CyPlot = MAP.plotByIndex(i)
					if CyPlot.getOwner() == iPlayer and CyPlot.getArea() == iArea and CyPlot.getImprovementType() != iBunker:
						x = CyPlot.getX()
						y = CyPlot.getY()
						iEast = x + 1
						if iEast > iGridX:
							if bWrapX:
								iEast = 0
							else:
								iEast = x
						iNorth = y + 1
						if iNorth > iGridY:
							if bWrapY:
								iNorth = 0
							else:
								iNorth = y
						iWest = x - 1
						if iWest < 0:
							if bWrapX:
								iWest = iGridX
							else:
								iWest = 0
						iSouth = y - 1
						if iSouth < 0:
							if bWrapY:
								iSouth = iGridY
							else:
								iSouth = 0
							iSouth += iGridY
						aCyPlotList = [
							MAP.plot(x, iNorth),		MAP.plot(x, iSouth),
							MAP.plot(iWest, y),			MAP.plot(iEast, y),
							MAP.plot(iWest, iNorth),	MAP.plot(iEast, iNorth),
							MAP.plot(iWest, iSouth),	MAP.plot(iEast, iSouth)
						]

						for CyPlotX in aCyPlotList:
							if CyPlotX.getArea() == iArea and CyPlotX.getOwner() != iPlayer:
								break # A frontier.
						else: # Not a frontier.
							continue

						for CyPlotX in aCyPlotList:
							if CyPlotX.getOwner() == iPlayer and CyPlotX.getImprovementType() == iBunker:
								break # No adjacent bunkers
						else: # New Bunker
							bOK = True
							CyPlot.setImprovementType(iBunker)
			else:
				print "Warning CvEventManager.onBuildingBuilt\n\tIMPROVEMENT_BUNKER doesn't exist"
		# Silk Road
		elif iBuilding == mapBuildingType["SILK_ROAD"]:
			iUnit = GC.getInfoTypeForString("UNIT_SPY")
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_SPY doesn't exist, aborting python effect for the silk road"
				return
			MAP = GC.getMap()
			CyPlot = CyCity.plot()
			iArea = CyPlot.getArea()
			CyPlot0 = CyPlot1 = GC.getPlayer(iPlayer).getCapitalCity().plot()

			iSilk = GC.getInfoTypeForString("BONUS_SILK")
			iSilkFarm = GC.getInfoTypeForString("IMPROVEMENT_SILK_FARM")
			iTrail = GC.getInfoTypeForString("ROUTE_TRAIL")
			iPath = GC.getInfoTypeForString("ROUTE_PATH")
			iRoute = GC.getInfoTypeForString("ROUTE_ROAD")
			if iRoute < 0:
				if iPath < 0:
					if iTrail < 0:
						print "Error CvEventManager.onBuildingBuilt\n\tROUTE_ROAD, ROUTE_PATH, and ROUTE_TRAIL doesn't exist, aborting python effect for the silk road"
						return
					else:
						iRoute = iTrail
						iTrail = -1
						print "Warning CvEventManager.onBuildingBuilt\n\tROUTE_ROAD and ROUTE_PATH doesn't exist."
				else:
					iRoute = iPath
					iPath = -1
					print "Warning CvEventManager.onBuildingBuilt\n\tROUTE_ROAD doesn't exist."
			if iTrail < 0:
				print "Warning CvEventManager.onBuildingBuilt\n\tROUTE_TRAIL doesn't exist."

			iThePopu = 0
			iThePath = 0
			for i in xrange(MAP.numPlots()):
				CyPlotZ = MAP.plotByIndex(i)
				if CyPlotZ.getArea() == iArea:
					iOwner = CyPlotZ.getOwner()
					if iOwner == iPlayer and iSilk > -1 and CyPlotZ.getBonusType(-1) == iSilk:
						if MAP.generatePathForHypotheticalUnit(CyPlot, CyPlotZ, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
							# Owned silk bonuses to the city that built the silk road.
							if iSilkFarm > -1:
								CyPlotZ.setImprovementType(iSilkFarm)
							for k in xrange(MAP.getLastPathStepNum()):
								CyPlotDo = MAP.getLastPathPlotByIndex(k)
								if CyPlotDo.getRouteType() in (-1, iTrail, iPath):
									CyPlotDo.setRouteType(iRoute)
					if CyPlotZ.isCity() == 1 and iOwner != iPlayer:
						if MAP.generatePathForHypotheticalUnit(CyPlot0, CyPlotZ, iPlayer, iUnit, PathingFlags.MOVE_IGNORE_DANGER+PathingFlags.MOVE_THROUGH_ENEMY, 1000):
							CyCityZ = CyPlotZ.getPlotCity()
							iPopu = CyCityZ.getPopulation()
							iPath = MAP.getLastPathStepNum()
							if iPopu > iThePopu or (iPath > iThePath and iPopu > iThePopu - iThePopu / 8):
								CyPlot1 = CyPlotZ
								if iPopu > iThePopu:
									iThePopu = iPopu
								iThePath = iPath
			# Capital to the city that built the silk road.
			if MAP.generatePathForHypotheticalUnit(CyPlot0, CyPlot, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
				for k in xrange(MAP.getLastPathStepNum()):
					CyPlotDo = MAP.getLastPathPlotByIndex(k)
					if CyPlotDo.getRouteType() in (-1, iTrail, iPath):
						CyPlotDo.setRouteType(iRoute)
			# Capital to a foreign metropolis.
			if MAP.generatePathForHypotheticalUnit(CyPlot0, CyPlot1, iPlayer, iUnit, PathingFlags.MOVE_IGNORE_DANGER+PathingFlags.MOVE_THROUGH_ENEMY, 1000):
				for k in xrange(MAP.getLastPathStepNum()):
					CyPlotDo = MAP.getLastPathPlotByIndex(k)
					if CyPlotDo.getRouteType() in (-1, iTrail, iPath):
						CyPlotDo.setRouteType(iRoute)
		# Route66
		elif iBuilding == mapBuildingType["ROUTE_66"]:
			iUnit = GC.getInfoTypeForString("UNIT_WORKER")
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_WORKER doesn't exist, aborting python effect for Route 66"
				return
			iCityID = CyCity.getID()
			MAP = GC.getMap()
			CyPlayer = GC.getPlayer(iPlayer)
			CyPlot = CyCity.plot()
			iAreaID = CyCity.area().getID()

			iThePopu = 0
			iThePath = 0
			CyCityX, i = CyPlayer.firstCity(False)
			while CyCityX:
				if CyCityX.area().getID() == iAreaID and CyCityX.getID() != iCityID:
					CyPlotX = CyCityX.plot()
					if MAP.generatePathForHypotheticalUnit(CyPlot, CyPlotX, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						iPopu = CyCityX.getPopulation()
						iPath = MAP.getLastPathStepNum()
						if iPopu > iThePopu or (iPath > iThePath and iPopu > iThePopu - iThePopu / 8):
							CyCityDo = CyCityX
							CyPlotDo = CyPlotX
							iThePath = iPath
							iThePopu = iPopu
				CyCityX, i = CyPlayer.nextCity(i, False)

			if MAP.generatePathForHypotheticalUnit(CyPlot, CyPlotDo, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
				iBuilding = GC.getInfoTypeForString("BUILDING_ROUTE_66_TERMINUS")
				if iBuilding > -1:
					CyCityDo.setNumRealBuilding(iBuilding, 1)
				iRoute = GC.getInfoTypeForString("ROUTE_MODERN_ROAD")
				if iRoute > -1:
					for k in xrange(MAP.getLastPathStepNum()):
						pRoutePlot = MAP.getLastPathPlotByIndex(k)
						pRoutePlot.setRouteType(iRoute)
		# Appian Way
		elif iBuilding == mapBuildingType["APPIAN_WAY"]:
			iUnit = GC.getInfoTypeForString("UNIT_WORKER")
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_WORKER doesn't exist, aborting python effect for Appian Way"
				return
			iRoute = GC.getInfoTypeForString("ROUTE_PAVED")
			if iRoute < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tROUTE_PAVED doesn't exist, aborting python effect for Appian Way"
				return
			if iPlayer == GAME.getActivePlayer():
				CvUtil.sendMessage(TRNSLTR.getText("TXT_APPIAN_BUILT",()), iPlayer)
			MAP = GC.getMap()
			CyPlayer = GC.getPlayer(iPlayer)

			# The appian city
			iCityID = CyCity.getID()
			iAreaID = CyCity.area().getID()

			# Find start-point and cache city info
			aCityList = []
			iMaxPath2Appia = 0
			iCities = 0
			CyCityStart = None
			CyCityX, i = CyPlayer.firstCity(False)
			while CyCityX:
				if CyCityX.getID() != iCityID and CyCityX.area().getID() == iAreaID:
					if MAP.generatePathForHypotheticalUnit(CyCity.plot(), CyCityX.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999):
						iPath2Appia = MAP.getLastPathStepNum()
						if iPath2Appia > iMaxPath2Appia:
							iMaxPath2Appia = iPath2Appia
							CyCityStart = CyCityX
							iIdx = iCities
						aCityList.append((CyCityX, iPath2Appia))
						iCities += 1
				CyCityX, i = CyPlayer.nextCity(i, False)

			if CyCityStart:
				aCityList.pop(iIdx)
				iCities -= 1
				if iCities:
					# Find end-point
					iMaxPath = 0
					i = 0
					while i < iCities:
						CyCityX = aCityList[i][0]
						MAP.generatePathForHypotheticalUnit(CyCityStart.plot(), CyCityX.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
						iPath = MAP.getLastPathStepNum()
						if iPath > iMaxPath:
							iMaxPath = iPath
							CyCityEnd = CyCityX
							iIdx = i
						i += 1
					aCityList.pop(iIdx)
					iCities -= 1
					MAP.generatePathForHypotheticalUnit(CyCityEnd.plot(), CyCity.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
					iOtherPath2Appia = MAP.getLastPathStepNum()
					aCityList0 = list(aCityList)
					iCities0 = iCities
				else:
					iOtherPath2Appia = 0

				# Build Appian way
				CyCityFrom = CyCityStart
				while True:
					# Find closest city that is not further away from the appian city.
					iMinPath = 0
					i = 0
					while i < iCities:
						CyCityX, iPath2Appia = aCityList[i]
						if iPath2Appia <= iMaxPath2Appia:
							MAP.generatePathForHypotheticalUnit(CyCityFrom.plot(), CyCityX.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
							iPath = MAP.getLastPathStepNum()
							if not iMinPath or iPath < iMinPath:
								iMinPath = iPath
								iIdx = i
						i += 1
					if iMinPath:
						# Connect two cities on the way to appia
						CyCityTo, iPath2Appia = aCityList.pop(iIdx)
						iCities -= 1
						MAP.generatePathForHypotheticalUnit(CyCityFrom.plot(), CyCityTo.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
						for j in xrange(iMinPath):
							MAP.getLastPathPlotByIndex(j).setRouteType(iRoute)
						CyCityFrom = CyCityTo
						iMaxPath2Appia = iPath2Appia
					else:
						# Connect to the appian city
						MAP.generatePathForHypotheticalUnit(CyCityFrom.plot(), CyCity.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
						for j in xrange(iMaxPath2Appia):
							MAP.getLastPathPlotByIndex(j).setRouteType(iRoute)
						if iOtherPath2Appia:
							CyCityFrom = CyCityEnd
							iMaxPath2Appia = iOtherPath2Appia
							iOtherPath2Appia = 0
							iCities = iCities0
							aCityList = aCityList0
						else:
							break # All done

			else: # The wonder should do something in this case.
				# Let's fill out the fat cross of the city with the route.
				for i in xrange(GC.getNUM_CITY_PLOTS()):
					CyPlot = CyCity.getCityIndexPlot(i)
					if CyPlot and CyPlot.canBuild(iRoute, iPlayer, False):
						CyPlot.setRouteType(iRoute)
		# Golden Spike
		elif iBuilding == mapBuildingType["GOLDEN_SPIKE"]:
			iUnit = GC.getInfoTypeForString("UNIT_WORKER_INDUSTRIAL")
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_WORKER_INDUSTRIAL doesn't exist, aborting python effect for Golden Spike"
				return
			iRoute = GC.getInfoTypeForString("ROUTE_RAILROAD")
			if iRoute < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tROUTE_RAILROAD doesn't exist, aborting python effect for Golden Spike"
				return
			MAP = GC.getMap()
			iX = CyCity.getX()
			iY = CyCity.getY()
			iMaxWestX = iMaxEastX = iX
			iMaxWestY = iMaxEastY = iY
			CyPlot = MAP.plot(iX, iY)
			iArea = CyPlot.getArea()
			pMaxWest = pMaxEast = CyPlot

			if iPlayer == GAME.getActivePlayer():
				CvUtil.sendMessage(TRNSLTR.getText("TXT_GOLDEN_SPIKE_BUILT",()), iPlayer)

			iGridWidth = MAP.getGridWidth()
			iGridHeight = MAP.getGridHeight()

			iPass = 0
			for x in xrange(iGridWidth):
				for y in xrange(iGridHeight):
					pWest = MAP.plot(x, y)
					pNext = MAP.plot(x+1, y)
					if x == (iGridWidth - 1):
						pNext = MAP.plot(0, y)
					if pWest.getArea() == iArea and pNext.getArea() == iArea and MAP.generatePathForHypotheticalUnit(CyPlot, pWest, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000) and MAP.generatePathForHypotheticalUnit(CyPlot, pNext, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						iPass += 1
						break
			if iPass == iGridWidth:
				iYParam1 = 100000
				iYParam2 = 100000
				iYParam3 = 100000
				iOffset = int(iGridWidth / 4)
				iXOff1 = iX - 3 * iOffset
				if iXOff1 < 0:
					iXOff1 = iX + iOffset
				iXOff2 = iX - 2 * iOffset
				if iXOff2 < 0:
					iXOff2 = iX + 2 * iOffset
				iXOff3 = iX - iOffset
				if iXOff3 < 0:
					iXOff3 = iX + 3 * iOffset
				for i in xrange(iGridHeight):
					pCheck1 = MAP.plot(iXOff1, i)
					pCheck2 = MAP.plot(iXOff2, i)
					pCheck3 = MAP.plot(iXOff3, i)
					if MAP.generatePathForHypotheticalUnit(CyPlot, pCheck1, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						if pCheck1.getArea() == iArea and abs(pCheck1.getY() - iY) <= abs(iYParam1 - iY):
							iYParam1 = i
					if MAP.generatePathForHypotheticalUnit(pCheck1, pCheck2, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						if pCheck2.getArea() == iArea and abs(pCheck2.getY() - iY) <= abs(iYParam2 - iY):
							iYParam2 = i
					if MAP.generatePathForHypotheticalUnit(pCheck2, pCheck3, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						if pCheck3.getArea() == iArea and abs(pCheck3.getY() - iY) <= abs(iYParam3 - iY):
							iYParam3 = i
					pCheckPoint1 = MAP.plot(iXOff1, iYParam1)
					pCheckPoint2 = MAP.plot(iXOff2, iYParam2)
					pCheckPoint3 = MAP.plot(iXOff3, iYParam3)
					if MAP.generatePathForHypotheticalUnit(CyPlot, pCheckPoint1, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						for i in xrange(MAP.getLastPathStepNum()):
							CyPlotZ = MAP.getLastPathPlotByIndex(i)
							CyPlotZ.setRouteType(iRoute)
					if MAP.generatePathForHypotheticalUnit(pCheckPoint1, pCheckPoint2, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						for i in xrange(MAP.getLastPathStepNum()):
							CyPlotZ = MAP.getLastPathPlotByIndex(i)
							CyPlotZ.setRouteType(iRoute)
					if MAP.generatePathForHypotheticalUnit(pCheckPoint2, pCheckPoint3, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						for i in xrange(MAP.getLastPathStepNum()):
							CyPlotZ = MAP.getLastPathPlotByIndex(i)
							CyPlotZ.setRouteType(iRoute)
					if MAP.generatePathForHypotheticalUnit(pCheckPoint3, CyPlot, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						for i in xrange(MAP.getLastPathStepNum()):
							CyPlotZ = MAP.getLastPathPlotByIndex(i)
							CyPlotZ.setRouteType(iRoute)

			elif iPass < iGridWidth:
				for i in xrange(MAP.numPlots()):
					CyPlotZ = MAP.plotByIndex(i)
					if CyPlotZ.getArea() == iArea:
						if CyPlotZ.getOwner() == iPlayer or not CyPlotZ.isOwned():
							if MAP.generatePathForHypotheticalUnit(CyPlotZ, CyPlot, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
								pCheckPlot = MAP.getLastPathPlotByIndex(1)
								iCheckX = pCheckPlot.getX()
								x = CyPlotZ.getX()
								y = CyPlotZ.getY()
								if iCheckX == x + 1 or (x == iGridWidth - 1 and not iCheckX) or x == iX - 1:
									if (x < iMaxWestX and x < iX and iMaxWestX <= iX) or (x - iGridWidth < iMaxWestX and iMaxWestX <= iX and x > iX) or (x < iMaxWestX and x > iX and iMaxWestX > iX):
										if x < iMaxWestX or (x > iX and iMaxWestX <= iX) or (x == iMaxWestX and abs(y - iY) <= abs(iMaxWestY - iY)):
											iMaxWestX = x
											iMaxWestY = y
											pMaxWest = CyPlotZ
								if iCheckX == x - 1 or (iCheckX == iGridWidth - 1 and not x) or x == iX + 1:
									if (x > iMaxEastX and x > iX and iMaxEastX >= iX) or (x + iGridWidth > iMaxEastX and iMaxEastX >= iX and x < iX) or (x > iMaxEastX and x < iX and iMaxEastX < iX):
										if x > iMaxEastX or (x < iX and iMaxEastX >= iX) or (x == iMaxEastX and abs(y - iY) <= abs(iMaxEastY - iY)):
											iMaxEastX = x
											iMaxEastY = y
											pMaxEast = CyPlotZ
				if MAP.generatePathForHypotheticalUnit(CyPlot, pMaxWest, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
					for i in xrange(MAP.getLastPathStepNum()):
						CyPlotZ = MAP.getLastPathPlotByIndex(i)
						CyPlotZ.setRouteType(iRoute)
				if MAP.generatePathForHypotheticalUnit(CyPlot, pMaxEast, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
					for i in xrange(MAP.getLastPathStepNum()):
						CyPlotZ = MAP.getLastPathPlotByIndex(i)
						CyPlotZ.setRouteType(iRoute)


	def onProjectBuilt(self, argsList):
		CyCity, iProject = argsList
		iPlayer = CyCity.getOwner()
		iPlayerAct = GAME.getActivePlayer()
		# Movie
		if not self.bNetworkMP and iPlayer == iPlayerAct:
			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
			popupInfo.setData1(iProject)
			popupInfo.setData2(CyCity.getID())
			popupInfo.setData3(2)
			popupInfo.setText('showWonderMovie')
			popupInfo.addPopup(iPlayer)

		# Eden project
		if iProject == GC.getInfoTypeForString("PROJECT_EDEN"):

			iTeam = GC.getPlayer(iPlayer).getTeam()
			iX = CyCity.getX()
			iY = CyCity.getY()
			DESERT		= GC.getInfoTypeForString('TERRAIN_DESERT')
			PLAINS		= GC.getInfoTypeForString('TERRAIN_PLAINS')
			GRASS		= GC.getInfoTypeForString('TERRAIN_GRASS')
			TAIGA		= GC.getInfoTypeForString('TERRAIN_TAIGA')
			TUNDRA		= GC.getInfoTypeForString('TERRAIN_TUNDRA')
			PERMAFROST	= GC.getInfoTypeForString('TERRAIN_PERMAFROST')
			ICE			= GC.getInfoTypeForString('TERRAIN_ICE')
			MARSH		= GC.getInfoTypeForString('TERRAIN_MARSH')

			A = GC.getInfoTypeForString('IMPROVEMENT_FARM')
			B = GC.getInfoTypeForString('IMPROVEMENT_VERTICAL_FARM')
			C = GC.getInfoTypeForString('IMPROVEMENT_WINDMILL')
			D = GC.getInfoTypeForString('IMPROVEMENT_PLANTATION')
			E = GC.getInfoTypeForString('IMPROVEMENT_OLIVE_FARM')
			F = GC.getInfoTypeForString('IMPROVEMENT_APPLE_FARM')
			G = GC.getInfoTypeForString('IMPROVEMENT_WINERY')
			H = GC.getInfoTypeForString('IMPROVEMENT_COTTAGE')
			I = GC.getInfoTypeForString('IMPROVEMENT_HAMLET')
			J = GC.getInfoTypeForString('IMPROVEMENT_VILLAGE')
			K = GC.getInfoTypeForString('IMPROVEMENT_TOWN')
			L = GC.getInfoTypeForString('IMPROVEMENT_FOREST_PRESERVE')

			MAP = GC.getMap()
			for x in xrange(iX - 50, iX + 50, 1):
				for y in xrange(iY - 50, iY + 50, 1):
					CyPlot = MAP.plot(x, y)
					if CyPlot.getTeam() == iTeam:
						iTerrain = CyPlot.getTerrainType()
						if iTerrain == GRASS:
							i = CyPlot.getImprovementType()
							if i > -1 and i in (A, B, C, D, E, F, G, H, I, J, K, L):
								continue
							if CyPlot.getFeatureType() != GC.getInfoTypeForString('FEATURE_JUNGLE'):
								CyPlot.setFeatureType(GC.getInfoTypeForString("FEATURE_FOREST"), 1)
						elif iTerrain == PLAINS:
							CyPlot.setTerrainType(GRASS, 1, 1)
						elif iTerrain == MARSH:
							CyPlot.setTerrainType(GRASS, 1, 1)
						elif iTerrain == TAIGA:
							CyPlot.setTerrainType(PLAINS, 1, 1)
						elif iTerrain == TUNDRA:
							CyPlot.setTerrainType(TAIGA, 1, 1)
						elif iTerrain == PERMAFROST:
							CyPlot.setTerrainType(TUNDRA, 1, 1)
						elif iTerrain == ICE:
							CyPlot.setTerrainType(PERMAFROST, 1, 1)
						elif iTerrain == DESERT:
							CyPlot.setTerrainType(PLAINS, 1, 1)
			# Team Message
			if iTeam == GAME.getActiveTeam():
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_EDEN_PYTHON",()), iPlayerAct, 16, 'Art/Interface/Buttons/Buildings/Eden.dds', ColorTypes(44), iX, iY, True, True, bForce=False)


	'''
	def onSelectionGroupPushMission(self, argsList): # AI never trigger this.
		iPlayer, iMission, iNumUnits, lUnitIDs = argsList


	def onUnitMove(self, argsList):
		pPlot, pUnit, pOldPlot = argsList
	'''


	''' Disabled in PythonCallbackDefines.xml (USE_ON_UNIT_SET_XY_CALLBACK = False)
	def onUnitSetXY(self, argsList):
		pPlot, pUnit = argsList
	'''


	def onUnitCreated(self, argsList): # Enabled in PythonCallbackDefines.xml (USE_ON_UNIT_CREATED_CALLBACK = True)
		CyUnit, = argsList
		iPlayer = CyUnit.getOwner()
		CvUnitInfo = GC.getUnitInfo(CyUnit.getUnitType())
		KEY = CvUnitInfo.getType()

		# Star Signs
		if CyUnit.isNPC() and not GAME.getSorenRandNum(100, "Random to get a Sign"):
			# 1% chance of getting a "sign promotion"
			import StarSigns
			StarSigns.give(GC, TRNSLTR, GAME, CyUnit, GC.getPlayer(iPlayer))

		# Beastmaster
		if self.UNIT_FEMALE_BEASTMASTER != -1 or self.UNIT_BEASTMASTER != -1:
			if KEY[:13] == 'UNIT_SUBDUED_' or KEY[:11] == 'UNIT_TAMED_':
				if self.UNIT_FEMALE_BEASTMASTER != -1 and self.UNIT_BEASTMASTER != -1:
					if 16 > GAME.getSorenRandNum(100, "Female Beastmaster"):
						CyUnit.setLeaderUnitType(self.UNIT_FEMALE_BEASTMASTER)
					else:
						CyUnit.setLeaderUnitType(self.UNIT_BEASTMASTER)
				elif self.UNIT_FEMALE_BEASTMASTER != -1:
					CyUnit.setLeaderUnitType(self.UNIT_FEMALE_BEASTMASTER)
				else:
					CyUnit.setLeaderUnitType(self.UNIT_BEASTMASTER)

		# Inspired Missionary
		aWonderTuple = self.aWonderTuple
		if "FA_MEN_SI" in aWonderTuple[0]:
			if iPlayer == aWonderTuple[4][aWonderTuple[0].index("FA_MEN_SI")]:
				if CvUnitInfo.getPrereqReligion() > -1:
					CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_FA_MEN_SI_INSPIRED"), True)


	def onUnitBuilt(self, argsList):
		CyCity, CyUnit = argsList
		CvAdvisorUtils.unitBuiltFeats(CyCity, CyUnit)
		CyPlayer = GC.getPlayer(CyUnit.getOwner())
		iUnit = CyUnit.getUnitType()

		# Star Signs
		if not CyUnit.isNPC() and not GAME.getSorenRandNum(100, "Random to get a Sign"):
			# 1% chance of getting a "sign promotion"
			import StarSigns
			StarSigns.give(GC, TRNSLTR, GAME, CyUnit, CyPlayer)

		# Technocracy - Free Promotion
		if CyPlayer.getCivics(self.CIVICOPTION_POWER) == self.CIVIC_TECHNOCRACY:
			CyTeam = GC.getTeam(CyPlayer.getTeam())
			if CyTeam.isHasTech(self.TECH_SMART_DUST):
				iSensors = self.PROMOTION_SENSORS
				if CyUnit.isPromotionValid(iSensors):
					CyUnit.setHasPromotion(iSensors, True)

		# Immigration Mod
		if iUnit == self.UNIT_IMMIGRANT:
			CyCity.setPopulation(CyCity.getPopulation() - 2)
			import Immigration
			CyCityX = Immigration.getLeastPopulatedCity(CyPlayer)
			if CyCityX:
				Immigration.doImmigrantPlacementAI(CyUnit, CyCityX)


	def onUnitKilled(self, argsList):
		CyUnit, iAttacker = argsList # iAttacker is a PlayerType
		# Player who lost the unit
		iPlayerL = CyUnit.getOwner()

		aWonderTuple = self.aWonderTuple
		if iPlayerL in aWonderTuple[4]:
			CyPlayerL = None
			for i, iPlayerX in enumerate(aWonderTuple[4]):
				if iPlayerX != iPlayerL: continue
				KEY = aWonderTuple[0][i]
				if KEY == "ALAMO":
					if CyPlayerL is None:
						CyPlayerL = GC.getPlayer(iPlayerL)
					iValue = CyUnit.baseCombatStr()
					CyCity, i = CyPlayerL.firstCity(False)
					while CyCity:
						CyCity.changeCulture(iPlayerL, iValue, False)
						CyCity, i = CyPlayerL.nextCity(i, False)
				elif KEY == "CYRUS_TOMB":
					if CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_LEADER")) or CyUnit.getUnitType() == GC.getInfoTypeForString("UNIT_GREAT_GENERAL"):
						if CyPlayerL is None:
							CyPlayerL = GC.getPlayer(iPlayerL)
						CyCity = CyPlayerL.getCity(aWonderTuple[3][i])
						iX = CyCity.getX()
						iY = CyCity.getY()
						szName = CyUnit.getNameNoDesc()
						CyUnitNew = CyPlayerL.initUnit(GC.getInfoTypeForString("UNIT_GREAT_GENERAL"), iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
						CyUnitNew.setName(szName)
						CyUnitNew.setImmobileTimer(8)
						# Message
						if iPlayerL == GAME.getActivePlayer():
							CvUtil.sendMessage(TRNSLTR.getText("TXT_GG_REVIVE", (szName,)), iPlayerL, 16, 'Art/Cyrus_Tomb/cyrustomb_button.dds', ColorTypes(11), iX, iY, True, True, bForce=False)

		# Beastmaster
		iLeaderUnit = CyUnit.getLeaderUnitType()
		if iLeaderUnit != -1 and iLeaderUnit in (self.UNIT_BEASTMASTER, self.UNIT_FEMALE_BEASTMASTER):
			# This will prevent a 'beastmaster lost' message when the unit is killed.
			CyUnit.setLeaderUnitType(GC.getInfoTypeForString("NO_UNIT"))


	''' Disabled in PythonCallbackDefines.xml (USE_ON_UNIT_LOST_CALLBACK = False)
	def onUnitLost(self, argsList):
		CyUnit, = argsList
	'''


	def onUnitPromoted(self, argsList):
		CyUnit, iPromotion = argsList
		CyPlayer = GC.getPlayer(CyUnit.getOwner())

		# AI promotion redirection
		if not CyPlayer.isHuman():
			aHiPriList = ('PROMOTION_LIVE1', 'PROMOTION_LIVE2')
			iDomainType = CyUnit.getDomainType()
			mapDomain = self.mapDomain
			if iDomainType == mapDomain['DOMAIN_LAND']:
				if CyUnit.isHasUnitCombat(GC.getInfoTypeForString("UNITCOMBAT_HUNTER")):
					aHiPriList += ("PROMOTION_HUNTER1", "PROMOTION_HUNTER2", "PROMOTION_HUNTER3", "PROMOTION_HUNTER_GREAT")
			elif iDomainType == mapDomain['DOMAIN_SEA']:
				if CyUnit.isHasUnitCombat(GC.getInfoTypeForString("UNITCOMBAT_RECON")):
					aHiPriList += ("PROMOTION_SEA_HUNTER1", "PROMOTION_SEA_HUNTER2", "PROMOTION_SEA_HUNTER3", "PROMOTION_SEA_HUNTER_GREAT")
			aList1 = []
			aList2 = []
			mapPromoType = self.mapPromoType
			for KEY, iPromo in mapPromoType.iteritems():
				if iPromo < 0: continue
				# Assign chance
				if KEY in aHiPriList:
					if iPromo == iPromotion:
						aList1 = []
						break
					aList1.append((iPromo, 0))
				elif KEY in ('PROMOTION_SNEAK', 'PROMOTION_MARAUDER', 'PROMOTION_INDUSTRYESPIONAGE'):
					if iPromo == iPromotion:
						aList2 = []
						break
					aList2.append((iPromo, 4))
			# Prioritize promos
			aList = []
			if aList1:
				aList = aList1
				if aList2:
					aList += aList2

			for iPromo, iChance in aList:
				if CyUnit.canAcquirePromotion(iPromo):
					if iChance and GAME.getSorenRandNum(iChance, "Generic"): continue
					CyUnit.setHasPromotion(iPromotion, False)
					CyUnit.setHasPromotion(iPromo, True)
					break



	''' Disabled in PythonCallbackDefines.xml (USE_ON_UNIT_SELECTED_CALLBACK = False)
	def onUnitSelected(self, argsList):
		print ("onUnitSelected", argsList)
		CyUnit = argsList[0]
	'''


	def onUnitRename(self, argsList):
		CyUnit, = argsList
		if CyUnit.getOwner() == GAME.getActivePlayer():
			self.__eventEditUnitNameBegin(CyUnit)


	'''
	def onUnitPillage(self, argsList):
		pUnit, iImprovement, iRoute, iOwner = argsList
	'''


	def onUnitSpreadReligionAttempt(self, argsList):
		CyUnit, iReligion, bSuccess = argsList
		if not bSuccess:
			iPlayer = CyUnit.getOwner()
			aWonderTuple = self.aWonderTuple
			if "FA_MEN_SI" in aWonderTuple[0]:
				if iPlayer == aWonderTuple[4][aWonderTuple[0].index("FA_MEN_SI")]:
					CyCity = GC.getMap().plot(CyUnit.getX(), CyUnit.getY()).getPlotCity()
					CyCity.setHasReligion(GC.getUnitInfo(CyUnit.getUnitType()).getPrereqReligion(), True, True, True)


	'''
	def onUnitGifted(self, argsList):
		pUnit, iGiftingPlayer, pPlotLocation = argsList


	def onUnitBuildImprovement(self, argsList):
		# Improvements and Routes
		pUnit, iBuild, bFinished = argsList


	def onGoodyReceived(self, argsList):
		iPlayer, pPlot, pUnit, iGoodyType = argsList
	'''


	def onGreatPersonBorn(self, argsList):
		CyUnit, iPlayer, CyCity = argsList

		aWonderTuple = self.aWonderTuple
		if "LASCAUX" in aWonderTuple[0]:
			if iPlayer == aWonderTuple[4][aWonderTuple[0].index("LASCAUX")]:
				CyCity.changeCulture(iPlayer, CyCity.getCultureThreshold() /10, True)
				iCount = CyCity.getAddedFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_GREAT_ARTIST"))
				for i in xrange(iCount):
					CyCity.changeCulture(iPlayer, CyCity.getCultureThreshold() / 20, True)


	def onTechAcquired(self, argsList):
		iTech, iTeam, iPlayer, bAnnounce = argsList
		# iPlayer may be NULL (-1)
		if not iPlayer > -1: return

		CyPlayer = GC.getPlayer(iPlayer)

		if CyPlayer.isNPC() or not GAME.isFinalInitialized():
			print "%s gained the technology %s" %(CyPlayer.getCivilizationDescription(0), GC.getTechInfo(iTech).getDescriptionForm(0))
			return

		# Show tech splash when applicable
		if bAnnounce and not CyInterface().noTechSplash():
			if iPlayer == GAME.getActivePlayer():
				if not GAME.GetWorldBuilderMode() and not self.bNetworkMP:
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setData1(iTech)
					popupInfo.setText('showTechSplash')
					popupInfo.addPopup(iPlayer)

		# Start as minor civ, become major.
		if self.GO_START_AS_MINORS:
			CyTeam = GC.getTeam(iTeam)

			if CyTeam.isMinorCiv() and CyTeam.isOpenBordersTrading():

				CyTeam.setIsMinorCiv(False, False)
				GC.getMap().verifyUnitValidPlot()
				# Message
				if iPlayer == GAME.getActivePlayer():
					CvUtil.sendMessage(
						TRNSLTR.getText("TXT_KEY_BARBCIV_DISCOVER_WRITING", ()), iPlayer, 16, 
						eColor = GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"),
						eMsgType = InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, bForce = False
					)


		# Free Gatherer from Gathering
		if iTech == self.TECH_GATHERING:
			X = -1
			Y = -1
			CyCity = CyPlayer.getCapitalCity() # This returns a city object even if the capital does not exist.
			# if CyPlayer.getCapitalCity(): # Always True
			if not CyCity.isNone():
				X = CyCity.getX(); Y = CyCity.getY()
			else:
				CyUnit, i = CyPlayer.firstUnit(False)

				if CyUnit:
					X = CyUnit.getX(); Y = CyUnit.getY()
				else:
					CyPlot = CyPlayer.getStartingPlot()
					X = CyPlot.getX(); Y = CyPlot.getY()

			if not -1 in (X, Y):
				if GC.getCivilizationInfo(CyPlayer.getCivilizationType()).getType() == "CIVILIZATION_NEANDERTHAL":
					iWorker = GC.getInfoTypeForString("UNIT_NEANDERTHAL_GATHERER")
				else:
					iWorker = GC.getInfoTypeForString("UNIT_GATHERER")

				CyUnit = CyPlayer.initUnit(iWorker, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

			else: print "Found no valid plot to place the gatherer. \nNew civ from revolution or barb. city perhaps?"

		# Obsolete python building-effects
		aWonderTuple = self.aWonderTuple
		if iTech in aWonderTuple[2]:
			i = aWonderTuple[2].index(iTech)
			if iPlayer == aWonderTuple[4][i]:
				KEY = aWonderTuple[0][i]
				if KEY == "TAIPEI_101":
					for iPlayerX in xrange(self.MAX_PC_PLAYERS):
						if iPlayerX == iPlayer:
							continue
						CyPlayerX = GC.getPlayer(iPlayerX)
						if not CyPlayerX.isAlive() or CyPlayerX.isHuman():
							continue
						CyPlayerX.AI_changeAttitudeExtra(iTeam, -2)
				elif KEY == "TSUKIJI":
					IMP = GC.getInfoTypeForString('IMPROVEMENT_FISHING_BOATS')
					CyTeam = GC.getTeam(iTeam)
					CyTeam.changeImprovementYieldChange(IMP, 0, -1)
					CyTeam.changeImprovementYieldChange(IMP, 2, -1)
				aWonderTuple[4][i] = -1

		# Nazca Lines
		if "NAZCA_LINES" in aWonderTuple[0]:
			i = aWonderTuple[0].index("NAZCA_LINES")
			if iPlayer == aWonderTuple[4][i]:
				CyTeam = GC.getTeam(iTeam)
				bNewEra = True
				for iTechX in xrange(GC.getNumTechInfos()):
					if CyTeam.isHasTech(iTechX) and iTechX != iTech:
						if GC.getTechInfo(iTechX).getEra() >= GC.getTechInfo(iTech).getEra():
							bNewEra = False
							break
				if bNewEra:
					NAZCA_LINES = GC.getInfoTypeForString("BUILDINGCLASS_NAZCA_LINES")
					CyCity = CyPlayer.getCity(aWonderTuple[3][i])
					iRandom = GAME.getSorenRandNum(8, "Nazca")
					if not iRandom:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 0)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 0, iBase + 4)
					elif iRandom == 1:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 1)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 1, iBase + 4)
					elif iRandom == 2:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 2)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 2, iBase + 4)
					elif iRandom == 3:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 3)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 3, iBase + 4)
					elif iRandom == 4:
						iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 0)
						CyCity.setBuildingYieldChange(NAZCA_LINES, 0, iBase + 4)
					elif iRandom == 5:
						iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 1)
						CyCity.setBuildingYieldChange(NAZCA_LINES, 1, iBase + 4)
					elif iRandom == 6:
						iBase = CyCity.getBuildingHappyChange(NAZCA_LINES)
						CyCity.setBuildingHappyChange(NAZCA_LINES, iBase + 2)
					else:
						iBase = CyCity.getBuildingHealthChange(NAZCA_LINES)
						CyCity.setBuildingHealthChange(NAZCA_LINES, iBase + 2)
					if iPlayer == GAME.getActivePlayer():
						CvUtil.sendMessage(TRNSLTR.getText("TXT_NAZCA_LINES",()), iPlayer)


	'''
	def onTechSelected(self, argsList):
		iTech, iPlayer = argsList
	'''


	def onReligionFounded(self, argsList):
		iReligion, iPlayer = argsList
		G = GAME
		if iPlayer == G.getActivePlayer():
			if not G.GetWorldBuilderMode() and not self.bNetworkMP:
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
				popupInfo.setData1(iReligion)
				popupInfo.setData2(G.getHolyCity(iReligion).getID())
				popupInfo.setData3(1)
				popupInfo.setText('showWonderMovie')
				popupInfo.addPopup(iPlayer)
		# Favorite religion
		for iPlayerX in xrange(self.MAX_PC_PLAYERS):
			if iPlayerX == iPlayer: continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive() and iReligion == GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getFavoriteReligion():
				CyPlayerX.getCapitalCity().setHasReligion(iReligion, True, True, True)
				if CyPlayerX.isHuman():
					strReligionName = GC.getReligionInfo(iReligion).getText()
					popup = PyPopup.PyPopup(-1)
					popup.setHeaderString(TRNSLTR.getText("TXT_REAL_FAVORITE_RELIGION_HEADER",()))
					popup.setBodyString(TRNSLTR.getText("TXT_REAL_FAVORITE_RELIGION_TEXT", (strReligionName, strReligionName)))
					popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)


	'''
	def onReligionSpread(self, argsList):
		iReligion, iOwner, pSpreadCity = argsList


	def onReligionRemove(self, argsList):
		iReligion, iOwner, pRemoveCity = argsList


	def onCorporationFounded(self, argsList):
		iCorporation, iFounder = argsList


	def onCorporationSpread(self, argsList):
		iCorporation, iOwner, pSpreadCity = argsList


	def onCorporationRemove(self, argsList):
		iCorporation, iOwner, pRemoveCity = argsList


	def onGoldenAge(self, argsList):
		iPlayer = argsList[0]


	def onEndGoldenAge(self, argsList):
		iPlayer = argsList[0]
	'''


	def onChangeWar(self, argsList):
		bIsWar, iAttacker, iDefender = argsList # iAttacker & iDefender are Teams not Players.
		if bIsWar:
			aWonderTuple = self.aWonderTuple
			if "THE_MOTHERLAND_CALLS" in aWonderTuple[0]:
				iPlayer = aWonderTuple[4][aWonderTuple[0].index("THE_MOTHERLAND_CALLS")]
				if iPlayer > -1:
					CyPlayer = GC.getPlayer(iPlayer)
					if CyPlayer.getTeam() == iDefender:
						CyCity, i = CyPlayer.firstCity(False)
						while CyCity:
							CyCity.changeHappinessTimer(10)
							CyUnit = CyPlayer.initUnit(CyCity.getConscriptUnit(), CyCity.getX(), CyCity.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
							CyCity.addProductionExperience(CyUnit, True)
							CyCity, i = CyPlayer.nextCity(i, False)


	'''
	def onChat(self, argsList):
		chatMessage = "%s" %(argsList[0],)


	def onSetPlayerAlive(self, argsList):
		iPlayer, bNewValue, = argsList

	def onPlayerChangeStateReligion(self, argsList):
		iPlayer, iNewReligion, iOldReligion = argsList


	def onPlayerGoldTrade(self, argsList):
		iFromPlayer, iToPlayer, iGoldAmount = argsList
	'''


	def onCityBuilt(self, argsList):
		CyCity, CyUnit, = argsList
		if not CyCity:
			print "[ERR] CvEventManager.onCityBuilt\n  Not a real city!"
			return
		iPlayer = CyCity.getOwner()
		# Give extra population to new cities
		iPop = 0
		aWonderTuple = self.aWonderTuple
		if "GREAT_BATH" in aWonderTuple[0]:
			if iPlayer == aWonderTuple[4][aWonderTuple[0].index("GREAT_BATH")]:
				iPop += 1
		if CyUnit:
			iUnit = CyUnit.getUnitType()
			if iUnit in self.mapSettlerPop:
				iPop += self.mapSettlerPop[iUnit]

			# Tribal Guardian
			if CyUnit.isHasPromotion(self.PROMO_GUARDIAN_TRIBAL):
				iUnitTG = GC.getInfoTypeForString("UNIT_TRIBAL_GUARDIAN")
				if iUnitTG > -1:
					CyPlayer = GC.getPlayer(iPlayer)
					iExp = CyUnit.getExperience()
					CyUnitTG = CyPlayer.initUnit(iUnitTG, CyUnit.getX(), CyUnit.getY(), UnitAITypes.UNITAI_PROPERTY_CONTROL, DirectionTypes.DIRECTION_SOUTH)
					CyUnitTG.setExperience(iExp, iExp)
		if iPop:
			CyCity.changePopulation(iPop)
			if self.GO_1_CITY_TILE_FOUNDING:
				CyCity.changeFood(CyCity.growthThreshold()/4)
			else:
				CyCity.changeFood(CyCity.growthThreshold()/8)
		# Human player city naming
		iActivePlayer = GAME.getActivePlayer()
		if iPlayer == iActivePlayer and not GAME.getAIAutoPlay(iActivePlayer):
			self.__eventEditCityNameBegin(CyCity, False)


	def onCityRazed(self, argsList):
		CyCity, iPlayer = argsList
		iCityID = CyCity.getID()
		# Handle Python Buildings
		aWonderTuple = self.aWonderTuple
		if iCityID in aWonderTuple[3]:
			n = 0
			temp = list(aWonderTuple[3])
			for i, ID in enumerate(temp):
				if ID != iCityID: continue
				idx = i - n
				KEY = aWonderTuple[0][idx]
				if KEY == "TAIPEI_101":
					CyPlayer = GC.getPlayer(iPlayer)
					if not CyPlayer.isNPC():
						iTeam = CyPlayer.getTeam()
						for iPlayerX in xrange(self.MAX_PC_PLAYERS):
							if iPlayerX == iPlayer:
								continue
							CyPlayerX = GC.getPlayer(iPlayerX)
							if not CyPlayerX.isAlive() or CyPlayerX.isHuman():
								continue
							CyPlayerX.AI_changeAttitudeExtra(iTeam, -2)
				elif KEY == "TSUKIJI":
					CyPlayer = GC.getPlayer(iPlayer)
					IMP = GC.getInfoTypeForString('IMPROVEMENT_FISHING_BOATS')
					CyTeam = GC.getTeam(CyPlayer.getTeam())
					CyTeam.changeImprovementYieldChange(IMP, 0, -1)
					CyTeam.changeImprovementYieldChange(IMP, 2, -1)
				del aWonderTuple[0][idx], aWonderTuple[1][idx], aWonderTuple[2][idx], aWonderTuple[3][idx], aWonderTuple[4][idx]
				n += 1
		# Messages - Wonder Destroyed
		NumWonders = CyCity.getNumWorldWonders()
		if NumWonders:

			if CyCity.isRevealed(GAME.getActiveTeam(), False):
				iActivePlayer = GAME.getActivePlayer()
				if iActivePlayer > -1:
					if iPlayer == iActivePlayer:
						bActive = True
					else:
						bActive = False
					eColor = ColorTypes(GC.getInfoTypeForString("COLOR_RED"))
					szPlayerName = GC.getPlayer(iPlayer).getName()
					iX = CyCity.getX()
					iY = CyCity.getY()
					artPath = 'Art/Interface/Buttons/General/warning_popup.dds'

					for iBuilding in xrange(GC.getNumBuildingInfos()):
						if not CyCity.getNumBuilding(iBuilding): continue
						CvBuildingInfo = GC.getBuildingInfo(iBuilding)
						if GC.getBuildingClassInfo(CvBuildingInfo.getBuildingClassType()).getMaxGlobalInstances() == 1:

							if bActive:
								szTxt = TRNSLTR.getText("TXT_KEY_YOU_DESTROYED_WONDER", (0, CvBuildingInfo.getDescription()))
							else:
								szTxt = TRNSLTR.getText("TXT_KEY_DESTROYED_WONDER", (szPlayerName, CvBuildingInfo.getDescription()))

							CvUtil.sendMessage(szTxt, iActivePlayer, 16, artPath, eColor, iX, iY, True, True, bForce = bActive)
		# Partisans!
		iPlayerHC = CyCity.findHighestCulture()
		if iPlayerHC not in (-1, iPlayer):
			CyPlayerHC = GC.getPlayer(iPlayerHC)
			if not CyPlayerHC.isNPC() and CyPlayerHC.getNumCities() > 0:
				if GC.getTeam(CyPlayerHC.getTeam()).isAtWar(GC.getPlayer(iPlayer).getTeam()):
					TRIGGER = GC.getInfoTypeForString('EVENTTRIGGER_PARTISANS')
					triggerData = CyPlayerHC.initTriggeredData(TRIGGER, true, -1, CyCity.getX(), CyCity.getY(), iPlayer, iCityID, -1, -1, -1, -1)
		# Ruin Arcology.
		mapBuildingType = self.mapBuildingType
		if CyCity.getNumRealBuilding(mapBuildingType["ARCOLOGY"]) or CyCity.getNumRealBuilding(mapBuildingType["ARCOLOGY_SHIELDING"]) or CyCity.getNumRealBuilding(mapBuildingType["ADVANCED_SHIELDING"]):
			self.iArcologyCityID = iCityID
		else:
			self.iArcologyCityID = -1


	def onCityLost(self, argsList):
		CyCity, = argsList
		self.iOldCityID = iCityID = CyCity.getID()
		# Ruin Arcology.
		if self.iArcologyCityID != -1:
			if iCityID == self.iArcologyCityID:
				CyCity.plot().setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS_ARCOLOGY"))
				self.iArcologyCityID = -1


	def onCityAcquired(self, argsList):
		iOwnerOld, iOwnerNew, CyCity, bConquest, bTrade = argsList
		iOldCityID = self.iOldCityID
		iCityID = CyCity.getID()
		aWonderTuple = self.aWonderTuple
		if bConquest:
			if "HELSINKI" in aWonderTuple[0]:
				if iOwnerNew == aWonderTuple[4][aWonderTuple[0].index("HELSINKI")]:
					iX = CyCity.getX()
					iY = CyCity.getY()
					for x in xrange(iX - 1, iX + 2):
						for y in xrange(iY - 1, iY + 2):
							CyPlot = GC.getMap().plot(x, y)
							iCulture = CyPlot.getCulture(iOwnerOld) / 10
							CyPlot.changeCulture(iOwnerOld,-iCulture, True)
							CyPlot.changeCulture(iOwnerNew, iCulture, True)
		if iOldCityID in aWonderTuple[3]:
			iTeamN = GC.getPlayer(iOwnerNew).getTeam()
			CyTeamN = GC.getTeam(iTeamN)
			iTeamO = GC.getPlayer(iOwnerOld).getTeam()
			CyTeamO = GC.getTeam(iTeamO)
			for i, ID in enumerate(aWonderTuple[3]):
				if ID != iOldCityID: continue
				aWonderTuple[3][i] = iCityID
				if -1 == aWonderTuple[4][i]:
					bObsoleteO = True
				else:
					bObsoleteO = False
				iBuilding = aWonderTuple[1][i]
				if CyTeamN.isObsoleteBuilding(iBuilding):
					bObsoleteN = True
					aWonderTuple[4][i] = -1
				else:
					bObsoleteN = False
					aWonderTuple[4][i] = iOwnerNew
				if bObsoleteN and bObsoleteO: continue
				KEY = aWonderTuple[0][i]
				if KEY == "CRUSADE" and not bObsoleteN:
					if CyCity.getBuildingOriginalOwner(iBuilding) != iOwnerNew:
						aWonderTuple[4][i] = -1
				elif KEY == "TAIPEI_101":
					for iPlayerX in xrange(self.MAX_PC_PLAYERS):
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isHuman() or not CyPlayerX.isAlive():
							continue
						if not bObsoleteN and iPlayerX != iOwnerNew:
							CyPlayerX.AI_changeAttitudeExtra(iTeamN, 2)
						if not bObsoleteO and iPlayerX != iOwnerOld:
							CyPlayerX.AI_changeAttitudeExtra(iTeamO, -2)
				elif KEY == "TSUKIJI":
					IMP = GC.getInfoTypeForString('IMPROVEMENT_FISHING_BOATS')
					if not bObsoleteO:
						CyTeamO.changeImprovementYieldChange(IMP, 0, -1)
						CyTeamO.changeImprovementYieldChange(IMP, 2, -1)
					if not bObsoleteN:
						CyTeamN.changeImprovementYieldChange(IMP, 0, 1)
						CyTeamN.changeImprovementYieldChange(IMP, 2, 1)
			self.iOldCityID = None


	def onCityAcquiredAndKept(self, argsList):
		iPlayer, CyCity = argsList
		# Messages - Wonder Captured
		NumWonders = CyCity.getNumWorldWonders()
		if NumWonders:
			if CyCity.isRevealed(GAME.getActiveTeam(), False):
				iActivePlayer = GAME.getActivePlayer()
				if iActivePlayer > -1:
					if iPlayer == iActivePlayer:
						bActive = True
						artPath = 'Art/Interface/Buttons/General/happy_person.dds'
						eColor = ColorTypes(GC.getInfoTypeForString("COLOR_GREEN"))
					else:
						bActive = False
						artPath = 'Art/Interface/Buttons/General/warning_popup.dds'
						eColor = -1

					szPlayerName = GC.getPlayer(iPlayer).getName()
					iX = CyCity.getX()
					iY = CyCity.getY()
					for iBuilding in xrange(GC.getNumBuildingInfos()):
						if CyCity.getNumBuilding(iBuilding):
							CvBuildingInfo = GC.getBuildingInfo(iBuilding)
							if GC.getBuildingClassInfo(CvBuildingInfo.getBuildingClassType()).getMaxGlobalInstances() == 1:

								if bActive:
									szTxt = TRNSLTR.getText("TXT_KEY_YOU_CAPTURED_WONDER", (0, CvBuildingInfo.getDescription()))
								else:
									szTxt = TRNSLTR.getText("TXT_KEY_CAPTURED_WONDER", (szPlayerName, CvBuildingInfo.getDescription()))

								CvUtil.sendMessage(szTxt, iActivePlayer, 16, artPath, eColor, iX, iY, True, True, bForce = bActive)


	'''
	def onCultureExpansion(self, argsList):
		pCity = argsList[0]
		iPlayer = argsList[1]


	def onCityGrowth(self, argsList):
		'City Population Growth'
		pCity = argsList[0]
		iPlayer = argsList[1]
	'''


	def onCityDoTurn(self, argsList):
		CyCity, iPlayer = argsList
		CvAdvisorUtils.cityAdvise(CyCity, iPlayer)
		iCityID = CyCity.getID()
		aWonderTuple = self.aWonderTuple
		if iCityID in aWonderTuple[3] and iPlayer in aWonderTuple[4]:
			for i, ID in enumerate(aWonderTuple[3]):
				if ID != iCityID: continue
				if iPlayer != aWonderTuple[4][i]: continue # Obsolete
				KEY = aWonderTuple[0][i]

				if KEY == "CRUSADE":
					if not int(GAME.getGameTurn() % 3 * self.fTrainPrcntGS):
						CyPlayer = GC.getPlayer(iPlayer)
						iUnit = GC.getInfoTypeForString("UNIT_CRUSADER")
						CyUnit = CyPlayer.initUnit(iUnit, CyCity.getX(), CyCity.getY(), UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.NO_DIRECTION)
						CyCity.addProductionExperience(CyUnit, False)

				elif KEY == "KENTUCKY_DERBY":
					if int(GAME.getGameTurn() % 2 * self.fTrainPrcntGS): continue
					CyPlayer = GC.getPlayer(iPlayer)
					iUnit = GC.getInfoTypeForString("UNIT_SUBDUED_HORSE")
					iX = CyCity.getX()
					iY = CyCity.getY()
					CyUnit = CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					CyCity.addProductionExperience(CyUnit, False)
					if iPlayer == GAME.getActivePlayer():
						CvUtil.sendMessage(TRNSLTR.getText("TXT_NEW_HORSE",(CyUnit.getName(),)), iPlayer, 16, CyUnit.getButton(), ColorTypes(11), iX, iY, True, True)

				elif KEY == "GREAT_ZIMBABWE":
					if not CyCity.isFoodProduction():
						CyCity.changeFood(CyCity.getYieldRate(0) - CyCity.foodConsumption(False, 0))
						if CyCity.getFood() >= CyCity.growthThreshold():
							CyCity.changePopulation(1)
							CyCity.setFood(CyCity.getFoodKept())

				elif KEY == "BIODOME":
					if not self.aBiodomeList or GAME.getGameTurn() % int(4*self.fVictoryDelayPrcntGS + 1):
						continue
					CyPlayer = GC.getPlayer(iPlayer)
					iX = CyCity.getX()
					iY = CyCity.getY()
					iUnit = self.aBiodomeList[GAME.getSorenRandNum(len(self.aBiodomeList), "Which Animal")]
					CyUnit = CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					CyCity.addProductionExperience(CyUnit, False)
					if iPlayer == GAME.getActivePlayer():
						CvUtil.sendMessage(TRNSLTR.getText("TXT_NEW_ANIMAL",(CyUnit.getName(),)), iPlayer, 16, CyUnit.getButton(), ColorTypes(11), iX, iY, True, True)


	'''
	def onCityBuildingUnit(self, argsList):
		CyCity = argsList[0]
		iType = argsList[1]


	def onCityBuildingBuilding(self, argsList):
		CyCity = argsList[0]
		iType = argsList[1]

	def onCityBuildingProcess(self, argsList):
		CyCity, iType, = argsList
	'''


	def onCityRename(self, argsList):
		CyCity, = argsList
		if CyCity.getOwner() == GAME.getActivePlayer():
			self.__eventEditCityNameBegin(CyCity, True)


	'''
	def onCityHurry(self, argsList):
		CyCity, iHurryType = argsList
	'''


	def onVictory(self, argsList):
		iTeam, iVictory = argsList
		if iVictory >= 0 and iVictory < GC.getNumVictoryInfos():
			victoryInfo = GC.getVictoryInfo(int(iVictory))
			CvUtil.pyPrint("Victory!  Team %d achieves a %s victory" %(iTeam, victoryInfo.getDescription()))


	def onVassalState(self, argsList):
		iMaster, iVassal, bVassal = argsList # iMaster & iVassal are Teams not Players.
		if bVassal:
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.getTeam() == iMaster and CyPlayerX.countNumBuildings(GC.getInfoTypeForString("BUILDING_REICHSTAG")):
					CyPlayerX.changeGoldenAgeTurns(CyPlayerX.getGoldenAgeLength())


	def onGameUpdate(self, argsList):
		# Called on each game turn slice.
#		turnSlice = argsList[0][0][0]	# (((turnSlice, ), ), )

		# OOS Logger
		if self.bNetworkMP and CyInterface().isOOSVisible():
			import OOSLogger
			OOSLogger.writeLog()
			self.bNetworkMP = False


	''' Disabled in PythonCallbackDefines.xml (USE_ON_UPDATE_CALLBACK = False)
	def onUpdate(self, argsList):
		print ("onUpdate", argsList)
		'Called every frame'
		fDeltaTime = argsList[0]
	'''

#################### TRIGGERED EVENTS ##################

	def __eventEditCityNameBegin(self, CyCity, bRename):
		import ScreenResolution as SR
		xRes = SR.x
		if xRes > 2500:
			header = "<font=4b>"
			body = "\n<font=3>"
			w = 560
			h = 184
		elif xRes > 1700:
			header = "<font=3b>"
			body = "\n<font=2>"
			w = 500
			h = 172
		elif xRes > 1400:
			header = "<font=2b>"
			body = "\n<font=1>"
			w = 440
			h = 160
		else:
			header = "<font=1b>"
			body = "\n<font=0>"
			w = 380
			h = 136
		name = CyCity.getName()
		szTxt = header + TRNSLTR.getText("TXT_KEY_NAME_CITY", ()) + body + name

		popup = CyPopup(5000, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setUserData((name, CyCity.getID(), bRename))
		popup.setSize(w, h)
		popup.setPosition(xRes/2 - w/2, SR.y/2 - h/2)
		popup.setBodyString(szTxt, 1<<0)
		popup.createEditBox(name, 0)
		popup.setEditBoxMaxCharCount(24, 0, 0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventEditCityNameApply(self, iPlayer, userData, popupReturn):
		oldName = userData[0]
		newName = popupReturn.getEditBoxString(0)
		if oldName != newName:
			GC.getPlayer(iPlayer).getCity(userData[1]).setName(newName, not userData[2])

	def __eventEditCityBegin(self, argsList):
		px, py = argsList
		import CvWBPopups
		CvWBPopups.CvWBPopups().initEditCity(argsList)

	def __eventEditCityApply(self, iPlayer, userData, popupReturn):
		import CvWBPopups
		CvWBPopups.CvWBPopups().applyEditCity((popupReturn, userData))

	def __eventEditUnitNameBegin(self, argsList):
		pUnit = argsList
		popup = PyPopup.PyPopup(5006, EventContextTypes.EVENTCONTEXT_ALL)
		popup.setUserData((pUnit.getID(),))
		popup.setBodyString(TRNSLTR.getText("TXT_KEY_RENAME_UNIT", ()))
		popup.createEditBox(pUnit.getNameNoDesc())
		popup.launch()

	def __eventEditUnitNameApply(self, iPlayer, userData, popupReturn):
		iUnitID = userData[0]
		unit = GC.getPlayer(iPlayer).getUnit(iUnitID)
		newName = popupReturn.getEditBoxString(0)
		if len(newName) > 25:
			newName = newName[:25]
		unit.setName(newName)

	def __eventWBAllPlotsPopupBegin(self, argsList):
		CvScreensInterface.getWorldBuilderScreen().allPlotsCB()

	def __eventWBAllPlotsPopupApply(self, iPlayer, userData, popupReturn):
		if popupReturn.getButtonClicked() >= 0:
			CvScreensInterface.getWorldBuilderScreen().handleAllPlotsCB(popupReturn)

	def __eventWBLandmarkPopupBegin(self, argsList):
		CvScreensInterface.getWorldBuilderScreen().setLandmarkCB("")

	def __eventWBLandmarkPopupApply(self, iPlayer, userData, popupReturn):
		if popupReturn.getEditBoxString(0):
			szLandmark = popupReturn.getEditBoxString(0)
			if szLandmark:
				CvScreensInterface.getWorldBuilderScreen().setLandmarkCB(szLandmark)

	def __eventWBScriptPopupBegin(self, argsList):
		popup = PyPopup.PyPopup(5010, EventContextTypes.EVENTCONTEXT_ALL)
		popup.setHeaderString(TRNSLTR.getText("TXT_KEY_WB_SCRIPT", ()))
		popup.createEditBox(CvScreensInterface.getWorldBuilderScreen().getCurrentScript())
		popup.launch()

	def __eventWBScriptPopupApply(self, iPlayer, userData, popupReturn):
		if popupReturn.getEditBoxString(0):
			szScriptName = popupReturn.getEditBoxString(0)
			CvScreensInterface.getWorldBuilderScreen().setScriptCB(szScriptName)

	def __eventWBStartYearPopupBegin(self, argsList):
		popup = PyPopup.PyPopup(5011, EventContextTypes.EVENTCONTEXT_ALL)
		popup.createSpinBox(0, "", GAME.getStartYear(), 1, 5000, -5000)
		popup.launch()

	def __eventWBStartYearPopupApply(self, iPlayer, userData, popupReturn):
		iStartYear = popupReturn.getSpinnerWidgetValue(int(0))
		CvScreensInterface.getWorldBuilderScreen().setStartYearCB(iStartYear)
