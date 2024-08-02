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

#ifndef INCLUDED_SVX_XBTMPIT_HXX
#define INCLUDED_SVX_XBTMPIT_HXX

#include <svx/svxdllapi.h>
#include <svx/xdef.hxx>
#include <svx/xit.hxx>
#include <vcl/GraphicObject.hxx>

class SdrModel;


class SVXCORE_DLLPUBLIC XFillBitmapItem final : public NameOrIndex
{
private:
    GraphicObject   maGraphicObject;

public:
            static SfxPoolItem* CreateDefault();
            XFillBitmapItem() : NameOrIndex(XATTR_FILLBITMAP, -1 ) {}
            XFillBitmapItem(const OUString& rName, const GraphicObject& rGraphicObject);
            XFillBitmapItem( const GraphicObject& rGraphicObject );
            XFillBitmapItem( const XFillBitmapItem& rItem );

    virtual bool            operator==( const SfxPoolItem& rItem ) const override;
    virtual XFillBitmapItem* Clone( SfxItemPool* pPool = nullptr ) const override;

    virtual bool            QueryValue( css::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const override;
    virtual bool            PutValue( const css::uno::Any& rVal, sal_uInt8 nMemberId ) override;

    virtual bool GetPresentation( SfxItemPresentation ePres,
                                  MapUnit eCoreMetric,
                                  MapUnit ePresMetric,
                                  OUString &rText, const IntlWrapper& ) const override;

    const GraphicObject& GetGraphicObject() const { return maGraphicObject;}
    bool isPattern() const;

    static bool CompareValueFunc( const NameOrIndex* p1, const NameOrIndex* p2 );
    std::unique_ptr<XFillBitmapItem> checkForUniqueItem( SdrModel& rModel ) const;

    virtual void dumpAsXml(xmlTextWriterPtr pWriter) const override;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
