#ifndef QPP_CAD_H
#define QPP_CAD_H

#include <qppcad/qppcad.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <qppcad/app_state.hpp>
#include <qppcad/gl_math.hpp>


namespace qpp{

  namespace cad {

    class c_app{

      private:

        static app_state_t* app_state;
        static GLFWwindow* curWindow;

        static void error_callback(int error, const char* description);

        static void key_callback(GLFWwindow* window,
                                 int key,
                                 int scancode,
                                 int action,
                                 int mods);

        static void resize_window_callback(GLFWwindow* window,
                                           int _width,
                                           int _height);

        static void mouse_callback( GLFWwindow *window,
                                    double x,
                                    double y );

        static void mouse_scroll_callback(GLFWwindow* window,
                                          double xoffset,
                                          double yoffset);

        static void mouse_button_callback( GLFWwindow * window,
                                           int button,
                                           int action,
                                           int mods );

        static void begin_render();

        static void render();
        static void render_direct();
        static void render_fb();

        static void end_render();

      public:

        static bool m_is_state_initialized;
        static void update_window_title(const std::string &new_title);
        static void log(const std::string &logText);
        static void run(int argc, char **argv);
        static void copy_to_clipboard(const char * data);

        static app_state_t & get_state(){
          return *app_state;
        }

    };
  }
}
#endif
