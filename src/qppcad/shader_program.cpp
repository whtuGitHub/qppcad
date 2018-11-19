#include <qppcad/shader_program.hpp>
#include <qppcad/app_state.hpp>

qpp::cad::shader_program_t::shader_program_t (QOpenGLContext *context,
                                              const std::string &_program_name,
                                              const std::string &_vs_text,
                                              const std::string &_fs_text) {

  auto f = context->functions();
  unf_rec.resize(qpp::cad::map_u2s.size());
  std::fill(unf_rec.begin(), unf_rec.end(), uniform_record({false, 0}));

  program_name = _program_name;
  GLuint vertexShaderID = f->glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShaderID = f->glCreateShader(GL_FRAGMENT_SHADER);
  program_id = f->glCreateProgram();

  GLint proc_res = GL_FALSE;
  GLint vs_proc_res = GL_FALSE;
  GLint fs_proc_res = GL_FALSE;

  int infoLogLength = 0;

  const char *_vs_text_c = _vs_text.c_str();
  const char *_fs_text_c = _fs_text.c_str();

  f->glShaderSource(vertexShaderID, 1, &_vs_text_c, nullptr);
  f->glCompileShader(vertexShaderID);
  f->glShaderSource(fragmentShaderID, 1, &_fs_text_c, nullptr);
  f->glCompileShader(fragmentShaderID);

  f->glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &vs_proc_res);
  f->glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &fs_proc_res);

//  qpp::cad::c_app::log(fmt::format("Program[{}] vs_sh_stat = {}, fs_sh_stat = {}",
//                                   program_name, vs_proc_res, fs_proc_res));

  f->glAttachShader(program_id, vertexShaderID);
  f->glAttachShader(program_id, fragmentShaderID);
  f->glBindAttribLocation(program_id, 0, "vs_position");
  f->glBindAttribLocation(program_id, 1, "vs_normal");

  f->glLinkProgram(program_id);

  f->glGetProgramiv(program_id, GL_LINK_STATUS, &proc_res);
  f->glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLogLength);

//  qpp::cad::c_app::log("Shader program["+program_name+"] compilation status:" +
//                       std::to_string(proc_res));

//  if (infoLogLength > 0) {
//      c_app::log("Shader/Program compilation/linking failed:");
//      std::vector<char> ProgramErrorMessage(infoLogLength+1);
//      glGetProgramInfoLog(program_id, infoLogLength, nullptr, &ProgramErrorMessage[0]);
//      std::string str(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
//      c_app::log(str);
//    }

  f->glDeleteShader(vertexShaderID);
  f->glDeleteShader(fragmentShaderID);

}

void qpp::cad::shader_program_t::u_on (QOpenGLContext *context,
                                       qpp::cad::sp_u_name _val) {
  auto f = context->functions();
  unf_rec[_val].enabled = true;
  unf_rec[_val].h_prog = f->glGetUniformLocation(program_id, map_u2s[_val].c_str());

//  if (unf_rec[_val].h_prog == -1) {
//      c_app::log(fmt::format("WARNING: Uniform[{}] doesn`t exist in program {}",
//                             map_u2s[_val], program_name ));
//    }
}

void qpp::cad::shader_program_t::set_u (QOpenGLContext *context,
                                        qpp::cad::sp_u_name _ut,
                                        GLfloat *_val) {
  auto f = context->functions();
  if (unf_rec[_ut].enabled){
      qpp::cad::sp_u_type _utype = qpp::cad::map_u2at[_ut];
      GLint uloc = unf_rec[_ut].h_prog;
      switch(_utype){

        case qpp::cad::sp_u_type::a_v3f :
          f->glUniform3fv(uloc, 1, _val);
          break;

        case qpp::cad::sp_u_type::a_m4f :
          f->glUniformMatrix4fv(uloc, 1, GL_FALSE, _val);
          break;

        case qpp::cad::sp_u_type::a_m3f :
          f->glUniformMatrix3fv(uloc, 1, GL_FALSE, _val);
          break;

        case qpp::cad::sp_u_type::a_sf :
          f->glUniform1fv(uloc, 1, _val);
          break;

        default:
          break;
        }
    }

  else {

    }
}

void qpp::cad::shader_program_t::set_u_sampler(QOpenGLContext *context,
                                               qpp::cad::sp_u_name _ut,
                                               GLint val) {
  auto f = context->functions();
  if (unf_rec[_ut].enabled) {
      //qpp::cad::sp_u_type _utype = qpp::cad::map_u2at[_ut];
      GLint uloc = unf_rec[_ut].h_prog;
      f->glUniform1i(uloc, val);
    }

}

void qpp::cad::shader_program_t::begin_shader_program (QOpenGLContext *context) {
  auto f = context->functions();
  app_state_t* astate = app_state_t::get_inst();

  f->glUseProgram(program_id);

//  if (unf_rec[sp_u_name::v_light_pos].enabled)
//    set_u(sp_u_name::v_light_pos, c_app::get_state().light_pos_tr.data());

  if (unf_rec[sp_u_name::texture_0].enabled)
    f->glUniform1i(unf_rec[sp_u_name::texture_0].h_prog, 0);

//  if (unf_rec[sp_u_name::screen_width].enabled)
//    glUniform1i(unf_rec[sp_u_name::screen_width].h_prog,
//        int(astate->viewport_size_c(0)));

//  if (unf_rec[sp_u_name::screen_height].enabled)
//    glUniform1i(unf_rec[sp_u_name::screen_height].h_prog,
//        int(astate->viewport_size_c(1)));

  if (unf_rec[sp_u_name::v_eye_pos].enabled && astate->camera){
      set_u(context, sp_u_name::v_eye_pos, astate->camera->m_view_point.data());
    }
}

void qpp::cad::shader_program_t::end_shader_program (QOpenGLContext *context) {
  auto f = context->functions();
  f->glUseProgram(0);
}



