#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvXMLLoadUtility.h
//
//  AUTHOR:  Eric MacDonald  --  8/2003
//
//  PURPOSE: Group of functions to load in the xml files for Civilization 4
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef XML_LOAD_UTILITY_H
#define XML_LOAD_UTILITY_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

//#include "CvStructs.h"
#include "CvInfos.h"
#include "CvGlobals.h"
#include "FVariableSystem.h"

class FXmlSchemaCache;
class FXml;
class CvGameText;
class CvCacheObject;
class CvImprovementBonusInfo;

class ParserErrorHandler : public xercesc::ErrorHandler
{
public:
	void warning(const xercesc::SAXParseException& ex)
	{
		throw ex;
	}

	void error(const xercesc::SAXParseException& ex)
	{
		throw ex;
	}

	void fatalError(const xercesc::SAXParseException& ex)
	{
		throw ex;
	}

	void resetErrors()
	{
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvXMLLoadUtility
//
//  DESC:   Group of functions to load in the xml files for Civilization 4
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvXMLLoadUtility
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	// default constructor
	DllExport CvXMLLoadUtility();
	// default destructor
	DllExport ~CvXMLLoadUtility();

	bool CreateFXml();
	void DestroyFXml();

	DllExport bool LoadPostMenuGlobals();
	DllExport bool LoadPreMenuGlobals();
	DllExport bool LoadBasicInfos();
	DllExport bool LoadPlayerOptions();
	DllExport bool LoadGraphicOptions();

	// read the global defines from a specific file
	bool ReadGlobalDefines(const TCHAR* szXMLFileName, CvCacheObject* cache);
	// loads globaldefines.xml and calls various other functions to load relevant global variables
	DllExport bool SetGlobalDefines();
	// loads globaltypes.xml and calls various other functions to load relevant global variables
	DllExport bool SetGlobalTypes();
	// loads calls various functions that load xml files that in turn load relevant global variables
	bool SetGlobals();
	// loads globaldefines.xml and calls various other functions to load relevant global variables
	DllExport bool SetPostGlobalsGlobalDefines();

	// calls various functions to release the memory associated with the global variables
	DllExport void CleanUpGlobalVariables();

	// releases global variables associated with items that can be reloaded
	DllExport void ResetLandscapeInfo();
	DllExport bool SetupGlobalLandscapeInfo();
	DllExport bool SetGlobalArtDefines();
	DllExport bool LoadGlobalText();
	bool SetHelpText();
	DllExport void ResetGlobalEffectInfo();

	// for progress bars
	typedef void (*ProgressCB)(int iStepNum, int iTotalSteps, const char* szMessage);
	static int GetNumProgressSteps();
	void RegisterProgressCB(ProgressCB cbFxn) { m_pCBFxn = cbFxn; }

	// moves the current xml node from where it is now to the next non-comment node, returns false if it can't find one
	// CHANGE 2013-10-15 (n47): now stored node is always an XML element, so it can't be a comment, thus we do not really need this method
	// Change 2013-11-24 (alberts2): removed all references
	bool SkipToNextVal() {return true;}

	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	// TODO 2013-11-21 (alberts2): check if this can be done with xerxes or remove all references.
	void MapChildren();	// call this before GetChildXMLValByName to use fast searching

	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetOptionalChildXmlValByName(int*   piVal, const wchar_t* szName, int   iDefault = 0) { return GetOptionalChildXmlValByName<int>  (piVal, szName, iDefault); }
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetOptionalChildXmlValByName(float* pfVal, const wchar_t* szName, float fDefault = 0.0f) { return GetOptionalChildXmlValByName<float>(pfVal, szName, fDefault); }
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetOptionalChildXmlValByName(bool*  pbVal, const wchar_t* szName, bool  bDefault = false) { return GetOptionalChildXmlValByName<bool> (pbVal, szName, bDefault); }
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetOptionalChildXmlValByName(std::string&  pszVal, const wchar_t* szName, char*  pszDefault = NULL);
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetOptionalChildXmlValByName(std::wstring& pszVal, const wchar_t* szName, wchar* pszDefault = NULL);
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	// TO DO - unsafe
	bool GetOptionalChildXmlValByName(char*  pszVal, const wchar_t* szName, char*  pszDefault = NULL);
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	// TO DO - unsafe
	bool GetOptionalChildXmlValByName(wchar* pszVal, const wchar_t* szName, wchar* pszDefault = NULL);
	
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetChildXmlValByName(std::string& pszVal, const wchar_t* szName, char* pszDefault = NULL);
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetChildXmlValByName(std::wstring& pszVal, const wchar_t* szName, wchar* pszDefault = NULL);
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	// TO DO - unsafe
	bool GetChildXmlValByName(char* pszVal, const wchar_t* szName, char* pszDefault = NULL);
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	// TO DO - unsafe
	bool GetChildXmlValByName(wchar* pszVal, const wchar_t* szName, wchar* pszDefault = NULL);
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetChildXmlValByName(int* piVal, const wchar_t* szName, int iDefault = 0)			{ return GetChildXmlValByName<int>  (piVal, szName, iDefault); }
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetChildXmlValByName(float* pfVal, const wchar_t* szName, float fDefault = 0.0f)	{ return GetChildXmlValByName<float>(pfVal, szName, fDefault); }
	// overloaded function that gets the child value of the tag with szName if there is only one child
	// value of that name
	bool GetChildXmlValByName(bool* pbVal, const wchar_t* szName, bool bDefault = false)	{ return GetChildXmlValByName<bool> (pbVal, szName, bDefault); }

	
	bool GetChildXmlValByName(std::string& pszVal, const char* szName, char* pszDefault = NULL)
	{ OutputDebugString("Call: bool GetChildXmlValByName(std::string& pszVal, const char* szName, char* pszDefault = NULL)");	FAssert(false); return false; }
	bool GetChildXmlValByName(std::wstring& pszVal, const char* szName, wchar* pszDefault = NULL)
	{ OutputDebugString("Call: bool GetChildXmlValByName(std::wstring& pszVal, const char* szName, wchar* pszDefault = NULL)");	FAssert(false); return false; }
	bool GetChildXmlValByName(char* pszVal, const char* szName, char* pszDefault = NULL) // TO DO - unsafe
	{ OutputDebugString("Call: GetChildXmlValByName(char* pszVal, const char* szName, char* pszDefault = NULL)");				FAssert(false); return false; }
	bool GetChildXmlValByName(wchar* pszVal, const char* szName, wchar* pszDefault = NULL) // TO DO - unsafe
	{ OutputDebugString("Call: bool GetChildXmlValByName(wchar* pszVal, const char* szName, wchar* pszDefault = NULL)");		FAssert(false); return false; }
	bool GetChildXmlValByName(int* piVal, const char* szName, int iDefault = 0)
	{ OutputDebugString("Call: bool GetChildXmlValByName(int* piVal, const char* szName, int iDefault = 0)");					FAssert(false); return false; }
	bool GetChildXmlValByName(float* pfVal, const char* szName, float fDefault = 0.0f)
	{ OutputDebugString("Call: bool GetChildXmlValByName(float* pfVal, const char* szName, float fDefault = 0.0f)");			FAssert(false); return false; }
	bool GetChildXmlValByName(bool* pbVal, const char* szName, bool bDefault = false)
	{ OutputDebugString("Call: bool GetChildXmlValByName(bool* pbVal, const char* szName, bool bDefault = false)");				FAssert(false); return false; }


	template<typename T>
	bool GetXmlVal(T* pVal, T pDefault)
	{
		// set the value to the default
		*pVal = pDefault;

		if (const XMLCh* nodesText = TryGetXmlFirstText())
		{
			std::wistringstream stream(nodesText);
			if( !(stream >> *pVal).fail() )
			{
				return true;
			}
			else
			{
				char* tmp = xercesc::XMLString::transcode(nodesText);
				FAssertMsg(false, tmp);
				xercesc::XMLString::release(&tmp);
				// not the right type, assume it is a global define
				char* nodeTextC = xercesc::XMLString::transcode(nodesText);
				if (GC.getDefinesVarSystem()->GetValue(nodeTextC, *pVal))
				{
					xercesc::XMLString::release(&nodeTextC);
					return true;
				}
				else
				{
					char szLog[1000];
					char* filePath = xercesc::XMLString::transcode(
						m_pParser->getDocument()->getDocumentURI());
					sprintf(szLog, "XML error: %s : Expected an %s or "
						"a name of %s global definition, found '%s'\n", 
						filePath, typeid(T).name(), typeid(T).name(), nodeTextC);
					xercesc::XMLString::release(&filePath);
					logMsg(szLog);
					gDLL->MessageBox(szLog, "Error");
					xercesc::XMLString::release(&nodeTextC);
					return false;
				}
			}
		}
		return false;
	}

	template<typename T>
	bool GetChildXmlValByName(T* pVal, const wchar_t* szName, T pDefault)
	{
		if (TryMoveToXmlFirstChild(szName))
		{
			// get the string value of the current xml node
			bool success = GetXmlVal(pVal, pDefault);
			MoveToXmlParent();
			return success;
		}
		else
		{
			char* tmp = xercesc::XMLString::transcode(szName);
			FAssertMsg(false, tmp);
			xercesc::XMLString::release(&tmp);
			*pVal = pDefault;

			return false;
		}
	}

	template<typename T>
	bool GetOptionalChildXmlValByName(T* pVal, const wchar_t* szName, T pDefault)
	{
		if (TryMoveToXmlFirstChild(szName))
		{
			// get the string value of the current xml node
			bool success = GetXmlVal(pVal, pDefault);
			MoveToXmlParent();
			return success;
		}
		else
		{
			*pVal = pDefault;
			return false;
		}
	}

	// loads an xml file into the FXml variable.  The szFilename parameter has
	// the m_szXmlPath member variable pre-pended to it to form the full pathname
	bool LoadCivXml(FXml* pFXml, const TCHAR* szFilename);

	int  GetXmlChildrenNumber() { return m_pCurrentXmlElement->getChildElementCount(); }
	int  GetXmlSiblingsNumber() 
	{ 
		if (xercesc::DOMNode* node = m_pCurrentXmlElement->getParentNode())
		{
			FAssert(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE);
			return reinterpret_cast<xercesc::DOMElement*>(node)->getChildElementCount();
		}
		else
		{
			return 1;
		}
	}

	const XMLCh* GetXmlTagName()        
	{ 
		// WARINING : here can be a problem, when we turn on namespaces
		return m_pCurrentXmlElement->getNodeName(); 
	}

	int  GetXmlChildrenNumber(const XMLCh* name) 
	{ 
		int number = 0;
		for (xercesc::DOMElement* child = m_pCurrentXmlElement->getFirstElementChild();
			child;
			child = child->getNextElementSibling())
		{
			if (xercesc::XMLString::equals(child->getTagName(), name))
				number++;
		}
		return number;
	}

	int  GetXmlSiblingsNumber(const XMLCh* name) 
	{ 
		int number = 0;
		for (xercesc::DOMElement* sibling = m_pCurrentXmlElement;
			sibling;
			sibling = sibling->getPreviousElementSibling())
		{
			if (xercesc::XMLString::equals(sibling->getTagName(), name))
				++number;
		}
		for (
			xercesc::DOMElement* sibling = m_pCurrentXmlElement->getNextElementSibling();
			sibling;
		sibling = sibling->getNextElementSibling())
		{
			if (xercesc::XMLString::equals(sibling->getTagName(), name))
				++number;
		}
		return number;
	}

	const XMLCh* GetXmlFirstText()      
	{
		for (xercesc::DOMNode* node = m_pCurrentXmlElement->getFirstChild();
			node;
			node = node->getNextSibling())
		{
			if (node->getNodeType() == xercesc::DOMNode::TEXT_NODE)
				return node->getNodeValue();
		}

		char* filePath = xercesc::XMLString::transcode(m_pParser->getDocument()->getDocumentURI());
		char* nodeTextC = xercesc::XMLString::transcode(m_pCurrentXmlElement->getTextContent());
		char szLog[2000];
		sprintf(szLog, "XML model (DOM) error: %s : No text in the element '%s'", filePath, nodeTextC);
		xercesc::XMLString::release(&filePath);
		logMsg(szLog);
		gDLL->MessageBox(szLog, "Error");
		return NULL;
	}

	const XMLCh* TryGetXmlFirstText()      
	{
		for (xercesc::DOMNode* node = m_pCurrentXmlElement->getFirstChild();
			node;
			node = node->getNextSibling())
		{
			if (node->getNodeType() == xercesc::DOMNode::TEXT_NODE)
				return node->getNodeValue();
		}
		return NULL;
	}

	bool  HasXmlNextSibling()    { return m_pCurrentXmlElement->getNextElementSibling(); }

	bool  TryMoveToXmlFirstChild()
	{ 
		if (xercesc::DOMElement* child = m_pCurrentXmlElement->getFirstElementChild())
		{
			m_pCurrentXmlElement = child;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool  TryMoveToXmlNextSibling()
	{ 
		if (xercesc::DOMElement* sibling = m_pCurrentXmlElement->getNextElementSibling())
		{
			m_pCurrentXmlElement = sibling; 
			return true;
		}
		else
		{
			return false;
		}
	}

	void  MoveToXmlParent()
	{
		xercesc::DOMNode* parent = m_pCurrentXmlElement->getParentNode();
		FAssert(parent);
		FAssert(parent->getNodeType() == xercesc::DOMNode::ELEMENT_NODE);
		m_pCurrentXmlElement = reinterpret_cast<xercesc::DOMElement*>(parent);
	}

	bool  TryMoveToXmlFirstChild(const XMLCh* name)
	{
		for (xercesc::DOMElement* child = m_pCurrentXmlElement->getFirstElementChild();
			child;
			child = child->getNextElementSibling())
		{
			if (xercesc::XMLString::equals(child->getTagName(), name))
			{
				m_pCurrentXmlElement = child;
				return true;
			}
		}
		return false;
	}

	bool  TryMoveToXmlFirstOfSiblings(const XMLCh* name)
	{
		xercesc::DOMElement* lastFound = 0;
		for (xercesc::DOMElement* sibling = m_pCurrentXmlElement;
			sibling;
			sibling = sibling->getPreviousElementSibling())
		{
			if (xercesc::XMLString::equals(sibling->getTagName(), name))
				lastFound = sibling;
		}
		if (lastFound)
		{
			m_pCurrentXmlElement = lastFound;
			return true;
		}
		else for (
			xercesc::DOMElement* sibling = m_pCurrentXmlElement->getNextElementSibling();
			sibling;
			sibling = sibling->getNextElementSibling())
		{
			if (xercesc::XMLString::equals(sibling->getTagName(), name))
			{
				m_pCurrentXmlElement = sibling;
				return true;
			}
		}
		return false;
	}

	bool  TryMoveToXmlNextSibling(const XMLCh* name)
	{
		for (
			xercesc::DOMElement* sibling = m_pCurrentXmlElement->getNextElementSibling();
			sibling;
			sibling = sibling->getNextElementSibling())
		{
			if (xercesc::XMLString::equals(sibling->getTagName(), name))
			{
				m_pCurrentXmlElement = sibling;
				return true;
			}
		}
		return false;
	}

	bool  TryMoveToXmlFirstMatchingElement(const XMLCh* xpath);

	// overloaded function that gets either the current xml node's or the next non-comment xml node's string value
	// depending on if the current node is a non-comment node or not
	bool GetXmlVal(std::wstring& pszVal, wchar* pszDefault = NULL);
	bool GetXmlVal(std::string& pszVal, char* pszDefault = NULL);
	// overloaded function that gets either the current xml node's or the next non-comment xml node's string value
	// depending on if the current node is a non-comment node or not
	bool GetXmlVal(wchar* pszVal, wchar* pszDefault = NULL); // TO DO - unsafe
	bool GetXmlVal(char* pszVal, char* pszDefault = NULL); // TO DO - unsafe
	// overloaded function that gets either the current xml node's or the next non-comment xml node's int value
	// depending on if the current node is a non-comment node or not
	bool GetXmlVal(int* piVal, int iDefault = 0)		{ return GetXmlVal<int>  (piVal, iDefault); }
	// overloaded function that gets either the current xml node's or the next non-comment xml node's float value
	// depending on if the current node is a non-comment node or not
	bool GetXmlVal(float* pfVal, float fDefault = 0.0f) { return GetXmlVal<float>(pfVal, fDefault); }
	// overloaded function that gets either the current xml node's or the next non-comment xml node's boolean value
	// depending on if the current node is a non-comment node or not
	bool GetXmlVal(bool* pbVal, bool bDefault = false)	{ return GetXmlVal<bool> (pbVal, bDefault); }

	// overloaded function that sets the current xml node to it's next sibling and then
	//	gets the next non-comment xml node's string value
	bool GetNextXmlVal(std::string& pszVal, char* pszDefault = NULL);
	bool GetNextXmlVal(std::wstring& pszVal, wchar* pszDefault = NULL);
	// overloaded function that sets the current xml node to it's next sibling and then
	//	gets the next non-comment xml node's string value
	bool GetNextXmlVal(char* pszVal, char* pszDefault = NULL); // TO DO - unsafe
	bool GetNextXmlVal(wchar* pszVal, wchar* pszDefault = NULL); // TO DO - unsafe
	// overloaded function that sets the current xml node to it's next sibling and then
	//	gets the next non-comment xml node's int value
	bool GetNextXmlVal(int* piVal, int iDefault = 0);
	// overloaded function that sets the current xml node to it's next sibling and then
	//	gets the next non-comment xml node's float value
	bool GetNextXmlVal(float* pfVal, float fDefault = 0.0f);
	// overloaded function that sets the current xml node to it's next sibling and then
	//	gets the next non-comment xml node's boolean value
	bool GetNextXmlVal(bool* pbVal, bool bDefault = false);

	// overloaded function that sets the current xml node to it's first non-comment child node
	//	and then gets that node's string value
	bool GetChildXmlVal(std::string& pszVal, char* pszDefault = NULL);
	bool GetChildXmlVal(std::wstring& pszVal, wchar* pszDefault = NULL);
	// overloaded function that sets the current xml node to it's first non-comment child node
	//	and then gets that node's string value
	bool GetChildXmlVal(char* pszVal, char* pszDefault = NULL); // TO DO - unsafe
	bool GetChildXmlVal(wchar* pszVal, wchar* pszDefault = NULL); // TO DO - unsafe
	// overloaded function that sets the current xml node to it's first non-comment child node
	//	and then gets that node's integer value
	bool GetChildXmlVal(int* piVal, int iDefault = 0);
	// overloaded function that sets the current xml node to it's first non-comment child node
	//	and then gets that node's float value
	bool GetChildXmlVal(float* pfVal, float fDefault = 0.0f);
	// overloaded function that sets the current xml node to it's first non-comment child node
	//	and then gets that node's boolean value
	bool GetChildXmlVal(bool* pbVal, bool bDefault = false);

#ifdef _USRDLL
	FXml* GetXML() { return NULL; }
	xercesc::DOMElement* GetCurrentXMLElement() { return m_pCurrentXmlElement; }
	void SetCurrentXMLElement(xercesc::DOMElement* element) { m_pCurrentXmlElement = element; }
#endif

	// loads the local yield from the xml file
	int SetYields(int** ppiYield);

#ifdef _USRDLL
	template <class T>
	int SetCommerce(T** ppiCommerce);
#endif
/************************************************************************************************/
/* Afforess	                  Start		 05/25/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// allocate and set the feature struct variables for the CvBuildInfo class
	void SetFeatureStruct(int** ppiFeatureTech, int** ppiFeatureTime, int** ppiFeatureProduction, bool** ppbFeatureRemove, bool** ppbNoTechCanRemoveWithNoProductionGain);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	// loads the improvement bonuses from the xml file
	void SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus);
	
/*************************************************************************************************/
/**	New Tag Defs	(XMLInfos)				08/09/08								Xienwolf	**/
/**																								**/
/**								Defines Function for Use in .cpp								**/
/*************************************************************************************************/
	// acquire a list of unknown length of String data from a Child setup in a String (typically for Pass 3 Use)
    void SetStringWithChildList(int* iNumEntries, std::vector<CvString>* aszXMLLoad);
	// acquire a list of unknown length of String data from a Child setup in an Array of INTs
    void SetIntWithChildList(int* iNumEntries, int** piXMLLoad);
	// acquire a list of known length of String data from a Child setup true values in an Array of BOOLs
    void SetBoolFromChildList(int iNumEntries, bool** pbXMLLoad);
/*************************************************************************************************/
/**	New Tag Defs							END													**/
/*************************************************************************************************/

	// Change 2013-11-21 (alberts2): we don't use this anymore use GetInfoClass(const TCHAR* pszVal)!
	static int FindInfoClass(const TCHAR* pszVal, bool hideAssert = false);
	//Searches the InfoClass for the pszVal and returns the location if a match
	//				is found.
	static int GetInfoClass(const TCHAR* pszVal);

#ifdef _USRDLL
	template <class T>
	static void InitList(T **ppList, int iListLen, T val = 0);
#endif
	void InitStringList(CvString **ppszList, int iListLen, CvString szString);

	void InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen);
	// allocate and initialize the civilization's default buildings
	void InitBuildingDefaults(int **ppiDefaults);
	// allocate and initialize the civilization's default units
	void InitUnitDefaults(int **ppiDefaults);
/************************************************************************************************/
/* Afforess	                  Start		 08/26/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// allocate and initialize a 2 dimensional array of bool pointers
	static void Init2DBoolList(bool*** pppbList, int iSizeX, int iSizeY);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	// allocate and initialize a 2 dimensional array of int pointers
	static void Init2DIntList(int*** pppiList, int iSizeX, int iSizeY);
	// allocate and initialize a 2 dimensional array of float pointers
	static void Init2DFloatList(float*** pppfList, int iSizeX, int iSizeY);
	// allocate and initialize a 2D array of DirectionTypes
	static void Init2DDirectionTypesList(DirectionTypes*** pppiList, int iSizeX, int iSizeY);
	// allocate an array of int pointers
	static void InitPointerIntList(int*** pppiList, int iSizeX);
	// allocate an array of float pointers
	static void InitPointerFloatList(float*** pppfList, int iSizeX);

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPair(int **ppiList, const wchar_t* szRootTagName, int iInfoBaseLength, int iDefaultListVal = 0);

/************************************************************************************************/
/* RevDCM  XMLloading                             05/05/10             phungus420               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// allocate and initialize a list from a tag pair in the xml by tag name
	void SetVariableListTagPair(int **ppiList, const wchar_t* szRootTagName,
		int iInfoBaseLength,
		const wchar_t* szValueTagName, int iValueInfoBaseLength, int iDefaultListVal = -1);
/************************************************************************************************/
/* RevDCM	                                 END                                                */
/************************************************************************************************/

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPair(bool **ppbList, const wchar_t* szRootTagName,
		int iInfoBaseLength, bool bDefaultListVal = false);

	// allocate and initialize a list from a tag pair in the xml
	// TODO 2013-11-24 (alberts2): not used anywhere
	void SetVariableListTagPair(float **ppfList, const wchar_t* szRootTagName,
		int iInfoBaseLength, float fDefaultListVal = 0.0f);

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPair(CvString **ppszList, const wchar_t* szRootTagName,
		int iInfoBaseLength, CvString szDefaultListVal = CvString());

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPair(int **ppiList, const wchar_t* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, int iDefaultListVal = 0);

	// allocate and initialize a list from a tag pair in the xml for audio scripts
	void SetVariableListTagPairForAudioScripts(int **ppiList, const wchar_t* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, int iDefaultListVal = -1);

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPairForAudioScripts(int **ppiList, const wchar_t* szRootTagName,
		int iInfoBaseLength, int iDefaultListVal = -1);

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPair(bool **ppbList, const wchar_t* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, bool bDefaultListVal = false);

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPair(CvString **ppszList, const wchar_t* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, CvString szDefaultListVal = CvString());

	// allocate and initialize a list from a tag pair in the xml
	void SetVariableListTagPair(std::vector<int>, const wchar_t* szRootTagName,
		int iInfoBaseLength, int iDefaultListVal = 0);

	void SetOptionalIntVector(std::vector<int>* aInfos, const wchar_t* szRootTagName){return SetOptionalVector<int>(aInfos, szRootTagName);}

	void SetOptionalIntVectorWithDelayedResolution(std::vector<int>& aInfos, const wchar_t* szRootTagName);
	static void CopyNonDefaultsFromIntVector(std::vector<int>& target, std::vector<int>& source){return CopyNonDefaultsFromVector<int>(target, source);}

	template<class T1, class T2, class T3>
	void SetOptionalPairVector(T1* aInfos, const wchar_t* szRootTagName)
	{
		CvString szTextVal;
		aInfos->clear();
		if (TryMoveToXmlFirstChild(szRootTagName))
		{
			int iNumSibs = GetXmlChildrenNumber();

			if (0 < iNumSibs)
			{
				if (TryMoveToXmlFirstChild())
				{
					for (int j = 0; j < iNumSibs; ++j)
					{
						if (GetChildXmlVal(szTextVal))
						{
							T2 eType = (T2)GetInfoClass(szTextVal);
							T3 iModifier;
							GetNextXmlVal(&iModifier);
							aInfos->push_back(std::make_pair(eType, iModifier));

							MoveToXmlParent();

						}

						if (!TryMoveToXmlNextSibling())
						{
							break;
						}

					}

					MoveToXmlParent();
				}
			}

			MoveToXmlParent();
		}
	}
	template<class T>
	static void CopyNonDefaultsFromVector(std::vector<T>& target, std::vector<T>& source)
	{
		for (typename std::vector<T>::const_iterator it = source.begin(), end = source.end(); it != end; ++it)
		{
			if ((*it) > -1 && find(target.begin(), target.end(), *it) == target.end())
			{
				target.push_back(*it);
			}
		}

		std::sort(target.begin(), target.end());
	}

	template<class T>
	void SetOptionalVector(std::vector<T>* aInfos, const wchar_t* szRootTagName)
	{
		if (TryMoveToXmlFirstChild(szRootTagName))
		{
			const int iNumSibs = GetXmlChildrenNumber();
			CvString szTextVal;

			if (0 < iNumSibs)
			{
				if (GetChildXmlVal(szTextVal))
				{
					for (int j = 0; j < iNumSibs; j++)
					{
						T value = static_cast<T>(GetInfoClass(szTextVal));
						if (value > -1  && find(aInfos->begin(), aInfos->end(), value) == aInfos->end())
						{
							aInfos->push_back(value);
						}
						if (!GetNextXmlVal(szTextVal))
						{
							break;
						}
					}

					std::sort(aInfos->begin(), aInfos->end());

					MoveToXmlParent();
				}
			}

			MoveToXmlParent();
		}
	}

	// create a hot key from a description
	CvWString CreateHotKeyFromDescription(const TCHAR* pszHotKey, bool bShift = false, bool bAlt = false, bool bCtrl = false);

	// set the variable to a default and load it from the xml if there are any children
	bool SetAndLoadVar(int** ppiVar, int iDefault=0);

	// function that sets the number of strings in a list, initializes the string to the correct length, and fills it from the
	// current xml file, it assumes that the current node is the parent node of the string list children
	bool SetStringList(CvString** ppszStringArray, int* piSize);

	// get the integer value for the keyboard mapping of the hotkey if it exists
	int GetHotKeyInt(const TCHAR* pszHotKeyVal);

	// Returns true if the dependency list is satisfied, false if not.
	inline bool CheckDependency();

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/30/07                                MRGENIE      */
/*                                                                                              */
/* Savegame compatibility                                                                       */
/************************************************************************************************/
	bool doResetGlobalInfoClasses();
	bool doResetInfoClasses();
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

	static void RemoveTGAFiller();
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 10/19/07                                MRGENIE      */
/*                                                                                              */
/* Needs to be public for CvXMLLoadUtilityModTools                                              */
/************************************************************************************************/
#ifdef _DEBUG	
	void XmlArtTagVerification(char* format, ... );
#endif
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/30/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void logMLF(char* format, ... );
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 END                                                  */
/************************************************************************************************/
	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
private:
	xercesc::DOMElement*      m_pCurrentXmlElement;
	xercesc::XercesDOMParser* m_pParser;
	//FXml* m_pFXml;						// member variable pointer to the current FXml class
	//FXmlSchemaCache* m_pSchemaCache;	// keep a single schema cache, instead of loading the same schemas multiple times
	int m_iCurProgressStep;
	ProgressCB m_pCBFxn;

//---------------------------------------PRIVATE INTERFACE---------------------------------
private:
	void UpdateProgressCB(const char* szMessage=NULL);

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/24/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

	// Python Modular Loading
	template <class T>
	void LoadPythonModulesInfo(std::vector<T*>& aInfos, const char* szFileRoot, const wchar_t* szXmlPath, bool bTwoPass);
	//Main control of the MLF feature
	void ModularLoadingControlXML();
	// In the next 2 methods we load the MLF classes
	template <class T>
	bool LoadModLoadControlInfo(std::vector<T*>& aInfos, const char* szFileRoot, const wchar_t* szXmlPath);
	template <class T>
	bool SetModLoadControlInfo(std::vector<T*>& aInfos, const wchar_t* szTagName, CvString szConfigString, CvString szDirDepth = "Modules\\", int iDirDepth = 0);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* SORT_ALPHABET                           11/17/07                                MRGENIE      */
/*                                                                                              */
/* Method for alphabetically order tags                                                         */
/************************************************************************************************/
//	template <class T>
//	void SortAlphabetGlobalClassInfo(std::vector<T*>& aInfos);
/************************************************************************************************/
/* SORT_ALPHABET                           END                                                  */
/************************************************************************************************/
	// take a character string of hex values and return their unsigned int value
	void MakeMaskFromString(unsigned int *puiMask, char* szMask);

	// find the tag name in the xml file and set the string parameter and num val parameter based on it's value
	void SetGlobalStringArray(CvString** ppszString, wchar_t* szTagName, int* iNumVals, bool bUseEnum=false);
	void SetDiplomacyCommentTypes(CvString** ppszString, int* iNumVals);	// sets diplomacy comments

	void SetGlobalUnitScales(float* pfLargeScale, float* pfSmallScale, const wchar_t* szTagName);

#ifdef _USRDLL
	template <class T>
		void SetGlobalDefine(const char* szDefineName, T*& piDefVal)
	{ 
		GC.getDefinesVarSystem()->GetValue(szDefineName, piDefVal); 
	}
#endif
	//
	// template which can handle all info classes
	//
	// a dynamic value for the list size
#ifdef _USRDLL
	template <class T>
	void SetGlobalClassInfo(std::vector<T*>& aInfos, const wchar_t* szTagName, bool bTwoPass, CvInfoReplacements<T>* pReplacements = NULL);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/17/08                                MRGENIE      */
/*                                                                                              */
/* This method is a replacement for the bTwoPass, if stuff that is depending on each other in   */
/* a loop, the bTwoPass would fail since it doesn't look first in the other Modules!            */
/************************************************************************************************/
	template <class T>
	void SetGlobalClassInfoTwoPassReplacement(std::vector<T*>& aInfos, const wchar_t* szTagName, CvInfoReplacements<T>* pReplacements = NULL);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	template <class T>
	void LoadGlobalClassInfo(std::vector<T*>& aInfos, const char* szFileRoot, const char* szFileDirectory, const wchar_t* szXmlPath, bool bTwoPass, CvInfoReplacements<T>* pReplacements = NULL);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/13/08                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	/*
	template <class T>
	void LoadGlobalClassInfoModular(std::vector<T*>& aInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, bool bTwoPass);*/
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
#endif
	void SetDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const wchar_t* szTagName);
	void LoadDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szFileRoot, const char* szFileDirectory, const wchar_t* szXmlPath, bool bUseCaching);

	//
	// special cases of set class info which don't use the template because of extra code they have
	//
	void SetGlobalActionInfo();
	//void SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, const wchar_t* szTagName, int* iNumVals);
	void SetGameText(const wchar_t* szTextGroup, const wchar_t* szTagName, std::vector<CvGameText>& texts);

	// create a keyboard string from a KB code, Delete would be returned for KB_DELETE
	CvWString CreateKeyStringFromKBCode(const TCHAR* pszHotKey);

	void orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength);
	void logMsg(char* format, ... );
	void logMsgW(wchar_t* format, ... );
/************************************************************************************************/
/* XML_CHECK_DOUBLE_TYPE                   10/10/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
#ifdef _DEBUG
	void logXmlCheckDoubleTypes(char* format, ... );
#endif
/************************************************************************************************/
/* XML_CHECK_DOUBLE_TYPE                   END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 02/20/08                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//#ifdef _DEBUG	
	void logXmlDependencyTypes(char* format, ... );
//#endif
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 06/13/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void logXML(char* format, ... );
	/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
};

#ifdef _USRDLL
//
/////////////////////////// inlines / templates
//
template <class T>
void CvXMLLoadUtility::InitList(T **ppList, int iListLen, T val)
{
	FAssertMsg((0 <= iListLen),"list size to allocate is less than 0");
	*ppList = new T[iListLen];

	for (int i=0;i<iListLen;i++)
		(*ppList)[i] = val;	
}

template <class T>
int CvXMLLoadUtility::SetCommerce(T** ppbCommerce)
{
	int i=0;			//loop counter
	int iNumSibs=0;		// the number of siblings the current xml node has
	T *pbCommerce;	// local pointer for the Commerce memory

	// get the total number of children the current xml node has
	iNumSibs = GetXmlChildrenNumber();
	InitList(ppbCommerce, NUM_COMMERCE_TYPES);

	pbCommerce = *ppbCommerce;
	if (0 < iNumSibs)
	{
		// if the call to the function that sets the current xml node to it's first non-comment
		// child and sets the parameter with the new node's value succeeds
		if (GetChildXmlVal(&pbCommerce[0]))
		{
			FAssertMsg((iNumSibs <= NUM_COMMERCE_TYPES) , "For loop iterator is greater than array size");
			// loop through all the siblings, we start at 1 since we already have the first value
			for (i=1;i<iNumSibs;i++)
			{
				if (!GetNextXmlVal(&pbCommerce[i]))
				{
					break;
				}
			}
			MoveToXmlParent();
		}
	}

	return iNumSibs;
	//int i=0;			//loop counter
	//int iNumSibs=0;		// the number of siblings the current xml node has
	//int iNumChildren;
	//T *pbCommerce;	// local pointer for the Commerce memory

	// get the total number of children the current xml node has
	//iNumChildren = GetXmlChildrenNumber();
	//InitList(ppbCommerce, NUM_COMMERCE_TYPES);

	//pbCommerce = *ppbCommerce;
	//if (0 < iNumChildren)
	//{
	//	 if the call to the function that sets the current xml node to it's first non-comment
	//	 child and sets the parameter with the new node's value succeeds
	//	if (GetChildXmlVal(&pbCommerce[0]))
	//	{
	//		FAssertMsg((iNumSibs <= NUM_COMMERCE_TYPES) , "For loop iterator is greater than array size");
	//		 loop through all the siblings, we start at 1 since we already have the first value
	//		for (i=1;i<iNumChildren;i++)
	//		{
	//			if (!GetNextXmlVal(&pbCommerce[i]))
	//			{
	//				break;
	//			}
	//		}
	//		MoveToXmlParent();
	//	}
	//}

	//return iNumChildren;
}

// Returns true if the dependency list is satisfied, false if not.
bool CvXMLLoadUtility::CheckDependency()
{
	CvString szTypeDepend;
	bool bDependencyNotFound = false;
	if (GetOptionalChildXmlValByName(szTypeDepend, L"DependencyType")) {
		if (bDependencyNotFound = -1 == GC.getInfoTypeForString(szTypeDepend, true))
			return false;
	}
	if (TryMoveToXmlFirstChild(L"AndDependencyTypes")) {
		if (GetChildXmlVal(szTypeDepend)) {	// If any of the AND depends is not loaded, set bNotFound
			while ( !(bDependencyNotFound = -1 == GC.getInfoTypeForString(szTypeDepend, true))
					&& GetNextXmlVal(szTypeDepend));
			MoveToXmlParent();
		}
		MoveToXmlParent();
		if (bDependencyNotFound)
			return false;
	}
	if (TryMoveToXmlFirstChild(L"OrDependencyTypes")) {
		if (GetChildXmlVal(szTypeDepend)) {	// If any of the OR depends is loaded, unset bNotFound
			while ( (bDependencyNotFound = -1 == GC.getInfoTypeForString(szTypeDepend, true))
					&& GetNextXmlVal(szTypeDepend));
			MoveToXmlParent();
		}
		MoveToXmlParent();
		if (bDependencyNotFound)
			return false;
	}
	return true;
}

#endif

#endif	// XML_LOAD_UTILITY_H
