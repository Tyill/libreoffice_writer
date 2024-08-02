/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <sal/types.h>
#include <tools/toolsdllapi.h>

/** Unique ID for an object.
 *
 * Generates a unique ID and stores it in a member variable, so the ID returned
 * by getId() is the same as long as the object is alive.
 *
 * ID numbers start with 1.
 *
 */
class TOOLS_DLLPUBLIC UniqueID final
{
private:
    sal_uInt64 mnID;

public:
    UniqueID();

    sal_uInt64 getID() const { return mnID; }
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
