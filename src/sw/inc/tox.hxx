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
#ifndef INCLUDED_SW_INC_TOX_HXX
#define INCLUDED_SW_INC_TOX_HXX

#include <vector>
#include <optional>

#include <cppuhelper/weakref.hxx>
#include <editeng/svxenum.hxx>
#include <i18nlangtag/lang.h>
#include <o3tl/typed_flags_set.hxx>
#include <sal/log.hxx>
#include <svl/listener.hxx>
#include <svl/poolitem.hxx>
#include <unotools/weakref.hxx>
#include <com/sun/star/text/XDocumentIndexMark.hpp>

#include "calbck.hxx"
#include "hints.hxx"
#include "swtypes.hxx"
#include "toxe.hxx"

class SwTOXType;
class SwTOXMark;
class SwTextTOXMark;
class SwDoc;
class SwRootFrame;
class SwContentFrame;
class SwXDocumentIndexMark;

typedef std::vector<SwTOXMark*> SwTOXMarks;

namespace sw {
    struct CollectTextMarksHint final : SfxHint {
        SwTOXMarks& m_rMarks;
        CollectTextMarksHint(SwTOXMarks& rMarks) : SfxHint(SfxHintId::SwCollectTextMarks), m_rMarks(rMarks) {}
    };
    struct FindContentFrameHint final : SfxHint {
        SwContentFrame*& m_rpContentFrame;
        const SwRootFrame& m_rLayout;
        FindContentFrameHint(SwContentFrame*& rpContentFrame,const SwRootFrame& rLayout)
            : m_rpContentFrame(rpContentFrame)
            , m_rLayout(rLayout)
        {}
    };
    struct CollectTextTOXMarksForLayoutHint final : SfxHint {
        std::vector<std::reference_wrapper<SwTextTOXMark>>& m_rMarks;
        const SwRootFrame* m_pLayout;
        CollectTextTOXMarksForLayoutHint(std::vector<std::reference_wrapper<SwTextTOXMark>>& rMarks, const SwRootFrame* pLayout)
            : SfxHint(SfxHintId::SwCollectTextTOXMarksForLayout), m_rMarks(rMarks), m_pLayout(pLayout) {}
    };
    SW_DLLPUBLIC auto PrepareJumpToTOXMark(SwDoc const& rDoc, std::u16string_view aName)
        -> std::optional<std::pair<SwTOXMark, sal_Int32>>;
}

// Entry of content index, alphabetical index or user defined index

extern const sal_Unicode C_NUM_REPL;
extern const sal_Unicode C_END_PAGE_NUM;

class SW_DLLPUBLIC SwTOXMark final
    : public SfxPoolItem
    , public sw::BroadcastingModify
    , public SvtListener
{
    friend void InitCore();
    friend class SwTextTOXMark;
    friend SwTOXMark* createSwTOXMarkForItemInfoPackage();
    // friend class ItemInfoPackageSwAttributes;

    const SwTOXType* m_pType;
    OUString m_aAltText;    // Text of caption is different.
    OUString m_aPrimaryKey;
    OUString m_aSecondaryKey;

    // three more strings for phonetic sorting
    OUString m_aTextReading;
    OUString m_aPrimaryKeyReading;
    OUString m_aSecondaryKeyReading;

    SwTextTOXMark* m_pTextAttr;

    sal_uInt16  m_nLevel;
    OUString    m_aBookmarkName;
    bool    m_bAutoGenerated;     // generated using a concordance file
    bool    m_bMainEntry;         // main entry emphasized by character style

    unotools::WeakReference<SwXDocumentIndexMark> m_wXDocumentIndexMark;

    SwTOXMark();                    // to create the default attribute in InitCore

    virtual void Notify(const SfxHint& rHint) override;

public:

    // single argument ctors shall be explicit.
    explicit SwTOXMark( const SwTOXType* pTyp );
    virtual ~SwTOXMark() override;

    SwTOXMark( const SwTOXMark& rCopy );
    SwTOXMark& operator=( const SwTOXMark& rCopy );

    // "pure virtual methods" of SfxPoolItem
    virtual bool            operator==( const SfxPoolItem& ) const override;
    virtual SwTOXMark*      Clone( SfxItemPool* pPool = nullptr ) const override;

    void InvalidateTOXMark();

    OUString                GetText(SwRootFrame const* pLayout) const;

    inline bool             IsAlternativeText() const;
    inline const OUString&  GetAlternativeText() const;

    inline void             SetAlternativeText( const OUString& rAlt );

    // content or user defined index
    inline void             SetLevel(sal_uInt16 nLevel);
    inline sal_uInt16       GetLevel() const;
    inline void             SetBookmarkName( const OUString& bName);
    inline const OUString&  GetBookmarkName() const;

    // for alphabetical index only
    inline void             SetPrimaryKey(const OUString& rStr );
    inline void             SetSecondaryKey(const OUString& rStr);
    inline void             SetTextReading(const OUString& rStr);
    inline void             SetPrimaryKeyReading(const OUString& rStr );
    inline void             SetSecondaryKeyReading(const OUString& rStr);

    inline OUString const & GetPrimaryKey() const;
    inline OUString const & GetSecondaryKey() const;
    inline OUString const & GetTextReading() const;
    inline OUString const & GetPrimaryKeyReading() const;
    inline OUString const & GetSecondaryKeyReading() const;

    bool                    IsAutoGenerated() const {return m_bAutoGenerated;}
    void                    SetAutoGenerated(bool bSet) {m_bAutoGenerated = bSet;}

    bool                    IsMainEntry() const {return m_bMainEntry;}
    void                    SetMainEntry(bool bSet) { m_bMainEntry = bSet;}

    inline const SwTOXType*    GetTOXType() const;

    const SwTextTOXMark* GetTextTOXMark() const   { return m_pTextAttr; }
          SwTextTOXMark* GetTextTOXMark()         { return m_pTextAttr; }

    SAL_DLLPRIVATE unotools::WeakReference<SwXDocumentIndexMark> const& GetXTOXMark() const
            { return m_wXDocumentIndexMark; }
    SAL_DLLPRIVATE void SetXTOXMark(rtl::Reference<SwXDocumentIndexMark> const& xMark);

    void RegisterToTOXType( SwTOXType& rMark );

    static constexpr OUString S_PAGE_DELI = u", "_ustr;
};

// index types
class SwTOXType final: public sw::BroadcastingModify
{
public:
    SwTOXType(SwDoc& rDoc, TOXTypes eTyp, OUString aName);

    // @@@ public copy ctor, but no copy assignment?
    SwTOXType(const SwTOXType& rCopy);

    inline const OUString&  GetTypeName() const;
    inline TOXTypes         GetType() const;
    SwDoc& GetDoc() const { return m_rDoc; }
    void CollectTextMarks(SwTOXMarks& rMarks) const
            { const_cast<SwTOXType*>(this)->GetNotifier().Broadcast(sw::CollectTextMarksHint(rMarks)); }
    SwContentFrame* FindContentFrame(const SwRootFrame& rLayout) const
    {
        SwContentFrame* pContentFrame = nullptr;
        const_cast<SwTOXType*>(this)->GetNotifier().Broadcast(sw::FindContentFrameHint(pContentFrame, rLayout));
        return pContentFrame;
    }
    void CollectTextTOXMarksForLayout(std::vector<std::reference_wrapper<SwTextTOXMark>>& rMarks, const SwRootFrame* pLayout) const
            { const_cast<SwTOXType*>(this)->GetNotifier().Broadcast(sw::CollectTextTOXMarksForLayoutHint(rMarks, pLayout)); }


private:
    SwDoc&          m_rDoc;
    OUString        m_aName;
    TOXTypes        m_eType;

    // @@@ public copy ctor, but no copy assignment?
    SwTOXType & operator= (const SwTOXType &) = delete;
};

// Structure of the index lines
#define FORM_TITLE              0
#define FORM_ALPHA_DELIMITER   1
#define FORM_PRIMARY_KEY        2
#define FORM_SECONDARY_KEY      3
#define FORM_ENTRY              4

/*
 Pattern structure

 <E#> - entry number                    <E# CharStyleName,PoolId>
 <ET> - entry text                      <ET CharStyleName,PoolId>
 <E>  - entry text and number           <E CharStyleName,PoolId>
 <T>  - tab stop                        <T,,Position,Adjust>
 <C>  - chapter info n = {0, 1, 2, 3, 4 } values of SwChapterFormat <C CharStyleName,PoolId>
 <TX> - text token                      <X CharStyleName,PoolId, TOX_STYLE_DELIMITERTextContentTOX_STYLE_DELIMITER>
 <#>  - Page number                     <# CharStyleName,PoolId>
 <LS> - Link start                      <LS>
 <LE> - Link end                        <LE>
 <A00> - Authority entry field          <A02 CharStyleName, PoolId>
 */

// These enum values are stored and must not be changed!
enum FormTokenType
{
    TOKEN_ENTRY_NO,
    TOKEN_ENTRY_TEXT,
    TOKEN_ENTRY,
    TOKEN_TAB_STOP,
    TOKEN_TEXT,
    TOKEN_PAGE_NUMS,
    TOKEN_CHAPTER_INFO,
    TOKEN_LINK_START,
    TOKEN_LINK_END,
    TOKEN_AUTHORITY,
    TOKEN_END
};

struct SW_DLLPUBLIC SwFormToken
{
    OUString        sText;
    OUString        sCharStyleName;
    SwTwips         nTabStopPosition;
    FormTokenType   eTokenType;
    sal_uInt16          nPoolId;
    SvxTabAdjust    eTabAlign;
    sal_uInt16          nChapterFormat;     //SwChapterFormat;
    sal_uInt16          nOutlineLevel;//the maximum permitted outline level in numbering
    sal_uInt16          nAuthorityField;    //enum ToxAuthorityField
    sal_Unicode     cTabFillChar;
    bool        bWithTab;      // true: do generate tab
                                   // character only the tab stop
                                   // #i21237#

    SwFormToken(FormTokenType eType ) :
        nTabStopPosition(0),
        eTokenType(eType),
        nPoolId(USHRT_MAX),
        eTabAlign( SvxTabAdjust::Left ),
        nChapterFormat(0 /*CF_NUMBER*/),
        nOutlineLevel(MAXLEVEL),   //default to maximum outline level
        nAuthorityField(0 /*AUTH_FIELD_IDENTIFIER*/),
        cTabFillChar(' '),
        bWithTab(true)  // #i21237#
    {}

    OUString GetString() const;
};

struct SwFormTokenEqualToFormTokenType
{
    FormTokenType eType;

    SwFormTokenEqualToFormTokenType(FormTokenType _eType) : eType(_eType) {}
    bool operator()(const SwFormToken & rToken)
    {
        return rToken.eTokenType == eType;
    }
};

/// Vector of tokens.
typedef std::vector<SwFormToken> SwFormTokens;

/**
   Helper class that converts vectors of tokens to strings and vice
   versa.
 */
class SwFormTokensHelper
{
    /// the tokens
    SwFormTokens m_Tokens;

public:
    /**
       constructor

       @param rStr   string representation of the tokens
    */
    SwFormTokensHelper(std::u16string_view aStr);

    /**
       Returns vector of tokens.

       @return vector of tokens
    */
    const SwFormTokens & GetTokens() const { return m_Tokens; }
};

class SW_DLLPUBLIC SwForm
{
    SwFormTokens    m_aPattern[ AUTH_TYPE_END + 1 ]; // #i21237#
    OUString  m_aTemplate[ AUTH_TYPE_END + 1 ];

    TOXTypes    m_eType;
    sal_uInt16      m_nFormMaxLevel;

    bool    m_bIsRelTabPos : 1;
    bool    m_bCommaSeparated : 1;

public:
    SwForm( TOXTypes eTOXType = TOX_CONTENT );
    SwForm( const SwForm& rForm );

    SwForm& operator=( const SwForm& rForm );

    inline void SetTemplate(sal_uInt16 nLevel, const OUString& rName);
    inline OUString const & GetTemplate(sal_uInt16 nLevel) const;

    // #i21237#
    void    SetPattern(sal_uInt16 nLevel, SwFormTokens&& rName);
    void    SetPattern(sal_uInt16 nLevel, std::u16string_view aStr);
    const SwFormTokens& GetPattern(sal_uInt16 nLevel) const;

    // fill tab stop positions from template to pattern- #i21237#
    void AdjustTabStops( SwDoc const & rDoc );

    inline TOXTypes GetTOXType() const;
    inline sal_uInt16   GetFormMax() const;

    bool IsRelTabPos() const    {   return m_bIsRelTabPos; }
    void SetRelTabPos( bool b ) {   m_bIsRelTabPos = b;       }

    bool IsCommaSeparated() const       { return m_bCommaSeparated;}
    void SetCommaSeparated( bool b)     { m_bCommaSeparated = b;}

    static sal_uInt16 GetFormMaxLevel( TOXTypes eType );

    static OUString GetFormEntry();
    static OUString GetFormTab();
    static OUString GetFormPageNums();
    static OUString GetFormLinkStt();
    static OUString GetFormLinkEnd();
    static OUString GetFormEntryNum();
    static OUString GetFormEntryText();
    static OUString GetFormChapterMark();
    static OUString GetFormText();
    static OUString GetFormAuth();
};

// Content to create indexes of
enum class SwTOXElement : sal_uInt16
{
    NONE                  = 0x0000,
    Mark                  = 0x0001,
    OutlineLevel          = 0x0002,
    Template              = 0x0004,
    Ole                   = 0x0008,
    Table                 = 0x0010,
    Graphic               = 0x0020,
    Frame                 = 0x0040,
    Sequence              = 0x0080,
    TableLeader           = 0x0100,
    TableInToc            = 0x0200,
    Bookmark              = 0x0400,
    Newline               = 0x0800,
    ParagraphOutlineLevel = 0x1000,
};
namespace o3tl {
    template<> struct typed_flags<SwTOXElement> : is_typed_flags<SwTOXElement, 0x3fff> {};
}

enum class SwTOIOptions : sal_uInt16
{
    NONE            = 0x00,
    SameEntry       = 0x01,
    FF              = 0x02,
    CaseSensitive   = 0x04,
    KeyAsEntry      = 0x08,
    AlphaDelimiter  = 0x10,
    Dash            = 0x20,
    InitialCaps     = 0x40,
};
namespace o3tl {
    template<> struct typed_flags<SwTOIOptions> : is_typed_flags<SwTOIOptions, 0x7f> {};
}

//which part of the caption is to be displayed
enum SwCaptionDisplay
{
    CAPTION_COMPLETE,
    CAPTION_NUMBER,
    CAPTION_TEXT
};

enum class SwTOOElements : sal_uInt16
{
    NONE            = 0x00,
    Math            = 0x01,
    Chart           = 0x02,
    Calc            = 0x08,
    DrawImpress     = 0x10,
    Other           = 0x80,
};
namespace o3tl {
    template<> struct typed_flags<SwTOOElements> : is_typed_flags<SwTOOElements, 0x9b> {};
}

#define TOX_STYLE_DELIMITER u'\x0001'

// Class for all indexes
class SW_DLLPUBLIC SwTOXBase : public SwClient
{
    SwForm      m_aForm;              // description of the lines
    OUString    m_aName;              // unique name
    OUString    m_aTitle;             // title
    OUString    m_aBookmarkName;      //Bookmark Name

    OUString    m_sMainEntryCharStyle; // name of the character style applied to main index entries

    OUString    m_aStyleNames[MAXLEVEL]; // (additional) style names TOX_CONTENT, TOX_USER
    OUString    m_sSequenceName;      // FieldTypeName of a caption sequence

    LanguageType    m_eLanguage;
    OUString        m_sSortAlgorithm;

    union {
        sal_uInt16      nLevel;             // consider outline levels
        SwTOIOptions    nOptions;           // options of alphabetical index
    } m_aData;

    SwTOXElement     m_nCreateType;        // sources to create the index from
    SwTOOElements    m_nOLEOptions;        // OLE sources
    SwCaptionDisplay m_eCaptionDisplay;
    bool        m_bProtected : 1;         // index protected ?
    bool        m_bFromChapter : 1;       // create from chapter or document
    bool        m_bFromObjectNames : 1;   // create a table or object index
                                    // from the names rather than the caption
    bool        m_bLevelFromChapter : 1; // User index: get the level from the source chapter

protected:
    // Add a data member, for record the TOC field expression of MS Word binary format
    // For keeping fidelity and may giving a better exporting performance
    OUString maMSTOCExpression;
    bool mbKeepExpression;

public:
    SwTOXBase( const SwTOXType* pTyp, const SwForm& rForm,
               SwTOXElement nCreaType, OUString aTitle );
    SwTOXBase( const SwTOXBase& rCopy, SwDoc* pDoc = nullptr );
    virtual ~SwTOXBase() override;

    virtual void SwClientNotify(const SwModify& rMod, const SfxHint& rHint) override
    {
        if(rHint.GetId() == SfxHintId::SwDocumentDying)
            GetRegisteredIn()->Remove(*this);
        else
            SwClient::SwClientNotify(rMod, rHint);
    }
    // a kind of CopyCtor - check if the TOXBase is at TOXType of the doc.
    // If not, so create it and copy all other used things.
    void                CopyTOXBase( SwDoc*, const SwTOXBase& );

    const SwTOXType*    GetTOXType() const;

    SwTOXElement        GetCreateType() const;      // creation types

    const OUString&     GetTOXName() const {return m_aName;}
    void                SetTOXName(const OUString& rSet) {m_aName = rSet;}

    // for record the TOC field expression of MS Word binary format
    const OUString&     GetMSTOCExpression() const{return maMSTOCExpression;}
    void                SetMSTOCExpression(const OUString& rExp) {maMSTOCExpression = rExp;}
    void                EnableKeepExpression() {mbKeepExpression = true;}
    void                DisableKeepExpression() {mbKeepExpression = false;}

    const OUString&     GetTitle() const;           // Title
    const OUString&     GetBookmarkName() const;
    OUString const &    GetTypeName() const;        // Name
    const SwForm&       GetTOXForm() const;         // description of the lines

    void                SetCreate(SwTOXElement);
    void                SetTitle(const OUString& rTitle);
    void                SetTOXForm(const SwForm& rForm);
    void                SetBookmarkName(const OUString& bName);

    TOXTypes            GetType() const;

    const OUString&     GetMainEntryCharStyle() const {return m_sMainEntryCharStyle;}
    void                SetMainEntryCharStyle(const OUString& rSet)  {m_sMainEntryCharStyle = rSet;}

    // content index only
    inline void             SetLevel(sal_uInt16);                   // consider outline level
    inline sal_uInt16       GetLevel() const;

    // alphabetical index only
    inline SwTOIOptions     GetOptions() const;                 // alphabetical index options
    inline void             SetOptions(SwTOIOptions nOpt);

    // index of objects
    SwTOOElements           GetOLEOptions() const {return m_nOLEOptions;}
    void                    SetOLEOptions(SwTOOElements nOpt) {m_nOLEOptions = nOpt;}

    // index of objects

    OUString const &        GetStyleNames(sal_uInt16 nLevel) const
                                {
                                SAL_WARN_IF( nLevel >= MAXLEVEL, "sw", "Which level?");
                                return m_aStyleNames[nLevel];
                                }
    void                    SetStyleNames(const OUString& rSet, sal_uInt16 nLevel)
                                {
                                SAL_WARN_IF( nLevel >= MAXLEVEL, "sw", "Which level?");
                                m_aStyleNames[nLevel] = rSet;
                                }
    bool                    IsFromChapter() const { return m_bFromChapter;}
    void                    SetFromChapter(bool bSet) { m_bFromChapter = bSet;}

    bool                    IsFromObjectNames() const {return m_bFromObjectNames;}
    void                    SetFromObjectNames(bool bSet) {m_bFromObjectNames = bSet;}

    bool                    IsLevelFromChapter() const {return m_bLevelFromChapter;}
    void                    SetLevelFromChapter(bool bSet) {m_bLevelFromChapter = bSet;}

    bool                    IsProtected() const { return m_bProtected; }
    void                    SetProtected(bool bSet) { m_bProtected = bSet; }

    const OUString&         GetSequenceName() const {return m_sSequenceName;}
    void                    SetSequenceName(const OUString& rSet) {m_sSequenceName = rSet;}

    SwCaptionDisplay        GetCaptionDisplay() const { return m_eCaptionDisplay;}
    void                    SetCaptionDisplay(SwCaptionDisplay eSet) {m_eCaptionDisplay = eSet;}

    bool                    IsTOXBaseInReadonly() const;

    const SfxItemSet*       GetAttrSet() const;
    void                    SetAttrSet( const SfxItemSet& );

    LanguageType    GetLanguage() const {return m_eLanguage;}
    void            SetLanguage(LanguageType nLang)  {m_eLanguage = nLang;}

    const OUString&         GetSortAlgorithm()const {return m_sSortAlgorithm;}
    void            SetSortAlgorithm(const OUString& rSet) {m_sSortAlgorithm = rSet;}
    // #i21237#
    void AdjustTabStops( SwDoc const & rDoc )
    {
        m_aForm.AdjustTabStops( rDoc );
    }

    SwTOXBase& operator=(const SwTOXBase& rSource);
    void RegisterToTOXType( SwTOXType& rMark );
    virtual bool IsVisible() const { return true; }
};

//SwTOXMark

inline const OUString& SwTOXMark::GetAlternativeText() const
    {   return m_aAltText;    }

inline const OUString& SwTOXMark::GetBookmarkName() const
    {   return m_aBookmarkName;    }

inline const SwTOXType* SwTOXMark::GetTOXType() const
    { return m_pType; }

inline bool SwTOXMark::IsAlternativeText() const
    { return !m_aAltText.isEmpty(); }

inline void SwTOXMark::SetAlternativeText(const OUString& rAlt)
{
    m_aAltText = rAlt;
}

inline void SwTOXMark::SetBookmarkName(const OUString& bName)
{
    m_aBookmarkName = bName;
}

inline void SwTOXMark::SetLevel( sal_uInt16 nLvl )
{
    SAL_WARN_IF( GetTOXType() && GetTOXType()->GetType() == TOX_INDEX, "sw", "Wrong type");
    m_nLevel = nLvl;
}

inline void SwTOXMark::SetPrimaryKey( const OUString& rKey )
{
    SAL_WARN_IF( GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    m_aPrimaryKey = rKey;
}

inline void SwTOXMark::SetSecondaryKey( const OUString& rKey )
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    m_aSecondaryKey = rKey;
}

inline void SwTOXMark::SetTextReading( const OUString& rText )
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    m_aTextReading = rText;
}

inline void SwTOXMark::SetPrimaryKeyReading( const OUString& rKey )
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    m_aPrimaryKeyReading = rKey;
}

inline void SwTOXMark::SetSecondaryKeyReading( const OUString& rKey )
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    m_aSecondaryKeyReading = rKey;
}

inline sal_uInt16 SwTOXMark::GetLevel() const
{
    SAL_WARN_IF( GetTOXType() && GetTOXType()->GetType() == TOX_INDEX, "sw", "Wrong type");
    return m_nLevel;
}

inline OUString const & SwTOXMark::GetPrimaryKey() const
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    return m_aPrimaryKey;
}

inline OUString const & SwTOXMark::GetSecondaryKey() const
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    return m_aSecondaryKey;
}

inline OUString const & SwTOXMark::GetTextReading() const
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    return m_aTextReading;
}

inline OUString const & SwTOXMark::GetPrimaryKeyReading() const
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    return m_aPrimaryKeyReading;
}

inline OUString const & SwTOXMark::GetSecondaryKeyReading() const
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    return m_aSecondaryKeyReading;
}

//SwForm

inline void SwForm::SetTemplate(sal_uInt16 nLevel, const OUString& rTemplate)
{
    SAL_WARN_IF(nLevel >= GetFormMax(), "sw", "Index >= GetFormMax()");
    m_aTemplate[nLevel] = rTemplate;
}

inline OUString const & SwForm::GetTemplate(sal_uInt16 nLevel) const
{
    SAL_WARN_IF(nLevel >= GetFormMax(), "sw", "Index >= GetFormMax()");
    return m_aTemplate[nLevel];
}

inline TOXTypes SwForm::GetTOXType() const
{
    return m_eType;
}

inline sal_uInt16 SwForm::GetFormMax() const
{
    return m_nFormMaxLevel;
}

//SwTOXType

inline const OUString& SwTOXType::GetTypeName() const
    {   return m_aName;   }

inline TOXTypes SwTOXType::GetType() const
    {   return m_eType;   }

// SwTOXBase

inline const SwTOXType* SwTOXBase::GetTOXType() const
    { return static_cast<const SwTOXType*>(GetRegisteredIn()); }

inline SwTOXElement SwTOXBase::GetCreateType() const
    { return m_nCreateType; }

inline const OUString& SwTOXBase::GetTitle() const
    { return m_aTitle; }

inline const OUString& SwTOXBase::GetBookmarkName() const
    { return m_aBookmarkName; }

inline OUString const & SwTOXBase::GetTypeName() const
    { return GetTOXType()->GetTypeName();  }

inline const SwForm& SwTOXBase::GetTOXForm() const
    { return m_aForm; }

inline void SwTOXBase::SetCreate(SwTOXElement nCreate)
    { m_nCreateType = nCreate; }

inline void SwTOXBase::SetTOXForm(const SwForm& rForm)
    {  m_aForm = rForm; }

inline TOXTypes SwTOXBase::GetType() const
    { return GetTOXType()->GetType(); }

inline void SwTOXBase::SetLevel(sal_uInt16 nLev)
{
    SAL_WARN_IF(GetTOXType()->GetType() == TOX_INDEX, "sw", "Wrong type");
    m_aData.nLevel = nLev;
}

inline sal_uInt16 SwTOXBase::GetLevel() const
{
    SAL_WARN_IF(GetTOXType()->GetType() == TOX_INDEX, "sw", "Wrong type");
    return m_aData.nLevel;
}

inline SwTOIOptions SwTOXBase::GetOptions() const
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    return m_aData.nOptions;
}

inline void SwTOXBase::SetOptions(SwTOIOptions nOpt)
{
    SAL_WARN_IF(GetTOXType()->GetType() != TOX_INDEX, "sw", "Wrong type");
    m_aData.nOptions = nOpt;
}

#endif // INCLUDED_SW_INC_TOX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
