#ifndef PHYS_LAB_0_WINDOW_H
#define PHYS_LAB_0_WINDOW_H
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <string>

class Window {
private:
    constexpr static float PI        = 3.14159265358979323846f;
    constexpr static float INS_ERROR = 0.05f;

    enum INF_TYPE {
        INF     = 0,
        SUCCESS = 1,
        ERR     = 2
    };

    const static inline ImVec4 colors[] = {
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // WHITE
            ImVec4(0.0f, 1.0f, 0.0f, 1.0f), // GREEN
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f)  // RED
    };

    const static inline float available_probabilities[] = {0.68f, 0.95f};

    const static inline std::map<std::pair<int, float>, float> REDUCED_T_MAP = {
            {{5, 0.68f}, 1.2f},
            {{5, 0.95f}, 2.8f},
            {{20, 0.68f}, 1.1f},
            {{20, 0.95f}, 2.1f}
    };

    struct Test {
        float diameter;
        float deviation;
        float square_deviation;
    };
    std::vector<Test> tests;

    float av_diameter;
    float av_deviation;
    float av_square_deviation;
    float std_conf_interval;
    float conf_interval;
    [[maybe_unused]] unsigned short num_of_tests;

    // Task 2 variables
    int num_of_tests_t2;
    struct Test_t2 {
        float diameter;
        float height;
    };
    std::vector<Test_t2> tests_t2;

    float av_diameter_t2;
    float av_height_t2;
    float volume;
    float volume_err;
    float surface;
    float surface_err;

    // Utility variables
    bool        is_modified;
    int         prob_choice;
    std::string curr_message;
    ImVec4      curr_color;
    bool        check_mode;
public:
    Window();
    Window(const Window&) = delete;
    ~Window();
    int run_window();
    /**
     * @brief Calculates values for task 1, such as deviation, sq. deviation, conf. interval and so on
     */
    void calculate_tests();
    /**
     * @brief Calculates values for task 2, such as volume and surface of a cylinder, as well as error values
     */
    void calculate_tests_t2();
    void send_message(const std::string &message, INF_TYPE color);
};


#endif //PHYS_LAB_0_WINDOW_H
