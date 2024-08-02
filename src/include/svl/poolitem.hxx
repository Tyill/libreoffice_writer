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

#ifndef INCLUDED_SVL_POOLITEM_HXX
#define INCLUDED_SVL_POOLITEM_HXX

#include <sal/config.h>

#include <memory>
#include <vector>

//#include <com/sun/star/uno/Any.hxx>
#include <svl/hint.hxx>
#include <svl/svldllapi.h>
#include <svl/typedwhich.hxx>
#include <tools/mapunit.hxx>
#include <tools/long.hxx>
#include <boost/property_tree/ptree_fwd.hpp>
#include <unordered_set>
#include <unordered_map>

class IntlWrapper;

#define SFX_ITEMS_MAXREF                    0xffffffff
#define CONVERT_TWIPS                       0x80    // Uno conversion for measurement (for MemberId)

// warning, if there is no boolean inside the any this will always return the value false
inline bool Any2Bool( const css::uno::Any&rValue )
{
    bool bValue = false;
    if( !(rValue >>= bValue) )
    {
        sal_Int32 nNum = 0;
        if( rValue >>= nNum )
            bValue = nNum != 0;
    }

    return bValue;
}

// Offer simple assert if Item is RefCounted (RefCnt > 1) and thus CANNOT be changed.
// This should be used at *all* SfxPoolItem set* methods. Remember that SfxPoolItems
// are by design intended to be create-one, read-only, shared data packages
#define ASSERT_CHANGE_REFCOUNTED_ITEM \
    assert(!GetRefCount() && "ERROR: RefCounted SfxPoolItem CANNOT be changed (!)")

/*
 * The values of this enum describe the degree of textual
 * representation of an item after calling the virtual
 * method <SfxPoolItem::GetPresentation()const>.
 */
enum class SfxItemPresentation
{
    Nameless,
    Complete
};

/**
 * These values have to match the values in the
 * css::frame::status::ItemState IDL
 * to be found at offapi/com/sun/star/frame/status/ItemState.idl
*/
enum class SfxItemState {

    /** Specifies an unknown state. */
    UNKNOWN  = 0,

    /** Specifies that the property is currently disabled. */
    DISABLED = 0x0001,

    /** Specifies that the property is currently in a don't care state
     * and thus invalid
     * <br/>
     * This is normally used if a selection provides more than one state
     * for a property at the same time, so the Item is over-defined and
     * has no valid state -> invalid
     */
    INVALID = 0x0010,

    /** Specifies that the property is currently in a default state. */
    DEFAULT  = 0x0020,

    /** The property has been explicitly set to a given value hence we know
     * we are not taking the default value.
     * <br/>
     * For example, you may want to get the font color and it might either
     * be the default one or one that has been explicitly set.
    */
    SET      = 0x0040
};

enum class SfxItemType : sal_uInt16 {
    AffineMatrixItemType,
    CntByteItemType,
    CntInt32ItemType,
    CntUInt16ItemType,
    CntUInt32ItemType,
    CntUnencodedStringItemType,
    DatabaseMapItemType,
    DbuTypeCollectionItemType,
    DriverPoolingSettingsItemType,
    InvalidOrDisabledItemType,
    MediaItemType,
    NameOrIndexType,
    OStringListItemType,
    OfaPtrItemType,
    OfaXColorListItemType,
    OptionalBoolItemType,
    RectangleAlignmentType,
    SbxItemType,
    ScCondFormatItemType,
    ScConsolidateItemType,
    ScHyphenateCellType,
    ScIndentItemType,
    ScInputStatusItemType,
    ScLineBreakCellType,
    ScMergeAttrType,
    ScPageHFItemType,
    ScPageScaleToItemType,
    ScPivotItemType,
    ScProtectionAttrType,
    ScQueryItemType,
    ScRotateValueItemType,
    ScShrinkToFitCellType,
    ScSolveItemType,
    ScSortItemType,
    ScSubTotalItemType,
    ScTabOpItemType,
    ScTpCalcItemType,
    ScTpDefaultsItemType,
    ScTpFormulaItemType,
    ScTpPrintItemType,
    ScTpViewItemType,
    ScUserListItemType,
    ScVerticalStackCellType,
    ScViewObjectModeItemType,
    SdOptionsMiscItemType,
    SdOptionsPrintItemType,
    SdrAllPositionXItemType,
    SdrAllPositionYItemType,
    SdrAllSizeHeightItemType,
    SdrAllSizeWidthItemType,
    SdrAngleItemType,
    SdrCaptionAngleItemType,
    SdrCaptionGapItemType,
    SdrCaptionEscAbsItemType,
    SdrCaptionEscDirItemType,
    SdrCaptionEscRelItemType,
    SdrCaptionEscIsRelItemType,
    SdrCaptionFitLineLenItemType,
    SdrCaptionLineLenItemType,
    SdrCaptionTypeItemType,
    SdrCircKindItemType,
    SdrCustomShapeGeometryItemType,
    SdrEdgeKindItemType,
    SdrEdgeLineDeltaCountItemType,
    SdrEdgeNode1GlueDistItemType,
    SdrEdgeNode1HorzDistItemType,
    SdrEdgeNode1VertDistItemType,
    SdrEdgeNode2GlueDistItemType,
    SdrEdgeNode2HorzDistItemType,
    SdrEdgeNode2VertDistItemType,
    SdrFractionItemType,
    SdrGrafBlueItemType,
    SdrGrafContrastItemType,
    SdrGrafGamma100ItemType,
    SdrGrafGreenItemType,
    SdrGrafLuminanceItemType,
    SdrGrafRedItemType,
    SdrGrafInvertItemType,
    SdrGrafModeItem_Base,
    SdrGrafTransparenceItemType,
    SdrLayerIdItemType,
    SdrLayerNameItemType,
    SdrLogicSizeHeightItemType,
    SdrLogicSizeWidthItemType,
    SdrMeasureBelowRefEdgeItemType,
    SdrMeasureDecimalPlacesItemType,
    SdrMeasureFormatStringItemType,
    SdrMeasureKindItemType,
    SdrMeasureOverhangItemType,
    SdrMeasureScaleItemType,
    SdrMeasureTextAutoAngleItemType,
    SdrMeasureTextAutoAngleViewItemType,
    SdrMeasureTextFixedAngleItemType,
    SdrMeasureTextHPosItemType,
    SdrMeasureTextIsFixedAngleItemType,
    SdrMeasureTextRota90ItemType,
    SdrMeasureTextUpsideDownItemType,
    SdrMeasureTextVPosItemType,
    SdrMeasureUnitItemType,
    SdrMetricItemType,
    SdrMoveXItemType,
    SdrMoveYItemType,
    SdrObjPrintableItemType,
    SdrObjVisibleItemType,
    SdrOnePositionXItemType,
    SdrOnePositionYItemType,
    SdrOneSizeHeightItemType,
    SdrOneSizeWidthItemType,
    SdrOnOffItemType,
    SdrPercentItemType,
    SdrResizeXAllItemType,
    SdrResizeXOneItemType,
    SdrResizeYAllItemType,
    SdrResizeYOneItemType,
    SdrRotateAllItemType,
    SdrRotateOneItemType,
    SdrScaleItemType,
    SdrShearAngleItemType,
    SdrSignedPercentItemType,
    SdrTextAniAmountItemType,
    SdrTextAniCountItemType,
    SdrTextAniDelayItemType,
    SdrTextAniDirectionItemType,
    SdrTextAniStartInsideItemType,
    SdrTextAniStopInsideItemType,
    SdrTextAniKindItemType,
    SdrTextFitToSizeTypeItemType,
    SdrTextFixedCellHeightItemType,
    SdrTextHorzAdjustType,
    SdrTextVertAdjustType,
    SdrTransformRef1XItemType,
    SdrTransformRef1YItemType,
    SdrTransformRef2XItemType,
    SdrTransformRef2YItemType,
    SdrHorzShearAllItemType,
    SdrVertShearAllItemType,
    SdrHorzShearOneItemType,
    SdrVertShearOneItemType,
    SdrYesNoItemType,
    SfxBoolItemType,
    SfxByteItemType,
    SfxDocumentInfoItemType,
    SfxEventNamesItemType,
    SfxFlagItemType,
    SfxFrameItemType,
    SfxGlobalNameItemType,
    SfxGrabBagItemType,
    SfxImageItemType,
    SfxInt16ItemType,
    SfxInt32ItemType,
    SfxInt64ItemType,
    SfxIntegerListItemType,
    SfxLinkItemType,
    SfxLockBytesItemType,
    SfxMacroInfoItemType,
    SfxMetricItemType,
    SfxObjectItemType,
    SfxObjectShellItemType,
    SfxPointItemType,
    SfxRangeItemType,
    SfxRectangleItemType,
    SfxRegionItemType,
    SfxSetItemType,
    SfxStringItemType,
    SfxStringListItemType,
    SfxTabDialogItemType,
    SfxTemplateItemType,
    SfxUInt16ItemType,
    SfxUInt32ItemType,
    SfxUnoAnyItemType,
    SfxUnoFrameItemType,
    SfxViewFrameItemType,
    SfxVisibilityItemType,
    SfxVoidItemType,
    SfxWatermarkItemType,
    SfxZoomItemType,
    Svx3DCharacterModeItemType,
    Svx3DCloseBackItemType,
    Svx3DCloseFrontItemType,
    Svx3DNormalsKindItemType,
    Svx3DPerspectiveItemType,
    Svx3DReducedLineGeometryItemType,
    Svx3DShadeModeItemType,
    Svx3DSmoothLidsItemType,
    Svx3DSmoothNormalsItemType,
    Svx3DTextureKindItemType,
    Svx3DTextureModeItemType,
    Svx3DTextureProjectionXItemType,
    Svx3DTextureProjectionYItemType,
    SvXMLAttrContainerItemType,
    SvxAdjustItemType,
    SvxAutoKernItemType,
    SvxB3DVectorItemType,
    SvxBitmapListItemType,
    SvxBlinkItemType,
    SvxBoxInfoItemType,
    SvxBoxItemType,
    SvxBrushItemType,
    SvxBulletItemType,
    SvxCaseMapItemType,
    SvxCharHiddenItemType,
    SvxCharReliefItemType,
    SvxCharRotateItemType,
    SvxCharScaleWidthItemType,
    SvxChartColorTableItemType,
    SvxChartIndicateItemType,
    SvxChartKindErrorItemType,
    SvxChartRegressItemType,
    SvxChartTextOrderItemType,
    SvxClipboardFormatItemType,
    SvxColorItemType,
    SvxColorListItemType,
    SvxColumnItemType,
    SvxContourItemType,
    SvxCrossedOutItemType,
    SvxDashListItemType,
    SvxDoubleItemType,
    SvxEmphasisMarkItemType,
    SvxEscapementItemType,
    SvxFieldItemType,
    SvxFirstLineIndentItemType,
    SvxFontHeightItemType,
    SvxFontItemType,
    SvxFontListItemType,
    SvxForbiddenRuleItemType,
    SvxFormatBreakItemType,
    SvxFrameDirectionItemType,
    SvxGalleryItemType,
    SvxGradientListItemType,
    SvxGraphicItemType,
    SvxGrfCropType,
    SvxGridItemType,
    SvxGutterLeftMarginItemType,
    SvxGutterRightMarginItemType,
    SvxHangingPunctuationItemType,
    SvxHatchListItemType,
    SvxHorJustifyItemType,
    SvxHyperlinkItemType,
    SvxHyphenZoneItemType,
    SvxJustifyMethodItemType,
    SvxKerningItemType,
    SvxLRSpaceItemType,
    SvxLanguageItemType,
    SvxLeftMarginItemType,
    SvxLineEndListItemType,
    SvxLineItemType,
    SvxLineSpacingItemType,
    SvxLongLRSpaceItemType,
    SvxLongULSpaceItemType,
    SvxMacroItemType,
    SvxMarginItemType,
    SvxNoHyphenItemType,
    SvxNumBulletItemType,
    SvxNumberInfoItemType,
    SvxObjectItemType,
    SvxOpaqueItemType,
    SvxOrientationItemType,
    SvxOrphansItemType,
    SvxOverlineItemType,
    SvxPageItemType,
    SvxPagePosSizeItemType,
    SvxPaperBinItemType,
    SvxParaGridItemType,
    SvxParaVertAlignItemType,
    SvxPatternListItemType,
    SvxPostItDateItemType,
    SvxPostureItemType,
    SvxPrintItemType,
    SvxProtectItemType,
    SvxRightMarginItemType,
    SvxRotateModeItemType,
    SvxRsidItemType,
    SvxScriptSetItemType,
    SvxScriptSpaceItemType,
    SvxSearchItemType,
    SvxShadowItemType,
    SvxShadowedItemType,
    SvxSizeItemType,
    SvxSmartTagItemType,
    SvxStatusItemType,
    SvxTabStopItemType,
    SvxTextLeftMarginItemType,
    SvxTextLineItemType,
    SvxTextRotateItemType,
    SvxTwoLinesItemType,
    SvxUnderlineItemType,
    SvxULSpaceItemType,
    SvxVerJustifyItemType,
    SvxViewLayoutItemType,
    SvxWeightItemType,
    SvxWidowsItemType,
    SvxWordLineItemType,
    SvxWritingModeItemType,
    SvxZoomSliderItemType,
    SwAddPrinterItemType,
    SwChannelGrfType,
    SwCondCollItemType,
    SwContrastGrfType,
    SwCropGrfType,
    SwDocDisplayItemType,
    SwDrawModeGrf_BaseType,
    SwElemItemType,
    SwEnvItemType,
    SwFltAnchorType,
    SwFltBookmarkType,
    SwFltRDFMarkType,
    SwFltRedlineType,
    SwFltTOXType,
    SwFmtAidsAutoComplItemType,
    SwFormatAnchorType,
    SwFormatAutoFormatType,
    SwFormatChainType,
    SwFormatCharFormatType,
    SwFormatColType,
    SwFormatContentControlType,
    SwFormatContentType,
    SwFormatDropType,
    SwFormatEditInReadonly,
    SwFormatFieldType,
    SwFormatFrameSizeType,
    SwFormatFillOrderType,
    SwFormatFlyCntType,
    SwFormatFlySplitType,
    SwFormatFollowTextFlowType,
    SwFormatFooterType,
    SwFormatFootnoteEndAtTextEndType,
    SwFormatFootnoteType,
    SwFormatHeaderType,
    SwFormatHoriOrientType,
    SwFormatINetFormatType,
    SwFormatLayoutSplitType,
    SwFormatLineNumberType,
    SwFormatLinebreakType,
    SwFormatMetaType,
    SwFormatNoBalancedColumnsType,
    SwFormatPageDescType,
    SwFormatRefMarkType,
    SwFormatRowSplitType,
    SwFormatRubyType,
    SwFormatSurroundType,
    SwFormatURLType,
    SwFormatVertOrientType,
    SwFormatWrapInfluenceOnOjPosType,
    SwFormatWrapTextAtFlyStartType,
    SwGammaGrfType,
    SwHeaderAndFooterEatSpacingItemType,
    SwInvertGrfType,
    SwLabItemType,
    SwLuminanceGrfType,
    SwMirrorGrfType,
    SwMsgPoolItemType,
    SwNumRuleItemType,
    SwPaMItemType,
    SwPageFootnoteInfoItemType,
    SwPtrItemType,
    SwRotationGrfType,
    SwShadowCursorItemType,
    SwTOXMarkType,
    SwTableBoxNumFormatType,
    SwTableBoxValueType,
    SwTableFormulaType,
    SwTextGridItemType,
    SwTransparencyGrfType,
    SwUINumRuleItemType,
    SwWrtShellItemType,
    XFillAttrSetItemType,
    XFillBackgroundItemType,
    XFillBmpPosItemType,
    XFillBmpPosOffsetXItemType,
    XFillBmpPosOffsetYItemType,
    XFillBmpSizeLogItemType,
    XFillBmpSizeXItemType,
    XFillBmpSizeYItemType,
    XFillBmpStretchItemType,
    XFillBmpTileItemType,
    XFillBmpTileOffsetXItemType,
    XFillBmpTileOffsetYItemType,
    XFillStyleItemType,
    XFillTransparenceItemType,
    XFillUseSlideBackgroundItemType,
    XFormTextAdjustItemType,
    XFormTextDistanceItemType,
    XFormTextHideFormItemType,
    XFormTextMirrorItemType,
    XFormTextOutlineItemType,
    XFormTextShadowItemType,
    XFormTextShadowTranspItemType,
    XFormTextShadowXValItemType,
    XFormTextShadowYValItemType,
    XFormTextStartItemType,
    XFormTextStyleItemType,
    XGradientStepCountItemType,
    XLineAttrSetItemType,
    XLineCapItemType,
    XLineEndCenterItemType,
    XLineEndWidthItemType,
    XLineJointItemType,
    XLineStartCenterItem,
    XLineStartWidthItemType,
    XLineStyleItemType,
    XLineTransparenceItemType,
    XLineWidthItemType
#ifdef DBG_UTIL
    , SwTestItemType
#endif
};


#ifdef DBG_UTIL
SVL_DLLPUBLIC size_t getAllocatedSfxPoolItemCount();
SVL_DLLPUBLIC size_t getUsedSfxPoolItemCount();
SVL_DLLPUBLIC void listAllocatedSfxPoolItems();
#endif

class SfxItemPool;
class SfxItemSet;
typedef struct _xmlTextWriter* xmlTextWriterPtr;
class ItemInstanceManager;

class SVL_DLLPUBLIC SfxPoolItem
{
    friend class SfxItemPool;
    friend class SfxItemSet;
    friend class InstanceManagerHelper;
    friend class ItemInfoStatic;
    friend class ItemInfoDynamic;

    // allow ItemSetTooling to access
    friend SfxPoolItem const* implCreateItemEntry(SfxItemPool&, SfxPoolItem const*, bool);
    friend void implCleanupItemEntry(SfxPoolItem const*);

    mutable sal_uInt32 m_nRefCount;
    sal_uInt16  m_nWhich;
    SfxItemType m_eItemType;

#ifdef DBG_UTIL
    // for debugging add a serial number, will be set in the constructor
    // and count up from zero. If you have a deterministic error case and
    // see the Item involved in the debugger you can  use that number in
    // the next run to see where that Item gets constructed and how it is
    // involved/ processed
    sal_uInt32  m_nSerialNumber;
#endif

    // bitfield for Item attributes that are Item-Dependent

    // Item is registered at some Pool as default.
    //   m_bStaticDefault: direct Pool Item (CAUTION:
    //     these are not really 'static', but should be)
    //   m_bDynamicDefault: dynamic pool item, e.g.
    //     SfxSetItems which are Pool dependent
    bool        m_bStaticDefault : 1;
    bool        m_bDynamicDefault : 1;

    // Item is derived from SfxSetItem -> is Pool-dependent
    bool        m_bIsSetItem : 1;

    // Defines if the Item can be shared/RefCounted else it will be cloned.
    // Default is true - as it should be for all Items. It is needed by some
    // SW items, so protected to let them set it in constructor. If this could
    // be fixed at that Items we may remove this again.
    bool        m_bShareable : 1;

protected:
#ifdef DBG_UTIL
    // this flag will make debugging item stuff much simpler
    bool        m_bDeleted : 1;
#endif

    void setStaticDefault() { m_bStaticDefault = true; }
    void setDynamicDefault() { m_bDynamicDefault = true; }
    void setIsSetItem() { m_bIsSetItem = true; }
    void setNonShareable() { m_bShareable = false; }

    // access ItemInstanceManager for this Item, default
    // is nullptr. If you overload this it is expected that
    // you return a ptr to a static, Item-local managed
    // instance that exists the whole office lifetime. This
    // usually means to have a static instance directly in the
    // implementation of the overloaded function (just grep
    // for examples)
    virtual ItemInstanceManager* getItemInstanceManager() const;

public:
    inline void AddRef(sal_uInt32 n = 1) const
    {
        assert(n <= SFX_ITEMS_MAXREF - m_nRefCount && "AddRef: refcount overflow");
        m_nRefCount += n;
    }

#ifdef DBG_UTIL
    sal_uInt32 getSerialNumber() const { return m_nSerialNumber; }
#endif

    bool isStaticDefault() const { return m_bStaticDefault; }
    bool isDynamicDefault() const { return m_bDynamicDefault; }
    bool isSetItem() const { return m_bIsSetItem; }
    bool isShareable() const { return m_bShareable; }
    bool isPooled() const { return GetRefCount() > 0; }


    // version that allows nullptrs
    static bool areSame(const SfxPoolItem* pItem1, const SfxPoolItem* pItem2);

    // if you have the items (not nullptrs) use this version
    static bool areSame(const SfxPoolItem& rItem1, const SfxPoolItem& rItem2);

private:
    inline sal_uInt32 ReleaseRef(sal_uInt32 n = 1) const
    {
        assert(n <= m_nRefCount);
        m_nRefCount -= n;
        return m_nRefCount;
    }

protected:

                             explicit SfxPoolItem( sal_uInt16 nWhich, SfxItemType );
                             SfxPoolItem( const SfxPoolItem& rCopy)
                                 : SfxPoolItem(rCopy.m_nWhich, rCopy.m_eItemType) {}

public:
    virtual                  ~SfxPoolItem();

    void                     SetWhich( sal_uInt16 nId )
                             {
                                 // can only change the Which before we are in a set
                                 assert(m_nRefCount==0);
                                 m_nWhich = nId;
                             }
    sal_uInt16               Which() const { return m_nWhich; }
    SfxItemType              ItemType() const { return m_eItemType;}

    // StaticWhichCast asserts if the TypedWhichId is not matching its type, otherwise it returns a reference.
    // You can use StaticWhichCast when you are sure about the type at compile time -- like a static_cast.
    template<class T> T& StaticWhichCast(TypedWhichId<T> nId)
    {
        (void)nId;
        assert(nId == m_nWhich);
        assert(dynamic_cast<T*>(this));
        return *static_cast<T*>(this);
    }
    template<class T> const T& StaticWhichCast(TypedWhichId<T> nId) const
    {
        (void)nId;
        assert(nId == m_nWhich);
        assert(dynamic_cast<const T*>(this));
        return *static_cast<const T*>(this);
    }
    // DynamicWhichCast returns nullptr if the TypedWhichId is not matching its type, otherwise it returns a typed pointer.
    // it asserts if the TypedWhichId matches its Which, but not the RTTI type.
    // You can use DynamicWhichCast when you are not sure about the type at compile time -- like a dynamic_cast.
    template<class T> T* DynamicWhichCast(TypedWhichId<T> nId)
    {
        if(m_nWhich != nId)
            return nullptr;
        assert(dynamic_cast<T*>(this));
        return static_cast<T*>(this);
    }
    template<class T> const T* DynamicWhichCast(TypedWhichId<T> nId) const
    {
        if(m_nWhich != nId)
            return nullptr;
        assert(dynamic_cast<const T*>(this));
        return static_cast<const T*>(this);
    }
    virtual bool             operator==( const SfxPoolItem& ) const = 0;
    bool                     operator!=( const SfxPoolItem& rItem ) const
                             { return !(*this == rItem); }
    // Used by HashedItemInstanceManager
    virtual bool             supportsHashCode() const;
    virtual size_t           hashCode() const;

    /**  @return true if it has a valid string representation */
    virtual bool             GetPresentation( SfxItemPresentation ePresentation,
                                    MapUnit eCoreMetric,
                                    MapUnit ePresentationMetric,
                                    OUString &rText,
                                    const IntlWrapper& rIntlWrapper ) const;

    virtual void             ScaleMetrics( tools::Long lMult, tools::Long lDiv );
    virtual bool             HasMetrics() const;

    virtual bool             QueryValue( css::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
    virtual bool             PutValue( const css::uno::Any& rVal, sal_uInt8 nMemberId );

    virtual SfxPoolItem*     Clone( SfxItemPool *pPool = nullptr ) const = 0;
    // clone and call SetWhich
    std::unique_ptr<SfxPoolItem> CloneSetWhich( sal_uInt16 nNewWhich ) const;
    template<class T> std::unique_ptr<T> CloneSetWhich( TypedWhichId<T> nId ) const
    {
        return std::unique_ptr<T>(static_cast<T*>(CloneSetWhich(sal_uInt16(nId)).release()));
    }

    sal_uInt32               GetRefCount() const { return m_nRefCount; }
    virtual void dumpAsXml(xmlTextWriterPtr pWriter) const;
    virtual boost::property_tree::ptree dumpAsJSON() const;

private:
    SfxPoolItem&             operator=( const SfxPoolItem& ) = delete;
};

// basic Interface definition
class SVL_DLLPUBLIC ItemInstanceManager
{
    // allow *only* ItemSetTooling to access
    friend SfxPoolItem const* implCreateItemEntry(SfxItemPool&, SfxPoolItem const*, bool);
    friend void implCleanupItemEntry(SfxPoolItem const*);

    // Define for which SfxItemType to register
    SfxItemType     m_aSfxItemType;

public:
    ItemInstanceManager(SfxItemType aSfxItemType)
    : m_aSfxItemType(aSfxItemType)
    {
    }
    virtual ~ItemInstanceManager() = default;

    SfxItemType ItemType() const { return m_aSfxItemType; }

private:
    // standard interface, accessed exclusively
    // by implCreateItemEntry/implCleanupItemEntry
    virtual const SfxPoolItem* find(const SfxPoolItem&) const = 0;
    virtual void add(const SfxPoolItem&) = 0;
    virtual void remove(const SfxPoolItem&) = 0;
};

// offering a default implementation that can be use for
// each SfxPoolItem (except when !isShareable()). It just
// uses an unordered_set holding ptrs to SfxPoolItems added
// and SfxPoolItem::operator== to linearly search for one.
// Thus this is not the fastest, but as fast as old 'poooled'
// stuff - better use an intelligent, pro-Item implementation
// that does e.g. hashing or whatever might be feasible for
// that specific Item (see other derivations)
class SVL_DLLPUBLIC DefaultItemInstanceManager : public ItemInstanceManager
{
    std::unordered_map<sal_uInt16, std::unordered_set<const SfxPoolItem*>>  maRegistered;

public:
    DefaultItemInstanceManager(SfxItemType aSfxItemType)
    : ItemInstanceManager(aSfxItemType)
    , maRegistered()
    {
    }

private:
    virtual const SfxPoolItem* find(const SfxPoolItem&) const override;
    virtual void add(const SfxPoolItem&) override;
    virtual void remove(const SfxPoolItem&) override;
};

/**
  Utility template to reduce boilerplate code when creating item instance managers
  for specific PoolItem subclasses that can be hashed which is faster than using
  the linear search with operator== that DefaultItemInstanceManager has to do
*/
class HashedItemInstanceManager final : public ItemInstanceManager
{
    struct ItemHash {
        size_t operator()(const SfxPoolItem* p) const
        {
            return p->hashCode();
        }
    };
    struct ItemEqual {
        bool operator()(const SfxPoolItem* lhs, const SfxPoolItem* rhs) const
        {
            return lhs->Which() == rhs->Which() && (*lhs) == (*rhs);
        }
    };

    std::unordered_set<const SfxPoolItem*, ItemHash, ItemEqual> maRegistered;

public:
    HashedItemInstanceManager(SfxItemType aSfxItemType)
    : ItemInstanceManager(aSfxItemType)
    , maRegistered(0, ItemHash(), ItemEqual())
    {
    }

    // standard interface, accessed exclusively
    // by implCreateItemEntry/implCleanupItemEntry
    virtual const SfxPoolItem* find(const SfxPoolItem& rItem) const override final
    {
        auto aHit(maRegistered.find(&rItem));
        if (aHit != maRegistered.end())
            return *aHit;
        return nullptr;
    }
    virtual void add(const SfxPoolItem& rItem) override final
    {
        maRegistered.insert(&rItem);
    }
    virtual void remove(const SfxPoolItem& rItem) override final
    {
        maRegistered.erase(&rItem);
    }
};


inline bool IsStaticDefaultItem(const SfxPoolItem *pItem )
{
    return pItem && pItem->isStaticDefault();
}

inline bool IsDynamicDefaultItem(const SfxPoolItem *pItem )
{
    return pItem && pItem->isDynamicDefault();
}

inline bool IsDefaultItem( const SfxPoolItem *pItem )
{
    return pItem && (pItem->isStaticDefault() || pItem->isDynamicDefault());
}

SVL_DLLPUBLIC extern SfxPoolItem const * const INVALID_POOL_ITEM;
SVL_DLLPUBLIC extern SfxPoolItem const * const DISABLED_POOL_ITEM;

inline bool IsInvalidItem(const SfxPoolItem *pItem)
{
    return pItem == INVALID_POOL_ITEM;
}

inline bool IsDisabledItem(const SfxPoolItem *pItem)
{
    return pItem == DISABLED_POOL_ITEM;
}

SVL_DLLPUBLIC bool areSfxPoolItemPtrsEqual(const SfxPoolItem* pItem1, const SfxPoolItem* pItem2);

class SVL_DLLPUBLIC SfxPoolItemHint final : public SfxHint
{
    SfxPoolItem* pObj;
public:
    explicit SfxPoolItemHint( SfxPoolItem* Object ) : pObj(Object) {}
    SfxPoolItem* GetObject() const { return pObj; }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
