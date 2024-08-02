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
#ifndef INCLUDED_SVL_PTITEM_HXX
#define INCLUDED_SVL_PTITEM_HXX

#include <svl/svldllapi.h>
#include <svl/poolitem.hxx>
#include <tools/gen.hxx>
#include <tools/debug.hxx>

class SvStream;

class SVL_DLLPUBLIC SfxPointItem final : public SfxPoolItem
{
    Point                    aVal;

public:
                             static SfxPoolItem* CreateDefault();
                             SfxPointItem();
                             SfxPointItem( sal_uInt16 nWhich, const Point& rVal );

    virtual bool             GetPresentation( SfxItemPresentation ePres,
                                  MapUnit eCoreMetric,
                                  MapUnit ePresMetric,
                                  OUString &rText,
                                  const IntlWrapper& ) const override;

    virtual bool             operator==( const SfxPoolItem& ) const override;

    virtual SfxPointItem*    Clone( SfxItemPool *pPool = nullptr ) const override;

    const Point&             GetValue() const { return aVal; }
            void             SetValue( const Point& rNewVal ) { ASSERT_CHANGE_REFCOUNTED_ITEM; aVal = rNewVal; }

    virtual bool             QueryValue( css::uno::Any& rVal,
                                          sal_uInt8 nMemberId = 0 ) const override;
    virtual bool             PutValue( const css::uno::Any& rVal,
                                          sal_uInt8 nMemberId ) override;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
