#ifndef QPPCAD_WS_ATOMS_LIST_CONTEXT_MENU
#define QPPCAD_WS_ATOMS_LIST_CONTEXT_MENU
#include <qppcad/ws_atoms_list.hpp>

namespace qpp::cad {
  struct ws_atoms_list_context_menu_helper {
      static void render_item_edit_menu(ws_atoms_list_t *al){

      }

      static void render_content_edit_menu(ws_atoms_list_t *al){
        if (al->support_content_editing()) {

            if (ImGui::BeginMenu("Selection")) {
                if (ImGui::MenuItem("Select all")) al->select_atoms(true);
                if (ImGui::MenuItem("Unselect all")) al->select_atoms(false);
                if (ImGui::MenuItem("Invert selection")) al->invert_selected_atoms();
                ImGui::EndMenu();
              }

            if (al->m_atom_idx_selection.size() == 2){
                if (ImGui::BeginMenu("Append new atom between")){

                    auto it1 = al->m_atom_idx_selection.begin();
                    auto it2 = it1++;
                    vector3<float> r_btw{0.0, 0.0, 0.0};

                    if (it1 != al->m_atom_idx_selection.end() && it2 != al->m_atom_idx_selection.end())
                      r_btw = (al->m_geom->pos(it1->m_atm, it1->m_idx) +
                               al->m_geom->pos(it2->m_atm, it2->m_idx))*0.5f;

                    for (auto at_type = 0; at_type < al->m_geom->n_types(); at_type++)
                      if (ImGui::MenuItem(al->m_geom->atom_of_type(at_type).c_str())) {
                          al->insert_atom(at_type, r_btw);
                        }

                    static string custom_atom_name;
                    ImGui::PushItemWidth(100);
                    ImGui::PushID(1);
                    ImGui::InputText("", &custom_atom_name);
                    ImGui::PopID();
                    ImGui::SameLine();
                    if (ImGui::Button("Add custom")){
                        al->m_geom->add(custom_atom_name, r_btw);
                      }
                    ImGui::EndMenu();
                  }

                if (ImGui::BeginMenu("Two atoms manipulation")){
                    auto it1 = al->m_atom_idx_selection.begin();
                    auto it2 = it1++;
                    float dist_btw = (al->m_geom->pos(it1->m_atm, it1->m_idx) -
                                      al->m_geom->pos(it2->m_atm, it2->m_idx)).norm();

                    if (ImGui::SliderFloat("Distance between two atoms", &dist_btw, 0.1f, 5.0f)){

                        vector3<float> r_btw = (al->m_geom->pos(it1->m_atm, it1->m_idx) +
                                                al->m_geom->pos(it2->m_atm, it2->m_idx))*0.5f;
                        vector3<float> dir_f = (al->m_geom->pos(it1->m_atm, it1->m_idx) - r_btw).normalized();
                        vector3<float> dir_s = (al->m_geom->pos(it2->m_atm, it2->m_idx) - r_btw).normalized();
                        al->m_geom->change_pos(it1->m_atm, r_btw + dir_f * dist_btw * 0.5f);
                        al->m_geom->change_pos(it2->m_atm, r_btw + dir_s * dist_btw * 0.5f);
                      }

                    ImGui::EndMenu();
                  }
              }

            if (al->m_atom_selection.size() > 0)
              if (ImGui::MenuItem(fmt::format("Delete selected atoms({})",
                                              al->m_atom_selection.size()).c_str())){
                  al->delete_selected_atoms();
                }

            ImGui::Separator();

            if (al->m_atom_idx_selection.size() == 0){
                if (ImGui::BeginMenu("Add new atom")){
                    static string custom_atom_name;
                    ImGui::BulletText("Add new atom in local atom list frame");
                    ImGui::Separator();
                    ImGui::InputFloat3("Position", al->m_new_atom_pos.data());
                    ImGui::InputText("New atom name", &custom_atom_name);
                    ImGui::SameLine();
                    if (ImGui::Button("Add"))
                      if (custom_atom_name != "") al->insert_atom(custom_atom_name,
                                                                  al->m_new_atom_pos);
                    ImGui::EndMenu();
                  }
              }
          }
      }
  };
}

#endif
