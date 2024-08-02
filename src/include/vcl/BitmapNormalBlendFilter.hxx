/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#ifndef INCLUDED_VCL_BITMAPNORMALBLENDFILTER_HXX
#define INCLUDED_VCL_BITMAPNORMALBLENDFILTER_HXX

#include <vcl/bitmapex.hxx>

class VCL_DLLPUBLIC BitmapNormalBlendFilter
{
private:
    BitmapEx maBitmapEx;
    BitmapEx maBitmapEx2;

public:
    BitmapNormalBlendFilter(BitmapEx const& rBmpEx, BitmapEx const& rBmpEx2);

    ~BitmapNormalBlendFilter();
    BitmapEx execute();
};
#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
