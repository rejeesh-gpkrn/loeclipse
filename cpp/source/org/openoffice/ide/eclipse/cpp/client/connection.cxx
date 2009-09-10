/*
 * This file has been automatically generated by the ooeclipse integration.
 * http://cedric.bosdonnat.free.fr/ooeclipseintegration
 *
 * Ported by Cedric Bosdonnat from the JODConverter code.
 * The original Java code has been written by Mirko Nasato <mirko@artofsolving.com>
 *
 * Copyright (C) 2009 Novell Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * http://www.gnu.org/copyleft/lesser.html
 *
 * Contributor:
 * Mirko Nasato <mirko@artofsolving.com>
 * Laurent Godard <lgodard@nuxeo.com>
 */

#include <iostream>

#include <rtl/bootstrap.hxx>
#include <cppuhelper/bootstrap.hxx>

#include <com/sun/star/connection/XConnector.hpp>
#include <com/sun/star/connection/XConnection.hpp>
#include <com/sun/star/connection/NoConnectException.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/bridge/XBridgeFactory.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "connection.hxx"

using namespace com::sun::star::frame;
using namespace com::sun::star::connection;
using namespace com::sun::star::beans;
using namespace com::sun::star::bridge;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star;

#define OUSTRING_TO_C( x )		rtl::OUStringToOString( x, RTL_TEXTENCODING_UTF8 ).getStr( )

namespace unoclienthelper {

SocketConnection::SocketConnection( int nPort, char* pHost )
{
	rtl::OUString sCnxString = rtl::OUString::createFromAscii( "socket,host=" );
	sCnxString = sCnxString.concat( rtl::OUString::createFromAscii( pHost ) );
	sCnxString = sCnxString.concat( rtl::OUString::createFromAscii( ",port=" ) );
	m_sConnectionString = sCnxString.concat( rtl::OUString::valueOf( sal_Int32( nPort ) ) );
}

SocketConnection::~SocketConnection()
{
}

PipeConnection::PipeConnection( char* pPipeName )
{
	rtl::OUString sCnxString = rtl::OUString::createFromAscii( "pipe,name=" );
	m_sConnectionString = sCnxString.concat( rtl::OUString::createFromAscii( pPipeName ) );
}

PipeConnection::~PipeConnection()
{
}

AbstractConnection::AbstractConnection() :
	m_bExpectingDisconnect( false ),
	m_bConnected ( false )
{
}

AbstractConnection::~AbstractConnection()
{
}

bool AbstractConnection::connect( )
{
	// Try to connect
	try
	{
		Reference< XComponentContext > xCtx = cppu::defaultBootstrap_InitialComponentContext();
		Reference< XMultiServiceFactory > xServiceFactory ( xCtx->getServiceManager(), UNO_QUERY );

		// Get a connection
		Reference< XConnector> xConnector ( xServiceFactory->createInstance(
				rtl::OUString::createFromAscii( "com.sun.star.connection.Connector" ) ), UNO_QUERY );
		Reference< XConnection > xConnection = xConnector->connect( m_sConnectionString );

		// Get a Bridge
		Reference< XBridgeFactory > xBridgeFactory( xServiceFactory->createInstance(
				rtl::OUString::createFromAscii( "com.sun.star.bridge.BridgeFactory" ) ), UNO_QUERY );
		m_xBridge = xBridgeFactory->createBridge( rtl::OUString( ), rtl::OUString::createFromAscii( "urp" ),
				xConnection, NULL );

		// Register a listener on the bridge
		Reference< XComponent > xBridgeComponent( m_xBridge, UNO_QUERY );
		xBridgeComponent->addEventListener( this );

		// Get the Remote service manager
		Reference< XMultiComponentFactory > xServiceMngr(
				m_xBridge->getInstance( rtl::OUString::createFromAscii( "StarOffice.ServiceManager" ) ),
				UNO_QUERY );
		m_xServiceMngr = xServiceMngr;

		// Get the remote default context
		Reference<XPropertySet> xPropSet( m_xServiceMngr, UNO_QUERY );
		Reference<XComponentContext> xRemoteCtxt(
				xPropSet->getPropertyValue( rtl::OUString::createFromAscii( "DefaultContext" ) ),
				UNO_QUERY );
		m_xCtx = xRemoteCtxt;

#if DEBUG
		fprintf( stderr, "Connected\n" );
#endif

		m_bConnected = true;
	}
	catch ( NoConnectException e )
	{
#if DEBUG
		fprintf( stderr, "connection failed: %s : %s\n",
				OUSTRING_TO_C( m_sConnectionString ),
				OUSTRING_TO_C( e.Message ) );
#endif
		m_bConnected = false;
	}
	return m_bConnected;
}

void AbstractConnection::disconnect()
{
	m_bExpectingDisconnect = true;
	Reference< XComponent > xBridgeComponent( m_xBridge, UNO_QUERY );
	xBridgeComponent->dispose( );
}

bool AbstractConnection::isConnected( )
{
	return m_bConnected;
}

Reference<XComponentContext> AbstractConnection::getContext( )
{
	return m_xCtx;
}

Reference<XMultiComponentFactory> AbstractConnection::getServiceManager( )
{
	return m_xServiceMngr;
}

Reference<XDesktop> AbstractConnection::getDesktop()
{
	Reference<XDesktop> xDesktop(
			getService( rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ),
			UNO_QUERY );
	return xDesktop;
}

Reference<XInterface> AbstractConnection::getService( rtl::OUString sServiceName )
{
	Reference< XInterface > xIface;

	bool bManagedConnection = true;
	if ( !m_bConnected )
	{
#if DEBUG
		fprintf( stderr, "Trying to (re)connect\n" );
#endif
		bManagedConnection = connect();
	}

	if ( bManagedConnection )
		xIface = m_xServiceMngr->createInstanceWithContext( sServiceName, m_xCtx );

	return xIface;
}

void AbstractConnection::disposing( const EventObject& source ) throw ( RuntimeException )
{
	m_bConnected = false;
#if DEBUG
	if ( m_bExpectingDisconnect )
	{
		fprintf( stderr, "Disconnected\n" );
	}
	else
	{
		fprintf( stderr, "Disconnected unexpectedly\n" );
	}
#endif
	m_bExpectingDisconnect = false;
}

} // End of namespace unoclienthelper
