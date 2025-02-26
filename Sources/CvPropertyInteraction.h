#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyInteraction.h
//
//  PURPOSE: Interaction of generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTY_INTERACTION_H
#define CV_PROPERTY_INTERACTION_H

#include "CvXMLLoadUtilityModTools.h"
#include "CvGameObject.h"

class CvGameObject;

// Base class of which the actual property interaction classes are derived
class CvPropertyInteraction
{
public:
	CvPropertyInteraction();
	CvPropertyInteraction(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	virtual ~CvPropertyInteraction();
	PropertyTypes getSourceProperty() const;
	PropertyTypes getTargetProperty() const;
	void setSourceProperty(PropertyTypes eProperty);
	void setTargetProperty(PropertyTypes eProperty);
	GameObjectTypes getObjectType() const;
	void setObjectType(GameObjectTypes eObjectType);
	RelationTypes getRelation() const;
	void setRelation(RelationTypes eRelation);
	int getRelationData() const;
	void setRelationData(int iRelationData);

	bool isActive(CvGameObject* pObject);

	virtual PropertyInteractionTypes getType() = 0;

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) = 0;
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) = 0;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	
	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);
protected:
	PropertyTypes m_eSourceProperty;
	PropertyTypes m_eTargetProperty;
	GameObjectTypes m_eObjectType;
	RelationTypes m_eRelation;
	int m_iRelationData;
	BoolExpr* m_pExprActive;
};


class CvPropertyInteractionConvertConstant : public CvPropertyInteraction
{
public:
	CvPropertyInteractionConvertConstant();
	CvPropertyInteractionConvertConstant(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	CvPropertyInteractionConvertConstant(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iAmountPerTurn);

	virtual PropertyInteractionTypes getType();

	int getAmountPerTurn();

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget);
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget);

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	
	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);

protected:
	int m_iAmountPerTurn;
};


class CvPropertyInteractionInhibitedGrowth : public CvPropertyInteraction
{
public:
	CvPropertyInteractionInhibitedGrowth();
	CvPropertyInteractionInhibitedGrowth(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	CvPropertyInteractionInhibitedGrowth(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iGrowthPercent, int iInhibitionPercent);

	virtual PropertyInteractionTypes getType();

	int getGrowthPercent();
	int getInhibitionPercent();

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget);
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget);

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);

protected:
	int m_iGrowthPercent;
	int m_iInhibitionPercent;
};


class CvPropertyInteractionConvertPercent : public CvPropertyInteraction
{
public:
	CvPropertyInteractionConvertPercent();
	CvPropertyInteractionConvertPercent(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	CvPropertyInteractionConvertPercent(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iPercent, int iNoConvertAmount);

	virtual PropertyInteractionTypes getType();

	int getPercent();
	int getNoConvertAmount();

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget);
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget);

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);

protected:
	int m_iPercent;
	int m_iNoConvertAmount;
};

#endif