#pragma once

#include "ACAPinc.h"
#include "ResourceIds.hpp"
#include "DGModule.hpp"

struct JsonExportSettingsData {
  GS::UniString filePath;
  GS::UniString linkPath;
  bool exportToFile;
  bool exportToLink;
  GS::Array<API_PropertyDefinitionFilter> propertyDefinitionFilters;
  GS::Array<GS::UniString> elemTypeNames;
  bool selectedOnly;
};

class JsonExportDialog :
  public DG::ModalDialog,
  public DG::PanelObserver,
  public DG::CompoundItemObserver,
  public DG::ButtonItemObserver,
  public DG::CheckItemObserver
{
public:
  enum DialogResourceIds
  {
    ExampleDialogResourceId = ID_ADDON_DLG,
    UseSelectionElementsCheckboxId = 1,
    UseAllElementsCheckboxId = 2,
    Separator1_Id = 3,
    UserDefinedCheckboxId = 4,
    FundamentalCheckboxId = 5,
    UserLevelCheckboxId = 6,
    AllPropertyCheckboxId = 7,
    ElementTypesLabelId = 8,
    ElementTypesTextEditId = 9,
    FilePathCheckboxId = 10,
    FilePathTextEditId = 11,
    UrlCheckboxId = 12,
    UrlTextEditId = 13,
    Separator2_Id = 14,
    CancelButtonId = 15,
    OKButtonId = 16
  };

  JsonExportDialog();
  ~JsonExportDialog();

  JsonExportSettingsData GetSettingsData() const;

private:
  virtual void ButtonClicked(const DG::ButtonClickEvent& ev) override;
  virtual void CheckItemChanged(const DG::CheckItemChangeEvent& ev) override;

  void InitDialog();
  void UpdateAvailableElementTypes();

  GS::Array<API_PropertyDefinitionFilter> GetPropertyDefinitionFilters() const;

  DG::CheckBox m_useSelectionElementsCheckbox;
  DG::CheckBox m_useAllElementsCheckbox;
  DG::Separator m_separator1;
  DG::CheckBox m_userDefinedCheckbox;
  DG::CheckBox m_fundamentalCheckbox;
  DG::CheckBox m_userLevelCheckbox;
  DG::CheckBox m_allPropertyCheckbox;
  DG::LeftText m_elementTypesLabel;
  DG::MultiLineEdit m_elementTypesTextEdit;
  DG::CheckBox m_filePathCheckBox;
  DG::MultiLineEdit m_filePathTextEdit;
  DG::CheckBox m_urlCheckBox;
  DG::MultiLineEdit m_urlTextEdit;
  DG::Separator	m_separator2;
  DG::Button m_okButton;
  DG::Button m_cancelButton;
};