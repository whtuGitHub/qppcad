#ifndef QPP_WORKSPACE_ITEM_H
#define QPP_WORKSPACE_ITEM_H

#include <qppcad/qppcad.hpp>
#include <geom/lace3d.hpp>
#include <geom/aabb.hpp>
#include <geom/ray.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stl.h>
#include <qppcad/json_adapter.hpp>

namespace qpp {

  namespace cad {

    class workspace_t;
    class app_state_t;

    // ws_item_t capabilities
    const uint32_t ws_item_flags_default                 = 0;
    const uint32_t ws_item_flags_support_translation     = 1 << 1;
    const uint32_t ws_item_flags_support_scaling         = 1 << 2;
    const uint32_t ws_item_flags_support_rotation        = 1 << 3;
    const uint32_t ws_item_flags_support_content_editing = 1 << 4;
    const uint32_t ws_item_flags_support_selection       = 1 << 5;
    const uint32_t ws_item_flags_support_rendering_bb    = 1 << 6;
    const uint32_t ws_item_flags_toolbar_extension       = 1 << 7;
    const uint32_t ws_item_flags_support_actions         = 1 << 8;
    const uint32_t ws_item_flags_support_delete          = 1 << 9;
    const uint32_t ws_item_flags_support_clone           = 1 << 10;
    const uint32_t ws_item_flags_support_moveto          = 1 << 11;
    const uint32_t ws_item_flags_support_rendering       = 1 << 12;

    class ws_item_t : public std::enable_shared_from_this<ws_item_t> {

      private:

        uint32_t p_flags;

      public:

        workspace_t *parent_ws{};
        app_state_t *app_state_c;
        std::string      m_name;
        aabb_3d_t<float> m_aabb;
        vector3<float>   m_pos_old;
        vector3<float>   m_pos{0.0f, 0.0f, 0.0f}; ///
        vector3<float>   m_scale{1.0f, 1.0f, 1.0f}; ///
        vector3<float>   m_rotation{1.0f, 1.0f, 1.0f}; ///
        vector3<float>   explicit_translation;
        bool             m_is_visible{true}; ///
        bool             m_draw_cell{true}; ///
        bool             m_selected{false}; ///
     //   bool             m_hide_gizmo_trigger{false};
        bool             m_marked_for_deletion{false};

        /// \brief vote_for_view_vectors
        /// \param vOutLookPos
        /// \param vOutLookAt
        virtual void vote_for_view_vectors(vector3<float> &vOutLookPos,
                                           vector3<float> &vOutLookAt) = 0;

        /// \brief set_parent_workspace
        /// \param _parent_ws
        void set_parent_workspace(workspace_t *_parent_ws);

        /// \brief get_name
        /// \return
        const std::string get_name();

        /// \brief set_name
        /// \param _name
        void set_name(const std::string &_name);

        /// \brief set_name
        /// \param _name
        void set_name(const char * _name);

        bool is_selected();

        /// \brief render
        virtual void render();

        virtual std::string get_ws_item_class_name() { return "ws_abstract_item";}
        virtual size_t get_content_count() = 0;
        /// \brief render_ui
        virtual void render_ui();
        virtual void render_overlay() = 0;
        virtual void render_work_panel_ui() = 0;
        virtual void td_context_menu_edit_item();
        virtual void td_context_menu_edit_content();

        /// \brief mouse_click
        /// \param ray
        /// \return
        virtual bool mouse_click(ray_t<float> *ray) = 0;

        void set_default_flags(uint32_t);

        uint32_t get_flags() const ;

        /// \brief get_amount_of_selected_content
        /// \return
        virtual uint32_t get_amount_of_selected_content() = 0;

        /// \brief compose_item_name
        /// \return
        virtual std::string compose_item_name() = 0;

        /// \brief update
        /// \param delta_time
        virtual void update(float delta_time);

        /// \brief get_bb_prescaller
        /// \return
        virtual float get_bb_prescaller();
        virtual bool is_bb_visible();
        /// \brief get_gizmo_content_barycenter
        /// \return
        virtual const vector3<float> get_gizmo_content_barycenter();

        virtual void on_begin_node_gizmo_translate();
        virtual void on_end_node_gizmo_translate();
        virtual void on_begin_content_gizmo_translate();
        virtual void apply_intermediate_translate_content(const vector3<float> &new_pos);
        virtual void on_end_content_gizmo_translate();

        virtual void write_to_json(json &data);
        virtual void read_from_json(json &data);
    };

  }

}

#endif
