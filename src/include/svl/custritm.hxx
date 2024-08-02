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

#ifndef INCLUDED_SVL_CUSTRITM_HXX
#define INCLUDED_SVL_CUSTRITM_HXX

#include <svl/svldllapi.h>
#include <svl/poolitem.hxx>
#include <cassert>
#include <utility>

class SVL_DLLPUBLIC CntUnencodedStringItem: public SfxPoolItem
{
    OUString m_aValue;

public:

    CntUnencodedStringItem(sal_uInt16 which, SfxItemType eItemType = SfxItemType::CntUnencodedStringItemType)
        : SfxPoolItem(which, eItemType)
    {}

    CntUnencodedStringItem(sal_uInt16 which, OUString aTheValue, SfxItemType eItemType = SfxItemType::CntUnencodedStringItemType):
        SfxPoolItem(which, eItemType), m_aValue(std::move(aTheValue))
    {}

    virtual bool operator ==(const SfxPoolItem & rItem) const override;

    virtual bool GetPresentation(SfxItemPresentation,
                                 MapUnit, MapUnit,
                                 OUString & rText,
                                 const IntlWrapper&) const override;

    virtual bool QueryValue(css::uno::Any& rVal,
                            sal_uInt8 nMemberId = 0) const override;

    virtual bool PutValue(const css::uno::Any& rVal, sal_uInt8 nMemberId) override;

    virtual CntUnencodedStringItem* Clone(SfxItemPool * = nullptr) const override;

    const OUString & GetValue() const { return m_aValue; }

    void SetValue(const OUString & rTheValue) { ASSERT_CHANGE_REFCOUNTED_ITEM; m_aValue = rTheValue; }
};

#endif // INCLUDED_SVL_CUSTRITM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
