#ifndef QPP_CAD_WS_ITEM_OBJ_INSP_H
#define QPP_CAD_WS_ITEM_OBJ_INSP_H

#include <qppcad/qppcad.hpp>
#include <qppcad/ws_item.hpp>
#include <qppcad/qbinded_inputs.hpp>
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

namespace qpp {

  namespace cad {

    struct ws_item_tab_widget_t {
        QScrollArea *tab_scroll;
        QWidget *tab_inner_widget;
        QVBoxLayout *tab_inner_widget_layout;
    };

    class ws_item_obj_insp_widget_t : public QTabWidget {

        Q_OBJECT

      public:
        ws_item_t *m_binded_item;

        ws_item_tab_widget_t *tab_general;
        QGroupBox *tg_info_widget;
        QFormLayout *tg_form_layout;
        QLabel *ws_item_name;
        QLabel *ws_item_type;

        QGroupBox *tg_actions;
        QHBoxLayout *tg_actions_layout;
        QPushButton *tg_actions_delete;
        QPushButton *tg_actions_rename;
        QPushButton *tg_actions_clone;

        QLabel *ws_item_is_visible_label;
        qbinded_checkbox *ws_item_is_visible;

        ws_item_tab_widget_t* define_tab(QString tab_name);
        virtual void bind_to_item(ws_item_t *_binding_item);
        virtual void unbind_item();
        virtual void update_from_ws_item();
        ws_item_obj_insp_widget_t();

      public slots:

        void rename_current_item();

    };

  }

}

#endif
