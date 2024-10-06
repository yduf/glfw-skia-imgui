#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Skia support
#define SK_GANESH
#define SK_GL
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/gl/GrGLAssembleInterface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkSurface.h"

#include "include/gpu/ganesh/gl/GrGLDirectContext.h"

#include <GL/gl.h>

// Setup Dear ImGui context
void setupImGui(GLFWwindow* window) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Specify your OpenGL version here
}

#include <iostream>

void renderImGui() {
    // Tell OpenGL a new frame is about to begin
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Example ImGui window
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text("This is a simple GUI example.");
    ImGui::End();

    ImGui::Render();
}


// [Skia setup](https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add)
GrDirectContext* sContext = nullptr;
SkSurface* sSurface = nullptr;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void init_skia(int w, int h) {
	auto interface = GrGLMakeNativeInterface();
	if (interface == nullptr) {
		//backup plan. see https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add?permalink_comment_id=4680136#gistcomment-4680136
		interface = GrGLMakeAssembledInterface(
			nullptr, (GrGLGetProc) * [](void*, const char* p) -> void* { return (void*)glfwGetProcAddress(p); });
	}
	sContext = GrDirectContexts::MakeGL(interface).release();

	GrGLFramebufferInfo framebufferInfo;
	framebufferInfo.fFBOID = 0; // assume default framebuffer
	// We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
	//(replace line below with this one to enable correct color spaces) framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
	framebufferInfo.fFormat = GL_RGBA8;

	SkColorType colorType = kRGBA_8888_SkColorType;
	GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(w, h,
		0, // sample count
		0, // stencil bits
		framebufferInfo);

	//(replace line below with this one to enable correct color spaces) sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, SkColorSpace::MakeSRGB(), nullptr).release();
	sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();
	if (sSurface == nullptr) abort();
}

const int kWidth = 960;
const int kHeight = 640;

int main(int argc, char **argv) {
    // Initialize GLFW
    if (!glfwInit()) return -1;

    // Create OpenGL window
    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Skia with OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Setup ImGui
    setupImGui(window);

    // Skia Initialization
	init_skia(kWidth, kHeight);

	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);

	// Draw to the surface via its SkCanvas.
	SkCanvas* canvas = sSurface->getCanvas(); // We don't manage this pointer's lifetime.


    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        // Start rendering ImGui
        renderImGui();

        // Skia
        SkPaint paint;
		paint.setColor(SK_ColorWHITE);
		canvas->drawPaint(paint);
		paint.setColor(SK_ColorBLUE);
		canvas->drawRect({100, 200, 300, 500}, paint);
		sContext->flush();


        // Render ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
