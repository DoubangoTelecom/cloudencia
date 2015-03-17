/* Copyright (C) 2011-2015 Mamadou DIOP
* Copyright (C) 2011-2015 Doubango Telecom <http://www.doubango.org>
*
* This file is part of Open Source Cloudencia WebRTC PaaS.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*/
#include "cloudencia/ca_nettransport.h"

#include "tnet_transport.h"

#include "tsk_buffer.h"
#include "tsk_memory.h"
#include "tsk_base64.h"

#include <assert.h>

#if !defined(kCAMaxStreamBufferSize)
#	define kCAMaxStreamBufferSize 0xFFFF
#endif

//
//	CANetPeer
//

CANetPeer::CANetPeer(CANetFd nFd, bool bConnected /*= false*/, const void* pcData /*= NULL*/, size_t nDataSize /*= 0*/)
{
    m_bConnected = bConnected;
    m_nFd = nFd;
    m_pWrappedBuffer = tsk_buffer_create(pcData, nDataSize);
    m_bRawContent = false;
    m_pWsKey = tsk_null;
}

CANetPeer::~CANetPeer()
{
    TSK_OBJECT_SAFE_FREE(m_pWrappedBuffer);
    TSK_FREE(m_pWsKey);
}

const void* CANetPeer::getDataPtr()
{
    return m_pWrappedBuffer ? m_pWrappedBuffer->data : NULL;
}

size_t CANetPeer::getDataSize()
{
    return m_pWrappedBuffer ? m_pWrappedBuffer->size : 0;
}

#if 0
// IMPORTANT: data sent using this function will never be encrypted
bool CANetPeer::sendData(const void* pcDataPtr, size_t nDataSize)
{
    if (!pcDataPtr || !nDataSize) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameNetTransport, "Invalid parameter");
        return false;
    }
    return (tnet_sockfd_send(getFd(), pcDataPtr, nDataSize, 0) == nDataSize);
}
#endif

bool CANetPeer::buildWsKey()
{
    char WsKey[30];
    const int count = sizeof(WsKey) / sizeof(WsKey[0]);
    for (int i = 0; i < count - 1; ++i) {
        WsKey[i] = rand() % 0xFF;
    }
    WsKey[count - 1] = '\0';

    return tsk_base64_encode((const uint8_t*)WsKey, (count - 1), &m_pWsKey) > 0;
}


//
//	CANetPeerStream
//

bool CANetPeerStream::appenData(const void* pcData, size_t nDataSize)
{
    return m_pWrappedBuffer ? tsk_buffer_append(m_pWrappedBuffer, pcData, nDataSize) == 0 : false;
}

bool CANetPeerStream::remoteData(size_t nPosition, size_t nSize)
{
    return m_pWrappedBuffer ? tsk_buffer_remove(m_pWrappedBuffer, nPosition, nSize) == 0 : false;
}

bool CANetPeerStream::cleanupData()
{
    return m_pWrappedBuffer ? tsk_buffer_cleanup(m_pWrappedBuffer) == 0 : false;
}


//
//	CANetTransport
//
CANetTransport::CANetTransport(CANetTransporType_t eType, const char* pcLocalIP, unsigned short nLocalPort)
    : m_bValid(false)
    , m_bStarted(false)
{
    m_eType = eType;
    const char *pcDescription;
    tnet_socket_type_t eSocketType;
    bool bIsIPv6 = false;

    if (pcLocalIP && nLocalPort) {
        bIsIPv6 = (tnet_get_family(pcLocalIP, nLocalPort) == AF_INET6);
    }

    switch (eType) {
    case CANetTransporType_TCP:
    case CANetTransporType_WS: {
        pcDescription = bIsIPv6 ? "TCP/IPv6 transport" : "TCP/IPv4 transport";
        eSocketType = bIsIPv6 ? tnet_socket_type_tcp_ipv6 : tnet_socket_type_tcp_ipv4;
        break;
    }
    case CANetTransporType_TLS:
    case CANetTransporType_WSS: {
        pcDescription = bIsIPv6 ? "TLS/IPv6 transport" : "TLS/IPv4 transport";
        eSocketType = bIsIPv6 ? tnet_socket_type_tls_ipv6 : tnet_socket_type_tls_ipv4;
        break;
    }
    default: {
        CA_ASSERT(false);
        return;
    }
    }

    if ((m_pWrappedTransport = tnet_transport_create(pcLocalIP, nLocalPort, eSocketType, pcDescription))) {
        if (TNET_SOCKET_TYPE_IS_STREAM(eSocketType)) {
            tnet_transport_set_callback(m_pWrappedTransport, CANetTransport::CANetTransportCb_Stream, this);
        }
        else {
            CA_ASSERT(false);
            return;
        }
    }

    m_oPeersMutex = new CAMutex();

    m_bValid = (m_oPeersMutex && m_pWrappedTransport);
}

CANetTransport::~CANetTransport()
{
    stop();
    TSK_OBJECT_SAFE_FREE(m_pWrappedTransport);
    CA_DEBUG_INFO("*** CANetTransport destroyed ***");
}

bool CANetTransport::setSSLCertificates(const char* pcPrivateKey, const char* pcPublicKey, const char* pcCA, bool bVerify /*= false*/)
{
    return (tnet_transport_tls_set_certs(m_pWrappedTransport, pcCA, pcPublicKey, pcPrivateKey, (bVerify ? tsk_true : tsk_false)) == 0);
}

bool CANetTransport::start()
{
    m_bStarted = (tnet_transport_start(m_pWrappedTransport) == 0);
    return m_bStarted;
}

CANetFd CANetTransport::connectTo(const char* pcHost, unsigned short nPort)
{
    if (!pcHost || !nPort) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameNetTransport, "Invalid parameter");
        return (CANetFd)TNET_INVALID_FD;
    }
    if (!isValid()) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameNetTransport, "Transport not valid");
        return (CANetFd)TNET_INVALID_FD;
    }
    if (!isStarted()) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameNetTransport, "Transport not started");
        return (CANetFd)TNET_INVALID_FD;
    }

    return tnet_transport_connectto_2(m_pWrappedTransport, pcHost, nPort);
}

bool CANetTransport::isConnected(CANetFd nFd)
{
    CAObjWrapper<CANetPeer*> oPeer = getPeerByFd(nFd);
    return (oPeer && oPeer->isConnected());
}

bool CANetTransport::sendData(CANetFd nFdFrom, const void* pcDataPtr, size_t nDataSize)
{
    if (!pcDataPtr || !nDataSize || !CANetFd_IsValid(nFdFrom)) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameNetTransport, "Invalid parameter");
        return false;
    }
    return (tnet_transport_send(m_pWrappedTransport, nFdFrom, pcDataPtr, nDataSize) == nDataSize);
}

bool CANetTransport::sendData(CAObjWrapper<CANetPeer*> oPeer, const void* pcDataPtr, size_t nDataSize)
{
    return sendData(oPeer->getFd(), pcDataPtr, nDataSize);
}

bool CANetTransport::close(CANetFd nFd)
{
    return (tnet_transport_remove_socket(m_pWrappedTransport, &nFd) == 0);
}

bool CANetTransport::stop()
{
    m_bStarted = false;
    return (tnet_transport_shutdown(m_pWrappedTransport) == 0);
}

CAObjWrapper<CANetPeer*> CANetTransport::getPeerByFd(CANetFd nFd)
{
    m_oPeersMutex->lock();
    CAObjWrapper<CANetPeer*> m_Peer = NULL;

    std::map<CANetFd, CAObjWrapper<CANetPeer*> >::iterator iter = m_Peers.find(nFd);
    if (iter != m_Peers.end()) {
        m_Peer = iter->second;
    }
    m_oPeersMutex->unlock();

    return m_Peer;
}

void CANetTransport::insertPeer(CAObjWrapper<CANetPeer*> oPeer)
{
    m_oPeersMutex->lock();
    if (oPeer) {
        m_Peers.insert(std::pair<CANetFd, CAObjWrapper<CANetPeer*> >(oPeer->getFd(), oPeer));
    }
    m_oPeersMutex->unlock();
}

void CANetTransport::removePeer(CANetFd nFd)
{
    m_oPeersMutex->lock();
    std::map<CANetFd, CAObjWrapper<CANetPeer*> >::iterator iter;
    if ((iter = m_Peers.find(nFd)) != m_Peers.end()) {
        CAObjWrapper<CANetPeer*> oPeer = iter->second;
        m_Peers.erase(iter);
    }
    m_oPeersMutex->unlock();
}

int CANetTransport::CANetTransportCb_Stream(const tnet_transport_event_t* e)
{
    CAObjWrapper<CANetPeer*> oPeer = NULL;
    CANetTransport* This = (CANetTransport*)e->callback_data;

    switch (e->type) {
    case event_removed:
    case event_closed: {
        oPeer = (This)->getPeerByFd(e->local_fd);
        if (oPeer) {
            oPeer->setConnected(false);
            (This)->removePeer(e->local_fd);
            if ((This)->m_oCallback) {
                (This)->m_oCallback->onConnectionStateChanged(oPeer);
            }
        }
        break;
    }
    case event_connected:
    case event_accepted: {
        oPeer = (This)->getPeerByFd(e->local_fd);
        if (oPeer) {
            oPeer->setConnected(true);
        }
        else {
            oPeer = new CANetPeerStream(e->local_fd, true);
            (This)->insertPeer(oPeer);
        }
        if ((This)->m_oCallback) {
            (This)->m_oCallback->onConnectionStateChanged(oPeer);
        }
        break;
    }

    case event_data: {
        oPeer = (This)->getPeerByFd(e->local_fd);
        if (!oPeer) {
            CA_DEBUG_ERROR_EX(kCAMobuleNameNetTransport, "Data event but no peer found!");
            return -1;
        }

        size_t nConsumedBytes = oPeer->getDataSize();
        if ((nConsumedBytes + e->size) > kCAMaxStreamBufferSize) {
            CA_DEBUG_ERROR_EX(kCAMobuleNameNetTransport, "Stream buffer too large[%u > %u]. Did you forget to consume the bytes?", (nConsumedBytes + e->size), kCAMaxStreamBufferSize);
            dynamic_cast<CANetPeerStream*>(*oPeer)->cleanupData();
        }
        else {
            if ((This)->m_oCallback) {
                if (dynamic_cast<CANetPeerStream*>(*oPeer)->appenData(e->data, e->size)) {
                    nConsumedBytes += e->size;
                }
                (This)->m_oCallback->onData(oPeer, nConsumedBytes);
            }
            if (nConsumedBytes) {
                dynamic_cast<CANetPeerStream*>(*oPeer)->remoteData(0, nConsumedBytes);
            }
        }
        break;
    }

    case event_error:
    default: {
        break;
    }
    }

    return 0;
}