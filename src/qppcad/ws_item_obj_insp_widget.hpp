#ifndef QPP_CAD_WS_ITEM_OBJ_INSP_H
#define QPP_CAD_WS_ITEM_OBJ_INSP_H

#include <qppcad/qppcad.hpp>
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFormLayout>
#include <QPushButton>

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

        ws_item_tab_widget_t *tab_general;
        ws_item_tab_widget_t* define_tab(QString tab_name);
        ws_item_obj_insp_widget_t();

    };

  }

}

#endif
