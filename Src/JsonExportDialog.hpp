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
    UseSelectionElementsCheckboxId = 1,
    UseAllElementsCheckboxId = 2,
    Separator1_Id = 3,
    UserDefinedCheckboxId = 4,
    FundamentalCheckboxId = 5,
    UserLevelCheckboxId = 6,
    AllPropertyCheckboxId = 7,
    ElementTypesTextEditId = 8,
    FilePathTextEditId = 9,
    Separator2_Id = 10,
    CancelButtonId = 11,
    OKButtonId = 12,
  };

  JsonExportDialog(bool selectionAvailable);
  ~JsonExportDialog();

  JsonExportSettingsData GetSettingsData() const;

private:
  virtual void ButtonClicked(const DG::ButtonClickEvent& ev) override;
  virtual void CheckItemChanged(const DG::CheckItemChangeEvent& ev) override;

  void InitDialog(bool selectionAvailable);
  void UpdateAvailableElementTypes();

  GS::Array<API_PropertyDefinitionFilter> GetPropertyDefinitionFilters() const;

  DG::CheckBox m_useSelectionElementsCheckbox;
  DG::CheckBox m_useAllElementsCheckbox;
  DG::Separator m_separator1;
  DG::CheckBox m_userDefinedCheckbox;
  DG::CheckBox m_fundamentalCheckbox;
  DG::CheckBox m_userLevelCheckbox;
  DG::CheckBox m_allPropertyCheckbox;
  DG::MultiLineEdit m_elementTypesTextEdit;
  DG::MultiLineEdit m_filePathTextEdit;
  DG::Separator	m_separator2;
  DG::Button m_okButton;
  DG::Button m_cancelButton;
};