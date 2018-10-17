#include <qppcad/simple_query.hpp>
#include <qppcad/ws_atoms_list.hpp>
#include <qppcad/app.hpp>

using namespace qpp;
using namespace qpp::cad;

simple_query_command_t::~simple_query_command_t () {

}

bool sq_command_select_item::execute(std::vector<std::string_view> &commands_list,
                                     std::string &output){

  app_state_t *astate = &(c_app::get_state());
  if (commands_list.size() != 2) {
      output += "> seli : require 1 argument\n";
      return false;
    }
  else {
      try {
        int i = std::stoi(commands_list[1].data());
        auto cur_ws = astate->ws_manager->get_current();
        if (cur_ws) {
            auto succes = cur_ws->set_selected_item(i);
            if (succes)
              output += fmt::format("Workspace item {} with index {} was selected\n",
                                    cur_ws->get_selected()->m_name, i);
            else output += fmt::format("Error : invalid item index {}\n", i);
          } else {
            output += "Error: nothing selected!\n";
            return false;
          }
      }
      catch(std::exception const & e) {
        output += "Error: wrong argument\n";
        return false;
      }
    }

  return true;
}

bool sq_command_echo::execute (std::vector<std::string_view> &commands_list, std::string &output) {

  bool status = true;
  if (commands_list.size() != 2) {
      output += "commands_list.size() != 2\n";
      return false;
    } else {
      output += fmt::format("echo says : {}\n", commands_list[1]);
    }
  return status;
}

bool sq_command_change_edit_type::execute (std::vector<std::string_view> &commands_list,
                                           std::string &output) {

  app_state_t *astate = &(c_app::get_state());
  auto cur_ws = astate->ws_manager->get_current();
  if (cur_ws) cur_ws->set_edit_type(cur_edit_type);
  output += "Current workspace edit mode was changed\n";
  return true;
}

sq_command_change_edit_type::sq_command_change_edit_type (const ws_edit_type edit_type) {
  cur_edit_type = edit_type;
}

bool sq_command_select_content::execute (std::vector<std::string_view> &commands_list,
                                         std::string &output){

  app_state_t *astate = &(c_app::get_state());
  auto cur_ws = astate->ws_manager->get_current();

  if (cur_ws) {
      auto sel_itm = cur_ws->get_selected();
      if (sel_itm) {
          auto sel_as_wsl = dynamic_cast<ws_atoms_list_t*>(sel_itm);
          if (sel_as_wsl) {
              try {
                int i = std::stoi(commands_list[1].data());
                if (sel_as_wsl->select_atom(i))
                  output += fmt::format("Atom {} was selected\n", i);
                else output += fmt::format("Wrong index {}\n", i);
              }
              catch(std::exception const & e) {
                output += "Error: wrong argument\n";
                return false;
              }
            }
        }
    }

  return true;
}

bool sq_command_translate_selected::execute (std::vector<std::string_view> &commands_list,
                                             std::string &output) {

  app_state_t *astate = &(c_app::get_state());
  auto cur_ws = astate->ws_manager->get_current();
  if (cur_ws) {
      auto sel_itm = cur_ws->get_selected();
      if (sel_itm) {
          auto sel_as_wsl = dynamic_cast<ws_atoms_list_t*>(sel_itm);
          if (sel_as_wsl) {
              try {
                float amount = std::stof(commands_list[1].data());
                //                if (sel_as_wsl->select_atom(i))
                //                  output += fmt::format("Atom {} was selected\n", i);
                //                else output += fmt::format("Wrong index {}\n", i);
                sel_as_wsl->translate_selected(p_axis * amount);
                return true;
              }
              catch(std::exception const & e) {
                output += "Error: wrong argument\n";
                return false;
              }
            }
        }
    }
  return true;
}

sq_command_translate_selected::sq_command_translate_selected (const vector3<float> axis) {
  p_axis = axis;
}

bool sq_command_make_super_cell::execute (std::vector<std::string_view> &commands_list,
                                          std::string &output) {
  app_state_t *astate = &(c_app::get_state());
  auto cur_ws = astate->ws_manager->get_current();

  if (cur_ws) {
      auto sel_itm = cur_ws->get_selected();
      auto sel_as_wsl = dynamic_cast<ws_atoms_list_t*>(sel_itm);
      if (sel_as_wsl && commands_list.size() >= 3) {
          //std::cout << "COMMAND LIST SIZE " << commands_list.size() << std::endl;
          int a = std::stoi(commands_list[1].data());
          int b = std::stoi(commands_list[2].data());
          int c = std::stoi(commands_list[3].data());
          sel_as_wsl->make_super_cell(a,b,c);
          astate->make_viewport_dirty();
          return true;
        }
    }

  return false;

}
