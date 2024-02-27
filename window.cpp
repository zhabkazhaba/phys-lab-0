//
// Created by zhabkazhaba on 2/27/24.
//

#include <numeric>
#include <cstdlib>
#include <complex>
#include "window.h"

Window::Window() {
    num_of_tests = 0;
    av_diameter = 0.0f;
    av_deviation = 0.0f;
    av_square_deviation = 0.0f;
    std_conf_interval = 0.0f;
    conf_interval = 0.0f;

    num_of_tests_t2 = 0;
    av_diameter_t2 = 0.0f;
    av_height_t2 = 0.0f;
    volume = 0.0f;
    volume_err = 0.0f;
    surface = 0.0f;
    surface_err = 0.0f;

    is_modified = false;
    prob_choice = 0;
    curr_message = "Welcome!";
    curr_color = colors[INF];
    check_mode = true;
}

Window::~Window() {
    tests.clear();
}

int Window::run_window() {
    // Initialize GLFW
    if (!glfwInit())
        return 1;

    // Creating a GLFW window
    GLFWwindow* window = glfwCreateWindow(1200, 720, "PHYS-LAB-1", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // Initializing ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Initializing temporary variables
    float diameter_tmp = 0.0f;
    float height_tmp = 0.0f;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Your ImGui code here
        ImGui::SetNextWindowPos(ImVec2(50, 100));
        ImGui::SetNextWindowSize(ImVec2(400,400));
        ImGui::Begin("Input");
        if (ImGui::CollapsingHeader("Task 1")) {
            ImGui::InputFloat("Diameter", &diameter_tmp);

            if (ImGui::Button("Add test")) {
                tests.push_back(Test {diameter_tmp, 0.0f, 0.0f});
                ++num_of_tests;
                is_modified = true;
            }
            const char* probabilities[] = { "0.68", "0.95" };
            ImGui::ListBox("Conf probability", &prob_choice, probabilities, IM_ARRAYSIZE(probabilities), 3);

            if (ImGui::Button("Clear all")) {
                tests.clear();
                num_of_tests = 0;
            }
        }

        if (ImGui::CollapsingHeader("Task 2")) {
            ImGui::InputFloat("Diameter", &diameter_tmp);
            ImGui::InputFloat("Height", &height_tmp);
            if (ImGui::Button("Add test")) {
                tests_t2.push_back(Test_t2 {diameter_tmp, height_tmp});
                ++num_of_tests_t2;
                is_modified = true;
            }
        }

        ImGui::Checkbox("Check mode", &check_mode);
        ImGui::End();

        /*<----------------------------------->*/
        ImGui::SetNextWindowPos(ImVec2(750, 100));
        ImGui::SetNextWindowSize(ImVec2(400,400));
        ImGui::Begin("Display");
        if (ImGui::CollapsingHeader("Table")) {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            if (ImGui::BeginTable("table1", 4, flags)) {
                ImGui::TableSetupColumn("No.");
                ImGui::TableSetupColumn("Diameter");
                ImGui::TableSetupColumn("Deviation");
                ImGui::TableSetupColumn("Square Deviation");
                ImGui::TableHeadersRow();
                calculate_tests();
                for (int i = 0; i < tests.size(); i++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", i + 1);
                    ImGui::TableNextColumn();
                    ImGui::Text("%f", tests[i].diameter);
                    ImGui::TableNextColumn();
                    ImGui::Text("%f", tests[i].deviation);
                    ImGui::TableNextColumn();
                    ImGui::Text("%f", tests[i].square_deviation);
                }
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Task 1")) {
            if (ImGui::Button("Calculate")) {
                if (tests.empty()) {
                    send_message("No data to calculate", ERR);
                } else {
                    calculate_tests();
                    send_message("Calculation is done", SUCCESS);
                }
            }
            ImGui::Text("N: %d", num_of_tests);
            ImGui::Text("<D>: %f", av_diameter);
            ImGui::Text("<delta D>: %f", av_deviation);
            ImGui::Text("S_n: %f", av_square_deviation);
            ImGui::Text("S\'_n: %f", std_conf_interval);
            ImGui::Text("delta D_np: %f", conf_interval);
            ImGui::Text("Student coefficient: %f", REDUCED_T_MAP.at({5, available_probabilities[prob_choice]})); //TODO: fix this
            ImGui::Spacing();
            ImGui::Text("D = %f +- %f; n = %d, p = %f, E = %f %%",
                        av_diameter, conf_interval, num_of_tests, available_probabilities[prob_choice], conf_interval/av_diameter*100);
        }

        if (ImGui::CollapsingHeader("Task 2")) {
            if (ImGui::Button("Calculate")) {
                if (tests_t2.empty()) {
                    send_message("No data to calculate", ERR);
                } else {
                    calculate_tests_t2();
                    send_message("Calculation is done", SUCCESS);
                }
            }
            ImGui::Text("N: %d", num_of_tests_t2);
            ImGui::Text("<D>: %f", av_diameter_t2);
            ImGui::Text("<H>: %f", av_height_t2);
            ImGui::Text("D = %f +- %f; H = %f +- %f", av_diameter_t2, INS_ERROR, av_height_t2, INS_ERROR);
            ImGui::Text("V = %f +- %f, E = %f", volume, volume * volume_err, volume_err * 100);
            ImGui::Text("S = %f +- %f, E = %f", surface, surface * surface_err, surface_err * 100);
        }
        ImGui::End();

        /*<----------------------------------->*/
        ImGui::SetNextWindowPos(ImVec2(450, 420));
        ImGui::SetNextWindowSize(ImVec2(300,80));
        ImGui::Begin("Message");
        ImGui::TextColored(curr_color, "%s", curr_message.c_str());
        ImGui::End();

        /*<----------------------------------->*/
        ImGui::SetNextWindowPos(ImVec2(450, 100));
        ImGui::SetNextWindowSize(ImVec2(300, 320));
        ImGui::Begin("Guide");
        ImGui::Text("1. Input data in the left window");
        ImGui::Text("2. Press 'Add test' to add data");
        ImGui::Text("3. Press 'Calculate' to calculate");
        ImGui::Text("4. Check table to verify your data");
        ImGui::Text("5. Check results in the right window");
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void Window::calculate_tests() {
    if (!is_modified || num_of_tests <= 1) {
        return;
    }

    av_diameter = std::accumulate(tests.begin(), tests.end(), 0.0f, [](float sum, const Test & test) {
        return sum + test.diameter;
    }) / (float) num_of_tests;

    for (auto & test : tests) {
        test.deviation = test.diameter - av_diameter;
        test.square_deviation = test.deviation * test.deviation;
    }

    av_deviation = std::accumulate(tests.begin(), tests.end(), 0.0f, [](float sum, const Test & test) {
        return sum + std::abs(test.deviation);
    }) / (float) num_of_tests;

    av_square_deviation = std::sqrt(std::accumulate(tests.begin(), tests.end(), 0.0f, [](float sum, const Test & test) {
        return sum + test.square_deviation;
    }) / (float) (num_of_tests - 1));

    std_conf_interval = av_square_deviation / std::sqrt((float) num_of_tests);

    conf_interval = REDUCED_T_MAP.at({5, available_probabilities[prob_choice]}) * std_conf_interval; //TODO: fix this

    if (check_mode) {
        for (auto &test: tests) {
            if (test.deviation > 3 * av_square_deviation) {
                send_message("Some tests may be incorrect!", ERR);
                return;
            }
        }
    }

    is_modified = false;
}

void Window::calculate_tests_t2() {
    if (!is_modified) { //NOLINT
        return;
    }

    av_diameter_t2 = std::accumulate(tests_t2.begin(), tests_t2.end(), 0.0f, [](float sum, const Test_t2 & test) {
        return sum + test.diameter;
    }) / (float) num_of_tests_t2;

    av_height_t2 = std::accumulate(tests_t2.begin(), tests_t2.end(), 0.0f, [](float sum, const Test_t2 & test) {
        return sum + test.height;
    }) / (float) num_of_tests_t2;

    volume = (PI * av_diameter_t2 * av_diameter_t2 * av_height_t2) / 4.0f;
    volume_err = (2 * INS_ERROR) / av_diameter_t2 + INS_ERROR / av_height_t2;

    surface = PI * av_diameter_t2 * (av_diameter_t2 / 2 + av_height_t2);
    surface_err = (2 * INS_ERROR) / av_diameter_t2 + (2 * INS_ERROR) / (2 * av_height_t2 + 1);

    is_modified = false;
}

void Window::send_message(const std::string &message, INF_TYPE color) {
    curr_message = message;
    curr_color = colors[color];
}