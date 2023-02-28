//----------------------------------------------------------
// File:		GmEntity.h
// Author:		Shawn Presser
// Created:		12-15-08
//
// Purpose:		To represent a visual entity.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/UPath.h"
#include "Common/URef.h"
#include "Common/MMat4x4.h"
#include "Common/MVec3.h"
#include "Graphics/GrModel.h"

// PhysX headers.
#include "NxBodyDesc.h"

// forward declarations.
class GmEntityMgr;

//**********************************************************
// class GmEntity
//**********************************************************
class GmEntity
{
	friend class GmEntityMgr;

protected:
	GmEntity( const tstring& name, URef< GrModel > model );
	virtual ~GmEntity();

public:

	// accessors.
	URef< GrModel >		GetModel() const				{	return _model;	}

	// updates the entity.
	virtual void		Update()=0;

protected:
	URef< GrModel >		_model;

private:
	tstring				_name;
};
