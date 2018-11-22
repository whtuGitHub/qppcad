#include <qppcad/workspace.hpp>
#include <qppcad/app_state.hpp>
#include <data/ptable.hpp>
#include <io/geomio.hpp>
#include <mathf/math.hpp>
#include <qppcad/ws_atoms_list.hpp>
#include <qppcad/ws_comp_chem_data.hpp>

using namespace qpp;
using namespace qpp::cad;

std::shared_ptr<ws_item_t> ws_item_factory::create_object(const std::string &obj_type) {
  if (obj_type == "ws_atoms_list") return std::make_shared<ws_atoms_list_t>();
  if (obj_type == "ws_comp_chem_data") return std::make_shared<ws_comp_chem_data_t>();
  return nullptr;
}

std::optional<size_t> workspace_t::get_selected_item () {
  for (size_t i = 0; i < m_ws_items.size(); i++)
    if (m_ws_items[i]->m_selected) return std::optional<size_t>(i);
  return std::nullopt;
}

ws_item_t *workspace_t::get_selected () {
  std::optional<size_t> sel_idx = get_selected_item();
  if (sel_idx) return m_ws_items[*sel_idx].get();
  else return nullptr;
}

bool workspace_t::set_selected_item (const size_t sel_idx, bool emit_signal) {

  app_state_t* astate = app_state_t::get_inst();

  unselect_all();

  if (sel_idx < m_ws_items.size() && !m_ws_items.empty()) {
      m_ws_items[sel_idx]->m_selected = true;
      if (m_ws_items[sel_idx]->get_flags() & ws_item_flags_support_translation) {
          m_gizmo->attached_item = m_ws_items[sel_idx].get();
          m_gizmo->update_gizmo(0.1f);
        }
      else {
          m_gizmo->attached_item = nullptr;
        }
      //astate->make_viewport_dirty();
      if (emit_signal) astate->astate_evd->current_workspace_selected_item_changed();
      return true;
    }

  //astate->make_viewport_dirty();
  if (emit_signal) astate->astate_evd->current_workspace_selected_item_changed();
  return false;
}

void workspace_t::unselect_all (bool emit_signal) {
  for (auto &ws_item : m_ws_items) ws_item->m_selected = false;
  app_state_t* astate = app_state_t::get_inst();
  if (emit_signal) astate->astate_evd->current_workspace_selected_item_changed();
}

void workspace_t::toggle_edit_mode () {
  app_state_t* astate = app_state_t::get_inst();

  if (m_edit_type == ws_edit_type::EDIT_WS_ITEM)
    m_edit_type = ws_edit_type::EDIT_WS_ITEM_CONTENT;
  else
    m_edit_type = ws_edit_type::EDIT_WS_ITEM;

}

void workspace_t::workspace_changed () {

}

void workspace_t::reset_camera () {
  m_camera->reset_camera();
  set_best_view();
}

void workspace_t::set_best_view () {

  if (m_ws_items.size() == 0) {
      m_camera->reset_camera();
      return;
    }

  vector3<float> vec_look_at  = vector3<float>(0.0, 0.0, 0.0);
  vector3<float> vec_look_pos = vector3<float>(0.0, 0.0, -3.0);

  for (auto &ws_item : m_ws_items)
    ws_item->vote_for_view_vectors(vec_look_pos, vec_look_at);

  vec_look_at  /= m_ws_items.size();
  vec_look_pos /= m_ws_items.size();

  m_camera->m_look_at = vec_look_at;
  m_camera->m_view_point = vec_look_pos;

  //  std::cout << "set bv " << _vLookAt << std::endl << _vLookPos << std::endl
  //            << "end bv " << std::endl;
  m_camera->orthogonalize_gs();

  if ((m_camera->m_look_at-m_camera->m_view_point).norm() < 0.4f || vec_look_at == vec_look_pos)
    m_camera->reset_camera();
}

void workspace_t::render() {

  app_state_t* astate = app_state_t::get_inst();

  if (m_gizmo->is_active && m_gizmo->attached_item) m_gizmo->render();

  if (astate->debug_show_selection_ray){
      astate->dp->begin_render_line();
      astate->dp->render_line(vector3<float>(1.0, 1.0, 0.0), m_ray_debug.start,
                              m_ray_debug.start + m_ray_debug.dir * 155.0);
      astate->dp->end_render_line();
    }

  if (astate->dp) {

      ///// Draw axis /////
      if (astate->show_axis) {
          vector3<float> vScrTW = astate->camera->unproject(-0.95f, -0.90f);
          float fAxisLen = 0.07f *astate->camera->m_stored_dist;
          if (astate->camera->cur_proj ==
              cam_proj_type::CAMERA_PROJ_PERSP)
            fAxisLen = 0.015f;

          astate->dp->begin_render_line();
          astate->dp->
              render_line(vector3<float>(1.0, 0.0, 0.0),
                          vector3<float>(0.0, 0.0, 0.0) + vScrTW,
                          vector3<float>(fAxisLen, 0.0, 0.0) + vScrTW);

          astate->dp->
              render_line(vector3<float>(0.0, 1.0, 0.0),
                          vector3<float>(0.0, 0.0, 0.0) + vScrTW,
                          vector3<float>(0.0, fAxisLen, 0.0) + vScrTW);

          astate->dp->
              render_line(vector3<float>(0.0, 0.0, 1.0),
                          vector3<float>(0.0, 0.0, 0.0) + vScrTW,
                          vector3<float>(0.0, 0.0, fAxisLen) + vScrTW);
          astate->dp->end_render_line();

        }
      ///// Draw axis end /////
    }

  for (auto &ws_item : m_ws_items) ws_item->render();

}

void workspace_t::mouse_click (const float mouse_x, const float mouse_y) {

  //if (ImGui::GetIO().WantCaptureMouse) return;
  app_state_t* astate = app_state_t::get_inst();

  m_ray_debug.dir = (m_camera->unproject(mouse_x, mouse_y) - m_camera->m_view_point).normalized();
  m_ray_debug.start = m_camera->m_view_point;

  if (m_gizmo->m_is_visible && m_gizmo->attached_item)
    if (m_gizmo->process_ray(&m_ray_debug)){
        //c_app::log("gizmo clicked");
        return;
      }

  bool hit_any = false;

  if (m_edit_type != ws_edit_type::EDIT_WS_ITEM_CONTENT) {
      for (auto &ws_item : m_ws_items) ws_item->m_selected = false;
      m_gizmo->attached_item = nullptr;
    }

  for (auto &ws_item : m_ws_items) {
      bool is_hit = ws_item->mouse_click(&m_ray_debug);
      hit_any = hit_any || is_hit;
      if (is_hit && m_edit_type == ws_edit_type::EDIT_WS_ITEM &&
          (ws_item->get_flags() & ws_item_flags_support_selection)) {
          m_gizmo->attached_item = ws_item.get();
          auto it = std::find(m_ws_items.begin(), m_ws_items.end(), ws_item);
          if (it != m_ws_items.end()) {
              auto index = std::distance(m_ws_items.begin(), it);
              set_selected_item(index);
              break;
            }
        }
    }

  if (m_edit_type != ws_edit_type::EDIT_WS_ITEM_CONTENT && !hit_any) {
      m_gizmo->attached_item = nullptr;
      unselect_all();
    }

}

void workspace_t::add_item_to_workspace (const std::shared_ptr<ws_item_t> &item_to_add) {

  item_to_add->set_parent_workspace(this);
  m_ws_items.push_back(item_to_add);
  workspace_changed();
  //c_app::log(fmt::format("New workspace {} size = {}", m_ws_name, m_ws_items.size()));

}


void workspace_t::save_workspace_to_json (const std::string filename) {

  std::ofstream out_file(filename);
  json data;

  data[JSON_QPPCAD_VERSION] = "1.0-aa";
  data[JSON_WS_NAME] = m_ws_name;
  json j_workspace_background = json::array({m_background_color[0],
                                             m_background_color[1],
                                             m_background_color[2], });
  data[JSON_BG_CLR] = j_workspace_background;

  json ws_objects = json::array({});
  for (const auto &ws_item : m_ws_items){
      json ws_object;
      ws_item->write_to_json(ws_object);
      ws_objects.push_back(ws_object);
    }

  data[JSON_OBJECTS] = ws_objects;

  out_file << data.dump(2);

}

void workspace_t::load_workspace_from_json (const std::string filename) {

  std::fstream ifile(filename);
  json data;

  try {
    data = json::parse(ifile);

    if (data.find(JSON_WS_NAME) != data.end()) m_ws_name = data[JSON_WS_NAME];
    if (data.find(JSON_BG_CLR) != data.end())
      for (uint8_t i = 0; i < 3; i++) m_background_color[i] = data[JSON_BG_CLR][i];

    if (data.find(JSON_OBJECTS) != data.end()){
        json objects = data[JSON_OBJECTS];
        for (auto &object : objects)
          if (object.find(JSON_WS_ITEM_TYPE) != object.end()){
              std::string obj_type = object[JSON_WS_ITEM_TYPE];
              std::shared_ptr<ws_item_t> obj = ws_item_factory::create_object(obj_type);
              obj->read_from_json(object);
              add_item_to_workspace(obj);
            } else {
              //              c_app::log(fmt::format("WARNING: Cannot find type for object \"{}\" in file \"{}\"!",
              //                                     object[JSON_WS_ITEM_NAME].get<std::string>(), filename));
            }
      }

    m_fs_path = filename;

  } catch (json::parse_error &e) {
    std::cerr << e.what() << " " << std::endl;
  }

}

void workspace_t::update (float delta_time) {

  if (m_first_render) {
      set_best_view();
      m_first_render = false;
    }

  m_gizmo->update_gizmo(delta_time);

  //handle deletion
  for (auto it = m_ws_items.begin(); it != m_ws_items.end(); ) {
      if ((*it)->m_marked_for_deletion) {

          if (it->get() == m_gizmo->attached_item)
            m_gizmo->attached_item = nullptr;

          it = m_ws_items.erase(it);
          workspace_changed();

        }
      else {
          ++it;
        }
    }

  //update cycle
  for (auto &ws_item : m_ws_items) ws_item->update(delta_time);

}

void workspace_t::set_edit_type (const ws_edit_type new_edit_type) {

  app_state_t* astate = app_state_t::get_inst();
  m_edit_type = new_edit_type;
  //astate->make_viewport_dirty();

}

workspace_manager_t::workspace_manager_t (app_state_t *_astate) {

  m_current_workspace_id = 0;
  cached_astate = _astate;
  //  _astate->kb_manager->connect("switch_to_ws_0", this, &workspace_manager_t::force_set_current<0>);
  //  _astate->kb_manager->connect("switch_to_ws_1", this, &workspace_manager_t::force_set_current<1>);
  //  _astate->kb_manager->connect("switch_to_ws_2", this, &workspace_manager_t::force_set_current<2>);
  //  _astate->kb_manager->connect("switch_to_ws_3", this, &workspace_manager_t::force_set_current<3>);
  //  _astate->kb_manager->connect("switch_to_ws_4", this, &workspace_manager_t::force_set_current<4>);
  //  _astate->kb_manager->connect("switch_to_ws_5", this, &workspace_manager_t::force_set_current<5>);
  //  _astate->kb_manager->connect("switch_to_ws_6", this, &workspace_manager_t::force_set_current<6>);
  //  _astate->kb_manager->connect("switch_to_ws_7", this, &workspace_manager_t::force_set_current<7>);
  //  _astate->kb_manager->connect("switch_to_ws_8", this, &workspace_manager_t::force_set_current<8>);

}

std::shared_ptr<workspace_t> workspace_manager_t::get_current () {

  if (m_current_workspace_id >= m_ws.size()) return nullptr;
  return m_ws[m_current_workspace_id];

}

std::optional<size_t> workspace_manager_t::get_current_id () {

  if (!m_ws.empty()) return std::optional<size_t>(m_current_workspace_id);
  return std::nullopt;

}

bool workspace_manager_t::set_current (const size_t ws_index) {

  //c_app::log("set current called");
  app_state_t* astate = app_state_t::get_inst();


  if (ws_index < m_ws.size() && has_wss()) {
      m_current_workspace_id = ws_index;
      //update_window_title();
      cached_astate->camera = m_ws[ws_index]->m_camera.get();
      cached_astate->camera->update_camera();
      astate->astate_evd->current_workspace_changed();
      return true;
    }

  astate->astate_evd->current_workspace_changed();
  return false;

}

void workspace_manager_t::init_default () {

  std::ifstream test_in_dev_env("../data/refs/laf3_p3.vasp");
  if (!test_in_dev_env.good()) return;

  auto _ws2 = std::make_shared<workspace_t>("d2");
  auto _ws3 = std::make_shared<workspace_t>("d1");

  auto _wsl2 = std::make_shared<ws_atoms_list_t>();
  _ws3->add_item_to_workspace(_wsl2);
  _wsl2->load_from_file(qc_file_fmt::standart_xyz,
                        "../deps/qpp/examples/io/ref_data/xyz/slab.xyz",
                        false);

  auto _wsl3 = std::make_shared<ws_atoms_list_t>();
  _ws2->add_item_to_workspace(_wsl3);
  _wsl3->load_from_file(qc_file_fmt::vasp_poscar, "../data/refs/POSCAR.mp-558947_SiO2",
                        false);

  auto _wsl32 = std::make_shared<ws_atoms_list_t>();
  _ws2->add_item_to_workspace(_wsl32);
  _wsl32->load_from_file(qc_file_fmt::standart_xyz,
                         "../deps/qpp/examples/io/ref_data/xyz/nanotube.xyz",
                         true);

  auto _wsl33 = std::make_shared<ws_atoms_list_t>();
  _ws2->add_item_to_workspace(_wsl33);
  _wsl33->load_from_file(qc_file_fmt::vasp_poscar, "../data/refs/mp-971662_Si.vasp",
                         false);


  auto _ws4 = std::make_shared<workspace_t>();
  _ws4->m_ws_name = "animtest1";
  auto _ws4_al = std::make_shared<ws_atoms_list_t>();
  _ws4->add_item_to_workspace(_ws4_al);

  _ws4_al->load_from_file(qc_file_fmt::firefly_output,
                          "../deps/qpp/examples/io/ref_data/firefly/dvb_ir.out",
                          false);

  _wsl3->m_name = "zeolite1";
  _wsl32->m_name = "nanotube1";
  _wsl32->m_pos = vector3<float>(0.0f, 0.0f, 14.0f);
  _wsl33->m_name = "ss1";
  _wsl33->m_pos = vector3<float>(0.0f, 22.0f, 2.0f);

  add_workspace(_ws3);
  add_workspace(_ws2);
  add_workspace(_ws4);

  //  _ws2->save_workspace_to_json("test.json");
  //  _ws4->save_workspace_to_json("test_with_anim.json");

  set_current(2);

}

void workspace_manager_t::render_current_workspace () {

  app_state_t* astate = app_state_t::get_inst();

  if (has_wss()) {
      if (m_current_workspace_id < m_ws.size()) {
          astate->glapi->glClearColor(m_ws[m_current_workspace_id]->m_background_color[0],
              m_ws[m_current_workspace_id]->m_background_color[1],
              m_ws[m_current_workspace_id]->m_background_color[2], 1);
          astate->glapi->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          m_ws[m_current_workspace_id]->render();
          return ;
        }
    }

  astate->glapi->glClearColor(0.4f, 0.4f, 0.4f, 1);
  astate->glapi->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//void workspace_manager_t::render_current_workspace_overlay () {

//  if (has_wss()){
//      if (m_current_workspace_id < m_ws.size()) {
//          for (auto &ws_item : m_ws[m_current_workspace_id]->m_ws_items)
//            ws_item->render_overlay();
//        }
//    }
//}

void workspace_manager_t::mouse_click () {

  app_state_t* astate = app_state_t::get_inst();
  //c_app::log(fmt::format("Mouse click {} {}", astate->mouse_x, astate->mouse_y));

  astate->log(fmt::format("Mouse click in ws {} {}",
                          astate->mouse_x_dc, astate->mouse_y_dc));

  if (has_wss()) {
      get_current()->mouse_click(astate->mouse_x_dc, astate->mouse_y_dc);
      astate->make_viewport_dirty();
    }

}

void workspace_manager_t::workspace_manager_changed() {
  app_state_t* astate = app_state_t::get_inst();
  astate->astate_evd->workspaces_changed();
}

void workspace_manager_t::add_workspace (const std::shared_ptr<workspace_t> &ws_to_add) {
  ws_to_add->m_owner = this;
  m_ws.push_back(ws_to_add);
  ws_to_add->workspace_changed();
  workspace_manager_changed();
}


void workspace_manager_t::import_file_as_new_workspace(const std::string &fname,
                                                       qc_file_fmt file_format){
  auto new_ws = std::make_shared<workspace_t>();
  std::string file_name_extr = qpp::extract_base_name(fname);
  new_ws->m_ws_name = file_name_extr;
  auto new_atoms_list = std::make_shared<ws_atoms_list_t>();
  new_ws->add_item_to_workspace(new_atoms_list);
  new_atoms_list->load_from_file(file_format, fname,
                                 qc_file_fmt_helper::need_to_auto_center(file_format));
  add_workspace(new_ws);
  set_current(m_ws.size()-1);
}

void workspace_manager_t::query_create_new_workspace(bool switch_to_new_workspace) {

  auto new_ws = std::make_shared<workspace_t>();
  new_ws->m_ws_name = fmt::format("new_workspace{}", m_ws.size());
  m_ws.push_back(new_ws);
  if (switch_to_new_workspace) set_current(m_ws.size()-1);
  workspace_manager_changed();

}

void workspace_manager_t::load_workspace_from_file(const std::string &filename) {

  app_state_t* astate = app_state_t::get_inst();

  auto new_ws = std::make_shared<workspace_t>();
  //new_ws->m_ws_name = "d4";
  new_ws->load_workspace_from_json(filename);
  add_workspace(new_ws);
  set_current(m_ws.size()-1);
  new_ws->set_best_view();
  //astate->make_viewport_dirty();
  workspace_manager_changed();
}


