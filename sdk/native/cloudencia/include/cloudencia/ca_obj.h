/* Copyright (C) 2011-2015 Mamadou DIOP
* Copyright (C) 2011-2015 Doubango Telecom <http://www.doubango.org>
*
* This file is part of Open Source Cloudendia WebRTC PaaS.
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
#if !defined(_CLOUDENCIA_OBJECT_H_)
#define _CLOUDENCIA_OBJECT_H_

#include "ca_config.h"

#define CAObjSafeRelease(pObject)	(pObject) = NULL
#define CAObjSafeFree				CAObjSafeRelease

class CLOUDENCIA_API CAObj
{
public:
	CAObj() :m_nRefCount(0) {}
	CAObj(const CAObj &) :m_nRefCount(0) {}
	virtual ~CAObj() {}

public:
	virtual CA_INLINE const char* getObjectId() = 0;
#if !defined(SWIG)
	CA_INLINE int getRefCount() const {
		return m_nRefCount;
	}
	void operator=(const CAObj &) {}
#endif


public:
	CA_INLINE int takeRef() { /*const*/
		ca_atomic_inc(&m_nRefCount);
		return m_nRefCount;
	}
	CA_INLINE int releaseRef() { /*const*/
		if (m_nRefCount) { // must never be equal to zero
			ca_atomic_dec(&m_nRefCount);
		}
		return m_nRefCount;
	}

private:
	volatile long m_nRefCount;
};

//
//	CAObjWrapper declaration
//
template<class CAObjType>
class CAObjWrapper
{

public:
	CA_INLINE CAObjWrapper(CAObjType obj = NULL);
	CA_INLINE CAObjWrapper(const CAObjWrapper<CAObjType> &obj);
	virtual ~CAObjWrapper();

public:
#if defined(SWIG)
	CAObjType unWrap() { return getWrappedObject(); }
#else
	CA_INLINE CAObjWrapper<CAObjType>& operator=(const CAObjType other);
	CA_INLINE CAObjWrapper<CAObjType>& operator=(const CAObjWrapper<CAObjType> &other);
	CA_INLINE bool operator ==(const CAObjWrapper<CAObjType> other) const;
	CA_INLINE bool operator!=(const CAObjWrapper<CAObjType> &other) const;
	CA_INLINE bool operator <(const CAObjWrapper<CAObjType> other) const;
	CA_INLINE CAObjType operator->() const;
	CA_INLINE CAObjType operator*() const;
	CA_INLINE operator bool() const;
#endif

protected:
	int takeRef();
	int releaseRef();

	CAObjType getWrappedObject() const;
	void wrapObject(CAObjType obj);

private:
	CAObjType m_WrappedObject;
};

//
//	CAObjWrapper implementation
//
template<class CAObjType>
CAObjWrapper<CAObjType>::CAObjWrapper(CAObjType obj)
{
	wrapObject(obj), takeRef();
}

template<class CAObjType>
CAObjWrapper<CAObjType>::CAObjWrapper(const CAObjWrapper<CAObjType> &obj)
{
	wrapObject(obj.getWrappedObject()),
		takeRef();
}

template<class CAObjType>
CAObjWrapper<CAObjType>::~CAObjWrapper()
{
	releaseRef(),
		wrapObject(NULL);
}


template<class CAObjType>
int CAObjWrapper<CAObjType>::takeRef()
{
	if (m_WrappedObject /*&& m_WrappedObject->getRefCount() At startup*/) {
		return m_WrappedObject->takeRef();
	}
	return 0;
}

template<class CAObjType>
int CAObjWrapper<CAObjType>::releaseRef()
{
	if (m_WrappedObject && m_WrappedObject->getRefCount()) {
		if (m_WrappedObject->releaseRef() == 0) {
			delete m_WrappedObject, m_WrappedObject = NULL;
		}
		else {
			return m_WrappedObject->getRefCount();
		}
	}
	return 0;
}

template<class CAObjType>
CAObjType CAObjWrapper<CAObjType>::getWrappedObject() const
{
	return m_WrappedObject;
}

template<class CAObjType>
void CAObjWrapper<CAObjType>::wrapObject(const CAObjType obj)
{
	if (obj) {
		if (!(m_WrappedObject = dynamic_cast<CAObjType>(obj))) {
			CA_DEBUG_ERROR("Trying to wrap an object with an invalid type");
		}
	}
	else {
		m_WrappedObject = NULL;
	}
}

template<class CAObjType>
CAObjWrapper<CAObjType>& CAObjWrapper<CAObjType>::operator=(const CAObjType obj)
{
	releaseRef();
	wrapObject(obj), takeRef();
	return *this;
}

template<class CAObjType>
CAObjWrapper<CAObjType>& CAObjWrapper<CAObjType>::operator=(const CAObjWrapper<CAObjType> &obj)
{
	releaseRef();
	wrapObject(obj.getWrappedObject()), takeRef();
	return *this;
}

template<class CAObjType>
bool CAObjWrapper<CAObjType>::operator ==(const CAObjWrapper<CAObjType> other) const
{
	return getWrappedObject() == other.getWrappedObject();
}

template<class CAObjType>
bool CAObjWrapper<CAObjType>::operator!=(const CAObjWrapper<CAObjType> &other) const
{
	return getWrappedObject() != other.getWrappedObject();
}

template<class CAObjType>
bool CAObjWrapper<CAObjType>::operator <(const CAObjWrapper<CAObjType> other) const
{
	return getWrappedObject() < other.getWrappedObject();
}

template<class CAObjType>
CAObjWrapper<CAObjType>::operator bool() const
{
	return (getWrappedObject() != NULL);
}

template<class CAObjType>
CAObjType CAObjWrapper<CAObjType>::operator->() const
{
	return getWrappedObject();
}

template<class CAObjType>
CAObjType CAObjWrapper<CAObjType>::operator*() const
{
	return getWrappedObject();
}

#endif /* _CLOUDENCIA_OBJECT_H_ */
