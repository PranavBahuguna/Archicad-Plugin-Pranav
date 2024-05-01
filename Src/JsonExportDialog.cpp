#include "JsonExportDialog.hpp"
#include "JsonExportUtils.hpp"

JsonExportDialog::JsonExportDialog() :
  DG::ModalDialog(ACAPI_GetOwnResModule(), ExampleDialogResourceId, ACAPI_GetOwnResModule()),
  m_useSelectionElementsCheckbox(GetReference(), UseSelectionElementsCheckboxId),
  m_useAllElementsCheckbox(GetReference(), UseAllElementsCheckboxId),
  m_separator1(GetReference(), Separator1_Id),
  m_userDefinedCheckbox(GetReference(), UserDefinedCheckboxId),
  m_fundamentalCheckbox(GetReference(), FundamentalCheckboxId),
  m_userLevelCheckbox(GetReference(), UserLevelCheckboxId),
  m_allPropertyCheckbox(GetReference(), AllPropertyCheckboxId),
  m_elementTypesLabel(GetReference(), ElementTypesLabelId),
  m_elementTypesTextEdit(GetReference(), ElementTypesTextEditId),
  m_filePathCheckBox(GetReference(), FilePathCheckboxId),
  m_filePathTextEdit(GetReference(), FilePathTextEditId),
  m_urlCheckBox(GetReference(), UrlCheckboxId),
  m_urlTextEdit(GetReference(), UrlTextEditId),
  m_separator2(GetReference(), Separator2_Id),
  m_closeButton(GetReference(), CloseButtonId),
  m_exportButton(GetReference(), ExportButtonId)
{
  AttachToAllItems(*this);
  Attach(*this);
  InitDialog();
}

JsonExportDialog::~JsonExportDialog()
{
  Detach(*this);
  DetachFromAllItems(*this);
}

void JsonExportDialog::ButtonClicked(const DG::ButtonClickEvent& ev)
{
  if (ev.GetSource() == &m_exportButton)
  {
    auto settingsData = GetSettingsData();
    JsonExportUtils::RunExportProcess(settingsData);
  }

  if (ev.GetSource() == &m_closeButton)
    PostCloseRequest(DG::ModalDialog::Cancel);
}

void JsonExportDialog::CheckItemChanged(const DG::CheckItemChangeEvent& ev)
{
  // Handle selected elements checkboxes
  if (ev.GetSource() == &m_useSelectionElementsCheckbox)
  {
    if (m_useSelectionElementsCheckbox.IsChecked())
      m_useAllElementsCheckbox.Uncheck();
    else
      m_useAllElementsCheckbox.Check();

    UpdateAvailableElementTypes();
  }
  if (ev.GetSource() == &m_useAllElementsCheckbox)
  {
    if (m_useAllElementsCheckbox.IsChecked())
      m_useSelectionElementsCheckbox.Uncheck();
    else
      m_useSelectionElementsCheckbox.Check();

    UpdateAvailableElementTypes();
  }

  // Handle property filter checkboxes
  if (ev.GetSource() == &m_allPropertyCheckbox)
  {
    if (m_allPropertyCheckbox.IsChecked())
    {
      m_userDefinedCheckbox.Check();
      m_userDefinedCheckbox.Disable();
      m_fundamentalCheckbox.Check();
      m_fundamentalCheckbox.Disable();
      m_userLevelCheckbox.Check();
      m_userLevelCheckbox.Disable();
    }
    else
    {
      m_userDefinedCheckbox.Enable();
      m_fundamentalCheckbox.Enable();
      m_userLevelCheckbox.Enable();
    }
  }

  // Handle file path / link checkboxes
  if (ev.GetSource() == &m_filePathCheckBox)
  {
    if (m_filePathCheckBox.IsChecked())
      m_filePathTextEdit.Enable();
    else
      m_filePathTextEdit.Disable();
  }
  if (ev.GetSource() == &m_urlCheckBox)
  {
    if (m_urlCheckBox.IsChecked())
      m_urlTextEdit.Enable();
    else
      m_urlTextEdit.Disable();
  }

  // Disable export button if either no filters or file path and url are not checked
  bool allFiltersUnchecked =
    !m_userDefinedCheckbox.IsChecked() &&
    !m_fundamentalCheckbox.IsChecked() &&
    !m_userLevelCheckbox.IsChecked() &&
    !m_allPropertyCheckbox.IsChecked();

  bool allExportsUnchecked = !m_filePathCheckBox.IsChecked() && !m_urlCheckBox.IsChecked();

  if (allFiltersUnchecked || allExportsUnchecked)
    m_exportButton.Disable();
  else
    m_exportButton.Enable();
}

void JsonExportDialog::InitDialog()
{
  // Init element selection options
  if (JsonExportUtils::IsAnyElementsSelected())
  {
    m_useSelectionElementsCheckbox.Check();
  }
  else
  {
    m_useSelectionElementsCheckbox.Disable();
    m_useAllElementsCheckbox.Disable();
    m_useAllElementsCheckbox.Check();
  }
  UpdateAvailableElementTypes();

  // Init property filter options
  m_userDefinedCheckbox.Check();
  m_userDefinedCheckbox.Disable();
  m_fundamentalCheckbox.Check();
  m_fundamentalCheckbox.Disable();
  m_userLevelCheckbox.Check();
  m_userLevelCheckbox.Disable();
  m_allPropertyCheckbox.Check();

  // Init file path / link export options
  m_filePathCheckBox.Check();
  m_urlTextEdit.Disable();
}

void JsonExportDialog::UpdateAvailableElementTypes()
{
  // Obtain list of available type names
  GS::Array<GS::UniString> elemTypeNames;
  bool selectedOnly = m_useSelectionElementsCheckbox.IsChecked();
  JsonExportUtils::GetAvailableElementTypeNames(selectedOnly, elemTypeNames);

  // Join names together in a comma-separated list and update the text edit
  GS::UniString allElemTypeNames;
  if (!elemTypeNames.IsEmpty())
  {
    allElemTypeNames = elemTypeNames.GetFirst();
    for (auto it = elemTypeNames.Begin() + 1; it != elemTypeNames.End(); ++it)
      allElemTypeNames += ", " + *it;
  }
  m_elementTypesTextEdit.SetText(allElemTypeNames);
}

JsonExportSettingsData JsonExportDialog::GetSettingsData() const
{
  auto elementNames = m_elementTypesTextEdit.GetText().Split(",");
  for (GS::UniString& name : elementNames)
    name.Trim();

  return JsonExportSettingsData
  {
    m_filePathTextEdit.GetText(),
    m_urlTextEdit.GetText(),
    m_filePathCheckBox.IsChecked(),
    m_urlCheckBox.IsChecked(),
    GetPropertyDefinitionFilters(),
    elementNames,
    m_useSelectionElementsCheckbox.IsChecked()
  };
}

GS::Array<API_PropertyDefinitionFilter> JsonExportDialog::GetPropertyDefinitionFilters() const
{
  if (m_allPropertyCheckbox.IsChecked())
    return { API_PropertyDefinitionFilter_All };

  GS::Array<API_PropertyDefinitionFilter> filters;
  if (m_userDefinedCheckbox.IsChecked())
    filters.Push(API_PropertyDefinitionFilter_UserDefined);
  if (m_fundamentalCheckbox.IsChecked())
    filters.Push(API_PropertyDefinitionFilter_FundamentalBuiltIn);
  if (m_userLevelCheckbox.IsChecked())
    filters.Push(API_PropertyDefinitionFilter_UserLevelBuiltIn);

  return filters;
}