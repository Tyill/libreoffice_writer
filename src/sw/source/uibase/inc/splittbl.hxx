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

#include <vcl/weld.hxx>
#include <tblenum.hxx>
#include <unotools/viewoptions.hxx>

class SwWrtShell;

class SwSplitTableDlg final : public weld::GenericDialogController
{
private:
    std::unique_ptr<weld::RadioButton> m_xBoxAttrCopyWithParaRB;
    std::unique_ptr<weld::RadioButton> m_xBoxAttrCopyNoParaRB;
    std::unique_ptr<weld::RadioButton> m_xBorderCopyRB;

    SwWrtShell& m_rShell;
    SplitTable_HeadlineOption m_nSplit;

    // tdf#131759 - remember last used option in split table dialog
    static SplitTable_HeadlineOption m_eRememberedSplitOption;

    void Apply();

public:
    SwSplitTableDlg(weld::Window* pParent, SwWrtShell& rSh);

    virtual short run() override
    {
        short nRet = GenericDialogController::run();
        if (nRet == RET_OK)
            Apply();
        return nRet;
    }

    SplitTable_HeadlineOption GetSplitMode() const
    {
        auto nSplit = SplitTable_HeadlineOption::ContentCopy;
        if (m_xBoxAttrCopyWithParaRB->get_active())
            nSplit = SplitTable_HeadlineOption::BoxAttrAllCopy;
        else if (m_xBoxAttrCopyNoParaRB->get_active())
            nSplit = SplitTable_HeadlineOption::BoxAttrCopy;
        else if (m_xBorderCopyRB->get_active())
            nSplit = SplitTable_HeadlineOption::BorderCopy;

        // tdf#131759 - remember last used option in split table dialog
        m_eRememberedSplitOption = nSplit;

        return nSplit;
    }
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
