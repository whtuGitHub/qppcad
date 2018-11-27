#include <qppcad/ws_item_obj_insp_widget.hpp>
#include <qppcad/app_state.hpp>

using namespace qpp;
using namespace qpp::cad;

ws_item_tab_widget_t *ws_item_obj_insp_widget_t::define_tab(QString tab_name) {

  ws_item_tab_widget_t *tmp = new ws_item_tab_widget_t;
  tmp->tab_scroll = new QScrollArea;
  tmp->tab_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  tmp->tab_scroll->setWidgetResizable(true);

  tmp->tab_inner_widget = new QWidget(tmp->tab_scroll);
  tmp->tab_scroll->setWidget(tmp->tab_inner_widget);

  tmp->tab_inner_widget_layout = new QVBoxLayout;
  tmp->tab_inner_widget_layout->setContentsMargins(3,3,3,3);
  tmp->tab_inner_widget_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  tmp->tab_inner_widget->setLayout(tmp->tab_inner_widget_layout);

  addTab(tmp->tab_scroll, tab_name);

  return tmp;
}

void ws_item_obj_insp_widget_t::bind_to_item(ws_item_t *_binding_item) {
  m_binded_item = _binding_item;
  update_from_ws_item();
}

void ws_item_obj_insp_widget_t::unbind_item() {
  m_binded_item = nullptr;
  ws_item_is_visible->unbind_value();
}

void ws_item_obj_insp_widget_t::update_from_ws_item() {
  if (m_binded_item) {
      ws_item_name->setText(QString::fromStdString(m_binded_item->get_name()));
      ws_item_type->setText(QString::fromStdString(m_binded_item->compose_item_name()));
      ws_item_is_visible->bind_value(&m_binded_item->m_is_visible);
    }
}

ws_item_obj_insp_widget_t::ws_item_obj_insp_widget_t() {

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  tab_general = define_tab(tr("General"));

  tg_info_widget = new QGroupBox;
  tg_info_widget->setTitle(tr("Item information"));
  tab_general->tab_inner_widget_layout->addWidget(tg_info_widget);

  tg_actions = new QGroupBox(tr("Item actions"));
  tg_actions_layout = new QHBoxLayout;
  tg_actions->setLayout(tg_actions_layout);
  tg_actions_delete = new QPushButton(tr("Delete"));
  tg_actions_rename = new QPushButton(tr("Rename"));
  connect(tg_actions_rename, SIGNAL(pressed()), this, SLOT(rename_current_item()));
  tg_actions_clone = new QPushButton(tr("Clone"));
  tg_actions_layout->addWidget(tg_actions_delete);
  tg_actions_layout->addWidget(tg_actions_rename);
  tg_actions_layout->addWidget(tg_actions_clone);
  tg_actions->setMaximumHeight(90);
  tab_general->tab_inner_widget_layout->addWidget(tg_actions);

  tg_form_layout = new QFormLayout;
  tg_info_widget->setLayout(tg_form_layout);
  ws_item_name = new QLabel;
  ws_item_type = new QLabel;
  ws_item_is_visible_label = new QLabel(tr("Is visible:"));
  ws_item_is_visible = new qbinded_checkbox;
  tg_form_layout->addRow(tr("Name:"), ws_item_name);
  tg_form_layout->addRow(tr("Type:"), ws_item_type);
  tg_form_layout->addRow(ws_item_is_visible_label, ws_item_is_visible);

  tg_form_layout->setLabelAlignment(Qt::AlignRight);



//  tab_general_child = new QWidget;

//  tab_general_child->setLayout(new QVBoxLayout);
//  tab_general_layout->addWidget(tab_general_child);

//  for(int i = 0; i < 27; i++)
//    tab_general_child->layout()->addWidget(new QPushButton("tre"));
//  //setUsesScrollButtons(false);
  //  //setElideMode(Qt::ElideLeft);
}

void ws_item_obj_insp_widget_t::rename_current_item() {
  if (m_binded_item) {
      app_state_t* astate = app_state_t::get_inst();
      bool ok;
      QString text = QInputDialog::getText(this, tr("Workspace item-> Rename"),
                                           tr("User name:"), QLineEdit::Normal,
                                           QString::fromStdString(m_binded_item->m_name), &ok);
      if (ok && text != "") {
          m_binded_item->m_name = text.toStdString();
          astate->astate_evd->current_workspace_selected_item_changed();
        }
    }
}
