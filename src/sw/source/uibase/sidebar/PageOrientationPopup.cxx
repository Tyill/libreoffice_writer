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
#include <PageOrientationPopup.hxx>
#include "PageOrientationControl.hxx"
#include <vcl/toolbox.hxx>

PageOrientationPopup::PageOrientationPopup(const css::uno::Reference<css::uno::XComponentContext>& rContext)
    : PopupWindowController(rContext, nullptr, OUString())
{
}

void PageOrientationPopup::initialize( const css::uno::Sequence< css::uno::Any >& rArguments )
{
    PopupWindowController::initialize(rArguments);

    ToolBox* pToolBox = nullptr;
    ToolBoxItemId nId;
    if (getToolboxId(nId, &pToolBox))
        pToolBox->SetItemBits(nId, ToolBoxItemBits::DROPDOWNONLY | pToolBox->GetItemBits(nId));
}

PageOrientationPopup::~PageOrientationPopup()
{
}

std::unique_ptr<WeldToolbarPopup> PageOrientationPopup::weldPopupWindow()
{
    return std::make_unique<sw::sidebar::PageOrientationControl>(this, m_pToolbar);
}

VclPtr<vcl::Window> PageOrientationPopup::createVclPopupWindow( vcl::Window* pParent )
{
    mxInterimPopover = VclPtr<InterimToolbarPopup>::Create(getFrameInterface(), pParent,
        std::make_unique<sw::sidebar::PageOrientationControl>(this, pParent->GetFrameWeld()));

    mxInterimPopover->Show();

    return mxInterimPopover;
}

OUString PageOrientationPopup::getImplementationName()
{
    return u"lo.writer.PageOrientationToolBoxControl"_ustr;
}

css::uno::Sequence<OUString> PageOrientationPopup::getSupportedServiceNames()
{
    return { u"com.sun.star.frame.ToolbarController"_ustr };
}

extern "C" SAL_DLLPUBLIC_EXPORT css::uno::XInterface *
lo_writer_PageOrientationToolBoxControl_get_implementation(
    css::uno::XComponentContext* rContext,
    css::uno::Sequence<css::uno::Any> const & )
{
    return cppu::acquire(new PageOrientationPopup(rContext));
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
