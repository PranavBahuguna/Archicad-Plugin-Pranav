#include "JsonExportDialog.hpp"

JsonExportDialog::JsonExportDialog(const GS::UniString& availableElementTypesText) :
  DG::ModalDialog(ACAPI_GetOwnResModule(), ExampleDialogResourceId, ACAPI_GetOwnResModule()),
  m_okButton(GetReference(), OKButtonId),
  m_cancelButton(GetReference(), CancelButtonId),
  m_elementTypesTextEdit(GetReference(), ElementTypesTextEditId),
  m_filePathTextEdit(GetReference(), FilePathTextEditId),
  m_userDefinedCheckbox(GetReference(), UserDefinedCheckboxId),
  m_fundamentalCheckbox(GetReference(), FundamentalCheckboxId),
  m_userLevelCheckbox(GetReference(), UserLevelCheckboxId),
  m_allPropertyCheckbox(GetReference(), AllPropertyCheckboxId),
  m_separator(GetReference(), SeparatorId)
{
  AttachToAllItems(*this);
  Attach(*this);
  InitDialog(availableElementTypesText);
}

JsonExportDialog::~JsonExportDialog()
{
  Detach(*this);
  DetachFromAllItems(*this);
}

JsonExportSettingsData JsonExportDialog::GetSettingsData() const
{
  return JsonExportSettingsData
  {
    m_filePathTextEdit.GetText(),
    GetPropertyDefinitionFilters(),
    m_elementTypesTextEdit.GetText().Split(",")
  };
}

void JsonExportDialog::InitDialog(const GS::UniString& availableElementTypesText)
{
  // Init property filter checkboxes
  m_userDefinedCheckbox.Disable();
  m_fundamentalCheckbox.Disable();
  m_userLevelCheckbox.Disable();
  m_allPropertyCheckbox.Check();

  // Init text edits
  m_elementTypesTextEdit.SetText(availableElementTypesText);
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