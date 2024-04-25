#pragma once

#include "ACAPinc.h"

#include "ResourceIds.hpp"
#include "DGModule.hpp"
#include "JsonExporter.hpp"

using json = nlohmann::json;

class JsonExporterDialog :
  public DG::ModalDialog,
  public DG::PanelObserver,
  public DG::ButtonItemObserver,
  public DG::CompoundItemObserver
{
public:
  enum DialogResourceIds
  {
    ExampleDialogResourceId = ID_ADDON_DLG,
    OKButtonId = 1,
    CancelButtonId = 2,
    ElementTypeFiltersTextEditId = 3,
    ExportPathTextEditId = 4,
    UserDefinedCheckboxId = 5,
    FundamentalCheckboxId = 6,
    UserLevelCheckboxId = 7,
    AllPropertyCheckboxId = 8,
    SeparatorId = 9,
  };

  JsonExporterDialog();
  ~JsonExporterDialog();

private:
  virtual void ButtonClicked(const DG::ButtonClickEvent& ev) override;

  void FillElementTypeFilters();
  void StartExport();
  std::vector<API_ElemTypeID> GetElementTypes();
  std::vector<API_PropertyDefinitionFilter> GetPropertyDefinitionFilters();

  DG::Button m_okButton;
  DG::Button m_cancelButton;
  DG::Separator	m_separator;
  DG::MultiLineEdit m_elementTypeFiltersTextEdit;
  DG::MultiLineEdit m_exportPathTextEdit;
  DG::CheckBox m_userDefinedCheckbox;
  DG::CheckBox m_fundamentalCheckbox;
  DG::CheckBox m_userLevelCheckbox;
  DG::CheckBox m_allPropertyCheckbox;

  JsonExporter m_exporter;
};