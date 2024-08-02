/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sal/types.h>

#include <utility>
#include <vector>

#include <rtl/strbuf.hxx>
#include <rtl/ustring.hxx>

//#include <com/sun/star/uno/Reference.hxx>

#include <svl/svldllapi.h>

// Is this length truly the maximum possible, or just a number that
// seemed large enough when the author tested this (with some type of
// certificates)? I suspect the latter.

// Used to be 0x4000 = 16384, but a sample signed PDF (produced by
// some other software) provided by the customer has a signature
// content that is 30000 bytes. The SampleSignedPDFDocument.pdf from
// Adobe has one that is 21942 bytes. So let's be careful. Pity this
// can't be dynamic, at least not without restructuring the code. Also
// note that the checks in the code for this being too small
// apparently are broken, if this overflows you end up with an invalid
// PDF. Need to fix that.

#define MAX_SIGNATURE_CONTENT_LENGTH 50000

namespace com::sun::star::security { class XCertificate; }
class SvStream;
struct SignatureInformation;

namespace svl::crypto {

/// Converts a hex-encoded string into a byte array.
SVL_DLLPUBLIC std::vector<unsigned char> DecodeHexString(std::string_view rHex);

/// Helper to cryptographically sign and verify
/// arbitrary data blocks.
class SVL_DLLPUBLIC Signing
{
public:

    Signing(css::uno::Reference<css::security::XCertificate> xCertificate) :
        m_xCertificate(std::move(xCertificate))
    {
    }

    /// Add a range to sign.
    /// Note: for efficiency this takes a naked pointer, which must remain valid
    /// until this object is discarded.
    void AddDataRange(const void* pData, sal_Int32 size)
    {
        m_dataBlocks.emplace_back(pData, size);
    }

    void SetSignTSA(const OUString& tsa) { m_aSignTSA = tsa; }
    void SetSignPassword(const OUString& password) { m_aSignPassword = password; }

    /// Signs one or more data blocks (as a single, contiguous, array).
    /// Returns the signature (in PKCS#7 format) as string (hex).
    bool Sign(OStringBuffer& rCMSHexBuffer);

    /// Verify and get Signature Information given a byte array.
    static bool Verify(const std::vector<unsigned char>& aData,
                       const bool bNonDetached,
                       const std::vector<unsigned char>& aSignature,
                       SignatureInformation& rInformation);

    /// Verify and get Signature Information given a signature and stream.
    static bool Verify(SvStream& rStream,
                       const std::vector<std::pair<size_t, size_t>>& aByteRanges,
                       const bool bNonDetached,
                       const std::vector<unsigned char>& aSignature,
                       SignatureInformation& rInformation);

private:
    /// The certificate to use for signing.
    const css::uno::Reference<css::security::XCertificate> m_xCertificate;

    /// Data blocks (pointer-size pairs).
    std::vector<std::pair<const void*, sal_Int32>> m_dataBlocks;
    OUString m_aSignTSA;
    OUString m_aSignPassword;
};

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

