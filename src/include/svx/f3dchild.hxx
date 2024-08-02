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

#ifndef INCLUDED_SVX_F3DCHILD_HXX
#define INCLUDED_SVX_F3DCHILD_HXX

#include <sfx2/childwin.hxx>
#include <svx/svxdllapi.h>

/*************************************************************************
|*
|* Derived from SfxChildWindow as "container" for 3D Window
|*
\************************************************************************/

class SAL_WARN_UNUSED SVX_DLLPUBLIC Svx3DChildWindow final : public SfxChildWindow
{
public:
    Svx3DChildWindow(vcl::Window*, sal_uInt16, SfxBindings*, SfxChildWinInfo*);

    SFX_DECL_CHILDWINDOW_WITHID(Svx3DChildWindow);
};

#endif // INCLUDED_SVX_F3DCHILD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
