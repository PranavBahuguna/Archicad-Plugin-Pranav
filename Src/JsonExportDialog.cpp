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
  m_elementTypesTextEdit(GetReference(), ElementTypesTextEditId),
  m_filePathTextEdit(GetReference(), FilePathTextEditId),
  m_separator2(GetReference(), Separator2_Id),
  m_cancelButton(GetReference(), CancelButtonId),
  m_okButton(GetReference(), OKButtonId)
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

/**
 * @brief Obtains settings data from this dialog
 * @returns Settings data, this consists of:
 * - Output file path string
 * - Array of property definition filters
 * - Array of element type names
 * - Using element selection bool value check
 */
JsonExportSettingsData JsonExportDialog::GetSettingsData() const
{
  auto elementNames = m_elementTypesTextEdit.GetText().Split(",");
  for (GS::UniString &name : elementNames)
    name.Trim();

  return JsonExportSettingsData
  {
    m_filePathTextEdit.GetText(),
    GetPropertyDefinitionFilters(),
    elementNames,
    m_useSelectionElementsCheckbox.IsChecked()
  };
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

  // Init text edits
  m_filePathTextEdit.SetText("Enter file path...");
}

void JsonExportDialog::ButtonClicked(const DG::ButtonClickEvent& ev)
{
  if (ev.GetSource() == &m_okButton)
    PostCloseRequest(DG::ModalDialog::Accept);
  if (ev.GetSource() == &m_cancelButton)
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