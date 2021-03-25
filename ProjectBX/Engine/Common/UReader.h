//----------------------------------------------------------
// File:		UReader.h
// Author:		Kevin Bray
// Created:		09-19-04
//
// Purpose:		To wrap and simplify pointer read operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UReader
//**********************************************************
class UReader
{
public:
	UReader( const UReader& reader );
	UReader( const void *buffer, unsigned long size, bool takeOwnership );
	~UReader();

	void			Reset()				{	_offset = 0;				}

	tstring			ReadString( bool peek = false );
	tstring			ReadLine( bool peek = false );
	int				CurLineLen() const;
	char			ReadChar( bool peek = false );
	unsigned char	ReadUChar( bool peek = false );
	short			ReadShort( bool peek = false );
	unsigned short	ReadUShort( bool peek = false );
	int				ReadInt( bool peek = false );
	unsigned int	ReadUInt( bool peek = false );
	float			ReadFloat( bool peek = false );
	void			ReadBytes( void *buffer, unsigned int count, bool peek = false );
	void			Skip( unsigned long bytes );
	void			Rewind( unsigned long bytes );
	unsigned long	BytesRemaining() const	{	return( _size - _offset );	}
	unsigned long	GetSize() const			{	return _size;				}
	const void *	GetDataPtr() const		{	return _buffer;				}
	const void *	GetCurPtr() const		{	return ( ( const char * )_buffer ) + _offset;	}
	bool			IsFinished() const		{	return ( BytesRemaining() == 0 );	}

	UReader&		operator = ( const UReader& reader );

private:
	void			Assign( const UReader& reader );

	unsigned long	_size;
	unsigned long	_offset;
	const void *	_buffer;
	bool			_own;
};
