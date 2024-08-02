/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#pragma once

#include <cassert>

#include <svl/hint.hxx>
#include <svl/broadcast.hxx>
#include <svl/poolitem.hxx>
#include "swdllapi.h"
#include "ring.hxx"
#include <type_traits>
#include <vector>
#include <optional>

class SwModify;
class SwFormat;
class SfxPoolItem;
class SwAttrSet;
class SwCellFrame;
class SwTabFrame;
class SwRowFrame;
class SwTable;

/*
    SwModify and SwClient cooperate in propagating attribute changes.
    If an attribute changes, the change is notified to all dependent
    formats and other interested objects, e.g. Nodes. The clients will detect
    if the change affects them. It could be that the changed attribute is
    overruled in the receiving object so that its change does not become
    effective or that the receiver is not interested in the particular attribute
    in general (though probably in other attributes of the SwModify object they
    are registered in).
    As SwModify objects are derived from SwClient, they can create a chain of SwClient
    objects where changes can get propagated through.
    Each SwClient can be registered at only one SwModify object, while each SwModify
    object is connected to a list of SwClient objects. If an object derived from SwClient
    wants to get notifications from more than one SwModify object, it must create additional
    SwClient objects. The SwDepend class allows to handle their notifications in the same
    notification callback as it forwards the Modify() calls it receives to a "master"
    SwClient implementation.
    The SwIterator class allows to iterate over the SwClient objects registered at an
    SwModify. For historical reasons its ability to use TypeInfo to restrict this iteration
    to objects of a particular type created a lot of code that misuses SwClient-SwModify
    relationships that basically should be used only for Modify/Notify callbacks.
    This is still subject to refactoring.
 */

namespace sw
{
    class ClientIteratorBase;
    class ListenerEntry;
    void ClientNotifyAttrChg(SwModify& rModify, const SwAttrSet& aSet, SwAttrSet& aOld, SwAttrSet& aNew);
    struct SAL_DLLPUBLIC_RTTI LegacyModifyHint final: SfxHint
    {
        LegacyModifyHint(const SfxPoolItem* pOld, const SfxPoolItem* pNew) : SfxHint(SfxHintId::SwLegacyModify), m_pOld(pOld), m_pNew(pNew) {};
        sal_uInt16 GetWhich() const { return m_pOld ? m_pOld->Which() : m_pNew ? m_pNew->Which() : 0; };
        virtual ~LegacyModifyHint() override;
        const SfxPoolItem* m_pOld;
        const SfxPoolItem* m_pNew;
    };
    struct ModifyChangedHint final: SfxHint
    {
        ModifyChangedHint(const SwModify* pNew) : SfxHint(SfxHintId::SwModifyChanged), m_pNew(pNew) {};
        const SwModify* m_pNew;
    };
    // Observer pattern using svl implementation
    // use this instead of SwClient/SwModify wherever possible
    // In writer layout, this might not always be possible,
    // but for listeners outside of it (e.g. unocore) this should be used.
    // The only "magic" signal this class issues is a ModifyChangedHint
    // proclaiming its death. It does NOT however provide a new SwModify for
    // listeners to switch to like the old SwModify/SwClient did, as that leads
    // to madness.
    class SW_DLLPUBLIC BroadcasterMixin {
        SvtBroadcaster m_aNotifier;
        public:
            BroadcasterMixin() = default;
            BroadcasterMixin(BroadcasterMixin const &) = default;
            BroadcasterMixin& operator=(const BroadcasterMixin&)
            {
                return *this; // Listeners are never copied or moved.
            }
            SvtBroadcaster& GetNotifier() { return m_aNotifier; }
    };
    /// refactoring out the same of the more sane SwClient functionality
    class SAL_DLLPUBLIC_RTTI WriterListener
    {
        friend class ::SwModify;
        friend class ::sw::ClientIteratorBase;
        private:
            WriterListener* m_pLeft;
            WriterListener* m_pRight; ///< double-linked list of other clients

            WriterListener(WriterListener const&) = delete;
            WriterListener& operator=(WriterListener const&) = delete;

        protected:
            WriterListener()
                : m_pLeft(nullptr), m_pRight(nullptr)
            {}
            virtual ~WriterListener() COVERITY_NOEXCEPT_FALSE {}
            virtual void SwClientNotify( const SwModify&, const SfxHint& rHint) =0;
        public:
            bool IsLast() const { return !m_pLeft && !m_pRight; }
            virtual const SwCellFrame* DynCastCellFrame() const { return nullptr; }
            virtual const SwTabFrame* DynCastTabFrame() const { return nullptr; }
            virtual const SwRowFrame* DynCastRowFrame() const { return nullptr; }
            virtual const SwTable* DynCastTable() const { return nullptr; }
    };
    enum class IteratorMode { Exact, UnwrapMulti };
}

// SwClient
class SW_DLLPUBLIC SwClient : public ::sw::WriterListener
{
    // avoids making the details of the linked list and the callback method public
    friend class SwModify;
    friend class sw::ClientIteratorBase;
    friend class sw::ListenerEntry;
    template<typename E, typename S, sw::IteratorMode> friend class SwIterator;

    SwModify *m_pRegisteredIn;        ///< event source

protected:
    // single argument ctors shall be explicit.
    inline explicit SwClient( SwModify* pToRegisterIn );

    // write access to pRegisteredIn shall be granted only to the object itself (protected access)
    SwModify* GetRegisteredInNonConst() const { return m_pRegisteredIn; }

    // when overriding this, you MUST call SwClient::SwClientNotify() in the override!
    virtual void SwClientNotify(const SwModify&, const SfxHint& rHint) override;

public:
    SwClient() : m_pRegisteredIn(nullptr) {}
    SwClient(SwClient&&) noexcept;
    virtual ~SwClient() override;


    // in case an SwModify object is destroyed that itself is registered in another SwModify,
    // its SwClient objects can decide to get registered to the latter instead by calling this method
    std::optional<sw::ModifyChangedHint> CheckRegistration( const SfxPoolItem* pOldValue );
    // SwFormat wants to die different than the rest: It wants to reparent every client to its parent
    // and then send a SwFormatChg hint.
    void CheckRegistrationFormat(SwFormat& rOld);

    const SwModify* GetRegisteredIn() const { return m_pRegisteredIn; }
    SwModify* GetRegisteredIn() { return m_pRegisteredIn; }
    void EndListeningAll();
    void StartListeningToSameModifyAs(const SwClient&);


    // get information about attribute
    virtual bool GetInfo( SfxPoolItem& ) const { return true; }
};


// SwModify

// class has a doubly linked list for dependencies
class SW_DLLPUBLIC SwModify: public SwClient
{
    friend class sw::ClientIteratorBase;
    friend void sw::ClientNotifyAttrChg(SwModify&, const SwAttrSet&, SwAttrSet&, SwAttrSet&);
    template<typename E, typename S, sw::IteratorMode> friend class SwIterator;
    sw::WriterListener* m_pWriterListeners;                // the start of the linked list of clients
    bool m_bModifyLocked;         // don't broadcast changes now

    SwModify(SwModify const &) = delete;
    SwModify &operator =(const SwModify&) = delete;
protected:
    virtual void SwClientNotify(const SwModify&, const SfxHint& rHint) override;
public:
    SwModify()
        : SwClient(), m_pWriterListeners(nullptr), m_bModifyLocked(false)
    {}

    // broadcasting mechanism
    virtual void CallSwClientNotify( const SfxHint& rHint ) const;

    virtual ~SwModify() override;

    void Add(SwClient& rDepend);
    void Remove(SwClient& rDepend);
    bool HasWriterListeners() const { return m_pWriterListeners; }
    bool HasOnlyOneListener() const { return m_pWriterListeners && m_pWriterListeners->IsLast(); }

    // get information about attribute
    virtual bool GetInfo( SfxPoolItem& ) const override;

    void LockModify()                   { m_bModifyLocked = true;  }
    void UnlockModify()                 { m_bModifyLocked = false; }
    bool IsModifyLocked() const     { return m_bModifyLocked;  }
};

template<typename TElementType, typename TSource, sw::IteratorMode eMode> class SwIterator;

namespace sw
{

    // this class is part of the migration: it still forwards the "old"
    // SwModify events and announces them both to the old SwClients still
    // registered and also to the new SvtListeners.
    // Still: in the long run the SwClient/SwModify interface should not be
    // used anymore, in which case a BroadcasterMixin should be enough instead
    // then.
    class SW_DLLPUBLIC SAL_LOPLUGIN_ANNOTATE("crosscast") BroadcastingModify :
        public SwModify, public BroadcasterMixin
    {
        public:
            virtual void CallSwClientNotify(const SfxHint& rHint) const override;
    };
    // this should be hidden but sadly SwIterator template needs it...
    class ListenerEntry final : public SwClient
    {
    private:
        template<typename E, typename S, sw::IteratorMode> friend class ::SwIterator;
        SwClient *m_pToTell;

    public:
        ListenerEntry(SwClient *const pTellHim, SwModify *const pDepend)
            : SwClient(pDepend), m_pToTell(pTellHim)
        {}
        ListenerEntry(ListenerEntry const &) = delete;
        ListenerEntry& operator=(ListenerEntry const&) = delete;
        ListenerEntry(ListenerEntry&& other) noexcept
            : SwClient(std::move(other))
            , m_pToTell(other.m_pToTell)
        { }
        ListenerEntry& operator=(ListenerEntry&& other) noexcept
        {
            m_pToTell = other.m_pToTell;
            other.GetRegisteredIn()->Add(*this);
            other.EndListeningAll();
            return *this;
        }

        /** get Client information */
        virtual bool GetInfo( SfxPoolItem& rInfo) const override;
    private:
        virtual void SwClientNotify(const SwModify& rModify, const SfxHint& rHint) override;
    };

    class SW_DLLPUBLIC WriterMultiListener final
    {
        SwClient& m_rToTell;
        std::vector<ListenerEntry> m_vDepends;
        public:
            WriterMultiListener(SwClient& rToTell);
            WriterMultiListener& operator=(WriterMultiListener const&) = delete; // MSVC2015 workaround
            WriterMultiListener(WriterMultiListener const&) = delete; // MSVC2015 workaround
            ~WriterMultiListener();
            void StartListening(SwModify* pDepend);
            void EndListening(SwModify* pDepend);
            bool IsListeningTo(const SwModify* const pDepend) const;
            void EndListeningAll();
    };
    class ClientIteratorBase : public sw::Ring< ::sw::ClientIteratorBase >
    {
            friend void SwModify::Remove(SwClient&);
            friend void SwModify::Add(SwClient&);
        protected:
            const SwModify& m_rRoot;
            // the current object in an iteration
            WriterListener* m_pCurrent;
            // in case the current object is already removed, the next object in the list
            // is marked down to become the current object in the next step
            // this is necessary because iteration requires access to members of the current object
            WriterListener* m_pPosition;
            static SW_DLLPUBLIC ClientIteratorBase* s_pClientIters;

            ClientIteratorBase( const SwModify& rModify )
                : m_rRoot(rModify)
            {
                MoveTo(s_pClientIters);
#if defined __GNUC__ && !defined __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif
                s_pClientIters = this;
#if defined __GNUC__ && !defined __clang__
#pragma GCC diagnostic pop
#endif
                m_pCurrent = m_pPosition = m_rRoot.m_pWriterListeners;
            }
            WriterListener* GetLeftOfPos() { return m_pPosition->m_pLeft; }
            WriterListener* GetRightOfPos() { return m_pPosition->m_pRight; }
            WriterListener* GoStart()
            {
                m_pPosition = m_rRoot.m_pWriterListeners;
                if(m_pPosition)
                    while( m_pPosition->m_pLeft )
                        m_pPosition = m_pPosition->m_pLeft;
                m_pCurrent = m_pPosition;
                return m_pCurrent;
            }
            ~ClientIteratorBase() override
            {
                assert(s_pClientIters);
                if(s_pClientIters == this)
                    s_pClientIters = unique() ? nullptr : GetNextInRing();
                MoveTo(nullptr);
            }
            // return "true" if an object was removed from a client chain in iteration
            // adding objects to a client chain in iteration is forbidden
            // SwModify::Add() asserts this
            bool IsChanged() const { return m_pPosition != m_pCurrent; }
            // ensures the iterator to point at a current client
            WriterListener* Sync() { m_pCurrent = m_pPosition; return m_pCurrent; }
    };
}

namespace sw::detail
{
    // Dynamic casting can be expensive when used a lot, so for certain type combinations,
    // we have faster routines.
    template<typename CastDest>
    inline const CastDest * internal_dyn_cast(const sw::WriterListener * pSource)
    {
        return dynamic_cast<const CastDest *>(pSource);
    }
    template<>
    inline const SwTable* internal_dyn_cast(const sw::WriterListener * pSource)
    {
        return pSource->DynCastTable();
    }
    template<>
    inline const SwCellFrame* internal_dyn_cast(const sw::WriterListener * pSource)
    {
        return pSource->DynCastCellFrame();
    }
    template<>
    inline const SwTabFrame* internal_dyn_cast(const sw::WriterListener * pSource)
    {
        return pSource->DynCastTabFrame();
    }
    template<>
    inline const SwRowFrame* internal_dyn_cast(const sw::WriterListener * pSource)
    {
        return pSource->DynCastRowFrame();
    }
} // namespace sw::detail

template<typename TElementType, typename TSource,
        sw::IteratorMode eMode = sw::IteratorMode::Exact> class SwIterator final
    : private sw::ClientIteratorBase
{
    //static_assert(!std::is_base_of<SwPageDesc,TSource>::value, "SwPageDesc as TSource is deprecated.");
    static_assert(std::is_base_of<SwClient,TElementType>::value, "TElementType needs to be derived from SwClient.");
    static_assert(std::is_base_of<SwModify,TSource>::value, "TSource needs to be derived from SwModify.");
public:
    SwIterator( const TSource& rSrc ) : sw::ClientIteratorBase(rSrc) {}
    TElementType* First()
    {
        GoStart();
        if(!m_pPosition)
            return nullptr;
        m_pCurrent = nullptr;
        return Next();
    }
    TElementType* Next()
    {
        if(!IsChanged())
            m_pPosition = GetRightOfPos();
        sw::WriterListener *pCurrent(m_pPosition);
        while (m_pPosition)
        {
            if (eMode == sw::IteratorMode::UnwrapMulti)
            {
                if (auto const pLE = dynamic_cast<sw::ListenerEntry const*>(m_pPosition))
                {
                    pCurrent = pLE->m_pToTell;
                }
            }
            if (sw::detail::internal_dyn_cast<TElementType>(pCurrent) == nullptr)
            {
                m_pPosition = GetRightOfPos();
                pCurrent = m_pPosition;
            }
            else
                break;
        }
        Sync();
        return static_cast<TElementType*>(pCurrent);
    }
    using sw::ClientIteratorBase::IsChanged;
};

template< typename TSource > class SwIterator<SwClient, TSource> final : private sw::ClientIteratorBase
{
    static_assert(std::is_base_of<SwModify,TSource>::value, "TSource needs to be derived from SwModify");
public:
    SwIterator( const TSource& rSrc ) : sw::ClientIteratorBase(rSrc) {}
    SwClient* First()
        { return static_cast<SwClient*>(GoStart()); }
    SwClient* Next()
    {
        if(!IsChanged())
            m_pPosition = GetRightOfPos();
        return static_cast<SwClient*>(Sync());
    }
    using sw::ClientIteratorBase::IsChanged;
};

SwClient::SwClient( SwModify* pToRegisterIn )
    : m_pRegisteredIn( nullptr )
{
    if(pToRegisterIn)
        pToRegisterIn->Add(*this);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
