#pragma once

#include "ACAPinc.h"
#include "ResourceIds.hpp"
#include "DGModule.hpp"

struct JsonExportSettingsData {
  GS::UniString filePath;
  GS::Array<API_PropertyDefinitionFilter> propertyDefinitionFilters;
  GS::Array<GS::UniString> elemTypeNames;
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
    UserDefinedCheckboxId = 1,
    FundamentalCheckboxId = 2,
    UserLevelCheckboxId = 3,
    AllPropertyCheckboxId = 4,
    ElementTypesTextEditId = 5,
    FilePathTextEditId = 6,
    SeparatorId = 7,
    OKButtonId = 8,
    CancelButtonId = 9,
  };

  JsonExportDialog(const GS::UniString& availableElementTypesText);
  ~JsonExportDialog();

  JsonExportSettingsData GetSettingsData() const;

private:
  virtual void ButtonClicked(const DG::ButtonClickEvent& ev) override;
  virtual void CheckItemChanged(const DG::CheckItemChangeEvent& ev) override;

  void InitDialog(const GS::UniString& availableElementTypesText);

  GS::Array<API_PropertyDefinitionFilter> GetPropertyDefinitionFilters() const;

  DG::CheckBox m_userDefinedCheckbox;
  DG::CheckBox m_fundamentalCheckbox;
  DG::CheckBox m_userLevelCheckbox;
  DG::CheckBox m_allPropertyCheckbox;
  DG::MultiLineEdit m_elementTypesTextEdit;
  DG::MultiLineEdit m_filePathTextEdit;
  DG::Separator	m_separator;
  DG::Button m_okButton;
  DG::Button m_cancelButton;
};