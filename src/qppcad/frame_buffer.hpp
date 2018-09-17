#ifndef QPPCAD_FRAMEBUFFER
#define QPPCAD_FRAMEBUFFER
#include <qppcad/qppcad.hpp>
#include <qppcad/frame_buffer_opengl.hpp>

namespace qpp {

  template<typename PROVIDER = frame_buffer_opengl_provider>
  class frame_buffer_t {
    public:

      static constexpr uint16_t default_width{800};
      static constexpr uint16_t default_height{800};

      PROVIDER m_provider;

      void bind(){
        m_provider.bind_fbo();
      }

      void unbind(){
        m_provider.unbid_fbo();
      }

      void resize(const uint16_t new_width, const uint16_t new_height){
        m_provider.resize_fbo(new_width, new_height);
      }

      frame_buffer_t(){
        m_provider.gen_fbo(default_width, default_height);
      }

      frame_buffer_t(const uint16_t width, const uint16_t height){
        m_provider.gen_fbo(width, height);
      }

      typename PROVIDER::tex_handle get_color_texture(){
        return  m_provider.m_tex_color;
      }

      typename PROVIDER::tex_handle get_depth_texture(){
        return m_provider.m_tex_deptj;
      }

      ~frame_buffer_t(){
        m_provider.destroy_fbo();
      }
  };

}

#endif
